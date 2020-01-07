#include <clapp/main_parser.h>
#include <clapp/option.h>
#include <clapp/parser.h>
#include <gmock/gmock.h>

template <class T, size_t N>
constexpr clapp::parser::arg_t make_arg_t(T (&arg)[N]) {
    return clapp::parser::arg_t{arg, N - 1};
}

class empty_basic_parser_t : public clapp::basic_parser_t {
   public:
    using clapp::basic_parser_t::basic_parser_t;
    ~empty_basic_parser_t();
};

empty_basic_parser_t::~empty_basic_parser_t() = default;

class param_option_parser_t : public clapp::basic_parser_t {
   public:
    using clapp::basic_parser_t::basic_parser_t;
    ~param_option_parser_t();

    clapp::option::bool_option_t bool_option{*this, "bool", 'b',
                                             "Bool option."};
};

param_option_parser_t::~param_option_parser_t() = default;

TEST(parser, construct_empty_basic_parser) { empty_basic_parser_t ebp; }

TEST(parser, construct_empty_basic_parser_and_parse_empty_arguments) {
    constexpr const char* const argv[]{nullptr};
    const clapp::parser::arg_t arg{make_arg_t(argv)};
    empty_basic_parser_t ebp;
    ebp.parse(arg.cbegin(), arg.cend());
}

TEST(parser, construct_empty_basic_parser_and_parse_unknown_arguments_throws) {
    constexpr const char* const argv[]{"unknown-argument", nullptr};
    const clapp::parser::arg_t arg{make_arg_t(argv)};
    empty_basic_parser_t ebp;
    ASSERT_THROW(ebp.parse(arg.cbegin(), arg.cend()), std::exception);
}

TEST(parser,
     construct_empty_basic_parser_and_parse_unknown_long_option_throws) {
    constexpr const char* const argv[]{"--long-option", nullptr};
    const clapp::parser::arg_t arg{make_arg_t(argv)};
    empty_basic_parser_t ebp;
    ASSERT_THROW(ebp.parse(arg.cbegin(), arg.cend()), std::exception);
}

TEST(parser,
     construct_empty_basic_parser_and_parse_unknown_short_option_throws) {
    constexpr const char* const argv[]{"-s", nullptr};
    const clapp::parser::arg_t arg{make_arg_t(argv)};
    empty_basic_parser_t ebp;
    ASSERT_THROW(ebp.parse(arg.cbegin(), arg.cend()), std::exception);
}

TEST(
    parser,
    construct_param_option_parser_and_parse_short_option_without_params_equal) {
    constexpr const char* const argv[]{"--bool=param", nullptr};
    const clapp::parser::arg_t arg{make_arg_t(argv)};
    param_option_parser_t ebp;
    ASSERT_THROW(ebp.parse(arg.cbegin(), arg.cend()), std::exception);
}

TEST(parser,
     construct_param_option_parser_and_parse_short_option_without_params) {
    constexpr const char* const argv[]{"--bool", "param", nullptr};
    const clapp::parser::arg_t arg{make_arg_t(argv)};
    param_option_parser_t ebp;
    ASSERT_THROW(ebp.parse(arg.cbegin(), arg.cend()), std::exception);
}

TEST(parser,
     construct_param_option_parser_and_parse_long_option_without_params_equal) {
    constexpr const char* const argv[]{"-b=param", nullptr};
    const clapp::parser::arg_t arg{make_arg_t(argv)};
    param_option_parser_t ebp;
    ASSERT_THROW(ebp.parse(arg.cbegin(), arg.cend()), std::exception);
}

TEST(parser,
     construct_param_option_parser_and_parse_long_option_without_params) {
    constexpr const char* const argv[]{"-b", "param", nullptr};
    const clapp::parser::arg_t arg{make_arg_t(argv)};
    param_option_parser_t ebp;
    ASSERT_THROW(ebp.parse(arg.cbegin(), arg.cend()), std::exception);
}
