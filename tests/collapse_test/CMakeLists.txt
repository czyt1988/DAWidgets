find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Test REQUIRED)

add_executable(dawidgets_collapse_test tst_dacollapse.cpp)
target_link_libraries(dawidgets_collapse_test
    PRIVATE
        DA::DAWidgets
        Qt${QT_VERSION_MAJOR}::Widgets
        Qt${QT_VERSION_MAJOR}::Test
)
set_target_properties(dawidgets_collapse_test PROPERTIES WIN32_EXECUTABLE OFF)
dawidgets_copy_runtime_dll(dawidgets_collapse_test)

add_test(NAME dawidgets_collapse_test COMMAND dawidgets_collapse_test)
