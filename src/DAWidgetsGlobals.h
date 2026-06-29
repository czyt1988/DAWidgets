#ifndef DAWIDGETSGLOBALS_H
#define DAWIDGETSGLOBALS_H
#include <memory>

// ---- 符号导出/导入宏 ----
// 不依赖 CMake GenerateExportHeader,手动实现,便于 qmake 等其他构建系统集成。
//
// 三种场景:
//   1. 静态构建 (DA_BUILT_AS_STATIC): 导出宏为空
//   2. 共享构建-编译库 (DAWIDGETS_EXPORTS): 符号导出
//   3. 共享构建-使用者侧: 符号导入
#if defined(DA_BUILT_AS_STATIC)
#define DAWIDGETS_EXPORT
#elif defined(DAWIDGETS_EXPORTS)
#if defined(_MSC_VER)
#define DAWIDGETS_EXPORT __declspec(dllexport)
#elif defined(__GNUC__) || defined(__clang__)
#define DAWIDGETS_EXPORT __attribute__((visibility("default")))
#else
#define DAWIDGETS_EXPORT
#endif
#else
#if defined(_MSC_VER)
#define DAWIDGETS_EXPORT __declspec(dllimport)
#elif defined(__GNUC__) || defined(__clang__)
#define DAWIDGETS_EXPORT
#else
#define DAWIDGETS_EXPORT
#endif
#endif

/**
 * @def DA_DECLARE_PRIVATE
 * @brief 模仿Q_DECLARE_PRIVATE，但不用前置声明而是作为一个内部类
 *
 * 例如:
 *
 * @code
 * //header
 * class A
 * {
 *  DA_DECLARE_PRIVATE(A)
 * };
 * @endcode
 *
 * 其展开效果为：
 *
 * @code
 * class A{
 *  class PrivateData;
 *  friend class A::PrivateData;
 *  std::unique_ptr< PrivateData > d_ptr;
 * }
 * @endcode
 *
 * 这样前置声明了一个内部类PrivateData，在cpp文件中建立这个内部类的实现
 *
 * @code
 * //cpp
 * class A::PrivateData{
 *  DA_DECLARE_PUBLIC(A)
 *  PrivateData(A* p):q_ptr(p){
 *  }
 * };
 *
 * A::A():d_ptr(new PrivateData(this)){
 * }
 * @endcode
 *
 */
#ifndef DA_DECLARE_PRIVATE
#define DA_DECLARE_PRIVATE(classname)                                                                                  \
    class PrivateData;                                                                                                 \
    friend class classname::PrivateData;                                                                               \
    std::unique_ptr< PrivateData > d_ptr;                                                                              \
    inline PrivateData* d_func()                                                                                       \
    {                                                                                                                  \
        return (d_ptr.get());                                                                                          \
    }                                                                                                                  \
    inline const PrivateData* d_func() const                                                                           \
    {                                                                                                                  \
        return (d_ptr.get());                                                                                          \
    }
#endif

/**
 * @def DA_DECLARE_PUBLIC
 * @brief 模仿Q_DECLARE_PUBLIC
 *
 * 配套DA_DECLARE_PRIVATE使用
 */
#ifndef DA_DECLARE_PUBLIC
#define DA_DECLARE_PUBLIC(classname)                                                                                   \
    friend class classname;                                                                                            \
    classname* q_ptr { nullptr };                                                                                      \
    inline classname* q_func()                                                                                         \
    {                                                                                                                  \
        return (static_cast< classname* >(q_ptr));                                                                     \
    }                                                                                                                  \
    inline const classname* q_func() const                                                                             \
    {                                                                                                                  \
        return (static_cast< const classname* >(q_ptr));                                                               \
    }
#endif

/**
 * @def  DA_PIMPL_CONSTRUCT
 *
 * 配套DA_DECLARE_PRIVATE使用,在构造函数中构建privatedata
 */
#ifndef DA_PIMPL_CONSTRUCT
#define DA_PIMPL_CONSTRUCT d_ptr(std::make_unique< PrivateData >(this))
#endif

/**
 *@def DA_D
 *@brief impl获取指针，参考Q_D
 */
#ifndef DA_D
#define DA_D(pointerName) PrivateData* pointerName = d_func()
#endif

/**
 *@def DA_DC
 *@brief impl获取指针，参考Q_D
 */
#ifndef DA_DC
#define DA_DC(pointerName) const PrivateData* pointerName = d_func()
#endif

/**
 *@def DA_Q
 *@brief impl获取指针，参考Q_Q
 */
#ifndef DA_Q
#define DA_Q(classname, pointerName) classname* pointerName = q_func()
#endif

/**
 *@def DA_QC
 *@brief impl获取指针，参考Q_Q
 */
#ifndef DA_QC
#define DA_QC(classname, pointerName) const classname* pointerName = q_func()
#endif

#endif  // DAWIDGETSGLOBALS_H
