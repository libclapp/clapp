#!/bin/bash

if [ -z "${CLANG_FORMAT_BIN}" ]; then
    if [ -z "${CLANG_COMPILER_VERSION}" ]; then
        echo "ERROR: Please either set the environment variable CLANG_COMPILER_VERSION or CLANG_FORMAT_BIN accordingly"
        exit 1
    fi

    CLANG_FORMAT_BIN="clang-format-${CLANG_COMPILER_VERSION}"
fi

if ! command -v "${CLANG_FORMAT_BIN}" >/dev/null 2>&1; then
    echo "ERROR: Binary '${CLANG_FORMAT_BIN}' does not exist, please either set the environment variable CLANG_FORMAT_BIN or CLANG_COMPILER_VERSION to the correct clang-format version."
    exit 1
fi

format_command="${CLANG_FORMAT_BIN} -i"

apply_format_script="$(dirname "${BASH_SOURCE[0]}")/apply_format.sh"

files_regex=".*\.(c|h)(pp)?$"

if ! "${apply_format_script}" --format-command "${format_command}" --files-regex "${files_regex}" "$@"; then
    echo "ERROR: Executing '${apply_format_script}' failed."
    exit 1
fi
