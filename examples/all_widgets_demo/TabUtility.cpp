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
#include <QPainter>
#include <QPushButton>
#include <QScrollArea>
#include <QSpinBox>
#include <QTextEdit>
#include <QTimer>
#include <QUndoCommand>
#include <QUndoGroup>
#include <QUndoStack>
#include <QVBoxLayout>
#include <memory>

#include <DAAbstractWidgetOverlay.h>
#include <DACollapseItem.h>
#include <DACollapseWidget.h>
#include <DAFilePathEditWidget.h>
#include <DAPropertyItemWidget.h>
#include <DATimeFormatEditWidget.h>
#include <DAUndoHistoryWidget.h>
#include <DAWaitCursorScoped.h>

// ============================================================
// DAFilePathEditWidget —— 文件路径编辑器（三种模式）
// ============================================================
static void addFilePathMode(QWidget *parent, QVBoxLayout *v,
                            const QString &title,
                            DA::DAFilePathEditWidget::Mode mode,
                            const QString &suffix,
                            const QString &filter = QString())
{
    auto *box = new QGroupBox(title, parent);
    auto *lay = new QVBoxLayout(box);
    lay->setContentsMargins(8, 4, 8, 4);
    auto *edit = new DA::DAFilePathEditWidget(box);
    edit->setMode(mode);
    if (!suffix.isEmpty()) {
        edit->setDefaultSuffix(suffix);
    }
    if (!filter.isEmpty()) {
        edit->setNameFilter(filter);
    }
    lay->addWidget(edit);

    auto *log = new QLabel(box);
    log->setStyleSheet(QStringLiteral("color:#909399;"));
    log->setWordWrap(true);
    lay->addWidget(log);
    QObject::connect(edit, &DA::DAFilePathEditWidget::selectedPath,
        [log](const QString &p) {
            log->setText(QStringLiteral("已选择: ") + p);
        });
    v->addWidget(box);
}

static QWidget *createFilePathContent(QWidget *parent)
{
    auto *w = new QWidget(parent);
    auto *v = new QVBoxLayout(w);
    v->setContentsMargins(8, 4, 8, 4);

    auto *desc = new QLabel(
        QStringLiteral("路径文本框 + 浏览按钮。三种模式共用同一控件：\n"
                       "OpenFile 选文件、SaveFile 选保存路径（带默认后缀）、"
                       "ExistingDirectory 选目录。"), w);
    desc->setWordWrap(true);
    v->addWidget(desc);

    addFilePathMode(w, v, QStringLiteral("OpenFile — 打开文件"),
                    DA::DAFilePathEditWidget::Mode::OpenFile, QString(),
                    QStringLiteral("C++ 文件 (*.cpp *.h);;所有文件 (*.*)"));
    addFilePathMode(w, v, QStringLiteral("SaveFile — 保存文件（默认后缀 txt）"),
                    DA::DAFilePathEditWidget::Mode::SaveFile, QStringLiteral("txt"),
                    QStringLiteral("文本文件 (*.txt);;所有文件 (*.*)"));
    addFilePathMode(w, v, QStringLiteral("ExistingDirectory — 选择目录"),
                    DA::DAFilePathEditWidget::Mode::ExistingDirectory, QString());

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
// DAUndoHistoryWidget —— 撤销历史 + 深度调节
// ============================================================
namespace
{
class DemoAddCommand : public QUndoCommand
{
public:
    DemoAddCommand(int *value, int delta)
        : mValue(value), mDelta(delta)
    {
        setText(QStringLiteral("加 %1").arg(delta));
    }
    void undo() override
    {
        *mValue -= mDelta;
    }
    void redo() override
    {
        *mValue += mDelta;
    }

private:
    int *mValue { nullptr };
    int mDelta { 0 };
};

class DemoOverlay : public DA::DAAbstractWidgetOverlay
{
public:
    explicit DemoOverlay(QWidget *parent) : DAAbstractWidgetOverlay(parent)
    {
    }

protected:
    void drawOverlay(QPainter *painter) const override
    {
        painter->fillRect(rect(), QColor(64, 158, 255, 90));
        painter->setPen(QColor(64, 158, 255));
        painter->drawRect(rect().adjusted(1, 1, -2, -2));
        painter->drawText(rect(), Qt::AlignCenter, QStringLiteral("Overlay"));
    }
};
}  // namespace

static QWidget *createUndoHistoryContent(QWidget *parent)
{
    auto *w = new QWidget(parent);
    auto *v = new QVBoxLayout(w);
    v->setContentsMargins(8, 4, 8, 4);

    auto *desc = new QLabel(
        QStringLiteral("QUndoView 跟随 QUndoGroup 的激活栈；深度 SpinBox 只发"
                       " undoLimitChangeRequested，由调用方持久化并施加到各栈。"), w);
    desc->setWordWrap(true);
    v->addWidget(desc);

    auto *value = new QLabel(w);
    value->setStyleSheet(QStringLiteral("color:#909399;"));
    // 用 shared_ptr 持有计数器：命令与 lambda 都可能比本函数活得久
    auto counter = std::make_shared< int >(0);
    auto refresh = [value, counter]() {
        value->setText(QStringLiteral("当前值: %1").arg(*counter));
    };
    refresh();

    auto *stack = new QUndoStack(w);
    auto *group = new QUndoGroup(w);
    group->addStack(stack);

    auto *history = new DA::DAUndoHistoryWidget(w);
    history->setUndoGroup(group);
    v->addWidget(history, 1);

    auto *btns = new QHBoxLayout();
    auto *plus = new QPushButton(QStringLiteral("+1"), w);
    auto *minus = new QPushButton(QStringLiteral("-1"), w);
    btns->addWidget(plus);
    btns->addWidget(minus);
    btns->addWidget(value);
    btns->addStretch(1);
    v->addLayout(btns);

    QObject::connect(plus, &QPushButton::clicked, w, [stack, counter, refresh]() {
        stack->push(new DemoAddCommand(counter.get(), 1));
        refresh();
    });
    QObject::connect(minus, &QPushButton::clicked, w, [stack, counter, refresh]() {
        stack->push(new DemoAddCommand(counter.get(), -1));
        refresh();
    });
    QObject::connect(history, &DA::DAUndoHistoryWidget::undoLimitChangeRequested, value,
                     [value](int limit) {
                         value->setText(QStringLiteral("请求撤销深度: %1（当前值见上方）").arg(limit));
                     });
    return w;
}

// ============================================================
// DAAbstractWidgetOverlay —— 半透明遮罩
// ============================================================
static QWidget *createOverlayContent(QWidget *parent)
{
    auto *w = new QWidget(parent);
    auto *v = new QVBoxLayout(w);
    v->setContentsMargins(8, 4, 8, 4);

    auto *desc = new QLabel(
        QStringLiteral("遮罩层叠在目标控件上，父控件 resize 时自动跟随；切换 MaskMode/"
                       "RenderMode 后调用 updateOverlay() 重算遮罩。"), w);
    desc->setWordWrap(true);
    v->addWidget(desc);

    auto *target = new QLabel(QStringLiteral("被遮罩的控件（QLabel）"), w);
    target->setAlignment(Qt::AlignCenter);
    target->setMinimumSize(360, 120);
    target->setStyleSheet(QStringLiteral("background:#f5f7fa;border:1px solid #dcdfe6;"));
    v->addWidget(target);

    auto *overlay = new DemoOverlay(target);
    overlay->resize(target->size());
    overlay->show();
    overlay->raise();

    auto *maskCombo = new QComboBox(w);
    maskCombo->addItem(QStringLiteral("NoMask"), static_cast<int>(DA::DAAbstractWidgetOverlay::NoMask));
    maskCombo->addItem(QStringLiteral("MaskHint"), static_cast<int>(DA::DAAbstractWidgetOverlay::MaskHint));
    maskCombo->addItem(QStringLiteral("AlphaMask"), static_cast<int>(DA::DAAbstractWidgetOverlay::AlphaMask));
    maskCombo->setCurrentIndex(1);

    auto *renderCombo = new QComboBox(w);
    renderCombo->addItem(QStringLiteral("AutoRenderMode"),
                         static_cast<int>(DA::DAAbstractWidgetOverlay::AutoRenderMode));
    renderCombo->addItem(QStringLiteral("CopyAlphaMask"),
                         static_cast<int>(DA::DAAbstractWidgetOverlay::CopyAlphaMask));
    renderCombo->addItem(QStringLiteral("DrawOverlay"), static_cast<int>(DA::DAAbstractWidgetOverlay::DrawOverlay));

    auto *row = new QHBoxLayout();
    row->addWidget(new QLabel(QStringLiteral("MaskMode:"), w));
    row->addWidget(maskCombo);
    row->addWidget(new QLabel(QStringLiteral("RenderMode:"), w));
    row->addWidget(renderCombo);
    row->addStretch(1);
    v->addLayout(row);

    QObject::connect(maskCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), w,
                     [overlay, maskCombo](int) {
                         overlay->setMaskMode(static_cast<DA::DAAbstractWidgetOverlay::MaskMode>(
                             maskCombo->currentData().toInt()));
                         overlay->updateOverlay();
                     });
    QObject::connect(renderCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), w,
                     [overlay, renderCombo](int) {
                         overlay->setRenderMode(static_cast<DA::DAAbstractWidgetOverlay::RenderMode>(
                             renderCombo->currentData().toInt()));
                         overlay->updateOverlay();
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
    collapse->addItem(QStringLiteral("DAUndoHistoryWidget — 撤销历史"), QStringLiteral("4"))
        ->setContentWidget(createUndoHistoryContent(collapse));
    collapse->addItem(QStringLiteral("DAAbstractWidgetOverlay — 遮罩层"), QStringLiteral("5"))
        ->setContentWidget(createOverlayContent(collapse));
    collapse->addItem(QStringLiteral("DAWaitCursorScoped — RAII 等待光标守卫"), QStringLiteral("6"))
        ->setContentWidget(createWaitCursorContent(collapse));
    collapse->setActiveNames({ QStringLiteral("1") });

    v->addWidget(collapse);
    v->addStretch(1);

    scroll->setWidget(content);
    return scroll;
}
