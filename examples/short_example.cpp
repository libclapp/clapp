#include <clapp/argument.h>
#include <clapp/main_parser.h>
#include <clapp/option.h>
#include <iostream>

class cli_parser_t : public clapp::basic_main_parser_t {
   public:
    using clapp::basic_main_parser_t::basic_main_parser_t;
    ~cli_parser_t();
    cli_parser_t(int argc, const char *const *argv) : cli_parser_t{} {
        parse(argc, argv);
    }

    clapp::bool_option_t help{*this, "help", 'h', "Show help options."};

    clapp::string_argument_t string_arg{*this, "string-arg", "String argument"};

    clapp::int32_argument_t int_arg{
        *this, "int-arg", "Int argument",
        clapp::min_max_value_t{std::int32_t{5}, std::int32_t{10}}};

    clapp::variadic_string_argument_t variadic_string_arg{
        *this, "variadic-string-arg", "Variadic String argument",
        purpose_t::optional};

    clapp::string_param_option_t string_param{
        *this, "string", 's', "String option 1.", purpose_t::mandatory};

    clapp::vector_string_param_option_t string_vector_param{
        *this, "string-vector", "String vector param."};
};

cli_parser_t::~cli_parser_t() = default;

int main(int argc, char *argv[]) {
    cli_parser_t cp{argc, argv};

    if (cp.help) {
        std::cout << "Usage: \n"
                  << cp.get_executable() << ' ' << cp.gen_help_msg();
        return EXIT_SUCCESS;
    }

    cp.validate();

    if (cp.string_arg) {
        std::cout << "string-arg: " << cp.string_arg.value() << "\n";
    } else {
        std::cout << "string-arg: not given\n";
    }

    if (cp.int_arg) {
        std::cout << "int-arg: " << cp.int_arg.value() << "\n";
    } else {
        std::cout << "int-arg: not given\n";
    }

    if (cp.variadic_string_arg) {
        std::cout << "variadic-string-arg (size: "
                  << cp.variadic_string_arg.value().size() << "): ";
        for (auto &val : cp.variadic_string_arg.value()) {
            std::cout << val << ", ";
        }
        std::cout << "\n";
    } else {
        std::cout << "variadic-string-arg: not given\n";
    }

    if (cp.string_param) {
        std::cout << "string_param: '" << cp.string_param.value() << "'\n";
    } else {
        std::cout << "string_param: not given\n";
    }

    if (cp.string_vector_param) {
        std::cout << "string_vector_param (size: "
                  << cp.string_vector_param.value().size() << "): ";
        for (auto &val : cp.string_vector_param.value()) {
            std::cout << val << ", ";
        }
        std::cout << "\n";

    } else {
        std::cout << "string_vector_param: not given\n";
    }
}
