set(sources value.cpp argument.cpp option.cpp parser.cpp sub_parser.cpp main_parser.cpp parser_container.cpp parser_types.cpp exception.cpp build_info.cpp type_traits.cpp)

add_executable(libclapp_tests ${sources})
target_link_libraries(libclapp_tests libClaPP_gmock_main libClaPP::clapp libClaPP_TEST_CXX_COMPILE_OPTIONS)
install(TARGETS libclapp_tests DESTINATION ${CMAKE_INSTALL_BINDIR})
if(libClaPP_CLANG_TIDY_ARGS_GTEST)
    set_target_properties(libclapp_tests PROPERTIES CXX_CLANG_TIDY "${libClaPP_CLANG_TIDY_ARGS_GTEST}")
endif()
add_test(NAME libclapp_tests COMMAND libclapp_tests)
