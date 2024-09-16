#!/bin/bash

function usage() {
    local script
    script=$(basename "${BASH_SOURCE[0]}")
    echo "$script [--help] --compiler clang|gcc --compiler-version <COMPILER-VERSION> [--source-dir <SOURCE-DIR>] [--build-dir <BUILD-DIR>] [--build-type <CMAKE-BUILD-TYPE>] [--generator <CMAKE-GENERATOR>] [--clang-tidy <CLANG-TIDY-BIN>] [--skip-doc] [--skip-examples] [--skip-tests] [--coverage] [--pedantic] [--verbose] [--parallel] [--install] [--address-sanitizer] [--memory-sanitizer] [--thread-sanitizer] [--undefined-behavior-sanitizer]"
}

coverage="Off"
pedantic="Off"
clang_tidy="Off"
verbose="Off"
sanitizer=()
build_type="Debug"
generator="Unix Makefiles"
build_dir=""
source_dir=""
parallel=""
install="0"
skip_tests="0"
skip_examples="0"
skip_doc="0"
while [[ "${#}" -gt 0 ]]; do
    case "${1}" in
    --compiler)
        compiler="${2}"
        shift
        ;;
    --compiler-version)
        compiler_version="${2}"
        shift
        ;;
    --source-dir)
        source_dir="${2}"
        shift
        ;;
    --build-dir)
        build_dir="${2}"
        shift
        ;;
    --build-type)
        build_type="${2}"
        shift
        ;;
    --generator)
        generator="${2}"
        shift
        ;;
    --parallel)
        parallel="${2}"
        shift
        ;;
    --clang-tidy)
        clang_tidy="On"
        clang_tidy_bin="${2}"
        shift
        ;;
    --coverage)
        coverage="On"
        ;;
    --pedantic)
        pedantic="On"
        ;;
    --verbose)
        verbose="On"
        ;;
    --install)
        install="1"
        ;;
    --skip-tests)
        skip_tests="1"
        ;;
    --skip-examples)
        skip_examples="1"
        ;;
    --skip-doc)
        skip_doc="1"
        ;;
    --address-sanitizer)
        sanitizer+=("ADDRESS")
        ;;
    --memory-sanitizer)
        sanitizer+=("MEMORY")
        ;;
    --thread-sanitizer)
        sanitizer+=("THREAD")
        ;;
    --undefined-behavior-sanitizer)
        sanitizer+=("UNDEFINED_BEHAVIOR")
        ;;
    --help)
        usage
        exit 0
        ;;
    -?*)
        echo "ERROR: Invalid argument: $1"
        usage
        exit 1
        ;;
    esac
    shift
done

if [ -z "${compiler}" ]; then
    echo "ERROR: Please give the compiler!"
    usage
    exit 1
fi

if ! [[ "${compiler}" =~ ^(clang|gcc)$ ]]; then
    echo "ERROR: Please give a supported compiler: Currently only clang and gcc are supported!"
    usage
    exit 1
fi

if [ -z "${compiler_version}" ]; then
    echo "ERROR: Please give the compiler version!"
    usage
    exit 1
fi

export CC="${compiler}-${compiler_version}"
if [ "${compiler}" = "gcc" ]; then
    export CXX="g++-${compiler_version}"
elif [ "${compiler}" = "clang" ]; then
    export CXX="${compiler}++-${compiler_version}"
fi

if [ -z "${source_dir}" ]; then
    source_dir=$(realpath "$(dirname "${BASH_SOURCE[0]}")/../..")
fi

if [ -z "${build_dir}" ]; then
    build_dir="${source_dir}/build_${compiler}-${compiler_version}_${build_type}_${generator/ /_}"
fi

echo "INFO: Trying to create a build with ${CXX} (and ${CC}) from ${source_dir} in ${build_dir}: (Build-Type: ${build_type}, Generator: ${generator}, Clang-Tidy: ${clang_tidy}, Coverage: ${coverage}, Pedantic: ${pedantic}, Sanitizer: (" "${sanitizer[@]}" "))!"

cmake_configure_options=()
cmake_configure_options+=("-DCMAKE_VERBOSE_MAKEFILE:BOOL=${verbose}")
cmake_configure_options+=("-DCMAKE_BUILD_TYPE:STRING=${build_type}")
if [ "${skip_tests}" != "1" ]; then
    cmake_configure_options+=("-DlibClaPP_BUILD_TESTS:BOOL=On")
fi
if [ "${skip_examples}" != "1" ]; then
    cmake_configure_options+=("-DlibClaPP_BUILD_EXAMPLES:BOOL=On")
fi
if [ "${skip_doc}" != "1" ]; then
    cmake_configure_options+=("-DlibClaPP_BUILD_DOC_CODE:BOOL=On")
fi
cmake_configure_options+=("-DlibClaPP_SUBMODULE_DEPENDENCIES:BOOL=On")
cmake_configure_options+=("-DlibClaPP_CLANG_TIDY:BOOL=${clang_tidy}")
if [ -n "${clang_tidy_bin}" ]; then
    cmake_configure_options+=("-DlibClaPP_CLANG_TIDY_BIN:STRING=${clang_tidy_bin}")
fi
cmake_configure_options+=("-DlibClaPP_BUILD_COVERAGE:BOOL=${coverage}")
cmake_configure_options+=("-DlibClaPP_PEDANTIC_COMPILER_FLAGS:BOOL=${pedantic}")
for sani in "${sanitizer[@]}"; do
    cmake_configure_options+=("-DlibClaPP_ENABLE_SANITIZER_${sani}:BOOL=On")
done

if ! cmake "${cmake_configure_options[@]}" -G "${generator}" -B "${build_dir}" -S "${source_dir}"; then
    echo "ERROR: CMake configure step failed"
    exit 1
fi

cmake_build_options=()
if [ -n "${parallel}" ]; then
    cmake_build_options+=("-j${paralel}")
fi
if [ "${install}" = "1" ]; then
    cmake_build_options+=("--target" "install")
fi
if ! cmake --build "${build_dir}" "${cmake_build_options[@]}"; then
    echo "ERROR: CMake build step failed"
    exit 1
fi
