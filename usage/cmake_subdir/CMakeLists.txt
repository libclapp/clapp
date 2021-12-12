cmake_minimum_required(VERSION 3.8.0)
project(libclapp-usage-cmake-pkgconfig)

set(libClaPP_SUBMODULE_DEPENDENCIES On CACHE BOOL "Enable all submodules shipped in libClaPP.")
add_subdirectory("clapp" EXCLUDE_FROM_ALL)

add_executable(usage_cmake_subdir ../main.cpp)
target_compile_features(usage_cmake_subdir PRIVATE cxx_std_17)
target_link_libraries(usage_cmake_subdir PRIVATE libClaPP::clapp)
