find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Test REQUIRED)

add_executable(dawidgets_filter_proxy_test tst_dastringfilterproxymodel.cpp)
target_link_libraries(dawidgets_filter_proxy_test
    PRIVATE
        DA::DAWidgets
        Qt${QT_VERSION_MAJOR}::Core
        Qt${QT_VERSION_MAJOR}::Test
)
set_target_properties(dawidgets_filter_proxy_test PROPERTIES WIN32_EXECUTABLE OFF)
dawidgets_copy_runtime_dll(dawidgets_filter_proxy_test)

add_test(NAME dawidgets_filter_proxy_test COMMAND dawidgets_filter_proxy_test)
