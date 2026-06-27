find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Test REQUIRED)

add_executable(dawidgets_logic_test tst_dachipsedit.cpp)
target_link_libraries(dawidgets_logic_test
    PRIVATE
        DA::DAWidgets
        Qt${QT_VERSION_MAJOR}::Widgets
        Qt${QT_VERSION_MAJOR}::Test
)
set_target_properties(dawidgets_logic_test PROPERTIES WIN32_EXECUTABLE OFF)

add_test(NAME dawidgets_logic_test COMMAND dawidgets_logic_test)
