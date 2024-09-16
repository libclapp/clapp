#!/bin/bash

function usage() {
    local script
    script=$(basename "${BASH_SOURCE[0]}")
    echo "$script [--help] --format-command <CMD> --files-regex <REGEX> [--output-patch-file <FILE>] [--exclude-files-regex <REGEX>]"
}

files_regex=""
output_patch_file="/dev/null"
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
    --output-patch-file)
        output_patch_file="${2}"
        shift
        ;;
    --format-command)
        format_command="${2}"
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

if [ -z "${format_command}" ]; then
    echo "ERROR: Please give the format_command!"
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

echo "INFO: Using command '${format_command}' to format the folowing files:" "${files[@]}"
if ! ${format_command} "${files[@]}"; then
    echo "ERROR: Could not execute command ${format_command}!"
    exit 1
fi

if ! git diff --exit-code HEAD -- "${files[@]}" >"${output_patch_file}"; then
    if [ "${output_patch_file}" != "/dev/null" ]; then
        echo "ERROR: Formatting issues found in files. Apply patch '${output_patch_file}' to resolve!"
        cat "${output_patch_file}"
        exit 1
    else
        echo "ERROR: Formatting issues found in files. They are applied inplace."
        exit 1
    fi
fi
