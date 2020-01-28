Value types for arguments or option parameters:
===============================================
This library supports different types for positional arguments and option parameters.
The following subsection introduces all supported types and how additional types can be added.
In the subsection below, some argument or option parameter restrictions are introduced.

Supported argument or option parameter types:
---------------------------------------------

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

As input format for these values the following formats can be used:
* `[+-][1-9][0-9]*`: Decimal numbers
* `[+-]0[0-9]+`: Octal numbers
* `[+-]0x[0-9]+`: Hexadecial numbers
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
* `clapp::argument::variadic_uint8_argument_t`
* `clapp::argument::variadic_uint16_argument_t`
* `clapp::argument::variadic_uint32_argument_t`
* `clapp::argument::variadic_uint64_argument_t`
* `clapp::argument::variadic_int8_argument_t`
* `clapp::argument::variadic_int16_argument_t`
* `clapp::argument::variadic_int32_argument_t`
* `clapp::argument::variadic_int64_argument_t`

If integral values should be used as option parameters, the following types can be used:
* `clapp::option::uint8_param_option_t`
* `clapp::option::uint16_param_option_t`
* `clapp::option::uint32_param_option_t`
* `clapp::option::uint64_param_option_t`
* `clapp::option::int8_param_option_t`
* `clapp::option::int16_param_option_t`
* `clapp::option::int32_param_option_t`
* `clapp::option::int64_param_option_t`
* `clapp::option::vector_uint8_param_option_t`
* `clapp::option::vector_uint16_param_option_t`
* `clapp::option::vector_uint32_param_option_t`
* `clapp::option::vector_uint64_param_option_t`
* `clapp::option::vector_int8_param_option_t`
* `clapp::option::vector_int16_param_option_t`
* `clapp::option::vector_int32_param_option_t`
* `clapp::option::vector_int64_param_option_t`

### Filesystem path values:
It is also possible to pass regular filesystem paths as argument or option parameter. 
The underlying type is the standardized filesystem type `std::filesystem::path`.

If filesystem path values should be used for positional arguments, the following types can be used:
* `clapp::argument::path_argument_t`
* `clapp::argument::variadic_path_argument_t`

If filesystem path values should be used as option parameters, the following types can be used:
* `clapp::option::path_param_option_t`
* `clapp::option::vector_path_param_option_t`

### Custom types:
Sometimes it is required to use custom types as argument or option parameter. For example the following enum declaration:

    enum class my_type_t {value1, value2, value3};

In order to allow this (or other types) as argument or option parameter, first it is required to create a new template specialization for the function `template<typename T> T clapp::value::convert_value<T>(const std::string_view param)`:

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

As you can see in this example, it is perfectly fine to throw an exception, if the parsed value is invalid..

Restrictions for arguments or option parameters
-----------------------------------------------
It is possible to define restrictions for arguments and option parameters.
This restrictions can be given to the argument or option parameter types in an arbitrary order as one of the variadic parameters.

### Mandatory/Optional:
All options can be either mandatory or optional:
* Mandatory options must always be given. This is also reflected in the help message: `-o|--option` or `-p|--option-with-param=<arg>`.
* Optional options are optional and thus are not required. This is also reflected in the help message: `[-o|--option]` or `[-p|--option-with-param=<arg>]`.
As a default (i.e. no mandatory is given), each option is optional! 

Some example options are: 

    clapp::option::int32_param_option_t int_opt{ptr_to_parser, "option", 'o', "Description for mandatory option.", clapp::purpose_t::mandatory};
    clapp::option::bool_option_t int_opt{ptr_to_parser, "yes", 'y', "Description for optional option.", clapp::purpose_t::optional};

Arguments can also be mandatory or optional:
* Mandatory arguments must always be given. This is also reflected in the help message: `<argument>`.
* Optional arguments are not necessarily required. This is also reflected in the help message: `[<argument>]`.
As a default (i.e. no optional is given), each argument is mandatory! 

Some example arguments are: 

    clapp::argument::int32_argument_t int_arg{ptr_to_parser, "argument", "Description for mandatory argument.", clapp::purpose_t::mandatory};
    clapp::argument::bool_option_t int_opt{ptr_to_parser, "argument", "Description for optional argument.", clapp::purpose_t::optional};

### Default value:
In order to provide a default value for arguments or option parameters, there exists the class `clapp::value::default_value_t`.
If an instance of this class is given to an optional option parameter constructor, this default value would be used, if the option value was not given.
The same reasoning applies also for arguments.
Note: Even if it is possible for mandatory arguments or option parameters to define a default value, this may be unnecessary.

A default value is also reflected in the help message, as `(default-value: <value>)` is appended to the argument/option description.

Examples are:

    clapp::option::int32_option_t int_opt{ptr_to_parser, "int-option", 'i', "Description for optional option.", clapp::purpose_t::optional, clapp::value::default_value_t{10}};
    clapp::option::string_argument_t string_arg{ptr_to_parser, "string-argument", "Description for optional argument.", clapp::purpose_t::optional, clapp::value::default_value_t{"default-string"}};

### Min/Max value:
In order to provide a range check for arguments or option parameters, there exists the class `clapp::value::min_max_value_t`.
If an instance of this class is given to an option parameter constructor, each given value is checked when the parsers `validate()` function is called.
The same reasoning applies also for arguments.

A min/max value is also reflected in the help message, as `(constraint: [<min>,<max>])` is appended to the argument/option description.

Examples are:

    clapp::option::int32_option_t int_opt{ptr_to_parser, "int-option", 'i', "Description for int-option.", clapp::value::min_max_value_t{10, 20}};
    clapp::option::int32_argument_t int_arg{ptr_to_parser, "int-argument", "Description for int-argument.", clapp::value::min_max_value_t{20, 40}};

### Path exists:
In order to provide a check for filesystem-path arguments or option parameters, there exists the class `clapp::value::path_exists_t`.
If an instance of this class is given to an option parameter constructor, each given path is checked for existence when the parsers `validate()` function is called.
The same reasoning applies also for arguments.

This restriction is also reflected in the help message, as `(existing path)` is appended to the argument/option description.

Note: this class may only be used for filesystem path values.

Examples are:

    clapp::option::path_param_option_t file_opt{ptr_to_parser, "filename-option", 'f', "Description for filename option.", clapp::value::path_exists_t{}};
    clapp::option::path_argument_t file_arg{ptr_to_parser, "filenem-argument", "Description for filename argument.", clapp::value::path_exists_t{}};

### Custom restrictions:
Of course, it is possible to create own restriction classes. 
This classes must contain at least one of these methods:
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
Typically, it is a derivied class with the base class `clapp::parser::basic_main_parser_t`.
One of the benefit of the libClaPP library is that all arguments and options can be registered 
as class members.
Thus, by simply passing around a reference (or a pointer) to the main parser, all parsed options 
or arguments are condensed in a class instance.

[//]:#begin_cpp_listing_simple_main_parser
```c++
#include <clapp/argument.h>
#include <clapp/main_parser.h>
#include <clapp/option.h>

class cli_parser_t : public clapp::basic_main_parser_t {
   public:
    cli_parser_t(int argc, const char *const *argv) {
        parse_and_validate(argc, argv);
    }

    ~cli_parser_t() override;

    clapp::help_option_t help{*this, "help", 'h', "Show help options."};

    clapp::string_argument_t string_arg{*this, "string-arg", "String argument"};

    //explicitly delete copy/move-ctors and assignmet operators 
    explicit cli_parser_t(const cli_parser_t &) = delete;
    explicit cli_parser_t(cli_parser_t &&) noexcept = delete;
    cli_parser_t &operator=(const cli_parser_t &) = delete;
    cli_parser_t &operator=(cli_parser_t &&) noexcept = delete;
};

cli_parser_t::~cli_parser_t() = default;

int main(int argc, char *argv[]) {
    try {
        cli_parser_t cp{argc, argv};  // parses and validates cli-arguments

        Expects(cp.string_arg);  // parser ensures mandatory arguments are given
        std::cout << "string-arg: " << cp.string_arg.value() << std::endl;
    } catch (std::exception &e) {
        std::cout << "Caught Exception: " << e.what() << std::endl;
    }
}
```
[//]:#end_cpp_listing_simple_main_parser
