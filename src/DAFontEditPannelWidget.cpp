#include "DAFontEditPannelWidget.h"
#include "ui_DAFontEditPannelWidget.h"

#include "DAColorPickerButton.h"

#include <QSignalBlocker>
#include <QToolButton>
#include <QFontComboBox>
#include <QComboBox>
#include <QVBoxLayout>
#include <QHBoxLayout>

namespace DA
{
class DAFontEditPannelWidget::PrivateData
{
    DA_DECLARE_PUBLIC(DAFontEditPannelWidget)
public:
    PrivateData(DAFontEditPannelWidget* p);
    void rebuildLayout();

public:
    Ui::DAFontEditPannelWidget ui;
    DAFontEditPannelWidget::LayoutMode mLayoutMode { DAFontEditPannelWidget::TwoRowsLayout };
    int mFontPointSize { 10 };
};

DAFontEditPannelWidget::PrivateData::PrivateData(DAFontEditPannelWidget* p) : q_ptr(p)
{
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
        vlay->addWidget(ui.fontComboBox);
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
