#include <clapp/argument.h>
#include <clapp/build_info.h>
#include <clapp/main_parser.h>
#include <clapp/option.h>
#include <clapp/sub_parser.h>
#include <unistd.h>

#include <iostream>

[[noreturn]] void print_version_and_exit();

[[noreturn]] void print_version_and_exit() {
    std::cout << clapp::build_info::build_info_string << std::endl;
    clapp::basic_main_parser_t::exit(EXIT_SUCCESS);
}

class cli_parser_t : public clapp::basic_main_parser_t {
   public:
    using clapp::basic_main_parser_t::basic_main_parser_t;
    cli_parser_t() = default;

    cli_parser_t(int argc, const char *const *argv) : cli_parser_t{} {
        parse_and_validate(argc, argv);
    }

    explicit cli_parser_t(const cli_parser_t &) = delete;
    explicit cli_parser_t(cli_parser_t &&) noexcept = delete;
    cli_parser_t &operator=(const cli_parser_t &) = delete;
    cli_parser_t &operator=(cli_parser_t &&) noexcept = delete;

    ~cli_parser_t() override;

    // if help is given, help is printed and exit(EXIT_SUCCESS) is called
    clapp::help_option_t help{*this, "help", 'h', "Show help options."};

    // if version is given, print_version_and_exit() is called.
    clapp::bool_option_t version{
        *this, "version", "Show version info",
        clapp::value::found_func_t{print_version_and_exit}};

    clapp::count_option_t verbose{*this,
                                  "verbose",
                                  'v',
                                  "Verbose option.",
                                  clapp::min_max_value_t<std::size_t>{0, 7},
                                  clapp::default_value_t<std::size_t>{2}};

    // create a subparser with a registered help option
    class sub_parser_t : public clapp::basic_sub_parser_t {
       protected:
        using clapp::basic_sub_parser_t::basic_sub_parser_t;

       public:
        explicit sub_parser_t(const sub_parser_t &) = delete;
        explicit sub_parser_t(sub_parser_t &&) noexcept = delete;
        sub_parser_t &operator=(const sub_parser_t &) = delete;
        sub_parser_t &operator=(sub_parser_t &&) noexcept = delete;

        ~sub_parser_t() override;

        clapp::help_option_t help{*this, "help", 'h', "Show help options."};
    };

    // first subparser declaration (inherits help from sub_parser_t)
    class first_parser_t : public sub_parser_t {
       public:
        using sub_parser_t::sub_parser_t;

        explicit first_parser_t(const first_parser_t &) = delete;
        explicit first_parser_t(first_parser_t &&) noexcept = delete;
        first_parser_t &operator=(const first_parser_t &) = delete;
        first_parser_t &operator=(first_parser_t &&) noexcept = delete;

        ~first_parser_t() override;

        clapp::bool_option_t short_bool{*this, 'b', "Short bool option.",
                                        purpose_t::mandatory};

        clapp::double_param_option_t double_opt{
            *this, 'd', "Double param option",
            clapp::default_value_t<double>{10}};

        clapp::string_param_option_t string{*this, 's', "String param option."};

        clapp::string_argument_t string_arg_x{
            *this, "string-arg-x", "String argument x", purpose_t::optional,
            clapp::default_value_t<std::string>{"abaa"}};
    };

    // second subparser declaration (inherits help from sub_parser_t)
    class second_parser_t : public sub_parser_t {
       public:
        using sub_parser_t::sub_parser_t;

        explicit second_parser_t(const second_parser_t &) = delete;
        explicit second_parser_t(second_parser_t &&) noexcept = delete;
        second_parser_t &operator=(const second_parser_t &) = delete;
        second_parser_t &operator=(second_parser_t &&) noexcept = delete;

        ~second_parser_t() override;

        clapp::int32_argument_t int_arg{
            *this, "int-arg", "Int argument",
            clapp::min_max_value_t<std::int32_t>{5, 10}};

        clapp::double_argument_t double_arg{
            *this, "double-arg", "Double argument",
            clapp::min_max_value_t<double>{-1000., 1000.}};

        clapp::string_argument_t string_arg_x{
            *this, "string-arg-x", "String argument x", purpose_t::optional,
            clapp::default_value_t<std::string>{"default-string-arg-x"}};

        clapp::variadic_int64_argument_t variadic_int_arg{
            *this, "variadic-int-arg", "Int argument", purpose_t::optional,
            clapp::min_max_value_t<std::int64_t>{5, 10}};
    };

    first_parser_t first{*this, "first", "First parser."};
    second_parser_t second{*this, "second", "Second parser."};
};

cli_parser_t::~cli_parser_t() = default;
cli_parser_t::sub_parser_t::~sub_parser_t() = default;
cli_parser_t::first_parser_t::~first_parser_t() = default;
cli_parser_t::second_parser_t::~second_parser_t() = default;

void process_first(const cli_parser_t::first_parser_t &first);
void process_second(const cli_parser_t::second_parser_t &second);

void process_first(const cli_parser_t::first_parser_t &first) {
    Expects(first.short_bool);  // parser ensures mandatory options are given
    std::cout << "short_bool: " << first.short_bool.value() << "\n";

    if (first.string) {  // if optional string option is given
        std::cout << "string: " << first.string.value() << "\n";
    } else {
        std::cout << "string: not given\n";
    }

    if (first.string_arg_x) {
        std::cout << "string-arg-x: " << first.string_arg_x.value() << "\n";
    } else {
        std::cout << "string-arg-x: not given\n";
    }

    Expects(first.double_opt);
    std::cout << "double-opt: " << first.double_opt.value() << "\n";
}

void process_second(const cli_parser_t::second_parser_t &second) {
    Expects(second.int_arg);  // parser ensures mandatory arguments are given
    std::cout << "int-arg: " << second.int_arg.value() << "\n";

    Expects(second.double_arg);  // parser ensures mandatory arguments are given
    std::cout << "double-arg: " << second.double_arg.value() << "\n";

    if (second.string_arg_x) {  // if optional string-arg-x option is given
        std::cout << "string-arg-x: " << second.string_arg_x.value() << "\n";
    } else {
        std::cout << "string-arg-x: not given\n";
    }

    if (second.variadic_int_arg) {  // if optional variadic int argument given
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
    try {
        cli_parser_t cp{argc, argv};

        if (cp.verbose) {  // if the optional verbose option is given
            std::cout << "verbose: " << cp.verbose.value() << "\n";
        } else {
            std::cout << "verbose: not given\n";
        }

        if (cp.first) {  // if the first sub-parser is selected
            std::cout << "first parser active" << std::endl;
            process_first(cp.first);
        } else {
            std::cout << "first parser not active" << std::endl;
        }

        if (cp.second) {  // if the first sub-parser is selected
            std::cout << "second parser active" << std::endl;
            process_second(cp.second);
        } else {
            std::cout << "second parser not active" << std::endl;
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
