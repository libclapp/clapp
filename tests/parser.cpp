#include <clapp/argument.h>
#include <clapp/main_parser.h>
#include <clapp/option.h>
#include <clapp/parser.h>
#include <clapp/sub_parser.h>
#include <gmock/gmock.h>

template <class T, size_t N>
inline clapp::parser::arg_t parser_make_arg_t(T (&arg)[N]) {
    return clapp::parser::arg_t{static_cast<const char* const*>(arg), N - 1};
}

class empty_basic_parser_t : public clapp::basic_parser_t {
   public:
    using clapp::basic_parser_t::basic_parser_t;
    ~empty_basic_parser_t() override;
};

empty_basic_parser_t::~empty_basic_parser_t() = default;

class simple_test_parser_t : public clapp::basic_parser_t {
   public:
    using clapp::basic_parser_t::basic_parser_t;
    ~simple_test_parser_t() override;

    clapp::option::bool_option_t bool_option{*this, "bool", 'b',
                                             "Bool option."};

    clapp::option::int32_param_option_t int_option{
        *this, "int", 'i', "Int option.",
        clapp::value::min_max_value_t<std::int32_t>{10, 200}};

    clapp::argument::string_argument_t string_arg{*this, "arg-name",
                                                  "Arg desc"};

    clapp::argument::variadic_string_argument_t variadic_string_arg{
        *this, "variadic-arg-name", "Variadic arg desc",
        clapp::basic_parser_t::purpose_t::optional};
};

simple_test_parser_t::~simple_test_parser_t() = default;

class simple_test_parser2_t : public clapp::basic_parser_t {
   public:
    using clapp::basic_parser_t::basic_parser_t;
    ~simple_test_parser2_t() override;

    clapp::option::bool_option_t count_option{
        *this, "count", 'c', "Count option.",
        clapp::basic_parser_t::purpose_t::mandatory};

    clapp::argument::string_argument_t string_arg{
        *this, "arg-name", "Arg desc",
        clapp::basic_parser_t::purpose_t::optional};
};

simple_test_parser2_t::~simple_test_parser2_t() = default;

class simple_test_parser3_t : public clapp::basic_parser_t {
   public:
    using clapp::basic_parser_t::basic_parser_t;
    ~simple_test_parser3_t() override;

    clapp::option::vector_int64_param_option_t int_option{
        *this,
        "int",
        'i',
        "Int option.",
        clapp::value::min_max_value_t<std::int64_t>{10, 200},
        clapp::basic_parser_t::purpose_t::mandatory};

    clapp::option::vector_string_param_option_t string_option{
        *this, "str", 's', "String option.",
        clapp::basic_parser_t::purpose_t::optional};

    clapp::argument::variadic_string_argument_t variadic_string_arg{
        *this, "variadic-arg-name", "Variadic arg desc",
        clapp::basic_parser_t::purpose_t::mandatory};
};

simple_test_parser3_t::~simple_test_parser3_t() = default;

class sub_parser_container_t : public clapp::basic_parser_t {
   public:
    using clapp::basic_parser_t::basic_parser_t;
    ~sub_parser_container_t() override;

    clapp::option::bool_option_t bool_option{*this, "bool", 'b',
                                             "Bool option."};

    clapp::option::bool_option_t bool_option2{*this, "second", '2',
                                              "Second bool option."};

    clapp::argument::string_argument_t string_arg{*this, "arg-name",
                                                  "Arg desc"};

    class simple_sub_parser_t : public clapp::parser::basic_sub_parser_t {
       public:
        ~simple_sub_parser_t() override;
        clapp::option::bool_option_t bool_option{*this, "bool", 'b',
                                                 "Bool option."};

        using clapp::parser::basic_sub_parser_t::basic_sub_parser_t;
        clapp::option::string_param_option_t string_option{*this, "string", 's',
                                                           "String option."};

        clapp::argument::string_argument_t string_arg{
            *this, "sub-arg-name", "Sub arg desc",
            clapp::basic_parser_t::purpose_t::optional};
    };

    simple_sub_parser_t sub_parser{*this, "sub-parser", "Sub parser desc"};
};

sub_parser_container_t::simple_sub_parser_t::~simple_sub_parser_t() = default;
sub_parser_container_t::~sub_parser_container_t() = default;

TEST(parser, construct_empty_basic_parser) { empty_basic_parser_t ebp; }

TEST(parser, construct_empty_basic_parser_and_parse_empty_arguments) {
    constexpr const char* const argv[]{nullptr};
    const clapp::parser::arg_t arg{parser_make_arg_t(argv)};
    empty_basic_parser_t ebp;
    ASSERT_NO_THROW(ebp.parse(arg.cbegin(), arg.cend()));
}

TEST(parser,
     construct_empty_basic_parser_and_parse_and_validate_empty_arguments) {
    constexpr const char* const argv[]{nullptr};
    const clapp::parser::arg_t arg{parser_make_arg_t(argv)};
    empty_basic_parser_t ebp;
    ASSERT_NO_THROW(ebp.parse(arg.cbegin(), arg.cend()));
    ASSERT_NO_THROW(ebp.validate());
}

TEST(parser, construct_empty_basic_parser_and_gen_help_prefix) {
    empty_basic_parser_t ebp;
    ASSERT_THAT(ebp.gen_help_prefix(), testing::StrEq("Usage: \n"));
}

TEST(parser, construct_empty_basic_parser_and_gen_help_message) {
    empty_basic_parser_t ebp;
    ASSERT_THAT(ebp.gen_help_msg(), testing::StrEq("\n"));
}

TEST(parser, gen_func_print_help_and_exit) {
    empty_basic_parser_t ebp;
    clapp::value::found_func_t ff{
        ebp.gen_func_print_help_and_exit(EXIT_SUCCESS)};
    testing::internal::CaptureStdout();
    ASSERT_EXIT(ff.found(), ::testing::ExitedWithCode(0), "");
    std::string output = testing::internal::GetCapturedStdout();
    ASSERT_THAT(output, testing::StrEq("Usage: \n \n"));
}

TEST(parser, construct_simple_test_parser_and_gen_help_message) {
    simple_test_parser_t stp;
    ASSERT_THAT(
        stp.gen_help_msg(),
        testing::StrEq(
            "[-b|--bool] [-i|--int=<arg>] <arg-name> [<variadic-arg-name>...] "
            "\n\nMandatory Arguments:\n  arg-name          Arg desc\n\n"
            "Optional Arguments:\n  variadic-arg-name Variadic arg desc "
            "(variadic argument)\n\n"
            "Optional Options:\n  -b|--bool         Bool option.\n"
            "  -i|--int=<arg>    Int option. (constraint: [10,200])\n"));
}

TEST(parser, construct_simple_test_parser2_and_gen_help_message) {
    simple_test_parser2_t stp;
    ASSERT_THAT(
        stp.gen_help_msg(),
        testing::StrEq("-c|--count [<arg-name>] \n\n"
                       "Optional Arguments:\n  arg-name   Arg desc\n\n"
                       "Mandatory Options:\n  -c|--count Count option.\n"));
}

TEST(parser, construct_simple_test_parser3_and_gen_help_message) {
    simple_test_parser3_t stp;
    ASSERT_THAT(
        stp.gen_help_msg(),
        testing::StrEq(
            "-i|--int=<arg>... [-s|--str=<arg>...] <variadic-arg-name>... \n\n"
            "Mandatory Arguments:\n  variadic-arg-name Variadic arg desc "
            "(variadic argument)\n\n"
            "Mandatory Options:\n  -i|--int=<arg>    Int option. (vector "
            "option, constraint: [10,200])\n\n"
            "Optional Options:\n  -s|--str=<arg>    String option. (vector "
            "option)\n"));
}

TEST(parser, construct_sub_parser_container_and_gen_help_message) {
    sub_parser_container_t spc;
    ASSERT_THAT(spc.gen_help_msg(),
                testing::StrEq(
                    "[-b|--bool] [-2|--second] <arg-name> <sub-parser> "
                    "[sub-parser args/opts...] \n\n"
                    "Mandatory Arguments:\n  arg-name    Arg desc\n\n"
                    "Available sub-parsers:\n  sub-parser  Sub parser desc\n\n"
                    "Optional Options:\n  -b|--bool   Bool option.\n  "
                    "-2|--second Second bool option.\n"));
}

TEST(parser, construct_sub_parser_container_and_gen_sub_parser_help_message) {
    sub_parser_container_t spc;
    ASSERT_THAT(spc.sub_parser.gen_help_msg(),
                testing::StrEq(
                    "[-b|--bool] [-s|--string=<arg>] [<sub-arg-name>] \n\n"
                    "Optional Arguments:\n  sub-arg-name      Sub arg desc\n\n"
                    "Optional Options:\n  -b|--bool         Bool option.\n"
                    "  -s|--string=<arg> String option.\n"));
}

TEST(parser, construct_empty_basic_parser_and_parse_unknown_arguments_throws) {
    constexpr const char* const argv[]{"unknown-argument", nullptr};
    const clapp::parser::arg_t arg{parser_make_arg_t(argv)};
    empty_basic_parser_t ebp;
    ASSERT_THROW(ebp.parse(arg.cbegin(), arg.cend()),
                 clapp::exception::clapp_exception_t);
}

TEST(parser,
     construct_empty_basic_parser_and_parse_unknown_long_option_throws) {
    constexpr const char* const argv[]{"--long-option", nullptr};
    const clapp::parser::arg_t arg{parser_make_arg_t(argv)};
    empty_basic_parser_t ebp;
    ASSERT_THROW(ebp.parse(arg.cbegin(), arg.cend()),
                 clapp::exception::option_exception_t);
}

TEST(parser,
     construct_empty_basic_parser_and_parse_unknown_short_option_throws) {
    constexpr const char* const argv[]{"-s", nullptr};
    const clapp::parser::arg_t arg{parser_make_arg_t(argv)};
    empty_basic_parser_t ebp;
    ASSERT_THROW(ebp.parse(arg.cbegin(), arg.cend()),
                 clapp::exception::option_exception_t);
}

TEST(
    parser,
    construct_simple_test_parser_and_parse_long_bool_option_with_param_throws) {
    constexpr const char* const argv[]{"--bool=param", nullptr};
    const clapp::parser::arg_t arg{parser_make_arg_t(argv)};
    simple_test_parser_t ebp;
    ASSERT_THROW(ebp.parse(arg.cbegin(), arg.cend()),
                 clapp::exception::option_param_exception_t);
}

TEST(parser,
     construct_simple_test_parser_and_parse_short_option_with_param_throws) {
    constexpr const char* const argv[]{"-b=param", nullptr};
    const clapp::parser::arg_t arg{parser_make_arg_t(argv)};
    simple_test_parser_t ebp;
    ASSERT_THROW(ebp.parse(arg.cbegin(), arg.cend()),
                 clapp::exception::option_param_exception_t);
}

TEST(
    parser,
    construct_simple_test_parser_and_parse_long_int_option_without_param_throws) {
    constexpr const char* const argv[]{"--int", nullptr};
    const clapp::parser::arg_t arg{parser_make_arg_t(argv)};
    simple_test_parser_t ebp;
    ASSERT_THROW(ebp.parse(arg.cbegin(), arg.cend()),
                 clapp::exception::option_param_exception_t);
}

TEST(
    parser,
    construct_simple_test_parser_and_parse_short_int_option_without_param_throws) {
    constexpr const char* const argv[]{"-i", nullptr};
    const clapp::parser::arg_t arg{parser_make_arg_t(argv)};
    simple_test_parser_t ebp;
    ASSERT_THROW(ebp.parse(arg.cbegin(), arg.cend()),
                 clapp::exception::option_param_exception_t);
}

TEST(
    parser,
    construct_simple_test_parser_and_parse_short_int_option_without_param_throws2) {
    constexpr const char* const argv[]{"-ib", nullptr};
    const clapp::parser::arg_t arg{parser_make_arg_t(argv)};
    simple_test_parser_t ebp;
    ASSERT_THROW(ebp.parse(arg.cbegin(), arg.cend()),
                 clapp::exception::option_param_exception_t);
}

TEST(
    parser,
    construct_simple_test_parser_parse_without_argument_and_validate_recursive_throws) {
    constexpr const char* const argv[]{nullptr};
    const clapp::parser::arg_t arg{parser_make_arg_t(argv)};
    simple_test_parser_t ebp;
    ASSERT_NO_THROW(ebp.parse(arg.cbegin(), arg.cend()));
    ASSERT_THAT(ebp.get_num_processed_arguments(), testing::Eq(0));
    ASSERT_THROW(ebp.validate_recursive(),
                 clapp::exception::argument_exception_t);
}

TEST(parser,
     construct_simple_test_parser_parse_argument_and_validate_recursive) {
    constexpr const char* const argv[]{"argument", nullptr};
    const clapp::parser::arg_t arg{parser_make_arg_t(argv)};
    simple_test_parser_t ebp;
    ASSERT_NO_THROW(ebp.parse(arg.cbegin(), arg.cend()));
    ASSERT_THAT(static_cast<bool>(ebp.bool_option), testing::Eq(false));
    ASSERT_THAT(static_cast<bool>(ebp.int_option), testing::Eq(false));
    ASSERT_THAT(static_cast<bool>(ebp.string_arg), testing::Eq(true));
    ASSERT_THAT(ebp.string_arg.value(), testing::Eq("argument"));
    ASSERT_THAT(static_cast<bool>(ebp.variadic_string_arg), testing::Eq(false));
    ASSERT_THAT(ebp.get_num_processed_arguments(), testing::Eq(1));
    ASSERT_NO_THROW(ebp.validate_recursive());
}

TEST(
    parser,
    construct_simple_test_parser_parse_argument_and_short_option_without_params_and_validate_recursive) {
    constexpr const char* const argv[]{"-b", "arg", nullptr};
    const clapp::parser::arg_t arg{parser_make_arg_t(argv)};
    simple_test_parser_t ebp;
    ASSERT_NO_THROW(ebp.parse(arg.cbegin(), arg.cend()));
    ASSERT_THAT(static_cast<bool>(ebp.bool_option), testing::Eq(true));
    ASSERT_THAT(static_cast<bool>(ebp.int_option), testing::Eq(false));
    ASSERT_THAT(static_cast<bool>(ebp.string_arg), testing::Eq(true));
    ASSERT_THAT(ebp.string_arg.value(), testing::StrEq("arg"));
    ASSERT_THAT(static_cast<bool>(ebp.variadic_string_arg), testing::Eq(false));
    ASSERT_THAT(ebp.get_num_processed_arguments(), testing::Eq(1));
    ASSERT_NO_THROW(ebp.validate_recursive());
}

TEST(
    parser,
    construct_simple_test_parser_parse_argument_and_long_option_with_param_and_validate) {
    constexpr const char* const argv[]{"--int", "123", "arg", nullptr};
    const clapp::parser::arg_t arg{parser_make_arg_t(argv)};
    simple_test_parser_t ebp;
    ASSERT_NO_THROW(ebp.parse(arg.cbegin(), arg.cend()));
    ASSERT_THAT(static_cast<bool>(ebp.bool_option), testing::Eq(false));
    ASSERT_THAT(static_cast<bool>(ebp.int_option), testing::Eq(true));
    ASSERT_THAT(ebp.int_option.value(), testing::Eq(123));
    ASSERT_THAT(static_cast<bool>(ebp.string_arg), testing::Eq(true));
    ASSERT_THAT(ebp.string_arg.value(), testing::StrEq("arg"));
    ASSERT_THAT(static_cast<bool>(ebp.variadic_string_arg), testing::Eq(false));
    ASSERT_THAT(ebp.get_num_processed_arguments(), testing::Eq(1));
    ASSERT_NO_THROW(ebp.validate_recursive());
}

TEST(
    parser,
    construct_simple_test_parser_parse_argument_variadic_argument_and_long_option_with_param_and_validate) {
    constexpr const char* const argv[]{"--int=123", "arg", "varg0", "varg1",
                                       nullptr};
    const clapp::parser::arg_t arg{parser_make_arg_t(argv)};
    simple_test_parser_t ebp;
    ASSERT_NO_THROW(ebp.parse(arg.cbegin(), arg.cend()));
    ASSERT_THAT(static_cast<bool>(ebp.bool_option), testing::Eq(false));
    ASSERT_THAT(static_cast<bool>(ebp.int_option), testing::Eq(true));
    ASSERT_THAT(ebp.int_option.value(), testing::Eq(123));
    ASSERT_THAT(static_cast<bool>(ebp.string_arg), testing::Eq(true));
    ASSERT_THAT(ebp.string_arg.value(), testing::StrEq("arg"));
    ASSERT_THAT(static_cast<bool>(ebp.variadic_string_arg), testing::Eq(true));
    ASSERT_THAT(ebp.variadic_string_arg.value().size(), testing::Eq(2));
    ASSERT_THAT(ebp.variadic_string_arg.value()[0], testing::StrEq("varg0"));
    ASSERT_THAT(ebp.variadic_string_arg.value()[1], testing::StrEq("varg1"));
    ASSERT_THAT(ebp.get_num_processed_arguments(), testing::Eq(3));
    ASSERT_NO_THROW(ebp.validate_recursive());
}

TEST(
    parser,
    construct_simple_test_parser_parse_argument_and_short_options_and_validate) {
    constexpr const char* const argv[]{"-bi=123", "arg", nullptr};
    const clapp::parser::arg_t arg{parser_make_arg_t(argv)};
    simple_test_parser_t ebp;
    ASSERT_NO_THROW(ebp.parse(arg.cbegin(), arg.cend()));
    ASSERT_THAT(static_cast<bool>(ebp.bool_option), testing::Eq(true));
    ASSERT_THAT(ebp.bool_option.value(), testing::Eq(true));
    ASSERT_THAT(static_cast<bool>(ebp.int_option), testing::Eq(true));
    ASSERT_THAT(ebp.int_option.value(), testing::Eq(123));
    ASSERT_THAT(static_cast<bool>(ebp.string_arg), testing::Eq(true));
    ASSERT_THAT(ebp.string_arg.value(), testing::StrEq("arg"));
    ASSERT_THAT(ebp.get_num_processed_arguments(), testing::Eq(1));
    ASSERT_NO_THROW(ebp.validate_recursive());
}

TEST(
    parser,
    construct_simple_test_parser2_and_parse_long_count_option_with_param_throws) {
    constexpr const char* const argv[]{"--count=param", nullptr};
    const clapp::parser::arg_t arg{parser_make_arg_t(argv)};
    simple_test_parser2_t ebp;
    ASSERT_THROW(ebp.parse(arg.cbegin(), arg.cend()),
                 clapp::option_param_exception_t);
}

TEST(parser,
     construct_simple_test_parser2_parse_without_mandatory_option_throws) {
    constexpr const char* const argv[]{nullptr};
    const clapp::parser::arg_t arg{parser_make_arg_t(argv)};
    simple_test_parser2_t ebp;
    ASSERT_NO_THROW(ebp.parse(arg.cbegin(), arg.cend()));
    ASSERT_THAT(static_cast<bool>(ebp.count_option), testing::Eq(false));
    ASSERT_THROW(ebp.validate(), clapp::option_param_exception_t);
}

TEST(parser,
     construct_simple_test_parser2_parse_option_without_param_and_validate) {
    constexpr const char* const argv[]{"-c", nullptr};
    const clapp::parser::arg_t arg{parser_make_arg_t(argv)};
    simple_test_parser2_t ebp;
    ASSERT_NO_THROW(ebp.parse(arg.cbegin(), arg.cend()));
    ASSERT_THAT(static_cast<bool>(ebp.count_option), testing::Eq(true));
    ASSERT_THAT(ebp.count_option.value(), testing::Eq(1));
    ASSERT_NO_THROW(ebp.validate());
}

TEST(
    parser,
    construct_simple_test_parser2_parse_option_without_param_optional_argument_and_validate) {
    constexpr const char* const argv[]{"-c", "opt-arg", nullptr};
    const clapp::parser::arg_t arg{parser_make_arg_t(argv)};
    simple_test_parser2_t ebp;
    ASSERT_NO_THROW(ebp.parse(arg.cbegin(), arg.cend()));
    ASSERT_THAT(static_cast<bool>(ebp.count_option), testing::Eq(true));
    ASSERT_THAT(ebp.count_option.value(), testing::Eq(1));
    ASSERT_THAT(static_cast<bool>(ebp.string_arg), testing::Eq(true));
    ASSERT_THAT(ebp.string_arg.value(), testing::StrEq("opt-arg"));
    ASSERT_NO_THROW(ebp.validate());
}

TEST(parser, construct_sub_parser_container_parse_subparser_and_validate) {
    constexpr const char* const argv[]{"string-arg", "sub-parser", "-bs=param",
                                       nullptr};
    const clapp::parser::arg_t arg{parser_make_arg_t(argv)};
    sub_parser_container_t spc;
    ASSERT_NO_THROW(spc.parse(arg.cbegin(), arg.cend()));
    ASSERT_THAT(static_cast<bool>(spc.string_arg), testing::Eq(true));
    ASSERT_THAT(spc.string_arg.value(), testing::StrEq("string-arg"));
    ASSERT_THAT(static_cast<bool>(spc.bool_option), testing::Eq(false));
    ASSERT_THAT(static_cast<bool>(spc.bool_option2), testing::Eq(false));
    ASSERT_THAT(static_cast<bool>(spc.sub_parser.bool_option),
                testing::Eq(true));
    ASSERT_THAT(spc.sub_parser.bool_option.value(), testing::Eq(true));
    ASSERT_THAT(static_cast<bool>(spc.sub_parser.string_option),
                testing::Eq(true));
    ASSERT_THAT(spc.sub_parser.string_option.value(), testing::StrEq("param"));
    ASSERT_NO_THROW(spc.validate());
}

TEST(parser, construct_sub_parser_container_parse_subparser_and_validate2) {
    constexpr const char* const argv[]{"string-arg", "-b", "sub-parser", "-s",
                                       "param",      "-2", nullptr};
    const clapp::parser::arg_t arg{parser_make_arg_t(argv)};
    sub_parser_container_t spc;
    ASSERT_NO_THROW(spc.parse(arg.cbegin(), arg.cend()));
    ASSERT_THAT(static_cast<bool>(spc.string_arg), testing::Eq(true));
    ASSERT_THAT(spc.string_arg.value(), testing::StrEq("string-arg"));
    ASSERT_THAT(static_cast<bool>(spc.bool_option), testing::Eq(true));
    ASSERT_THAT(spc.bool_option.value(), testing::Eq(true));
    ASSERT_THAT(static_cast<bool>(spc.bool_option2), testing::Eq(true));
    ASSERT_THAT(spc.bool_option2.value(), testing::Eq(true));
    ASSERT_THAT(static_cast<bool>(spc.sub_parser.bool_option),
                testing::Eq(false));
    ASSERT_THAT(static_cast<bool>(spc.sub_parser.string_option),
                testing::Eq(true));
    ASSERT_THAT(spc.sub_parser.string_option.value(), testing::StrEq("param"));
    ASSERT_NO_THROW(spc.validate());
}
