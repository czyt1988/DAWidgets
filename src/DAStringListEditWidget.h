#ifndef DASTRINGLISTEDITWIDGET_H
#define DASTRINGLISTEDITWIDGET_H

#include <QString>
#include <QStringList>
#include <QWidget>
#include "DAWidgetsGlobals.h"

namespace DA
{

/**
 * @brief 纯字符串列表编辑器（可自由输入、可编辑、可排序）
 *
 * 面向 QStringList 类数据（表单 list 字段、模块搜索路径等）：
 * - Inline 模式：Add 追加一行空字符串并立即进入编辑，不插入占位文本；
 * - External 模式：Add 只发 addRequested()，由调用方决定加什么（如目录选择），
 *   再调用 appendItem() 落入列表；
 * - 可选的 Remove / Up / Down 按钮，排序用按钮而不是拖拽。
 *
 * 空字符串允许存在（调用方可自行校验）；setItems 整体替换不发 itemsChanged。
 */
class DAWIDGETS_EXPORT DAStringListEditWidget : public QWidget
{
    Q_OBJECT
    DA_DECLARE_PRIVATE(DAStringListEditWidget)
public:
    /// Add 按钮行为
    enum class AddMode
    {
        Inline,   ///< 追加可编辑空行
        External  ///< 只发 addRequested()，由调用方追加
    };

    explicit DAStringListEditWidget(QWidget* parent = nullptr);
    ~DAStringListEditWidget();

    // 整体替换（不发 itemsChanged）
    void setItems(const QStringList& items);
    // 获取当前列表
    QStringList items() const;
    // 追加一条（External 模式下由调用方在 addRequested 后使用；发 itemsChanged）
    void appendItem(const QString& text);

    // Add 按钮行为（默认 Inline）
    void setAddMode(AddMode m);
    AddMode addMode() const;

    // Up/Down 按钮显隐
    void setReorderEnabled(bool on);
    bool isReorderEnabled() const;

    // Remove 按钮显隐
    void setRemoveEnabled(bool on);
    bool isRemoveEnabled() const;

    // 是否允许编辑条目文本
    void setEditable(bool on);
    bool isEditable() const;

    // 数量上限（<=0 表示不限）
    void setMaxCount(int n);
    int maxCount() const;

    // 空列表时的提示文本
    void setPlaceholderText(const QString& t);
    QString placeholderText() const;

Q_SIGNALS:
    /**
     * @brief 列表内容变化（增/删/排序/编辑，appendItem 也发；setItems 不发）
     */
    void itemsChanged();
    /**
     * @brief External 模式下点击 Add
     */
    void addRequested();

protected:
    bool eventFilter(QObject* obj, QEvent* e) override;

private:
    Q_DISABLE_COPY(DAStringListEditWidget)
};
}  // namespace DA

#endif  // DASTRINGLISTEDITWIDGET_H
