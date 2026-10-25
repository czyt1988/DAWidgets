find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Test REQUIRED)

add_executable(dawidgets_overlay_test tst_daabstractwidgetoverlay.cpp)
target_link_libraries(dawidgets_overlay_test
    PRIVATE
        DA::DAWidgets
        Qt${QT_VERSION_MAJOR}::Widgets
        Qt${QT_VERSION_MAJOR}::Test
)
set_target_properties(dawidgets_overlay_test PROPERTIES WIN32_EXECUTABLE OFF)
dawidgets_copy_runtime_dll(dawidgets_overlay_test)

add_test(NAME dawidgets_overlay_test COMMAND dawidgets_overlay_test)
