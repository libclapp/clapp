#!/usr/bin/env bats

FIRST_LINE_REGEX='^libClaPP\ v.[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3} [a-zA-Z]+\-Build: '
CLAPP_EXCEPTION_REGEX='^Caught\ ClaPP-Exception:.*$'

@test "large-example: no arguments/options given throws" {
    run ./libclapp_example_large
    [ "$status" -eq 1 ]
    [[ "${lines[0]}" =~ $FIRST_LINE_REGEX ]]
    [[ "${lines[1]}" =~ $CLAPP_EXCEPTION_REGEX ]]
}

@test "large-example: show help with long option --help" {
    run ./libclapp_example_large --help
    [ "$status" -eq 0 ]
    [[ "${lines[0]}" =~ $FIRST_LINE_REGEX ]]
    [ "${lines[1]}" = "Usage:" ]
}

@test "large-example: show help with short option -h" {
    run ./libclapp_example_large -h
    [ "$status" -eq 0 ]
    [[ "${lines[0]}" =~ $FIRST_LINE_REGEX ]]
    [ "${lines[1]}" = "Usage:" ]
}

@test "large-example: give all mandatory options/arguments" {
    run ./libclapp_example_large str -o -m -c -e entry1 --mandatory-int 1 --time-hours 17
    [ "$status" -eq 0 ]
    [[ "${lines[0]}" =~ $FIRST_LINE_REGEX ]]
    [ "${lines[1]}" = "string-arg: str" ]
    [ "${lines[2]}" = "verbose: not given" ]
    [ "${lines[3]}" = "string_param: not given" ]
    [ "${lines[4]}" = "string_vector_param: not given" ]
    [ "${lines[5]}" = "short_bool: not given" ]
    [ "${lines[6]}" = "restricted_bool: not given" ]
    [ "${lines[7]}" = "long_bool: not given" ]
    [ "${lines[8]}" = "count: 1" ]
    [ "${lines[9]}" = "test-file: not given" ]
    [ "${lines[10]}" = "constrained_int: not given" ]
    [ "${lines[11]}" = "mandatory_bool: 1" ]
    [ "${lines[12]}" = "mandatory_int: 1" ]
    [ "${lines[13]}" = "default_int: 10" ]
    [ "${lines[14]}" = "entry_param: 'entry1'" ]
    [ "${lines[15]}" = "time-container: time-hours: 17" ]
    [ "${lines[16]}" = "cmd1 not given" ]
    [ "${lines[17]}" = "cmd2 not given" ]
}

@test "large-example: give mandatory and optional options/arguments" {
    run ./libclapp_example_large str -o -m -c -e entry2 --mandatory-int 3 -b --long-bool -r -v --verbose -s string --string-vector vec1 --string-vector=vec2 --test-file=/tmp --default-int=12 --optional-int 1 -f 15 --time-min 200
    [ "$status" -eq 0 ]
    [[ "${lines[0]}" =~ $FIRST_LINE_REGEX ]]
    [ "${lines[1]}" = "string-arg: str" ]
    [ "${lines[2]}" = "verbose: 4" ]
    [ "${lines[3]}" = "string_param: 'string'" ]
    [ "${lines[4]}" = "string_vector_param (size: 2): vec1, vec2, " ]
    [ "${lines[5]}" = "short_bool: given" ]
    [ "${lines[6]}" = "restricted_bool: given" ]
    [ "${lines[7]}" = "long_bool: 1" ]
    [ "${lines[8]}" = "count: 1" ]
    [ "${lines[9]}" = "test-file: \"/tmp\"" ]
    [ "${lines[10]}" = "constrained_int: 15" ]
    [ "${lines[11]}" = "mandatory_bool: 1" ]
    [ "${lines[12]}" = "mandatory_int: 3" ]
    [ "${lines[13]}" = "default_int: 12" ]
    [ "${lines[14]}" = "optional_int: 1" ]
    [ "${lines[15]}" = "entry_param: 'entry2'" ]
    [ "${lines[16]}" = "time-container: time-minutes: 200" ]
    [ "${lines[17]}" = "cmd1 not given" ]
    [ "${lines[18]}" = "cmd2 not given" ]
}

@test "large-example: give mandatory and optional options/arguments as well as cmd1" {
    run ./libclapp_example_large str -o -m -c -e entry2 --mandatory-int 3 -b --long-bool -r --verbose -s string --string-vector vec1 --string-vector=vec2 --test-file=/tmp --default-int=12 --optional-int 1 -f 15 --time-sec 7 cmd1 -s 1 -b entry1 7
    [ "$status" -eq 0 ]
    [[ "${lines[0]}" =~ $FIRST_LINE_REGEX ]]
    [ "${lines[1]}" = "string-arg: str" ]
    [ "${lines[2]}" = "verbose: 3" ]
    [ "${lines[3]}" = "string_param: 'string'" ]
    [ "${lines[4]}" = "string_vector_param (size: 2): vec1, vec2, " ]
    [ "${lines[5]}" = "short_bool: given" ]
    [ "${lines[6]}" = "restricted_bool: given" ]
    [ "${lines[7]}" = "long_bool: 1" ]
    [ "${lines[8]}" = "count: 1" ]
    [ "${lines[9]}" = "test-file: \"/tmp\"" ]
    [ "${lines[10]}" = "constrained_int: 15" ]
    [ "${lines[11]}" = "mandatory_bool: 1" ]
    [ "${lines[12]}" = "mandatory_int: 3" ]
    [ "${lines[13]}" = "default_int: 12" ]
    [ "${lines[14]}" = "optional_int: 1" ]
    [ "${lines[15]}" = "entry_param: 'entry2'" ]
    [ "${lines[16]}" = "time-container: time-seconds: 7" ]
    [ "${lines[17]}" = "cmd1 given" ]
    [ "${lines[18]}" = "string: 1" ]
    [ "${lines[19]}" = "int-arg: 7" ]
    [ "${lines[20]}" = "string-arg-x: abaa" ]
    [ "${lines[21]}" = "entry-arg: entry1" ]
    [ "${lines[22]}" = "cmd2 not given" ]
}

@test "large-example: give mandatory and optional options/arguments as well as cmd2" {
    run ./libclapp_example_large str -o -m -c -e entry2 --mandatory-int 3 -b --long-bool -r -v --verbose -s string --string-vector vec1 --string-vector=vec2 --test-file=/tmp --default-int=12 --optional-int 1 -f 15 --time-sec 6 cmd2 string1 5 6 7
    [ "$status" -eq 0 ]
    [[ "${lines[0]}" =~ $FIRST_LINE_REGEX ]]
    [ "${lines[1]}" = "string-arg: str" ]
    [ "${lines[2]}" = "verbose: 4" ]
    [ "${lines[3]}" = "string_param: 'string'" ]
    [ "${lines[4]}" = "string_vector_param (size: 2): vec1, vec2, " ]
    [ "${lines[5]}" = "short_bool: given" ]
    [ "${lines[6]}" = "restricted_bool: given" ]
    [ "${lines[7]}" = "long_bool: 1" ]
    [ "${lines[8]}" = "count: 1" ]
    [ "${lines[9]}" = "test-file: \"/tmp\"" ]
    [ "${lines[10]}" = "constrained_int: 15" ]
    [ "${lines[11]}" = "mandatory_bool: 1" ]
    [ "${lines[12]}" = "mandatory_int: 3" ]
    [ "${lines[13]}" = "default_int: 12" ]
    [ "${lines[14]}" = "optional_int: 1" ]
    [ "${lines[15]}" = "entry_param: 'entry2'" ]
    [ "${lines[16]}" = "time-container: time-seconds: 6" ]
    [ "${lines[17]}" = "cmd1 not given" ]
    [ "${lines[18]}" = "cmd2 given" ]
    [ "${lines[19]}" = "short-bool: 0" ]
    [ "${lines[20]}" = "string-arg-x: string1" ]
    [ "${lines[21]}" = "int_arg (size: 3): 5, 6, 7, " ]
}
