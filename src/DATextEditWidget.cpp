#include "DATextEditWidget.h"
#include <QAction>
#include <QFontDatabase>
#include <QPlainTextEdit>
#include <QSignalBlocker>
#include <QToolBar>
#include <QVBoxLayout>

namespace DA
{

class DATextEditWidget::PrivateData
{
    DA_DECLARE_PUBLIC(DATextEditWidget)
public:
    PrivateData(DATextEditWidget* p);
    void init();

    QToolBar* mToolbar { nullptr };
    QPlainTextEdit* mEdit { nullptr };
    QFont mDefaultFont;
    bool mMonospace { false };
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

    mEdit = new QPlainTextEdit(q);
    mEdit->setObjectName(QStringLiteral("textEdit"));
    mDefaultFont = mEdit->font();
    root->addWidget(mEdit, 1);

    QObject::connect(mEdit, &QPlainTextEdit::textChanged, q, [ q ]() { Q_EMIT q->textChanged(); });
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

}  // namespace DA
