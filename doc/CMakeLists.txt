if(CLANG_TIDY)
    set(CLANG_TIDY_DOC_ARGS "${CLANG_TIDY_ARGS},-misc-non-private-member-variables-in-classes,-cppcoreguidelines-avoid-magic-numbers,-readability-magic-numbers")
endif()

find_program(RAKU_EXECUTABLE NAMES raku perl6)

add_custom_command( OUTPUT simple_sub_parser.cpp simple_main_parser.cpp DEPENDS doc.md
    COMMAND ${RAKU_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/sample_extractor.p6 ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(libclapp_doc_simple_main_parser simple_main_parser.cpp)
target_link_libraries(libclapp_doc_simple_main_parser clapp)
if(libClaPP_PEDANTIC_COMPILER_FLAGS)
  target_compile_options(libclapp_doc_simple_main_parser PRIVATE ${libClaPP_PEDANTIC_COMPILE_OPTIONS})
endif()
if(CLANG_TIDY)
  set_target_properties(libclapp_doc_simple_main_parser PROPERTIES CXX_CLANG_TIDY "${CLANG_TIDY_DOC_ARGS}")
endif()
add_test(NAME libclapp_doc_simple_main_parser_test COMMAND ${RAKU_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/sample_executor.p6 simple_main_parser.out)

add_executable(libclapp_doc_simple_sub_parser simple_sub_parser.cpp)
target_link_libraries(libclapp_doc_simple_sub_parser clapp)
if(libClaPP_PEDANTIC_COMPILER_FLAGS)
  target_compile_options(libclapp_doc_simple_sub_parser PRIVATE ${libClaPP_PEDANTIC_COMPILE_OPTIONS})
endif()
if(CLANG_TIDY)
  set_target_properties(libclapp_doc_simple_sub_parser PROPERTIES CXX_CLANG_TIDY "${CLANG_TIDY_DOC_ARGS}")
endif()
add_test(NAME libclapp_doc_simple_sub_parser_test COMMAND ${RAKU_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/sample_executor.p6 simple_sub_parser.out)
