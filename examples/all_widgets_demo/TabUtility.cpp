#include <QComboBox>
#include <QCoreApplication>
#include <QDateTime>
#include <QDir>
#include <QEventLoop>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMenuBar>
#include <QMenu>
#include <QMessageBox>
#include <QPushButton>
#include <QScrollArea>
#include <QSpinBox>
#include <QTextEdit>
#include <QTimer>
#include <QVBoxLayout>

#include <DAFilePathEditWidget.h>
#include <DAPropertyItemWidget.h>
#include <DARecentFilesManager.h>
#include <DATimeFormatEditWidget.h>
#include <DAWaitCursorScoped.h>

// ============================================================
// DAFilePathEditWidget —— 文件路径编辑器
// ============================================================
static QGroupBox *createFilePathGroup(QWidget *parent)
{
    auto *box = new QGroupBox(QStringLiteral("DAFilePathEditWidget — 文件路径编辑器"), parent);
    auto *v = new QVBoxLayout(box);

    auto *desc = new QLabel(
        QStringLiteral("路径文本框 + 浏览按钮，点击浏览弹出 QFileDialog 选择文件。"), box);
    desc->setWordWrap(true);
    v->addWidget(desc);

    auto *edit = new DA::DAFilePathEditWidget(box);
    edit->setNameFilter(QStringLiteral("C++ 文件 (*.cpp *.h);;所有文件 (*.*)"));
    v->addWidget(edit);

    auto *log = new QLabel(box);
    log->setStyleSheet(QStringLiteral("color:#909399;"));
    v->addWidget(log);

    QObject::connect(edit, &DA::DAFilePathEditWidget::selectedPath,
        [log](const QString &p) {
            log->setText(QStringLiteral("已选择: ") + p);
        });

    return box;
}

// ============================================================
// DATimeFormatEditWidget —— 时间格式编辑器
// ============================================================
static QGroupBox *createTimeFormatGroup(QWidget *parent)
{
    auto *box = new QGroupBox(QStringLiteral("DATimeFormatEditWidget — 时间格式编辑器"), parent);
    auto *v = new QVBoxLayout(box);

    auto *desc = new QLabel(
        QStringLiteral("预置常用日期/时间格式的下拉框，也可手动输入自定义格式。"), box);
    desc->setWordWrap(true);
    v->addWidget(desc);

    auto *edit = new DA::DATimeFormatEditWidget(box);
    v->addWidget(edit);

    // 用当前时间按所选格式显示预览
    auto *preview = new QLabel(box);
    preview->setStyleSheet(QStringLiteral("color:#909399;"));

    auto updatePreview = [edit, preview]() {
        QString fmt = edit->getTimeFormat();
        preview->setText(QStringLiteral("预览: ")
                         + QDateTime::currentDateTime().toString(fmt));
    };
    updatePreview();

    // 通过 QLineEdit 的 textChanged 信号来监听格式变化
    auto *combo = edit->findChild<QComboBox *>();
    if (combo) {
        QObject::connect(combo, &QComboBox::currentTextChanged, edit, [updatePreview]() { updatePreview(); });
    }

    // 定时刷新预览
    auto *timer = new QTimer(box);
    timer->start(1000);
    QObject::connect(timer, &QTimer::timeout, updatePreview);

    v->addWidget(preview);

    return box;
}

// ============================================================
// DAPropertyItemWidget —— 属性项组件
// ============================================================
static QGroupBox *createPropertyItemGroup(QWidget *parent)
{
    auto *box = new QGroupBox(QStringLiteral("DAPropertyItemWidget — 属性项组件"), parent);
    auto *v = new QVBoxLayout(box);

    auto *desc = new QLabel(
        QStringLiteral("将属性名标签与编辑控件配对显示，支持 InlineLayout（左右排列）\n"
                       "和 BelowLayout（上下排列）两种布局模式。"), box);
    desc->setWordWrap(true);
    v->addWidget(desc);

    // ---- InlineLayout 模式 ----
    v->addWidget(new QLabel(QStringLiteral("InlineLayout 模式（标签在左，编辑器在右）:")));

    auto *prop1 = new DA::DAPropertyItemWidget(box);
    prop1->setPropertyName(QStringLiteral("名称"));
    prop1->setPropertyDescription(QStringLiteral("设置对象名称"));
    prop1->setEditorWidget(new QLineEdit(QStringLiteral("MyObject"), prop1));

    auto *prop2 = new DA::DAPropertyItemWidget(box);
    prop2->setPropertyName(QStringLiteral("字号"));
    prop2->setPropertyDescription(QStringLiteral("设置字体大小"));
    auto *spin = new QSpinBox(prop2);
    spin->setRange(1, 999);
    spin->setValue(14);
    prop2->setEditorWidget(spin);

    v->addWidget(prop1);
    v->addWidget(prop2);

    // ---- BelowLayout 模式 ----
    v->addSpacing(8);
    v->addWidget(new QLabel(QStringLiteral("BelowLayout 模式（标签在上，编辑器在下）:")));

    auto *prop3 = new DA::DAPropertyItemWidget(box);
    prop3->setLayoutMode(DA::DAPropertyItemWidget::BelowLayout);
    prop3->setPropertyName(QStringLiteral("备注"));
    prop3->setPropertyDescription(QStringLiteral("输入备注信息"));
    prop3->setEditorWidget(new QTextEdit(prop3));
    prop3->editorWidget()->setMaximumHeight(60);

    v->addWidget(prop3);

    auto *log = new QLabel(box);
    log->setStyleSheet(QStringLiteral("color:#909399;"));
    v->addWidget(log);

    QObject::connect(prop1, &DA::DAPropertyItemWidget::valueChanged,
        [log](int id) { log->setText(QStringLiteral("属性 #%1 值变更").arg(id)); });
    QObject::connect(prop2, &DA::DAPropertyItemWidget::valueChanged,
        [log](int id) { log->setText(QStringLiteral("属性 #%1 值变更").arg(id)); });

    return box;
}

// ============================================================
// DARecentFilesManager —— 最近文件管理器
// ============================================================
static QGroupBox *createRecentFilesGroup(QWidget *parent)
{
    auto *box = new QGroupBox(QStringLiteral("DARecentFilesManager — 最近文件管理器"), parent);
    auto *v = new QVBoxLayout(box);

    auto *desc = new QLabel(
        QStringLiteral("管理「最近打开的文件」列表，持久化到 QSettings，自动过滤不存在的路径。\n"
                       "点击下方菜单栏体验。"), box);
    desc->setWordWrap(true);
    v->addWidget(desc);

    auto *menuBar = new QMenuBar(box);
    auto *fileMenu = menuBar->addMenu(QStringLiteral("文件(&F)"));

    auto *mgr = new DA::DARecentFilesManager(box, 8, QStringLiteral("DAWidgets"), QStringLiteral("AllWidgetsDemo"));
    mgr->attachToMenu(fileMenu);

    // 添加可执行文件自身路径用于演示（保证文件存在）
    mgr->addFile(QCoreApplication::applicationFilePath());

    auto *log = new QLabel(box);
    log->setStyleSheet(QStringLiteral("color:#909399;"));
    v->addWidget(menuBar);
    v->addWidget(log);

    QObject::connect(mgr, &DA::DARecentFilesManager::fileSelected,
        [log](const QString &path) {
            log->setText(QStringLiteral("已选择文件: ") + path);
        });

    return box;
}

// ============================================================
// DAWaitCursorScoped —— RAII 等待光标守卫
// ============================================================
static QGroupBox *createWaitCursorGroup(QWidget *parent)
{
    auto *box = new QGroupBox(QStringLiteral("DAWaitCursorScoped — RAII 等待光标守卫"), parent);
    auto *v = new QVBoxLayout(box);

    auto *desc = new QLabel(
        QStringLiteral("构造时设置等待光标（沙漏），析构时自动恢复，防止异常路径导致光标卡死。\n"
                       "也可使用宏 DA_WAIT_CURSOR_SCOPED_NS() 快速创建。"), box);
    desc->setWordWrap(true);
    v->addWidget(desc);

    auto *btn = new QPushButton(QStringLiteral("模拟耗时操作（2秒等待光标）"), box);
    v->addWidget(btn);

    auto *log = new QLabel(box);
    log->setStyleSheet(QStringLiteral("color:#909399;"));
    v->addWidget(log);

    QObject::connect(btn, &QPushButton::clicked, [log]() {
        // RAII 守卫：构造时显示沙漏，离开作用域自动恢复
        DA::DAWaitCursorScoped waitCursor;
        log->setText(QStringLiteral("正在执行耗时操作..."));
        QEventLoop loop;
        QTimer::singleShot(2000, &loop, &QEventLoop::quit);
        loop.exec();
        log->setText(QStringLiteral("耗时操作完成，光标已恢复"));
    });

    return box;
}

// ============================================================
// 入口
// ============================================================
QWidget *createUtilityTab(QWidget *parent)
{
    auto *scroll = new QScrollArea(parent);
    scroll->setWidgetResizable(true);
    auto *content = new QWidget;
    auto *v = new QVBoxLayout(content);
    v->setSpacing(12);

    v->addWidget(createFilePathGroup(content));
    v->addWidget(createTimeFormatGroup(content));
    v->addWidget(createPropertyItemGroup(content));
    v->addWidget(createRecentFilesGroup(content));
    v->addWidget(createWaitCursorGroup(content));
    v->addStretch(1);

    scroll->setWidget(content);
    return scroll;
}
