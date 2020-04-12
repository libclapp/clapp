#!/usr/bin/env bats

VERSION_REGEX='^libClaPP\ v.[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3} [a-zA-Z]+\-Build: '
CLAPP_EXCEPTION_REGEX='^Caught\ ClaPP-Exception:.*$'

@test "short-example: no arguments/options given throws" {
    run ./libclapp_example_short
    [ "$status" -eq 1 ]
    [[ "${lines[0]}" =~ $CLAPP_EXCEPTION_REGEX ]]
}

@test "short-example: show help with long option --help" {
    run ./libclapp_example_short --help
    [ "$status" -eq 0 ]
    [ "${lines[0]}" = "Usage:" ]
}

@test "short-example: show help with short option -h" {
    run ./libclapp_example_short -h
    [ "$status" -eq 0 ]
    [ "${lines[0]}" = "Usage:" ]
}

@test "short-example: show help with short option -?" {
    run ./libclapp_example_short -?
    [ "$status" -eq 0 ]
    [ "${lines[0]}" = "Usage:" ]
}

@test "short-example: show version with long option --version" {
    run ./libclapp_example_short --version
    [ "$status" -eq 0 ]
    [[ "${lines[0]}" =~ $VERSION_REGEX ]]
}

@test "short-example: show version with long option --vers" {
    run ./libclapp_example_short --vers
    [ "$status" -eq 0 ]
    [[ "${lines[0]}" =~ $VERSION_REGEX ]]
}

@test "short-example: show version with short option -v" {
    run ./libclapp_example_short -v
    [ "$status" -eq 0 ]
    [[ "${lines[0]}" =~ $VERSION_REGEX ]]
}

@test "short-example: give only mandatory arguments fails" {
    run ./libclapp_example_short string 7
    [ "$status" -eq 1 ]
    [[ "${lines[0]}" =~ $CLAPP_EXCEPTION_REGEX ]]
}

@test "short-example: give only mandatory short option -s fails" {
    run ./libclapp_example_short -s option_string
    [ "$status" -eq 1 ]
    [[ "${lines[0]}" =~ $CLAPP_EXCEPTION_REGEX ]]
}

@test "short-example: give only mandatory short option -1 fails" {
    run ./libclapp_example_short -1=option_string
    [ "$status" -eq 1 ]
    [[ "${lines[0]}" =~ $CLAPP_EXCEPTION_REGEX ]]
}

@test "short-example: give only mandatory short option --string fails" {
    run ./libclapp_example_short --string option_string
    [ "$status" -eq 1 ]
    [[ "${lines[0]}" =~ $CLAPP_EXCEPTION_REGEX ]]
}

@test "short-example: give only mandatory arguments and long option succeed" {
    run ./libclapp_example_short string 8 --string opt_string
    [ "$status" -eq 0 ]
    [ "${lines[0]}" = "string-arg: string" ]
    [ "${lines[1]}" = "int-arg: 8" ]
    [ "${lines[2]}" = "variadic-string-arg: not given" ]
    [ "${lines[3]}" = "string_param: 'opt_string'" ]
    [ "${lines[4]}" = "string_vector_param: not given" ]
}

@test "short-example: give only mandatory arguments and short option succeeds" {
    run ./libclapp_example_short -s opt_string_short str 7
    [ "$status" -eq 0 ]
    [ "${lines[0]}" = "string-arg: str" ]
    [ "${lines[1]}" = "int-arg: 7" ]
    [ "${lines[2]}" = "variadic-string-arg: not given" ]
    [ "${lines[3]}" = "string_param: 'opt_string_short'" ]
    [ "${lines[4]}" = "string_vector_param: not given" ]
}

@test "short-example: give only mandatory arguments and short option with variadic argument succeeds" {
    run ./libclapp_example_short s 6 var1 var2 var3 -s opt_string_short
    [ "$status" -eq 0 ]
    [ "${lines[0]}" = "string-arg: s" ]
    [ "${lines[1]}" = "int-arg: 6" ]
    [ "${lines[2]}" = "variadic-string-arg (size: 3): var1, var2, var3, " ]
    [ "${lines[3]}" = "string_param: 'opt_string_short'" ]
    [ "${lines[4]}" = "string_vector_param: not given" ]
}

@test "short-example: give only mandatory arguments and long option with variadic argument succeeds" {
    run ./libclapp_example_short s 6 --string-vector var1 --string-vector var2 --string-vector var3 --string opt_string_long
    [ "$status" -eq 0 ]
    [ "${lines[0]}" = "string-arg: s" ]
    [ "${lines[1]}" = "int-arg: 6" ]
    [ "${lines[2]}" = "variadic-string-arg: not given" ]
    [ "${lines[3]}" = "string_param: 'opt_string_long'" ]
    [ "${lines[4]}" = "string_vector_param (size: 3): var1, var2, var3, " ]
}
