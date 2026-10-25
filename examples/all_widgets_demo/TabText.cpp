#include <QAction>
#include <QCheckBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QScrollArea>
#include <QVBoxLayout>

#include <DACollapseItem.h>
#include <DACollapseWidget.h>
#include <DATextEditWidget.h>

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
                       "控件本身不拥有 OK/Cancel。"), w);
    desc->setWordWrap(true);
    v->addWidget(desc);

    auto *edit = new DA::DATextEditWidget(w);
    edit->setText(QStringLiteral("第一行文本\n第二行：等宽字体会让列对齐更整齐\n"
                                 "    indented line\n"
                                 "The quick brown fox jumps over the lazy dog."));
    edit->setPlaceholderText(QStringLiteral("输入文本..."));

    auto *readOnly = new QCheckBox(QStringLiteral("只读"), w);
    auto *mono = new QCheckBox(QStringLiteral("等宽字体"), w);
    auto *wrap = new QCheckBox(QStringLiteral("自动换行"), w);
    wrap->setChecked(true);
    auto *toolbar = new QCheckBox(QStringLiteral("显示工具条"), w);
    toolbar->setChecked(true);

    auto *controls = new QHBoxLayout();
    controls->addWidget(readOnly);
    controls->addWidget(mono);
    controls->addWidget(wrap);
    controls->addWidget(toolbar);
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
    QObject::connect(saveAction, &QAction::triggered, log,
                     [log]() { log->setText(QStringLiteral("注入的“另存为”动作被触发")); });
    QObject::connect(edit, &DA::DATextEditWidget::textChanged, log,
                     [log, edit]() { log->setText(QStringLiteral("textChanged，当前 %1 字符").arg(edit->text().size())); });

    return w;
}

// ============================================================
// 入口
// ============================================================
QWidget *createTextTab(QWidget *parent)
{
    auto *scroll = new QScrollArea(parent);
    scroll->setWidgetResizable(true);
    auto *content = new QWidget;
    auto *v = new QVBoxLayout(content);
    v->setSpacing(12);

    auto *collapse = new DA::DACollapseWidget(content);
    collapse->addItem(QStringLiteral("DATextEditWidget — 文本编辑/查看"), QStringLiteral("1"))
        ->setContentWidget(createTextEditContent(collapse));
    collapse->setActiveNames({ QStringLiteral("1") });

    v->addWidget(collapse);
    v->addStretch(1);

    scroll->setWidget(content);
    return scroll;
}
