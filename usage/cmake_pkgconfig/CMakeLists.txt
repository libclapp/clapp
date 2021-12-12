cmake_minimum_required(VERSION 3.8.0)
project(libclapp-usage-cmake-pkgconfig)

find_package(PkgConfig REQUIRED)

pkg_check_modules(LIBCLAPP REQUIRED libclapp)
add_library(libClaPP_clapp INTERFACE)
target_include_directories(libClaPP_clapp SYSTEM INTERFACE ${LIBCLAPP_INCLUDE_DIRS})
target_link_libraries(libClaPP_clapp INTERFACE ${LIBCLAPP_LIBRARIES})
add_library(libClaPP::clapp ALIAS libClaPP_clapp)

find_path(libClaPP_GSL_INCLUDE_DIR NAMES gsl_assert span PATH_SUFFIXES gsl)
if(NOT libClaPP_GSL_INCLUDE_DIR)
    message(FATAL_ERROR "Could not find GSL library.")
endif()
add_library(libClaPP_GSL INTERFACE)
target_include_directories(libClaPP_GSL INTERFACE ${libClaPP_GSL_INCLUDE_DIR}/..)

add_executable(usage_cmake_pkgconfig ../main.cpp)
target_compile_features(usage_cmake_pkgconfig PRIVATE cxx_std_17)
target_link_libraries(usage_cmake_pkgconfig PRIVATE libClaPP::clapp libClaPP_GSL)
