#include "DATextEditWidget.h"
#include "DAQt5Qt6Compat.h"

#include <QAction>
#include <QCheckBox>
#include <QFontDatabase>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPainter>
#include <QPlainTextEdit>
#include <QShortcut>
#include <QSignalBlocker>
#include <QStyle>
#include <QTextBlock>
#include <QTextDocument>
#include <QTextEdit>
#include <QToolBar>
#include <QToolButton>
#include <QVBoxLayout>

namespace DA
{
namespace
{
class DAInternalTextEdit;

// 行号区：只负责把绘制转回编辑器
class DAInternalLineNumberArea : public QWidget
{
public:
    explicit DAInternalLineNumberArea(DAInternalTextEdit* editor);
    QSize sizeHint() const override;
    void paintEvent(QPaintEvent* e) override;

private:
    DAInternalTextEdit* mEditor { nullptr };
};

// 编辑器子类：用 viewportMargins 在左侧挤出宽度并自绘行号
class DAInternalTextEdit : public QPlainTextEdit
{
public:
    explicit DAInternalTextEdit(QWidget* parent = nullptr);

    int lineNumberAreaWidth() const;
    void setLineNumberAreaVisible(bool on);
    bool lineNumberAreaVisible() const
    {
        return mLineNumbers;
    }
    void paintLineNumbers(QPaintEvent* e);
    void updateLineNumberAreaWidth(int newBlockCount);
    void updateLineNumberArea(const QRect& rect, int dy);

protected:
    void resizeEvent(QResizeEvent* e) override;

private:
    QWidget* mLineNumberArea { nullptr };
    bool mLineNumbers { false };
};

DAInternalLineNumberArea::DAInternalLineNumberArea(DAInternalTextEdit* editor)
    : QWidget(editor), mEditor(editor)
{
    setObjectName(QStringLiteral("lineNumberArea"));
}

QSize DAInternalLineNumberArea::sizeHint() const
{
    return QSize(mEditor ? mEditor->lineNumberAreaWidth() : 0, 0);
}

void DAInternalLineNumberArea::paintEvent(QPaintEvent* e)
{
    if (mEditor)
        mEditor->paintLineNumbers(e);
}

DAInternalTextEdit::DAInternalTextEdit(QWidget* parent) : QPlainTextEdit(parent)
{
    mLineNumberArea = new DAInternalLineNumberArea(this);
    mLineNumberArea->hide();
    connect(this, &QPlainTextEdit::blockCountChanged, this, &DAInternalTextEdit::updateLineNumberAreaWidth);
    connect(this, &QPlainTextEdit::updateRequest, this, &DAInternalTextEdit::updateLineNumberArea);
    updateLineNumberAreaWidth(0);
}

int DAInternalTextEdit::lineNumberAreaWidth() const
{
    int digits = 1;
    int max    = qMax(1, blockCount());
    while (max >= 10) {
        max /= 10;
        ++digits;
    }
    return 8 + Qt5Qt6Compat_fontMetrics_width(fontMetrics(), QString(digits, QLatin1Char('9')));
}

void DAInternalTextEdit::setLineNumberAreaVisible(bool on)
{
    if (mLineNumbers == on)
        return;
    mLineNumbers = on;
    if (on) {
        mLineNumberArea->show();
        updateLineNumberAreaWidth(0);
    } else {
        mLineNumberArea->hide();
        setViewportMargins(0, 0, 0, 0);
    }
}

void DAInternalTextEdit::updateLineNumberAreaWidth(int)
{
    const int width = mLineNumbers ? lineNumberAreaWidth() : 0;
    setViewportMargins(width, 0, 0, 0);
    if (mLineNumbers && mLineNumberArea) {
        const QRect cr = contentsRect();
        mLineNumberArea->setGeometry(QRect(cr.left(), cr.top(), width, cr.height()));
    }
}

void DAInternalTextEdit::updateLineNumberArea(const QRect& rect, int dy)
{
    if (!mLineNumbers || !mLineNumberArea)
        return;
    if (dy)
        mLineNumberArea->scroll(0, dy);
    else
        mLineNumberArea->update(0, rect.y(), mLineNumberArea->width(), rect.height());
    if (rect.contains(viewport()->rect()))
        updateLineNumberAreaWidth(0);
}

void DAInternalTextEdit::resizeEvent(QResizeEvent* e)
{
    QPlainTextEdit::resizeEvent(e);
    if (!mLineNumbers || !mLineNumberArea)
        return;
    const QRect cr = contentsRect();
    mLineNumberArea->setGeometry(QRect(cr.left(), cr.top(), lineNumberAreaWidth(), cr.height()));
}

void DAInternalTextEdit::paintLineNumbers(QPaintEvent* e)
{
    QPainter painter(mLineNumberArea);
    painter.fillRect(e->rect(), QColor(0xF5, 0xF7, 0xFA));
    painter.setPen(QColor(0x90, 0x93, 0x99));

    QTextBlock block = firstVisibleBlock();
    int blockNumber  = block.blockNumber();
    int top          = qRound(blockBoundingGeometry(block).translated(contentOffset()).top());
    int bottom       = top + qRound(blockBoundingRect(block).height());
    while (block.isValid() && top <= e->rect().bottom()) {
        if (block.isVisible() && bottom >= e->rect().top()) {
            painter.drawText(0, top, mLineNumberArea->width() - 4, fontMetrics().height(),
                             Qt::AlignRight, QString::number(blockNumber + 1));
        }
        block = block.next();
        top    = bottom;
        bottom = top + qRound(blockBoundingRect(block).height());
        ++blockNumber;
    }
}
}  // namespace

class DATextEditWidget::PrivateData
{
    DA_DECLARE_PUBLIC(DATextEditWidget)
public:
    PrivateData(DATextEditWidget* p);
    void init();
    void buildFindBar();
    int collectMatches(const QString& key, QVector< QPair< int, int > >& ranges) const;
    void applyFindHighlight();
    void updateFindCount();
    void clearFindState();
    void refreshFind(bool jumpToFirst);
    void moveToMatch(bool forward);

    QToolBar* mToolbar { nullptr };
    DAInternalTextEdit* mEdit { nullptr };
    QFont mDefaultFont;
    bool mMonospace { false };

    QWidget* mFindBar { nullptr };
    QLineEdit* mFindEdit { nullptr };
    QLabel* mFindCount { nullptr };
    QCheckBox* mFindCase { nullptr };
    QToolButton* mFindPrev { nullptr };
    QToolButton* mFindNext { nullptr };
    QToolButton* mFindClose { nullptr };
    QAction* mFindAction { nullptr };
    bool mFindEnabled { false };
    bool mFindCaseSensitive { false };
    QVector< QPair< int, int > > mFindRanges;
    int mFindIndex { -1 };
};

DATextEditWidget::PrivateData::PrivateData(DATextEditWidget* p) : q_ptr(p)
{
}

void DATextEditWidget::PrivateData::init()
{
    DA_Q(DATextEditWidget, q);
    QVBoxLayout* root = new QVBoxLayout(q);
    root->setContentsMargins(0, 0, 0, 0);
    root->setSpacing(0);

    mToolbar = new QToolBar(q);
    mToolbar->setObjectName(QStringLiteral("textToolbar"));
    mToolbar->setIconSize(QSize(16, 16));
    // 注入的动作可能没有图标，用 TextBesideIcon 保证文字可见
    mToolbar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    mToolbar->setVisible(false);
    root->addWidget(mToolbar);

    buildFindBar();
    mFindBar->setVisible(false);
    root->addWidget(mFindBar);

    mEdit = new DAInternalTextEdit(q);
    mEdit->setObjectName(QStringLiteral("textEdit"));
    mDefaultFont = mEdit->font();
    root->addWidget(mEdit, 1);

    // 查找动作：context 限定在控件子树，只读时同样可用
    mFindAction = new QAction(q);
    mFindAction->setShortcut(QKeySequence::Find);
    mFindAction->setShortcutContext(Qt::WidgetWithChildrenShortcut);
    mFindAction->setEnabled(false);
    QObject::connect(mFindAction, &QAction::triggered, q, [ q ]() { q->showFindBar(); });
    q->addAction(mFindAction);

    QObject::connect(mEdit, &QPlainTextEdit::textChanged, q, [ q ]() { Q_EMIT q->textChanged(); });
}

void DATextEditWidget::PrivateData::buildFindBar()
{
    DA_Q(DATextEditWidget, q);
    mFindBar = new QWidget(q);
    mFindBar->setObjectName(QStringLiteral("findBar"));
    auto* lay = new QHBoxLayout(mFindBar);
    lay->setContentsMargins(4, 2, 4, 2);
    lay->setSpacing(4);

    mFindEdit = new QLineEdit(mFindBar);
    mFindEdit->setObjectName(QStringLiteral("findEdit"));
    mFindEdit->setPlaceholderText(DATextEditWidget::tr("Find in text"));  //cn:在文本中查找

    mFindPrev = new QToolButton(mFindBar);
    mFindPrev->setObjectName(QStringLiteral("findPrev"));
    mFindPrev->setAutoRaise(true);
    mFindPrev->setIcon(q->style()->standardIcon(QStyle::SP_ArrowUp));
    mFindPrev->setToolTip(DATextEditWidget::tr("Previous"));  //cn:上一个

    mFindNext = new QToolButton(mFindBar);
    mFindNext->setObjectName(QStringLiteral("findNext"));
    mFindNext->setAutoRaise(true);
    mFindNext->setIcon(q->style()->standardIcon(QStyle::SP_ArrowDown));
    mFindNext->setToolTip(DATextEditWidget::tr("Next"));  //cn:下一个

    mFindCount = new QLabel(QStringLiteral("0 / 0"), mFindBar);
    mFindCount->setObjectName(QStringLiteral("findCount"));

    mFindCase = new QCheckBox(DATextEditWidget::tr("Match case"), mFindBar);  //cn:区分大小写
    mFindCase->setObjectName(QStringLiteral("findCase"));

    mFindClose = new QToolButton(mFindBar);
    mFindClose->setObjectName(QStringLiteral("findClose"));
    mFindClose->setAutoRaise(true);
    mFindClose->setIcon(q->style()->standardIcon(QStyle::SP_TitleBarCloseButton));
    mFindClose->setToolTip(DATextEditWidget::tr("Close"));  //cn:关闭

    lay->addWidget(mFindEdit, 1);
    lay->addWidget(mFindPrev);
    lay->addWidget(mFindNext);
    lay->addWidget(mFindCount);
    lay->addWidget(mFindCase);
    lay->addWidget(mFindClose);

    QObject::connect(mFindEdit, &QLineEdit::textChanged, q, [ this ](const QString&) { refreshFind(true); });
    QObject::connect(mFindPrev, &QToolButton::clicked, q, [ this ]() { moveToMatch(false); });
    QObject::connect(mFindNext, &QToolButton::clicked, q, [ this ]() { moveToMatch(true); });
    QObject::connect(mFindClose, &QToolButton::clicked, q, [ q ]() { q->hideFindBar(); });
    QObject::connect(mFindCase, &QCheckBox::toggled, q, [ q ](bool on) { q->setFindCaseSensitive(on); });

    auto* esc = new QShortcut(QKeySequence(Qt::Key_Escape), mFindEdit);
    QObject::connect(esc, &QShortcut::activated, q, [ q ]() { q->hideFindBar(); });
}

int DATextEditWidget::PrivateData::collectMatches(const QString& key,
                                                  QVector< QPair< int, int > >& ranges) const
{
    ranges.clear();
    if (key.isEmpty() || !mEdit)
        return 0;
    QTextDocument::FindFlags flags;
    if (mFindCaseSensitive)
        flags |= QTextDocument::FindCaseSensitively;
    QTextCursor cursor(mEdit->document());
    while (true) {
        cursor = mEdit->document()->find(key, cursor, flags);
        if (cursor.isNull())
            break;
        const int start = cursor.selectionStart();
        ranges.append(qMakePair(start, cursor.selectionEnd() - start));
    }
    return ranges.size();
}

void DATextEditWidget::PrivateData::applyFindHighlight()
{
    QList< QTextEdit::ExtraSelection > selections;
    for (int i = 0; i < mFindRanges.size(); ++i) {
        const auto& r = mFindRanges.at(i);
        QTextEdit::ExtraSelection sel;
        sel.cursor = QTextCursor(mEdit->document());
        sel.cursor.setPosition(r.first);
        sel.cursor.setPosition(r.first + r.second, QTextCursor::KeepAnchor);
        sel.format.setBackground(i == mFindIndex ? QColor(0xFF, 0xD6, 0x66, 180)
                                                 : QColor(0xFF, 0xF1, 0xB8, 140));
        selections.append(sel);
    }
    mEdit->setExtraSelections(selections);
}

void DATextEditWidget::PrivateData::updateFindCount()
{
    if (!mFindCount)
        return;
    const int total = mFindRanges.size();
    const int shown = (total > 0 && mFindIndex >= 0) ? (mFindIndex + 1) : 0;
    mFindCount->setText(QStringLiteral("%1 / %2").arg(shown).arg(total));
    const bool invalid = total == 0 && mFindEdit && !mFindEdit->text().isEmpty();
    mFindEdit->setProperty("invalid", invalid);
    mFindEdit->style()->unpolish(mFindEdit);
    mFindEdit->style()->polish(mFindEdit);
    mFindEdit->update();
}

void DATextEditWidget::PrivateData::clearFindState()
{
    mFindRanges.clear();
    mFindIndex = -1;
    if (mEdit)
        mEdit->setExtraSelections({});
    if (mFindCount)
        mFindCount->setText(QStringLiteral("0 / 0"));
}

void DATextEditWidget::PrivateData::refreshFind(bool jumpToFirst)
{
    if (!mFindEdit)
        return;
    const QString key = mFindEdit->text();
    collectMatches(key, mFindRanges);
    if (mFindRanges.isEmpty()) {
        mFindIndex = -1;
        mEdit->setExtraSelections({});
        updateFindCount();
        return;
    }
    mFindIndex = jumpToFirst ? 0 : qBound(0, mFindIndex, mFindRanges.size() - 1);
    const auto& r = mFindRanges.at(mFindIndex);
    QTextCursor c(mEdit->document());
    c.setPosition(r.first);
    c.setPosition(r.first + r.second, QTextCursor::KeepAnchor);
    mEdit->setTextCursor(c);
    mEdit->centerCursor();
    applyFindHighlight();
    updateFindCount();
}

void DATextEditWidget::PrivateData::moveToMatch(bool forward)
{
    if (mFindRanges.isEmpty()) {
        refreshFind(true);
        return;
    }
    const int total = mFindRanges.size();
    mFindIndex      = forward ? (mFindIndex + 1) % total : (mFindIndex - 1 + total) % total;
    const auto& r   = mFindRanges.at(mFindIndex);
    QTextCursor c(mEdit->document());
    c.setPosition(r.first);
    c.setPosition(r.first + r.second, QTextCursor::KeepAnchor);
    mEdit->setTextCursor(c);
    mEdit->centerCursor();
    applyFindHighlight();
    updateFindCount();
}

//===============================================================
// DATextEditWidget
//===============================================================

/**
 * @brief 构造文本编辑控件
 * @param parent 父控件
 */
DATextEditWidget::DATextEditWidget(QWidget* parent) : QWidget(parent), DA_PIMPL_CONSTRUCT
{
    DA_D(d);
    d->init();
}

/**
 * @brief 析构
 */
DATextEditWidget::~DATextEditWidget()
{
}

/**
 * @brief 设置只读
 * @param on true = 只读
 */
void DATextEditWidget::setReadOnly(bool on)
{
    DA_D(d);
    d->mEdit->setReadOnly(on);
}

/**
 * @brief 是否只读
 * @return true = 只读
 */
bool DATextEditWidget::isReadOnly() const
{
    DA_DC(d);
    return d->mEdit->isReadOnly();
}

/**
 * @brief 设置等宽字体
 * @param on true = 使用系统等宽字体
 */
void DATextEditWidget::setMonospace(bool on)
{
    DA_D(d);
    if (on == d->mMonospace) {
        return;
    }
    d->mMonospace = on;
    if (on) {
        QFont fixed = QFontDatabase::systemFont(QFontDatabase::FixedFont);
        fixed.setPointSize(d->mDefaultFont.pointSize());
        d->mEdit->setFont(fixed);
    } else {
        d->mEdit->setFont(d->mDefaultFont);
    }
    d->mEdit->updateLineNumberAreaWidth(0);
}

/**
 * @brief 是否等宽字体
 * @return true = 等宽
 */
bool DATextEditWidget::isMonospace() const
{
    DA_DC(d);
    return d->mMonospace;
}

/**
 * @brief 设置自动换行
 * @param on true = 按控件宽度换行
 */
void DATextEditWidget::setLineWrapEnabled(bool on)
{
    DA_D(d);
    d->mEdit->setLineWrapMode(on ? QPlainTextEdit::WidgetWidth : QPlainTextEdit::NoWrap);
}

/**
 * @brief 是否自动换行
 * @return true = 自动换行
 */
bool DATextEditWidget::isLineWrapEnabled() const
{
    DA_DC(d);
    return d->mEdit->lineWrapMode() != QPlainTextEdit::NoWrap;
}

/**
 * @brief 设置文本（不发 textChanged）
 * @param text 文本
 */
void DATextEditWidget::setText(const QString& text)
{
    DA_D(d);
    QSignalBlocker blocker(d->mEdit);
    d->mEdit->setPlainText(text);
    // 信号被屏蔽，blockCountChanged 不会来，行号宽度需手动重算
    d->mEdit->updateLineNumberAreaWidth(0);
    // 旧高亮指向旧文档，必须清掉
    d->clearFindState();
    d->updateFindCount();
}

/**
 * @brief 获取文本
 * @return 纯文本
 */
QString DATextEditWidget::text() const
{
    DA_DC(d);
    return d->mEdit->toPlainText();
}

/**
 * @brief 设置占位提示
 * @param t 提示文本
 */
void DATextEditWidget::setPlaceholderText(const QString& t)
{
    DA_D(d);
    d->mEdit->setPlaceholderText(t);
}

/**
 * @brief 工具条显隐
 * @param on 是否显示
 */
void DATextEditWidget::setToolbarVisible(bool on)
{
    DA_D(d);
    d->mToolbar->setVisible(on);
}

/**
 * @brief 工具条是否显示
 * @return true = 显示
 */
bool DATextEditWidget::isToolbarVisible() const
{
    DA_DC(d);
    return d->mToolbar->isVisibleTo(const_cast< DATextEditWidget* >(this));
}

/**
 * @brief 注入工具动作
 * @param action 动作（由调用方负责生命周期）
 */
void DATextEditWidget::addToolAction(QAction* action)
{
    DA_D(d);
    if (!action) {
        return;
    }
    d->mToolbar->addAction(action);
}

/**
 * @brief 打开/关闭左侧行号栏
 * @param on 是否显示（默认 false）
 */
void DATextEditWidget::setLineNumbersVisible(bool on)
{
    DA_D(d);
    d->mEdit->setLineNumberAreaVisible(on);
}

/** @brief 行号栏是否启用 */
bool DATextEditWidget::lineNumbersVisible() const
{
    DA_DC(d);
    return d->mEdit->lineNumberAreaVisible();
}

/**
 * @brief 启用/禁用 Ctrl+F
 * @param on 是否启用（默认 false）
 */
void DATextEditWidget::setFindEnabled(bool on)
{
    DA_D(d);
    if (d->mFindEnabled == on)
        return;
    d->mFindEnabled = on;
    if (d->mFindAction)
        d->mFindAction->setEnabled(on);
    if (!on)
        hideFindBar();
}

/** @brief Ctrl+F 是否启用 */
bool DATextEditWidget::findEnabled() const
{
    DA_DC(d);
    return d->mFindEnabled;
}

/**
 * @brief 直接显隐查找条
 * @param on 是否显示（不检查 findEnabled）
 */
void DATextEditWidget::setFindBarVisible(bool on)
{
    DA_D(d);
    if (on)
        showFindBar();
    else
        hideFindBar();
}

/** @brief 查找条是否可见 */
bool DATextEditWidget::findBarVisible() const
{
    DA_DC(d);
    return d->mFindBar && d->mFindBar->isVisibleTo(const_cast< DATextEditWidget* >(this));
}

/**
 * @brief 打开查找条并可预填关键词
 * @param initialText 初始关键词；空串沿用上次
 */
void DATextEditWidget::showFindBar(const QString& initialText)
{
    DA_D(d);
    if (!d->mFindBar)
        return;
    d->mFindBar->setVisible(true);
    if (!initialText.isEmpty())
        d->mFindEdit->setText(initialText);
    d->mFindEdit->setFocus();
    d->mFindEdit->selectAll();
    d->refreshFind(true);
}

/**
 * @brief 隐藏查找条、清空高亮并把焦点还给编辑器
 */
void DATextEditWidget::hideFindBar()
{
    DA_D(d);
    if (!d->mFindBar)
        return;
    d->mFindBar->setVisible(false);
    d->clearFindState();
    d->mEdit->setFocus();
}

/**
 * @brief 设置查找是否区分大小写
 * @param on 是否区分（默认 false）
 */
void DATextEditWidget::setFindCaseSensitive(bool on)
{
    DA_D(d);
    if (d->mFindCaseSensitive == on)
        return;
    d->mFindCaseSensitive = on;
    if (d->mFindCase && d->mFindCase->isChecked() != on) {
        QSignalBlocker blocker(d->mFindCase);
        d->mFindCase->setChecked(on);
    }
    if (d->mFindBar && d->mFindBar->isVisible())
        d->refreshFind(true);
}

/** @brief 查找是否区分大小写 */
bool DATextEditWidget::findCaseSensitive() const
{
    DA_DC(d);
    return d->mFindCaseSensitive;
}

}  // namespace DA
