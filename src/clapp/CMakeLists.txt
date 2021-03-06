add_library(clapp SHARED option.cpp value.cpp parser.cpp main_parser.cpp sub_parser.cpp exception.cpp)
set(libClaPP_INCLUDE_DIRS $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/../include>)
target_include_directories(clapp PRIVATE ${libClaPP_INCLUDE_DIRS}
  SYSTEM INTERFACE ${libClaPP_INCLUDE_DIRS} $<INSTALL_INTERFACE:../include>)
target_link_libraries(clapp PUBLIC ClaPP_OPTIONS libClaPP_GSL ${libClaPP_FS_LINKER_FLAG})
target_compile_options(clapp PRIVATE ${libClaPP_COMPILE_OPTIONS})
set_target_properties(clapp PROPERTIES VERSION "${libClaPP_VERSION}" POSITION_INDEPENDENT_CODE ON)
if(CLANG_TIDY)
  set_target_properties(clapp PROPERTIES CXX_CLANG_TIDY "${CLANG_TIDY_ARGS}")
endif()

install(TARGETS clapp LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    target_compile_options(clapp PRIVATE -Wnull-dereference -Wuseless-cast -Wswitch-default)
endif()
