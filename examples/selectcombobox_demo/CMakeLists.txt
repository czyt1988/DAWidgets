add_executable(dawidgets_selectcombobox_demo main.cpp)
target_link_libraries(dawidgets_selectcombobox_demo
    PRIVATE
        DA::DAWidgets
        Qt${QT_VERSION_MAJOR}::Widgets
)
set_target_properties(dawidgets_selectcombobox_demo PROPERTIES WIN32_EXECUTABLE OFF)
