include(CheckCXXCompilerFlag)

#################
# Global stuff: #
#################
string(TOLOWER "${CMAKE_BUILD_TYPE}" libClaPP_BUILD_TYPE)
if (${libClaPP_BUILD_TYPE} MATCHES debug)
    message(STATUS "debug mode with gdb-flags")
    list(APPEND libClaPP_COMPILE_OPTIONS -ggdb -O0)
elseif(${libClaPP_BUILD_TYPE} MATCHES release OR "${libClaPP_BUILD_TYPE}" STREQUAL "")
    message(STATUS "release mode with no-debug")
    list(APPEND libClaPP_COMPILE_OPTIONS -O2)
    list(APPEND libClaPP_COMPILE_DEFINITIONS NDEBUG)
else()
    message(FATAL_ERROR "Invalid build-type ${libClaPP_BUILD_TYPE} use 'debug' or 'release'")
endif()

if(libClaPP_BUILD_COVERAGE)
    list(APPEND libClaPP_COMPILE_OPTIONS -coverage)
    list(APPEND libClaPP_LINK_LIBRARIES -coverage)
endif()

##############
# CXX stuff: #
##############
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    list(APPEND libClaPP_CXX_PEDANTIC_COMPILE_OPTIONS -Wall -Wextra -Wunreachable-code -Wfloat-equal -Werror -Wpedantic -Wcast-align -Wcast-qual -Wpointer-arith -Wwrite-strings -Wstrict-overflow=2 -Wformat=2 -Wlogical-op -Wduplicated-cond -Wduplicated-branches -Wdouble-promotion -Wold-style-cast -Wuninitialized -Wmaybe-uninitialized -Wconversion -Wunused -Wnarrowing -Wconversion-null -Wsign-conversion -Wrestrict -Wnull-dereference -Wmisleading-indentation -Wnon-virtual-dtor -Woverloaded-virtual -Wshadow -Wuseless-cast -Wno-duplicated-branches -Weffc++ -Warray-bounds=2)
    if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL "10.0.0")
        list(APPEND libClaPP_CXX_PEDANTIC_COMPILE_OPTIONS -Warith-conversion -Wredundant-tags -Wmismatched-tags)
    endif()
    if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL "11.0.0")
        list(APPEND libClaPP_CXX_PEDANTIC_COMPILE_OPTIONS -Wdeprecated-enum-enum-conversion -Wdeprecated-enum-float-conversion -Wrange-loop-construct)
    endif()
    set(libClaPP_TEST_CXX_PEDANTIC_COMPILE_OPTIONS ${libClaPP_CXX_PEDANTIC_COMPILE_OPTIONS})
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    list(APPEND libClaPP_CXX_PEDANTIC_COMPILE_OPTIONS -Weverything -Wno-c++98-compat -Wno-c++98-compat-pedantic -Wno-padded -Werror -pedantic-errors)
    set(libClaPP_TEST_CXX_PEDANTIC_COMPILE_OPTIONS ${libClaPP_CXX_PEDANTIC_COMPILE_OPTIONS})
    list(APPEND libClaPP_TEST_CXX_PEDANTIC_COMPILE_OPTIONS -Wno-global-constructors)
endif()

CHECK_CXX_COMPILER_FLAG("-lstdc++fs" COMPILER_SUPPORTS_FILESYSTEM)
if(COMPILER_SUPPORTS_FILESYSTEM)
    set(libClaPP_CXX_FS_LINKER_FLAG "-lstdc++fs")
endif()

add_library(libClaPP_CXX_COMPILE_OPTIONS INTERFACE)
target_compile_features(libClaPP_CXX_COMPILE_OPTIONS INTERFACE cxx_std_17)
target_link_libraries(libClaPP_CXX_COMPILE_OPTIONS INTERFACE ${libClaPP_LINK_LIBRARIES})
target_compile_options(libClaPP_CXX_COMPILE_OPTIONS INTERFACE ${libClaPP_COMPILE_OPTIONS})
target_compile_definitions(libClaPP_CXX_COMPILE_OPTIONS INTERFACE ${libClaPP_COMPILE_DEFINITIONS})
if(libClaPP_PEDANTIC_COMPILER_FLAGS)
    target_compile_options(libClaPP_CXX_COMPILE_OPTIONS INTERFACE ${libClaPP_CXX_PEDANTIC_COMPILE_OPTIONS})
endif()

add_library(libClaPP_TEST_CXX_COMPILE_OPTIONS INTERFACE)
target_compile_features(libClaPP_TEST_CXX_COMPILE_OPTIONS INTERFACE cxx_std_17)
target_link_libraries(libClaPP_TEST_CXX_COMPILE_OPTIONS INTERFACE ${libClaPP_LINK_LIBRARIES})
target_compile_options(libClaPP_TEST_CXX_COMPILE_OPTIONS INTERFACE ${libClaPP_COMPILE_OPTIONS})
target_compile_definitions(libClaPP_TEST_CXX_COMPILE_OPTIONS INTERFACE ${libClaPP_COMPILE_DEFINITIONS})
if(libClaPP_PEDANTIC_COMPILER_FLAGS)
    target_compile_options(libClaPP_TEST_CXX_COMPILE_OPTIONS INTERFACE ${libClaPP_TEST_CXX_PEDANTIC_COMPILE_OPTIONS})
endif()
