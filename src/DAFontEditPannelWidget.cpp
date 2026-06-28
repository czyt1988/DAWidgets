#include "DAFontEditPannelWidget.h"
#include "ui_DAFontEditPannelWidget.h"

#include "DAColorPickerButton.h"

#include <QSignalBlocker>
#include <QToolButton>
#include <QFontComboBox>
#include <QComboBox>

namespace DA
{
class DAFontEditPannelWidget::PrivateData
{
public:
    Ui::DAFontEditPannelWidget ui;
    int mFontPointSize { 10 };
};

DAFontEditPannelWidget::DAFontEditPannelWidget(QWidget* parent)
    : QWidget(parent), d(new PrivateData)
{
    d->ui.setupUi(this);
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

DAFontEditPannelWidget::~DAFontEditPannelWidget()
{
    delete d;
}

QColor DAFontEditPannelWidget::getCurrentFontColor() const
{
    return d->ui.colorButton->color();
}

void DAFontEditPannelWidget::setCurrentFontColor(const QColor& c)
{
    if (d->ui.colorButton->color() == c) {
        return;
    }
    d->ui.colorButton->setColor(c);
}

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

QFont DAFontEditPannelWidget::getCurrentFont()
{
    QFont f = d->ui.fontComboBox->currentFont();
    f.setBold(d->ui.toolButtonBold->isChecked());
    f.setItalic(d->ui.toolButtonItalic->isChecked());
    f.setUnderline(d->ui.toolButtonUnderline->isChecked());
    f.setPointSize(d->mFontPointSize);
    return f;
}

void DAFontEditPannelWidget::setFontWeight(int w)
{
    d->ui.comboBoxFontSize->setCurrentText(QString::number(w));
    d->mFontPointSize = w;
    signalEmitFontChanged();
}

int DAFontEditPannelWidget::getFontWeight() const
{
    return d->ui.comboBoxFontSize->currentText().toInt();
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
}
}  // namespace DA
