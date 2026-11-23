# DAStringFilterProxyModel 文本过滤代理

`DAStringFilterProxyModel` 是面向列表/树的通用文本过滤代理：按 `Qt::DisplayRole`（可改为其它角色）匹配，默认大小写不敏感，可限定列；树形模型开启「子节点命中放行父节点」后，父节点不会因为自身不匹配而被过滤掉。

## 效果预览

![DAStringFilterProxyModel：树形模型的子节点过滤，父节点自动放行](../images/gallery/widgets/filter-tree.png)

上图中的树有三组节点，输入过滤文本（例如 `ban`、`胡萝`、`wrench`）后，只有命中的叶子节点及其父节点保留；清空输入即恢复全部。演示程序在下方实时显示过滤后顶层项数与源模型顶层项数。

## 核心特性

**特性**

- ✅ **按展示文本过滤**：默认 `Qt::DisplayRole`，可用 `setFilterRole()` 改为自定义角色
- ✅ **大小写策略**：`setFilterCaseSensitivity()`，默认不敏感
- ✅ **单列/所有列**：`setFilterColumn(-1)` 表示所有列参与匹配
- ✅ **树形父节点放行**：`setIncludeParentsOfMatches(true)` 保留命中子节点的父链
- ✅ **可复用匹配函数**：静态 `matchesText()` 供未挂代理的视图复用同一套语义

## 用法

```cpp
#include "DAStringFilterProxyModel.h"
#include <QStandardItemModel>
#include <QTreeView>

auto *model = new QStandardItemModel(parent);
auto *fruit = new QStandardItem("水果");
fruit->appendRow(new QStandardItem("苹果 Apple"));
fruit->appendRow(new QStandardItem("香蕉 Banana"));
model->appendRow(fruit);

auto *proxy = new DA::DAStringFilterProxyModel(parent);
proxy->setSourceModel(model);
proxy->setIncludeParentsOfMatches(true);   // 树形模型必须开启，否则父节点被过滤
proxy->setFilterCaseSensitivity(Qt::CaseInsensitive);

auto *view = new QTreeView(parent);
view->setModel(proxy);
view->setHeaderHidden(true);
view->expandAll();

// 输入框变化时更新过滤文本
QObject::connect(searchEdit, &QLineEdit::textChanged,
                 proxy, &DA::DAStringFilterProxyModel::setFilterText);
```

!!! warning "树形模型记得开启 includeParentsOfMatches"
    代理默认只检查每一行自身；如果父节点文本不包含过滤词，父节点会被隐藏，子节点也就无法显示。树形数据请设置 `setIncludeParentsOfMatches(true)`。

## 核心方法

| 方法 | 说明 |
|------|------|
| `setFilterText(QString)` / `filterText()` | 过滤文本（空串 = 不过滤） |
| `setFilterCaseSensitivity(Qt::CaseSensitivity)` | 大小写策略 |
| `setFilterColumn(int)` | 参与匹配的列（`-1` = 所有列） |
| `setFilterRole(int)` | 匹配使用的数据角色（默认 `Qt::DisplayRole`） |
| `setIncludeParentsOfMatches(bool)` | 子节点命中时放行父节点 |
| `matchesText(text, filter, cs)` | 静态匹配函数，语义与代理一致 |

## 参考

- [DACheckListWidget 勾选列表](item-selection.md)
- 示例路径：`examples/all_widgets_demo`（过滤 tab）
