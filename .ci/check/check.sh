#!/bin/bash

function usage() {
    local script
    script=$(basename "${BASH_SOURCE[0]}")
    echo "$script [--help] --check-command <CMD> --files-regex <REGEX> [--exclude-files-regex <REGEX>]"
}

files_regex=""
exclude_files_regex="^$"
while [[ "${#}" -gt 0 ]]; do
    case "${1}" in
    --exclude-files-regex)
        exclude_files_regex="${2}"
        shift
        ;;
    --files-regex)
        files_regex="${2}"
        shift
        ;;
    --check-command)
        check_command="${2}"
        shift
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

if [ -z "${check_command}" ]; then
    echo "ERROR: Please give the check_command!"
    usage
    exit 1
fi

if [ -z "${files_regex}" ]; then
    echo "ERROR: Please give a files regex!"
    usage
    exit 1
fi

echo "INFO: Searching for files matching '${files_regex}' in the git repository and exclude all matching '${exclude_files_regex}'"
if ! files=$(git ls-files | grep -E "${files_regex}"); then
    echo "ERROR: Could not find any files matching '${files_regex}' in the git repository!"
    exit 1
fi

mapfile -t files < <(echo "${files}" | grep -Ev "${exclude_files_regex}")

echo "INFO: Using command '${check_command}' to check the folowing files:" "${files[@]}"
if ! ${check_command} "${files[@]}"; then
    echo "ERROR: Some issues found in files!"
    exit 1
fi
