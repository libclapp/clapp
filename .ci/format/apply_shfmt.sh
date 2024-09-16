#!/bin/bash

if [ -z "${SHFMT_BIN}" ]; then
    SHFMT_BIN="shfmt"
fi

if ! command -v "${SHFMT_BIN}" >/dev/null 2>&1; then
    echo "ERROR: Binary '${SHFMT_BIN}' does not exist, please set the environment variable SHFMT_BIN to the correct shfmt version."
    exit 1
fi

format_command="${SHFMT_BIN} -w -i 4"

apply_format_script="$(dirname "${BASH_SOURCE[0]}")/apply_format.sh"

files_regex=".*\.sh$"

if ! "${apply_format_script}" --format-command "${format_command}" --files-regex "${files_regex}" "$@"; then
    echo "ERROR: Executing '${apply_format_script}' failed."
    exit 1
fi
