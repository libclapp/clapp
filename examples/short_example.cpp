#include <clapp/argument.h>
#include <clapp/build_info.h>
#include <clapp/main_parser.h>
#include <clapp/option.h>
#include <clapp/parser_container.h>
#include <unistd.h>

#include <iostream>

clapp::value::found_func_t::ret_t print_version_and_exit(
    const std::string &option);

class cli_parser_t : public clapp::basic_main_parser_t {
   public:
    cli_parser_t() = default;

    explicit cli_parser_t(const cli_parser_t &) = delete;
    explicit cli_parser_t(cli_parser_t &&) noexcept = delete;
    cli_parser_t &operator=(const cli_parser_t &) = delete;
    cli_parser_t &operator=(cli_parser_t &&) noexcept = delete;

    ~cli_parser_t() override;

    // if help is given, help is printed and exit(EXIT_SUCCESS) is called
    clapp::help_option_t help{*this, std::vector<std::string>{"help", "usage"},
                              std::vector<char>{'h', '?'},
                              "Show help options."};

    // if version is given, print_version_and_exit() is called.
    clapp::bool_option_t version{
        *this, std::vector<std::string>{"version", "vers"}, 'v',
        "Show version info",
        clapp::value::found_func_t{print_version_and_exit}};

    // first mandatory argument of string
    clapp::string_argument_t string_arg{*this, "string-arg", "String argument"};

    // second mandatory argument of int32_t
    clapp::int32_argument_t int_arg{
        *this, "int-arg", "Int argument",
        clapp::min_max_value_t<std::int32_t>{5, 10}};

    // third optional variadic arguments of strings
    clapp::variadic_string_argument_t variadic_string_arg{
        *this, "variadic-string-arg", "Variadic String argument",
        purpose_t::optional};

    // mandatory string option
    clapp::string_param_option_t string_param{
        *this, "string", std::vector<char>{'s', '1'}, "String option 1.",
        purpose_t::mandatory};

    // optional string option (multiple string vectors)
    clapp::vector_string_param_option_t string_vector_param{
        *this, "string-vector", "String vector param."};
};

clapp::value::found_func_t::ret_t print_version_and_exit(
    const std::string & /*option*/) {
    std::cout << clapp::build_info::build_info_string << std::endl;
    return clapp::value::exit_t::exit(EXIT_SUCCESS);
}

cli_parser_t::~cli_parser_t() = default;

using parser_t = clapp::parser::basic_parser_container_t<cli_parser_t>;

int main(int argc, char *argv[]) {
    try {
        parser_t parser;
        const std::optional<clapp::value::exit_t> exit{
            parser.parse_and_validate(argc, argv)};
        if (exit) {
            return exit.value().get_exit_code();
        }

        Expects(
            parser
                ->string_arg);  // parser ensures mandatory arguments are given
        std::cout << "string-arg: " << parser->string_arg.value() << std::endl;

        Expects(
            parser->int_arg);  // parser ensures mandatory arguments are given
        std::cout << "int-arg: " << parser->int_arg.value() << std::endl;

        if (parser->variadic_string_arg) {  // if variadic_string_arg is given
            std::cout << "variadic-string-arg (size: "
                      << parser->variadic_string_arg.value().size() << "): ";
            // iterate over the vector of arguments
            for (auto &val : parser->variadic_string_arg.value()) {
                std::cout << val << ", ";
            }
            std::cout << std::endl;
        } else {
            std::cout << "variadic-string-arg: not given" << std::endl;
        }

        Expects(parser->string_param);  // The parser ensures that
                                        // mandatory options are given
        std::cout << "string_param: '" << parser->string_param.value() << "'"
                  << std::endl;

        if (parser->string_vector_param) {  // if string_vector_param is given
            std::cout << "string_vector_param (size: "
                      << parser->string_vector_param.value().size() << "): ";
            // iterate over the vector of options
            for (auto &val : parser->string_vector_param.value()) {
                std::cout << val << ", ";
            }
            std::cout << std::endl;
        } else {
            std::cout << "string_vector_param: not given" << std::endl;
        }
    } catch (clapp::exception::clapp_exception_t &e) {
        std::cout << "Caught ClaPP-Exception: " << e.what() << std::endl;
        return EXIT_FAILURE;
    } catch (std::exception &e) {
        std::cout << "Caught Exception: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
