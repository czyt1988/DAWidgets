#include "DABrushEditWidget.h"
#include "ui_DABrushEditWidget.h"

#include "DAColorPickerButton.h"
#include "DABrushStyleComboBox.h"

#include <QColorDialog>
#include <QFileDialog>
#include <QGradient>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QDialogButtonBox>
#include <QPainter>
#include <QPixmap>
#include <QSignalBlocker>
#include <QVBoxLayout>
#include <QRadioButton>
#include <QButtonGroup>
#include <QSpinBox>
#include <QTimer>
#include <QtMath>

namespace DA
{
//===================================================
// 渐变编辑对话框(内部使用)
//===================================================
class DABrushEditWidget::PrivateData
{
public:
    Ui::DABrushEditWidget ui;
    QBrush mBrush;
};

/**
 * @brief 构造画刷编辑窗口
 * @param parent 父控件
 * @details 初始化UI，颜色按钮初始为无效色，连接颜色和样式变更信号
 */
DABrushEditWidget::DABrushEditWidget(QWidget* parent) : QWidget(parent), d(new PrivateData)
{
    d->ui.setupUi(this);
    d->ui.colorButton->setColor(QColor());
    connect(d->ui.colorButton, &DAColorPickerButton::colorChanged, this, &DABrushEditWidget::onColorChanged);
    connect(d->ui.comboBox, &DABrushStyleComboBox::currentBrushStyleChanged, this, &DABrushEditWidget::onBrushStyleChanged);
}

/**
 * @brief 析构函数
 */
DABrushEditWidget::~DABrushEditWidget()
{
    delete d;
}

/**
 * @brief 获取当前画刷
 * @return 当前画刷
 */
QBrush DABrushEditWidget::getCurrentBrush() const
{
    return d->mBrush;
}

/**
 * @brief 设置是否在样式上显示文字
 * @param on 是否显示
 */
void DABrushEditWidget::setStyleTextVisible(bool on)
{
    d->ui.comboBox->setStyleTextVisible(on);
}

/**
 * @brief 返回是否在样式上显示文字
 * @return 是否显示
 */
bool DABrushEditWidget::isStyleTextVisible() const
{
    return d->ui.comboBox->isStyleTextVisible();
}

/**
 * @brief 获取当前画刷样式
 * @return 当前画刷样式
 */
Qt::BrushStyle DABrushEditWidget::getCurrentBrushStyle() const
{
    return d->ui.comboBox->getCurrentBrushStyle();
}

/**
 * @brief 获取当前画刷颜色
 * @return 当前画刷颜色（纯色/图案模式下有效）
 */
QColor DABrushEditWidget::getCurrentBrushColor() const
{
    return d->ui.colorButton->color();
}

/**
 * @brief 设置当前画刷
 * @param b 新画刷
 * @details 支持纯色/图案/渐变/纹理四种模式。渐变模式下颜色按钮显示起始色，纹理模式下显示无效色
 */
void DABrushEditWidget::setCurrentBrush(const QBrush& b)
{
    QSignalBlocker b1(d->ui.colorButton), b2(d->ui.comboBox);
    d->mBrush = b;
    if (b.style() >= Qt::LinearGradientPattern && b.style() <= Qt::ConicalGradientPattern) {
        // 渐变:颜色按钮显示起始色
        if (b.gradient() && !b.gradient()->stops().isEmpty()) {
            d->ui.colorButton->setColor(b.gradient()->stops().first().second);
        } else {
            d->ui.colorButton->setColor(b.color());
        }
    } else if (b.style() == Qt::TexturePattern) {
        // 纹理:颜色按钮显示无效色
        d->ui.colorButton->setColor(QColor());
    } else {
        d->ui.colorButton->setColor(b.color());
    }
    d->ui.comboBox->setCurrentBrushStyle(b.style());
    Q_EMIT brushChanged(b);
}

/**
 * @brief 设置当前画刷样式
 * @param s 画刷样式
 */
void DABrushEditWidget::setCurrentBrushStyle(Qt::BrushStyle s)
{
    d->ui.comboBox->setCurrentBrushStyle(s);
}

void DABrushEditWidget::onColorChanged(const QColor& c)
{
    Qt::BrushStyle s = getCurrentBrushStyle();
    if (c.isValid()) {
        if (s == Qt::NoBrush) {
            QSignalBlocker b1(d->ui.comboBox);
            d->ui.comboBox->setCurrentBrushStyle(Qt::SolidPattern);
            d->mBrush.setStyle(Qt::SolidPattern);
        }
    } else {
        if (s != Qt::NoBrush && s < Qt::LinearGradientPattern) {
            QSignalBlocker b1(d->ui.comboBox);
            d->ui.comboBox->setCurrentBrushStyle(Qt::NoBrush);
            d->mBrush.setStyle(Qt::NoBrush);
        }
    }
    d->mBrush.setColor(c);
    // 同步更新 combobox 的颜色(修复原版 onBrushStyleChanged 未回写颜色的 bug)
    d->ui.comboBox->setBrushColor(c);
    Q_EMIT brushChanged(d->mBrush);
}

void DABrushEditWidget::onBrushStyleChanged(Qt::BrushStyle s)
{
    // 渐变/纹理样式:弹出编辑器
    if (s >= Qt::LinearGradientPattern && s <= Qt::ConicalGradientPattern) {
        editGradient(s);
        return;
    }
    if (s == Qt::TexturePattern) {
        editTexture();
        return;
    }

    // 普通样式
    if (d->mBrush.style() == s) {
        return;
    }
    d->mBrush.setStyle(s);
    // 修复联动 bug:样式改变时同步颜色到 combobox
    if (d->mBrush.color().isValid()) {
        d->ui.comboBox->setBrushColor(d->mBrush.color());
    }
    Q_EMIT brushChanged(d->mBrush);
}

void DABrushEditWidget::editGradient(Qt::BrushStyle style)
{
    // 弹出渐变编辑对话框
    QDialog dlg(this);
    dlg.setWindowTitle(QStringLiteral("Gradient Editor"));

    auto* layout = new QVBoxLayout(&dlg);

    // 渐变类型选择
    auto* typeGroup = new QButtonGroup(&dlg);
    auto* typeLayout = new QHBoxLayout();
    auto* rbLinear = new QRadioButton(QStringLiteral("Linear"), &dlg);
    auto* rbRadial = new QRadioButton(QStringLiteral("Radial"), &dlg);
    auto* rbConical = new QRadioButton(QStringLiteral("Conical"), &dlg);
    typeGroup->addButton(rbLinear, Qt::LinearGradientPattern);
    typeGroup->addButton(rbRadial, Qt::RadialGradientPattern);
    typeGroup->addButton(rbConical, Qt::ConicalGradientPattern);
    // 选中当前类型
    if (style == Qt::RadialGradientPattern) rbRadial->setChecked(true);
    else if (style == Qt::ConicalGradientPattern) rbConical->setChecked(true);
    else rbLinear->setChecked(true);
    typeLayout->addWidget(rbLinear);
    typeLayout->addWidget(rbRadial);
    typeLayout->addWidget(rbConical);
    typeLayout->addStretch();
    layout->addLayout(typeLayout);

    // 起始色 / 结束色
    auto* colorLayout = new QHBoxLayout();
    colorLayout->addWidget(new QLabel(QStringLiteral("Start:"), &dlg));
    auto* btnStart = new DAColorPickerButton(&dlg);
    colorLayout->addWidget(btnStart);
    colorLayout->addWidget(new QLabel(QStringLiteral("End:"), &dlg));
    auto* btnEnd = new DAColorPickerButton(&dlg);
    colorLayout->addWidget(btnEnd);
    colorLayout->addStretch();
    layout->addLayout(colorLayout);

    // 从现有 brush 提取颜色
    QColor startColor = Qt::white, endColor = Qt::black;
    if (d->mBrush.gradient() && !d->mBrush.gradient()->stops().isEmpty()) {
        const auto& stops = d->mBrush.gradient()->stops();
        startColor = stops.first().second;
        endColor = stops.last().second;
    } else if (d->mBrush.color().isValid()) {
        startColor = d->mBrush.color();
        endColor = d->mBrush.color().darker(200);
    }
    btnStart->setColor(startColor);
    btnEnd->setColor(endColor);

    // 预览
    auto* previewLabel = new QLabel(&dlg);
    previewLabel->setMinimumHeight(40);
    previewLabel->setFrameShape(QFrame::Box);
    layout->addWidget(previewLabel);

    // 角度(仅 Linear/Conical 用)
    auto* angleLayout = new QHBoxLayout();
    angleLayout->addWidget(new QLabel(QStringLiteral("Angle:"), &dlg));
    auto* spinAngle = new QSpinBox(&dlg);
    spinAngle->setRange(0, 360);
    spinAngle->setSuffix(QStringLiteral("°"));
    angleLayout->addWidget(spinAngle);
    angleLayout->addStretch();
    layout->addLayout(angleLayout);

    // 按钮
    auto* btnBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dlg);
    layout->addWidget(btnBox);
    connect(btnBox, &QDialogButtonBox::accepted, &dlg, &QDialog::accept);
    connect(btnBox, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);

    // 更新预览
    auto updatePreview = [&]() {
        int w = previewLabel->width();
        int h = previewLabel->height();
        if (w <= 0) w = 200;
        if (h <= 0) h = 40;
        QPixmap pm(w, h);
        QPainter painter(&pm);
        int type = typeGroup->checkedId();
        int angle = spinAngle->value();
        if (type == Qt::LinearGradientPattern) {
            QLinearGradient g(0, 0, w, 0);
            // 按角度旋转
            qreal rad = qDegreesToRadians(static_cast<qreal>(angle));
            g.setStart(0, 0);
            g.setFinalStop(w * qCos(rad), h * qSin(rad));
            g.setColorAt(0.0, btnStart->color());
            g.setColorAt(1.0, btnEnd->color());
            painter.fillRect(pm.rect(), QBrush(g));
        } else if (type == Qt::RadialGradientPattern) {
            QRadialGradient g(w / 2.0, h / 2.0, qMax(w, h) / 2.0);
            g.setColorAt(0.0, btnStart->color());
            g.setColorAt(1.0, btnEnd->color());
            painter.fillRect(pm.rect(), QBrush(g));
        } else {
            QConicalGradient g(w / 2.0, h / 2.0, angle);
            g.setColorAt(0.0, btnStart->color());
            g.setColorAt(1.0, btnEnd->color());
            painter.fillRect(pm.rect(), QBrush(g));
        }
        previewLabel->setPixmap(pm);
    };

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    connect(typeGroup, &QButtonGroup::idClicked, [&](int) { updatePreview(); });
#else
    connect(typeGroup, QOverload<int>::of(&QButtonGroup::buttonClicked), [&](int) { updatePreview(); });
#endif
    connect(btnStart, &DAColorPickerButton::colorChanged, [&](const QColor&) { updatePreview(); });
    connect(btnEnd, &DAColorPickerButton::colorChanged, [&](const QColor&) { updatePreview(); });
    connect(spinAngle, QOverload<int>::of(&QSpinBox::valueChanged), [&](int) { updatePreview(); });

    // 延迟更新预览(等 layout 完成后)
    QTimer::singleShot(0, [&]() { updatePreview(); });

    if (dlg.exec() == QDialog::Accepted) {
        int type = typeGroup->checkedId();
        int angle = spinAngle->value();
        QGradient* gradient = nullptr;
        if (type == Qt::LinearGradientPattern) {
            qreal rad = qDegreesToRadians(static_cast<qreal>(angle));
            auto* g = new QLinearGradient(0, 0, 100 * qCos(rad), 100 * qSin(rad));
            g->setColorAt(0.0, btnStart->color());
            g->setColorAt(1.0, btnEnd->color());
            gradient = g;
        } else if (type == Qt::RadialGradientPattern) {
            auto* g = new QRadialGradient(0.5, 0.5, 0.5);
            g->setCoordinateMode(QGradient::ObjectBoundingMode);
            g->setColorAt(0.0, btnStart->color());
            g->setColorAt(1.0, btnEnd->color());
            gradient = g;
        } else {
            auto* g = new QConicalGradient(0.5, 0.5, angle);
            g->setCoordinateMode(QGradient::ObjectBoundingMode);
            g->setColorAt(0.0, btnStart->color());
            g->setColorAt(1.0, btnEnd->color());
            gradient = g;
        }

        QBrush newBrush(*gradient);
        delete gradient;
        d->mBrush = newBrush;
        // 颜色按钮显示起始色
        QSignalBlocker b(d->ui.colorButton);
        d->ui.colorButton->setColor(btnStart->color());
        Q_EMIT brushChanged(d->mBrush);
    } else {
        // 取消:恢复原来的样式
        QSignalBlocker b(d->ui.comboBox);
        d->ui.comboBox->setCurrentBrushStyle(d->mBrush.style());
    }
}

void DABrushEditWidget::editTexture()
{
    QString path = QFileDialog::getOpenFileName(
        this, QStringLiteral("Select Texture Image"), QString(),
        QStringLiteral("Images (*.png *.jpg *.jpeg *.bmp *.gif *.svg);;All Files (*)"));
    if (path.isEmpty()) {
        // 取消:恢复原来的样式
        QSignalBlocker b(d->ui.comboBox);
        d->ui.comboBox->setCurrentBrushStyle(d->mBrush.style());
        return;
    }
    QPixmap pm(path);
    if (pm.isNull()) {
        QSignalBlocker b(d->ui.comboBox);
        d->ui.comboBox->setCurrentBrushStyle(d->mBrush.style());
        return;
    }
    d->mBrush.setTexture(pm);
    QSignalBlocker b(d->ui.colorButton);
    d->ui.colorButton->setColor(QColor());  // 纹理模式下颜色无效
    Q_EMIT brushChanged(d->mBrush);
}
}  // namespace DA
