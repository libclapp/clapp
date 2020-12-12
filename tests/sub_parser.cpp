#include <clapp/argument.h>
#include <clapp/exception.h>
#include <clapp/option.h>
#include <clapp/parser.h>
#include <clapp/sub_parser.h>
#include <gmock/gmock.h>

class print_and_exit_t {
   public:
    MOCK_METHOD2(print_and_exit, void(const std::string_view print_msg,
                                      std::optional<int> exit_code));
};

template <class T, size_t N>
inline clapp::parser::arg_t sub_parser_make_arg_t(T (&arg)[N]) {
    return clapp::parser::arg_t{static_cast<const char* const*>(arg), N - 1};
}

class empty_test_parser_t : public clapp::parser::basic_parser_t {
   public:
    using clapp::parser::basic_parser_t::sub_parser_descriptions_vec_t;
    using clapp::parser::basic_parser_t::sub_parsers_map_t;

    using clapp::parser::basic_parser_t::get_sub_parser_descriptions;
    using clapp::parser::basic_parser_t::get_sub_parsers;

    ~empty_test_parser_t() override;

    [[nodiscard]] std::string gen_short_line_prefix() const override;
};

empty_test_parser_t::~empty_test_parser_t() = default;

[[nodiscard]] std::string empty_test_parser_t::gen_short_line_prefix() const {
    return "empty-test-parser" + gen_short_line();
}

class empty_sub_parser_t : public clapp::parser::basic_sub_parser_t {
   public:
    using clapp::parser::basic_sub_parser_t::basic_sub_parser_t;

    ~empty_sub_parser_t() override;
};

empty_sub_parser_t::~empty_sub_parser_t() = default;

class simple_test_sub_parser_t : public clapp::parser::basic_parser_t {
   public:
    using clapp::parser::basic_parser_t::sub_parser_descriptions_vec_t;
    using clapp::parser::basic_parser_t::sub_parsers_map_t;

    using clapp::parser::basic_parser_t::get_sub_parser_descriptions;
    using clapp::parser::basic_parser_t::get_sub_parsers;

    clapp::option::count_option_t count_option{*this, "count", 'c',
                                               "Count option."};

    ~simple_test_sub_parser_t() override;

    [[nodiscard]] std::string gen_short_line_prefix() const override;
};

simple_test_sub_parser_t::~simple_test_sub_parser_t() = default;

[[nodiscard]] std::string simple_test_sub_parser_t::gen_short_line_prefix()
    const {
    return "simple-test-sub-parser" + gen_short_line();
}

class simple_sub_parser_t : public clapp::parser::basic_sub_parser_t {
   public:
    using clapp::parser::basic_sub_parser_t::basic_sub_parser_t;
    clapp::option::bool_option_t bool_option{
        *this, "bool", 'b', "Bool option.",
        basic_parser_t::purpose_t::mandatory};

    ~simple_sub_parser_t() override;
};

simple_sub_parser_t::~simple_sub_parser_t() = default;

class variadic_argument_test_sub_parser_t
    : public clapp::parser::basic_parser_t {
   public:
    clapp::argument::variadic_string_argument_t variadic_string_argument{
        *this, "arg", "Arg"};

    ~variadic_argument_test_sub_parser_t() override;

    [[nodiscard]] std::string gen_short_line_prefix() const override;
};

variadic_argument_test_sub_parser_t::~variadic_argument_test_sub_parser_t() =
    default;

[[nodiscard]] std::string
variadic_argument_test_sub_parser_t::gen_short_line_prefix() const {
    return "variadic-argument-test-sub-parser" + gen_short_line();
}

class optional_argument_test_sub_parser_t
    : public clapp::parser::basic_parser_t {
   public:
    clapp::argument::string_argument_t string_argument{
        *this, "arg", "Arg", clapp::basic_parser_t::purpose_t::optional};

    ~optional_argument_test_sub_parser_t() override;

    [[nodiscard]] std::string gen_short_line_prefix() const override;
};

optional_argument_test_sub_parser_t::~optional_argument_test_sub_parser_t() =
    default;

[[nodiscard]] std::string
optional_argument_test_sub_parser_t::gen_short_line_prefix() const {
    return "optional-argument-test-sub-parser" + gen_short_line();
}

TEST(subParser, constructEmptySubParserAndParseEmptyArguments) {
    const std::string sub_parser{"sub"};
    const std::string description{"sub parser"};

    constexpr const char* const argv[]{"sub", nullptr};
    const clapp::parser::arg_t arg{sub_parser_make_arg_t(argv)};

    empty_test_parser_t tp;
    empty_sub_parser_t sub{tp, sub_parser, description};

    empty_test_parser_t::sub_parsers_map_t long_options{tp.get_sub_parsers()};
    empty_test_parser_t::sub_parser_descriptions_vec_t descs{
        tp.get_sub_parser_descriptions()};

    ASSERT_THAT(descs.size(), testing::Eq(1));
    ASSERT_THAT(descs[0].sub_parser_string, testing::StrEq(sub_parser));
    ASSERT_THAT(descs[0].description, testing::StrEq(description));

    ASSERT_THAT(static_cast<bool>(sub), testing::Eq(false));
    ASSERT_THAT(sub.get_sub_parser_name(), testing::StrEq(sub_parser));

    tp.parse(arg.begin(), arg.end());
}

TEST(subParser, constructSimpleSubParserAndParseSubOption) {
    const std::string sub_parser{"subbb"};
    const std::string description{"subbb parser"};

    constexpr const char* const argv[]{"subbb", "--bool", nullptr};
    const clapp::parser::arg_t arg{sub_parser_make_arg_t(argv)};

    simple_test_sub_parser_t tp;
    simple_sub_parser_t sub{tp, sub_parser, description};

    simple_test_sub_parser_t::sub_parsers_map_t long_options{
        tp.get_sub_parsers()};
    simple_test_sub_parser_t::sub_parser_descriptions_vec_t descs{
        tp.get_sub_parser_descriptions()};

    ASSERT_THAT(descs.size(), testing::Eq(1));
    ASSERT_THAT(descs[0].sub_parser_string, testing::StrEq(sub_parser));
    ASSERT_THAT(descs[0].description, testing::StrEq(description));

    ASSERT_THAT(static_cast<bool>(sub), testing::Eq(false));
    ASSERT_THAT(sub.get_sub_parser_name(), testing::StrEq(sub_parser));

    ASSERT_THAT(static_cast<bool>(sub.bool_option), testing::Eq(true));
    ASSERT_THAT(sub.bool_option.has_value(), testing::Eq(true));
    ASSERT_THAT(sub.bool_option.value(), testing::Eq(false));
    tp.parse(arg.begin(), arg.end());
    ASSERT_THAT(static_cast<bool>(sub.bool_option), testing::Eq(true));
}

TEST(subParser, constructSimpleSubParserAndParseBaseOption) {
    const std::string sub_parser{"subbb"};
    const std::string description{"subbb parser"};

    constexpr const char* const argv[]{"--count", "subbb", "-b", nullptr};
    const clapp::parser::arg_t arg{sub_parser_make_arg_t(argv)};

    simple_test_sub_parser_t tp;
    simple_sub_parser_t sub{tp, sub_parser, description};

    simple_test_sub_parser_t::sub_parsers_map_t long_options{
        tp.get_sub_parsers()};
    simple_test_sub_parser_t::sub_parser_descriptions_vec_t descs{
        tp.get_sub_parser_descriptions()};

    ASSERT_THAT(descs.size(), testing::Eq(1));
    ASSERT_THAT(descs[0].sub_parser_string, testing::StrEq(sub_parser));
    ASSERT_THAT(descs[0].description, testing::StrEq(description));

    ASSERT_THAT(static_cast<bool>(sub), testing::Eq(false));
    ASSERT_THAT(sub.get_sub_parser_name(), testing::StrEq(sub_parser));

    ASSERT_THAT(tp.count_option.value(), testing::Eq(0));
    tp.parse(arg.begin(), arg.end());
    ASSERT_THAT(tp.count_option.value(), testing::Eq(1));
}

TEST(subParser, constructSubParserWithSameNameThrows) {
    const std::string sub_parser{"sub"};
    const std::string description{"sub parser"};

    simple_test_sub_parser_t tp;
    simple_sub_parser_t sub{tp, sub_parser, description};
    ASSERT_THROW((simple_sub_parser_t{tp, sub_parser, description}),
                 clapp::exception::sub_parser_exception_t);
}

TEST(subParser, constructSubParserWithVariadicArgumentsParserThrows) {
    const std::string sub_parser{"sub"};
    const std::string description{"sub parser"};

    variadic_argument_test_sub_parser_t tp;
    ASSERT_THROW((simple_sub_parser_t{tp, sub_parser, description}),
                 clapp::exception::sub_parser_exception_t);
}

TEST(subParser, constructSubParserWithOptionalArgumentParserThrows) {
    const std::string sub_parser{"sub"};
    const std::string description{"sub parser"};

    optional_argument_test_sub_parser_t tp;
    ASSERT_THROW((simple_sub_parser_t{tp, sub_parser, description}),
                 clapp::exception::sub_parser_exception_t);
}

TEST(subParser, constructSubParserAndGenHelpPrefix) {
    const std::string sub_parser{"sub"};
    const std::string description{"sub parser"};

    empty_test_parser_t tp;
    empty_sub_parser_t sub{tp, sub_parser, description};

    ASSERT_THAT(sub.gen_short_line_prefix(),
                testing::StrEq("empty-test-parser sub"));
}

TEST(subParser, constructSubParserIsActiveIsFalse) {
    const std::string sub_parser{"sub"};
    const std::string description{"sub parser"};

    empty_test_parser_t tp;
    empty_sub_parser_t sub{tp, sub_parser, description};
    ASSERT_THAT(sub.is_active(), testing::Eq(false));
}

TEST(subParser, constructSubParserAfterParsingSubBecomesActive) {
    const std::string sub_parser{"sub"};
    const std::string description{"sub parser"};
    constexpr const char* const argv[]{"sub", nullptr};
    const clapp::parser::arg_t arg{sub_parser_make_arg_t(argv)};

    empty_test_parser_t tp;
    empty_sub_parser_t sub{tp, sub_parser, description};

    ASSERT_THAT(&tp.get_active_parser(), testing::Eq(&tp));
    ASSERT_THAT(sub.is_active(), testing::Eq(false));

    tp.parse(arg.begin(), arg.end());
    ASSERT_THAT(sub.is_active(), testing::Eq(true));
    ASSERT_THAT(&tp.get_active_parser(), testing::Eq(&sub));
}

TEST(subParser, constructSubParserAndValidateRecursiveDoNotThrow) {
    const std::string sub_parser{"sub"};
    const std::string description{"sub parser"};
    constexpr const char* const argv[]{"sub", "-b", nullptr};
    const clapp::parser::arg_t arg{sub_parser_make_arg_t(argv)};

    empty_test_parser_t tp;
    simple_sub_parser_t sub{tp, sub_parser, description};

    tp.parse(arg.begin(), arg.end());
    ASSERT_NO_THROW(tp.validate_recursive());
}

TEST(subParser, constructSubParserAndValidateRecursiveDoThrow) {
    const std::string sub_parser{"sub"};
    const std::string description{"sub parser"};
    constexpr const char* const argv[]{"sub", nullptr};
    const clapp::parser::arg_t arg{sub_parser_make_arg_t(argv)};

    empty_test_parser_t tp;
    simple_sub_parser_t sub{tp, sub_parser, description};

    tp.parse(arg.begin(), arg.end());
    ASSERT_THROW(tp.validate_recursive(), clapp::clapp_exception_t);
}

TEST(subParser, constructSubParserSetAndCallPrintAndExitWithStringAndExitCode) {
    print_and_exit_t pae;
    empty_test_parser_t tp;
    tp.set_print_and_exit_func([&pae](const std::string_view text,
                                      const std::optional<int> exit_code) {
        pae.print_and_exit(text, exit_code);
    });

    const std::string sub_parser{"sub"};
    const std::string description{"sub parser"};
    simple_sub_parser_t sub{tp, sub_parser, description};

    const std::string text{"text-string"};
    const std::optional<int> exit_code{10};
    EXPECT_CALL(pae, print_and_exit(std::string_view{text}, exit_code));
    sub.get_print_and_exit_func()(text, exit_code);
}
