cmake_minimum_required(VERSION 3.8.0)
project(libclapp-usage-cmake-import)

find_package(libclapp REQUIRED)
find_package(Microsoft.GSL REQUIRED)

add_executable(usage_cmake_import ../main.cpp)
target_compile_features(usage_cmake_import PRIVATE cxx_std_17)
target_link_libraries(usage_cmake_import PRIVATE libClaPP::clapp)
