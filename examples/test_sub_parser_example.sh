#!/usr/bin/env bats

VERSION_REGEX='^libClaPP\ v.[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3} [a-zA-Z]+\-Build: '
CLAPP_EXCEPTION_REGEX='^Caught\ ClaPP-Exception:.*$'

@test "sub-parser-example: no arguments/options given throws" {
    run ./libclapp_example_sub_parser
    [ "$status" -eq 0 ]
    [ "${lines[0]}" = "verbose: 2" ]
    [ "${lines[1]}" = "first parser not active" ]
    [ "${lines[2]}" = "second parser not active" ]
}

@test "sub-parser-example: show help with long option --help" {
    run ./libclapp_example_sub_parser --help
    [ "$status" -eq 0 ]
    [ "${lines[0]}" = "Usage:" ]
}

@test "sub-parser-example: show help with short option -h" {
    run ./libclapp_example_sub_parser -h
    [ "$status" -eq 0 ]
    [ "${lines[0]}" = "Usage:" ]
}

@test "sub-parser-example: show version with long option --version" {
    run ./libclapp_example_sub_parser --version
    [ "$status" -eq 0 ]
    [[ "${lines[0]}" =~ $VERSION_REGEX ]]
}

@test "sub-parser-example: give verbose options multiple times" {
    run ./libclapp_example_sub_parser --verbose -v
    [ "$status" -eq 0 ]
    [ "${lines[0]}" = "verbose: 4" ]
    [ "${lines[1]}" = "first parser not active" ]
    [ "${lines[2]}" = "second parser not active" ]
}

@test "sub-parser-example: first: give only mandatory short option -b" {
    run ./libclapp_example_sub_parser first -b
    [ "$status" -eq 0 ]
    [ "${lines[0]}" = "verbose: 2" ]
    [ "${lines[1]}" = "first parser active" ]
    [ "${lines[2]}" = "short_bool: 1" ]
    [ "${lines[3]}" = "string: not given" ]
    [ "${lines[4]}" = "string-arg-x: abaa" ]
    [ "${lines[5]}" = "double-opt: 10" ]
    [ "${lines[6]}" = "second parser not active" ]
}

@test "sub-parser-example: first: give only mandatory short option -b and optional option -d" {
    run ./libclapp_example_sub_parser first -b -d 11
    [ "$status" -eq 0 ]
    [ "${lines[0]}" = "verbose: 2" ]
    [ "${lines[1]}" = "first parser active" ]
    [ "${lines[2]}" = "short_bool: 1" ]
    [ "${lines[3]}" = "string: not given" ]
    [ "${lines[4]}" = "string-arg-x: abaa" ]
    [ "${lines[5]}" = "double-opt: 11" ]
    [ "${lines[6]}" = "second parser not active" ]
}

@test "sub-parser-example: first: give only mandatory short option -b and optional option -s" {
    run ./libclapp_example_sub_parser first -b -s string
    [ "$status" -eq 0 ]
    [ "${lines[0]}" = "verbose: 2" ]
    [ "${lines[1]}" = "first parser active" ]
    [ "${lines[2]}" = "short_bool: 1" ]
    [ "${lines[3]}" = "string: string" ]
    [ "${lines[4]}" = "string-arg-x: abaa" ]
    [ "${lines[5]}" = "double-opt: 10" ]
    [ "${lines[6]}" = "second parser not active" ]
}

@test "sub-parser-example: second: give only mandatory args" {
    run ./libclapp_example_sub_parser second 9 121
    [ "$status" -eq 0 ]
    [ "${lines[0]}" = "verbose: 2" ]
    [ "${lines[1]}" = "first parser not active" ]
    [ "${lines[2]}" = "second parser active" ]
    [ "${lines[3]}" = "int-arg: 9" ]
    [ "${lines[4]}" = "double-arg: 121" ]
    [ "${lines[5]}" = "string-arg-x: default-string-arg-x" ]
    [ "${lines[6]}" = "variadic-int-arg: not given" ]
}
@test "sub-parser-example: second: give only mandatory args and optional args" {
    run ./libclapp_example_sub_parser second 9 121 string-arg 6 7
    [ "$status" -eq 0 ]
    [ "${lines[0]}" = "verbose: 2" ]
    [ "${lines[1]}" = "first parser not active" ]
    [ "${lines[2]}" = "second parser active" ]
    [ "${lines[3]}" = "int-arg: 9" ]
    [ "${lines[4]}" = "double-arg: 121" ]
    [ "${lines[5]}" = "string-arg-x: string-arg" ]
    [ "${lines[6]}" = "variadic-int-arg (size: 2): 6, 7, " ]
}
