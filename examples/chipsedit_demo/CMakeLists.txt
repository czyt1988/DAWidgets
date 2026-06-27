add_executable(dawidgets_chipsedit_demo main.cpp)
target_link_libraries(dawidgets_chipsedit_demo
    PRIVATE
        DA::DAWidgets
        Qt${QT_VERSION_MAJOR}::Widgets
)
# 保持控制台输出可见（便于打印信号日志）
set_target_properties(dawidgets_chipsedit_demo PROPERTIES WIN32_EXECUTABLE OFF)
