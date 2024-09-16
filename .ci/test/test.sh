#!/bin/bash

function usage() {
    local script
    script=$(basename "${BASH_SOURCE[0]}")
    echo "$script [--help] --build-dir <BUILD_DIR> [--gcov-bin <GCOV-BIN>]"
}

source_dir=$(realpath "$(dirname "${BASH_SOURCE[0]}")/../..")
xml_report="${source_dir}/xml/coverage.xml"
html_report="${source_dir}/html/index.html"

build_dir=""
gcov_bin=""
while [[ "${#}" -gt 0 ]]; do
    case "${1}" in
    --build-dir)
        build_dir="${2}"
        shift
        ;;
    --gcov-bin)
        gcov_bin="${2}"
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

if [ -z "${build_dir}" ]; then
    echo "ERROR: Please give the build-dir!"
    usage
    exit 1
fi

test_dirs=()
while IFS= read -r -d '' dir; do
    test_dirs+=("${dir}")
done < <(find . -maxdepth 1 -type d -name "${build_dir}" -print0)

failed=0
for dir in "${test_dirs[@]}"; do
    echo "INFO: Running tests in '${dir}' (matching 'libclapp_tests'):"
    if ! ctest --test-dir "${dir}" -R "libclapp_tests"; then
        failed=1
    fi
done

if [ "${failed}" = "1" ]; then
    echo "ERROR: CTest failed"
    exit 1
fi

if [ -n "${gcov_bin}" ]; then
    if ! mkdir -p "$(dirname "${xml_report}")"; then
        echo "ERROR: Could not create dir '${xml_report}'"
        exit 1
    fi

    if ! mkdir -p "$(dirname "${html_report}")"; then
        echo "ERROR: Could not create dir '${html_report}'"
        exit 1
    fi

    if ! gcovr \
        --root "${source_dir}" \
        --gcov-executable="${gcov_bin}" \
        --html="${html_report}" \
        --html-details \
        --xml="${xml_report}" \
        --xml-pretty \
        --print-summary; then
        echo "ERROR: Running gcovr failed"
        exit 1
    fi

    if ! sed --in-place \
        --expression="s;<source>${source_dir}</source>;<source>.</source>;g" \
        "${xml_report}"; then
        echo "ERROR: Patching xml-report failed"
        exit 1
    fi
fi

failed=0
for dir in "${test_dirs[@]}"; do
    echo "INFO: Running tests in '${dir}' (not matching 'libclapp_tests'):"
    if ! ctest --test-dir "${dir}" -E "libclapp_tests"; then
        failed=1
    fi
done
