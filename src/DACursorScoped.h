#ifndef DACURSORSCOPED_H
#define DACURSORSCOPED_H
#include <QCursor>
#include "DAWidgetsGlobals.h"
#include "DAWIDGETS_EXPORT.h"
namespace DA
{
/**
 * @brief 用于图标保护，防止异常导致图标无法恢复
 */
class DAWIDGETS_EXPORT DACursorScoped
{
public:
	DACursorScoped(const QCursor& cur);
	virtual ~DACursorScoped();
	void setCursor(const QCursor& cur);
	void release();
};
}  // end namespace DA

#endif  // DACURSORSCOPED_H
