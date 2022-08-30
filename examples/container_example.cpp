#include <clapp/build_info.h>
#include <clapp/main_parser.h>
#include <clapp/option.h>
#include <clapp/parser_container.h>
#include <clapp/sub_parser.h>
#include <unistd.h>

#include <iostream>

clapp::value::found_func_t::ret_t print_version_and_exit(
    const std::string &option);

class cli_parser_t : public clapp::basic_main_parser_t {
   public:
    cli_parser_t();

    explicit cli_parser_t(const cli_parser_t &) = delete;
    explicit cli_parser_t(cli_parser_t &&) noexcept = delete;
    cli_parser_t &operator=(const cli_parser_t &) = delete;
    cli_parser_t &operator=(cli_parser_t &&) noexcept = delete;

    ~cli_parser_t() override;

    // if help is given, help is printed and exit(EXIT_SUCCESS) is requested
    clapp::help_option_t help{*this, "help", 'h', "Show help options."};

    // if version is given, print_version_and_exit() is called.
    clapp::bool_option_t version{
        *this, "version", "Show version info",
        clapp::value::found_func_t{print_version_and_exit}};

    class and_option_container_t : public clapp::option_container_t {
       public:
        using clapp::option_container_t::option_container_t;

        ~and_option_container_t() override;

        clapp::count_option_t verbose{*this,
                                      "verbose",
                                      'v',
                                      "Verbose option, can be given multiple "
                                      "times to increase verbosity.",
                                      clapp::min_max_value_t<std::size_t>{0, 7},
                                      clapp::default_value_t<std::size_t>{2},
                                      purpose_t::optional};

        // mandatory string option
        clapp::string_param_option_t string_param{
            *this, "string", std::vector<char>{'s', '1'}, "String option 1.",
            purpose_t::mandatory};

        class xor_option_container_t : public clapp::option_container_t {
           public:
            using clapp::option_container_t::option_container_t;

            ~xor_option_container_t() override;

            clapp::int32_param_option_t int32{*this, "i32", "Int32 option."};

            clapp::uint32_param_option_t uint32{*this, "u32", "UInt32 option."};

            explicit xor_option_container_t(const xor_option_container_t &) =
                delete;
            explicit xor_option_container_t(
                xor_option_container_t &&) noexcept = delete;
            xor_option_container_t &operator=(const xor_option_container_t &) =
                delete;
            xor_option_container_t &operator=(
                xor_option_container_t &&) noexcept = delete;
        };

        xor_option_container_t int_options{
            *this, clapp::parser::types::logic_operator_type_t::logic_xor};

        explicit and_option_container_t(const and_option_container_t &) =
            delete;
        explicit and_option_container_t(and_option_container_t &&) noexcept =
            delete;
        and_option_container_t &operator=(const and_option_container_t &) =
            delete;
        and_option_container_t &operator=(and_option_container_t &&) noexcept =
            delete;
    };

    and_option_container_t options{
        *this, clapp::parser::types::logic_operator_type_t::logic_and};
};

clapp::value::found_func_t::ret_t print_version_and_exit(
    const std::string & /*option*/) {
    std::cout << clapp::build_info::build_info_string << std::endl;
    return clapp::value::exit_t::exit(EXIT_SUCCESS);
}

cli_parser_t::cli_parser_t()
    : clapp::basic_main_parser_t{
          clapp::parser::types::logic_operator_type_t::logic_xor} {}

cli_parser_t::~cli_parser_t() = default;
cli_parser_t::and_option_container_t::~and_option_container_t() = default;
cli_parser_t::and_option_container_t::xor_option_container_t::
    ~xor_option_container_t() = default;

using parser_t = clapp::parser::basic_parser_container_t<cli_parser_t>;

int main(int argc, char **argv) {
    try {
        parser_t parser;
        const std::optional<clapp::value::exit_t> exit{
            parser.parse_and_validate(argc, argv)};
        if (exit) {
            return exit.value().get_exit_code();
        }

        if (parser->options.verbose
                .given()) {  // if the optional verbose-option is given
            std::cout << "verbose: " << parser->options.verbose.value() << "\n";
        } else {
            std::cout << "verbose: not given\n";
        }

        Expects(
            parser->options
                .string_param);  // parser ensures, that string-option is given
        std::cout << "string-param: " << parser->options.string_param.value()
                  << std::endl;
        if (parser->options.int_options.int32) {
            std::cout << "int32-param: "
                      << parser->options.int_options.int32.value() << std::endl;
        } else {
            std::cout << "uint32-param: "
                      << parser->options.int_options.uint32.value()
                      << std::endl;
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
