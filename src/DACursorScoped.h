#ifndef DACURSORSCOPED_H
#define DACURSORSCOPED_H
#include <QCursor>
#include "DAWidgetsGlobals.h"
namespace DA
{
/**
 * @brief 用于图标保护，防止异常导致图标无法恢复
 */
class DAWIDGETS_EXPORT DACursorScoped
{
public:
	// 构造光标守卫，构造时立即设置指定光标
	DACursorScoped(const QCursor& cur);
	// 析构时自动恢复原光标，保证异常路径下也能恢复
	virtual ~DACursorScoped();
	// 设置新的覆盖光标，叠加到应用级光标栈
	void setCursor(const QCursor& cur);
	// 手动释放，提前恢复光标
	void release();
};
}  // end namespace DA

#endif  // DACURSORSCOPED_H
