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

    explicit DASelectComboBox(QWidget *parent = nullptr);
    ~DASelectComboBox();

    QStringList options() const;
    void setOptions(const QStringList &options);

    QStringList tags() const;
    void setTags(const QStringList &tags);
    void clear();

    bool allowCreate() const;
    void setAllowCreate(bool on);
    bool filterable() const;
    void setFilterable(bool on);

    QString placeholder() const;
    void setPlaceholder(const QString &text);

    HeightMode heightMode() const;
    void setHeightMode(HeightMode mode);
    int fixedRowCount() const;
    void setFixedRowCount(int rows);

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

Q_SIGNALS:
    void tagsChanged(const QStringList &tags);
    void optionSelected(const QString &text);
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
