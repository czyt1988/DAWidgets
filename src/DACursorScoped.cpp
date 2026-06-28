#include "DACursorScoped.h"
#include <QApplication>
namespace DA
{

/**
 * @brief 传入图标并全局设置为当前图标
 * @param cur
 */
DACursorScoped::DACursorScoped(const QCursor& cur)
{
    setCursor(cur);
}

/**
 * @brief 析构函数
 * @details 通过调用 release() 恢复覆盖光标，利用 RAII 保证异常安全路径下光标也能恢复
 */
DACursorScoped::~DACursorScoped()
{
    release();
}

/**
 * @brief 设置覆盖光标
 * @param cur 要设置的光标
 * @details 调用 QApplication::setOverrideCursor 压入光标栈，原光标被覆盖
 */
void DACursorScoped::setCursor(const QCursor& cur)
{
    QApplication::setOverrideCursor(cur);
}

/**
 * @brief 提前恢复图标
 */
void DACursorScoped::release()
{
    QApplication::restoreOverrideCursor();
}
}  // end namespace DA
