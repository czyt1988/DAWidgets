find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Test REQUIRED)

add_executable(dawidgets_undo_history_test tst_daundohistorywidget.cpp)
target_link_libraries(dawidgets_undo_history_test
    PRIVATE
        DA::DAWidgets
        Qt${QT_VERSION_MAJOR}::Widgets
        Qt${QT_VERSION_MAJOR}::Test
)
set_target_properties(dawidgets_undo_history_test PROPERTIES WIN32_EXECUTABLE OFF)
dawidgets_copy_runtime_dll(dawidgets_undo_history_test)

add_test(NAME dawidgets_undo_history_test COMMAND dawidgets_undo_history_test)
