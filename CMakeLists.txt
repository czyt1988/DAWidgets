cmake_minimum_required(VERSION 3.16)

project(DAWidgets
    VERSION 0.1.0
    DESCRIPTION "DAWidgets - Qt QWidget supplement library inspired by Element UI, for Qt5 and Qt6"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# ---- Options ----
option(DAWIDGETS_BUILD_SHARED "Build DAWidgets as a shared library (DLL/dylib/so); OFF=static" ON)
option(DAWIDGETS_BUILD_DEMO   "Build DAWidgets demo applications" ON)
option(DAWIDGETS_BUILD_TESTS  "Build DAWidgets tests" OFF)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

# ---- Install / find_package 支持 ----
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

# 源码为 UTF-8（含中文注释/字面量），强制 MSVC 以 UTF-8 解码，避免代码页乱码
if(MSVC)
    add_compile_options(/utf-8)
endif()

# ---- Qt (Qt5 / Qt6 compatible) ----
find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets Svg REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets Svg REQUIRED)

message(STATUS "DAWidgets: using Qt ${QT_VERSION_MAJOR} (${Qt${QT_VERSION_MAJOR}_VERSION})")
message(STATUS "DAWidgets: shared=${DAWIDGETS_BUILD_SHARED} demo=${DAWIDGETS_BUILD_DEMO} tests=${DAWIDGETS_BUILD_TESTS}")

set(_DAWIDGETS_SOURCES
    src/DAWidgetsGlobals.h
    src/DAQt5Qt6Compat.h
    src/DAFlowLayout.h
    src/DAFlowLayout.cpp
    src/DAChip.h
    src/DAChip.cpp
    src/DAChipsEdit.h
    src/DAChipsEdit.cpp
    src/DASelectComboBox.h
    src/DASelectComboBox.cpp
    src/DACollapseWidget.h
    src/DACollapseWidget.cpp
    src/DACollapseItem.h
    src/DACollapseItem.cpp
    src/DACollapseLayout.h
    src/DACollapseLayout.cpp
    src/DACursorScoped.h
    src/DACursorScoped.cpp
    src/DAWaitCursorScoped.h
    src/DAWaitCursorScoped.cpp
    src/DAPropertyItemWidget.h
    src/DAPropertyItemWidget.cpp
    src/DATimeFormatEditWidget.h
    src/DATimeFormatEditWidget.cpp
    src/DATimeFormatEditWidget.ui
    src/DAAligmentEditWidget.h
    src/DAAligmentEditWidget.cpp
    src/DAAligmentEditWidget.ui
    src/DAAligmentPositionEditWidget.h
    src/DAAligmentPositionEditWidget.cpp
    src/DAAligmentPositionEditWidget.ui
    src/DATextAlignmentEditWidget.h
    src/DATextAlignmentEditWidget.cpp
    src/DAFilePathEditWidget.h
    src/DAFilePathEditWidget.cpp
    src/DAFilePathEditWidget.ui
    src/DABrushStyleComboBox.h
    src/DABrushStyleComboBox.cpp
    src/DAPenStyleComboBox.h
    src/DAPenStyleComboBox.cpp
    src/DAColorGridWidget.h
    src/DAColorGridWidget.cpp
    src/DAColorPaletteGridWidget.h
    src/DAColorPaletteGridWidget.cpp
    src/DAColorMenu.h
    src/DAColorMenu.cpp
    src/DAColorPickerButton.h
    src/DAColorPickerButton.cpp
    src/DAItemListEditWidget.h
    src/DAItemListEditWidget.cpp
    src/DAItemListPickDialog.h
    src/DAItemListPickDialog.cpp
    src/DACheckListWidget.h
    src/DACheckListWidget.cpp
    src/DAStringListEditWidget.h
    src/DAStringListEditWidget.cpp
    src/DATextEditWidget.h
    src/DATextEditWidget.cpp
    src/DAStringFilterProxyModel.h
    src/DAStringFilterProxyModel.cpp
    src/DAUndoHistoryWidget.h
    src/DAUndoHistoryWidget.cpp
    src/DAAbstractWidgetOverlay.h
    src/DAAbstractWidgetOverlay.cpp
    src/DAColorScalePreviewWidget.h
    src/DAColorScalePreviewWidget.cpp
    src/DAPenEditWidget.h
    src/DAPenEditWidget.cpp
    src/DABrushEditWidget.h
    src/DABrushEditWidget.cpp
    src/DABrushEditWidget.ui
    src/DAFontEditPannelWidget.h
    src/DAFontEditPannelWidget.cpp
    src/DAFontEditPannelWidget.ui
    src/DAShapeEditPannelWidget.h
    src/DAShapeEditPannelWidget.cpp
    src/DAShapeEditPannelWidget.ui
    src/resources/dawidgetsicons.qrc
)

# ---- Library ----
if(DAWIDGETS_BUILD_SHARED)
    add_library(DAWidgets SHARED ${_DAWIDGETS_SOURCES})
    # SHARED 模式：不定义 DA_BUILT_AS_STATIC，使 DAWIDGETS_EXPORT 在编译库时
    # 展开为 dllexport、在使用者侧展开为 dllimport，从而生成导入库。
    target_compile_definitions(DAWidgets PRIVATE DAWIDGETS_EXPORTS)
else()
    add_library(DAWidgets STATIC ${_DAWIDGETS_SOURCES})
    # 静态库使用者也需要定义此宏，避免 DAWIDGETS_EXPORT 被展开成 dllimport
    target_compile_definitions(DAWidgets PUBLIC DA_BUILT_AS_STATIC)
endif()
add_library(DA::DAWidgets ALIAS DAWidgets)

# Windows 下把共享库 DLL 复制到每个可执行目标旁边：
# ctest 与 demo 直接从构建目录运行时，Windows 不会搜索兄弟目录，
# 缺这步会以 0xc0000135（DLL not found）失败。
function(dawidgets_copy_runtime_dll target)
    if(WIN32 AND DAWIDGETS_BUILD_SHARED)
        add_custom_command(TARGET ${target} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                $<TARGET_FILE:DAWidgets> $<TARGET_FILE_DIR:${target}>
            COMMENT "Copy DAWidgets runtime DLL next to ${target}"
        )
    endif()
endfunction()

target_include_directories(DAWidgets
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/DAWidgets>
)

target_link_libraries(DAWidgets
    PUBLIC
        Qt${QT_VERSION_MAJOR}::Widgets
        Qt${QT_VERSION_MAJOR}::Svg
)

if(QT_VERSION_MAJOR EQUAL 6)
    target_compile_definitions(DAWidgets PRIVATE QT_DISABLE_DEPRECATED_BEFORE=0x060000)
else()
    target_compile_definitions(DAWidgets PRIVATE QT_DISABLE_DEPRECATED_BEFORE=0x050F00)
endif()

# ---- Demo ----
if(DAWIDGETS_BUILD_DEMO)
    add_subdirectory(examples/all_widgets_demo)
endif()

# ---- Tests ----
if(DAWIDGETS_BUILD_TESTS)
    # add_test() 只有 enable_testing() 之后才会生成 CTestTestfile.cmake，
    # 否则 ctest 会报 "No tests were found!!!"（构建通过但测试从未被登记）
    enable_testing()
    add_subdirectory(tests/logic_test)
    add_subdirectory(tests/combobox_test)
    add_subdirectory(tests/collapse_test)
    add_subdirectory(tests/file_path_edit_test)
    add_subdirectory(tests/check_list_test)
    add_subdirectory(tests/string_list_test)
    add_subdirectory(tests/text_edit_test)
    add_subdirectory(tests/filter_proxy_test)
    add_subdirectory(tests/undo_history_test)
    add_subdirectory(tests/overlay_test)
    add_subdirectory(tests/item_list_edit_test)
    add_subdirectory(tests/text_alignment_test)
endif()

# ---- Install ----
# 安装库目标（ARCHIVE=静态库/导入库, LIBRARY=动态库, RUNTIME=可执行文件/DLL）
install(TARGETS DAWidgets
    EXPORT DAWidgetsTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# 安装公共头文件（src/ 下所有 .h）
install(DIRECTORY src/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/DAWidgets
    FILES_MATCHING PATTERN "*.h"
)

# 导出 Targets 文件（供 find_package 使用）
install(EXPORT DAWidgetsTargets
    FILE DAWidgetsTargets.cmake
    NAMESPACE DA::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/DAWidgets
)

# 生成版本文件
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/DAWidgetsConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

# 生成 Config 文件（模板中的 @QT_VERSION_MAJOR@ 会被替换为实际 Qt 版本）
configure_package_config_file(
    "${CMAKE_CURRENT_LIST_DIR}/cmake/DAWidgetsConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/DAWidgetsConfig.cmake"
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/DAWidgets
)

# 安装 Config 与 ConfigVersion 文件
install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/DAWidgetsConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/DAWidgetsConfigVersion.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/DAWidgets
)
