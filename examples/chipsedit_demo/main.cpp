#include <QApplication>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>

#include <DAChipsEdit.h>

static const char *kStyle = R"(
DA--DAChipsEdit { background:#ffffff; border:1px solid #dcdfe6; border-radius:4px; }
DA--DAChipsEdit:focus { border:1px solid #409eff; }
DA--DAChip { background:#ecf5ff; border:1px solid #d9ecff; border-radius:4px; }
DA--DAChip:hover { background:#d9ecff; }
QLabel#DAChipLabel { color:#409eff; }
QToolButton#DAChipCloseButton { border:none; color:#909399; font-weight:bold; }
QToolButton#DAChipCloseButton:hover { color:#f56c6c; }
QLineEdit#DAChipsEditInput { border:none; background:transparent; }
)";

int main(int argc, char **argv)
{
    QApplication app(argc, argv);
    app.setStyleSheet(QString::fromLatin1(kStyle));

    QWidget w;
    w.setWindowTitle(QStringLiteral("DAChipsEdit Demo"));

    auto *v = new QVBoxLayout(&w);

    auto *edit = new DA::DAChipsEdit(&w);
    edit->setPlaceholder(QStringLiteral("请选择文章标签（回车或逗号提交，Backspace 删除）"));
    edit->setSeparators(QList<DA::DAChipsEdit::Separator>{
        DA::DAChipsEdit::Separator::Comma,
        DA::DAChipsEdit::Separator::Enter,
        DA::DAChipsEdit::Separator::Semicolon,
    });
    edit->setTags({ QStringLiteral("Qt"), QStringLiteral("C++") });
    v->addWidget(edit);

    auto *row = new QHBoxLayout;
    auto *lbl = new QLabel(QStringLiteral("tags: Qt, C++"));
    auto *addBtn = new QPushButton(QStringLiteral("append 'QWidget'"));
    auto *clearBtn = new QPushButton(QStringLiteral("clear"));
    row->addWidget(lbl, 1);
    row->addWidget(addBtn);
    row->addWidget(clearBtn);
    v->addLayout(row);

    QObject::connect(edit, &DA::DAChipsEdit::tagsChanged,
                     [lbl](const QStringList &t) {
                         lbl->setText(QStringLiteral("tags: ") + t.join(QStringLiteral(", ")));
                     });
    QObject::connect(edit, &DA::DAChipsEdit::tagAdded,
                     [](const QString &t) { qDebug("added: %s", qPrintable(t)); });
    QObject::connect(edit, &DA::DAChipsEdit::tagRemoved,
                     [](const QString &t) { qDebug("removed: %s", qPrintable(t)); });
    QObject::connect(addBtn, &QPushButton::clicked,
                     [edit] { edit->appendTag(QStringLiteral("QWidget")); });
    QObject::connect(clearBtn, &QPushButton::clicked, edit, &DA::DAChipsEdit::clear);

    w.resize(460, 140);
    w.show();
    return app.exec();
}
