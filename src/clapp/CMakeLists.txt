add_library(clapp SHARED option.cpp value.cpp parser.cpp main_parser.cpp sub_parser.cpp)
target_include_directories(clapp PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/../include>
  $<INSTALL_INTERFACE:../include>
)
target_link_libraries(clapp PUBLIC libClaPP_GSL ${libClaPP_FS_LINKER_FLAG})
set_target_properties(clapp PROPERTIES VERSION "${project_VERSION_MAJOR}.${project_VERSION_MINOR}" POSITION_INDEPENDENT_CODE ON)
install(TARGETS clapp LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    target_compile_options(clapp PRIVATE -Wnull-dereference -Wuseless-cast -Wswitch-default)
endif()
