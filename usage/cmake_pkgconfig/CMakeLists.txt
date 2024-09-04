cmake_minimum_required(VERSION 3.8.0)
project(libclapp-usage-cmake-pkgconfig)

find_package(PkgConfig REQUIRED)

pkg_check_modules(LIBCLAPP REQUIRED libclapp)
add_library(libClaPP_clapp INTERFACE)
target_include_directories(libClaPP_clapp SYSTEM INTERFACE ${LIBCLAPP_INCLUDE_DIRS})
target_link_libraries(libClaPP_clapp INTERFACE ${LIBCLAPP_LIBRARIES})
add_library(libClaPP::clapp ALIAS libClaPP_clapp)

add_executable(usage_cmake_pkgconfig ../main.cpp)
target_compile_features(usage_cmake_pkgconfig PRIVATE cxx_std_20)
target_link_libraries(usage_cmake_pkgconfig PRIVATE libClaPP::clapp)
