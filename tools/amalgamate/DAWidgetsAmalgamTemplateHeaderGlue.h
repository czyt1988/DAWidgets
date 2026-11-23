// This file provides an extra level of indirection for the @remap in the template.
// 合并 cpp 时，模板中的 @remap 会把下面的公共头文件清单替换为已生成的
// DAWidgets.h，避免把所有头文件内容在 .cpp 里再内联一份。
#include "DAWidgetsAmalgamTemplatePublicHeaders.h"
