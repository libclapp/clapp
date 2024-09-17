#!/bin/bash

if [ -z "${CMAKE_FORMAT_BIN}" ]; then
    CMAKE_FORMAT_BIN="cmake-format"
fi

if ! command -v "${CMAKE_FORMAT_BIN}" >/dev/null 2>&1; then
    echo "ERROR: Binary '${CMAKE_FORMAT_BIN}' does not exist, please set the environment variable CMAKE_FORMAT_BIN to the correct cmake-format version."
    exit 1
fi

format_command="${CMAKE_FORMAT_BIN} -i"

apply_format_script="$(dirname "${BASH_SOURCE[0]}")/apply_format.sh"

files_regex="(.*\.cmake|CMakeLists.txt)$"

if ! "${apply_format_script}" --format-command "${format_command}" --files-regex "${files_regex}" "$@"; then
    echo "ERROR: Executing '${apply_format_script}' failed." >&2
    exit 1
fi
