#include "DAChip.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QMouseEvent>
#include <QToolButton>

namespace DA
{
class DAChip::PrivateData
{
public:
    PrivateData(DAChip *) {}
    QString text;
    bool closable { true };
    QLabel *label { nullptr };
    QToolButton *closeBtn { nullptr };
};

/**
 * @brief 构造带文本的标签胶囊
 * @param text 标签显示文本
 * @param parent 父控件
 */
DAChip::DAChip(const QString &text, QWidget *parent) : QFrame(parent), DA_PIMPL_CONSTRUCT
{
    DA_D(d);
    d->text = text;
    init();
}

/**
 * @brief 构造空文本的标签胶囊
 * @param parent 父控件
 */
DAChip::DAChip(QWidget *parent) : QFrame(parent), DA_PIMPL_CONSTRUCT
{
    init();
}

/**
 * @brief 析构
 * @details PIMPL 指针由 DA_PIMPL_CONSTRUCT 宏自动释放
 */
DAChip::~DAChip() = default;

/**
 * @brief 初始化标签胶囊内部结构
 * @details 创建水平布局（QLabel + QToolButton），设置对象名与 qss 选择器，
 *          连接关闭按钮 clicked 到 closeRequested 信号
 */
void DAChip::init()
{
    DA_D(d);
    setObjectName(QStringLiteral("DAChip"));
    setFrameShape(QFrame::NoFrame);

    auto *lay = new QHBoxLayout(this);
    lay->setContentsMargins(6, 0, 4, 0);
    lay->setSpacing(4);

    d->label = new QLabel(d->text, this);
    d->label->setObjectName(QStringLiteral("DAChipLabel"));
    lay->addWidget(d->label);

    d->closeBtn = new QToolButton(this);
    d->closeBtn->setObjectName(QStringLiteral("DAChipCloseButton"));
    d->closeBtn->setText(QStringLiteral("×"));  // "×"
    d->closeBtn->setAutoRaise(true);
    d->closeBtn->setFocusPolicy(Qt::NoFocus);
    d->closeBtn->setCursor(Qt::ArrowCursor);
    d->closeBtn->setFixedSize(16, 16);
    lay->addWidget(d->closeBtn);

    connect(d->closeBtn, &QToolButton::clicked, this, &DAChip::closeRequested);
    d->closeBtn->setVisible(d->closable);

    setAttribute(Qt::WA_LayoutUsesWidgetRect);
}

/**
 * @brief 获取标签显示文本
 * @return 当前文本
 */
QString DAChip::text() const
{
    DA_DC(d);
    return d->text;
}

/**
 * @brief 设置标签显示文本
 * @param text 新文本
 * @details 文本变化时更新 QLabel 并发射 textChanged
 */
void DAChip::setText(const QString &text)
{
    DA_D(d);
    if (d->text == text)
        return;
    d->text = text;
    d->label->setText(text);
    Q_EMIT textChanged(text);
}

/**
 * @brief 是否显示关闭按钮
 * @return 可关闭返回 true
 */
bool DAChip::closable() const
{
    DA_DC(d);
    return d->closable;
}

/**
 * @brief 设置是否显示关闭按钮
 * @param on 是否可关闭
 * @details 变化时切换关闭按钮可见性并发射 closableChanged
 */
void DAChip::setClosable(bool on)
{
    DA_D(d);
    if (d->closable == on)
        return;
    d->closable = on;
    d->closeBtn->setVisible(on);
    Q_EMIT closableChanged(on);
}

/**
 * @brief 鼠标按下事件
 * @param e 鼠标事件
 * @details 左键点击时发射 clicked 信号
 */
void DAChip::mousePressEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton)
        Q_EMIT clicked();
    QFrame::mousePressEvent(e);
}
}  // namespace DA
