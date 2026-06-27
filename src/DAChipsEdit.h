#ifndef DACHIPSEDIT_H
#define DACHIPSEDIT_H

#include <QFrame>
#include <QList>
#include <QSet>
#include <QStringList>
#include "DAWidgetsGlobals.h"
#include "DAWIDGETS_EXPORT.h"

class QLineEdit;
class QKeyEvent;
class QRegularExpression;

namespace DA
{
class DAChip;
class DAFlowLayout;

/**
 * @brief 流式多标签输入文本框。
 *
 * 行为对标 Element UI el-select 的 multiple + allow-create 输入态：
 * 输入文本后按回车或分隔符提交为一个标签（DAChip），Backspace 在空输入时
 * 删除末尾标签，每个标签可点击 × 关闭。支持去重、限量、自定义分隔符。
 *
 * 分隔符同时提供枚举重载（编译期类型安全）与字符串重载（运行时/UI 配置）。
 *
 * 扩展点（供 DASelectComboBox 等组合控件使用）：inputText()/clearInput()
 * 读写内嵌输入框文本，inputTextChanged/inputFocusChanged/inputKeyEvent
 * 信号暴露输入态与按键事件，便于外部拦截回车/方向键实现下拉候选逻辑。
 */
class DAWIDGETS_EXPORT DAChipsEdit : public QFrame
{
    Q_OBJECT
    Q_PROPERTY(QStringList tags READ tags WRITE setTags NOTIFY tagsChanged)
    Q_PROPERTY(QString placeholder READ placeholder WRITE setPlaceholder)
    Q_PROPERTY(bool allowCreate READ allowCreate WRITE setAllowCreate)
    Q_PROPERTY(bool allowDuplicate READ allowDuplicate WRITE setAllowDuplicate)
    Q_PROPERTY(int maxCount READ maxCount WRITE setMaxCount)
    DA_DECLARE_PRIVATE(DAChipsEdit)
public:
    /// 提交标签的触发字符。Enter 由回车键触发，不参与文本内分隔检测。
    enum class Separator {
        Comma,     ///< ','
        Semicolon, ///< ';'
        Space,     ///< ' '
        Enter,     ///< 回车键
        Tab        ///< '\t'（按 Tab 提交，阻止焦点跳转）
    };
    Q_ENUM(Separator)

    explicit DAChipsEdit(QWidget *parent = nullptr);
    ~DAChipsEdit();

    QStringList tags() const;
    void setTags(const QStringList &tags);

    void appendTag(const QString &text);
    bool insertTag(int index, const QString &text);
    void removeTag(const QString &text);
    void removeAt(int index);
    void clear();

    QString placeholder() const;
    void setPlaceholder(const QString &text);

    bool allowCreate() const;
    void setAllowCreate(bool on);

    bool allowDuplicate() const;
    void setAllowDuplicate(bool on);

    int maxCount() const;
    void setMaxCount(int count);

    // 枚举重载（推荐，C++ 编译期）
    void setSeparators(const QList<Separator> &separators);
    QList<Separator> separators() const;
    // 字符串重载：以给定字符集合中任意字符作为分隔符（不限枚举）
    void setSeparators(const QString &chars);

    // 内嵌输入框访问（供 DASelectComboBox 等组合控件使用）
    QString inputText() const;
    void clearInput();

    /// 单行高度（输入框 sizeHint + layout 上下 margin），供固定高度模式估算
    int rowHeightHint() const;

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

Q_SIGNALS:
    void tagAdded(const QString &text);
    void tagRemoved(const QString &text);
    void tagsChanged(const QStringList &tags);
    void inputTextChanged(const QString &text);
    void inputFocusChanged(bool focused);
    /// 按键事件拦截：外部可将 *handled 置 true 阻止 DAChipsEdit 默认处理。
    void inputKeyEvent(QKeyEvent *event, bool *handled);

protected:
    bool eventFilter(QObject *watched, QEvent *event) override;
    void focusInEvent(QFocusEvent *event) override;

private:
    void rebuildChips();
    void addChipWidget(const QString &text);
    void commitInput();
    void onInputEdited(const QString &text);
    QSet<QChar> separatorChars() const;
    QRegularExpression separatorRegex() const;
};
}  // namespace DA

#endif // DACHIPSEDIT_H
