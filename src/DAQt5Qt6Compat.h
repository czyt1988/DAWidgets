#ifndef DAQT5QT6COMPAT_H
#define DAQT5QT6COMPAT_H

//
// 本头文件提供 Qt5/Qt6 差异兼容宏。
//
// 维护契约:
//   此处的宏定义必须与 data-workbench 项目的 src/DAGlobals.h 中对应宏
//   逐字一致。两侧均以 #ifndef 守卫,任一翻译单元先 include 谁都安全
//   (first-include-wins)。不可独立修改本文件中的宏定义——若需变更,
//   需同步更新 data-workbench 的 DAGlobals.h。
//
// 引入策略:
//   默认仅在 .cpp 文件中按需 include,避免通过公共头传递,从而
//   data-workbench 引入 DAWidgets 时不会被动引入新的兼容宏。
//

#include <QtGlobal>

/**
 *@def Qt5Qt6Compat_QXXEvent_Pos
 *@brief QXXEvent_Pos函数的兼容宏
 *
 *'pos' is deprecated: Use position().toPoint()
 *
 *qevent.h:740:5: note: 'pos' has been explicitly marked deprecated here
 *
 *qglobal.h:369:44: note: expanded from macro 'QT_DEPRECATED_VERSION_X_6_0'
 *
 *qglobal.h:281:33: note: expanded from macro 'QT_DEPRECATED_X'
 *
 */
#ifndef Qt5Qt6Compat_QXXEvent_Pos
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
#define Qt5Qt6Compat_QXXEvent_Pos(valuePtr) valuePtr->pos()
#else
#define Qt5Qt6Compat_QXXEvent_Pos(valuePtr) valuePtr->position().toPoint()
#endif
#endif

/**
 *@def Qt5Qt6Compat_QXXEvent_x
 *@brief QXXEvent_x函数的兼容宏
 *
 * warning: 'int QMouseEvent::x() const' is deprecated: Use position()
 *
 * QMouseEvent::x
 */
#ifndef Qt5Qt6Compat_QXXEvent_x
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
#define Qt5Qt6Compat_QXXEvent_x(valuePtr) valuePtr->pos().x()
#else
#define Qt5Qt6Compat_QXXEvent_x(valuePtr) valuePtr->position().x()
#endif
#endif

/**
 *@def Qt5Qt6Compat_QXXEvent_y
 *@brief QXXEvent_y函数的兼容宏
 *
 * warning: 'int QMouseEvent::y() const' is deprecated: Use position()
 *
 * QMouseEvent::y
 */
#ifndef Qt5Qt6Compat_QXXEvent_y
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
#define Qt5Qt6Compat_QXXEvent_y(valuePtr) valuePtr->pos().y()
#else
#define Qt5Qt6Compat_QXXEvent_y(valuePtr) valuePtr->position().y()
#endif
#endif

/**
 * @def Qt5Qt6Compat_fontMetrics_width
 * @brief QFontMetrics的字体宽度适配
 */
#ifndef Qt5Qt6Compat_fontMetrics_width
#if QT_VERSION < QT_VERSION_CHECK(5, 12, 0)
#define Qt5Qt6Compat_fontMetrics_width(fontMeter, str) fontMeter.width(str)
#else
#define Qt5Qt6Compat_fontMetrics_width(fontMeter, str) fontMeter.horizontalAdvance(str)
#endif
#endif

#ifndef Qt5Qt6Compat_Connect_ButtonGroupClicked_int
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
#define Qt5Qt6Compat_Connect_ButtonGroupClicked_int(buttonGroup, funName)                                              \
    do {                                                                                                               \
        connect(buttonGroup, QOverload< int >::of(&QButtonGroup::buttonClicked), this, &funName);                      \
    } while (0)
#else
#define Qt5Qt6Compat_Connect_ButtonGroupClicked_int(buttonGroup, funName)                                              \
    do {                                                                                                               \
        connect(buttonGroup, &QButtonGroup::idClicked, this, &funName);                                                \
    } while (0)
#endif
#endif

#endif  // DAQT5QT6COMPAT_H
