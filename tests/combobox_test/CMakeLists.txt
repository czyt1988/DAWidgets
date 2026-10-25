find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Test REQUIRED)

add_executable(dawidgets_combobox_test tst_daselectcombobox.cpp)
target_link_libraries(dawidgets_combobox_test
    PRIVATE
        DA::DAWidgets
        Qt${QT_VERSION_MAJOR}::Widgets
        Qt${QT_VERSION_MAJOR}::Test
)
set_target_properties(dawidgets_combobox_test PROPERTIES WIN32_EXECUTABLE OFF)
dawidgets_copy_runtime_dll(dawidgets_combobox_test)

add_test(NAME dawidgets_combobox_test COMMAND dawidgets_combobox_test)
