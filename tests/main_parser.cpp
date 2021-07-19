#include <clapp/main_parser.h>
#include <clapp/option.h>
#include <clapp/parser.h>
#include <gmock/gmock.h>

class empty_main_parser_t : public clapp::parser::basic_main_parser_t {
   public:
    ~empty_main_parser_t() override;
};

empty_main_parser_t::~empty_main_parser_t() = default;

TEST(mainParser, constructMainParser) {
    constexpr const char* const argv[]{"main", nullptr};
    empty_main_parser_t emp;

    emp.parse(1, static_cast<const char* const*>(argv));
}

TEST(mainParser, constructMainParserBoolOperator) {
    constexpr const char* const argv[]{"main-exec", nullptr};
    empty_main_parser_t emp;
    ASSERT_THAT(static_cast<bool>(emp), testing::Eq(false));

    emp.parse(1, static_cast<const char* const*>(argv));
    ASSERT_THAT(static_cast<bool>(emp), testing::Eq(true));
}

TEST(mainParser, constructMainParserGetExecutable) {
    constexpr const char* const argv[]{"main-exec", nullptr};
    empty_main_parser_t emp;
    ASSERT_THROW(emp.get_executable(),
                 clapp::exception::no_executable_exception_t);

    emp.parse(1, static_cast<const char* const*>(argv));
    ASSERT_THAT(emp.get_executable(), testing::StrEq("main-exec"));
}

TEST(mainParser, constructMainParserAndGenHelpPrefix) {
    constexpr const char* const argv[]{"main", nullptr};
    empty_main_parser_t emp;

    ASSERT_THROW(emp.gen_short_line_prefix(),
                 clapp::exception::no_executable_exception_t);

    emp.parse(1, static_cast<const char* const*>(argv));
    ASSERT_THAT(emp.gen_short_line_prefix(), testing::StrEq("main"));
}

TEST(mainParser, constructMainParserParseAndValidate) {
    constexpr const char* const argv[]{"main", nullptr};
    empty_main_parser_t emp;

    emp.parse_and_validate(1, static_cast<const char* const*>(argv));
}

TEST(mainParser, constructMainParserIsActiveIsTrue) {
    empty_main_parser_t emp;
    ASSERT_THAT(emp.is_active(), testing::Eq(true));
}

TEST(mainParser, constructMainParserGetActiveReturnsThisRef) {
    empty_main_parser_t emp;
    ASSERT_THAT(&emp.get_active_parser(), testing::Eq(&emp));
}
