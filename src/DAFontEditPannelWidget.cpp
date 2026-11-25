#include "DAFontEditPannelWidget.h"
#include "ui_DAFontEditPannelWidget.h"

#include "DAColorPickerButton.h"
#include "DAQt5Qt6Compat.h"

#include <QAbstractItemView>
#include <QAction>
#include <QFontComboBox>
#include <QComboBox>
#include <QFontInfo>
#include <QFontMetrics>
#include <QHBoxLayout>
#include <QMenu>
#include <QPainter>
#include <QPixmap>
#include <QSignalBlocker>
#include <QStyledItemDelegate>
#include <QStyleOptionViewItem>
#include <QToolButton>
#include <QVBoxLayout>

namespace DA
{
namespace
{
/**
 * @brief 字体下拉的预览委托：每项用「自身 family + 面板当前样式」渲染
 * @details 只改 option->font，选中高亮/省略/禁用等仍由基类处理
 */
class FontPreviewDelegate : public QStyledItemDelegate
{
public:
    explicit FontPreviewDelegate(QObject* parent = nullptr) : QStyledItemDelegate(parent)
    {
    }

    void setPreviewFont(const QFont& f)
    {
        mPreviewFont = f;
    }

    void setMaxRowHeight(int h)
    {
        mMaxRowHeight = h;
    }

protected:
    void initStyleOption(QStyleOptionViewItem* option, const QModelIndex& index) const override
    {
        QStyledItemDelegate::initStyleOption(option, index);
        const QString family = index.data(Qt::DisplayRole).toString();
        if (family.isEmpty())
            return;
        QFont f = mPreviewFont;
        f.setFamily(family);
        // 未安装的字体族退回面板字体，避免整行豆腐块
        if (QFontInfo(f).family() != family) {
            f = option->font;
        }
        option->font = f;
    }

    QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const override
    {
        QSize s = QStyledItemDelegate::sizeHint(option, index);
        if (s.height() > mMaxRowHeight)
            s.setHeight(mMaxRowHeight);
        return s;
    }

private:
    QFont mPreviewFont;
    int mMaxRowHeight { 40 };
};

// 时钟图标（不新增 qrc 资源）
QIcon makeClockIcon()
{
    QPixmap pm(16, 16);
    pm.fill(Qt::transparent);
    QPainter p(&pm);
    p.setRenderHint(QPainter::Antialiasing);
    p.setPen(QPen(QColor(0x60, 0x66, 0x6E), 1.4));
    p.drawEllipse(QRectF(1.5, 1.5, 13, 13));
    p.drawLine(QPointF(8, 4.5), QPointF(8, 8));
    p.drawLine(QPointF(8, 8), QPointF(11, 9.5));
    p.end();
    return QIcon(pm);
}
}  // namespace

class DAFontEditPannelWidget::PrivateData
{
    DA_DECLARE_PUBLIC(DAFontEditPannelWidget)
public:
    PrivateData(DAFontEditPannelWidget* p);
    void rebuildLayout();
    void syncPreviewFont();
    void initRecentUi();
    void syncRecentMenu();
    void applyRecentFamily(const QString& family);

public:
    Ui::DAFontEditPannelWidget ui;
    DAFontEditPannelWidget::LayoutMode mLayoutMode { DAFontEditPannelWidget::TwoRowsLayout };
    int mFontPointSize { 10 };
    bool mFontPreviewEnabled { false };
    FontPreviewDelegate* mPreviewDelegate { nullptr };
    QAbstractItemDelegate* mQtFontDelegate { nullptr };
    int mMaxRecentFontCount { 5 };
    QStringList mRecentFontFamilies;
    QToolButton* mRecentButton { nullptr };
    QMenu* mRecentMenu { nullptr };
};

DAFontEditPannelWidget::PrivateData::PrivateData(DAFontEditPannelWidget* p) : q_ptr(p)
{
}

void DAFontEditPannelWidget::PrivateData::initRecentUi()
{
    if (mRecentButton)
        return;
    mRecentButton = new QToolButton(q_ptr);
    mRecentButton->setObjectName(QStringLiteral("recentFontButton"));
    mRecentButton->setAutoRaise(true);
    mRecentButton->setMinimumSize(16, 16);
    mRecentButton->setPopupMode(QToolButton::InstantPopup);
    mRecentButton->setToolTip(DAFontEditPannelWidget::tr("Recent Fonts"));  //cn:最近字体
    mRecentButton->setIcon(makeClockIcon());
    mRecentButton->setVisible(false);
    mRecentMenu = new QMenu(mRecentButton);
    mRecentMenu->setObjectName(QStringLiteral("recentFontMenu"));
    mRecentButton->setMenu(mRecentMenu);
}

void DAFontEditPannelWidget::PrivateData::syncRecentMenu()
{
    if (!mRecentButton || !mRecentMenu)
        return;
    mRecentMenu->clear();
    for (const QString& family : mRecentFontFamilies) {
        QAction* action = mRecentMenu->addAction(family);
        action->setFont(QFont(family));
        action->setData(family);
        QObject::connect(action, &QAction::triggered, q_ptr,
                         [ this, family ]() { applyRecentFamily(family); });
    }
    mRecentButton->setVisible(!mRecentFontFamilies.isEmpty());
    // 两排布局只有在列表非空时才把下拉和按钮包进同一行
    rebuildLayout();
}

void DAFontEditPannelWidget::PrivateData::applyRecentFamily(const QString& family)
{
    // 只改字体族：字号仍由 mFontPointSize 决定（不能用本类 setCurrentFont）
    QFont f(family);
    f.setPointSize(mFontPointSize);
    ui.fontComboBox->setCurrentFont(f);
}

void DAFontEditPannelWidget::PrivateData::syncPreviewFont()
{
    if (!mFontPreviewEnabled || !mPreviewDelegate)
        return;
    QFont f = q_ptr->getCurrentFont();
    f.setPointSize(qBound(9, f.pointSize(), 20));
    f.setFamily(QString());  // family 由委托逐项替换
    const int base = QFontMetrics(ui.fontComboBox->font()).height() + 6;
    mPreviewDelegate->setPreviewFont(f);
    mPreviewDelegate->setMaxRowHeight(base * 2);
    if (ui.fontComboBox->view())
        ui.fontComboBox->view()->viewport()->update();
}

/**
 * @brief 根据当前布局模式重建布局
 * @details 控件由父widget持有，删除布局不会删除控件，仅重新排列
 */
void DAFontEditPannelWidget::PrivateData::rebuildLayout()
{
    // 移除旧布局的所有子项后删除（QLayoutItem析构不删除widget，仅解除布局管理）
    if (QLayout* oldLay = q_ptr->layout()) {
        while (oldLay->count() > 0) {
            QLayoutItem* item = oldLay->takeAt(0);
            delete item;
        }
        delete oldLay;
    }
    if (mLayoutMode == DAFontEditPannelWidget::SingleRowLayout) {
        // 单排：字体族(拉伸) + 样式按钮 + 颜色 + 字号 + 增减按钮
        q_ptr->setMinimumSize(0, 0);
        QHBoxLayout* lay = new QHBoxLayout(q_ptr);
        lay->setContentsMargins(2, 2, 2, 2);
        lay->setSpacing(2);
        lay->addWidget(ui.fontComboBox, 1);
        if (mRecentButton)
            lay->addWidget(mRecentButton);
        lay->addWidget(ui.toolButtonBold);
        lay->addWidget(ui.toolButtonItalic);
        lay->addWidget(ui.toolButtonUnderline);
        lay->addWidget(ui.colorButton);
        lay->addWidget(ui.comboBoxFontSize);
        lay->addWidget(ui.toolButtonFontSizeAdd);
        lay->addWidget(ui.toolButtonFontSizeSub);
    } else {
        // 两排：第一排字体族，第二排样式按钮+颜色+字号+增减按钮（与.ui默认一致）
        q_ptr->setMinimumSize(0, 54);
        QVBoxLayout* vlay = new QVBoxLayout(q_ptr);
        vlay->setContentsMargins(2, 2, 2, 2);
        vlay->setSpacing(5);
        if (mRecentButton && !mRecentFontFamilies.isEmpty()) {
            auto* fontRow = new QHBoxLayout();
            fontRow->setSpacing(2);
            fontRow->addWidget(ui.fontComboBox, 1);
            fontRow->addWidget(mRecentButton);
            vlay->addLayout(fontRow);
        } else {
            vlay->addWidget(ui.fontComboBox);
        }
        QHBoxLayout* hlay = new QHBoxLayout();
        hlay->setSpacing(0);
        hlay->addWidget(ui.toolButtonBold);
        hlay->addWidget(ui.toolButtonItalic);
        hlay->addWidget(ui.toolButtonUnderline);
        hlay->addWidget(ui.colorButton);
        hlay->addWidget(ui.comboBoxFontSize);
        hlay->addWidget(ui.toolButtonFontSizeAdd);
        hlay->addWidget(ui.toolButtonFontSizeSub);
        vlay->addLayout(hlay);
    }
}

/**
 * @brief 构造字体编辑面板
 * @param parent 父控件
 * @details 初始化字号下拉列表、颜色按钮，连接字体/粗体/斜体/下划线/字号变更信号
 */
DAFontEditPannelWidget::DAFontEditPannelWidget(QWidget* parent)
    : QWidget(parent), d(new PrivateData(this))
{
    d->ui.setupUi(this);
    d->initRecentUi();
    d->rebuildLayout();
    d->ui.comboBoxFontSize->addItems(
        { "6", "8", "9", "10", "11", "12", "14", "16", "18", "20", "22", "24", "26", "28", "36", "48", "72" });
    d->ui.comboBoxFontSize->setCurrentText("10");
    d->ui.colorButton->setColor(Qt::black);

    connect(d->ui.fontComboBox, &QFontComboBox::currentFontChanged, this, &DAFontEditPannelWidget::signalEmitFontChanged);
    connect(d->ui.colorButton, &DAColorPickerButton::colorChanged, this, &DAFontEditPannelWidget::currentFontColorChanged);
    connect(d->ui.toolButtonBold, &QToolButton::clicked, this, &DAFontEditPannelWidget::signalEmitFontChanged);
    connect(d->ui.toolButtonItalic, &QToolButton::clicked, this, &DAFontEditPannelWidget::signalEmitFontChanged);
    connect(d->ui.toolButtonUnderline, &QToolButton::clicked, this, &DAFontEditPannelWidget::signalEmitFontChanged);
    connect(d->ui.comboBoxFontSize, &QComboBox::currentTextChanged, this, &DAFontEditPannelWidget::onComboBoxFontSizeTextChanged);
    connect(d->ui.toolButtonFontSizeAdd, &QToolButton::clicked, this, &DAFontEditPannelWidget::onFontSizeAdd);
    connect(d->ui.toolButtonFontSizeSub, &QToolButton::clicked, this, &DAFontEditPannelWidget::onFontSizeSub);
}

/**
 * @brief 析构函数
 */
DAFontEditPannelWidget::~DAFontEditPannelWidget()
{
    delete d;
}

/**
 * @brief 设置布局模式
 *
 * 默认TwoRowsLayout（两排布局）。SingleRowLayout将所有控件排为一行，
 * 适合嵌入工具栏等高度受限的场景。切换时重建布局，控件与状态保留。
 * @param m 布局模式
 * @see layoutMode
 */
void DAFontEditPannelWidget::setLayoutMode(LayoutMode m)
{
    if (d->mLayoutMode == m) {
        return;
    }
    d->mLayoutMode = m;
    d->rebuildLayout();
    updateGeometry();
}

/**
 * @brief 获取布局模式
 * @return 当前布局模式
 * @see setLayoutMode
 */
DAFontEditPannelWidget::LayoutMode DAFontEditPannelWidget::layoutMode() const
{
    return d->mLayoutMode;
}

/**
 * @brief 获取当前字体颜色
 * @return 当前字体颜色
 */
QColor DAFontEditPannelWidget::getCurrentFontColor() const
{
    return d->ui.colorButton->color();
}

/**
 * @brief 设置当前字体颜色
 * @param c 新颜色
 */
void DAFontEditPannelWidget::setCurrentFontColor(const QColor& c)
{
    if (d->ui.colorButton->color() == c) {
        return;
    }
    d->ui.colorButton->setColor(c);
}

/**
 * @brief 设置当前字体
 * @param f 新字体
 * @details 同步更新字体下拉框、字号、粗体/斜体/下划线状态
 */
void DAFontEditPannelWidget::setCurrentFont(const QFont& f)
{
    if (f == getCurrentFont()) {
        return;
    }
    QSignalBlocker b1(d->ui.fontComboBox), b2(d->ui.comboBoxFontSize);
    d->ui.toolButtonBold->setChecked(f.bold());
    d->ui.toolButtonItalic->setChecked(f.italic());
    d->ui.toolButtonUnderline->setChecked(f.underline());
    d->ui.fontComboBox->setCurrentFont(f);
    d->ui.comboBoxFontSize->setCurrentText(QString::number((f.pointSize() < 0) ? f.pixelSize() : f.pointSize()));
    signalEmitFontChanged();
}

/**
 * @brief 获取当前字体
 * @return 由字体下拉框、粗体/斜体/下划线按钮、字号组合而成的字体
 */
QFont DAFontEditPannelWidget::getCurrentFont()
{
    QFont f = d->ui.fontComboBox->currentFont();
    f.setBold(d->ui.toolButtonBold->isChecked());
    f.setItalic(d->ui.toolButtonItalic->isChecked());
    f.setUnderline(d->ui.toolButtonUnderline->isChecked());
    f.setPointSize(d->mFontPointSize);
    return f;
}

/**
 * @brief 设置字体粗细（字号）
 * @param w 字号值
 * @details Qt5中字重为0-99整数值，Qt6中为QFont::Weight枚举，此处用int兼容
 */
void DAFontEditPannelWidget::setFontWeight(int w)
{
    d->ui.comboBoxFontSize->setCurrentText(QString::number(w));
    d->mFontPointSize = w;
    signalEmitFontChanged();
}

/**
 * @brief 获取字体粗细（字号）
 * @return 当前字号
 */
int DAFontEditPannelWidget::getFontWeight() const
{
    return d->ui.comboBoxFontSize->currentText().toInt();
}

/**
 * @brief 打开/关闭字体下拉的每项预览
 * @param on 是否启用（默认 false）
 * @details 关闭时把 Qt 自己的委托原样还回去，像素与原生下拉一致
 */
void DAFontEditPannelWidget::setFontPreviewEnabled(bool on)
{
    if (d->mFontPreviewEnabled == on)
        return;
    QAbstractItemView* view = d->ui.fontComboBox->view();
    if (!view)
        return;
    d->mFontPreviewEnabled = on;
    if (on) {
        if (!d->mQtFontDelegate)
            d->mQtFontDelegate = view->itemDelegate();
        if (!d->mPreviewDelegate)
            d->mPreviewDelegate = new FontPreviewDelegate(view);
        // 直接设到 view：Qt5 的 QComboBox::setItemDelegate 会 delete 旧委托，
        // 那样存下来的原生委托就成了悬空指针，关闭时无法恢复
        view->setItemDelegate(d->mPreviewDelegate);
        d->syncPreviewFont();
    } else {
        if (d->mQtFontDelegate)
            view->setItemDelegate(d->mQtFontDelegate);
        if (d->mPreviewDelegate) {
            d->mPreviewDelegate->deleteLater();
            d->mPreviewDelegate = nullptr;
        }
    }
}

/** @brief 字体下拉每项预览是否启用 */
bool DAFontEditPannelWidget::isFontPreviewEnabled() const
{
    return d->mFontPreviewEnabled;
}

/**
 * @brief 设置最近字体条数上限
 * @param n 1–20，越界会限幅；变小时立即截断当前列表
 */
void DAFontEditPannelWidget::setMaxRecentFontCount(int n)
{
    const int clamped = qBound(1, n, 20);
    if (d->mMaxRecentFontCount == clamped)
        return;
    d->mMaxRecentFontCount = clamped;
    if (d->mRecentFontFamilies.size() > clamped) {
        d->mRecentFontFamilies = d->mRecentFontFamilies.mid(0, clamped);
    }
    d->syncRecentMenu();
}

/** @brief 最近字体条数上限 */
int DAFontEditPannelWidget::maxRecentFontCount() const
{
    return d->mMaxRecentFontCount;
}

/**
 * @brief 注入最近字体列表
 * @param families 最近 → 最旧；空串丢弃、保序去重、按上限截断
 */
void DAFontEditPannelWidget::setRecentFontFamilies(const QStringList& families)
{
    d->mRecentFontFamilies.clear();
    for (const QString& raw : families) {
        const QString family = raw.trimmed();
        if (family.isEmpty() || d->mRecentFontFamilies.contains(family))
            continue;
        d->mRecentFontFamilies << family;
        if (d->mRecentFontFamilies.size() >= d->mMaxRecentFontCount)
            break;
    }
    d->syncRecentMenu();
}

/** @brief 当前最近字体列表（最近优先） */
QStringList DAFontEditPannelWidget::recentFontFamilies() const
{
    return d->mRecentFontFamilies;
}

void DAFontEditPannelWidget::onComboBoxFontSizeTextChanged(const QString& t)
{
    bool isok;
    int v = t.toInt(&isok);
    if (!isok) {
        QSignalBlocker bl(d->ui.comboBoxFontSize);
        d->ui.comboBoxFontSize->setCurrentText(QString::number(d->mFontPointSize));
        return;
    }
    d->mFontPointSize = v;
    signalEmitFontChanged();
}

void DAFontEditPannelWidget::onFontSizeAdd()
{
    d->mFontPointSize += 1;
    QSignalBlocker bl(d->ui.comboBoxFontSize);
    d->ui.comboBoxFontSize->setCurrentText(QString::number(d->mFontPointSize));
    signalEmitFontChanged();
}

void DAFontEditPannelWidget::onFontSizeSub()
{
    d->mFontPointSize -= 1;
    if (d->mFontPointSize <= 0) {
        d->mFontPointSize = 1;
    }
    QSignalBlocker bl(d->ui.comboBoxFontSize);
    d->ui.comboBoxFontSize->setCurrentText(QString::number(d->mFontPointSize));
    signalEmitFontChanged();
}

void DAFontEditPannelWidget::signalEmitFontChanged()
{
    QFont f = getCurrentFont();
    Q_EMIT currentFontChanged(f);
    if (d->mFontPreviewEnabled)
        d->syncPreviewFont();
}
}  // namespace DA
