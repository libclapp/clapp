#include <clapp/main_parser.h>
#include <clapp/option.h>
#include <clapp/parser.h>
#include <gmock/gmock.h>

class empty_main_parser_t : public clapp::parser::basic_main_parser_t {
   public:
    ~empty_main_parser_t() override;
};

empty_main_parser_t::~empty_main_parser_t() = default;

class bo_main_parser_t : public clapp::parser::basic_main_parser_t {
   public:
    clapp::bool_option_t bo{
        *this, std::vector<char>{'b'}, "Exits if given.",
        clapp::value::found_func_t{[](const std::string_view /*name*/) {
            return clapp::value::exit_t::exit(EXIT_SUCCESS);
        }}};

    ~bo_main_parser_t() override;
};

bo_main_parser_t::~bo_main_parser_t() = default;

TEST(mainParser, constructEmptyMainParserAndParse) {
    constexpr const char* const argv[]{"main", nullptr};
    empty_main_parser_t emp;

    ASSERT_THAT(emp.parse(1, static_cast<const char* const*>(argv)).has_value(),
                testing::Eq(false));
}

TEST(mainParser, constructEmptyMainParserBoolOperator) {
    constexpr const char* const argv[]{"main-exec", nullptr};
    empty_main_parser_t emp;
    ASSERT_THAT(static_cast<bool>(emp), testing::Eq(false));

    ASSERT_THAT(emp.parse(1, static_cast<const char* const*>(argv)).has_value(),
                testing::Eq(false));
    ASSERT_THAT(static_cast<bool>(emp), testing::Eq(true));
}

TEST(mainParser, constructEmptyMainParserGetExecutable) {
    constexpr const char* const argv[]{"main-exec", nullptr};
    empty_main_parser_t emp;
    ASSERT_THROW(static_cast<void>(emp.get_executable()),
                 clapp::exception::no_executable_exception_t);

    ASSERT_THAT(emp.parse(1, static_cast<const char* const*>(argv)).has_value(),
                testing::Eq(false));
    ASSERT_THAT(emp.get_executable(), testing::StrEq("main-exec"));
}

TEST(mainParser, constructEmptyMainParserAndGenHelpPrefix) {
    constexpr const char* const argv[]{"main", nullptr};
    empty_main_parser_t emp;

    ASSERT_THROW(static_cast<void>(emp.gen_short_line_prefix()),
                 clapp::exception::no_executable_exception_t);

    ASSERT_THAT(emp.parse(1, static_cast<const char* const*>(argv)).has_value(),
                testing::Eq(false));
    ASSERT_THAT(emp.gen_short_line_prefix(), testing::StrEq("main"));
}

TEST(mainParser, constructEmptyMainParserAndParseAndValidate) {
    constexpr const char* const argv[]{"main", nullptr};
    empty_main_parser_t emp;

    ASSERT_THAT(emp.parse_and_validate(1, static_cast<const char* const*>(argv))
                    .has_value(),
                testing::Eq(false));
}

TEST(mainParser, constructBoMainParserAndParse) {
    constexpr const char* const argv[]{"main", "-b", nullptr};
    bo_main_parser_t bmp;

    const std::optional<clapp::value::exit_t> exit{
        bmp.parse(2, static_cast<const char* const*>(argv))};
    ASSERT_THAT(exit.has_value(), testing::Eq(true));
    ASSERT_THAT(exit.value().get_exit_code(), testing::Eq(EXIT_SUCCESS));
}

TEST(mainParser, constructBoMainParserAndParseAndValidate) {
    constexpr const char* const argv[]{"main", "-b", nullptr};
    bo_main_parser_t bmp;

    const std::optional<clapp::value::exit_t> exit{
        bmp.parse_and_validate(2, static_cast<const char* const*>(argv))};
    ASSERT_THAT(exit.has_value(), testing::Eq(true));
    ASSERT_THAT(exit.value().get_exit_code(), testing::Eq(EXIT_SUCCESS));
}

TEST(mainParser, constructMainParserIsActiveIsTrue) {
    empty_main_parser_t emp;
    ASSERT_THAT(emp.is_active(), testing::Eq(true));
}

TEST(mainParser, constructMainParserGetActiveReturnsThisRef) {
    empty_main_parser_t emp;
    ASSERT_THAT(&emp.get_active_parser(), testing::Eq(&emp));
}

TEST(mainParser, constructEmptyMainParserGenHelpMsg) {
    constexpr const char* const argv[]{"main", nullptr};
    empty_main_parser_t emp;

    const std::optional<clapp::value::exit_t> exit{
        emp.parse(1, static_cast<const char* const*>(argv))};

    ASSERT_THAT(emp.gen_help_msg(255), testing::StrEq("main\n"));
    ASSERT_THAT(exit.has_value(), testing::Eq(false));
}

TEST(mainParser, constructBoMainParserGenHelpMsg) {
    constexpr const char* const argv[]{"main", "-b", nullptr};
    bo_main_parser_t bmp;

    const std::optional<clapp::value::exit_t> exit{
        bmp.parse(2, static_cast<const char* const*>(argv))};

    ASSERT_THAT(
        bmp.gen_help_msg(255),
        testing::StrEq(
            "main [-b]\n\n  Options:\n    -b Exits if given. (optional)\n"));

    ASSERT_THAT(exit.has_value(), testing::Eq(true));
    ASSERT_THAT(exit.value().get_exit_code(), testing::Eq(EXIT_SUCCESS));
}
