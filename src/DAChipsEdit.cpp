#include "DAChipsEdit.h"

#include "DAChip.h"
#include "DAFlowLayout.h"

#include <QClipboard>
#include <QColor>
#include <QFocusEvent>
#include <QGuiApplication>
#include <QInputMethodEvent>
#include <QKeyEvent>
#include <QKeySequence>
#include <QLabel>
#include <QLineEdit>
#include <QPalette>
#include <QRegularExpression>
#include <QResizeEvent>
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
    bool pasteSplitEnabled { false };
    bool imeComposing { false };
    int tagElideWidth { 0 };
    bool showCount { false };
    QLabel *countLabel { nullptr };
    // 打开计数前 layout 的底部 margin，关闭时原样恢复
    int countMarginBackup { -1 };
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
    // 计数标签文本由标签列表统一驱动，覆盖 append/insert/remove/clear/setTags/截断
    connect(this, &DAChipsEdit::tagsChanged, this, [this] { updateCountLabel(); });
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
 * @brief 打开/关闭粘贴批量拆分
 * @param on 是否开启（默认 false）
 * @details 关闭时不拦截任何按键、不读剪贴板，行为与旧版本一致
 */
void DAChipsEdit::setPasteSplitEnabled(bool on)
{
    DA_D(d);
    d->pasteSplitEnabled = on;
}

/**
 * @brief 粘贴批量拆分是否开启
 * @return 开启返回 true
 */
bool DAChipsEdit::isPasteSplitEnabled() const
{
    DA_DC(d);
    return d->pasteSplitEnabled;
}

/**
 * @brief 粘贴的唯一入口
 * @param text 待粘贴文本
 * @details 开关关闭或 allowCreate 为 false 时退化为普通插入输入框；
 *          开启时按分隔符拆成 token，先发 pasteSplit，再逐个追加为标签。
 *          快捷键、单测与 demo 的"模拟粘贴"共用本函数。
 */
void DAChipsEdit::pasteText(const QString &text)
{
    DA_D(d);
    if (text.trimmed().isEmpty())
        return;
    if (!d->allowCreate || !d->pasteSplitEnabled) {
        d->input->insert(text);
        return;
    }
    const QStringList tokens = splitForPaste(text);
    if (tokens.isEmpty())
        return;
    Q_EMIT pasteSplit(tokens);
    applyPastedTokens(tokens);
}

/**
 * @brief 设置 chip 最大宽度
 * @param px 最大宽度（像素），0 表示不省略（默认）
 */
void DAChipsEdit::setTagElideWidth(int px)
{
    DA_D(d);
    px = qMax(0, px);
    d->tagElideWidth = px;
    for (DAChip *chip : d->chips)
        chip->setElideWidth(px);
    d->layout->invalidate();
}

/**
 * @brief 获取 chip 最大宽度
 * @return 最大宽度（像素），0 表示不省略
 */
int DAChipsEdit::tagElideWidth() const
{
    DA_DC(d);
    return d->tagElideWidth;
}

/**
 * @brief 显示/隐藏右下角数量计数
 * @param on 是否显示（默认 false）
 * @details 开启时创建私有 QLabel 并预留底部 margin，标签不进 DAFlowLayout，
 *          因此不会夺走末尾输入框的自动伸展；关闭时删除标签并恢复原 margin。
 */
void DAChipsEdit::setShowCount(bool on)
{
    DA_D(d);
    if (d->showCount == on)
        return;
    d->showCount = on;
    QMargins m = d->layout->contentsMargins();
    if (on) {
        if (!d->countLabel) {
            d->countLabel = new QLabel(this);
            d->countLabel->setObjectName(QStringLiteral("DAChipsEditCount"));
            d->countLabel->setFocusPolicy(Qt::NoFocus);
            d->countLabel->setAttribute(Qt::WA_TransparentForMouseEvents);
            QFont f = d->countLabel->font();
            f.setPixelSize(11);
            d->countLabel->setFont(f);
            QPalette pal = d->countLabel->palette();
            pal.setColor(QPalette::WindowText, QColor(0xA8, 0xAB, 0xB2));
            d->countLabel->setPalette(pal);
        }
        // 记下调用方当前 margin，关闭时原样恢复，不写死 4
        d->countMarginBackup = m.bottom();
        m.setBottom(m.bottom() + d->countLabel->sizeHint().height() + 2);
        d->layout->setContentsMargins(m);
        updateCountLabel();
        d->countLabel->show();
        d->countLabel->raise();
    } else {
        if (d->countLabel) {
            d->countLabel->hide();
            d->countLabel->deleteLater();
            d->countLabel = nullptr;
        }
        if (d->countMarginBackup >= 0) {
            m.setBottom(d->countMarginBackup);
            d->layout->setContentsMargins(m);
            d->countMarginBackup = -1;
        }
    }
}

/**
 * @brief 是否显示右下角数量计数
 * @return 显示返回 true
 */
bool DAChipsEdit::showCount() const
{
    DA_DC(d);
    return d->showCount;
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
    // 未触发截断时没有 tagsChanged，计数标签仍要跟着换分母
    updateCountLabel();
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
 * @details FocusIn/FocusOut 发射 inputFocusChanged；KeyPress 先做 IME 组字与粘贴
 *          拦截，再发射 inputKeyEvent 供外部拦截，未拦截时处理 Backspace 删末尾标签、
 *          Tab 提交、Escape 清空
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
        } else if (t == QEvent::InputMethod) {
            // 只观察不消费：组字中（preedit 非空且无 commit）与组字结束的判定
            auto *ime = static_cast< QInputMethodEvent * >(event);
            if (!ime->commitString().isEmpty() || ime->replacementLength() > 0)
                d->imeComposing = false;
            else if (!ime->preeditString().isEmpty())
                d->imeComposing = true;
        } else if (t == QEvent::KeyPress) {
            auto *ke = static_cast< QKeyEvent * >(event);
            const int key = ke->key();
            // IME 守卫必须在 inputKeyEvent 之前：否则 DASelectComboBox 已经选中候选
            if (d->imeComposing && (key == Qt::Key_Return || key == Qt::Key_Enter))
                return true;
            // 粘贴拦截同样先于 inputKeyEvent，避免外部先消费 Ctrl+V
            if (d->pasteSplitEnabled && ke->matches(QKeySequence::Paste)) {
                const QString clip = QGuiApplication::clipboard()->text();
                if (clip.isEmpty())
                    Q_EMIT pasteRejected(QStringLiteral("clipboard"));
                else
                    pasteText(clip);
                return true;
            }
            bool handled = false;
            Q_EMIT inputKeyEvent(ke, &handled);
            if (handled)
                return true;
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
 * @brief 尺寸变化时重新定位右下角计数标签
 * @param event 尺寸事件
 * @details 只做 setGeometry，不改 margin，避免与流式布局互相触发
 */
void DAChipsEdit::resizeEvent(QResizeEvent *event)
{
    QFrame::resizeEvent(event);
    positionCountLabel();
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
    chip->setElideWidth(d->tagElideWidth);
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
    // 第二道闸：returnPressed 可能绕过事件过滤器
    if (d->imeComposing)
        return;
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

/**
 * @brief 按分隔符拆分待粘贴文本
 * @param text 原始粘贴文本
 * @return 已 trim、已丢弃空串的 token 列表
 * @details 换行先归一为 \n；仅当分隔符集合含 Enter 时把 \n 当作分隔符，
 *          避免 DASelectComboBox（无分隔符）里粘贴一行文字被拆成多个候选过滤词。
 *          上界：分隔符集合为空时整段作为一个 token。
 */
QStringList DAChipsEdit::splitForPaste(const QString &text) const
{
    DA_DC(d);
    QString normalized = text;
    normalized.replace(QStringLiteral("\r\n"), QStringLiteral("\n"));
    normalized.replace(QLatin1Char('\r'), QLatin1Char('\n'));

    // 字符集来自 QSet，天然去重；空集合不能构造 "[]" 这种非法正则
    QString chars;
    for (const QChar &c : separatorChars())
        chars += c;
    if (d->separators.contains(Separator::Enter))
        chars += QLatin1Char('\n');
    if (chars.isEmpty()) {
        const QString whole = normalized.trimmed();
        return whole.isEmpty() ? QStringList() : QStringList { whole };
    }

    QString pattern = QStringLiteral("[");
    for (const QChar &c : chars) {
        if (c == QLatin1Char(']') || c == QLatin1Char('\\') || c == QLatin1Char('^')
            || c == QLatin1Char('-') || c == QLatin1Char('['))
            pattern += QLatin1Char('\\');
        pattern += c;
    }
    pattern += QStringLiteral("]");

#if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
    const QStringList parts = normalized.split(QRegularExpression(pattern), Qt::SkipEmptyParts);
#else
    const QStringList parts = normalized.split(QRegularExpression(pattern), QString::SkipEmptyParts);
#endif
    QStringList out;
    out.reserve(parts.size());
    for (const QString &p : parts) {
        const QString s = p.trimmed();
        if (!s.isEmpty())
            out << s;
    }
    return out;
}

/**
 * @brief 把粘贴拆出的 token 逐个追加为标签
 * @param tokens splitForPaste() 的输出
 * @details 去重与 maxCount 仍由 appendTag() 判定；只有被 maxCount 挡住时才发
 *          pasteRejected("maxCount")，重复项按既有语义静默丢弃。
 */
void DAChipsEdit::applyPastedTokens(const QStringList &tokens)
{
    DA_D(d);
    bool hitMaxCount = false;
    for (const QString &t : tokens) {
        if (d->maxCount >= 0 && d->tags.size() >= d->maxCount) {
            hitMaxCount = true;
            break;
        }
        appendTag(t);
    }
    if (hitMaxCount)
        Q_EMIT pasteRejected(QStringLiteral("maxCount"));
}

/**
 * @brief 刷新右下角计数标签文本与位置
 * @details maxCount >= 0 显示 "n / max"，否则只显示 n
 */
void DAChipsEdit::updateCountLabel()
{
    DA_D(d);
    if (!d->countLabel)
        return;
    const int n = d->tags.size();
    d->countLabel->setText(d->maxCount >= 0
                               ? QStringLiteral("%1 / %2").arg(n).arg(d->maxCount)
                               : QString::number(n));
    positionCountLabel();
}

/**
 * @brief 把计数标签定位到右下角
 * @details 标签不进 DAFlowLayout，否则会抢走末尾输入框的伸展
 */
void DAChipsEdit::positionCountLabel()
{
    DA_D(d);
    if (!d->countLabel)
        return;
    const QSize hint = d->countLabel->sizeHint();
    d->countLabel->setGeometry(width() - 4 - hint.width(), height() - 4 - hint.height(),
                               hint.width(), hint.height());
}
}  // namespace DA
