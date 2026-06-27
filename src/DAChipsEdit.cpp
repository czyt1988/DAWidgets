#include "DAChipsEdit.h"

#include "DAChip.h"
#include "DAFlowLayout.h"

#include <QFocusEvent>
#include <QKeyEvent>
#include <QLineEdit>
#include <QRegularExpression>
#include <QSet>

namespace DA
{
class DAChipsEdit::PrivateData
{
public:
    PrivateData(DAChipsEdit *) {}
    QStringList tags;
    DAFlowLayout *layout { nullptr };
    QLineEdit *input { nullptr };
    QList<DAChip *> chips;
    QString placeholder;
    bool allowCreate { true };
    bool allowDuplicate { false };
    int maxCount { -1 };
    QList<Separator> separators;
    QSet<QChar> extraSeparators;
};

DAChipsEdit::DAChipsEdit(QWidget *parent) : QFrame(parent), DA_PIMPL_CONSTRUCT
{
    DA_D(d);
    setObjectName(QStringLiteral("DAChipsEdit"));
    setFrameShape(QFrame::StyledPanel);
    setFocusPolicy(Qt::StrongFocus);

    d->layout = new DAFlowLayout(this, 2, 4, 4);
    d->layout->setContentsMargins(4, 4, 4, 4);

    d->input = new QLineEdit(this);
    d->input->setObjectName(QStringLiteral("DAChipsEditInput"));
    d->input->setFrame(false);
    d->input->setClearButtonEnabled(false);
    d->input->setPlaceholderText(d->placeholder);
    d->input->installEventFilter(this);
    d->layout->addWidget(d->input);  // 末尾，trailing expanding

    d->separators = { Separator::Comma, Separator::Enter };

    connect(d->input, &QLineEdit::returnPressed, this, &DAChipsEdit::commitInput);
    connect(d->input, &QLineEdit::textEdited, this, &DAChipsEdit::onInputEdited);
    connect(d->input, &QLineEdit::textChanged, this, &DAChipsEdit::inputTextChanged);
}

DAChipsEdit::~DAChipsEdit() = default;

QStringList DAChipsEdit::tags() const
{
    DA_DC(d);
    return d->tags;
}

void DAChipsEdit::setTags(const QStringList &tags)
{
    DA_D(d);
    QStringList clean;
    clean.reserve(tags.size());
    for (const QString &t : tags) {
        const QString s = t.trimmed();
        if (s.isEmpty())
            continue;
        if (!d->allowDuplicate && clean.contains(s))
            continue;
        clean << s;
    }
    if (d->maxCount >= 0 && clean.size() > d->maxCount)
        clean = clean.mid(0, d->maxCount);
    d->tags = clean;
    rebuildChips();
    Q_EMIT tagsChanged(d->tags);
}

void DAChipsEdit::appendTag(const QString &text)
{
    DA_D(d);
    const QString t = text.trimmed();
    if (t.isEmpty())
        return;
    if (d->maxCount >= 0 && d->tags.size() >= d->maxCount)
        return;
    if (!d->allowDuplicate && d->tags.contains(t))
        return;
    d->tags.append(t);
    addChipWidget(t);
    Q_EMIT tagAdded(t);
    Q_EMIT tagsChanged(d->tags);
}

bool DAChipsEdit::insertTag(int index, const QString &text)
{
    DA_D(d);
    const QString t = text.trimmed();
    if (t.isEmpty())
        return false;
    if (d->maxCount >= 0 && d->tags.size() >= d->maxCount)
        return false;
    if (!d->allowDuplicate && d->tags.contains(t))
        return false;
    index = qBound(0, index, d->tags.size());
    d->tags.insert(index, t);
    rebuildChips();
    Q_EMIT tagAdded(t);
    Q_EMIT tagsChanged(d->tags);
    return true;
}

void DAChipsEdit::removeTag(const QString &text)
{
    DA_D(d);
    const int idx = d->tags.indexOf(text);
    if (idx < 0)
        return;
    removeAt(idx);
}

void DAChipsEdit::removeAt(int index)
{
    DA_D(d);
    if (index < 0 || index >= d->tags.size())
        return;
    const QString t = d->tags.takeAt(index);
    if (index < d->chips.size()) {
        DAChip *chip = d->chips.takeAt(index);
        d->layout->removeWidget(chip);
        delete chip;
    }
    Q_EMIT tagRemoved(t);
    Q_EMIT tagsChanged(d->tags);
}

void DAChipsEdit::clear()
{
    DA_D(d);
    if (d->tags.isEmpty())
        return;
    d->tags.clear();
    rebuildChips();
    Q_EMIT tagsChanged(d->tags);
}

QString DAChipsEdit::placeholder() const
{
    DA_DC(d);
    return d->placeholder;
}

void DAChipsEdit::setPlaceholder(const QString &text)
{
    DA_D(d);
    d->placeholder = text;
    d->input->setPlaceholderText(text);
}

bool DAChipsEdit::allowCreate() const
{
    DA_DC(d);
    return d->allowCreate;
}

void DAChipsEdit::setAllowCreate(bool on)
{
    DA_D(d);
    d->allowCreate = on;
}

bool DAChipsEdit::allowDuplicate() const
{
    DA_DC(d);
    return d->allowDuplicate;
}

void DAChipsEdit::setAllowDuplicate(bool on)
{
    DA_D(d);
    d->allowDuplicate = on;
}

int DAChipsEdit::maxCount() const
{
    DA_DC(d);
    return d->maxCount;
}

void DAChipsEdit::setMaxCount(int count)
{
    DA_D(d);
    d->maxCount = count;
    if (d->maxCount >= 0 && d->tags.size() > d->maxCount) {
        d->tags = d->tags.mid(0, d->maxCount);
        rebuildChips();
        Q_EMIT tagsChanged(d->tags);
    }
}

void DAChipsEdit::setSeparators(const QList<Separator> &separators)
{
    DA_D(d);
    d->separators = separators;
}

QList<DAChipsEdit::Separator> DAChipsEdit::separators() const
{
    DA_DC(d);
    return d->separators;
}

void DAChipsEdit::setSeparators(const QString &chars)
{
    DA_D(d);
    d->extraSeparators.clear();
    for (const QChar &c : chars)
        d->extraSeparators.insert(c);
}

QString DAChipsEdit::inputText() const
{
    DA_DC(d);
    return d->input->text();
}

void DAChipsEdit::clearInput()
{
    DA_D(d);
    d->input->clear();
}

int DAChipsEdit::rowHeightHint() const
{
    DA_DC(d);
    const auto m = d->layout->contentsMargins();
    return d->input->sizeHint().height() + m.top() + m.bottom();
}

QSize DAChipsEdit::sizeHint() const
{
    return QFrame::sizeHint();
}

QSize DAChipsEdit::minimumSizeHint() const
{
    return QFrame::minimumSizeHint();
}

bool DAChipsEdit::eventFilter(QObject *watched, QEvent *event)
{
    DA_D(d);
    if (watched == d->input) {
        const QEvent::Type t = event->type();
        if (t == QEvent::FocusIn) {
            Q_EMIT inputFocusChanged(true);
        } else if (t == QEvent::FocusOut) {
            Q_EMIT inputFocusChanged(false);
        } else if (t == QEvent::KeyPress) {
            auto *ke = static_cast< QKeyEvent * >(event);
            bool handled = false;
            Q_EMIT inputKeyEvent(ke, &handled);
            if (handled)
                return true;
            const int key = ke->key();
            if (key == Qt::Key_Backspace && d->input->text().isEmpty() && !d->chips.isEmpty()) {
                removeAt(d->chips.size() - 1);
                return true;
            }
            if (key == Qt::Key_Tab && d->separators.contains(Separator::Tab)) {
                commitInput();
                return true;
            }
            if (key == Qt::Key_Escape) {
                d->input->clear();
                return true;
            }
        }
    }
    return QFrame::eventFilter(watched, event);
}

void DAChipsEdit::focusInEvent(QFocusEvent *event)
{
    QFrame::focusInEvent(event);
    DA_D(d);
    d->input->setFocus();
}

void DAChipsEdit::rebuildChips()
{
    DA_D(d);
    while (!d->chips.isEmpty()) {
        DAChip *chip = d->chips.takeLast();
        d->layout->removeWidget(chip);
        delete chip;
    }
    for (const QString &t : d->tags)
        addChipWidget(t);
}

void DAChipsEdit::addChipWidget(const QString &text)
{
    DA_D(d);
    auto *chip = new DAChip(text, this);
    chip->setClosable(true);
    const int idx = d->chips.size();  // 插在末尾输入框之前
    d->layout->insertWidget(idx, chip);
    d->chips.append(chip);
    chip->show();  // parent 可能已可见，需显式 show，否则 QWidgetItem sizeHint 为 0
    connect(chip, &DAChip::closeRequested, this, [this, chip]() {
        DA_D(d);
        const int i = d->chips.indexOf(chip);
        if (i >= 0)
            removeAt(i);
    });
}

void DAChipsEdit::commitInput()
{
    DA_D(d);
    const QString raw = d->input->text();
    if (raw.isEmpty())
        return;
    d->input->clear();
    if (!d->allowCreate)
        return;
    appendTag(raw);
}

void DAChipsEdit::onInputEdited(const QString &text)
{
    DA_D(d);
    if (!d->allowCreate)
        return;
    const QSet< QChar > seps = separatorChars();
    if (seps.isEmpty())
        return;
    bool has = false;
    for (const QChar &c : text) {
        if (seps.contains(c)) {
            has = true;
            break;
        }
    }
    if (!has)
        return;
#if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
    const QStringList parts = text.split(separatorRegex(), Qt::SkipEmptyParts);
#else
    const QStringList parts = text.split(separatorRegex(), QString::SkipEmptyParts);
#endif
    d->input->clear();
    for (const QString &p : parts)
        appendTag(p);
}

QSet< QChar > DAChipsEdit::separatorChars() const
{
    DA_DC(d);
    QSet< QChar > s = d->extraSeparators;
    for (Separator sep : d->separators) {
        switch (sep) {
        case Separator::Comma: s << QLatin1Char(','); break;
        case Separator::Semicolon: s << QLatin1Char(';'); break;
        case Separator::Space: s << QLatin1Char(' '); break;
        case Separator::Tab: s << QLatin1Char('\t'); break;
        case Separator::Enter: break;  // 由 returnPressed 处理
        }
    }
    return s;
}

QRegularExpression DAChipsEdit::separatorRegex() const
{
    QString pattern = QStringLiteral("[");
    for (const QChar &c : separatorChars()) {
        if (c == QLatin1Char(']') || c == QLatin1Char('\\') || c == QLatin1Char('^') || c == QLatin1Char('-'))
            pattern += QLatin1Char('\\');
        pattern += c;
    }
    pattern += QStringLiteral("]");
    return QRegularExpression(pattern);
}
}  // namespace DA
