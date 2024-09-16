#!/bin/bash

if [ -z "${SHELLCHECK_BIN}" ]; then
    SHELLCHECK_BIN="shellcheck"
fi

if ! command -v "${SHELLCHECK_BIN}" >/dev/null 2>&1; then
    echo "ERROR: Binary '${SHELLCHECK_BIN}' does not exist, please set the environment variable SHELLCHECK_BIN to the correct shellcheck version."
    exit 1
fi

check_command="${SHELLCHECK_BIN}"

check_script="$(dirname "${BASH_SOURCE[0]}")/check.sh"

files_regex=".*\.sh$"

if ! "${check_script}" --check-command "${check_command}" --files-regex "${files_regex}" "$@"; then
    echo "ERROR: Executing '${check_script}' failed."
    exit 1
fi
