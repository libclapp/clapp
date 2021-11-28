if(NOT libClaPP_CLANG_TIDY_BIN)
	find_program(libClaPP_CLANG_TIDY_BIN NAMES "clang-tidy" DOC "Path to clang-tidy executable")

    if(NOT libClaPP_CLANG_TIDY_BIN)
        message(STATUS "clang-tidy not found.")
    else()
        message(STATUS "found CLANG_TIDY by find_program().")
    endif()
endif()

if(libClaPP_CLANG_TIDY_BIN)
    ### default
    message(STATUS "clang-tidy found: ${libClaPP_CLANG_TIDY_BIN}")
    set(libClaPP_CLANG_TIDY_HEADER_FILTER CACHE STRING ".*/src/include/clapp./*")
    list(APPEND libClaPP_CLANG_TIDY_CHECKS_LIST "*")

    list(APPEND libClaPP_CLANG_TIDY_CHECKS_LIST -modernize-use-trailing-return-type)
    list(APPEND libClaPP_CLANG_TIDY_CHECKS_LIST -fuchsia-default-arguments-calls)
    list(APPEND libClaPP_CLANG_TIDY_CHECKS_LIST -fuchsia-overloaded-operator)
    list(APPEND libClaPP_CLANG_TIDY_CHECKS_LIST -google-runtime-references)

    if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL "11.0.0")
        list(APPEND libClaPP_CLANG_TIDY_CHECKS_LIST -llvmlibc-*)
        list(APPEND libClaPP_CLANG_TIDY_CHECKS_LIST -misc-no-recursion)
    endif()

    if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL "13.0.0")
        list(APPEND libClaPP_CLANG_TIDY_CHECKS_LIST -altera-unroll-loops)
        list(APPEND libClaPP_CLANG_TIDY_CHECKS_LIST -altera-id-dependent-backward-branch)
    endif()

    set(libClaPP_CLANG_TIDY_CHECKS ${libClaPP_CLANG_TIDY_CHECKS_LIST})
    string(REPLACE ";" "," libClaPP_CLANG_TIDY_CHECKS "${libClaPP_CLANG_TIDY_CHECKS}")

    set(libClaPP_CLANG_TIDY_ARGS "${libClaPP_CLANG_TIDY_BIN}" "-header-filter=${libClaPP_CLANG_TIDY_HEADER_FILTER};-warnings-as-errors=*;-checks=${libClaPP_CLANG_TIDY_CHECKS}")

    ### examples
    set(libClaPP_CLANG_TIDY_CHECKS_EXAMPLES_LIST ${libClaPP_CLANG_TIDY_CHECKS_LIST})

    list(APPEND libClaPP_CLANG_TIDY_CHECKS_EXAMPLES_LIST -misc-non-private-member-variables-in-classes)
    list(APPEND libClaPP_CLANG_TIDY_CHECKS_EXAMPLES_LIST -cppcoreguidelines-avoid-magic-numbers)
    list(APPEND libClaPP_CLANG_TIDY_CHECKS_EXAMPLES_LIST -readability-magic-numbers)
    list(APPEND libClaPP_CLANG_TIDY_CHECKS_EXAMPLES_LIST -altera-struct-pack-align)

    set(libClaPP_CLANG_TIDY_CHECKS_EXAMPLES ${libClaPP_CLANG_TIDY_CHECKS_EXAMPLES_LIST})
    string(REPLACE ";" "," libClaPP_CLANG_TIDY_CHECKS_EXAMPLES "${libClaPP_CLANG_TIDY_CHECKS_EXAMPLES}")

    set(libClaPP_CLANG_TIDY_ARGS_EXAMPLES "${libClaPP_CLANG_TIDY_BIN}" "-header-filter=${libClaPP_CLANG_TIDY_HEADER_FILTER};-warnings-as-errors=*;-checks=${libClaPP_CLANG_TIDY_CHECKS_EXAMPLES}")

    ### doc-examples
    set(libClaPP_CLANG_TIDY_ARGS_DOC "${libClaPP_CLANG_TIDY_BIN}" "-header-filter=${libClaPP_CLANG_TIDY_HEADER_FILTER};-warnings-as-errors=*;-checks=${libClaPP_CLANG_TIDY_CHECKS_EXAMPLES}")

    ### gtest
    set(libClaPP_CLANG_TIDY_CHECKS_GTEST_LIST ${libClaPP_CLANG_TIDY_CHECKS_LIST})

    list(APPEND libClaPP_CLANG_TIDY_CHECKS_GTEST_LIST -fuchsia-statically-constructed-objects)
    list(APPEND libClaPP_CLANG_TIDY_CHECKS_GTEST_LIST -modernize-avoid-c-arrays)
    list(APPEND libClaPP_CLANG_TIDY_CHECKS_GTEST_LIST -cppcoreguidelines-avoid-c-arrays)
    list(APPEND libClaPP_CLANG_TIDY_CHECKS_GTEST_LIST -cppcoreguidelines-owning-memory)
    list(APPEND libClaPP_CLANG_TIDY_CHECKS_GTEST_LIST -cppcoreguidelines-avoid-non-const-global-variables)
    list(APPEND libClaPP_CLANG_TIDY_CHECKS_GTEST_LIST -cppcoreguidelines-non-private-member-variables-in-classes)
    list(APPEND libClaPP_CLANG_TIDY_CHECKS_GTEST_LIST -cppcoreguidelines-special-member-functions)
    list(APPEND libClaPP_CLANG_TIDY_CHECKS_GTEST_LIST -cppcoreguidelines-pro-type-vararg)
    list(APPEND libClaPP_CLANG_TIDY_CHECKS_GTEST_LIST -cppcoreguidelines-avoid-goto)
    list(APPEND libClaPP_CLANG_TIDY_CHECKS_GTEST_LIST -cert-err58-cpp)
    list(APPEND libClaPP_CLANG_TIDY_CHECKS_GTEST_LIST -hicpp-avoid-goto)
    list(APPEND libClaPP_CLANG_TIDY_CHECKS_GTEST_LIST -hicpp-avoid-c-arrays)
    list(APPEND libClaPP_CLANG_TIDY_CHECKS_GTEST_LIST -hicpp-vararg)
    list(APPEND libClaPP_CLANG_TIDY_CHECKS_GTEST_LIST -hicpp-special-member-functions)
    list(APPEND libClaPP_CLANG_TIDY_CHECKS_GTEST_LIST -misc-non-private-member-variables-in-classes)

    if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL "12.0.0")
        list(APPEND libClaPP_CLANG_TIDY_CHECKS_GTEST_LIST -readability-function-cognitive-complexity)
    endif()

    set(libClaPP_CLANG_TIDY_CHECKS_GTEST ${libClaPP_CLANG_TIDY_CHECKS_GTEST_LIST})
    string(REPLACE ";" "," libClaPP_CLANG_TIDY_CHECKS_GTEST "${libClaPP_CLANG_TIDY_CHECKS_GTEST}")

    set(libClaPP_CLANG_TIDY_ARGS_GTEST "${libClaPP_CLANG_TIDY_BIN}" "-header-filter=${libClaPP_CLANG_TIDY_HEADER_FILTER};-warnings-as-errors=*;-checks=${libClaPP_CLANG_TIDY_CHECKS_GTEST}")
endif()
