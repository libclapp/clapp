#include <clapp/argument.h>
#include <clapp/build_info.h>
#include <clapp/main_parser.h>
#include <clapp/option.h>
#include <clapp/parser_container.h>
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

        class and_option_container_t : public clapp::option_container_t {
           public:
            using clapp::option_container_t::option_container_t;

            ~and_option_container_t() override;

            clapp::bool_option_t short_bool{*this, 'b', "Short bool option.",
                                            purpose_t::mandatory};

            // mandatory string option
            clapp::string_param_option_t string_param{
                *this, "string", std::vector<char>{'s', '1'},
                "String option 1.", purpose_t::mandatory};

            // optional string option (multiple string vectors)
            clapp::vector_string_param_option_t string_vector_param{
                *this, "string-vector", "String vector param.",
                purpose_t::optional};

            explicit and_option_container_t(const and_option_container_t &) =
                delete;
            explicit and_option_container_t(
                and_option_container_t &&) noexcept = delete;
            and_option_container_t &operator=(const and_option_container_t &) =
                delete;
            and_option_container_t &operator=(
                and_option_container_t &&) noexcept = delete;
        };

        and_option_container_t options{
            *this, clapp::parser::types::logic_operator_type_t::logic_and};

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

        clapp::bool_option_t help{*this, "help", 'h', "Show help options.",
                                  purpose_t::optional};

        clapp::bool_option_t short_bool{*this, 'b', "Short bool option.",
                                        purpose_t::optional};

        string_argument_t string_arg_x{
            *this, "string-arg-x", "String argument x", purpose_t::optional,
            clapp::default_value_t<std::string>{"default-string-arg-x"}};
        variadic_int_argument_t int_arg{
            *this, "variadic-int-arg", "Int argument", purpose_t::optional,
            clapp::min_max_value_t<std::int32_t>{5, 10}};
    };

    cmd1_parser_t cmd1{*this, "cmd1", "First usable command.",
                       clapp::parser::types::logic_operator_type_t::logic_xor};
    cmd2_parser_t cmd2{*this, "cmd2", "Second usable command.",
                       clapp::parser::types::logic_operator_type_t::logic_and};

    string_argument_t string_arg{*this, "string-arg", "String argument"};

    clapp::help_option_t help{*this, "help", 'h', "Show help options.",
                              purpose_t::optional};
    clapp::bool_option_t short_bool{*this, 'b', "Short bool option.",
                                    purpose_t::optional};
    clapp::bool_option_t long_bool{*this, "long-bool", "Long bool option.",
                                   purpose_t::optional};
    clapp::bool_option_t restricted_bool{*this,
                                         "restricted",
                                         'r',
                                         "restricted bool option.",
                                         restriction_t{},
                                         no_action_t{},
                                         purpose_t::optional};
    clapp::bool_option_t mandatory_bool{
        *this, 'o', "Mandatory short bool option.", purpose_t::mandatory};
    clapp::bool_option_t mandatory_restricted_bool{
        *this,
        "mandatory-restricted",
        'm',
        "Mandatory restricted bool option.",
        purpose_t::mandatory,
        restriction_t{}};

    class time_container_t : public clapp::option_container_t {
       public:
        using clapp::option_container_t::option_container_t;

        explicit time_container_t(const time_container_t &) = delete;
        explicit time_container_t(time_container_t &&) noexcept = delete;
        time_container_t &operator=(const time_container_t &) = delete;
        time_container_t &operator=(time_container_t &&) noexcept = delete;

        ~time_container_t() override;

        clapp::hours_param_option_t time_hours{
            *this, "time-hours", "Time in hours.", purpose_t::mandatory};
        clapp::min_param_option_t time_minutes{
            *this, "time-min", "Time in minutes.", purpose_t::mandatory};
        clapp::sec_param_option_t time_seconds{
            *this, "time-sec", "Time in seconds.", purpose_t::mandatory,
            clapp::min_max_value_t<std::chrono::seconds>{
                std::chrono::seconds{0}, std::chrono::seconds{7}}};
    };

    time_container_t time{
        *this, clapp::parser::types::logic_operator_type_t::logic_xor};

    clapp::count_option_t verbose{*this,
                                  "verbose",
                                  'v',
                                  "Verbose option.",
                                  clapp::min_max_value_t<std::size_t>{0, 7},
                                  clapp::default_value_t<std::size_t>{2},
                                  purpose_t::optional};
    clapp::count_option_t count{*this, 'c', "Count option ",
                                purpose_t::mandatory};

    string_param_t string_param{*this, "string", 's', "String option 1.",
                                purpose_t::optional};

    string_vector_param_t string_vector_param{
        *this, "string-vector", "String vector param.", purpose_t::optional};

#ifdef CLAPP_FS_AVAIL
    clapp::path_param_option_t test_file{*this, "test-file", "Test File.",
                                         clapp::path_exists_t{},
                                         purpose_t::optional};
#endif

    entry_param_t entry_param{*this,
                              "entry",
                              'e',
                              "Entry option 1. ",
                              entry_value_constraint_t{},
                              purpose_t::mandatory};

    int32_param_t mandatory_int{*this, "mandatory-int", "Mandatory Int option.",
                                purpose_t::mandatory, purpose_t::optional};
    int32_param_t default_int{*this, "default-int", "Default Int option.",
                              clapp::default_value_t<std::int32_t>{10},
                              purpose_t::optional};
    int32_param_t optional_int{*this, "optional-int", "Optional Int option.",
                               purpose_t::optional};

    int32_param_t constrained_int{*this,
                                  "constrained-int",
                                  'f',
                                  "Constrained default Int option.",
                                  15,
                                  clapp::min_max_value_t<std::int32_t>{10, 20},
                                  purpose_t::optional};
};

std::ostream &operator<<(std::ostream &out, const entry_t &entry);

template <>
entry_t clapp::value::convert_value<entry_t>(std::string_view param);

static void process_options_and_args(const cli_parser_t &parser);
static void process_time_container(const cli_parser_t::time_container_t &cont);
static void process_cmd1(const cli_parser_t::cmd1_parser_t &cmd1);
static void process_cmd2(const cli_parser_t::cmd2_parser_t &cmd2);

cli_parser_t::~cli_parser_t() = default;
cli_parser_t::cmd1_parser_t::~cmd1_parser_t() = default;
cli_parser_t::cmd1_parser_t::and_option_container_t::~and_option_container_t() =
    default;
cli_parser_t::cmd2_parser_t::~cmd2_parser_t() = default;
cli_parser_t::time_container_t::~time_container_t() = default;

std::ostream &operator<<(std::ostream &out, const entry_t &entry) {
    if (entry == entry_t::entry1) {
        out << "entry1";
    } else if (entry == entry_t::entry2) {
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

void process_time_container(const cli_parser_t::time_container_t &cont) {
    std::cout << "time-container: ";
    if (cont.time_hours) {
        std::cout << "time-hours: " << cont.time_hours.value().count() << "\n";
    } else if (cont.time_minutes) {
        std::cout << "time-minutes: " << cont.time_minutes.value().count()
                  << "\n";
    } else if (cont.time_seconds) {
        std::cout << "time-seconds: " << cont.time_seconds.value().count()
                  << "\n";
    }
}

void process_cmd1(const cli_parser_t::cmd1_parser_t &cmd1) {
    if (cmd1.help.value()) {
        std::cout << "Usage: \n"
                  << cmd1.get_sub_parser_name() << ' ' << cmd1.gen_help_msg(0);
        return;
    }

    cmd1.validate();

    if (cmd1.options.string_param) {
        std::cout << "string: " << cmd1.options.string_param.value() << "\n";
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
    std::cout << "short-bool: " << cmd2.short_bool.value() << std::endl;

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

void process_options_and_args(const cli_parser_t &parser) {
    if (parser.string_arg) {
        std::cout << "string-arg: " << parser.string_arg.value() << "\n";
    } else {
        std::cout << "string-arg: not given\n";
    }

    if (parser.verbose.given()) {
        std::cout << "verbose: " << parser.verbose.value() << "\n";
    } else {
        std::cout << "verbose: not given\n";
    }

    if (parser.string_param) {
        std::cout << "string_param: '" << parser.string_param.value() << "'\n";
    } else {
        std::cout << "string_param: not given\n";
    }

    if (parser.string_vector_param) {
        std::cout << "string_vector_param (size: "
                  << parser.string_vector_param.value().size() << "): ";
        for (auto &val : parser.string_vector_param.value()) {
            std::cout << val << ", ";
        }
        std::cout << "\n";

    } else {
        std::cout << "string_vector_param: not given\n";
    }

    if (parser.short_bool.given()) {
        std::cout << "short_bool: given\n";
    } else {
        std::cout << "short_bool: not given\n";
    }

    if (parser.restricted_bool.given()) {
        std::cout << "restricted_bool: given\n";
    } else {
        std::cout << "restricted_bool: not given\n";
    }

    if (parser.long_bool.given()) {
        std::cout << "long_bool: " << parser.long_bool.value() << "\n";
    } else {
        std::cout << "long_bool: not given\n";
    }

    if (parser.count.given()) {
        std::cout << "count: " << parser.count.value() << "\n";
    } else {
        std::cout << "count: not given\n";
    }

#ifdef CLAPP_FS_AVAIL
    if (parser.test_file) {
        std::cout << "test-file: " << parser.test_file.value() << "\n";
    } else {
        std::cout << "test-file: not given\n";
    }
#else
    std::cout << "without fs\n";
#endif

    if (parser.constrained_int) {
        std::cout << "constrained_int: " << parser.constrained_int.value()
                  << "\n";
    } else {
        std::cout << "constrained_int: not given\n";
    }
    std::cout << "mandatory_bool: " << parser.mandatory_bool.value() << "\n";

    std::cout << "mandatory_int: " << parser.mandatory_int.value() << "\n";
    std::cout << "default_int: " << parser.default_int.value() << "\n";

    if (parser.optional_int) {
        std::cout << "optional_int: " << parser.optional_int.value() << "\n";
    }

    std::cout << "entry_param: '" << parser.entry_param.value() << "'\n";
}

using parser_t = clapp::parser::basic_parser_container_t<cli_parser_t>;

int main(int argc, char *argv[]) {
    try {
        std::cout << clapp::build_info::build_info_string << std::endl;
        parser_t parser;
        const std::optional<clapp::value::exit_t> exit{
            parser.parse_and_validate(argc, argv)};
        if (exit) {
            return exit.value().get_exit_code();
        }

        if (!*parser) {
            std::cout << "Parsing failed!" << std::endl;
            return EXIT_FAILURE;
        }

        if (parser->help.value()) {
            std::cout << "Usage: \n"
                      << parser->get_executable() << ' '
                      << parser->gen_help_msg(0);
            return EXIT_SUCCESS;
        }

        parser->validate();

        process_options_and_args(*parser);

        process_time_container(parser->time);

        if (parser->cmd1) {
            std::cout << "cmd1 given" << std::endl;
            process_cmd1(parser->cmd1);
        } else {
            std::cout << "cmd1 not given" << std::endl;
        }

        if (parser->cmd2) {
            std::cout << "cmd2 given" << std::endl;
            process_cmd2(parser->cmd2);
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
