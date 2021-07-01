#include <clapp/argument.h>
#include <clapp/build_info.h>
#include <clapp/main_parser.h>
#include <clapp/option.h>
#include <clapp/sub_parser.h>
#include <clapp/value.h>

#include <iostream>

using string_argument_t = clapp::basic_argument_t<std::string>;
using variadic_string_argument_t =
    clapp::basic_variadic_argument_t<std::string>;
using int_argument_t = clapp::basic_argument_t<std::int32_t>;
using variadic_int_argument_t = clapp::basic_variadic_argument_t<std::int32_t>;

enum class entry_t { entry1, entry2 };

class entry_value_constraint_t {
   public:
    [[nodiscard]] static std::string append_restriction_text() {
        return "possible values: entry1, entry2";
    }
};

using string_param_t = clapp::basic_param_option_t<std::string>;

using string_vector_param_t = clapp::basic_vector_param_option_t<std::string>;

using int32_param_t = clapp::basic_param_option_t<std::int32_t>;

using entry_param_t = clapp::basic_param_option_t<entry_t>;

using entry_argument_t = clapp::basic_argument_t<entry_t>;

struct no_action_t {};

struct restriction_t {
    [[nodiscard]] static std::string append_restriction_text() {
        return "restriction";
    }
};

class cli_parser_t : public clapp::basic_main_parser_t {
   public:
    cli_parser_t() = default;

    cli_parser_t(int argc, const char *const *argv) : cli_parser_t{} {
        parse(argc, argv);
    }

    explicit cli_parser_t(const cli_parser_t &) = delete;
    explicit cli_parser_t(cli_parser_t &&) noexcept = delete;
    cli_parser_t &operator=(const cli_parser_t &) = delete;
    cli_parser_t &operator=(cli_parser_t &&) noexcept = delete;

    ~cli_parser_t() override;

    class cmd1_parser_t : public clapp::basic_sub_parser_t {
       public:
        using clapp::basic_sub_parser_t::basic_sub_parser_t;

        explicit cmd1_parser_t(const cmd1_parser_t &) = delete;
        explicit cmd1_parser_t(cmd1_parser_t &&) noexcept = delete;
        cmd1_parser_t &operator=(const cmd1_parser_t &) = delete;
        cmd1_parser_t &operator=(cmd1_parser_t &&) noexcept = delete;

        ~cmd1_parser_t() override;
        clapp::bool_option_t help{*this, "help", 'h', "Show help options."};
        clapp::bool_option_t short_bool{*this, 'b', "Short bool option.",
                                        purpose_t::mandatory};
        string_param_t string{*this, 's', "String param option."};

        entry_argument_t entry_arg{*this, "entry-arg", "Entry argument",
                                   entry_value_constraint_t{}};

        int_argument_t int_arg{*this, "int-arg", "Int argument",
                               clapp::min_max_value_t<std::int32_t>{5, 10}};
        string_argument_t string_arg_x{
            *this, "string-arg-x", "String argument x", purpose_t::optional,
            clapp::default_value_t<std::string>{"abaa"}};
    };

    class cmd2_parser_t : public clapp::basic_sub_parser_t {
       public:
        using clapp::basic_sub_parser_t::basic_sub_parser_t;

        explicit cmd2_parser_t(const cmd2_parser_t &) = delete;
        explicit cmd2_parser_t(cmd2_parser_t &&) noexcept = delete;
        cmd2_parser_t &operator=(const cmd2_parser_t &) = delete;
        cmd2_parser_t &operator=(cmd2_parser_t &&) noexcept = delete;

        ~cmd2_parser_t() override;
        clapp::bool_option_t help{*this, "help", 'h', "Show help options."};
        string_argument_t string_arg_x{
            *this, "string-arg-x", "String argument x", purpose_t::optional,
            clapp::default_value_t<std::string>{"default-string-arg-x"}};
        variadic_int_argument_t int_arg{
            *this, "variadic-int-arg", "Int argument", purpose_t::optional,
            clapp::min_max_value_t<std::int32_t>{5, 10}};
        // string_argument_t string_arg_y{*this, "string-arg-y", "String
        // argument x", purpose_t::optional};//TODO: ensure that this throws
    };

    cmd1_parser_t cmd1{*this, "cmd1", "First usable command."};
    cmd2_parser_t cmd2{*this, "cmd2", "Second usable command."};

    string_argument_t string_arg{*this, "string-arg", "String argument"};
    // string_argument_t string_arg_2{*this, "string-arg-2", "String argument
    // 2", purpose_t::optional};//TODO: ensure that this throws...

    clapp::help_option_t help{*this, "help", 'h', "Show help options."};
    clapp::bool_option_t short_bool{*this, 'b', "Short bool option."};
    clapp::bool_option_t long_bool{*this, "long-bool", "Long bool option."};
    clapp::bool_option_t restricted_bool{
        *this,           "restricted", 'r', "restricted bool option.",
        restriction_t{}, no_action_t{}};
    clapp::bool_option_t mandatory_bool{
        *this, 'o', "Mandatory short bool option.", purpose_t::mandatory};
    clapp::bool_option_t mandatory_restricted_bool{
        *this,
        "mandatory-restricted",
        'm',
        "Mandatory restricted bool option.",
        purpose_t::mandatory,
        restriction_t{}};

    clapp::hours_param_option_t hours{
        *this, "hours", "hours option.",
        clapp::default_value_t<std::chrono::hours>{std::chrono::hours{100}}};
    clapp::min_param_option_t minutes{
        *this, "minutes", "minutes option.",
        clapp::min_max_value_t<std::chrono::minutes>{std::chrono::minutes{0},
                                                     std::chrono::minutes{7}}};
    clapp::sec_param_option_t seconds{*this, "seconds", "seconds option."};
    clapp::ms_param_option_t milliseconds{*this, "milliseconds",
                                          "milliseconds option."};
    clapp::us_param_option_t microseconds{
        *this, "microseconds", "microseconds option.",
        clapp::not_null_value_t<std::chrono::microseconds>{}};
    clapp::ns_param_option_t nanoseconds{*this, "nanoseconds",
                                         "nanoseconds option."};

    clapp::count_option_t verbose{*this,
                                  "verbose",
                                  'v',
                                  "Verbose option.",
                                  clapp::min_max_value_t<std::size_t>{0, 7},
                                  clapp::default_value_t<std::size_t>{2}};
    clapp::count_option_t count{*this, 'c', "Count option ",
                                purpose_t::mandatory};

    string_param_t string_param{*this, "string", 's', "String option 1."};

    string_vector_param_t string_vector_param{*this, "string-vector",
                                              "String vector param."};

#ifdef CLAPP_FS_AVAIL
    clapp::path_param_option_t test_file{*this, "test-file", "Test File.",
                                         clapp::path_exists_t {}};
#endif

    entry_param_t entry_param{*this,
                              "entry",
                              'e',
                              "Entry option 1. ",
                              entry_value_constraint_t{},
                              purpose_t::mandatory};

    int32_param_t mandatory_int{*this, "mandatory-int", "Mandatory Int option.",
                                purpose_t::mandatory};
    int32_param_t default_int{*this, "default-int", "Default Int option.",
                              clapp::default_value_t<std::int32_t>{10}};
    int32_param_t optional_int{*this, "optional-int", "Optional Int option."};

    int32_param_t constrained_int{
        *this, "constrained-int",
        'f',   "Constrained default Int option.",
        15,    clapp::min_max_value_t<std::int32_t>{10, 20}};
};

std::ostream &operator<<(std::ostream &out, const entry_t &e);

template <>
entry_t clapp::value::convert_value<entry_t>(std::string_view param);

static void process_options_and_args(const cli_parser_t &cp);
static void process_cmd1(const cli_parser_t::cmd1_parser_t &cmd1);
static void process_cmd2(const cli_parser_t::cmd2_parser_t &cmd2);

cli_parser_t::~cli_parser_t() = default;
cli_parser_t::cmd1_parser_t::~cmd1_parser_t() = default;
cli_parser_t::cmd2_parser_t::~cmd2_parser_t() = default;

std::ostream &operator<<(std::ostream &out, const entry_t &e) {
    if (e == entry_t::entry1) {
        out << "entry1";
    } else if (e == entry_t::entry2) {
        out << "entry2";
    }
    return out;
}

template <>
entry_t clapp::value::convert_value<entry_t>(const std::string_view param) {
    if (param == "entry1") {
        return entry_t::entry1;
    }
    if (param == "entry2") {
        return entry_t::entry2;
    }
    throw std::runtime_error("Invalid enum param type.");
}

void process_cmd1(const cli_parser_t::cmd1_parser_t &cmd1) {
    if (cmd1.help.value()) {
        std::cout << "Usage: \n"
                  << cmd1.get_sub_parser_name() << ' ' << cmd1.gen_help_msg(0);
        return;
    }

    cmd1.validate();

    if (cmd1.string) {
        std::cout << "string: " << cmd1.string.value() << "\n";
    } else {
        std::cout << "string: not given\n";
    }

    if (cmd1.int_arg) {
        std::cout << "int-arg: " << cmd1.int_arg.value() << "\n";
    } else {
        std::cout << "int-arg: not given\n";
    }

    if (cmd1.string_arg_x) {
        std::cout << "string-arg-x: " << cmd1.string_arg_x.value() << "\n";
    } else {
        std::cout << "string-arg-x: not given\n";
    }

    if (cmd1.entry_arg) {
        std::cout << "entry-arg: " << cmd1.entry_arg.value() << "\n";
    } else {
        std::cout << "entry-arg: not given\n";
    }
}

void process_cmd2(const cli_parser_t::cmd2_parser_t &cmd2) {
    if (cmd2.help.value()) {
        std::cout << "Usage: \n"
                  << cmd2.get_sub_parser_name() << ' ' << cmd2.gen_help_msg(0);
        return;
    }

    cmd2.validate();

    if (cmd2.string_arg_x) {
        std::cout << "string-arg-x: " << cmd2.string_arg_x.value() << "\n";
    } else {
        std::cout << "string-arg-x: not given\n";
    }

    if (cmd2.int_arg) {
        std::cout << "int_arg (size: " << cmd2.int_arg.value().size() << "): ";
        for (auto &val : cmd2.int_arg.value()) {
            std::cout << val << ", ";
        }
        std::cout << "\n";
    } else {
        std::cout << "int_arg: not given\n";
    }
}

void process_options_and_args(const cli_parser_t &cp) {
    if (cp.string_arg) {
        std::cout << "string-arg: " << cp.string_arg.value() << "\n";
    } else {
        std::cout << "string-arg: not given\n";
    }

    if (cp.verbose.given()) {
        std::cout << "verbose: " << cp.verbose.value() << "\n";
    } else {
        std::cout << "verbose: not given\n";
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

    if (cp.short_bool.given()) {
        std::cout << "short_bool: given\n";
    } else {
        std::cout << "short_bool: not given\n";
    }

    if (cp.restricted_bool.given()) {
        std::cout << "restricted_bool: given\n";
    } else {
        std::cout << "restricted_bool: not given\n";
    }

    if (cp.long_bool.given()) {
        std::cout << "long_bool: " << cp.long_bool.value() << "\n";
    } else {
        std::cout << "long_bool: not given\n";
    }

    if (cp.count.given()) {
        std::cout << "count: " << cp.count.value() << "\n";
    } else {
        std::cout << "count: not given\n";
    }

#ifdef CLAPP_FS_AVAIL
    if (cp.test_file) {
        std::cout << "test-file: " << cp.test_file.value() << "\n";
    } else {
        std::cout << "test-file: not given\n";
    }
#else
    std::cout << "without fs\n";
#endif

    if (cp.constrained_int) {
        std::cout << "constrained_int: " << cp.constrained_int.value() << "\n";
    } else {
        std::cout << "constrained_int: not given\n";
    }
    std::cout << "mandatory_bool: " << cp.mandatory_bool.value() << "\n";

    std::cout << "mandatory_int: " << cp.mandatory_int.value() << "\n";
    std::cout << "default_int: " << cp.default_int.value() << "\n";

    if (cp.optional_int) {
        std::cout << "optional_int: " << cp.optional_int.value() << "\n";
    }

    std::cout << "entry_param: '" << cp.entry_param.value() << "'\n";
}

int main(int argc, char *argv[]) {
    try {
        std::cout << clapp::build_info::build_info_string << std::endl;
        // create parser and parse the args
        // cli_parser_t cp;
        // cp.parse(argc, argv);
        // or do it all in one.
        cli_parser_t cp{argc, argv};

        if (!cp) {
            std::cout << "Parsing failed!" << std::endl;
            return EXIT_FAILURE;
        }

        if (cp.help.value()) {
            std::cout << "Usage: \n"
                      << cp.get_executable() << ' ' << cp.gen_help_msg(0);
            return EXIT_SUCCESS;
        }

        cp.validate();

        process_options_and_args(cp);

        if (cp.cmd1) {
            std::cout << "cmd1 given" << std::endl;
            process_cmd1(cp.cmd1);
        } else {
            std::cout << "cmd1 not given" << std::endl;
        }

        if (cp.cmd2) {
            std::cout << "cmd2 given" << std::endl;
            process_cmd2(cp.cmd2);
        } else {
            std::cout << "cmd2 not given" << std::endl;
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
