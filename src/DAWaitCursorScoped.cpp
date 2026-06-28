#include "DAWaitCursorScoped.h"
#include <QApplication>

namespace DA
{

/**
 * @brief 构造等待光标守卫
 * @details 以 Qt::WaitCursor 作为参数委托给基类 DACursorScoped，构造时即显示等待光标
 */
DAWaitCursorScoped::DAWaitCursorScoped() : DACursorScoped(QCursor(Qt::WaitCursor))
{
}

/**
 * @brief 析构函数
 * @details 恢复覆盖光标，用于在离开作用域时还原鼠标指针
 */
DAWaitCursorScoped::~DAWaitCursorScoped()
{
    QApplication::restoreOverrideCursor();
}
}  // end namespace DA
