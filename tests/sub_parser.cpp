#include <clapp/sub_parser.h>
#include <clapp/option.h>
#include <clapp/parser.h>
#include <gmock/gmock.h>

template <class T, size_t N>
constexpr clapp::parser::arg_t make_arg_t(T (&arg)[N]) {
    return clapp::parser::arg_t{arg, N - 1};
}

class empty_test_parser_t : public clapp::parser::basic_parser_t {
   public:
    using clapp::parser::basic_parser_t::sub_parser_descriptions_vec_t;
    using clapp::parser::basic_parser_t::sub_parsers_map_t;

    using clapp::parser::basic_parser_t::get_sub_parsers;
    using clapp::parser::basic_parser_t::get_sub_parser_descriptions;
};

class empty_sub_parser_t : public clapp::parser::basic_sub_parser_t {
    public:
    using clapp::parser::basic_sub_parser_t::basic_sub_parser_t;
};

class simple_test_parser_t : public clapp::parser::basic_parser_t {
   public:
    using clapp::parser::basic_parser_t::sub_parser_descriptions_vec_t;
    using clapp::parser::basic_parser_t::sub_parsers_map_t;

    using clapp::parser::basic_parser_t::get_sub_parsers;
    using clapp::parser::basic_parser_t::get_sub_parser_descriptions;

    clapp::option::count_option_t count_option{*this, "count", 'c',
                                             "Count option."};
};

class simple_sub_parser_t : public clapp::parser::basic_sub_parser_t {
    public:
    using clapp::parser::basic_sub_parser_t::basic_sub_parser_t;
    clapp::option::bool_option_t bool_option{*this, "bool", 'b',
                                             "Bool option."};
};

TEST(sub_parser, construct_empty_sub_parser_and_parse_empty_arguments) {
    const std::string sub_parser{"sub"};
    const std::string description{"sub parser"};

    constexpr const char* const argv[]{"sub", nullptr};
    const clapp::parser::arg_t arg{make_arg_t(argv)};

    empty_test_parser_t tp;
    empty_sub_parser_t sub{tp, sub_parser, description};

    empty_test_parser_t::sub_parsers_map_t long_options{tp.get_sub_parsers()};
    empty_test_parser_t::sub_parser_descriptions_vec_t descs{
        tp.get_sub_parser_descriptions()};

    ASSERT_THAT(descs.size(), testing::Eq(1));
    ASSERT_THAT(descs[0].sub_parser_string,
                testing::StrEq(sub_parser));
    ASSERT_THAT(descs[0].description,
                testing::StrEq(description));

    ASSERT_THAT(static_cast<bool>(sub), testing::Eq(false));
    ASSERT_THAT(sub.get_sub_parser_name(), testing::StrEq(sub_parser));

    tp.parse(arg.cbegin(), arg.cend());
}

TEST(sub_parser, construct_simple_sub_parser_and_parse_sub_option) {
    const std::string sub_parser{"subbb"};
    const std::string description{"subbb parser"};

    constexpr const char* const argv[]{"subbb", "--bool", nullptr};
    const clapp::parser::arg_t arg{make_arg_t(argv)};

    simple_test_parser_t tp;
    simple_sub_parser_t sub{tp, sub_parser, description};

    simple_test_parser_t::sub_parsers_map_t long_options{tp.get_sub_parsers()};
    simple_test_parser_t::sub_parser_descriptions_vec_t descs{
        tp.get_sub_parser_descriptions()};

    ASSERT_THAT(descs.size(), testing::Eq(1));
    ASSERT_THAT(descs[0].sub_parser_string,
                testing::StrEq(sub_parser));
    ASSERT_THAT(descs[0].description,
                testing::StrEq(description));

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
    const clapp::parser::arg_t arg{make_arg_t(argv)};

    simple_test_parser_t tp;
    simple_sub_parser_t sub{tp, sub_parser, description};

    simple_test_parser_t::sub_parsers_map_t long_options{tp.get_sub_parsers()};
    simple_test_parser_t::sub_parser_descriptions_vec_t descs{
        tp.get_sub_parser_descriptions()};

    ASSERT_THAT(descs.size(), testing::Eq(1));
    ASSERT_THAT(descs[0].sub_parser_string,
                testing::StrEq(sub_parser));
    ASSERT_THAT(descs[0].description,
                testing::StrEq(description));

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
    const clapp::parser::arg_t arg{make_arg_t(argv)};

    simple_test_parser_t tp;
    simple_sub_parser_t sub{tp, sub_parser, description, false};

    simple_test_parser_t::sub_parsers_map_t long_options{tp.get_sub_parsers()};
    simple_test_parser_t::sub_parser_descriptions_vec_t descs{
        tp.get_sub_parser_descriptions()};

    ASSERT_THAT(descs.size(), testing::Eq(1));
    ASSERT_THAT(descs[0].sub_parser_string,
                testing::StrEq(sub_parser));
    ASSERT_THAT(descs[0].description,
                testing::StrEq(description));

    ASSERT_THAT(static_cast<bool>(sub), testing::Eq(false));
    ASSERT_THAT(sub.get_sub_parser_name(), testing::StrEq(sub_parser));

    ASSERT_THAT(tp.count_option.value(), testing::Eq(0));
    ASSERT_THROW(tp.parse(arg.cbegin(), arg.cend()), std::exception);
    ASSERT_THAT(tp.count_option.value(), testing::Eq(0));
}
