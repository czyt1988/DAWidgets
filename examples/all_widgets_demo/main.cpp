#include <QApplication>
#include <QDebug>
#include <QTabWidget>
#include <QTimer>

#include "DemoScene.h"
#include "GalleryExport.h"

// 全局样式：合并三个已有 demo 的 qss，并补充 TabWidget 自身样式
static const char *kStyle = R"(
/* ---- QTabWidget ---- */
QTabWidget::pane { border: 1px solid #dcdfe6; top: -1px; }
QTabBar::tab {
    background: #f5f7fa; border: 1px solid #dcdfe6;
    padding: 6px 16px; margin-right: 2px;
    border-top-left-radius: 4px; border-top-right-radius: 4px;
}
QTabBar::tab:selected { background: #ffffff; border-bottom: 1px solid #ffffff; }
QTabBar::tab:hover:!selected { background: #ecf5ff; }

/* ---- DAChip / DAChipsEdit ---- */
DA--DAChipsEdit { background:#ffffff; border:1px solid #dcdfe6; border-radius:4px; }
DA--DAChipsEdit:focus { border:1px solid #409eff; }
DA--DAChip { background:#ecf5ff; border:1px solid #d9ecff; border-radius:4px; }
DA--DAChip:hover { background:#d9ecff; }
QLabel#DAChipLabel { color:#409eff; }
QToolButton#DAChipCloseButton { border:none; color:#909399; font-weight:bold; }
QToolButton#DAChipCloseButton:hover { color:#f56c6c; }
QLineEdit#DAChipsEditInput { border:none; background:transparent; }

/* ---- DASelectComboBox popup ---- */
QListWidget#DASelectPopup { border:1px solid #dcdfe6; background:#fff; outline:0; }
QListWidget#DASelectPopup::item { padding:4px 8px; }
QListWidget#DASelectPopup::item:selected { background:#ecf5ff; color:#409eff; }

/* ---- DACollapse ---- */
DA--DACollapseItem {
    border:1px solid #ebeef5; border-radius:4px; margin-bottom:8px;
}
QFrame#header { background:#fafafa; border:none; border-top-left-radius:4px; border-top-right-radius:4px; }
QFrame#header:hover { background:#f5f7fa; }
QLabel#DACollapseItemTitle { color:#303133; font-weight:bold; }
DA--DACollapseItem[disabled="true"] QFrame#header { color:#c0c4cc; }
DA--DACollapseItem[disabled="true"] QFrame#header:hover { background:#fafafa; }
QFrame#DACollapseItemContent { background:#ffffff; }

/* ---- 通用 ---- */
QLabel#sectionTitle { font-size: 14px; font-weight: bold; color: #303133; }
)";

// 打印命令行帮助
static void printUsage(const QString &exe)
{
    qInfo().noquote() << "Usage:" << exe << "[options]";
    qInfo().noquote() << "  (no option)                    open the all-widgets demo window";
    qInfo().noquote() << "  --shot <png> [tabIndex]        grab one tab and exit";
    qInfo().noquote()
        << "  --export-gallery [outputDir]   export tab/widget/popup screenshots (default docs/images/gallery)";
    qInfo().noquote() << "Example:" << exe << "--export-gallery docs/images/gallery";
}

int main(int argc, char **argv)
{
    QApplication app(argc, argv);
    app.setApplicationName(QStringLiteral("dawidgets_all_widgets_demo"));
    app.setStyleSheet(QString::fromLatin1(kStyle));

    const QStringList args = app.arguments();

    // 一键导出文档画廊截图（不打开主窗口，导出完直接退出）
    const int galleryArg = args.indexOf(QStringLiteral("--export-gallery"));
    if (galleryArg >= 0) {
        QString outputDir = QStringLiteral("docs/images/gallery");
        if (galleryArg + 1 < args.size() && !args.at(galleryArg + 1).startsWith(QLatin1String("--"))) {
            outputDir = args.at(galleryArg + 1);
        }
        QString error;
        if (!exportDemoGallery(outputDir, &error)) {
            qCritical().noquote() << "Export failed:" << error;
            return 1;
        }
        return 0;
    }

    if (args.contains(QStringLiteral("--help")) || args.contains(QStringLiteral("-h"))) {
        printUsage(args.value(0));
        return 0;
    }

    QTabWidget tabs;
    tabs.setWindowTitle(QStringLiteral("DAWidgets 全控件演示 — 一个窗口体验所有组件"));
    const QVector< DemoTab > demos = demoTabs();
    for (const DemoTab &tab : demos) {
        tabs.addTab(createSceneTab(tab.scenes, &tabs), tab.title);
    }

    tabs.resize(720, 640);
    tabs.show();

    // 截图模式（用于验收留档/回归对比，不参与正常交互）：
    //   dawidgets_all_widgets_demo.exe --shot <png> [tabIndex]
    const int shotArg = args.indexOf(QStringLiteral("--shot"));
    if (shotArg >= 0 && shotArg + 1 < args.size()) {
        const QString shotPath = args.at(shotArg + 1);
        const int tabIndex     = (shotArg + 2 < args.size()) ? args.at(shotArg + 2).toInt() : 0;
        tabs.setCurrentIndex(qBound(0, tabIndex, tabs.count() - 1));
        QTimer::singleShot(300, &app, [ &tabs, shotPath, &app ]() {
            tabs.grab().save(shotPath);
            app.quit();
        });
    }

    return app.exec();
}
