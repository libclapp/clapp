set(sources value.cpp argument.cpp option.cpp parser.cpp sub_parser.cpp main_parser.cpp)

add_executable(libclapp_tests ${sources})
target_link_libraries(libclapp_tests gtest_main gmock clapp)
install(TARGETS libclapp_tests DESTINATION ${CMAKE_INSTALL_BINDIR})
