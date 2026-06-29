#include <QApplication>
#include <QTabWidget>

#include "TabPages.h"

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
QGroupBox {
    font-weight: bold; border: 1px solid #e4e7ed; border-radius: 4px;
    margin-top: 10px; padding-top: 14px;
}
QGroupBox::title { subcontrol-origin: margin; left: 10px; padding: 0 4px; }
QLabel#sectionTitle { font-size: 14px; font-weight: bold; color: #303133; }
)";

int main(int argc, char **argv)
{
    QApplication app(argc, argv);
    app.setStyleSheet(QString::fromLatin1(kStyle));

    QTabWidget tabs;
    tabs.setWindowTitle(QStringLiteral("DAWidgets 全控件演示 — 一个窗口体验所有组件"));
    tabs.addTab(createSelectorTab(&tabs),      QStringLiteral("选择器 / 标签"));
    tabs.addTab(createCollapseTab(&tabs),      QStringLiteral("折叠面板"));
    tabs.addTab(createColorTab(&tabs),         QStringLiteral("颜色"));
    tabs.addTab(createDrawingStyleTab(&tabs),  QStringLiteral("绘图样式"));
    tabs.addTab(createAlignmentTab(&tabs),     QStringLiteral("对齐"));
    tabs.addTab(createUtilityTab(&tabs),       QStringLiteral("工具组件"));

    tabs.resize(720, 640);
    tabs.show();
    return app.exec();
}
