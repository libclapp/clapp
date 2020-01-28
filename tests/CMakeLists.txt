set(sources value.cpp argument.cpp option.cpp parser.cpp sub_parser.cpp main_parser.cpp exception.cpp)

add_executable(libclapp_tests ${sources})
target_link_libraries(libclapp_tests libClaPP_gmock_main clapp)
target_compile_options(libclapp_tests PRIVATE ${libClaPP_TEST_COMPILE_OPTIONS})
install(TARGETS libclapp_tests DESTINATION ${CMAKE_INSTALL_BINDIR})
if(CLANG_TIDY)
  set_target_properties(libclapp_tests PROPERTIES CXX_CLANG_TIDY "${CLANG_TIDY_ARGS},-llvm-include-order,-clang-diagnostic-global-constructors,-hicpp-special-member-functions,-cppcoreguidelines-owning-memory,-cert-err58-cpp,-cppcoreguidelines-special-member-functions,-clang-diagnostic-used-but-marked-unused,-cppcoreguidelines-pro-type-vararg,-clang-diagnostic-covered-switch-default,-hicpp-vararg,-fuchsia-statically-constructed-objects,-cppcoreguidelines-avoid-goto,-hicpp-avoid-goto")
endif()

