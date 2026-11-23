// DAWidgets 单文件合并模板 —— 公共头文件清单
//
// 本文件只被合并工具读取，不要直接编译。Amalgamate 会把下面列出的所有头文件
// 依次内联进 src-amalgamate/DAWidgets.h；头文件之间通过裸文件名互相包含
// （例如 "DAWidgetsGlobals.h"），合并工具会递归展开并自动去重，因此这里的
// 顺序只影响输出排版，不影响编译结果。
//
// 新增公开类时：把新的 .h 加到这里，把对应的 .cpp 加到
// DAWidgetsAmalgamTemplate.cpp，然后重新运行 tools/Amalgamate.ps1。

#include "DAWidgetsGlobals.h"
#include "DAQt5Qt6Compat.h"

// ---- Selector 选择器家族 ----
#include "DAFlowLayout.h"
#include "DAChip.h"
#include "DAChipsEdit.h"
#include "DASelectComboBox.h"

// ---- Collapse 折叠面板家族 ----
#include "DACollapseLayout.h"
#include "DACollapseItem.h"
#include "DACollapseWidget.h"

// ---- 光标 / 遮罩 / 撤销历史等工具组件 ----
#include "DACursorScoped.h"
#include "DAWaitCursorScoped.h"
#include "DAAbstractWidgetOverlay.h"
#include "DAUndoHistoryWidget.h"

// ---- Property Editing 属性编辑家族 ----
#include "DAPropertyItemWidget.h"
#include "DATimeFormatEditWidget.h"
#include "DAAligmentEditWidget.h"
#include "DAAligmentPositionEditWidget.h"
#include "DATextAlignmentEditWidget.h"
#include "DAFilePathEditWidget.h"
#include "DATextEditWidget.h"
#include "DABrushStyleComboBox.h"
#include "DAPenStyleComboBox.h"
#include "DAColorGridWidget.h"
#include "DAColorPaletteGridWidget.h"
#include "DAColorMenu.h"
#include "DAColorPickerButton.h"
#include "DAPenEditWidget.h"
#include "DABrushEditWidget.h"
#include "DAFontEditPannelWidget.h"
#include "DAShapeEditPannelWidget.h"

// ---- Item Selection & Preview 条目选择与预览家族 ----
#include "DAItemListEditWidget.h"
#include "DAItemListPickDialog.h"
#include "DACheckListWidget.h"
#include "DAStringListEditWidget.h"
#include "DAColorScalePreviewWidget.h"
#include "DAStringFilterProxyModel.h"
