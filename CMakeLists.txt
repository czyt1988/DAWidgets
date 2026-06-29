cmake_minimum_required(VERSION 3.16)

project(DAWidgets
    VERSION 0.1.0
    DESCRIPTION "DA Selector - Qt tag/chip selector widgets (el-select style) for Qt5 and Qt6"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# ---- Options ----
option(DAWIDGETS_BUILD_SHARED "Build DAWidgets as a shared library (DLL/dylib/so); OFF=static" OFF)
option(DAWIDGETS_BUILD_DEMO   "Build DAWidgets demo applications" ON)
option(DAWIDGETS_BUILD_TESTS  "Build DAWidgets tests" OFF)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

# 源码为 UTF-8（含中文注释/字面量），强制 MSVC 以 UTF-8 解码，避免代码页乱码
if(MSVC)
    add_compile_options(/utf-8)
endif()

# ---- 自动生成内嵌资源 ----
# dawidgets_resources.{h,cpp} 由 tools/generate_resources.py 从 tools/*.png 生成。
# 若文件缺失（如 clone 后首次构建），自动调用脚本生成，保证开箱即用。
set(_DA_RES_H "${CMAKE_CURRENT_SOURCE_DIR}/src/dawidgets_resources.h")
set(_DA_RES_CPP "${CMAKE_CURRENT_SOURCE_DIR}/src/dawidgets_resources.cpp")
if(NOT EXISTS "${_DA_RES_H}" OR NOT EXISTS "${_DA_RES_CPP}")
    find_package(Python3 COMPONENTS Interpreter QUIET)
    if(Python3_Interpreter_FOUND)
        message(STATUS "DAWidgets: generating resources via ${Python3_EXECUTABLE}")
        execute_process(
            COMMAND ${Python3_EXECUTABLE} "${CMAKE_CURRENT_SOURCE_DIR}/tools/generate_resources.py"
            WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
            RESULT_VARIABLE _gen_res
        )
        if(NOT _gen_res EQUAL 0)
            message(WARNING "DAWidgets: resource generation failed (${_gen_res}); "
                            "ensure tools/*.png exist and Python3 is available")
        endif()
    else()
        message(WARNING "DAWidgets: dawidgets_resources.* missing and Python3 not found; "
                        "build will fail. Run: python tools/generate_resources.py")
    endif()
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
    src/DACollapse.h
    src/DACollapse.cpp
    src/DACollapseItem.h
    src/DACollapseItem.cpp
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
    src/DAFilePathEditWidget.h
    src/DAFilePathEditWidget.cpp
    src/DAFilePathEditWidget.ui
    src/DARecentFilesManager.h
    src/DARecentFilesManager.cpp
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
    src/dawidgets_resources.h
    src/dawidgets_resources.cpp
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

target_include_directories(DAWidgets
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
        $<INSTALL_INTERFACE:include>
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
    add_subdirectory(examples/chipsedit_demo)
    add_subdirectory(examples/selectcombobox_demo)
    add_subdirectory(examples/collapse_demo)
    add_subdirectory(examples/all_widgets_demo)
endif()

# ---- Tests ----
if(DAWIDGETS_BUILD_TESTS)
    add_subdirectory(tests/logic_test)
    add_subdirectory(tests/combobox_test)
    add_subdirectory(tests/collapse_test)
endif()
