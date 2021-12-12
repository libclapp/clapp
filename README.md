      _____  _        _____   _____
     / ____|| | __ _ |  __ \ |  __ \
    | |     | |/ _` || |__) || |__) |
    | |     |_|\__,_||  ___/ |  ___/
    | |____          | |     | |
     \_____|         |_|     |_|


libClaPP:
=========
[![pipeline status](https://git.libclapp.org/libclapp/clapp/badges/master/pipeline.svg)](https://git.libclapp.org/libclapp/clapp/-/commits/master)
[![coverage report](https://git.libclapp.org/libclapp/clapp/badges/master/coverage.svg)](https://git.libclapp.org/libclapp/clapp/-/commits/master)

libClaPP is an open source command line argument processing library for C++.
It supports the processing of GNU options (long and short options) as well as positional arguments.
It is a strongly typed library that can be used to parse the command line arguments into the correct type,
validate the arguments using custom constraints and automatically generates a help message with respect
to the types and the constraints.

Build:
------

### Dependencies:
Besides the dependency to CMake, the library `libClaPP` only depends on the [GSL](https://github.com/microsoft/GSL). Note: this repository ships with an appropriate version of GSL as git-submodule.

If the unit tests of this libray should be built too (can be selected by the CMake option `-DlibClaPP_BUILD_TESTS=On`), also [google test](https://github.com/google/googletest) is required (is also shipped as git-submodule dependency).

If the listings in the [doc/](doc/)-folder should be extracted and build too, a Raku-(Perl 6)-interpreter is required. A very common Raku interpreter is [Rakudo](https://rakudo.org/), which can be installed on Debian/Ubuntu by executing `apt install rakudo`. Just give the CMake option `-DlibClaPP_BUILD_DOC_CODE=On` to build the code in the documentation.

### Build the library:
Since all build dependencies are already included in this repository as submodules, you may prefer to use the shipped dependencies via the CMake option `libClaPP_SUBMODULE_DEPENDENCIES` enabled:

    git clone --recurse-submodules https://git.libclapp.org/libclapp/clapp.git
    mkdir build
    cd build
    cmake .. -DlibClaPP_SUBMODULE_DEPENDENCIES=On
    cmake --build .

#### Build with system libraries:
But if all dependencies are installed on your system, the following steps are sufficient.

    git clone https://git.libclapp.org/libclapp/clapp.git
    mkdir build
    cd build
    cmake ..
    cmake --build .

#### Build the library in debug mode with submodules, unitests and check code coverage:
Set `GCOV_EXECUTABLE` accordingly (i.e. `llvm-cov-11 gcov`, `gcov-10`)

    git clone --recurse-submodules https://git.libclapp.org/libclapp/clapp.git
    mkdir build
    cd build
    cmake -DlibClaPP_BUILD_TESTS=On -DlibClaPP_BUILD_COVERAGE=On -DlibClaPP_SUBMODULE_DEPENDENCIES=On -DCMAKE_BUILD_TYPE=Debug ..
    cmake --build .
    ctest -V -R libclapp_tests
    gcovr -r ../ -e ../third_party/ -e ../tests/ -e ../examples/ -e doc/ -e ../doc/ --gcov-executable="${GCOV_EXECUTABLE}"

#### Build the library in debug mode with submodules, unitests, examples and check code coverage:

Set `GCOV_EXECUTABLE` accordingly (i.e. `llvm-cov-11 gcov`, `gcov-10`)

    git clone --recurse-submodules https://git.libclapp.org/libclapp/clapp.git
    mkdir build
    cd build
    cmake -DlibClaPP_BUILD_TESTS=On -DlibClaPP_BUILD_EXAMPLES=On -DlibClaPP_BUILD_COVERAGE=On -DlibClaPP_SUBMODULE_DEPENDENCIES=On -DCMAKE_BUILD_TYPE=Debug ..
    cmake --build .
    ctest -V -R libclapp_tests
    gcovr -r ../ -e ../third_party/ -e ../tests/ -e ../examples/ -e doc/ -e ../doc/ --gcov-executable="${GCOV_EXECUTABLE}"
    ctest -V -E libclapp_tests

#### Build with gcc/clang sanitizers:
LibClaPP can be built with the following GCC/Clang-sanitizers: Address sanitizer, memory sanitizer, undefined behavior sanitizer or a thread sanitizer.
To enable these sanitizers, one of the following CMake-options must be enabled (Note: depending on the actual compiler, it may or may not possible to combine different sanitizers): 
- `libClaPP_ENABLE_SANITIZER_ADDRESS`
- `libClaPP_ENABLE_SANITIZER_MEMORY`
- `libClaPP_ENABLE_SANITIZER_UNDEFINED_BEHAVIOR`
- `libClaPP_ENABLE_SANITIZER_THREAD`

For easy switching between different sanitizers, the tool `ccmake` may be used instead of pure `cmake`.

### Install the library

To install the library on your system, add `--target install` to the `cmake --build` command above (i.e. `cmake --build . --target install`).

To uninstall the library, remove all files that were installed:

    xargs rm < install_manifest.txt

Documentation:
--------------
A short documentation of libClaPP is available in [doc/doc.md](doc/doc.md).

Examples:
---------
Some examples can be found in the [examples](examples) folder.
Additionally, [doc/doc.md](doc/doc.md) also contains a few examples.

For a quick start take a look at [examples/short_example.cpp](examples/short_example.cpp).

Usage:
------
Some examples how libClaPP can be used (CMake-subdir, CMake-package, CMake with `pkgconfig`, `make` with `pkgconfig`) can be found in the [usage](usage) folder.

Clang-Tidy:
-----------

    CXX=clang++-11 CC=clang-11 cmake -DCLANG_TIDY=clang-tidy-11 -DlibClaPP_BUILD_TESTS=On -DlibClaPP_BUILD_EXAMPLES=On -DlibClaPP_SUBMODULE_DEPENDENCIES=On -DlibClaPP_BUILD_COVERAGE=On -DCMAKE_BUILD_TYPE=Debug -DlibClaPP_CLANG_TIDY=On ..

Clang-Format:
-------------

This project uses a clang-format style based on the Google style (`clang-format-11 -style=Google -dump-config`) but modified a bit (see [.clang-format](.clang-format)).

To format the whole project, use the following command:

    find src tests examples usage -iname *.h -o -iname *.cpp -o -iname *.c -o -iname *.hpp | xargs clang-format-11 -i
