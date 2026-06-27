#include <QApplication>
#include <QCheckBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QVBoxLayout>
#include <QWidget>

#include <DASelectComboBox.h>

// DASelectComboBox 自带现代化默认样式（边框/drop-down/箭头），
// 这里只补充 chip 与 popup 的样式，不覆盖 QComboBox 相关规则。
static const char *kStyle = R"(
DA--DAChip { background:#ecf5ff; border:1px solid #d9ecff; border-radius:4px; }
DA--DAChip:hover { background:#d9ecff; }
QLabel#DAChipLabel { color:#409eff; }
QToolButton#DAChipCloseButton { border:none; color:#909399; font-weight:bold; }
QToolButton#DAChipCloseButton:hover { color:#f56c6c; }
QLineEdit#DAChipsEditInput { border:none; background:transparent; }
QListWidget#DASelectPopup { border:1px solid #dcdfe6; background:#fff; outline:0; }
QListWidget#DASelectPopup::item { padding:4px 8px; }
QListWidget#DASelectPopup::item:selected { background:#ecf5ff; color:#409eff; }
)";

int main(int argc, char **argv)
{
    QApplication app(argc, argv);
    app.setStyleSheet(QString::fromLatin1(kStyle));

    QWidget w;
    w.setWindowTitle(QStringLiteral("DASelectComboBox Demo"));

    auto *v = new QVBoxLayout(&w);

    auto *cb = new DA::DASelectComboBox(&w);
    cb->setPlaceholder(QStringLiteral("请选择文章标签（输入过滤，回车选中；不在列表中可直接创建）"));
    cb->setOptions({
        QStringLiteral("Qt"), QStringLiteral("C++"), QStringLiteral("Python"),
        QStringLiteral("QML"), QStringLiteral("Qt Quick"), QStringLiteral("JavaScript"),
        QStringLiteral("Rust"), QStringLiteral("Go"), QStringLiteral("Kotlin"),
    });
    cb->setAllowCreate(true);
    cb->setTags({ QStringLiteral("Qt"), QStringLiteral("C++") });
    v->addWidget(cb);

    auto *row = new QHBoxLayout;
    auto *lbl = new QLabel(QStringLiteral("tags: Qt, C++"), &w);
    auto *growCb = new QCheckBox(QStringLiteral("随标签增高 (取消则固定高度滚动)"), &w);
    growCb->setChecked(true);
    row->addWidget(lbl, 1);
    row->addWidget(growCb);
    v->addLayout(row);

    QObject::connect(cb, &DA::DASelectComboBox::tagsChanged,
                     [lbl](const QStringList &t) {
                         lbl->setText(QStringLiteral("tags: ") + t.join(QStringLiteral(", ")));
                     });
    QObject::connect(growCb, &QCheckBox::toggled,
                     [cb](bool on) {
                         cb->setHeightMode(on ? DA::DASelectComboBox::HeightMode::GrowWithTags
                                              : DA::DASelectComboBox::HeightMode::FixedHeightScrollable);
                     });
    QObject::connect(cb, &DA::DASelectComboBox::optionSelected,
                     [](const QString &t) { qDebug("selected: %s", qPrintable(t)); });
    QObject::connect(cb, &DA::DASelectComboBox::created,
                     [](const QString &t) { qDebug("created: %s", qPrintable(t)); });

    w.resize(480, 180);
    w.show();
    return app.exec();
}
