/**
 * @file DAWidgetsResources.cpp
 * @brief 集中注册 Qt RCC 图标资源
 * @details 静态库链接时，rcc 生成的目标文件如果没有任何符号被引用，会被链接器丢弃，
 *          导致运行时找不到 :/DAWidgetsIcons 下的图标。此前每个用到图标的控件构造函数
 *          各自调用 Q_INIT_RESOURCE(dawidgetsicons)；但 Q_INIT_RESOURCE 展开出的
 *          extern 声明写在 namespace DA 内时，GCC/Clang 会把它声明成
 *          DA::qInitResources_dawidgetsicons()，而 rcc 生成的是全局符号，静态链接
 *          （Linux CI、静态库使用者）会报 undefined reference。MSVC 恰好按全局符号
 *          处理，把这个问题掩盖了。
 *
 *          因此把 Q_INIT_RESOURCE 放进全局作用域的函数里，再由 namespace DA 的
 *          ensureResourcesInitialized() 调用；控件构造函数改为调用后者。
 */
#include "DAWidgetsGlobals.h"

#include <QtCore/QResource>

// 必须位于全局作用域：Q_INIT_RESOURCE 在这里声明的是全局函数
// qInitResources_dawidgetsicons()，与 rcc 生成的定义一致。
static void initDawidgetsIconsResource()
{
    Q_INIT_RESOURCE(dawidgetsicons);
}

namespace DA {

void ensureResourcesInitialized()
{
    // 函数内静态变量保证只注册一次（C++11 起线程安全）
    static const bool initialized = (initDawidgetsIconsResource(), true);
    Q_UNUSED(initialized)
}

}  // namespace DA
