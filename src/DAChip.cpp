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

DAChip::DAChip(const QString &text, QWidget *parent) : QFrame(parent), DA_PIMPL_CONSTRUCT
{
    DA_D(d);
    d->text = text;
    init();
}

DAChip::DAChip(QWidget *parent) : QFrame(parent), DA_PIMPL_CONSTRUCT
{
    init();
}

DAChip::~DAChip() = default;

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

QString DAChip::text() const
{
    DA_DC(d);
    return d->text;
}

void DAChip::setText(const QString &text)
{
    DA_D(d);
    if (d->text == text)
        return;
    d->text = text;
    d->label->setText(text);
    Q_EMIT textChanged(text);
}

bool DAChip::closable() const
{
    DA_DC(d);
    return d->closable;
}

void DAChip::setClosable(bool on)
{
    DA_D(d);
    if (d->closable == on)
        return;
    d->closable = on;
    d->closeBtn->setVisible(on);
    Q_EMIT closableChanged(on);
}

void DAChip::mousePressEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton)
        Q_EMIT clicked();
    QFrame::mousePressEvent(e);
}
}  // namespace DA
