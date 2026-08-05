#include <QComboBox>
#include <QCoreApplication>
#include <QDateTime>
#include <QDir>
#include <QEventLoop>
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

#include <DACollapseItem.h>
#include <DACollapseWidget.h>
#include <DAFilePathEditWidget.h>
#include <DAPropertyItemWidget.h>
#include <DATimeFormatEditWidget.h>
#include <DAWaitCursorScoped.h>

// ============================================================
// DAFilePathEditWidget —— 文件路径编辑器
// ============================================================
static QWidget *createFilePathContent(QWidget *parent)
{
    auto *w = new QWidget(parent);
    auto *v = new QVBoxLayout(w);
    v->setContentsMargins(8, 4, 8, 4);

    auto *desc = new QLabel(
        QStringLiteral("路径文本框 + 浏览按钮，点击浏览弹出 QFileDialog 选择文件。"), w);
    desc->setWordWrap(true);
    v->addWidget(desc);

    auto *edit = new DA::DAFilePathEditWidget(w);
    edit->setNameFilter(QStringLiteral("C++ 文件 (*.cpp *.h);;所有文件 (*.*)"));
    v->addWidget(edit);

    auto *log = new QLabel(w);
    log->setStyleSheet(QStringLiteral("color:#909399;"));
    v->addWidget(log);

    QObject::connect(edit, &DA::DAFilePathEditWidget::selectedPath,
        [log](const QString &p) {
            log->setText(QStringLiteral("已选择: ") + p);
        });

    return w;
}

// ============================================================
// DATimeFormatEditWidget —— 时间格式编辑器
// ============================================================
static QWidget *createTimeFormatContent(QWidget *parent)
{
    auto *w = new QWidget(parent);
    auto *v = new QVBoxLayout(w);
    v->setContentsMargins(8, 4, 8, 4);

    auto *desc = new QLabel(
        QStringLiteral("预置常用日期/时间格式的下拉框，也可手动输入自定义格式。"), w);
    desc->setWordWrap(true);
    v->addWidget(desc);

    auto *edit = new DA::DATimeFormatEditWidget(w);
    v->addWidget(edit);

    // 用当前时间按所选格式显示预览
    auto *preview = new QLabel(w);
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
    auto *timer = new QTimer(w);
    timer->start(1000);
    QObject::connect(timer, &QTimer::timeout, updatePreview);

    v->addWidget(preview);

    return w;
}

// ============================================================
// DAPropertyItemWidget —— 属性项组件
// ============================================================
static QWidget *createPropertyItemContent(QWidget *parent)
{
    auto *w = new QWidget(parent);
    auto *v = new QVBoxLayout(w);
    v->setContentsMargins(8, 4, 8, 4);

    auto *desc = new QLabel(
        QStringLiteral("将属性名标签与编辑控件配对显示，支持 InlineLayout（左右排列）\n"
                       "和 BelowLayout（上下排列）两种布局模式。"), w);
    desc->setWordWrap(true);
    v->addWidget(desc);

    // ---- InlineLayout 模式 ----
    v->addWidget(new QLabel(QStringLiteral("InlineLayout 模式（标签在左，编辑器在右）:")));

    auto *prop1 = new DA::DAPropertyItemWidget(w);
    prop1->setPropertyName(QStringLiteral("名称"));
    prop1->setPropertyDescription(QStringLiteral("设置对象名称"));
    prop1->setEditorWidget(new QLineEdit(QStringLiteral("MyObject"), prop1));

    auto *prop2 = new DA::DAPropertyItemWidget(w);
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

    auto *prop3 = new DA::DAPropertyItemWidget(w);
    prop3->setLayoutMode(DA::DAPropertyItemWidget::BelowLayout);
    prop3->setPropertyName(QStringLiteral("备注"));
    prop3->setPropertyDescription(QStringLiteral("输入备注信息"));
    prop3->setEditorWidget(new QTextEdit(prop3));
    prop3->editorWidget()->setMaximumHeight(60);

    v->addWidget(prop3);

    auto *log = new QLabel(w);
    log->setStyleSheet(QStringLiteral("color:#909399;"));
    v->addWidget(log);

    QObject::connect(prop1, &DA::DAPropertyItemWidget::valueChanged,
        [log](int id) { log->setText(QStringLiteral("属性 #%1 值变更").arg(id)); });
    QObject::connect(prop2, &DA::DAPropertyItemWidget::valueChanged,
        [log](int id) { log->setText(QStringLiteral("属性 #%1 值变更").arg(id)); });

    return w;
}

// ============================================================
// DAWaitCursorScoped —— RAII 等待光标守卫
// ============================================================
static QWidget *createWaitCursorContent(QWidget *parent)
{
    auto *w = new QWidget(parent);
    auto *v = new QVBoxLayout(w);
    v->setContentsMargins(8, 4, 8, 4);

    auto *desc = new QLabel(
        QStringLiteral("构造时设置等待光标（沙漏），析构时自动恢复，防止异常路径导致光标卡死。\n"
                       "也可使用宏 DA_WAIT_CURSOR_SCOPED_NS() 快速创建。"), w);
    desc->setWordWrap(true);
    v->addWidget(desc);

    auto *btn = new QPushButton(QStringLiteral("模拟耗时操作（2秒等待光标）"), w);
    v->addWidget(btn);

    auto *log = new QLabel(w);
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

    return w;
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

    auto *collapse = new DA::DACollapseWidget(content);
    collapse->addItem(QStringLiteral("DAFilePathEditWidget — 文件路径编辑器"), QStringLiteral("1"))
        ->setContentWidget(createFilePathContent(collapse));
    collapse->addItem(QStringLiteral("DATimeFormatEditWidget — 时间格式编辑器"), QStringLiteral("2"))
        ->setContentWidget(createTimeFormatContent(collapse));
    collapse->addItem(QStringLiteral("DAPropertyItemWidget — 属性项组件"), QStringLiteral("3"))
        ->setContentWidget(createPropertyItemContent(collapse));
    collapse->addItem(QStringLiteral("DAWaitCursorScoped — RAII 等待光标守卫"), QStringLiteral("5"))
        ->setContentWidget(createWaitCursorContent(collapse));
    collapse->setActiveNames({ QStringLiteral("1") });

    v->addWidget(collapse);
    v->addStretch(1);

    scroll->setWidget(content);
    return scroll;
}
