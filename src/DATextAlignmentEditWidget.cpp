#include "DATextAlignmentEditWidget.h"
#include <QButtonGroup>
#include <QEvent>
#include <QHBoxLayout>
#include <QIcon>
#include <QToolButton>
#include <QVBoxLayout>

namespace DA
{

// 图标资源（本库 QRC，前缀 /DAWidgetsIcons）。
// 注意：AligmentCenter.svg 实际是「垂直居中」语义（横线在中间、文本块上下居中），
// 水平居中用本控件新增的 AligmentHCenter.svg。
namespace
{
constexpr int c_buttonSize   = 26;  ///< 单按钮边长（16px 图标 + 边距，两行合计约 53px）
constexpr int c_iconSize     = 16;  ///< 图标边长
constexpr int c_layoutSpacing = 1;  ///< 行内/行间间距
}  // namespace

/**
 * @brief 文本对齐编辑控件私有数据
 */
class DATextAlignmentEditWidget::PrivateData
{
    DA_DECLARE_PUBLIC(DATextAlignmentEditWidget)
public:
    explicit PrivateData(DATextAlignmentEditWidget* p) : q_ptr(p) {}

    // 组装一行按钮（三个）
    void setupButton(QToolButton* btn, const QString& objName, const QString& iconPath);
    // 按当前值刷新两行按钮的选中态
    void syncButtonStates();
    // 取水平/垂直位（缺位返回空）
    static Qt::Alignment horizontalOf(Qt::Alignment al);
    static Qt::Alignment verticalOf(Qt::Alignment al);
    // 用户点击某个按钮：更新对应轴，值有变化才发信号
    void onButtonClicked(Qt::Alignment axisValue, bool horizontal);
    // 切换语言时刷新提示文本
    void retranslateUi();

    // 当前值（默认与表格默认一致：左对齐 + 垂直居中）
    Qt::Alignment mAlignment { Qt::AlignLeft | Qt::AlignVCenter };

    QToolButton* mBtnLeft { nullptr };
    QToolButton* mBtnHCenter { nullptr };
    QToolButton* mBtnRight { nullptr };
    QToolButton* mBtnTop { nullptr };
    QToolButton* mBtnVCenter { nullptr };
    QToolButton* mBtnBottom { nullptr };
    QButtonGroup* mHGroup { nullptr };  ///< 水平行互斥组
    QButtonGroup* mVGroup { nullptr };  ///< 垂直行互斥组
};

/**
 * @brief 组装单个按钮（图标、可选中、自适应扁平样式）
 */
void DATextAlignmentEditWidget::PrivateData::setupButton(QToolButton* btn, const QString& objName, const QString& iconPath)
{
    btn->setObjectName(objName);
    btn->setAutoRaise(true);
    btn->setCheckable(true);
    btn->setIcon(QIcon(iconPath));
    btn->setIconSize(QSize(c_iconSize, c_iconSize));
    btn->setFixedSize(c_buttonSize, c_buttonSize);
}

/**
 * @brief 按当前值刷新两行按钮的选中态
 */
void DATextAlignmentEditWidget::PrivateData::syncButtonStates()
{
    const Qt::Alignment h = horizontalOf(mAlignment);
    const Qt::Alignment v = verticalOf(mAlignment);
    mBtnLeft->setChecked(h == Qt::AlignLeft);
    mBtnHCenter->setChecked(h == Qt::AlignHCenter);
    mBtnRight->setChecked(h == Qt::AlignRight);
    mBtnTop->setChecked(v == Qt::AlignTop);
    mBtnVCenter->setChecked(v == Qt::AlignVCenter);
    mBtnBottom->setChecked(v == Qt::AlignBottom);
}

/**
 * @brief 取对齐值中的水平位（AlignLeft/AlignHCenter/AlignRight；都没有返回空）
 */
Qt::Alignment DATextAlignmentEditWidget::PrivateData::horizontalOf(Qt::Alignment al)
{
    if (al.testFlag(Qt::AlignLeft)) {
        return Qt::AlignLeft;
    }
    if (al.testFlag(Qt::AlignRight)) {
        return Qt::AlignRight;
    }
    if (al.testFlag(Qt::AlignHCenter)) {
        return Qt::AlignHCenter;
    }
    return Qt::Alignment();
}

/**
 * @brief 取对齐值中的垂直位（AlignTop/AlignVCenter/AlignBottom；都没有返回空）
 */
Qt::Alignment DATextAlignmentEditWidget::PrivateData::verticalOf(Qt::Alignment al)
{
    if (al.testFlag(Qt::AlignTop)) {
        return Qt::AlignTop;
    }
    if (al.testFlag(Qt::AlignBottom)) {
        return Qt::AlignBottom;
    }
    if (al.testFlag(Qt::AlignVCenter)) {
        return Qt::AlignVCenter;
    }
    return Qt::Alignment();
}

/**
 * @brief 用户点击按钮：替换对应轴后同步选中态，值有变化才发 alignmentChanged
 * @param axisValue AlignLeft/AlignHCenter/AlignRight 或 AlignTop/AlignVCenter/AlignBottom
 * @param horizontal true=水平行，false=垂直行
 */
void DATextAlignmentEditWidget::PrivateData::onButtonClicked(Qt::Alignment axisValue, bool horizontal)
{
    const Qt::Alignment old = mAlignment;
    if (horizontal) {
        mAlignment = (mAlignment & ~Qt::AlignHorizontal_Mask) | axisValue;
    } else {
        mAlignment = (mAlignment & ~Qt::AlignVertical_Mask) | axisValue;
    }
    syncButtonStates();
    if (mAlignment != old) {
        Q_EMIT q_func()->alignmentChanged(mAlignment);
    }
}

/**
 * @brief 切换语言时刷新按钮提示文本
 */
void DATextAlignmentEditWidget::PrivateData::retranslateUi()
{
    mBtnLeft->setToolTip(DATextAlignmentEditWidget::tr("Align Left"));  // cn:左对齐
    mBtnHCenter->setToolTip(DATextAlignmentEditWidget::tr("Align Center"));  // cn:居中
    mBtnRight->setToolTip(DATextAlignmentEditWidget::tr("Align Right"));  // cn:右对齐
    mBtnTop->setToolTip(DATextAlignmentEditWidget::tr("Align Top"));  // cn:顶端对齐
    mBtnVCenter->setToolTip(DATextAlignmentEditWidget::tr("Align Middle"));  // cn:垂直居中
    mBtnBottom->setToolTip(DATextAlignmentEditWidget::tr("Align Bottom"));  // cn:底端对齐
}

/**
 * @brief 构造函数：两行互斥按钮组（水平 → 垂直）
 */
DATextAlignmentEditWidget::DATextAlignmentEditWidget(QWidget* parent) : QWidget(parent), DA_PIMPL_CONSTRUCT
{
    DA_D(d);
    // 水平行：左 / 居中 / 右
    QHBoxLayout* hRow = new QHBoxLayout;
    hRow->setContentsMargins(0, 0, 0, 0);
    hRow->setSpacing(c_layoutSpacing);
    d->mBtnLeft    = new QToolButton(this);
    d->mBtnHCenter = new QToolButton(this);
    d->mBtnRight   = new QToolButton(this);
    d->setupButton(d->mBtnLeft, QStringLiteral("tbAlignLeft"), QStringLiteral(":/DAWidgetsIcons/AligmentLeft.svg"));
    d->setupButton(d->mBtnHCenter, QStringLiteral("tbAlignHCenter"), QStringLiteral(":/DAWidgetsIcons/AligmentHCenter.svg"));
    d->setupButton(d->mBtnRight, QStringLiteral("tbAlignRight"), QStringLiteral(":/DAWidgetsIcons/AligmentRight.svg"));
    hRow->addWidget(d->mBtnLeft);
    hRow->addWidget(d->mBtnHCenter);
    hRow->addWidget(d->mBtnRight);
    hRow->addStretch(1);
    // 垂直行：顶端 / 居中 / 底端
    QHBoxLayout* vRow = new QHBoxLayout;
    vRow->setContentsMargins(0, 0, 0, 0);
    vRow->setSpacing(c_layoutSpacing);
    d->mBtnTop     = new QToolButton(this);
    d->mBtnVCenter = new QToolButton(this);
    d->mBtnBottom  = new QToolButton(this);
    d->setupButton(d->mBtnTop, QStringLiteral("tbAlignTop"), QStringLiteral(":/DAWidgetsIcons/AligmentTop.svg"));
    d->setupButton(d->mBtnVCenter, QStringLiteral("tbAlignVCenter"), QStringLiteral(":/DAWidgetsIcons/AligmentCenter.svg"));
    d->setupButton(d->mBtnBottom, QStringLiteral("tbAlignBottom"), QStringLiteral(":/DAWidgetsIcons/AligmentBottom.svg"));
    vRow->addWidget(d->mBtnTop);
    vRow->addWidget(d->mBtnVCenter);
    vRow->addWidget(d->mBtnBottom);
    vRow->addStretch(1);

    QVBoxLayout* root = new QVBoxLayout(this);
    root->setContentsMargins(0, 0, 0, 0);
    root->setSpacing(c_layoutSpacing);
    root->addLayout(hRow);
    root->addLayout(vRow);

    // 两行各自互斥（QButtonGroup 保证同一行只能选中一个）
    d->mHGroup = new QButtonGroup(this);
    d->mHGroup->setExclusive(true);
    d->mHGroup->addButton(d->mBtnLeft);
    d->mHGroup->addButton(d->mBtnHCenter);
    d->mHGroup->addButton(d->mBtnRight);
    d->mVGroup = new QButtonGroup(this);
    d->mVGroup->setExclusive(true);
    d->mVGroup->addButton(d->mBtnTop);
    d->mVGroup->addButton(d->mBtnVCenter);
    d->mVGroup->addButton(d->mBtnBottom);

    // 点击即应用（逐按钮连接，避免 Qt5/Qt6 按钮组信号差异）；重复点击已选中的按钮不改变值、不发信号
    QObject::connect(d->mBtnLeft, &QToolButton::clicked, this, [ d ]() { d->onButtonClicked(Qt::AlignLeft, true); });
    QObject::connect(d->mBtnHCenter, &QToolButton::clicked, this, [ d ]() { d->onButtonClicked(Qt::AlignHCenter, true); });
    QObject::connect(d->mBtnRight, &QToolButton::clicked, this, [ d ]() { d->onButtonClicked(Qt::AlignRight, true); });
    QObject::connect(d->mBtnTop, &QToolButton::clicked, this, [ d ]() { d->onButtonClicked(Qt::AlignTop, false); });
    QObject::connect(d->mBtnVCenter, &QToolButton::clicked, this, [ d ]() { d->onButtonClicked(Qt::AlignVCenter, false); });
    QObject::connect(d->mBtnBottom, &QToolButton::clicked, this, [ d ]() { d->onButtonClicked(Qt::AlignBottom, false); });

    d->retranslateUi();
    d->syncButtonStates();
}

/**
 * @brief 析构函数
 */
DATextAlignmentEditWidget::~DATextAlignmentEditWidget()
{
}

/**
 * @brief 当前对齐（水平位 + 垂直位）
 */
Qt::Alignment DATextAlignmentEditWidget::alignment() const
{
    DA_DC(d);
    return d->mAlignment;
}

/**
 * @brief 当前水平对齐位
 */
Qt::Alignment DATextAlignmentEditWidget::horizontalAlignment() const
{
    DA_DC(d);
    return PrivateData::horizontalOf(d->mAlignment);
}

/**
 * @brief 当前垂直对齐位
 */
Qt::Alignment DATextAlignmentEditWidget::verticalAlignment() const
{
    DA_DC(d);
    return PrivateData::verticalOf(d->mAlignment);
}

/**
 * @brief 设置对齐：缺水平位/垂直位的那一半保持当前值；与当前值相同不发信号
 */
void DATextAlignmentEditWidget::setAlignment(Qt::Alignment al)
{
    DA_D(d);
    Qt::Alignment h = PrivateData::horizontalOf(al);
    Qt::Alignment v = PrivateData::verticalOf(al);
    const Qt::Alignment old = d->mAlignment;
    if (h != Qt::Alignment()) {
        d->mAlignment = (d->mAlignment & ~Qt::AlignHorizontal_Mask) | h;
    }
    if (v != Qt::Alignment()) {
        d->mAlignment = (d->mAlignment & ~Qt::AlignVertical_Mask) | v;
    }
    d->syncButtonStates();
    if (d->mAlignment != old) {
        Q_EMIT alignmentChanged(d->mAlignment);
    }
}

/**
 * @brief 切换语言时刷新提示文本
 */
void DATextAlignmentEditWidget::changeEvent(QEvent* e)
{
    QWidget::changeEvent(e);
    if (e->type() == QEvent::LanguageChange) {
        DA_D(d);
        d->retranslateUi();
    }
}

}  // namespace DA
