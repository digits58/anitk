add_executable(test_main test.cpp)

target_include_directories(test_main PRIVATE
    ${PROJECT_SOURCE_DIR}
    ../extern/Catch2/src)
target_link_libraries(test_main PRIVATE Catch2::Catch2WithMain lib)
# target_link_libraries(test_main PRIVATE Catch2::Catch2)

include(Catch)
catch_discover_tests(test_main)
