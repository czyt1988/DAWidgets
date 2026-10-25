#include <QDialog>
#include <QFileDialog>
#include <QGroupBox>
#include <QLabel>
#include <QPushButton>
#include <QScrollArea>
#include <QVBoxLayout>

#include <DACheckListWidget.h>
#include <DACollapseItem.h>
#include <DACollapseWidget.h>
#include <DAItemListPickDialog.h>
#include <DAStringListEditWidget.h>

using DA::DAItemEntry;

static QVector< DAItemEntry > makeEntries(const QStringList &texts)
{
    QVector< DAItemEntry > res;
    res.reserve(texts.size());
    for (const QString &t : texts) {
        res.append(DAItemEntry { t, QVariant() });
    }
    return res;
}

static QString entriesText(const QVector< DAItemEntry > &entries)
{
    QStringList texts;
    for (const DAItemEntry &e : entries) {
        texts << e.text;
    }
    return texts.join(QStringLiteral(", "));
}

static QLabel *makeLog(QWidget *parent)
{
    auto *log = new QLabel(parent);
    log->setStyleSheet(QStringLiteral("color:#909399;"));
    log->setWordWrap(true);
    return log;
}

// ============================================================
// DACheckListWidget —— 普通勾选（全选/反选）
// ============================================================
static QWidget *createCheckListContent(QWidget *parent)
{
    auto *w = new QWidget(parent);
    auto *v = new QVBoxLayout(w);
    v->setContentsMargins(8, 4, 8, 4);

    auto *desc = new QLabel(
        QStringLiteral("带搜索的勾选列表。全选只作用于当前可见、可勾选、非 forced 的条目；"
                       "过滤只隐藏行，勾选状态保留。"), w);
    desc->setWordWrap(true);
    v->addWidget(desc);

    auto *check = new DA::DACheckListWidget(w);
    check->setItems(makeEntries({ QStringLiteral("alpha"), QStringLiteral("beta"),
                                  QStringLiteral("gamma"), QStringLiteral("delta") }));
    check->setSearchPlaceholderText(QStringLiteral("搜索条目..."));
    check->setSelectAllVisible(true);
    check->setInvertVisible(true);
    v->addWidget(check, 1);

    auto *log = makeLog(w);
    v->addWidget(log);
    QObject::connect(check, &DA::DACheckListWidget::checkedEntriesChanged, log, [ log, check ]() {
        log->setText(QStringLiteral("已勾选(%1): %2")
                         .arg(check->checkedCount())
                         .arg(entriesText(check->checkedEntries())));
    });
    return w;
}

// ============================================================
// DACheckListWidget —— 子 Agent 工具白名单（forced + disabled）
// ============================================================
static QWidget *createForcedListContent(QWidget *parent)
{
    auto *w = new QWidget(parent);
    auto *v = new QVBoxLayout(w);
    v->setContentsMargins(8, 4, 8, 4);

    auto *desc = new QLabel(
        QStringLiteral("模拟子 Agent 工具白名单：内置工具 save_project 强制勾选且不可取消；"
                       "其余工具用户可选。"), w);
    desc->setWordWrap(true);
    v->addWidget(desc);

    auto *check = new DA::DACheckListWidget(w);
    check->setItems(makeEntries({ QStringLiteral("read_file"), QStringLiteral("write_file"),
                                  QStringLiteral("run_script"), QStringLiteral("fetch_data"),
                                  QStringLiteral("save_project") }));
    const DAItemEntry forced { QStringLiteral("save_project"), QVariant() };
    check->setEntryForcedChecked(forced, true);
    check->setEntryEnabled(forced, false);
    check->setCheckedEntries(makeEntries({ QStringLiteral("read_file") }));
    check->setSelectAllVisible(true);
    check->setInvertVisible(true);
    v->addWidget(check, 1);

    auto *log = makeLog(w);
    v->addWidget(log);
    auto update = [ log, check, forced ]() {
        log->setText(QStringLiteral("勾选数 %1（含 forced: %2）: %3")
                         .arg(check->checkedCount())
                         .arg(check->checkedEntries().contains(forced) ? QStringLiteral("是")
                                                                      : QStringLiteral("否"))
                         .arg(entriesText(check->checkedEntries())));
    };
    QObject::connect(check, &DA::DACheckListWidget::checkedEntriesChanged, log, update);
    update();
    return w;
}

// ============================================================
// DAItemListPickDialog —— 搜索 + 多选一次性添加
// ============================================================
static QWidget *createPickDialogContent(QWidget *parent)
{
    auto *w = new QWidget(parent);
    auto *v = new QVBoxLayout(w);
    v->setContentsMargins(8, 4, 8, 4);

    auto *desc = new QLabel(
        QStringLiteral("DAItemListPickDialog 是 DACheckListWidget 的对话框壳："
                       "搜索 + 勾选 + OK/Cancel。"), w);
    desc->setWordWrap(true);
    v->addWidget(desc);

    auto *btn = new QPushButton(QStringLiteral("打开条目勾选对话框"), w);
    v->addWidget(btn);
    auto *log = makeLog(w);
    v->addWidget(log);
    QObject::connect(btn, &QPushButton::clicked, w, [ w, log ]() {
        DA::DAItemListPickDialog dlg(w);
        dlg.setItems(makeEntries({ QStringLiteral("列 A"), QStringLiteral("列 B"),
                                   QStringLiteral("列 C"), QStringLiteral("列 D") }));
        dlg.setCheckedEntries(makeEntries({ QStringLiteral("列 B") }));
        if (dlg.exec() == QDialog::Accepted) {
            log->setText(QStringLiteral("勾选: %1").arg(entriesText(dlg.checkedEntries())));
        }
    });
    return w;
}

// ============================================================
// DAStringListEditWidget —— 纯字符串列表（Inline / External）
// ============================================================
static QWidget *createStringListContent(QWidget *parent)
{
    auto *w = new QWidget(parent);
    auto *v = new QVBoxLayout(w);
    v->setContentsMargins(8, 4, 8, 4);

    auto *desc = new QLabel(
        QStringLiteral("Inline：Add 追加可编辑空行（不再插入 New Item 占位）；"
                       "External：Add 只发 addRequested，由调用方选择目录后 appendItem。"), w);
    desc->setWordWrap(true);
    v->addWidget(desc);

    // Inline
    auto *inlineBox = new QGroupBox(QStringLiteral("Inline 模式（可编辑 + 上下移动）"), w);
    auto *inlineLay = new QVBoxLayout(inlineBox);
    auto *inlineEdit = new DA::DAStringListEditWidget(inlineBox);
    inlineEdit->setAddMode(DA::DAStringListEditWidget::AddMode::Inline);
    inlineEdit->setReorderEnabled(true);
    inlineEdit->setPlaceholderText(QStringLiteral("点击 Add 追加一行，直接输入文本"));
    inlineEdit->setItems({ QStringLiteral("C:/Qt"), QStringLiteral("C:/Python311") });
    inlineLay->addWidget(inlineEdit);
    auto *inlineLog = makeLog(inlineBox);
    inlineLay->addWidget(inlineLog);
    QObject::connect(inlineEdit, &DA::DAStringListEditWidget::itemsChanged, inlineLog,
                     [ inlineLog, inlineEdit ]() {
                         inlineLog->setText(QStringLiteral("items: %1")
                                                .arg(inlineEdit->items().join(QStringLiteral(" | "))));
                     });
    v->addWidget(inlineBox);

    // External
    auto *externalBox = new QGroupBox(QStringLiteral("External 模式（Add 触发目录选择）"), w);
    auto *externalLay = new QVBoxLayout(externalBox);
    auto *externalEdit = new DA::DAStringListEditWidget(externalBox);
    externalEdit->setAddMode(DA::DAStringListEditWidget::AddMode::External);
    externalEdit->setPlaceholderText(QStringLiteral("点击 Add 选择目录"));
    externalLay->addWidget(externalEdit);
    auto *externalLog = makeLog(externalBox);
    externalLay->addWidget(externalLog);
    QObject::connect(externalEdit, &DA::DAStringListEditWidget::addRequested, externalEdit,
                     [ externalEdit ]() {
                         const QString d = QFileDialog::getExistingDirectory(
                             externalEdit, QStringLiteral("选择模块搜索路径"));
                         if (!d.isEmpty()) {
                             externalEdit->appendItem(d);
                         }
                     });
    QObject::connect(externalEdit, &DA::DAStringListEditWidget::itemsChanged, externalLog,
                     [ externalLog, externalEdit ]() {
                         externalLog->setText(QStringLiteral("items: %1")
                                                  .arg(externalEdit->items().join(QStringLiteral(" | "))));
                     });
    v->addWidget(externalBox);

    return w;
}

// ============================================================
// 入口
// ============================================================
QWidget *createItemSelectionTab(QWidget *parent)
{
    auto *scroll = new QScrollArea(parent);
    scroll->setWidgetResizable(true);
    auto *content = new QWidget;
    auto *v = new QVBoxLayout(content);
    v->setSpacing(12);

    auto *collapse = new DA::DACollapseWidget(content);
    collapse->addItem(QStringLiteral("DACheckListWidget — 普通勾选（全选/反选）"), QStringLiteral("1"))
        ->setContentWidget(createCheckListContent(collapse));
    collapse->addItem(QStringLiteral("DACheckListWidget — 子 Agent 工具白名单（forced）"), QStringLiteral("2"))
        ->setContentWidget(createForcedListContent(collapse));
    collapse->addItem(QStringLiteral("DAItemListPickDialog — 条目勾选对话框"), QStringLiteral("3"))
        ->setContentWidget(createPickDialogContent(collapse));
    collapse->addItem(QStringLiteral("DAStringListEditWidget — 纯字符串列表"), QStringLiteral("4"))
        ->setContentWidget(createStringListContent(collapse));
    collapse->setActiveNames({ QStringLiteral("1") });

    v->addWidget(collapse);
    v->addStretch(1);

    scroll->setWidget(content);
    return scroll;
}
