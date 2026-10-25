find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Test REQUIRED)

add_executable(dawidgets_string_list_test tst_dastringlistedwidget.cpp)
target_link_libraries(dawidgets_string_list_test
    PRIVATE
        DA::DAWidgets
        Qt${QT_VERSION_MAJOR}::Widgets
        Qt${QT_VERSION_MAJOR}::Test
)
set_target_properties(dawidgets_string_list_test PROPERTIES WIN32_EXECUTABLE OFF)
dawidgets_copy_runtime_dll(dawidgets_string_list_test)

add_test(NAME dawidgets_string_list_test COMMAND dawidgets_string_list_test)
