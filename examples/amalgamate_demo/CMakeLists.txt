# DAWidgets 单文件合并示例
#
# 关键点：不链接 DA::DAWidgets，只把 src-amalgamate 下的 .h/.cpp 编进可执行文件。
# - AUTOMOC 由根 CMakeLists 统一开启；DAWidgets.h 里的 Q_OBJECT 类需要 moc，
#   因此必须把 DAWidgets.h 列进源文件，让 AUTOMOC 为它生成 moc_DAWidgets.cpp。
# - DA_BUILT_AS_STATIC 必须定义：合成文件不是 DLL，定义后 DAWIDGETS_EXPORT 展开为空，
#   否则 Windows 上会展开成 dllimport 导致链接失败。
# - Qt Svg 是必需依赖（属性编辑控件的 SVG 图标通过 Qt Svg 渲染）。

add_executable(dawidgets_amalgamate_demo
    main.cpp
    ${CMAKE_CURRENT_LIST_DIR}/../../src-amalgamate/DAWidgets.h
    ${CMAKE_CURRENT_LIST_DIR}/../../src-amalgamate/DAWidgets.cpp
)

target_include_directories(dawidgets_amalgamate_demo
    PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/../../src-amalgamate
)

target_compile_definitions(dawidgets_amalgamate_demo
    PRIVATE
        DA_BUILT_AS_STATIC
)

target_link_libraries(dawidgets_amalgamate_demo
    PRIVATE
        Qt${QT_VERSION_MAJOR}::Widgets
        Qt${QT_VERSION_MAJOR}::Svg
)

if(MSVC)
    # 合成文件是单一翻译单元，功能一多就会超过 COFF 默认的 65536 个 section
    #（fatal error C1128），统一加 /bigobj；同时保持源码 UTF-8 解码。
    target_compile_options(dawidgets_amalgamate_demo PRIVATE /bigobj /utf-8)
endif()

# 保持控制台输出可见，便于 CI 打印 --smoke 结果
set_target_properties(dawidgets_amalgamate_demo PROPERTIES WIN32_EXECUTABLE OFF)
