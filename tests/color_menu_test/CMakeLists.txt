find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Test REQUIRED)

add_executable(dawidgets_color_menu_test tst_dacolormenu.cpp)
target_link_libraries(dawidgets_color_menu_test
    PRIVATE
        DA::DAWidgets
        Qt${QT_VERSION_MAJOR}::Widgets
        Qt${QT_VERSION_MAJOR}::Test
)
set_target_properties(dawidgets_color_menu_test PROPERTIES WIN32_EXECUTABLE OFF)
dawidgets_copy_runtime_dll(dawidgets_color_menu_test)

add_test(NAME dawidgets_color_menu_test COMMAND dawidgets_color_menu_test)
