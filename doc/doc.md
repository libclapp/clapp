Introduction:
=============

Table of Contents:
------------------
[[_TOC_]]

Terminology:
-------------
In this project the following terms are used:

* Option: An option can be either a short option or a long option.
    A short option consists of a single `-` followed by single character (e.g. `-h`).
    A long option consists of two `--` followed by an arbitrary string (e.g. `--help`).
* Parameter: Some options may require additional parameters (.e.g. `--file=/path/to/file`).
* Argument: An argument is the short form for positional argument.
    Thus, arguments higly depend on the position where they occur.
    Unlinke to options that may occur anywhere else.
* Sub-Parser: A subparser can be triggered by a particular argument name. Once the current parser detects an argument name of a sub-parser, the current parser hands over the parsing-task to the corresponding sub-parser.
Thus, this sup-parser argument is always implicitly the last argument handled by the current parser. (e.g. the commands `git commit -f` or `git log` can be implemented by sub-parsers, where the sub-parser for `commit` must at least be capable of the option  `-f` and the sub-parser for `log` does not get any further options or arguments at all)

Outlook:
--------
This document provides an overview of the features of libClaPP.

The first section starts with the individual value types (for options and arguments). 
This includes an introduction to the supported CLI-options and CLI-arguments.
Also, a complete collection of shipped types is given. Including a description, how
custom types can be used.
Finally, additional options for options, arguments or their parameters are described (containing
constraints or additional help information).

The next section concludes with the different parser types: main-parser and sub-parser.
This section also contains some complete examples that show, how a ClaPP-based parser looks like.

Examples:
---------
This document already contains some short examples that show the basic usage of this library.
However, some further examples can be found in the [../examples](../examples) folder.


Value types for arguments or options with or without parameters:
================================================================
This library supports different types for positional arguments and option parameters.
The following subsection introduces all supported types and how additional types can be added.
In the subsection below, some argument or option parameter restrictions are introduced.

CLI-Options:
------------

### CLI-Option construction:

In general, the constructors of all shipped options have these signatures:

```c++
ABSTRACT_OPTION(basic_parser_t& parser, LONG_OPTION long_option, SHORT_OPTION short_option, const std::string& description, ADDITIONAL_PARAMS... params);
ABSTRACT_OPTION(basic_parser_t& parser, LONG_OPTION long_option, const std::string& description, ADDITIONAL_PARAMS... params);
ABSTRACT_OPTION(basic_parser_t& parser, SHORT_OPTION short_option, const std::string& description, ADDITIONAL_PARAMS... params);
```

where `LONG_OPTION`, `SHORT_OPTION` and `ADDITIONAL_PARAMS` are placeholders for the following types:
* `LONG_OPTION` can either be a `std::string`, a C-style string or a `std::vector<std::string>`.
* `SHORT_OPTION` can either be a `char` or a `std::vector<char>`.
* `ADDITIONAL_PARAMS` are the variadic types that involve additional parameters for the option (For a complete collection of additional parameters see [Additional parameters for arguments or options](#additional-parameters-for-arguments-or-options)).

Currently, there exist different types of options:
* non-parameter options: These options do not support any parameters. I.e. the CLI parser only checks if the option is given or not. One example is the common `--help|-h` option.
* parameter options: Some options require parameters. I.e. options that take numbers, strings or paths. The parameters must be given directly after the option either separated with `=` or with ` `.
Examples are `--string-opt='parameter'` or `--string-opt 'parameter'`. 
* vector-parameter options: These are required, if parameter options can be given multiple times. In this case, the parsed parameters are stored in a `std::vector`. E.g. `--path /tmp/xxx --path=/tmp/yyy`.

As a default, all options are optional. If an option is required, the additional parameter `clapp::parser::types::purpose_t::mandatory` must be given to the constructor.

A complete colletion of supported options is available in [Complete collection of supported argument or option parameter types](#complete-collection-of-supported-argument-or-option-parameter-types).

Example constructor calls for some different options are the following lines:

```c++
clapp::option::uint8_param_option_t{parser_inst, "--long-uint8-param", "a description for an unit8-param", clapp::parser::types::purpose_t::mandatory};
clapp::option::sec_param_option_t{parser_inst, "-s", "number of seconds"}
clapp::option::vector_path_param_option_t{parser_inst, "--path", '-p', "a path the the required file", clapp::value::path_exists_t{}};
clapp::option::vector_path_param_option_t{parser_inst, {"--file", "--f"}, '-f', "a path to the required file", clapp::value::path_exists_t{}};
```

### CLI-Option usage:
In general, all shipped CLI-options can be used in similar ways, as all support the following methods:

```c++
template<typename T>
T value() const;
explicit operator bool() const;
bool has_value() const;
bool given() const noexcept;
```

The method `value()` can be used to retrieve the parsed value from a CLI-option.
If the CLI-parser could not parse the option value from the command line arguments (i.e. the CLI-option contains no value),
a call to `value()` would throw a `clapp::exception::value_undefined_t`-exception.
To check if `value()` would throw without calling `value()` directly, the methods `has_value()` or `operator bool()`
can be used. Both return `false`, if a call to `value()` would throw and `true` if a call to `value()` would not throw.

If the CLI option was constructed with an additional `clapp::value::default_value_t`-parameter, a call to `T value()` will 
never throw, as `value()` would return the default value.
To distinguish between default values and given values, the method `given()` can be used: 
`given()` returns `false`, if `value()` would return the default value from the additional `clapp::value::default_value_t`-parameter 
and it returns `true`, if `value()` would return a value that is parsed from the CLI arguments.

Positional CLI-Arguments:
-------------------------

In general, the constructors of all shipped (positional) arguments have these signature:

```c++
ABSTRACT_ARGUMENT(basic_parser_t& parser, const std::string& argument, const std::string& description, ADDITIONAL_PARAMS... params);
```

where `ADDITIONAL_PARAMS` are placeholders for variadic types that involve additional parameters for the argument (For a complete collection of additional parameters see [Additional parameters for arguments or options](#additional-parameters-for-arguments-or-options)).

Currently, there exist two types of positional arguments:
* regular arguments: are positional arguments that are either mandatory or optional. Note, that mandatory arguments must be registered before optional arguments. Otherwise, positional arguments can't be assigned uniquely.
* variadic arguments: sometimes, it is required to give a variable number of arguments. In this case, variadic position argument types can be used. Similar to the optional arguments before, it is required to define all mandatory arguments before variadic arguments are registered. Also note, that optional arguments and variadic arguments can't be used together. In order to make sure, that the arguments can be parsed uniquely.

As a default, all arguments are mandatory. If an argument is optional, the additional parameter `clapp::parser::types::purpose_t::optional` must be given to the constructor.

A complete colletion of supported arguments is available in [Complete collection of supported argument or option parameter types](#complete-collection-of-supported-argument-or-option-parameter-types).


Example constructor calls for arguments are the following lines:

```c++
clapp::string_argument_t string_arg{parser_inst, "string-arg", "String argument"};
clapp::int32_argument_t int_arg{parser_inst, "int-arg", "Int argument", clapp::parser::types::purpose_t::optional};
clapp::variadic_string_argument_t variadic_string_arg{parser_inst, "variadic-string-arg", "Variadic String argument"};
```

### CLI-Argument usage:
In general, all shipped CLI-arguments can be used in similar ways, as all support the following methods:

```c++
template<typename T>
T value() const;
explicit operator bool() const;
bool has_value() const;
bool given() const noexcept;
```

The method `value()` can be used to retrieve the parsed value from a CLI-argument.
If the CLI-parser could not parse the argument value from the command line arguments (i.e. the CLI-argument contains no value),
a call to `value()` would throw a `clapp::exception::value_undefined_t`-exception.
To check if `value()` would throw without calling `value()` directly, the methods `has_value()` or `operator bool()`
can be used. Both return `false`, if a call to `value()` would throw and `true` if a call to `value()` would not throw.

If the CLI argument was constructed with an additional `clapp::value::default_value_t`-parameter, a call to `T value()` will 
never throw, as `value()` would return the default value.
To distinguish between default values and given values, the method `given()` can be used: 
`given()` returns `false`, if `value()` would return the default value from the additional `clapp::value::default_value_t`-parameter 
and it returns `true`, if `value()` would return a value that is parsed from the CLI arguments.


Complete collection of supported argument or option parameter types:
--------------------------------------------------------------------

### String values:
The most basic types for CLI options are string types (`std::string`), as all values in the `argv` parameter can be converted implicitly to (`std::string`).

If string values should be used for positional arguments, the following types can be used:
* `clapp::argument::string_argument_t`
* `clapp::argument::variadic_string_argument_t`

If string values should be used as option parameters, the following types can be used:
* `clapp::option::string_param_option_t`
* `clapp::option::vector_string_param_option_t`

### Integral values:
The following integral types are supported:
* `std::int8_t`
* `std::int16_t`
* `std::int32_t`
* `std::int64_t`
* `std::uint8_t`
* `std::uint16_t`
* `std::uint32_t`
* `std::uint64_t`
* `std::ptrdiff_t`
* `std::size_t`

As input format for these values the following formats can be used:
* `[+-]?[1-9][0-9]*`: Decimal numbers
* `[+-]?0[0-7]+`: Octal numbers
* `[+-]?0[Xx][0-9a-fA-F]+`: Hexadecial numbers

If one of these values contains a `.`, only the decimals before this `.` are used. 
Subsequent decimals are truncted.

Examples: 
* '0xff' -> `std::uint32_t{255}==std::uint8_t{0xff}==std::uint8_t{0377}`.
* '077' -> `std::int32_t{63}==std::uint8_t{0x3f}==std::uint8_t{077}`.
* '100' -> `std::int32_t{100}==std::uint8_t{0x64}==std::uint8_t{0144}`.

If integral values should be used for positional arguments, the following types can be used:
* `clapp::argument::uint8_argument_t`
* `clapp::argument::uint16_argument_t`
* `clapp::argument::uint32_argument_t`
* `clapp::argument::uint64_argument_t`
* `clapp::argument::int8_argument_t`
* `clapp::argument::int16_argument_t`
* `clapp::argument::int32_argument_t`
* `clapp::argument::int64_argument_t`
* `clapp::argument::ptrdiff_argument_t`
* `clapp::argument::size_argument_t`
* `clapp::argument::variadic_uint8_argument_t`
* `clapp::argument::variadic_uint16_argument_t`
* `clapp::argument::variadic_uint32_argument_t`
* `clapp::argument::variadic_uint64_argument_t`
* `clapp::argument::variadic_int8_argument_t`
* `clapp::argument::variadic_int16_argument_t`
* `clapp::argument::variadic_int32_argument_t`
* `clapp::argument::variadic_int64_argument_t`
* `clapp::argument::variadic_ptrdiff_argument_t`
* `clapp::argument::variadic_size_argument_t`

If integral values should be used as option parameters, the following types can be used:
* `clapp::option::uint8_param_option_t`
* `clapp::option::uint16_param_option_t`
* `clapp::option::uint32_param_option_t`
* `clapp::option::uint64_param_option_t`
* `clapp::option::int8_param_option_t`
* `clapp::option::int16_param_option_t`
* `clapp::option::int32_param_option_t`
* `clapp::option::int64_param_option_t`
* `clapp::option::ptrdiff_param_option_t`
* `clapp::option::size_param_option_t`
* `clapp::option::vector_uint8_param_option_t`
* `clapp::option::vector_uint16_param_option_t`
* `clapp::option::vector_uint32_param_option_t`
* `clapp::option::vector_uint64_param_option_t`
* `clapp::option::vector_int8_param_option_t`
* `clapp::option::vector_int16_param_option_t`
* `clapp::option::vector_int32_param_option_t`
* `clapp::option::vector_int64_param_option_t`
* `clapp::option::vector_ptrdiff_param_option_t`
* `clapp::option::vector_size_param_option_t`

### Bool values:
It is also possible to use `bool` values.

As input format for these values the following formats can be used:
* `true` becomes `true`
* `TRUE` becomes `true`
* `1` becomes  `true`
* `false` becomes `false`
* `FALSE` becomes `false`
* `0` becomes  `false`

If bool values should be used for positional arguments, the following type can be used:
* `clapp::argument::bool_argument_t`

If integral values should be used as option parameters, the following type can be used:
* `clapp::option::bool_option_t`

### Filesystem path values:
It is also possible to pass regular filesystem paths as argument or option parameter. 
The underlying type is the standardized filesystem type `std::filesystem::path`.

If filesystem path values should be used as positional arguments, the following types can be used:
* `clapp::argument::path_argument_t`
* `clapp::argument::variadic_path_argument_t`

If filesystem path values should be used as option parameters, the following types can be used:
* `clapp::option::path_param_option_t`
* `clapp::option::vector_path_param_option_t`

### Floating point values:
Besides the classic integer values, it is also possible to use floating point values as arguments or options.
The underlying types are the standardized types `double` or `float`.

If floating point values should be used as positional arguments, the following types can be used:
* `clapp::argument::double_argument_t`
* `clapp::argument::float_argument_t`
* `clapp::argument::variadic_double_argument_t`
* `clapp::argument::variadic_float_argument_t`

If floating point values should be used as otion parameters, the following types can be used:
* `clapp::option::double_param_option_t`
* `clapp::option::float_param_option_t`
* `clapp::option::vector_double_param_option_t`
* `clapp::option::vector_float_param_option_t`

As input format for these values the following formats can be used:
* `[+-]?[0-9]+(\.[0-9]*)?([eE][+-]?[0-9]+)?`: Decimal numbers (with optional decimal exponent base 10)
* `[+-]?0[xX][0-9a-fA-F]+(\.[0-9]*)?([pP][+-]?[0-9a-fA-F]+)?`: Hexadecial numbers (with optional decimal exponent base 2)
* `[+-]?[Ii][Nn][Ff]([Ii][Nn][Ii][Tt][Yy])?`: INF or INFINITY case insensitive with optional sign.
* `[+-]?[Nn][Aa][Nn]?`: NAN case insensitive with optional sign.

### Chrono values:
It is also possible to use crhono types as arguments or options.
The underlying types are the standardized types `std::chrono::hours`...`std::chrono::nanoseconds`.

If chrono values should be used as positional arguments, the following types can be used:
* `clapp::argument::ns_argument_t`
* `clapp::argument::us_argument_t`
* `clapp::argument::ms_argument_t`
* `clapp::argument::sec_argument_t`
* `clapp::argument::min_argument_t`
* `clapp::argument::hours_argument_t`
* `clapp::argument::variadic_ns_argument_t`
* `clapp::argument::variadic_us_argument_t`
* `clapp::argument::variadic_ms_argument_t`
* `clapp::argument::variadic_sec_argument_t`
* `clapp::argument::variadic_min_argument_t`
* `clapp::argument::variadic_hours_argument_t`

If chrono values should be used as option parameters, the following types can be used:
* `clapp::option::ns_param_option_t`
* `clapp::option::us_param_option_t`
* `clapp::option::ms_param_option_t`
* `clapp::option::sec_param_option_t`
* `clapp::option::min_param_option_t`
* `clapp::option::hours_param_option_t`
* `clapp::option::vector_ns_param_option_t`
* `clapp::option::vector_us_param_option_t`
* `clapp::option::vector_ms_param_option_t`
* `clapp::option::vector_sec_param_option_t`
* `clapp::option::vector_min_param_option_t`
* `clapp::option::vector_hours_param_option_t`

### Custom types:
Sometimes it is required to use custom types as argument or option parameter. For example the following enum declaration:

```c++
enum class my_type_t {value1, value2, value3};
```

In order to allow this (or other types) as argument or option parameter, first it is required to create a new template specialization for the function `template<typename T> T clapp::value::convert_value<T>(const std::string_view param)`:

```c++
template<>
my_type_t clapp::value::convert_value<my_type_t>(const std::string_view param) {
    if(param == "value1") {
        return my_type_t::value1;
    } else if(param == "value2") {
        return my_type_t::value2;
    } else if(param == "value3") {
        return my_type_t::value3;
    } else {
        throw std::runtime_error("Invalid value for my_type_t.");
    }
}
```

As you can see in this example, it is perfectly fine to throw an exception, if the parsed value is invalid..

Additional parameters for arguments or options
----------------------------------------------
It is possible to define restrictions for arguments and option parameters.
This restrictions can be given to the argument or option parameter types in an arbitrary order as one of the variadic parameters.

### Mandatory/Optional:
All options can be either mandatory or optional:
* Mandatory options must always be given. This is also reflected in the help message: `-o|--option` or `-p|--option-with-param=<arg>`.
* Optional options are optional and thus are not required. This is also reflected in the help message: `[-o|--option]` or `[-p|--option-with-param=<arg>]`.

As a default (i.e. no mandatory is given), each option is optional! 

Some example options are: 

```c++
clapp::option::int32_param_option_t int_opt{ptr_to_parser, "option", 'o', "Description for mandatory option.", clapp::parser::types::purpose_t::mandatory};
clapp::option::bool_option_t int_opt{ptr_to_parser, "yes", 'y', "Description for optional option.", clapp::parser::types::purpose_t::optional};
```

Arguments can also be mandatory or optional:
* Mandatory arguments must always be given. This is also reflected in the help message: `<argument>`.
* Optional arguments are not necessarily required. This is also reflected in the help message: `[<argument>]`.

As a default (i.e. no optional is given), each argument is mandatory! 

Some example arguments are: 

```c++
clapp::argument::int32_argument_t int_arg{ptr_to_parser, "argument", "Description for mandatory argument.", clapp::parser::types::purpose_t::mandatory};
clapp::argument::bool_option_t int_opt{ptr_to_parser, "argument", "Description for optional argument.", clapp::parser::types::purpose_t::optional};
```

### Default value:
In order to provide a default value for arguments or option parameters, there exists the class `clapp::value::default_value_t`.
If an instance of this class is given to an optional option parameter constructor, this default value would be used, if the option value was not given.
The same reasoning applies also for arguments.
Note: Even if it is possible for mandatory arguments or mandatory option parameters to define a default value, this is unnecessary, as the parser requires the user to give a value!

A default value is also reflected in the help message, as `(default-value: <value>)` is appended to the argument/option description.

Examples are:

```c++
clapp::option::int32_option_t int_opt{ptr_to_parser, "int-option", 'i', "Description for optional option.", clapp::parser::types::purpose_t::optional, clapp::value::default_value_t{10}};
clapp::option::string_argument_t string_arg{ptr_to_parser, "string-argument", "Description for optional argument.", clapp::parser::types::purpose_t::optional, clapp::value::default_value_t{"default-string"}};
```

### Min/Max value:
In order to provide a range check for arguments or option parameters, there exists the class `clapp::value::min_max_value_t`.
If an instance of this class is given to an option parameter constructor, each given value is checked when the parsers `validate()` function is called.
The same reasoning applies also for arguments.

A min/max value is also reflected in the help message, as `(constraint: [<min>,<max>])` is appended to the argument/option description.

Examples are:

```c++
clapp::option::int32_option_t int_opt{ptr_to_parser, "int-option", 'i', "Description for int-option.", clapp::value::min_max_value_t<std::int32_t>{10, 20}};
clapp::option::int32_argument_t int_arg{ptr_to_parser, "int-argument", "Description for int-argument.", clapp::value::min_max_value_t<std::int32_t>{20, 40}};
```

### Not-null value:
In order to provide a range check for arguments or option parameters, there exists the class `clapp::value::not_null_value_t`.
If an instance of this class is given to an option parameter constructor, each given value is checked when the parsers `validate()` function is called.
The same reasoning applies also for arguments.

A not-null value is also reflected in the help message, as `(constraint: not null)` is appended to the argument/option description.

Examples are:

```c++
clapp::option::int32_option_t int_opt{ptr_to_parser, "int-option", 'i', "Description for int-option.", clapp::value::not_null_value_t<std::int32_t>{}};
clapp::option::int32_argument_t int_arg{ptr_to_parser, "int-argument", "Description for int-argument.", clapp::value::not_null_value_t<std::int32_t>{}};
```

### Path exists:
In order to provide a check for filesystem-path arguments or option parameters, there exists the class `clapp::value::path_exists_t`.
If an instance of this class is given to an option parameter constructor, each given path is checked for existence when the parsers `validate()` function is called.
The same reasoning applies also for arguments.

This restriction is also reflected in the help message, as `(existing path)` is appended to the argument/option description.

Note: this class may only be used for filesystem path values.

Examples are:

```c++
clapp::option::path_param_option_t file_opt{ptr_to_parser, "filename-option", 'f', "Description for filename option.", clapp::value::path_exists_t{}};
clapp::option::path_argument_t file_arg{ptr_to_parser, "filenem-argument", "Description for filename argument.", clapp::value::path_exists_t{}};
```

### Found callback:
In order to provide a callback-function for arguments or options which gets invoked at the time when the parser detects the option or argument, there exists the class `clapp::value::found_func_t`.
This class expects a callable object (or function pointer) which matches the following signature: `std::function<clapp::value::found_func_t::ret_t(const std::string&)>`.
If an instance of this class is given to an option- or argument-constructor, the parser will invoke the callable object inside this instance once it detects it (and even before it parses anything).
Since the return-value is a `std::optional`, the callable object may return `std::nullopt` (if the parser should proceed parsing) or the exit reason `clapp::value::exit_t` (if the parser should immediately stop parsing).
In the latter case, the parser stops parsing immediately and propagates the exit reason instance `clapp::value::exit_t` down to the caller of the `parse()`- and `parse_and_validate()`-methods.

In order to create an exit reason, `clapp::value::stop_t::exit(<EXIT-CODE>)` must be called:

Examples for creating an exit reason are the following calls:
```c++
clapp::value::stop_t::exit(EXIT_FAILURE);
clapp::value::stop_t::exit(EXIT_SUCCESS);
```

Note: when parsing was stopped by an exit reason, using any properties/methods of options or arguments of this parser lead to undefined behavior (as parsing was interrupted). Therefore, the parser should use a `clapp::parser::basic_parser_container_t` for encapsulation. It ensures that the parser is always used in a valid state.

One example of a found callback is the help-option `--help`.
If the user gives `--help`, any further parsing may not be adequate (either, because it )
Therefore, the `help_option_t` returns an exit reason (`EXIT_SUCCESS`) and the caller gets informed about this, as the `parse()`- or `parse_and_validate()`-methods return this exit reason.

### Custom restrictions:
Of course, it is possible to create own restriction classes. 
These classes must contain at least one of these methods:
* `template<typename T> T default_value() const;`
    This method can be used to define a default value, if the argument or parameter option is optional.
* `std::string append_restriction_text() const;`
    The returend strig will be appended to the description of the argumen or option.
* `void validate(const T &value, const std::string &param_name) const;`
    If this method is defined, it will be called during the `validate()`-call of the parser.

Parser:
=======

Main Parser:
------------
The main parser is the key element of the libClaPP CLI parsing library.
Typically, it is a derivied class of the base class `clapp::parser::basic_main_parser_t`.
One of the benefits of the libClaPP library is that all arguments and options can be registered
as class members.
Thus, by simply passing around a reference (or a pointer) to this main parser, all parsed
arguments or option can be be accessed easily.

### Main parser construction:
The main-parser base class `clapp::parser::basic_main_parser_t` inherits its constructors from its base-class `clapp::parser::basic_parser_t`.
This constructor has no arguments, but since it is a base-class, you can extend it easily by
your derived class. See the listing below for an example.

### Main parser usage:
The main-parser base class `clapp::parser::basic_main_parser_t` is shipped with several useful methods.
Most of them are listed here, but for a complete collection take a look at
[src/include/clapp/main_parser.h](src/include/clapp/main_parser.h).
Since the main-parser base class `clapp::parser::basic_main_parser_t` is itself derived from
`clapp::parser::basic_parser_t`, you may also look at
[src/include/clapp/parser.h](src/include/clapp/parser.h)
for a complete collection of derived methods.

```c++
std::optional<clapp::value::stop_t> parse(int argc, const char* const* argv);
std::optional<clapp::value::stop_t> parse(const arg_t& arg);
std::optional<clapp::value::stop_t> parse_and_validate(int argc, const char* const* argv);
void validate() const;
void validate_recursive() const;
explicit operator bool() const;
std::string get_executable() const;
std::string gen_help_msg(std::size_t rec_depth) const;
std::size_t get_num_processed_arguments() const;
const basic_parser_t& get_active_parser() const;
```
#### The `parse()` methods:
The `parse()` methods can be used to parse the command line arguments.
These functions may throw exceptions of type `clapp::clapp_exception_t` (or a derived type of this type).
For example, if invalid or unknown options are given, or given values could not be converted to the required type.
But these functions only try to parse the values.
Thus, in order to check if all mandatory options and arguments are given, the methods `validate()`, `validate_recursive()` or `parse_and_validate()` may be used.
Note: if an exit reason (from a `found_func_t`-callback) is returned, this exit reason is returned from the `parse()` methods.

#### The `validate()` method:
The `validate()` method can be used to check the requirements of the parsed result.
Thus, it checks if all mandatory arguments or options were given and ensures that all constraints are met.
Note: this method only validates all arguments or options of the current parser instance. Thus, if subparsers are used, the `validate()` method ignores all subparsers. If you want to validate the main-parser and all sub-parsers, use `validate_recursive()`.
If the validation fails, it throws an exception of type `clapp::clapp_exception_t` (or a derived type of this type).

#### The `validate_recursive()` method:
This method ensures that all mandatory arguments or options were given and ensures that all constraints are met. (Similar to the `validate()` method in the `validate()` section).
The only difference is, that it validates all subparsers too.
This is done in a recursive way.
Thus, it validates the current parser and subparsers that were selected by the cli-parameters.

#### The `parse_and_validate()` method:
This method is a combination of the `parse()`- and `validate_recursive()`-methods.
Thus, it first parses the cli-arguments and if the parsing succeeds, it validates the parsed results.
Note: if an exit reason (from a `found_func_t`-callback) is returned, this exit reason is returned from the `parse()` methods.

#### The `operator bool()` method:
For main-parsers, this method always returns `true`, if an executable is set.
If an executable is set, it can be requested by calling `get_executable()`.

#### The `get_executable()` method:
This method returns the name of the executable that is used in the cli-arguments.
If it is not set, the exception `no_executable_exception_t` is thrown.

#### The `gen_help_msg()` method:
This method generates the usage message of the parser class.
This output is the same as it would be generated by the `help_option_t`-option.
`gen_help_msg()` requires an additional parameter `rec_depth`:
It controls the depth of the help message.
I.e. the number of encapsulated (sub-)parsers.
A number of 0 does not include any subparsers and would only include the main parser.

#### The `get_num_processed_arguments()` method:
This method reurns how many arguments were given via the cli-interface.

#### The `get_active_parser()` method:
This method returns a reference to the selected (sub-)parser.
If no sub-parser was selected by the cli-arguments, this method returns a reference
to this current main-parser instance. If a sub-parser was selected it would return a reference to
this sub-parser.

### Example code listing for a main parser:
The following code-listing illustrates a very basic example of a main parser with several options and a
string argument:

[//]:#begin_cpp_listing_simple_main_parser
```c++
#include <clapp/argument.h>
#include <clapp/main_parser.h>
#include <clapp/option.h>

class cli_parser_t : public clapp::basic_main_parser_t {
   public:
    clapp::help_option_t help{*this, "help", 'h', "Show help options."};

    clapp::bool_option_t bool_opt{*this, "bool", 'b', "Bool option."};

    clapp::string_param_option_t string_opt{*this, "str", 's',
                                            "String option."};

    clapp::string_argument_t string_arg{*this, "string-arg", "String argument"};
};

int main(int argc, char *argv[]) {
    try {
        cli_parser_t clip;  // create parser instance
        const std::optional<clapp::value::exit_t> exit{clip.parse_and_validate(
            argc, argv)};  // parses and validates cli-arguments
        if (exit) {
            return exit.value().get_exit_code();
        }
        Ensures(
            clip.string_arg);  // parser ensures mandatory arguments are given
        Ensures(
            clip.bool_opt);  // parser ensures mandatory bool-option is given
        Ensures(clip.string_opt);  // parser ensures mandatory string-option is
                                   // given
        std::cout << "string-opt: " << clip.string_opt.value() << std::endl;
        std::cout << "string-arg: " << clip.string_arg.value() << std::endl;
    } catch (std::exception &e) {
        std::cout << "Caught Exception: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
```
[//]:#end_cpp_listing_simple_main_parser

Note: The previous listing explicitly deletes the copy/move assignment operators and the copy/move constructors.
Depending on your code style, this may not be necessary, as `clapp::basic_main_parser_t` already deletes the copy/move assignment operators and the copy/move constructors.
But if you want to be conformant to [CppCoreGuideline C.21](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#c21-if-you-define-or-delete-any-default-operation-define-or-delete-them-all), you should declare them.

### Example cli-outputs when using the previous listing:
If the previous example listing is executed, you get the following output:
[//]:#begin_calls_simple_main_parser
```bash
# Print the help message:
$ ./libclapp_doc_simple_main_parser -h  # this is the same as with option `--help`
Usage:
./libclapp_doc_simple_main_parser [-h|--help] -b|--bool -s|--str=<param> <string-arg>

  Arguments:
    string-arg       String argument (mandatory)

  Options:
    -h|--help        Show help options. (optional)
    -b|--bool        Bool option. (mandatory)
    -s|--str=<param> String option. (mandatory)

# Give mandatory arguments and options:
$ ./libclapp_doc_simple_main_parser -b -s "my-string-opt" my-string-arg
string-opt: my-string-opt
string-arg: my-string-arg

# Give no mandatory argument throws:
$ ./libclapp_doc_simple_main_parser -b -s "my-string-opt"
Caught Exception: Mandatory argument 'string-arg' not given.

# Give no mandatory option throws:
$ ./libclapp_doc_simple_main_parser -b my-string-arg
Caught Exception: Only the following mandatory options '-b|--bool' were given, but at least the following options are required: '-b|--bool', '-s|--str'

```
[//]:#end_calls_simple_main_parser

Parser-Container:
-----------------
In particular, if the `parse()` function received an exit reason (from a `found_func_t`-callback)
the parser aborts parsing immediately.
However, this leaves the parser-object (and its options or arguments) in an an unspecified
(but valid) state, as the `found_func_t`-callback is called directly when the parser reaches
the particular option or argument.
Thus, the parser state after an exit reason is amongst others dependent on the order of the
cli-arguments and parsing may not be finished at the timepoint when the exit reason was received.

To prevent the users from accessing members in an unspecified state, the parser-container
`basic_parser_container_t` was introduced.
It is a wrapper well suited for a main parser (similar to a `std::unique_ptr` for a raw-pointer).

### Parser container construction:
Since the parser container is a template, it requires the typename of the main parser which
should be wrapped.
Its constructor forwards all its arguments to the wrapped main parser constructor.

### Parser container usage:
The parser container base class `clapp::parser::basic_parse_container_t` ships only with
`parse()` and `parse_and_validate()` methods.
To get access to the underlying (main) parser, the operators `->` and `*` were also overloaded.
These operators can be used to get access to the members or methods of the underlying (main) parser.

Most of them are listed here, but for a complete collection take a look at
[src/include/clapp/main_parser.h](src/include/clapp/main_parser.h).
Since the main-parser base class `clapp::parser::basic_main_parser_t` is itself derived from
`clapp::parser::basic_parser_t`, you may also look at
[src/include/clapp/parser.h](src/include/clapp/parser.h)
for a complete collection of derived methods.

```c++
template <typename... ARGS_T>
std::optional<clapp::value::stop_t> parse(ARGS_T&&...);
template <typename... ARGS_T>
std::optional<clapp::value::stop_t> parse_and_validate(ARGS_T&&...);

parser_t* operator->();
parser_t& operator*();
```
#### The `parse()` methods:
The `parse()` method forwards all arguments to the corresponding `parse()` methods of the
underlying (main) parser.
Furthermore, it forwards possible exit reasons from the `parse()` method of the main parser.

#### The `parse_and_validate()` method:
The `parse_and_validate()` method forwards all arguments to the `parse_and_validate()` method
of the underlying (main) parser.
Furthermore, it forwards possible exit reasons from the `parse()` method of the main parser.

#### The `operator->()` method:
This operator provides access to the members or methods of the underlying (main) parser.

#### The `operator*()` method:
This operator provides access to the members or methods of the underlying (main) parser.

### Example code listing for a parser container:
The following code-listing illustrates the same example as before but it makes use of a
parser container:

[//]:#begin_cpp_listing_parser_container
```c++
#include <clapp/argument.h>
#include <clapp/main_parser.h>
#include <clapp/option.h>
#include <clapp/parser_container.h>

#include <optional>

class cli_parser_t : public clapp::basic_main_parser_t {
   public:
    clapp::help_option_t help{*this, "help", 'h', "Show help options."};

    clapp::string_argument_t string_arg{*this, "string-arg", "String argument"};
};


using parser_t = clapp::parser::basic_parser_container_t<cli_parser_t>;

int main(int argc, char *argv[]) {
    try {
        parser_t parser;  // create parser instance
        const std::optional<clapp::value::exit_t> exit{
            parser.parse_and_validate(
                argc, argv)};  // parses and validates cli-arguments
        if (exit) {
            return exit.value().get_exit_code();
        }
        Ensures(
            parser->string_arg);  // parser ensures mandatory arguments are given
        std::cout << "string-arg: " << parser->string_arg.value() << std::endl;
    } catch (std::exception &e) {
        std::cout << "Caught Exception: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
```
[//]:#end_cpp_listing_parser_container

Note: The previous listing explicitly deletes the copy/move assignment operators and the copy/move constructors.
Depending on your code style, this may not be necessary, as `clapp::basic_main_parser_t` already deletes the copy/move assignment operators and the copy/move constructors.
But if you want to be conformant to [CppCoreGuideline C.21](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#c21-if-you-define-or-delete-any-default-operation-define-or-delete-them-all), you should declare them.

### Example cli-outputs when using the previous listing:
If the previous example listing is executed, you get the following output:

[//]:#begin_calls_parser_container
```bash
# Print the help message:
$ ./libclapp_doc_parser_container -h  # this is the same as with option `--help`
Usage:
./libclapp_doc_parser_container [-h|--help] <string-arg>

  Arguments:
    string-arg String argument (mandatory)

  Options:
    -h|--help  Show help options. (optional)

# Give mandatory argument:
$ ./libclapp_doc_parser_container my-string
string-arg: my-string

# Give no mandatory argument throws:
$ ./libclapp_doc_parser_container
Caught Exception: Mandatory argument 'string-arg' not given.

```
[//]:#end_calls_parser_container

Sub Parser:
-----------
The sub-parser is another key element of the libClaPP CLI parsing library.
Typically, it is a derivied class of the base class `clapp::parser::basic_sub_parser_t`.

In particular, if you want to have different cli-interface modes, a sub-parser may be what you want:
A sub parser is similar to a main-parser, as it also contains different arguments and options, 
but it is typically invoked by the surrounding main parser, if a special keyword is given in the CLI 
arguments.
Thus, typically there is no need to invoke a sub-parser directly.

Typically, a sub parser can be used to switch between different modes of a program, that need different
arguments or options.
A typical example is the git command: By calling `git log` or `git commit` you get different cli options and arguments each.
In the context of libclapp it is required to define a sub parser that handles the `log` options and arguments 
and another sub parser that handles the `commit` options and arguments.

For example, if it is required that a binary supports 3 modi: a default mode and two additional modes: mode1 and mode2.
The default mode is active, if none of the additional modes was activated. Both modes can define other arguments and options:

Note: if sup-parsers are used, these sub-parsers always take the role of the last argument.
Thus, if at least one sub-parser is registered, no variadic or optional arguments can be used.
Furthermore, if a sub-parser is registered before an argument, the argument position will always be before registered sub-parsers.

Note: sub-parsers can be stacked. I.e. a sub-parser can itself contain other sub-parsers which iself can contain additional sub-parsers.

### Sub parser construction:
The sub-parser base class `clapp::parser::basic_sub_parser_t` contains only one constructor:

```c++
basic_sub_parser_t(basic_parser_t& parser, const std::string& sub_parser_name, const std::string& description);
```

Similar to the option and argument constructors, the first argument is a reference to the surrounding parser (i.e. a main parser or other sub-parsers.
The other two arguments are the name of the sub-parser and a description of the sub-parser.
See the listing below for an example how a sub-parser can be created.

### Sub parser usage:
The sub-parser base class `clapp::parser::basic_sub_parser_t` is shipped with several useful methods.
Most of them are listed here, but for a complete collection take a look at
[src/include/clapp/sub_parser.h](src/include/clapp/sub_parser.h).
Since the sub-parser base class `clapp::parser::basic_sub_parser_t` is itself derived from
`clapp::parser::basic_parser_t`, you may also look at
[src/include/clapp/parser.h](src/include/clapp/parser.h)
for a complete collection of derived methods.

```c++
void validate() const;
void validate_recursive() const;
bool is_active() const;
explicit operator bool() const;
std::string get_sub_parser_name() const;
std::size_t get_num_processed_arguments() const;
const basic_parser_t& get_active_parser() const;
```

#### The `validate()` method:
The `validate()` method is similar to `basic_main_parser::validate()` and thus can be used to check the requirements of the parsed result.
Thus, it checks if all mandatory arguments or options were given and ensures that all constraints are met.
Note: this method only validates all arguments or options of the current parser instance. Thus, if further subparsers are used, the `validate()` method ignores all subparsers. If you want to validate this sub-parser and all its sub-parsers, use `validate_recursive()`.
If the validation fails, it throws an exception of type `clapp::clapp_exception_t` (or a derived type of this type).

#### The `validate_recursive()` method:
This method is similar to `basic_main_parser::validate_recursive()` and thus ensures that all mandatory arguments or options were given and ensures that all constraints are met. (Similar to the `validate()` method in the `validate()` section).
The only difference is, that it validates all subparsers too.
This is done in a recursive way.
Thus, it validates the current parser and subparsers that were selected by the cli-parameters.

#### The `bool is_active()` method:
This method always returns `true`, if the sub-parser is selected by the cli-arguments.
If other sub-parsers are selected (that are not members of this sub-parser), this method returns `false`.

#### The `operator bool()` method:
For sub-parsers this method always returns `true`, if the sub-parser is selected by the cli-arguments.
(See also method `is_active()`.)

#### The `get_sub_parser_name()` method:
This method returns the name of the sub-parser.

#### The `get_num_processed_arguments()` method:
This method reurns how many arguments were given via the cli-interface.

#### The `get_active_parser()` method:
This method returns a reference to the selected sub-parser (if it is a member of this sub-parser).
If no sub-parser-member was selected by the cli-arguments, this method returns a reference
to this sub-parser instance. If a sub-parser was selected it would return a reference to
this sub-parser.

### Example code listing using sub-parsers and parser containers:

[//]:#begin_cpp_listing_simple_sub_parser
```c++
#include <clapp/argument.h>
#include <clapp/main_parser.h>
#include <clapp/option.h>
#include <clapp/parser_container.h>
#include <clapp/sub_parser.h>

#include <optional>

class cli_parser_t : public clapp::basic_main_parser_t {
   public:
    clapp::help_option_t help{*this, "help", 'h', "Show help options."};

    clapp::int32_param_option_t int_opt{*this, 'i', "Int option"};

    class mode1_parser_t : public clapp::basic_sub_parser_t {
       public:
        using clapp::basic_sub_parser_t::basic_sub_parser_t;

        clapp::help_option_t help{*this, "help", 'h', "Show help options."};

        clapp::string_param_option_t string{*this, 's', "String param option."};
    };

    class mode2_parser_t : public clapp::basic_sub_parser_t {
       public:
        using clapp::basic_sub_parser_t::basic_sub_parser_t;

        clapp::help_option_t help{*this, "help", 'h', "Show help options."};

        clapp::string_argument_t string_arg{*this, "string-arg",
                                            "String argument"};
    };

    mode1_parser_t mode1{*this, "mode1", "mode1 sub-parser."};
    mode2_parser_t mode2{*this, "mode2", "mode2 sub-parser."};
};

using parser_t = clapp::parser::basic_parser_container_t<cli_parser_t>;

int main(int argc, char *argv[]) {
    try {
        parser_t parser;  // create parser instance
        const std::optional<clapp::value::exit_t> exit{
            parser.parse_and_validate(
                argc, argv)};  // parses and validates cli-arguments
        if (exit) {
            return exit.value().get_exit_code();
        }

        if (parser->mode1) {
            std::cout << "mode1: ";
            if (parser->mode1.string) {
                std::cout << "string: '" << parser->mode1.string.value()
                          << "' ";
            }
            if (parser->int_opt) {
                std::cout << "int-opt: '" << parser->int_opt.value() << "' ";
            }
            std::cout << std::endl;
        } else if (parser->mode2) {
            std::cout << "mode2: ";
            // parser ensures mandatory arguments are given
            Ensures(parser->mode2.string_arg);
            std::cout << "string_arg: '" << parser->mode2.string_arg.value()
                      << "'" << std::endl;
        } else {
            std::cout << "default mode: ";
            if (parser->int_opt) {
                std::cout << "int-opt: '" << parser->int_opt.value() << "'";
            }
            std::cout << std::endl;
        }
    } catch (std::exception &e) {
        std::cout << "Caught Exception: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
```
[//]:#end_cpp_listing_simple_sub_parser

The mode1-subparser and the mode2-subparser support both different options and arguments. In the following section, you can see the output of this example when invoked with different CLI-arguments.

### Example cli-outputs when using the sub-parser listing:
[//]:#begin_calls_simple_sub_parser
```bash
# Print the overall help message:
$ ./libclapp_doc_simple_sub_parser --help
Usage:
./libclapp_doc_simple_sub_parser [-h|--help] [-i=<param>] mode1 [-h|--help] [-s=<param>]
./libclapp_doc_simple_sub_parser [-h|--help] [-i=<param>] mode2 [-h|--help] <string-arg>
./libclapp_doc_simple_sub_parser [-h|--help] [-i=<param>]

  Options:
    -h|--help  Show help options. (optional)
    -i=<param> Int option (optional)

  Subparser:
    mode1      mode1 sub-parser.
      Options:
        -h|--help  Show help options. (optional)
        -s=<param> String param option. (optional)

    mode2      mode2 sub-parser.
      Arguments:
        string-arg String argument (mandatory)

      Options:
        -h|--help  Show help options. (optional)


# Print the help message of mode1:
$ ./libclapp_doc_simple_sub_parser mode1 -h
Usage:
./libclapp_doc_simple_sub_parser [-h|--help] [-i=<param>] mode1 [-h|--help] [-s=<param>]

  Options:
    -h|--help  Show help options. (optional)
    -s=<param> String param option. (optional)

# Print the help message of mode2:
$ ./libclapp_doc_simple_sub_parser mode2 --help
Usage:
./libclapp_doc_simple_sub_parser [-h|--help] [-i=<param>] mode2 [-h|--help] <string-arg>

  Arguments:
    string-arg String argument (mandatory)

  Options:
    -h|--help  Show help options. (optional)

# Give no sub-parser:
$ ./libclapp_doc_simple_sub_parser
default mode: 

# Give no sub-parser, but optional int-option `-i`:
$ ./libclapp_doc_simple_sub_parser -i 12                    #decimal
default mode: int-opt: '12'

# Give invalid sub-parser:
$ ./libclapp_doc_simple_sub_parser mode
Caught Exception: Unknown argument/sub-parser 'mode'.

# Give sub-parser `mode1`:
$ ./libclapp_doc_simple_sub_parser mode1
mode1: 

# Give sub-parser `mode1` and optional int-option `-i`:
$ ./libclapp_doc_simple_sub_parser -i 011 mode1             #octal
mode1: int-opt: '9' 

# Give sub-parser `mode1` and optional string-option `-s`:
$ ./libclapp_doc_simple_sub_parser mode1 -s str
mode1: string: 'str' 

# Give sub-parser `mode1` and optional string-option `-s` and int-option `-i`:
$ ./libclapp_doc_simple_sub_parser -i=0x10 mode1 -s=str     #hex
mode1: string: 'str' int-opt: '16' 

# Give sub-parser `mode2` without mandatory string-argument `string-arg`:
$ ./libclapp_doc_simple_sub_parser mode2
Caught Exception: Mandatory argument 'string-arg' not given.

# Give sub-parser `mode2` with mandatory string-argument `string-arg`:
$ ./libclapp_doc_simple_sub_parser mode2 argument
mode2: string_arg: 'argument'

# Give sub-parser `mode2` with mandatory string-argument `string-arg` and invalid option `-i`:
$ ./libclapp_doc_simple_sub_parser mode2 argument -i 1
Caught Exception: Invalid (sub-)parser option '-i'

```
[//]:#end_calls_simple_sub_parser
