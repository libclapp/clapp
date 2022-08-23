#!/usr/bin/env bats

VERSION_REGEX='^libClaPP\ v.[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3} [a-zA-Z]+\-Build: '
CLAPP_EXCEPTION_REGEX='^Caught\ ClaPP-Exception:.*$'

@test "container-example: no arguments/options given throws" {
    run ./libclapp_example_container
    [ "$status" -eq 1 ]
    [[ "${lines[0]}" =~ $CLAPP_EXCEPTION_REGEX ]]
}

@test "container-example: show help with long option --help" {
    run ./libclapp_example_container --help
    [ "$status" -eq 0 ]
    [ "${lines[0]}" = "Usage:" ]
}

@test "container-example: show help with short option -h" {
    run ./libclapp_example_container -h
    [ "$status" -eq 0 ]
    [ "${lines[0]}" = "Usage:" ]
}

@test "container-example: show version with long option --version" {
    run ./libclapp_example_container --version
    [ "$status" -eq 0 ]
    [[ "${lines[0]}" =~ $VERSION_REGEX ]]
}

@test "container-example: give mandatory options and u32" {
    run ./libclapp_example_container --string str --u32 15
    [ "$status" -eq 0 ]
    [ "${lines[0]}" = "verbose: not given" ]
    [ "${lines[1]}" = "string-param: str" ]
    [ "${lines[2]}" = "u32-param: 15" ]
}

@test "container-example: give mandatory options and i32" {
    run ./libclapp_example_container -s string --i32 8
    [ "$status" -eq 0 ]
    [ "${lines[0]}" = "verbose: not given" ]
    [ "${lines[1]}" = "string-param: string" ]
    [ "${lines[2]}" = "i32-param: 8" ]
}

@test "container-example: give mandatory options i32 and verbose" {
    run ./libclapp_example_container -s string --i32 17 --verbose -v
    [ "$status" -eq 0 ]
    [ "${lines[0]}" = "verbose: 2" ]
    [ "${lines[1]}" = "string-param: string" ]
    [ "${lines[2]}" = "i32-param: 17" ]
}

@test "container-example: missing int-options throw" {
    run ./libclapp_example_container -s string
    [ "$status" -eq 1 ]
    [[ "${lines[0]}" =~ $CLAPP_EXCEPTION_REGEX ]]
}

@test "container-example: missing string-option throws" {
    run ./libclapp_example_container --i32 17
    [ "$status" -eq 1 ]
    [[ "${lines[0]}" =~ $CLAPP_EXCEPTION_REGEX ]]
}
