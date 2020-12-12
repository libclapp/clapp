#include <clapp/argument.h>
#include <clapp/main_parser.h>
#include <clapp/option.h>
#include <clapp/parser.h>
#include <clapp/sub_parser.h>
#include <gmock/gmock.h>

TEST(help_entry_t, Construct) {
    clapp::parser::basic_parser_t::help_entry_t he{"option-string",
                                                   "description"};
}

TEST(help_entry_t, ConstructAndCompare) {
    const std::string option_str{"option-string"};
    const std::string description_str{"desc"};
    const std::string option_str2{"os2"};
    const std::string description_str2{"ds2"};
    clapp::parser::basic_parser_t::help_entry_t he{option_str, description_str};
    ASSERT_THAT((clapp::parser::basic_parser_t::help_entry_t{option_str,
                                                             description_str}),
                testing::Eq(he));
    ASSERT_THAT((clapp::parser::basic_parser_t::help_entry_t{option_str2,
                                                             description_str}),
                testing::Ne(he));
    ASSERT_THAT((clapp::parser::basic_parser_t::help_entry_t{option_str2,
                                                             description_str2}),
                testing::Ne(he));
    ASSERT_THAT((clapp::parser::basic_parser_t::help_entry_t{option_str,
                                                             description_str2}),
                testing::Ne(he));
}

TEST(purpose_t, ToCstring) {
    ASSERT_THAT(clapp::parser::basic_parser_t::to_cstring(
                    clapp::parser::basic_parser_t::purpose_t::optional),
                testing::StrEq("optional"));
    ASSERT_THAT(clapp::parser::basic_parser_t::to_cstring(
                    clapp::parser::basic_parser_t::purpose_t::optional),
                testing::StrEq("optional"));
}

class print_and_exit_t {
   public:
    MOCK_METHOD2(print_and_exit, void(const std::string_view print_msg,
                                      std::optional<int> exit_code));
};

template <class T, size_t N>
inline clapp::parser::arg_t parser_make_arg_t(T (&arg)[N]) {
    return clapp::parser::arg_t{static_cast<const char* const*>(arg), N - 1};
}

class empty_basic_parser_t : public clapp::basic_parser_t {
   public:
    using clapp::basic_parser_t::basic_parser_t;
    ~empty_basic_parser_t() override;
    [[nodiscard]] std::string gen_short_line_prefix() const override;
    void set_max_option_string_size(std::size_t max_option_size) override;
    [[nodiscard]] std::size_t get_max_option_string_size() const override;

   private:
    std::size_t max_option_string_size{0};
};

empty_basic_parser_t::~empty_basic_parser_t() = default;

[[nodiscard]] std::string empty_basic_parser_t::gen_short_line_prefix() const {
    return "arg-test-parser" + gen_short_line();
}

void empty_basic_parser_t::set_max_option_string_size(
    const std::size_t max_option_size) {
    max_option_string_size = max_option_size;
}

[[nodiscard]] std::size_t empty_basic_parser_t::get_max_option_string_size()
    const {
    return max_option_string_size;
}

class simple_test_parser_t : public clapp::basic_parser_t {
   public:
    using clapp::basic_parser_t::basic_parser_t;
    ~simple_test_parser_t() override;

    static constexpr std::int32_t min_int{10};
    static constexpr std::int32_t max_int{200};

    clapp::option::bool_option_t bool_option{*this, "bool", 'b',
                                             "Bool option."};

    clapp::option::int32_param_option_t int_option{
        *this, "int", 'i', "Int option.",
        clapp::value::min_max_value_t<std::int32_t>{min_int, max_int}};

    clapp::argument::string_argument_t string_arg{*this, "arg-name",
                                                  "Arg desc"};

    clapp::argument::variadic_string_argument_t variadic_string_arg{
        *this, "variadic-arg-name", "Variadic arg desc",
        clapp::basic_parser_t::purpose_t::optional};

    [[nodiscard]] std::string gen_short_line_prefix() const override;
    void set_max_option_string_size(std::size_t max_option_size) override;
    [[nodiscard]] std::size_t get_max_option_string_size() const override;

   private:
    std::size_t max_option_string_size{0};
};

simple_test_parser_t::~simple_test_parser_t() = default;

[[nodiscard]] std::string simple_test_parser_t::gen_short_line_prefix() const {
    return "simple-test-parser" + gen_short_line();
}

void simple_test_parser_t::set_max_option_string_size(
    const std::size_t max_option_size) {
    max_option_string_size = max_option_size;
}

[[nodiscard]] std::size_t simple_test_parser_t::get_max_option_string_size()
    const {
    return max_option_string_size;
}

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

    [[nodiscard]] std::string gen_short_line_prefix() const override;
    void set_max_option_string_size(std::size_t max_option_size) override;
    [[nodiscard]] std::size_t get_max_option_string_size() const override;

   private:
    std::size_t max_option_string_size{0};
};

simple_test_parser2_t::~simple_test_parser2_t() = default;

[[nodiscard]] std::string simple_test_parser2_t::gen_short_line_prefix() const {
    return "simple-test-parser2" + gen_short_line();
}

void simple_test_parser2_t::set_max_option_string_size(
    const std::size_t max_option_size) {
    max_option_string_size = max_option_size;
}

[[nodiscard]] std::size_t simple_test_parser2_t::get_max_option_string_size()
    const {
    return max_option_string_size;
}

class simple_test_parser3_t : public clapp::basic_parser_t {
   public:
    using clapp::basic_parser_t::basic_parser_t;
    ~simple_test_parser3_t() override;

    static constexpr std::int32_t min_int{10};
    static constexpr std::int32_t max_int{200};

    clapp::option::vector_int64_param_option_t int_option{
        *this,
        "int",
        'i',
        "Int option.",
        clapp::value::min_max_value_t<std::int64_t>{min_int, max_int},
        clapp::basic_parser_t::purpose_t::mandatory};

    clapp::option::vector_string_param_option_t string_option{
        *this, "str", 's', "String option.",
        clapp::basic_parser_t::purpose_t::optional};

    clapp::argument::variadic_string_argument_t variadic_string_arg{
        *this, "variadic-arg-name", "Variadic arg desc",
        clapp::basic_parser_t::purpose_t::mandatory};

    [[nodiscard]] std::string gen_short_line_prefix() const override;
    void set_max_option_string_size(std::size_t max_option_size) override;
    [[nodiscard]] std::size_t get_max_option_string_size() const override;

   private:
    std::size_t max_option_string_size{0};
};

simple_test_parser3_t::~simple_test_parser3_t() = default;

[[nodiscard]] std::string simple_test_parser3_t::gen_short_line_prefix() const {
    return "simple-test-parser3" + gen_short_line();
}

void simple_test_parser3_t::set_max_option_string_size(
    const std::size_t max_option_size) {
    max_option_string_size = max_option_size;
}

[[nodiscard]] std::size_t simple_test_parser3_t::get_max_option_string_size()
    const {
    return max_option_string_size;
}

class sub_parser_container_t : public clapp::basic_parser_t {
   public:
    using clapp::basic_parser_t::basic_parser_t;
    ~sub_parser_container_t() override;

    [[nodiscard]] std::string gen_short_line_prefix() const override;
    void set_max_option_string_size(std::size_t max_option_size) override;
    [[nodiscard]] std::size_t get_max_option_string_size() const override;

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

   private:
    std::size_t max_option_string_size{0};
};

sub_parser_container_t::simple_sub_parser_t::~simple_sub_parser_t() = default;
sub_parser_container_t::~sub_parser_container_t() = default;

[[nodiscard]] std::string sub_parser_container_t::gen_short_line_prefix()
    const {
    return "sub_parser_container" + gen_short_line();
}

void sub_parser_container_t::set_max_option_string_size(
    const std::size_t max_option_size) {
    max_option_string_size = max_option_size;
}

[[nodiscard]] std::size_t sub_parser_container_t::get_max_option_string_size()
    const {
    return max_option_string_size;
}

TEST(parser, constructEmptyBasicParser) { empty_basic_parser_t ebp; }

TEST(parser, constructEmptyBasicParserAndParseEmptyArguments) {
    constexpr const char* const argv[]{nullptr};
    const clapp::parser::arg_t arg{parser_make_arg_t(argv)};
    empty_basic_parser_t ebp;
    ASSERT_NO_THROW(ebp.parse(arg.begin(), arg.end()));
}

TEST(parser, constructEmptyBasicParserAndParseAndValidateEmptyArguments) {
    constexpr const char* const argv[]{nullptr};
    const clapp::parser::arg_t arg{parser_make_arg_t(argv)};
    empty_basic_parser_t ebp;
    ASSERT_NO_THROW(ebp.parse(arg.begin(), arg.end()));
    ASSERT_NO_THROW(ebp.validate());
}

TEST(parser, constructEmptyBasicParserAndGenUsagePrefix) {
    empty_basic_parser_t ebp;
    ASSERT_THAT(ebp.gen_usage_prefix(), testing::StrEq("Usage:\n"));
}

TEST(parser, constructEmptyBasicParserAndGenHelpMessage) {
    empty_basic_parser_t ebp;
    ASSERT_THAT(ebp.gen_help_msg(255), testing::StrEq("arg-test-parser\n"));
}

TEST(parser, constructEmptyBasicParserIsActiveIsTrue) {
    empty_basic_parser_t ebp;
    ASSERT_THAT(ebp.is_active(), testing::Eq(true));
}

TEST(parser, constructEmptyBasicParserGetActiveReturnsThisRef) {
    empty_basic_parser_t ebp;
    ASSERT_THAT(&ebp.get_active_parser(), testing::Eq(&ebp));
}

TEST(parser, callDefaultPrintAndExitWithString) {
    const std::string text{"text-to-print"};
    testing::internal::CaptureStdout();
    empty_basic_parser_t::default_print_and_exit(text, std::nullopt);
    const std::string output{testing::internal::GetCapturedStdout()};
    ASSERT_THAT(output, testing::StrEq(text));
}

TEST(parser, callDefaultPrintAndExitWithStringAndExitCode) {
    const std::string text{"text"};
    const int exit_code{2};
    testing::internal::CaptureStdout();
    ASSERT_EXIT(empty_basic_parser_t::default_print_and_exit(text, exit_code),
                ::testing::ExitedWithCode(exit_code), "");
    const std::string output{testing::internal::GetCapturedStdout()};
    ASSERT_THAT(output, testing::StrEq(text));
}

TEST(parser, setAndCallPrintAndExitWithString) {
    print_and_exit_t pae;
    simple_test_parser_t stp;
    stp.set_print_and_exit_func([&pae](const std::string_view text,
                                       const std::optional<int> exit_code) {
        pae.print_and_exit(text, exit_code);
    });

    const std::string text{"text"};
    const std::optional<int> exit_code;
    EXPECT_CALL(pae, print_and_exit(std::string_view{text}, exit_code));
    stp.get_print_and_exit_func()(text, exit_code);
}

TEST(parser, setAndCallPrintAndExitWithStringAndExitCode) {
    print_and_exit_t pae;
    simple_test_parser_t stp;
    stp.set_print_and_exit_func([&pae](const std::string_view text,
                                       const std::optional<int> exit_code) {
        pae.print_and_exit(text, exit_code);
    });

    const std::string text{"text-string"};
    const std::optional<int> exit_code{10};
    EXPECT_CALL(pae, print_and_exit(std::string_view{text}, exit_code));
    stp.get_print_and_exit_func()(text, exit_code);
}

TEST(parser, genFuncPrintHelpAndExit) {
    empty_basic_parser_t ebp;
    clapp::value::found_func_t ff{
        ebp.gen_func_print_help_and_exit(EXIT_SUCCESS)};
    testing::internal::CaptureStdout();
    ASSERT_EXIT(ff.found(), ::testing::ExitedWithCode(0), "");
    std::string output{testing::internal::GetCapturedStdout()};
    ASSERT_THAT(output, testing::StrEq("Usage:\narg-test-parser\n"));
}

TEST(parser, constructSimpleTestParserAndGenHelpMessage) {
    simple_test_parser_t stp;
    constexpr std::size_t max_option_string_size{25};
    stp.set_max_option_string_size(max_option_string_size);
    ASSERT_THAT(
        stp.gen_help_msg(255),
        testing::StrEq(
            "simple-test-parser [-b|--bool] [-i|--int=<param>] <arg-name> "
            "[<variadic-arg-name>...]\n\n  Mandatory Arguments:\n    "
            "arg-name          Arg desc\n\n  Optional Arguments:\n    "
            "variadic-arg-name Variadic arg desc (variadic "
            "argument)\n\n  Options:\n    -b|--bool         "
            "Bool option.\n    -i|--int=<param>  Int option. "
            "(constraint: [10,200])\n"));
}

TEST(parser, constructSimpleTestParser2AndGenHelpMessage) {
    simple_test_parser2_t stp;
    constexpr std::size_t max_option_string_size{20};
    stp.set_max_option_string_size(max_option_string_size);
    ASSERT_THAT(stp.gen_help_msg(255),
                testing::StrEq(
                    "simple-test-parser2 -c|--count [<arg-name>]\n\n  Optional "
                    "Arguments:\n    arg-name   Arg desc\n\n  "
                    "Options:\n    -c|--count Count option.\n"));
}

TEST(parser, constructSimpleTestParser3AndGenHelpMessage) {
    simple_test_parser3_t stp;
    constexpr std::size_t max_option_string_size{30};
    stp.set_max_option_string_size(max_option_string_size);
    ASSERT_THAT(
        stp.gen_help_msg(255),
        testing::StrEq(
            "simple-test-parser3 -i|--int=<param>... [-s|--str=<param>...] "
            "<variadic-arg-name>...\n\n  Mandatory Arguments:\n    "
            "variadic-arg-name Variadic arg desc (variadic "
            "argument)\n\n  Options:\n    -i|--int=<param>  Int option. "
            "(vector option, constraint: [10,200])\n  "
            "  -s|--str=<param>  String "
            "option. (vector option)\n"));
}

TEST(parser, constructSubParserContainerAndGenHelpMessage) {
    sub_parser_container_t spc;
    constexpr std::size_t max_option_string_size{25};
    spc.set_max_option_string_size(max_option_string_size);
    ASSERT_THAT(
        spc.gen_help_msg(255),
        testing::StrEq(
            "sub_parser_container [-b|--bool] [-2|--second] <arg-name> "
            "sub-parser [-b|--bool] [-s|--string=<param>] [<sub-arg-name>]\n"
            "sub_parser_container [-b|--bool] [-2|--second] <arg-name>\n\n"
            "  Mandatory Arguments:\n    arg-name    "
            "Arg desc\n\n  Options:\n    -b|--bool   "
            "Bool option.\n    -2|--second Second bool "
            "option.\n\n"
            "  Subparser:\n    sub-parser  Sub "
            "parser desc\n      Optional Arguments:\n        sub-arg-name      "
            "  Sub arg desc\n\n      Options:\n        -b|--bool    "
            "       Bool option.\n        -s|--string=<param> String "
            "option.\n\n"));
}

TEST(parser, constructSubParserContainerAndGenSubParserHelpMessage) {
    sub_parser_container_t spc;
    constexpr std::size_t max_option_string_size{35};
    spc.set_max_option_string_size(max_option_string_size);
    ASSERT_THAT(
        spc.sub_parser.gen_help_msg(255),
        testing::StrEq(
            "sub_parser_container [-b|--bool] [-2|--second] <arg-name> "
            "sub-parser [-b|--bool] [-s|--string=<param>] [<sub-arg-name>]\n\n "
            " "
            "Optional Arguments:\n    sub-arg-name        Sub arg "
            "desc\n\n  Options:\n    -b|--bool           "
            "Bool option.\n    -s|--string=<param> String "
            "option.\n"));
}

TEST(parser, constructEmptyBasicParserAndParseUnknownArgumentsThrows) {
    constexpr const char* const argv[]{"unknown-argument", nullptr};
    const clapp::parser::arg_t arg{parser_make_arg_t(argv)};
    empty_basic_parser_t ebp;
    ASSERT_THROW(ebp.parse(arg.begin(), arg.end()),
                 clapp::exception::clapp_exception_t);
}

TEST(parser, constructEmptyBasicParserAndParseUnknownLongOptionThrows) {
    constexpr const char* const argv[]{"--long-option", nullptr};
    const clapp::parser::arg_t arg{parser_make_arg_t(argv)};
    empty_basic_parser_t ebp;
    ASSERT_THROW(ebp.parse(arg.begin(), arg.end()),
                 clapp::exception::option_exception_t);
}

TEST(parser, constructEmptyBasicParserAndParseUnknownShortOptionThrows) {
    constexpr const char* const argv[]{"-s", nullptr};
    const clapp::parser::arg_t arg{parser_make_arg_t(argv)};
    empty_basic_parser_t ebp;
    ASSERT_THROW(ebp.parse(arg.begin(), arg.end()),
                 clapp::exception::option_exception_t);
}

TEST(parser, constructSimpleTestParserAndParseUnknownLongOptionThrows) {
    constexpr const char* const argv[]{"--long-option", nullptr};
    const clapp::parser::arg_t arg{parser_make_arg_t(argv)};
    simple_test_parser_t ebp;
    ASSERT_THROW(ebp.parse(arg.begin(), arg.end()),
                 clapp::exception::option_exception_t);
}

TEST(parser, constructSimpleTestParserAndParseUnknownShortOptionThrows) {
    constexpr const char* const argv[]{"-s", nullptr};
    const clapp::parser::arg_t arg{parser_make_arg_t(argv)};
    simple_test_parser_t ebp;
    ASSERT_THROW(ebp.parse(arg.begin(), arg.end()),
                 clapp::exception::option_exception_t);
}

TEST(parser, constructSimpleTestParserAndParseLongBoolOptionWithParamThrows) {
    constexpr const char* const argv[]{"--bool=param", nullptr};
    const clapp::parser::arg_t arg{parser_make_arg_t(argv)};
    simple_test_parser_t ebp;
    ASSERT_THROW(ebp.parse(arg.begin(), arg.end()),
                 clapp::exception::option_param_exception_t);
}

TEST(parser, constructSimpleTestParserAndParseShortOptionWithParamThrows) {
    constexpr const char* const argv[]{"-b=param", nullptr};
    const clapp::parser::arg_t arg{parser_make_arg_t(argv)};
    simple_test_parser_t ebp;
    ASSERT_THROW(ebp.parse(arg.begin(), arg.end()),
                 clapp::exception::option_param_exception_t);
}

TEST(parser, constructSimpleTestParserAndParseLongIntOptionWithoutParamThrows) {
    constexpr const char* const argv[]{"--int", nullptr};
    const clapp::parser::arg_t arg{parser_make_arg_t(argv)};
    simple_test_parser_t ebp;
    ASSERT_THROW(ebp.parse(arg.begin(), arg.end()),
                 clapp::exception::option_param_exception_t);
}

TEST(parser,
     constructSimpleTestParserAndParseShortIntOptionWithoutParamThrows) {
    constexpr const char* const argv[]{"-i", nullptr};
    const clapp::parser::arg_t arg{parser_make_arg_t(argv)};
    simple_test_parser_t ebp;
    ASSERT_THROW(ebp.parse(arg.begin(), arg.end()),
                 clapp::exception::option_param_exception_t);
}

TEST(parser,
     constructSimpleTestParserAndParseShortIntOptionWithoutParamThrows2) {
    constexpr const char* const argv[]{"-ib", nullptr};
    const clapp::parser::arg_t arg{parser_make_arg_t(argv)};
    simple_test_parser_t ebp;
    ASSERT_THROW(ebp.parse(arg.begin(), arg.end()),
                 clapp::exception::option_param_exception_t);
}

TEST(parser,
     constructSimpleTestParserParseWithoutArgumentAndValidateRecursiveThrows) {
    constexpr const char* const argv[]{nullptr};
    const clapp::parser::arg_t arg{parser_make_arg_t(argv)};
    simple_test_parser_t ebp;
    ASSERT_NO_THROW(ebp.parse(arg.begin(), arg.end()));
    ASSERT_THAT(ebp.get_num_processed_arguments(), testing::Eq(0));
    ASSERT_THROW(ebp.validate_recursive(),
                 clapp::exception::argument_exception_t);
}

TEST(parser, constructSimpleTestParserParseArgumentAndValidateRecursive) {
    constexpr const char* const argv[]{"argument", nullptr};
    const clapp::parser::arg_t arg{parser_make_arg_t(argv)};
    simple_test_parser_t ebp;
    ASSERT_NO_THROW(ebp.parse(arg.begin(), arg.end()));
    ASSERT_THAT(static_cast<bool>(ebp.bool_option), testing::Eq(true));
    ASSERT_THAT(ebp.bool_option.has_value(), testing::Eq(true));
    ASSERT_THAT(ebp.bool_option.value(), testing::Eq(false));
    ASSERT_THAT(static_cast<bool>(ebp.int_option), testing::Eq(false));
    ASSERT_THAT(static_cast<bool>(ebp.string_arg), testing::Eq(true));
    ASSERT_THAT(ebp.string_arg.value(), testing::Eq("argument"));
    ASSERT_THAT(static_cast<bool>(ebp.variadic_string_arg), testing::Eq(false));
    ASSERT_THAT(ebp.get_num_processed_arguments(), testing::Eq(1));
    ASSERT_NO_THROW(ebp.validate_recursive());
}

TEST(
    parser,
    constructSimpleTestParserParseArgumentAndShortOptionWithoutParamsAndValidateRecursive) {
    constexpr const char* const argv[]{"-b", "arg", nullptr};
    const clapp::parser::arg_t arg{parser_make_arg_t(argv)};
    simple_test_parser_t ebp;
    ASSERT_NO_THROW(ebp.parse(arg.begin(), arg.end()));
    ASSERT_THAT(static_cast<bool>(ebp.bool_option), testing::Eq(true));
    ASSERT_THAT(ebp.bool_option.has_value(), testing::Eq(true));
    ASSERT_THAT(ebp.bool_option.value(), testing::Eq(true));
    ASSERT_THAT(static_cast<bool>(ebp.int_option), testing::Eq(false));
    ASSERT_THAT(static_cast<bool>(ebp.string_arg), testing::Eq(true));
    ASSERT_THAT(ebp.string_arg.value(), testing::StrEq("arg"));
    ASSERT_THAT(static_cast<bool>(ebp.variadic_string_arg), testing::Eq(false));
    ASSERT_THAT(ebp.get_num_processed_arguments(), testing::Eq(1));
    ASSERT_NO_THROW(ebp.validate_recursive());
}

TEST(parser,
     constructSimpleTestParserParseArgumentAndLongOptionWithParamAndValidate) {
    constexpr const char* const argv[]{"--int", "123", "arg", nullptr};
    const clapp::parser::arg_t arg{parser_make_arg_t(argv)};
    simple_test_parser_t ebp;
    ASSERT_NO_THROW(ebp.parse(arg.begin(), arg.end()));
    ASSERT_THAT(static_cast<bool>(ebp.bool_option), testing::Eq(true));
    ASSERT_THAT(ebp.bool_option.has_value(), testing::Eq(true));
    ASSERT_THAT(ebp.bool_option.value(), testing::Eq(false));
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
    constructSimpleTestParserParseArgumentVariadicArgumentAndLongOptionWithParamAndValidate) {
    constexpr const char* const argv[]{"--int=123", "arg", "varg0", "varg1",
                                       nullptr};
    const clapp::parser::arg_t arg{parser_make_arg_t(argv)};
    simple_test_parser_t ebp;
    ASSERT_NO_THROW(ebp.parse(arg.begin(), arg.end()));
    ASSERT_THAT(static_cast<bool>(ebp.bool_option), testing::Eq(true));
    ASSERT_THAT(ebp.bool_option.has_value(), testing::Eq(true));
    ASSERT_THAT(ebp.bool_option.value(), testing::Eq(false));
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

TEST(parser, constructSimpleTestParserParseArgumentAndShortOptionsAndValidate) {
    constexpr const char* const argv[]{"-bi=123", "arg", nullptr};
    const clapp::parser::arg_t arg{parser_make_arg_t(argv)};
    simple_test_parser_t ebp;
    ASSERT_NO_THROW(ebp.parse(arg.begin(), arg.end()));
    ASSERT_THAT(static_cast<bool>(ebp.bool_option), testing::Eq(true));
    ASSERT_THAT(ebp.bool_option.has_value(), testing::Eq(true));
    ASSERT_THAT(ebp.bool_option.value(), testing::Eq(true));
    ASSERT_THAT(static_cast<bool>(ebp.int_option), testing::Eq(true));
    ASSERT_THAT(ebp.int_option.value(), testing::Eq(123));
    ASSERT_THAT(static_cast<bool>(ebp.string_arg), testing::Eq(true));
    ASSERT_THAT(ebp.string_arg.value(), testing::StrEq("arg"));
    ASSERT_THAT(ebp.get_num_processed_arguments(), testing::Eq(1));
    ASSERT_NO_THROW(ebp.validate_recursive());
}

TEST(parser, constructSimpleTestParser2AndParseLongCountOptionWithParamThrows) {
    constexpr const char* const argv[]{"--count=param", nullptr};
    const clapp::parser::arg_t arg{parser_make_arg_t(argv)};
    simple_test_parser2_t ebp;
    ASSERT_THROW(ebp.parse(arg.begin(), arg.end()),
                 clapp::option_param_exception_t);
}

TEST(parser, constructSimpleTestParser2ParseWithoutMandatoryOptionThrows) {
    constexpr const char* const argv[]{nullptr};
    const clapp::parser::arg_t arg{parser_make_arg_t(argv)};
    simple_test_parser2_t ebp;
    ASSERT_NO_THROW(ebp.parse(arg.begin(), arg.end()));
    ASSERT_THAT(static_cast<bool>(ebp.count_option), testing::Eq(true));
    ASSERT_THAT(ebp.count_option.has_value(), testing::Eq(true));
    ASSERT_THAT(ebp.count_option.value(), testing::Eq(0));
    ASSERT_THROW(ebp.validate(), clapp::option_param_exception_t);
}

TEST(parser, constructSimpleTestParser2ParseOptionWithoutParamAndValidate) {
    constexpr const char* const argv[]{"-c", nullptr};
    const clapp::parser::arg_t arg{parser_make_arg_t(argv)};
    simple_test_parser2_t ebp;
    ASSERT_NO_THROW(ebp.parse(arg.begin(), arg.end()));
    ASSERT_THAT(static_cast<bool>(ebp.count_option), testing::Eq(true));
    ASSERT_THAT(ebp.count_option.has_value(), testing::Eq(true));
    ASSERT_THAT(ebp.count_option.value(), testing::Eq(1));
    ASSERT_NO_THROW(ebp.validate());
}

TEST(
    parser,
    constructSimpleTestParser2ParseOptionWithoutParamOptionalArgumentAndValidate) {
    constexpr const char* const argv[]{"-c", "opt-arg", nullptr};
    const clapp::parser::arg_t arg{parser_make_arg_t(argv)};
    simple_test_parser2_t ebp;
    ASSERT_NO_THROW(ebp.parse(arg.begin(), arg.end()));
    ASSERT_THAT(static_cast<bool>(ebp.count_option), testing::Eq(true));
    ASSERT_THAT(ebp.count_option.value(), testing::Eq(1));
    ASSERT_THAT(static_cast<bool>(ebp.string_arg), testing::Eq(true));
    ASSERT_THAT(ebp.string_arg.value(), testing::StrEq("opt-arg"));
    ASSERT_NO_THROW(ebp.validate());
}

TEST(parser, constructSubParserContainerParseSubparserAndValidate) {
    constexpr const char* const argv[]{"string-arg", "sub-parser", "-bs=param",
                                       nullptr};
    const clapp::parser::arg_t arg{parser_make_arg_t(argv)};
    sub_parser_container_t spc;
    ASSERT_NO_THROW(spc.parse(arg.begin(), arg.end()));
    ASSERT_THAT(static_cast<bool>(spc.string_arg), testing::Eq(true));
    ASSERT_THAT(spc.string_arg.value(), testing::StrEq("string-arg"));
    ASSERT_THAT(static_cast<bool>(spc.bool_option), testing::Eq(true));
    ASSERT_THAT(spc.bool_option.has_value(), testing::Eq(true));
    ASSERT_THAT(spc.bool_option.value(), testing::Eq(false));
    ASSERT_THAT(static_cast<bool>(spc.bool_option2), testing::Eq(true));
    ASSERT_THAT(spc.bool_option2.has_value(), testing::Eq(true));
    ASSERT_THAT(spc.bool_option2.value(), testing::Eq(false));
    ASSERT_THAT(static_cast<bool>(spc.sub_parser.bool_option),
                testing::Eq(true));
    ASSERT_THAT(spc.sub_parser.bool_option.value(), testing::Eq(true));
    ASSERT_THAT(spc.sub_parser.bool_option.has_value(), testing::Eq(true));
    ASSERT_THAT(spc.sub_parser.bool_option.value(), testing::Eq(true));
    ASSERT_THAT(static_cast<bool>(spc.sub_parser.string_option),
                testing::Eq(true));
    ASSERT_THAT(spc.sub_parser.string_option.value(), testing::StrEq("param"));
    ASSERT_NO_THROW(spc.validate());
}

TEST(parser, constructSubParserContainerParseSubparserAndValidate2) {
    constexpr const char* const argv[]{
        "string-arg", "-b", "-2", "sub-parser", "-s", "param", nullptr};
    const clapp::parser::arg_t arg{parser_make_arg_t(argv)};
    sub_parser_container_t spc;
    ASSERT_NO_THROW(spc.parse(arg.begin(), arg.end()));
    ASSERT_THAT(static_cast<bool>(spc.string_arg), testing::Eq(true));
    ASSERT_THAT(spc.string_arg.value(), testing::StrEq("string-arg"));
    ASSERT_THAT(static_cast<bool>(spc.bool_option), testing::Eq(true));
    ASSERT_THAT(spc.bool_option.has_value(), testing::Eq(true));
    ASSERT_THAT(spc.bool_option.value(), testing::Eq(true));
    ASSERT_THAT(static_cast<bool>(spc.bool_option2), testing::Eq(true));
    ASSERT_THAT(spc.bool_option2.has_value(), testing::Eq(true));
    ASSERT_THAT(spc.bool_option2.value(), testing::Eq(true));
    ASSERT_THAT(static_cast<bool>(spc.sub_parser.bool_option),
                testing::Eq(true));
    ASSERT_THAT(spc.sub_parser.bool_option.has_value(), testing::Eq(true));
    ASSERT_THAT(spc.sub_parser.bool_option.value(), testing::Eq(false));
    ASSERT_THAT(static_cast<bool>(spc.sub_parser.string_option),
                testing::Eq(true));
    ASSERT_THAT(spc.sub_parser.string_option.value(), testing::StrEq("param"));
    ASSERT_NO_THROW(spc.validate());
}
