#include <clapp/argument.h>
#include <clapp/main_parser.h>
#include <clapp/option.h>
#include <clapp/sub_parser.h>
#include <iostream>

class cli_parser_t : public clapp::basic_main_parser_t {
   public:
    using clapp::basic_main_parser_t::basic_main_parser_t;
    cli_parser_t(int argc, const char *const *argv) : cli_parser_t{} {
        parse(argc, argv);
    }

    ~cli_parser_t();

    clapp::bool_option_t help{*this, "help", 'h', "Show help options."};

    clapp::count_option_t verbose{
        *this,
        "verbose",
        'v',
        "Verbose option.",
        clapp::min_max_value_t{std::size_t{0}, std::size_t{7}},
        clapp::default_value_t{std::size_t{2}}};

    class first_parser_t : public clapp::basic_sub_parser_t {
       public:
        using clapp::basic_sub_parser_t::basic_sub_parser_t;
        ~first_parser_t();
        clapp::bool_option_t help{*this, "help", 'h', "Show help options."};
        clapp::bool_option_t short_bool{*this, 'b', "Short bool option.",
                                        purpose_t::mandatory};

        clapp::string_param_option_t string{*this, 's', "String param option."};

        clapp::string_argument_t string_arg_x{
            *this, "string-arg-x", "String argument x", purpose_t::optional,
            clapp::default_value_t{"abaa"}};
    };

    class second_parser_t : public clapp::basic_sub_parser_t {
       public:
        using clapp::basic_sub_parser_t::basic_sub_parser_t;
        ~second_parser_t();
        clapp::bool_option_t help{*this, "help", 'h', "Show help options."};

        clapp::int32_argument_t int_arg{
            *this, "int-arg", "Int argument",
            clapp::min_max_value_t{std::int32_t{5}, std::int32_t{10}}};

        clapp::string_argument_t string_arg_x{
            *this, "string-arg-x", "String argument x", purpose_t::optional,
            clapp::default_value_t{"default-string-arg-x"}};

        clapp::variadic_int64_argument_t variadic_int_arg{
            *this, "variadic-int-arg", "Int argument", purpose_t::optional,
            clapp::min_max_value_t{std::int64_t{5}, std::int64_t{10}}};
    };

    first_parser_t first{*this, "first", "First parser."};
    second_parser_t second{*this, "second", "Second parser."};
};

cli_parser_t::~cli_parser_t() = default;
cli_parser_t::first_parser_t::~first_parser_t() = default;
cli_parser_t::second_parser_t::~second_parser_t() = default;

void process_first(const cli_parser_t::first_parser_t &first);
void process_second(const cli_parser_t::second_parser_t &second);

void process_first(const cli_parser_t::first_parser_t &first) {
    if (first.help) {
        std::cout << "Usage: \n"
                  << first.get_sub_parser_name() << ' ' << first.gen_help_msg();
        return;
    }

    first.validate();

    if (first.short_bool) {
        std::cout << "short_bool: " << first.short_bool.value() << "\n";
    } else {
        std::cout << "short_bool: not given\n";
    }

    if (first.string) {
        std::cout << "string: " << first.string.value() << "\n";
    } else {
        std::cout << "string: not given\n";
    }

    if (first.string_arg_x) {
        std::cout << "string-arg-x: " << first.string_arg_x.value() << "\n";
    } else {
        std::cout << "string-arg-x: not given\n";
    }
}

void process_second(const cli_parser_t::second_parser_t &second) {
    if (second.help) {
        std::cout << "Usage: \n"
                  << second.get_sub_parser_name() << ' '
                  << second.gen_help_msg();
        return;
    }

    second.validate();

    if (second.int_arg) {
        std::cout << "int-arg: " << second.int_arg.value() << "\n";
    } else {
        std::cout << "int-arg: not given\n";
    }

    if (second.string_arg_x) {
        std::cout << "string-arg-x: " << second.string_arg_x.value() << "\n";
    } else {
        std::cout << "string-arg-x: not given\n";
    }

    if (second.variadic_int_arg) {
        std::cout << "variadic-int-arg (size: "
                  << second.variadic_int_arg.value().size() << "): ";
        for (auto &val : second.variadic_int_arg.value()) {
            std::cout << val << ", ";
        }
        std::cout << "\n";
    } else {
        std::cout << "variadic-int-arg: not given\n";
    }
}

int main(int argc, char *argv[]) {
    cli_parser_t cp{argc, argv};

    if (cp.help) {
        std::cout << "Usage: \n"
                  << cp.get_executable() << ' ' << cp.gen_help_msg();
        return EXIT_SUCCESS;
    }

    cp.validate();

    if (cp.verbose) {
        std::cout << "verbose: " << cp.verbose.value() << "\n";
    } else {
        std::cout << "verbose: not given\n";
    }

    if (cp.first) {
        std::cout << "first parser active" << std::endl;
        process_first(cp.first);
    } else {
        std::cout << "first parser not active" << std::endl;
    }

    if (cp.second) {
        std::cout << "second parser active" << std::endl;
        process_second(cp.second);
    } else {
        std::cout << "second parser not active" << std::endl;
    }
}
