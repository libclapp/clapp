add_library(libClaPP_gmock_main INTERFACE)
if(libClaPP_BUILD_TESTS)
    if(libClaPP_SUBMODULE_DEPENDENCIES)
        if(NOT TARGET gmock_main)
            add_subdirectory("${THIRD_PARTY_DIR}/googletest" EXCLUDE_FROM_ALL)
            target_compile_features(gtest PUBLIC cxx_std_17)    #changing gtest (and its further libs gmock and gmock_main) to be C++17, otherwise we will get linker error.
        endif()
    else()
        if(NOT TARGET GTest::gmock_main)
            find_package(GTest.gmock_main 1.10.0 REQUIRED)
        endif()
    endif()
endif()

target_link_libraries(libClaPP_gmock_main INTERFACE GTest::gmock_main)

if(libClaPP_PEDANTIC_COMPILER_FLAGS)
    if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
        target_compile_options(libClaPP_gmock_main INTERFACE -Wno-used-but-marked-unused -Wno-covered-switch-default -Wno-exit-time-destructors)
    endif()
endif()
