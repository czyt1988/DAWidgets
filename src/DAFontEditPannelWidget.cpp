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

/**
 * @brief 构造字体编辑面板
 * @param parent 父控件
 * @details 初始化字号下拉列表、颜色按钮，连接字体/粗体/斜体/下划线/字号变更信号
 */
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

/**
 * @brief 析构函数
 */
DAFontEditPannelWidget::~DAFontEditPannelWidget()
{
    delete d;
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
