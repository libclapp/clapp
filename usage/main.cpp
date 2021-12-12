#include <clapp/argument.h>
#include <clapp/build_info.h>
#include <clapp/exception.h>
#include <clapp/filesystem.h>
#include <clapp/main_parser.h>
#include <clapp/option.h>
#include <clapp/parser.h>
#include <clapp/parser_container.h>
#include <clapp/sub_parser.h>
#include <clapp/type_traits.h>
#include <clapp/value.h>

clapp::value::found_func_t::ret_t print_version_and_exit(
    const std::string &option);

class cli_parser_t : public clapp::basic_main_parser_t {
   public:
    cli_parser_t() = default;

    ~cli_parser_t() override;

    clapp::help_option_t help{*this, "help", 'h', "Show help options."};

    clapp::bool_option_t version{
        *this, std::vector<std::string>{"version", "vers"}, 'v',
        "Show version info",
        clapp::value::found_func_t{print_version_and_exit}};

    clapp::string_argument_t string_arg{*this, "string-arg", "String argument"};

    explicit cli_parser_t(const cli_parser_t &) = delete;
    explicit cli_parser_t(cli_parser_t &&) noexcept = delete;
    cli_parser_t &operator=(const cli_parser_t &) = delete;
    cli_parser_t &operator=(cli_parser_t &&) noexcept = delete;
};

cli_parser_t::~cli_parser_t() = default;

clapp::value::found_func_t::ret_t print_version_and_exit(
    const std::string & /*option*/) {
    std::cout << clapp::build_info::build_info_string << std::endl;
    return clapp::value::exit_t::exit(EXIT_SUCCESS);
}

using parser_t = clapp::parser::basic_parser_container_t<cli_parser_t>;

int main(int argc, char *argv[]) {
    try {
        parser_t p;
        const std::optional<clapp::value::exit_t> exit{
            p.parse_and_validate(argc, argv)};
        if (exit) {
            return exit.value().get_exit_code();
        }
        Ensures(p->string_arg);
        std::cout << "string-arg: " << p->string_arg.value() << std::endl;
    } catch (std::exception &e) {
        std::cout << "Caught Exception: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
