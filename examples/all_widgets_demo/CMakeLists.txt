add_executable(dawidgets_all_widgets_demo
    main.cpp
    TabSelector.cpp
    TabColor.cpp
    TabDrawingStyle.cpp
    TabAlignment.cpp
    TabUtility.cpp
    TabItemSelection.cpp
    TabText.cpp
    TabFilter.cpp
)
target_link_libraries(dawidgets_all_widgets_demo
    PRIVATE
        DA::DAWidgets
        Qt${QT_VERSION_MAJOR}::Widgets
)
# 保持控制台输出可见（便于打印信号日志）
set_target_properties(dawidgets_all_widgets_demo PROPERTIES WIN32_EXECUTABLE OFF)
dawidgets_copy_runtime_dll(dawidgets_all_widgets_demo)
