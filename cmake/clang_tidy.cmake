if(NOT libClaPP_CLANG_TIDY_BIN)
    find_program(
        libClaPP_CLANG_TIDY_BIN
        NAMES "clang-tidy"
        DOC "Path to clang-tidy executable")

    if(NOT libClaPP_CLANG_TIDY_BIN)
        message(STATUS "clang-tidy not found.")
    else()
        message(STATUS "found CLANG_TIDY by find_program().")
    endif()
endif()

if(libClaPP_CLANG_TIDY_BIN)
    # default
    message(STATUS "clang-tidy found: ${libClaPP_CLANG_TIDY_BIN}")
    set(libClaPP_CLANG_TIDY_HEADER_FILTER
        ".*/(include/clapp/.*\.h|include/clapp/.*\.hpp|tests.*\.h)\$"
        CACHE STRING "Header-Filter used for clang-tidy")
    list(APPEND libClaPP_CLANG_TIDY_CHECKS_LIST "*")

    list(APPEND libClaPP_CLANG_TIDY_CHECKS_LIST
         -modernize-use-trailing-return-type)
    list(APPEND libClaPP_CLANG_TIDY_CHECKS_LIST
         -fuchsia-default-arguments-calls)
    list(APPEND libClaPP_CLANG_TIDY_CHECKS_LIST -fuchsia-overloaded-operator)
    list(APPEND libClaPP_CLANG_TIDY_CHECKS_LIST -google-runtime-references)
    list(APPEND libClaPP_CLANG_TIDY_CHECKS_LIST
         -bugprone-easily-swappable-parameters)
    list(APPEND libClaPP_CLANG_TIDY_CHECKS_LIST -altera-struct-pack-align)
    list(APPEND libClaPP_CLANG_TIDY_CHECKS_LIST
         -misc-non-private-member-variables-in-classes)
    list(APPEND libClaPP_CLANG_TIDY_CHECKS_LIST
         -cppcoreguidelines-non-private-member-variables-in-classes)
    list(APPEND libClaPP_CLANG_TIDY_CHECKS_LIST -llvm-include-order)
    list(APPEND libClaPP_CLANG_TIDY_CHECKS_LIST
         -cppcoreguidelines-avoid-const-or-ref-data-members)
    list(APPEND libClaPP_CLANG_TIDY_CHECKS_LIST -llvmlibc-*)
    list(APPEND libClaPP_CLANG_TIDY_CHECKS_LIST -misc-no-recursion)
    list(APPEND libClaPP_CLANG_TIDY_CHECKS_LIST -misc-include-cleaner)
    list(APPEND libClaPP_CLANG_TIDY_CHECKS_LIST
         -cppcoreguidelines-avoid-do-while)
    list(APPEND libClaPP_CLANG_TIDY_CHECKS_LIST -altera-unroll-loops)
    list(APPEND libClaPP_CLANG_TIDY_CHECKS_LIST
         -altera-id-dependent-backward-branch)
    list(APPEND libClaPP_CLANG_TIDY_CHECKS_LIST
         -bugprone-unchecked-optional-access)
    list(APPEND libClaPP_CLANG_TIDY_CHECKS_LIST
         -cppcoreguidelines-missing-std-forward)
    list(APPEND libClaPP_CLANG_TIDY_CHECKS_LIST -misc-const-correctness)
    list(APPEND libClaPP_CLANG_TIDY_CHECKS_LIST
         -readability-function-cognitive-complexity)

    set(libClaPP_CLANG_TIDY_CHECKS ${libClaPP_CLANG_TIDY_CHECKS_LIST})
    string(REPLACE ";" "," libClaPP_CLANG_TIDY_CHECKS
                   "${libClaPP_CLANG_TIDY_CHECKS}")

    set(libClaPP_CLANG_TIDY_ARGS
        "${libClaPP_CLANG_TIDY_BIN}"
        "-header-filter=${libClaPP_CLANG_TIDY_HEADER_FILTER};-warnings-as-errors=*;-checks=${libClaPP_CLANG_TIDY_CHECKS}"
    )

    # examples
    set(libClaPP_CLANG_TIDY_CHECKS_EXAMPLES_LIST
        ${libClaPP_CLANG_TIDY_CHECKS_LIST})

    list(APPEND libClaPP_CLANG_TIDY_CHECKS_EXAMPLES_LIST
         -cppcoreguidelines-avoid-magic-numbers)
    list(APPEND libClaPP_CLANG_TIDY_CHECKS_EXAMPLES_LIST
         -readability-magic-numbers)

    set(libClaPP_CLANG_TIDY_CHECKS_EXAMPLES
        ${libClaPP_CLANG_TIDY_CHECKS_EXAMPLES_LIST})
    string(REPLACE ";" "," libClaPP_CLANG_TIDY_CHECKS_EXAMPLES
                   "${libClaPP_CLANG_TIDY_CHECKS_EXAMPLES}")

    set(libClaPP_CLANG_TIDY_ARGS_EXAMPLES
        "${libClaPP_CLANG_TIDY_BIN}"
        "-header-filter=${libClaPP_CLANG_TIDY_HEADER_FILTER};-warnings-as-errors=*;-checks=${libClaPP_CLANG_TIDY_CHECKS_EXAMPLES}"
    )

    # doc-examples
    set(libClaPP_CLANG_TIDY_ARGS_DOC
        "${libClaPP_CLANG_TIDY_BIN}"
        "-header-filter=${libClaPP_CLANG_TIDY_HEADER_FILTER};-warnings-as-errors=*;-checks=${libClaPP_CLANG_TIDY_CHECKS_EXAMPLES}"
    )

    # gtest
    set(libClaPP_CLANG_TIDY_CHECKS_GTEST_LIST
        ${libClaPP_CLANG_TIDY_CHECKS_LIST})

    list(APPEND libClaPP_CLANG_TIDY_CHECKS_GTEST_LIST
         -fuchsia-statically-constructed-objects)
    list(APPEND libClaPP_CLANG_TIDY_CHECKS_GTEST_LIST -modernize-avoid-c-arrays)
    list(APPEND libClaPP_CLANG_TIDY_CHECKS_GTEST_LIST
         -cppcoreguidelines-avoid-c-arrays)
    list(APPEND libClaPP_CLANG_TIDY_CHECKS_GTEST_LIST
         -cppcoreguidelines-owning-memory)
    list(APPEND libClaPP_CLANG_TIDY_CHECKS_GTEST_LIST
         -cppcoreguidelines-avoid-non-const-global-variables)
    list(APPEND libClaPP_CLANG_TIDY_CHECKS_GTEST_LIST
         -cppcoreguidelines-special-member-functions)
    list(APPEND libClaPP_CLANG_TIDY_CHECKS_GTEST_LIST
         -cppcoreguidelines-pro-type-vararg)
    list(APPEND libClaPP_CLANG_TIDY_CHECKS_GTEST_LIST
         -cppcoreguidelines-avoid-goto)
    list(APPEND libClaPP_CLANG_TIDY_CHECKS_GTEST_LIST -cert-err58-cpp)
    list(APPEND libClaPP_CLANG_TIDY_CHECKS_GTEST_LIST -hicpp-avoid-goto)
    list(APPEND libClaPP_CLANG_TIDY_CHECKS_GTEST_LIST -hicpp-avoid-c-arrays)
    list(APPEND libClaPP_CLANG_TIDY_CHECKS_GTEST_LIST -hicpp-vararg)
    list(APPEND libClaPP_CLANG_TIDY_CHECKS_GTEST_LIST
         -hicpp-special-member-functions)
    list(APPEND libClaPP_CLANG_TIDY_CHECKS_GTEST_LIST -llvm-header-guard)

    set(libClaPP_CLANG_TIDY_CHECKS_GTEST
        ${libClaPP_CLANG_TIDY_CHECKS_GTEST_LIST})
    string(REPLACE ";" "," libClaPP_CLANG_TIDY_CHECKS_GTEST
                   "${libClaPP_CLANG_TIDY_CHECKS_GTEST}")

    set(libClaPP_CLANG_TIDY_ARGS_GTEST
        "${libClaPP_CLANG_TIDY_BIN}"
        "-header-filter=${libClaPP_CLANG_TIDY_HEADER_FILTER};-warnings-as-errors=*;-checks=${libClaPP_CLANG_TIDY_CHECKS_GTEST}"
    )
endif()
