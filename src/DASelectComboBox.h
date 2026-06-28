#ifndef DASELECTCOMBOBOX_H
#define DASELECTCOMBOBOX_H

#include <QComboBox>
#include <QStringList>
#include "DAWidgetsGlobals.h"
#include "DAWIDGETS_EXPORT.h"

class QListWidget;
class QListWidgetItem;
class QScrollArea;
class QWidget;

namespace DA
{
class DAChipsEdit;

/**
 * @brief 多标签选择下拉框，对标 Element UI el-select 的
 * multiple + filterable + allow-create + default-first-option 完整形态。
 *
 * 继承 QComboBox，保留原生箭头按钮、CC_ComboBox 样式边框与 showPopup 键盘
 * 助记键（F4 / Alt+Down）；编辑区替换为 DAChipsEdit 实现标签流式输入。下拉
 * 候选用自定义 QListWidget，支持输入实时过滤、方向键导航、回车选中
 * （default-first-option）；无匹配且 allowCreate 时回车创建。
 *
 * 高度策略 #heightMode：GrowWithTags（默认，随标签换行增高，el-select 风格）
 * 或 FixedHeightScrollable（固定行数，超出滚动）。
 */
class DAWIDGETS_EXPORT DASelectComboBox : public QComboBox
{
    Q_OBJECT
    Q_PROPERTY(QStringList options READ options WRITE setOptions)
    Q_PROPERTY(QStringList tags READ tags WRITE setTags NOTIFY tagsChanged)
    Q_PROPERTY(bool allowCreate READ allowCreate WRITE setAllowCreate)
    Q_PROPERTY(bool filterable READ filterable WRITE setFilterable)
    Q_PROPERTY(QString placeholder READ placeholder WRITE setPlaceholder)
    Q_PROPERTY(HeightMode heightMode READ heightMode WRITE setHeightMode)
    Q_PROPERTY(int fixedRowCount READ fixedRowCount WRITE setFixedRowCount)
    DA_DECLARE_PRIVATE(DASelectComboBox)
public:
    /// 高度策略
    enum class HeightMode {
        GrowWithTags,          ///< 随标签换行自动增高（el-select multiple 风格）
        FixedHeightScrollable  ///< 固定显示 fixedRowCount 行，超出在编辑区内滚动
    };
    Q_ENUM(HeightMode)

    // 构造多标签选择下拉框
    explicit DASelectComboBox(QWidget *parent = nullptr);
    // 析构，释放内部资源
    ~DASelectComboBox();

    // 获取候选选项列表
    QStringList options() const;
    // 设置候选选项列表，替换当前所有选项
    void setOptions(const QStringList &options);

    // 获取已选标签列表
    QStringList tags() const;
    // 设置已选标签列表，替换当前所有标签
    void setTags(const QStringList &tags);
    // 清空所有已选标签与候选选项
    void clear();

    // 是否允许在无匹配时创建新标签
    bool allowCreate() const;
    // 设置是否允许创建新标签
    void setAllowCreate(bool on);
    // 下拉列表是否支持输入过滤
    bool filterable() const;
    // 设置下拉列表是否支持输入过滤
    void setFilterable(bool on);

    // 获取输入框占位提示文本
    QString placeholder() const;
    // 设置输入框占位提示文本
    void setPlaceholder(const QString &text);

    // 获取当前高度策略
    HeightMode heightMode() const;
    // 设置高度策略（随标签增高或固定行数滚动）
    void setHeightMode(HeightMode mode);
    // 获取固定高度模式下的显示行数
    int fixedRowCount() const;
    // 设置固定高度模式下的显示行数
    void setFixedRowCount(int rows);

    // 推荐尺寸，依据当前标签数量与高度策略计算
    QSize sizeHint() const override;
    // 最小推荐尺寸，保证至少一行可显示
    QSize minimumSizeHint() const override;

Q_SIGNALS:
    /**
     * @brief 已选标签列表变化时发射
     * @param tags 当前的标签列表
     */
    void tagsChanged(const QStringList &tags);
    /**
     * @brief 从候选列表选中某项时发射
     * @param text 被选中项的文本
     */
    void optionSelected(const QString &text);
    /**
     * @brief 用户创建新标签时发射
     * @param text 新创建标签的文本
     */
    void created(const QString &text);

protected:
    void resizeEvent(QResizeEvent *event) override;
    void showPopup() override;
    bool eventFilter(QObject *watched, QEvent *event) override;

private:
    void applyHeightMode();
    void applyDefaultStyle();
    void showCustomPopup();
    void hideCustomPopup();
    void refreshPopup(const QString &filter);
    void pickCurrent();
    void onInputTextChanged(const QString &text);
    void onInputFocusChanged(bool focused);
    void onInputKey(QKeyEvent *event, bool *handled);
    void onPopupItemClicked(QListWidgetItem *item);
};
}  // namespace DA

#endif // DASELECTCOMBOBOX_H
