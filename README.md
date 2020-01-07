      _____  _        _____   _____  
     / ____|| | __ _ |  __ \ |  __ \ 
    | |     | |/ _` || |__) || |__) |
    | |     |_|\__,_||  ___/ |  ___/ 
    | |____          | |     | |     
     \_____|         |_|     |_|     


libClaPP:
=========

libClaPP is an open source command line argument processing library for C++.
It supports the processing of GNU options (long and short options) as well as positional arguments.

Build:
------

### Dependencies:
libClaPP only depends on the [GSL](https://github.com/microsoft/GSL).
If you want to build the unit tests, also [google test](https://github.com/google/googletest) is required.

### Build the library
Since all dependencies are already included in this repository as submodules, you may prefer to use the shipped dependencies via the CMake option `libClaPP_SUBMODULE_DEPENDENCIES` enabled:

    git clone --recurse-submodules https://git.libclapp.org/libclapp/clapp.git
    mkdir build
    cd build
    cmake .. -DlibClaPP_SUBMODULE_DEPENDENCIES=On
    make

But if all dependencies are installed on your system, the following steps are sufficient.

    git clone https://git.libclapp.org/libclapp/clapp.git
    mkdir build
    cd build
    cmake ..
    make

### Install the library

To install the library on your system, type the following command.

    make install

To uninstall the library, remove all files that were installed:

    xargs rm < install_manifest.txt

Terminology:
-------------
In this project the following terms are used:

* Option: An option can be either a short option or a long option. 
    A short option consists of a single `-` followed by single character (e.g. `-h`).
    A long option consists of two `--` followed by an arbitrary string (e.g. `--help`).
* Parameter: Some options may require additional parameters (.e.g. `--file=/path/to/file`).
* Argument: An argument is the short form for positional argument. 
    Thus, arguments higly depend on the position where they occur. 
    Unlinke to options that may occur anywhere else.
* Sub-Parser: A subparser is always implicitly the last argument of a the current parser.
    Once this argument is given, a completely new parsing instance is started.

Example:
--------
Some examples can be found in the [examples](examples) folder.

Coverage:
---------

    cmake -DlibClaPP_BUILD_COVERAGE=On -DCMAKE_BUILD_TYPE=Debug -DlibClaPP_SUBMODULE_DEPENDENCIES=On -DlibClaPP_BUILD_TESTS=On ..
    gcovr -r ../ -e ../third_party/ -e ../tests/ -e ../examples/
