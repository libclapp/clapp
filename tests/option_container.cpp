#include <clapp/option.h>
#include <clapp/option_container.h>
#include <clapp/sub_parser.h>
#include <gmock/gmock.h>

class fake_test_parser_t : public clapp::basic_parser_t {
   public:
    using clapp::basic_parser_t::basic_parser_t;
    ~fake_test_parser_t() override;
    [[nodiscard]] std::string gen_short_line_prefix() const override;
};

fake_test_parser_t::~fake_test_parser_t() = default;

std::string fake_test_parser_t::gen_short_line_prefix() const {
    return "fake-test-parser" + gen_short_line();
}

class empty_basic_option_container_t : public clapp::basic_option_container_t {
   public:
    using clapp::basic_option_container_t::basic_option_container_t;
    ~empty_basic_option_container_t() override;

    [[nodiscard]] clapp::basic_parser_t& get_parser() override;

    [[nodiscard]] const clapp::basic_parser_t& get_const_parser()
        const override;

    using clapp::basic_option_container_t::get_option_help;

    using clapp::basic_option_container_t::gen_short_option_line;
};

empty_basic_option_container_t::~empty_basic_option_container_t() = default;

clapp::parser::basic_parser_t&
empty_basic_option_container_t::empty_basic_option_container_t::get_parser() {
    throw std::runtime_error("throws get parser");
}

const clapp::parser::basic_parser_t& empty_basic_option_container_t::
    empty_basic_option_container_t::get_const_parser() const {
    throw std::runtime_error("throws get parser");
}

class simple_test_option_container_t : public clapp::option_container_t {
   public:
    using clapp::option_container_t::option_container_t;
    ~simple_test_option_container_t() override;

    static constexpr std::int32_t min_int{10};
    static constexpr std::int32_t max_int{200};

    clapp::option::bool_option_t bool_option{*this, "bool", 'b',
                                             "Bool option."};

    clapp::option::int32_param_option_t int_option{
        *this, "int", 'i', "Int option.",
        clapp::value::min_max_value_t<std::int32_t>{min_int, max_int}};

    using clapp::basic_option_container_t::get_option_help;

    using clapp::basic_option_container_t::gen_short_option_line;
};

simple_test_option_container_t::~simple_test_option_container_t() = default;

class simple_test_option_container2_t : public clapp::option_container_t {
   public:
    using clapp::option_container_t::option_container_t;
    ~simple_test_option_container2_t() override;

    clapp::option::string_param_option_t string_option{*this, "str", 's',
                                                       "String option."};

    clapp::option::int64_param_option_t int_option{*this, "int", 'i',
                                                   "Int option."};

    clapp::option::uint64_param_option_t uint_option{*this, "uint", 'u',
                                                     "Unsigned int option."};

    using clapp::basic_option_container_t::get_option_help;

    using clapp::basic_option_container_t::gen_short_option_line;
};

simple_test_option_container2_t::~simple_test_option_container2_t() = default;

TEST(optionContainer, constructEmptyBasicOptionContainerDoesNotThrow) {
    ASSERT_NO_THROW(empty_basic_option_container_t{
        clapp::parser::types::logic_operator_type_t::logic_and});
}

TEST(optionContainer,
     constructEmptyBasicOptionContainerAndCallGenShortOptionLine) {
    empty_basic_option_container_t eboc{
        clapp::parser::types::logic_operator_type_t::logic_and};
    ASSERT_THAT(eboc.gen_short_option_line().empty(), testing::Eq(true));
}

TEST(optionContainer, constructEmptyBasicOptionContainerAndCallGetOptionHelp) {
    empty_basic_option_container_t eboc{
        clapp::parser::types::logic_operator_type_t::logic_and};
    ASSERT_THAT(eboc.get_option_help().empty(), testing::Eq(true));
}

TEST(optionContainer, constructSimpleTestOptionContainerWithXorDoesNotThrow) {
    fake_test_parser_t ftp;
    ASSERT_NO_THROW((simple_test_option_container_t{
        ftp, clapp::parser::types::logic_operator_type_t::logic_xor}));
}

TEST(optionContainer,
     constructSimpleTestOptionContainerWithXorAndCallGenShortOptionLine) {
    fake_test_parser_t ftp;
    simple_test_option_container_t stoc{
        ftp, clapp::parser::types::logic_operator_type_t::logic_xor};
    ASSERT_THAT(stoc.gen_short_option_line(),
                testing::StrEq("[-b|--bool] | [-i|--int=<param>]"));
}

TEST(optionContainer,
     constructSimpleTestOptionContainerWithXorAndCallGetOptionHelp) {
    fake_test_parser_t ftp;
    simple_test_option_container_t stoc{
        ftp, clapp::parser::types::logic_operator_type_t::logic_xor};
    ASSERT_THAT(stoc.get_option_help().size(), testing::Eq(2U));
}

TEST(optionContainer, constructSimpleTestOptionContainerWithAndDoesNotThrow) {
    fake_test_parser_t ftp;
    ASSERT_NO_THROW((simple_test_option_container_t{
        ftp, clapp::parser::types::logic_operator_type_t::logic_and}));
}

TEST(optionContainer,
     constructSimpleTestOptionContainerWithAndAndCallGenShortOptionLine) {
    fake_test_parser_t ftp;
    simple_test_option_container_t stoc{
        ftp, clapp::parser::types::logic_operator_type_t::logic_and};
    ASSERT_THAT(stoc.gen_short_option_line(),
                testing::StrEq("[-b|--bool] [-i|--int=<param>]"));
}

TEST(optionContainer,
     constructSimpleTestOptionContainerWithAndAndCallGetOptionHelp) {
    fake_test_parser_t ftp;
    simple_test_option_container_t stoc{
        ftp, clapp::parser::types::logic_operator_type_t::logic_and};
    ASSERT_THAT(stoc.get_option_help().size(), testing::Eq(2U));
}

TEST(optionContainer, constructSimpleTestOptionContainer2WithXorDoesNotThrow) {
    fake_test_parser_t ftp;
    ASSERT_NO_THROW((simple_test_option_container2_t{
        ftp, clapp::parser::types::logic_operator_type_t::logic_xor}));
}

TEST(optionContainer,
     constructSimpleTestOptionContainer2WithXorAndCallGenShortOptionLine) {
    fake_test_parser_t ftp;
    simple_test_option_container2_t stoc{
        ftp, clapp::parser::types::logic_operator_type_t::logic_xor};
    ASSERT_THAT(
        stoc.gen_short_option_line(),
        testing::StrEq(
            "[-s|--str=<param>] | [-i|--int=<param>] | [-u|--uint=<param>]"));
}

TEST(optionContainer,
     constructSimpleTestOptionContainer2WithXorAndCallGetOptionHelp) {
    fake_test_parser_t ftp;
    simple_test_option_container2_t stoc{
        ftp, clapp::parser::types::logic_operator_type_t::logic_xor};
    ASSERT_THAT(stoc.get_option_help().size(), testing::Eq(3U));
}

TEST(optionContainer, constructSimpleTestOptionContainer2WithAndDoesNotThrow) {
    fake_test_parser_t ftp;
    ASSERT_NO_THROW((simple_test_option_container2_t{
        ftp, clapp::parser::types::logic_operator_type_t::logic_and}));
}

TEST(optionContainer,
     constructSimpleTestOptionContainer2WithAndAndCallGenShortOptionLine) {
    fake_test_parser_t ftp;
    simple_test_option_container2_t stoc{
        ftp, clapp::parser::types::logic_operator_type_t::logic_and};
    ASSERT_THAT(
        stoc.gen_short_option_line(),
        testing::StrEq(
            "[-s|--str=<param>] [-i|--int=<param>] [-u|--uint=<param>]"));
}

TEST(optionContainer,
     constructSimpleTestOptionContainer2WithAndAndCallGetOptionHelp) {
    fake_test_parser_t ftp;
    simple_test_option_container2_t stoc{
        ftp, clapp::parser::types::logic_operator_type_t::logic_and};
    ASSERT_THAT(stoc.get_option_help().size(), testing::Eq(3U));
}
