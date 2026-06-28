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

    // 构造流式多标签输入文本框
    explicit DAChipsEdit(QWidget *parent = nullptr);
    // 析构，释放内部资源
    ~DAChipsEdit();

    // 获取当前所有标签文本列表
    QStringList tags() const;
    // 设置标签列表，替换当前所有标签
    void setTags(const QStringList &tags);

    // 追加一个标签到末尾
    void appendTag(const QString &text);
    // 在指定位置插入标签，索引越界返回 false
    bool insertTag(int index, const QString &text);
    // 移除第一个匹配文本的标签
    void removeTag(const QString &text);
    // 移除指定索引处的标签
    void removeAt(int index);
    // 清空所有标签
    void clear();

    // 获取占位提示文本
    QString placeholder() const;
    // 设置占位提示文本
    void setPlaceholder(const QString &text);

    // 是否允许添加非候选列表中的文本作为标签
    bool allowCreate() const;
    // 设置是否允许创建新标签
    void setAllowCreate(bool on);

    // 是否允许重复标签
    bool allowDuplicate() const;
    // 设置是否允许重复标签
    void setAllowDuplicate(bool on);

    // 获取标签数量上限，-1 表示无限制
    int maxCount() const;
    // 设置标签数量上限，-1 表示无限制
    void setMaxCount(int count);

    // 枚举重载（推荐，C++ 编译期）
    void setSeparators(const QList<Separator> &separators);
    // 获取当前分隔符枚举列表
    QList<Separator> separators() const;
    // 字符串重载：以给定字符集合中任意字符作为分隔符（不限枚举）
    void setSeparators(const QString &chars);

    // 内嵌输入框访问（供 DASelectComboBox 等组合控件使用）
    QString inputText() const;
    // 清空内嵌输入框文本
    void clearInput();

    /// 单行高度（输入框 sizeHint + layout 上下 margin），供固定高度模式估算
    int rowHeightHint() const;

    // 推荐尺寸，依据当前标签与布局计算
    QSize sizeHint() const override;
    // 最小推荐尺寸，保证输入框至少可显示
    QSize minimumSizeHint() const override;

Q_SIGNALS:
    /**
     * @brief 添加标签后发射
     * @param text 新增标签的文本
     */
    void tagAdded(const QString &text);
    /**
     * @brief 移除标签后发射
     * @param text 被移除标签的文本
     */
    void tagRemoved(const QString &text);
    /**
     * @brief 标签列表变化时发射
     * @param tags 当前的标签列表
     */
    void tagsChanged(const QStringList &tags);
    /**
     * @brief 内嵌输入框文本变化时发射
     * @param text 当前输入框文本
     */
    void inputTextChanged(const QString &text);
    /**
     * @brief 内嵌输入框焦点变化时发射
     * @param focused 是否获得焦点
     */
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
