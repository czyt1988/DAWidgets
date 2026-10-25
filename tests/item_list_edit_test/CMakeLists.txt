find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Test REQUIRED)

add_executable(dawidgets_item_list_edit_test tst_daitemlisteditwidget.cpp)
target_link_libraries(dawidgets_item_list_edit_test
    PRIVATE
        DA::DAWidgets
        Qt${QT_VERSION_MAJOR}::Widgets
        Qt${QT_VERSION_MAJOR}::Test
)
set_target_properties(dawidgets_item_list_edit_test PROPERTIES WIN32_EXECUTABLE OFF)
dawidgets_copy_runtime_dll(dawidgets_item_list_edit_test)

add_test(NAME dawidgets_item_list_edit_test COMMAND dawidgets_item_list_edit_test)
