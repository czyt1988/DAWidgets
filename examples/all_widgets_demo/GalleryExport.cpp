#include "GalleryExport.h"

#include <QApplication>
#include <QColor>
#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QEventLoop>
#include <QListWidget>
#include <QPixmap>
#include <QPoint>
#include <QTabWidget>
#include <QTimer>
#include <QVBoxLayout>

#include <DAColorGridWidget.h>
#include <DAColorMenu.h>
#include <DAColorPaletteGridWidget.h>
#include <DAItemListPickDialog.h>
#include <DASelectComboBox.h>

#include "DemoScene.h"

namespace
{

// showPopup()/hidePopup() 在 DASelectComboBox 中是 protected，这里仅为截图开放
class PopupSelectComboBox : public DA::DASelectComboBox
{
public:
    explicit PopupSelectComboBox(QWidget *parent = nullptr) : DA::DASelectComboBox(parent)
    {
    }
    using DA::DASelectComboBox::hidePopup;
    using DA::DASelectComboBox::showPopup;
};

// 等待界面完成布局/动画/弹出，再截图
void waitForUi(int ms)
{
    QEventLoop loop;
    QTimer::singleShot(ms, &loop, &QEventLoop::quit);
    loop.exec();
    QCoreApplication::processEvents();
}

QVector< DA::DAItemEntry > makeEntries(const QStringList &texts)
{
    QVector< DA::DAItemEntry > res;
    res.reserve(texts.size());
    for (const QString &t : texts) {
        res.append(DA::DAItemEntry { t, QVariant() });
    }
    return res;
}

bool savePixmap(const QPixmap &pixmap, const QString &path, QString *errorMessage)
{
    if (pixmap.isNull()) {
        if (errorMessage) {
            *errorMessage = QStringLiteral("render returned a null pixmap: %1").arg(path);
        }
        return false;
    }
    if (!pixmap.save(path, "PNG")) {
        if (errorMessage) {
            *errorMessage = QStringLiteral("failed to write: %1").arg(path);
        }
        return false;
    }
    qInfo().noquote() << "  " << QDir::toNativeSeparators(path);
    return true;
}

// 把一个场景内容放进白底卡片里渲染成 PNG
QPixmap renderScene(const DemoScene &scene)
{
    QWidget card;
    card.setObjectName(QStringLiteral("galleryCard"));
    card.setAttribute(Qt::WA_StyledBackground, true);
    card.setStyleSheet(QStringLiteral(
        "QWidget#galleryCard { background: #ffffff; border: 1px solid #dcdfe6; border-radius: 6px; }"));

    auto *layout = new QVBoxLayout(&card);
    layout->setContentsMargins(20, 20, 20, 20);
    layout->setSpacing(0);

    QWidget *content = scene.create(&card);
    layout->addWidget(content);

    // 先给一个临时尺寸让 sizeHint 生效，再按内容宽度与场景偏好高度定稿
    card.resize(760, 240);
    card.show();
    waitForUi(120);

    const int hintWidth  = content->sizeHint().width() + 40;
    const int width      = qBound(640, qMax(760, hintWidth), 1000);
    // 含伸缩子控件的场景用固定高度，其余按内容收紧，避免大片空白
    const int height = scene.preferredHeight > 0 ? scene.preferredHeight
                                                 : content->sizeHint().height() + 40;
    card.resize(width, height);
    waitForUi(160);

    const QPixmap shot = card.grab();
    card.hide();
    return shot;
}

void exportSceneShots(const QString &widgetDir, const QVector< DemoTab > &tabs, int *count, QString *error)
{
    for (const DemoTab &tab : tabs) {
        qInfo().noquote() << QStringLiteral("[%1]").arg(tab.slug);
        for (const DemoScene &scene : tab.scenes) {
            const QString path = widgetDir + QLatin1Char('/') + scene.id + QStringLiteral(".png");
            if (savePixmap(renderScene(scene), path, error)) {
                ++(*count);
            }
        }
    }
}

void exportTabShots(const QString &dir, const QVector< DemoTab > &tabs, int *count, QString *error)
{
    QTabWidget host;
    host.setWindowTitle(QStringLiteral("DAWidgets 全控件演示"));
    for (const DemoTab &tab : tabs) {
        host.addTab(createSceneTab(tab.scenes, &host, true), tab.title);
    }
    host.resize(960, 840);
    host.show();
    waitForUi(500);

    for (int i = 0; i < tabs.size(); ++i) {
        host.setCurrentIndex(i);
        waitForUi(350);
        const QString name = QStringLiteral("%1-%2.png").arg(i + 1, 2, 10, QLatin1Char('0')).arg(tabs.at(i).slug);
        if (savePixmap(host.grab(), dir + QStringLiteral("/tabs/") + name, error)) {
            ++(*count);
        }
    }

    // 主窗口首屏（README 顶部大图）
    host.setCurrentIndex(0);
    waitForUi(350);
    if (savePixmap(host.grab(), dir + QStringLiteral("/all-widgets-demo.png"), error)) {
        ++(*count);
    }
    host.hide();
}

void exportPopupShots(const QString &widgetDir, int *count, QString *error)
{
    // 1) DASelectComboBox 下拉弹层
    {
        QWidget host;
        host.setWindowTitle(QStringLiteral("DASelectComboBox popup"));
        auto *layout = new QVBoxLayout(&host);
        layout->setContentsMargins(16, 16, 16, 16);
        auto *combo = new PopupSelectComboBox(&host);
        combo->setPlaceholder(QStringLiteral("请选择标签（输入过滤，回车选中）"));
        combo->setOptions({ QStringLiteral("Qt"), QStringLiteral("C++"), QStringLiteral("Python"),
                            QStringLiteral("QML"), QStringLiteral("Qt Quick"),
                            QStringLiteral("JavaScript"), QStringLiteral("Rust"), QStringLiteral("Go"),
                            QStringLiteral("Kotlin") });
        combo->setTags({ QStringLiteral("Qt"), QStringLiteral("C++") });
        layout->addWidget(combo);
        layout->addStretch(1);
        host.resize(620, 180);
        host.move(120, 120);
        host.show();
        waitForUi(200);

        combo->showPopup();
        waitForUi(450);
        // 自定义弹出框是 Qt::ToolTip 顶层窗口，不属于 activePopupWidget，按 objectName 查找
        QListWidget *popup = nullptr;
        for (QWidget *top : QApplication::topLevelWidgets()) {
            if (top->objectName() == QLatin1String("DASelectPopup") && top->isVisible()) {
                popup = qobject_cast< QListWidget * >(top);
                break;
            }
        }
        if (popup) {
            if (savePixmap(popup->grab(), widgetDir + QStringLiteral("/select-combobox-popup.png"),
                           error)) {
                ++(*count);
            }
            combo->hidePopup();
        }
        waitForUi(120);
    }

    // 2) DAColorMenu 弹出菜单（含 None 与自定义色记录）
    {
        DA::DAColorMenu menu(QStringLiteral("选择颜色"));
        menu.enableNoneColorAction(true);
        if (auto *palette = menu.colorPaletteGridWidget()) {
            palette->setColorIconSize(QSize(18, 18));
        }
        if (auto *grid = menu.customColorsWidget()) {
            grid->setColorList({ QColor(QStringLiteral("#409EFF")), QColor(QStringLiteral("#67C23A")),
                                 QColor(QStringLiteral("#E6A23C")), QColor(QStringLiteral("#F56C6C")),
                                 QColor(QStringLiteral("#909399")) });
        }
        menu.popup(QPoint(220, 220));
        waitForUi(400);
        // QWidgetAction 里的调色板在显示后才有最终 sizeHint，重新布局避免被裁切
        menu.ensurePolished();
        menu.adjustSize();
        waitForUi(150);
        if (savePixmap(menu.grab(), widgetDir + QStringLiteral("/color-menu-popup.png"), error)) {
            ++(*count);
        }
        menu.hide();
        waitForUi(120);
    }

    // 3) DAItemListPickDialog 条目勾选对话框
    {
        DA::DAItemListPickDialog dialog;
        dialog.setWindowTitle(QStringLiteral("选择条目"));
        dialog.setItems(makeEntries({ QStringLiteral("列 A"), QStringLiteral("列 B"),
                                      QStringLiteral("列 C"), QStringLiteral("列 D"),
                                      QStringLiteral("列 E"), QStringLiteral("列 F") }));
        dialog.setCheckedEntries(makeEntries({ QStringLiteral("列 B"), QStringLiteral("列 E") }));
        dialog.resize(440, 430);
        dialog.move(180, 100);
        dialog.show();
        waitForUi(400);
        if (savePixmap(dialog.grab(), widgetDir + QStringLiteral("/pick-dialog-window.png"), error)) {
            ++(*count);
        }
        dialog.close();
        waitForUi(120);
    }
}

}  // namespace

bool exportDemoGallery(const QString &dir, QString *errorMessage)
{
    const QString base = QDir(dir).absolutePath();
    if (!QDir().mkpath(base + QStringLiteral("/tabs")) ||
        !QDir().mkpath(base + QStringLiteral("/widgets"))) {
        if (errorMessage) {
            *errorMessage = QStringLiteral("cannot create output directory: %1").arg(base);
        }
        return false;
    }

    const QVector< DemoTab > tabs = demoTabs();
    int count                     = 0;
    QString error;

    qInfo().noquote() << QStringLiteral("Tab screenshots -> %1/tabs").arg(QDir::toNativeSeparators(base));
    exportTabShots(base, tabs, &count, &error);
    if (!error.isEmpty()) {
        if (errorMessage) {
            *errorMessage = error;
        }
        return false;
    }

    qInfo().noquote() << QStringLiteral("Widget screenshots -> %1/widgets").arg(QDir::toNativeSeparators(base));
    exportSceneShots(base + QStringLiteral("/widgets"), tabs, &count, &error);
    if (!error.isEmpty()) {
        if (errorMessage) {
            *errorMessage = error;
        }
        return false;
    }

    qInfo().noquote() << QStringLiteral("Popup screenshots -> %1/widgets").arg(QDir::toNativeSeparators(base));
    exportPopupShots(base + QStringLiteral("/widgets"), &count, &error);
    if (!error.isEmpty()) {
        if (errorMessage) {
            *errorMessage = error;
        }
        return false;
    }

    qInfo().noquote() << QStringLiteral("Done: %1 screenshots -> %2")
                             .arg(count)
                             .arg(QDir::toNativeSeparators(base));
    return true;
}
