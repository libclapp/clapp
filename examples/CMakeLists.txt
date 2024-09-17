add_executable(libclapp_example_large large_example.cpp)
target_link_libraries(libclapp_example_large libClaPP::clapp
                      libClaPP_CXX_COMPILE_OPTIONS)
if(libClaPP_CLANG_TIDY_ARGS_EXAMPLES)
    set_target_properties(
        libclapp_example_large
        PROPERTIES CXX_CLANG_TIDY "${libClaPP_CLANG_TIDY_ARGS_EXAMPLES}")
endif()
install(TARGETS libclapp_example_large DESTINATION ${CMAKE_INSTALL_BINDIR})
configure_file(test_large_example.bats test_large_example.bats COPYONLY)
add_test(
    NAME large_example_tests
    COMMAND test_large_example.bats
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

add_executable(libclapp_example_short short_example.cpp)
target_link_libraries(libclapp_example_short libClaPP::clapp
                      libClaPP_CXX_COMPILE_OPTIONS)
if(libClaPP_CLANG_TIDY_ARGS_EXAMPLES)
    set_target_properties(
        libclapp_example_short
        PROPERTIES CXX_CLANG_TIDY "${libClaPP_CLANG_TIDY_ARGS_EXAMPLES}")
endif()
install(TARGETS libclapp_example_short DESTINATION ${CMAKE_INSTALL_BINDIR})
configure_file(test_short_example.bats test_short_example.bats COPYONLY)
add_test(
    NAME short_example_tests
    COMMAND test_short_example.bats
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

add_executable(libclapp_example_sub_parser sub_parser_example.cpp)
target_link_libraries(libclapp_example_sub_parser libClaPP::clapp
                      libClaPP_CXX_COMPILE_OPTIONS)
if(libClaPP_CLANG_TIDY_ARGS_EXAMPLES)
    set_target_properties(
        libclapp_example_sub_parser
        PROPERTIES CXX_CLANG_TIDY "${libClaPP_CLANG_TIDY_ARGS_EXAMPLES}")
endif()
install(TARGETS libclapp_example_sub_parser DESTINATION ${CMAKE_INSTALL_BINDIR})
configure_file(test_sub_parser_example.bats test_sub_parser_example.bats
               COPYONLY)
add_test(
    NAME sub_parser_example_tests
    COMMAND test_sub_parser_example.bats
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

add_executable(libclapp_example_container container_example.cpp)
target_link_libraries(libclapp_example_container libClaPP::clapp
                      libClaPP_CXX_COMPILE_OPTIONS)
if(libClaPP_CLANG_TIDY_ARGS_EXAMPLES)
    set_target_properties(
        libclapp_example_container
        PROPERTIES CXX_CLANG_TIDY "${libClaPP_CLANG_TIDY_ARGS_EXAMPLES}")
endif()
install(TARGETS libclapp_example_container DESTINATION ${CMAKE_INSTALL_BINDIR})
configure_file(test_container_example.bats test_container_example.bats COPYONLY)
add_test(
    NAME container_example_tests
    COMMAND test_container_example.bats
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
