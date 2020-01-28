#include <clapp/argument.h>
#include <clapp/exception.h>
#include <clapp/option.h>
#include <clapp/parser.h>
#include <clapp/sub_parser.h>
#include <gmock/gmock.h>

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
};

empty_test_parser_t::~empty_test_parser_t() = default;

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
};

simple_test_sub_parser_t::~simple_test_sub_parser_t() = default;

class simple_sub_parser_t : public clapp::parser::basic_sub_parser_t {
   public:
    using clapp::parser::basic_sub_parser_t::basic_sub_parser_t;
    clapp::option::bool_option_t bool_option{*this, "bool", 'b',
                                             "Bool option."};

    ~simple_sub_parser_t() override;
};

simple_sub_parser_t::~simple_sub_parser_t() = default;

class variadic_argument_test_sub_parser_t
    : public clapp::parser::basic_parser_t {
   public:
    clapp::argument::variadic_string_argument_t variadic_string_argument{
        *this, "arg", "Arg"};

    ~variadic_argument_test_sub_parser_t() override;
};

variadic_argument_test_sub_parser_t::~variadic_argument_test_sub_parser_t() =
    default;

class optional_argument_test_sub_parser_t
    : public clapp::parser::basic_parser_t {
   public:
    clapp::argument::string_argument_t string_argument{
        *this, "arg", "Arg", clapp::basic_parser_t::purpose_t::optional};

    ~optional_argument_test_sub_parser_t() override;
};

optional_argument_test_sub_parser_t::~optional_argument_test_sub_parser_t() =
    default;

TEST(sub_parser, construct_empty_sub_parser_and_parse_empty_arguments) {
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

    tp.parse(arg.cbegin(), arg.cend());
}

TEST(sub_parser, construct_simple_sub_parser_and_parse_sub_option) {
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

    ASSERT_THAT(static_cast<bool>(sub.bool_option), testing::Eq(false));
    tp.parse(arg.cbegin(), arg.cend());
    ASSERT_THAT(static_cast<bool>(sub.bool_option), testing::Eq(true));
}

TEST(sub_parser, construct_simple_sub_parser_and_parse_base_option) {
    const std::string sub_parser{"subbb"};
    const std::string description{"subbb parser"};

    constexpr const char* const argv[]{"subbb", "--count", nullptr};
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
    tp.parse(arg.cbegin(), arg.cend());
    ASSERT_THAT(tp.count_option.value(), testing::Eq(1));
}

TEST(sub_parser, construct_simple_sub_parser_and_ignore_main_option) {
    const std::string sub_parser{"subbb"};
    const std::string description{"subbb parser"};

    constexpr const char* const argv[]{"subbb", "--count", nullptr};
    const clapp::parser::arg_t arg{sub_parser_make_arg_t(argv)};

    simple_test_sub_parser_t tp;
    simple_sub_parser_t sub{tp, sub_parser, description, false};

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
    ASSERT_THROW(tp.parse(arg.cbegin(), arg.cend()),
                 clapp::exception::option_exception_t);
    ASSERT_THAT(tp.count_option.value(), testing::Eq(0));
    ASSERT_NO_THROW(tp.validate_recursive());
}

TEST(sub_parser, construct_sub_parser_with_same_name_throws) {
    const std::string sub_parser{"sub"};
    const std::string description{"sub parser"};

    simple_test_sub_parser_t tp;
    simple_sub_parser_t sub{tp, sub_parser, description, false};
    ASSERT_THROW((simple_sub_parser_t{tp, sub_parser, description, false}),
                 clapp::exception::sub_parser_exception_t);
}

TEST(sub_parser, construct_sub_parser_with_variadic_arguments_parser_throws) {
    const std::string sub_parser{"sub"};
    const std::string description{"sub parser"};

    variadic_argument_test_sub_parser_t tp;
    ASSERT_THROW((simple_sub_parser_t{tp, sub_parser, description, false}),
                 clapp::exception::sub_parser_exception_t);
}

TEST(sub_parser, construct_sub_parser_with_optional_argument_parser_throws) {
    const std::string sub_parser{"sub"};
    const std::string description{"sub parser"};

    optional_argument_test_sub_parser_t tp;
    ASSERT_THROW((simple_sub_parser_t{tp, sub_parser, description, false}),
                 clapp::exception::sub_parser_exception_t);
}

TEST(parser, construct_sub_parser_and_gen_help_prefix) {
    const std::string sub_parser{"sub"};
    const std::string description{"sub parser"};

    empty_test_parser_t tp;
    empty_sub_parser_t sub{tp, sub_parser, description};

    ASSERT_THAT(sub.gen_help_prefix(), testing::StrEq("Usage: \n sub"));
}
