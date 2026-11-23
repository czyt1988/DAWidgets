// DAWidgets 单文件合并示例
//
// 本示例只依赖 src-amalgamate/DAWidgets.h + src-amalgamate/DAWidgets.cpp，
// 不链接 DAWidgets 库目标，用来验证合成文件可以按两个文件直接编译使用。
// 支持 --smoke：启动后立即退出，供无界面 CI 做链接与资源注册的冒烟测试。

#include "DAWidgets.h"

#include <QApplication>
#include <QLabel>
#include <QTimer>
#include <QVBoxLayout>
#include <QWidget>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QWidget window;
    window.setWindowTitle(QStringLiteral("DAWidgets - single-file amalgamation demo"));
    window.resize(520, 360);

    auto *layout = new QVBoxLayout(&window);

    // Selector 家族：多标签下拉选择框
    auto *select = new DA::DASelectComboBox(&window);
    select->setPlaceholder(QStringLiteral("Pick or create tags"));
    select->setOptions({ QStringLiteral("Qt"), QStringLiteral("C++"),
                         QStringLiteral("Python"), QStringLiteral("CMake") });
    select->setTags({ QStringLiteral("Qt") });
    select->setAllowCreate(true);
    layout->addWidget(select);

    // Collapse 家族：带展开动画的折叠面板
    auto *collapse = new DA::DACollapseWidget(&window);
    auto *first = collapse->addItem(QStringLiteral("Selector"),
                                    QStringLiteral("selector"));
    first->contentLayout()->addWidget(new QLabel(QStringLiteral(
        "DASelectComboBox, DAChipsEdit, DAChip and DAFlowLayout.")));
    auto *second = collapse->addItem(QStringLiteral("Collapse"),
                                     QStringLiteral("collapse"));
    second->contentLayout()->addWidget(new QLabel(QStringLiteral(
        "DACollapseWidget, DACollapseItem and DACollapseLayout.")));
    collapse->setActiveNames({ QStringLiteral("selector") });
    layout->addWidget(collapse);
    layout->addStretch(1);

    window.show();

    if (app.arguments().contains(QStringLiteral("--smoke")))
        QTimer::singleShot(0, &app, &QCoreApplication::quit);

    return app.exec();
}
