add_library(clapp SHARED option.cpp value.cpp parser.cpp parser_types.cpp main_parser.cpp sub_parser.cpp exception.cpp)
set(libClaPP_INCLUDE_DIRS $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>
                          $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/../include>
                          $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
                          )

target_include_directories(clapp PUBLIC ${libClaPP_INCLUDE_DIRS})

target_link_libraries(clapp PUBLIC libClaPP_SANITIZER libClaPP_GSL ${libClaPP_CXX_FS_LINKER_FLAG})
target_link_libraries(clapp PRIVATE libClaPP_CXX_COMPILE_OPTIONS)
set_target_properties(clapp PROPERTIES
	                    VERSION "${libClaPP_VERSION}"
			    SOVERSION "${libClaPP_VERSION_MAJOR}.${libClaPP_VERSION_MINOR}"
			    POSITION_INDEPENDENT_CODE ON)
if(libClaPP_CLANG_TIDY_ARGS)
  set_target_properties(clapp PROPERTIES CXX_CLANG_TIDY "${libClaPP_CLANG_TIDY_ARGS}")
endif()

add_library(libClaPP::clapp ALIAS clapp)

install(TARGETS clapp libClaPP_GSL libClaPP_SANITIZER libClaPP_CXX_COMPILE_OPTIONS
    EXPORT libclapp-targets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(EXPORT libclapp-targets
        FILE libclapp-targets.cmake
        NAMESPACE libClaPP::
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/libclapp
)

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    target_compile_options(clapp PRIVATE -Wnull-dereference -Wuseless-cast)
endif()

configure_package_config_file(${CMAKE_CURRENT_SOURCE_DIR}/config.cmake.in
  "${CMAKE_CURRENT_BINARY_DIR}/libclapp-config.cmake"
  INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/libclapp
)

# generate the version file for the config file
write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/libclapp-config-version.cmake"
  VERSION "${libClaPP_VERSION}"
  COMPATIBILITY SameMinorVersion
)

install(FILES
          "${CMAKE_CURRENT_BINARY_DIR}/libclapp-config.cmake"
          "${CMAKE_CURRENT_BINARY_DIR}/libclapp-config-version.cmake"
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/libclapp
)
