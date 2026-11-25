#include "DAColorMenu.h"

#include "DAColorPaletteGridWidget.h"
#include "DAColorGridWidget.h"
#include "DAQt5Qt6Compat.h"

#include <QAction>
#include <QLabel>
#include <QWidgetAction>
#include <QColorDialog>
#include <QHBoxLayout>
#include <QPainter>
#include <QPixmap>
#include <QPointer>
#include <QSignalBlocker>
#include <QSlider>
#include <utility>

namespace DA
{
class DAColorMenu::PrivateData
{
    DA_DECLARE_PUBLIC(DAColorMenu)
public:
    PrivateData(DAColorMenu* p);
    QLabel* mTitleLabel { nullptr };
    QWidgetAction* mTitleLabelAction { nullptr };
    DAColorPaletteGridWidget* mThemeColorsWidget { nullptr };
    QWidgetAction* mThemeColorPaletteAction { nullptr };
    QAction* mCustomColorAction { nullptr };
    DAColorGridWidget* mCustomColorsWidget { nullptr };
    QWidgetAction* mCustomColorsWidgetAction { nullptr };
    QAction* mNoneColorAction { nullptr };
    QList<QColor> mCustomColors;
    int mMaxCustomColorSize { 10 };
    QPointer<QColorDialog> mColorDlg;

    // alpha
    bool mAlphaEnabled { false };
    int mAlpha { 255 };
    QWidget* mAlphaRow { nullptr };
    QSlider* mAlphaSlider { nullptr };
    QLabel* mAlphaValueLabel { nullptr };
    QLabel* mAlphaPreview { nullptr };
    QWidgetAction* mAlphaAction { nullptr };
    QColor mLastColor;

    // 最近色 / 恢复默认
    bool mRecentColorsEnabled { false };
    QList<QColor> mRecentColors;
    int mMaxRecentColors { 5 };
    QAction* mDefaultColorAction { nullptr };
    QColor mDefaultColor;
};

DAColorMenu::PrivateData::PrivateData(DAColorMenu* p) : q_ptr(p)
{
}

//===================================================
// DAColorMenu
//===================================================

/**
 * @brief 构造默认主题色的颜色菜单
 * @param parent 父控件
 * @details 使用DAColorPaletteGridWidget的默认Office主题色初始化
 */
DAColorMenu::DAColorMenu(QWidget* parent) : QMenu(parent), DA_PIMPL_CONSTRUCT
{
    // 默认主题色由 DAColorPaletteGridWidget 构造函数提供
    init({});
}

/**
 * @brief 构造带标题的颜色菜单
 * @param title 菜单标题
 * @param parent 父控件
 */
DAColorMenu::DAColorMenu(const QString& title, QWidget* parent) : QMenu(title, parent), DA_PIMPL_CONSTRUCT
{
    init({});
}

/**
 * @brief 析构函数
 */
DAColorMenu::~DAColorMenu()
{
}

void DAColorMenu::init(const QList<QColor>& themeCls)
{
    DA_D(d);
    // 标题
    d->mTitleLabel = new QLabel(QStringLiteral("Theme Colors"), this);
    d->mTitleLabelAction = new QWidgetAction(this);
    d->mTitleLabelAction->setDefaultWidget(d->mTitleLabel);
    addAction(d->mTitleLabelAction);

    // 主题色板
    if (themeCls.isEmpty())
        d->mThemeColorsWidget = new DAColorPaletteGridWidget(this);
    else
        d->mThemeColorsWidget = new DAColorPaletteGridWidget(themeCls, this);
    d->mThemeColorsWidget->setColorCheckable(false);
    d->mThemeColorPaletteAction = new QWidgetAction(this);
    d->mThemeColorPaletteAction->setDefaultWidget(d->mThemeColorsWidget);
    addAction(d->mThemeColorPaletteAction);

    // Custom Color action
    d->mCustomColorAction = new QAction(QStringLiteral("Custom Color"), this);
    addAction(d->mCustomColorAction);

    // 自定义颜色记录网格
    d->mCustomColorsWidget = new DAColorGridWidget(this);
    d->mCustomColorsWidget->setRowMinimumHeight(0, d->mThemeColorsWidget->colorIconSize().height());
    d->mCustomColorsWidget->setHorizontalSpacerToRight(true);
    d->mCustomColorsWidget->setColorIconSize(d->mThemeColorsWidget->colorIconSize());
    d->mCustomColorsWidget->setColumnCount(d->mMaxCustomColorSize);
    d->mCustomColorsWidget->setColorCheckable(false);
    d->mCustomColorsWidgetAction = new QWidgetAction(this);
    d->mCustomColorsWidgetAction->setDefaultWidget(d->mCustomColorsWidget);
    addAction(d->mCustomColorsWidgetAction);

    // 连线
    connect(d->mCustomColorAction, &QAction::triggered, this, &DAColorMenu::onCustomColorActionTriggered);
    connect(d->mThemeColorsWidget, &DAColorPaletteGridWidget::colorClicked, this, &DAColorMenu::emitSelectedColor);
    connect(d->mCustomColorsWidget, &DAColorGridWidget::colorClicked, this, &DAColorMenu::emitSelectedColor);
    // 最近色重排/预览更新推迟到菜单显示时，避免在按钮点击回调里重建网格
    connect(this, &QMenu::aboutToShow, this, [ this ]() {
        DA_D(d);
        refreshRecordGrid();
        updateAlphaPreview();
    });
}

/**
 * @brief 获取主题色板的 QWidgetAction
 * @return 主题色板对应的QWidgetAction
 */
QWidgetAction* DAColorMenu::themeColorsPaletteAction() const
{
    DA_DC(d);
    return d->mThemeColorPaletteAction;
}

/**
 * @brief 获取自定义颜色记录网格的 QWidgetAction
 * @return 自定义颜色网格对应的QWidgetAction
 */
QWidgetAction* DAColorMenu::getCustomColorsWidgetAction() const
{
    DA_DC(d);
    return d->mCustomColorsWidgetAction;
}

/**
 * @brief 获取"自定义颜色"Action
 * @return 自定义颜色Action，点击后弹出QColorDialog
 */
QAction* DAColorMenu::customColorAction() const
{
    DA_DC(d);
    return d->mCustomColorAction;
}

/**
 * @brief 获取主题色板网格控件
 * @return 主题色板控件指针
 */
DAColorPaletteGridWidget* DAColorMenu::colorPaletteGridWidget() const
{
    DA_DC(d);
    return d->mThemeColorsWidget;
}

/**
 * @brief 获取自定义颜色记录网格控件
 * @return 自定义颜色网格控件指针
 */
DAColorGridWidget* DAColorMenu::customColorsWidget() const
{
    DA_DC(d);
    return d->mCustomColorsWidget;
}

/**
 * @brief 启用或禁用"无颜色"Action
 * @param on true启用，false禁用
 * @details 启用时在Custom Color之前插入None Action，禁用时移除
 */
void DAColorMenu::enableNoneColorAction(bool on)
{
    DA_D(d);
    if (on) {
        if (!d->mNoneColorAction) {
            d->mNoneColorAction = new QAction(createNoneColorIcon(), QStringLiteral("None"), this);
            connect(d->mNoneColorAction, &QAction::triggered, this, &DAColorMenu::onNoneColorActionTriggered);
            // 插入到 Custom Color action 之前
            insertAction(d->mCustomColorAction, d->mNoneColorAction);
        }
    } else {
        if (d->mNoneColorAction) {
            removeAction(d->mNoneColorAction);
        }
    }
}

/**
 * @brief 获取"无颜色"Action
 * @return 无颜色Action，未启用时返回nullptr
 */
QAction* DAColorMenu::noneColorAction() const
{
    DA_DC(d);
    return d->mNoneColorAction;
}

/**
 * @brief 发射 selectedColor 信号并隐藏菜单
 * @param c 选中的颜色，无效颜色表示"无颜色"
 */
void DAColorMenu::emitSelectedColor(const QColor& c)
{
    selectColor(c, true);
}

void DAColorMenu::onCustomColorActionTriggered(bool)
{
    const QColor c = getColorByDialog();
    if (c.isValid())
        emitSelectedColor(c);
}

void DAColorMenu::onNoneColorActionTriggered(bool)
{
    emitSelectedColor(QColor());  // 无效颜色表示"无颜色"
}

QColor DAColorMenu::getColorByDialog()
{
    DA_D(d);
    if (!d->mColorDlg) {
        d->mColorDlg = new QColorDialog(this);
        d->mColorDlg->setOption(QColorDialog::ShowAlphaChannel, true);
    }
    if (d->mAlphaEnabled && d->mLastColor.isValid()) {
        d->mColorDlg->setCurrentColor(
            QColor(d->mLastColor.red(), d->mLastColor.green(), d->mLastColor.blue(), d->mAlpha));
    }
    if (d->mColorDlg->exec() == QDialog::Accepted) {
        const QColor c = d->mColorDlg->currentColor();
        // 对话框返回的 alpha 回写滑杆，保持"滑杆是唯一真源"
        if (d->mAlphaEnabled)
            setAlpha(c.alpha());
        return c;
    }
    return QColor();
}

void DAColorMenu::recordCustomColor(const QColor& c)
{
    DA_D(d);
    if (d->mCustomColors.size() < d->mMaxCustomColorSize) {
        d->mCustomColors.append(c);
    } else {
        // FIFO 淘汰最旧
        for (int i = 0; i < d->mCustomColors.size() - 1; ++i) {
            d->mCustomColors[i] = d->mCustomColors[i + 1];
        }
        d->mCustomColors.back() = c;
    }
}

QIcon DAColorMenu::createNoneColorIcon()
{
    QPixmap pixmap(32, 32);
    pixmap.fill(Qt::transparent);
    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
    QRect r = pixmap.rect().adjusted(1, 1, -1, -1);
    // 红色对角线
    QPen pen(Qt::red, 1, Qt::SolidLine, Qt::RoundCap);
    painter.setPen(pen);
    int ss = r.width() / 3;
    painter.drawLine(QPoint(r.x() + ss, r.bottom()), QPoint(r.right() - ss, r.top()));
    // 黑色边框
    pen.setColor(Qt::black);
    painter.setPen(pen);
    painter.drawRect(r);
    return QIcon(pixmap);
}

/**
 * @brief 打开/关闭透明度行
 * @param on 是否启用（默认 false）
 * @details 打开时把行追加到菜单末尾；关闭时只移除 action，行对象保留，滑杆位置不丢
 */
void DAColorMenu::setAlphaEnabled(bool on)
{
    DA_D(d);
    if (d->mAlphaEnabled == on)
        return;
    d->mAlphaEnabled = on;
    if (on) {
        if (!d->mAlphaAction)
            buildAlphaRow();
        addAction(d->mAlphaAction);
        updateAlphaPreview();
    } else if (d->mAlphaAction) {
        removeAction(d->mAlphaAction);
    }
}

/** @brief 透明度行是否启用 */
bool DAColorMenu::isAlphaEnabled() const
{
    DA_DC(d);
    return d->mAlphaEnabled;
}

/** @brief 当前 alpha（0–255） */
int DAColorMenu::alpha() const
{
    DA_DC(d);
    return d->mAlpha;
}

/**
 * @brief 设置当前 alpha
 * @param a 0–255，越界限幅；值不变时不发 alphaChanged
 */
void DAColorMenu::setAlpha(int a)
{
    DA_D(d);
    const int clamped = qBound(0, a, 255);
    if (clamped == d->mAlpha)
        return;
    d->mAlpha = clamped;
    if (d->mAlphaSlider && d->mAlphaSlider->value() != clamped) {
        QSignalBlocker blocker(d->mAlphaSlider);
        d->mAlphaSlider->setValue(clamped);
    }
    updateAlphaPreview();
    Q_EMIT alphaChanged(clamped);
}

void DAColorMenu::buildAlphaRow()
{
    DA_D(d);
    d->mAlphaRow = new QWidget(this);
    auto* lay = new QHBoxLayout(d->mAlphaRow);
    lay->setContentsMargins(6, 2, 6, 2);
    lay->setSpacing(6);

    auto* label = new QLabel(tr("Opacity"), d->mAlphaRow);  //cn:不透明度
    d->mAlphaSlider = new QSlider(Qt::Horizontal, d->mAlphaRow);
    d->mAlphaSlider->setObjectName(QStringLiteral("DAAlphaSlider"));
    d->mAlphaSlider->setRange(0, 255);
    d->mAlphaSlider->setValue(d->mAlpha);

    d->mAlphaValueLabel = new QLabel(d->mAlphaRow);
    d->mAlphaValueLabel->setObjectName(QStringLiteral("DAAlphaValueLabel"));
    // 定宽，避免 9%→100% 时行宽抖动
    const int w = Qt5Qt6Compat_fontMetrics_width(d->mAlphaValueLabel->fontMetrics(), QStringLiteral("100%")) + 6;
    d->mAlphaValueLabel->setFixedWidth(w);

    d->mAlphaPreview = new QLabel(d->mAlphaRow);
    d->mAlphaPreview->setObjectName(QStringLiteral("DAAlphaPreview"));
    d->mAlphaPreview->setFixedSize(24, 18);

    lay->addWidget(label);
    lay->addWidget(d->mAlphaSlider, 1);
    lay->addWidget(d->mAlphaValueLabel);
    lay->addWidget(d->mAlphaPreview);

    d->mAlphaAction = new QWidgetAction(this);
    d->mAlphaAction->setDefaultWidget(d->mAlphaRow);
    connect(d->mAlphaSlider, &QSlider::valueChanged, this, &DAColorMenu::setAlpha);
    updateAlphaPreview();
}

void DAColorMenu::updateAlphaPreview()
{
    DA_D(d);
    if (!d->mAlphaSlider || !d->mAlphaValueLabel || !d->mAlphaPreview)
        return;
    d->mAlphaValueLabel->setText(QStringLiteral("%1%").arg(qRound(d->mAlpha * 100.0 / 255.0)));
    QPixmap pm(24, 18);
    pm.fill(Qt::transparent);
    QPainter p(&pm);
    for (int y = 0; y < 18; y += 6) {
        for (int x = 0; x < 24; x += 6) {
            const bool odd = ((x / 6) + (y / 6)) % 2 != 0;
            p.fillRect(QRect(x, y, 6, 6), odd ? QColor(0xE6, 0xE9, 0xEE) : QColor(Qt::white));
        }
    }
    QColor c = d->mLastColor.isValid() ? d->mLastColor : QColor(64, 158, 255);
    c.setAlpha(d->mAlpha);
    p.fillRect(pm.rect(), c);
    p.end();
    d->mAlphaPreview->setPixmap(pm);
}

/**
 * @brief 启用最近色策略
 * @param colors 初始最近色（最近优先），去重后截断到 5
 */
void DAColorMenu::setRecentColors(const QList<QColor>& colors)
{
    DA_D(d);
    d->mRecentColorsEnabled = true;
    d->mRecentColors.clear();
    for (const QColor& c : colors) {
        if (!c.isValid())
            continue;
        const QRgb key = c.toRgb().rgba();
        bool seen = false;
        for (const QColor& e : std::as_const(d->mRecentColors)) {
            if (e.toRgb().rgba() == key) {
                seen = true;
                break;
            }
        }
        if (seen)
            continue;
        d->mRecentColors << c;
        if (d->mRecentColors.size() >= d->mMaxRecentColors)
            break;
    }
    d->mCustomColorsWidget->setAlphaBackgroundEnabled(true);
    const QSize icon = d->mCustomColorsWidget->colorIconSize();
    d->mCustomColorsWidget->setMinimumWidth(icon.width() * d->mMaxRecentColors
                                            + d->mCustomColorsWidget->horizontalSpacing()
                                                  * (d->mMaxRecentColors - 1));
    refreshRecordGrid();
}

/** @brief 获取最近色（最近优先）；未启用时为空 */
QList<QColor> DAColorMenu::recentColors() const
{
    DA_DC(d);
    return d->mRecentColors;
}

/** @brief 把当前生效的记录列表刷进网格 */
void DAColorMenu::refreshRecordGrid()
{
    DA_D(d);
    if (!d->mCustomColorsWidget)
        return;
    d->mCustomColorsWidget->setColorList(d->mRecentColorsEnabled ? d->mRecentColors : d->mCustomColors);
    d->mCustomColorsWidget->updateGeometry();
}

void DAColorMenu::recordRecentColor(const QColor& c)
{
    DA_D(d);
    const QRgb key = c.toRgb().rgba();
    for (int i = d->mRecentColors.size() - 1; i >= 0; --i) {
        if (d->mRecentColors.at(i).toRgb().rgba() == key)
            d->mRecentColors.removeAt(i);
    }
    d->mRecentColors.prepend(c);
    while (d->mRecentColors.size() > d->mMaxRecentColors)
        d->mRecentColors.removeLast();
}

/**
 * @brief 设置/移除「恢复默认」入口
 * @param color 有效色插入 action；无效色移除入口
 */
void DAColorMenu::setDefaultColor(const QColor& color)
{
    DA_D(d);
    d->mDefaultColor = color;
    if (color.isValid()) {
        if (!d->mDefaultColorAction) {
            d->mDefaultColorAction = new QAction(tr("Restore Default"), this);  //cn:恢复默认
            connect(d->mDefaultColorAction, &QAction::triggered, this, [ this ]() {
                DA_D(d);
                selectColor(d->mDefaultColor, false);
            });
        }
        insertAction(d->mCustomColorAction, d->mDefaultColorAction);
    } else if (d->mDefaultColorAction) {
        removeAction(d->mDefaultColorAction);
        d->mDefaultColorAction->deleteLater();
        d->mDefaultColorAction = nullptr;
    }
}

/** @brief 当前默认色（未设置时无效） */
QColor DAColorMenu::defaultColor() const
{
    DA_DC(d);
    return d->mDefaultColor;
}

/**
 * @brief 统一选色出口
 * @param c 选中的颜色
 * @param recordToRecent true = 常规选色：应用 alpha 并记录；false = 恢复默认：原样发出
 */
void DAColorMenu::selectColor(const QColor& c, bool recordToRecent)
{
    DA_D(d);
    QColor out = c;
    if (d->mAlphaEnabled && recordToRecent && out.isValid())
        out.setAlpha(d->mAlpha);
    if (out.isValid())
        d->mLastColor = out;
    if (out.isValid() && recordToRecent) {
        if (d->mRecentColorsEnabled)
            recordRecentColor(out);
        else
            recordCustomColor(out);
    }
    Q_EMIT selectedColor(out);
    hide();
}
}  // namespace DA
