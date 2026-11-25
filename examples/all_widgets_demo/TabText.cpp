#include <QAction>
#include <QCheckBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QVBoxLayout>

#include <DATextEditWidget.h>

#include "TabPages.h"

// ============================================================
// DATextEditWidget —— 文本编辑/查看
// ============================================================
static QWidget *createTextEditContent(QWidget *parent)
{
    auto *w = new QWidget(parent);
    auto *v = new QVBoxLayout(w);
    v->setContentsMargins(8, 4, 8, 4);

    auto *desc = new QLabel(
        QStringLiteral("只读/等宽/换行可切换；工具动作由调用方注入（这里演示“另存为”只打日志），"
                       "控件本身不拥有 OK/Cancel。\n"
                       "勾选「行号」左侧显示行号（行号 = 逻辑块号）；勾选「查找」后按 Ctrl+F 打开查找条，"
                       "Esc 关闭，只读模式下同样可用。"), w);
    desc->setWordWrap(true);
    v->addWidget(desc);

    auto *edit = new DA::DATextEditWidget(w);
    QStringList sample;
    for (int i = 1; i <= 80; ++i) {
        if (i % 10 == 0)
            sample << QStringLiteral("%1: ERROR 数据校验失败，请检查第 %1 行").arg(i);
        else
            sample << QStringLiteral("%1: INFO the quick brown fox jumps over the lazy dog").arg(i);
    }
    edit->setText(sample.join(QLatin1Char('\n')));
    edit->setPlaceholderText(QStringLiteral("输入文本..."));

    auto *readOnly = new QCheckBox(QStringLiteral("只读"), w);
    auto *mono = new QCheckBox(QStringLiteral("等宽字体"), w);
    auto *wrap = new QCheckBox(QStringLiteral("自动换行"), w);
    wrap->setChecked(true);
    auto *toolbar = new QCheckBox(QStringLiteral("显示工具条"), w);
    toolbar->setChecked(true);
    auto *lineNumbers = new QCheckBox(QStringLiteral("行号"), w);
    auto *find = new QCheckBox(QStringLiteral("查找 (Ctrl+F)"), w);

    auto *controls = new QHBoxLayout();
    controls->addWidget(readOnly);
    controls->addWidget(mono);
    controls->addWidget(wrap);
    controls->addWidget(toolbar);
    controls->addWidget(lineNumbers);
    controls->addWidget(find);
    controls->addStretch(1);
    v->addLayout(controls);

    auto *saveAction = new QAction(QStringLiteral("另存为"), edit);
    edit->addToolAction(saveAction);
    edit->setToolbarVisible(true);

    v->addWidget(edit, 1);

    auto *log = new QLabel(w);
    log->setStyleSheet(QStringLiteral("color:#909399;"));
    log->setWordWrap(true);
    v->addWidget(log);

    QObject::connect(readOnly, &QCheckBox::toggled, edit, &DA::DATextEditWidget::setReadOnly);
    QObject::connect(mono, &QCheckBox::toggled, edit, &DA::DATextEditWidget::setMonospace);
    QObject::connect(wrap, &QCheckBox::toggled, edit, &DA::DATextEditWidget::setLineWrapEnabled);
    QObject::connect(toolbar, &QCheckBox::toggled, edit, &DA::DATextEditWidget::setToolbarVisible);
    QObject::connect(lineNumbers, &QCheckBox::toggled, edit, &DA::DATextEditWidget::setLineNumbersVisible);
    QObject::connect(find, &QCheckBox::toggled, edit, &DA::DATextEditWidget::setFindEnabled);
    lineNumbers->setChecked(true);  // demo 显式打开行号，截图可见
    QObject::connect(saveAction, &QAction::triggered, log,
                     [log]() { log->setText(QStringLiteral("注入的“另存为”动作被触发")); });
    QObject::connect(edit, &DA::DATextEditWidget::textChanged, log,
                     [log, edit]() { log->setText(QStringLiteral("textChanged，当前 %1 字符").arg(edit->text().size())); });

    return w;
}

// ============================================================
// 场景列表（tab 与截图导出共用）
// ============================================================
QVector< DemoScene > textScenes()
{
    return {
        { QStringLiteral("text-edit"), QStringLiteral("DATextEditWidget — 文本编辑/查看"),
          createTextEditContent, 340 },
    };
}
