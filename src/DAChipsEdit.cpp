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

/**
 * @brief 构造流式多标签输入文本框
 * @param parent 父控件
 * @details 创建 DAFlowLayout 流式布局和内嵌 QLineEdit 输入框，输入框设为布局末尾项
 *          以实现 trailing expanding 自适应宽度。安装事件过滤器拦截按键与焦点事件。
 *          默认分隔符为逗号和回车。
 */
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

/**
 * @brief 析构
 * @details PIMPL 指针由 DA_PIMPL_CONSTRUCT 宏自动释放
 */
DAChipsEdit::~DAChipsEdit() = default;

/**
 * @brief 获取当前所有标签文本列表
 * @return 标签字符串列表
 */
QStringList DAChipsEdit::tags() const
{
    DA_DC(d);
    return d->tags;
}

/**
 * @brief 设置标签列表
 * @param tags 标签字符串列表
 * @details 替换当前所有标签，自动去空白、去重（若不允许重复）、截断至 maxCount，
 *          然后重建所有 DAChip 子控件并发射 tagsChanged
 */
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

/**
 * @brief 追加一个标签到末尾
 * @param text 标签文本
 * @details 空文本、达到 maxCount 上限、重复（若不允许重复）时静默忽略。
 *          成功时创建 DAChip 子控件并发射 tagAdded 与 tagsChanged
 */
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

/**
 * @brief 在指定位置插入标签
 * @param index 插入位置
 * @param text 标签文本
 * @return 成功返回 true，失败（空文本/超限/重复/越界）返回 false
 * @details 成功后重建所有 DAChip 子控件并发射 tagAdded 与 tagsChanged
 */
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

/**
 * @brief 移除第一个匹配文本的标签
 * @param text 要移除的标签文本
 */
void DAChipsEdit::removeTag(const QString &text)
{
    DA_D(d);
    const int idx = d->tags.indexOf(text);
    if (idx < 0)
        return;
    removeAt(idx);
}

/**
 * @brief 移除指定索引处的标签
 * @param index 标签索引
 * @details 移除对应的 DAChip 子控件并发射 tagRemoved 与 tagsChanged
 */
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

/**
 * @brief 清空所有标签
 * @details 若已无标签则不触发信号，否则重建子控件并发射 tagsChanged
 */
void DAChipsEdit::clear()
{
    DA_D(d);
    if (d->tags.isEmpty())
        return;
    d->tags.clear();
    rebuildChips();
    Q_EMIT tagsChanged(d->tags);
}

/**
 * @brief 获取占位提示文本
 * @return 占位提示文本
 */
QString DAChipsEdit::placeholder() const
{
    DA_DC(d);
    return d->placeholder;
}

/**
 * @brief 设置占位提示文本
 * @param text 占位提示文本
 * @details 同步更新内嵌输入框的 placeholder
 */
void DAChipsEdit::setPlaceholder(const QString &text)
{
    DA_D(d);
    d->placeholder = text;
    d->input->setPlaceholderText(text);
}

/**
 * @brief 是否允许添加非候选列表中的文本作为标签
 * @return 允许创建返回 true
 */
bool DAChipsEdit::allowCreate() const
{
    DA_DC(d);
    return d->allowCreate;
}

/**
 * @brief 设置是否允许创建新标签
 * @param on 是否允许
 */
void DAChipsEdit::setAllowCreate(bool on)
{
    DA_D(d);
    d->allowCreate = on;
}

/**
 * @brief 是否允许重复标签
 * @return 允许重复返回 true
 */
bool DAChipsEdit::allowDuplicate() const
{
    DA_DC(d);
    return d->allowDuplicate;
}

/**
 * @brief 设置是否允许重复标签
 * @param on 是否允许
 */
void DAChipsEdit::setAllowDuplicate(bool on)
{
    DA_D(d);
    d->allowDuplicate = on;
}

/**
 * @brief 获取标签数量上限
 * @return 上限值，-1 表示无限制
 */
int DAChipsEdit::maxCount() const
{
    DA_DC(d);
    return d->maxCount;
}

/**
 * @brief 设置标签数量上限
 * @param count 上限值，-1 表示无限制
 * @details 若当前标签数超过新上限则截断并重建子控件，发射 tagsChanged
 */
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

/**
 * @brief 设置分隔符枚举列表
 * @param separators 分隔符枚举列表
 * @details 指定哪些字符/按键触发标签提交，Enter 由回车键触发不参与文本内分隔检测
 */
void DAChipsEdit::setSeparators(const QList<Separator> &separators)
{
    DA_D(d);
    d->separators = separators;
}

/**
 * @brief 获取当前分隔符枚举列表
 * @return 分隔符枚举列表
 */
QList<DAChipsEdit::Separator> DAChipsEdit::separators() const
{
    DA_DC(d);
    return d->separators;
}

/**
 * @brief 以字符串形式设置额外分隔符
 * @param chars 分隔符字符集合
 * @details 字符串中任意字符均可触发标签提交，补充枚举之外的运行时分隔符
 */
void DAChipsEdit::setSeparators(const QString &chars)
{
    DA_D(d);
    d->extraSeparators.clear();
    for (const QChar &c : chars)
        d->extraSeparators.insert(c);
}

/**
 * @brief 获取内嵌输入框当前文本
 * @return 输入框文本
 */
QString DAChipsEdit::inputText() const
{
    DA_DC(d);
    return d->input->text();
}

/**
 * @brief 清空内嵌输入框文本
 */
void DAChipsEdit::clearInput()
{
    DA_D(d);
    d->input->clear();
}

/**
 * @brief 计算单行高度
 * @return 输入框 sizeHint 高度 + layout 上下 margin
 * @details 供 DASelectComboBox 固定高度模式估算多行高度
 */
int DAChipsEdit::rowHeightHint() const
{
    DA_DC(d);
    const auto m = d->layout->contentsMargins();
    return d->input->sizeHint().height() + m.top() + m.bottom();
}

/**
 * @brief 计算推荐尺寸
 * @return 推荐的 QSize
 */
QSize DAChipsEdit::sizeHint() const
{
    return QFrame::sizeHint();
}

/**
 * @brief 计算最小推荐尺寸
 * @return 最小 QSize
 */
QSize DAChipsEdit::minimumSizeHint() const
{
    return QFrame::minimumSizeHint();
}

/**
 * @brief 事件过滤器，拦截内嵌输入框的焦点与按键事件
 * @param watched 被监听的对象
 * @param event 事件
 * @return 是否拦截事件
 * @details FocusIn/FocusOut 发射 inputFocusChanged；KeyPress 先发射 inputKeyEvent
 *          供外部拦截，未拦截时处理 Backspace 删末尾标签、Tab 提交、Escape 清空
 */
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

/**
 * @brief 获焦时将焦点转发到内嵌输入框
 * @param event 焦点事件
 */
void DAChipsEdit::focusInEvent(QFocusEvent *event)
{
    QFrame::focusInEvent(event);
    DA_D(d);
    d->input->setFocus();
}

/**
 * @brief 重建所有 DAChip 子控件
 * @details 先删除现有所有 chip，再按 d->tags 逐一创建。用于标签列表整体替换后刷新视图
 */
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

/**
 * @brief 创建并插入一个 DAChip 子控件
 * @param text 标签文本
 * @details 插入到布局末尾输入框之前，连接 closeRequested 信号到移除逻辑。
 *          需显式 show()：parent 可能已可见，未 show 的 QWidgetItem sizeHint 为 0
 */
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

/**
 * @brief 提交输入框文本为标签
 * @details 由回车键触发。空文本不操作，allowCreate 为 false 时不创建标签，
 *          否则清空输入框并调用 appendTag
 */
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

/**
 * @brief 输入框文本编辑时检测内嵌分隔符
 * @param text 当前输入框文本
 * @details 若文本包含分隔符字符则按分隔符拆分并逐个提交标签，清空输入框。
 *          allowCreate 为 false 时不处理
 */
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

/**
 * @brief 获取所有分隔符字符集合
 * @return 分隔符 QSet
 * @details 合并枚举分隔符（Comma/Semicolon/Space/Tab，Enter 除外）与额外字符串分隔符
 */
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

/**
 * @brief 构造分隔符正则表达式
 * @return 匹配任意分隔符字符的 QRegularExpression
 * @details 用于 split 拆分输入文本，正则字符集内特殊字符已转义
 */
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
