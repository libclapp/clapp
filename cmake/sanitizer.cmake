if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    set(libClaPP_SANITIZERS "")

    option(libClaPP_ENABLE_SANITIZER_ADDRESS "Address sanitizer" FALSE)
    if(libClaPP_ENABLE_SANITIZER_ADDRESS)
      list(APPEND libClaPP_SANITIZERS "address")
    endif()

    option(libClaPP_ENABLE_SANITIZER_LEAK "Leak sanitizer" FALSE)
    if(libClaPP_ENABLE_SANITIZER_LEAK)
      list(APPEND libClaPP_SANITIZERS "leak")
    endif()

    option(libClaPP_ENABLE_SANITIZER_MEMORY "Memory sanitizer" FALSE)
    if(libClaPP_ENABLE_SANITIZER_MEMORY)
      list(APPEND libClaPP_SANITIZERS "memory")
    endif()

    option(libClaPP_ENABLE_SANITIZER_UNDEFINED_BEHAVIOR "Undefined behavior sanitizer" FALSE)
    if(libClaPP_ENABLE_SANITIZER_UNDEFINED_BEHAVIOR)
      list(APPEND libClaPP_SANITIZERS "undefined")
    endif()

    option(libClaPP_ENABLE_SANITIZER_THREAD "Thread sanitizer" FALSE)
    if(libClaPP_ENABLE_SANITIZER_THREAD)
      list(APPEND libClaPP_SANITIZERS "thread")
    endif()

    set(libClaPP_LIST_OF_SANITIZERS ${libClaPP_SANITIZERS})
    string(REPLACE ";" "," libClaPP_LIST_OF_SANITIZERS "${libClaPP_LIST_OF_SANITIZERS}")
endif()

add_library(libClaPP_SANITIZER INTERFACE)
if(libClaPP_LIST_OF_SANITIZERS)
    message(STATUS "Using the following sanitizers: ${libClaPP_LIST_OF_SANITIZERS}")
    if(NOT "${libClaPP_LIST_OF_SANITIZERS}" STREQUAL "")
        target_compile_options(libClaPP_SANITIZER
                               INTERFACE -fsanitize=${libClaPP_LIST_OF_SANITIZERS})
        target_link_libraries(libClaPP_SANITIZER
                              INTERFACE -fsanitize=${libClaPP_LIST_OF_SANITIZERS})
    endif()
else()
    message(STATUS "Using no sanitizers")
endif()
