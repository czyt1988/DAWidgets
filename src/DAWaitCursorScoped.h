#ifndef DAWAITCURSORSCOPED_H
#define DAWAITCURSORSCOPED_H
#include <QCursor>
#include "DAWidgetsGlobals.h"
#include "DAWIDGETS_EXPORT.h"
#include "DACursorScoped.h"
namespace DA
{
/**
 * @brief 一个等待鼠标的保护类
 */
class DAWIDGETS_EXPORT DAWaitCursorScoped : public DACursorScoped
{
public:
	DAWaitCursorScoped();
	virtual ~DAWaitCursorScoped();
};
}  // end namespace DA

#ifndef DA_WAIT_CURSOR_SCOPED
#define DA_WAIT_CURSOR_SCOPED()                                                                                        \
	DA::DAWaitCursorScoped __da__wait__cursor;                                                                         \
	Q_UNUSED(__da__wait__cursor)
#endif

#ifndef DA_WAIT_CURSOR_SCOPED_NS
#define DA_WAIT_CURSOR_SCOPED_NS()                                                                                     \
	DA::DAWaitCursorScoped __da__wait__cursor;                                                                         \
	Q_UNUSED(__da__wait__cursor)
#endif
#endif  // DAWAITCURSORSCOPED_H
