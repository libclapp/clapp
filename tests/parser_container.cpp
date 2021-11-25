#include <clapp/main_parser.h>
#include <clapp/option.h>
#include <clapp/parser.h>
#include <clapp/parser_container.h>
#include <gmock/gmock.h>

class example_main_parser_t : public clapp::parser::basic_main_parser_t {
   public:
    ~example_main_parser_t() override;
};

class example_main_parser2_t : public clapp::parser::basic_main_parser_t {
   public:
    clapp::option::bool_option_t count_option{
        *this, "count", 'c', "Count option.",
        clapp::basic_parser_t::purpose_t::mandatory};

    clapp::bool_option_t bo{
        *this, std::vector<std::string>{"bo"}, "Exits if given.",
        clapp::value::found_func_t{[](const std::string_view /*name*/) {
            return clapp::value::exit_t::exit(EXIT_SUCCESS);
        }}};

    ~example_main_parser2_t() override;
};

example_main_parser_t::~example_main_parser_t() = default;

example_main_parser2_t::~example_main_parser2_t() = default;

using example_parser_container_t =
    clapp::parser::basic_parser_container_t<example_main_parser_t>;

using example_parser_container2_t =
    clapp::parser::basic_parser_container_t<example_main_parser2_t>;

TEST(parserContainer, stateToStringView) {
    ASSERT_THAT(example_parser_container_t::to_string_view(
                    example_parser_container_t::state_t::not_parsed)
                    .value(),
                testing::StrEq("not_parsed"));
    ASSERT_THAT(example_parser_container_t::to_string_view(
                    example_parser_container_t::state_t::parsed)
                    .value(),
                testing::StrEq("parsed"));
    ASSERT_THAT(example_parser_container_t::to_string_view(
                    example_parser_container_t::state_t::parsing)
                    .value(),
                testing::StrEq("parsing"));
    ASSERT_THAT(
        example_parser_container_t::to_string_view(
            static_cast<example_parser_container_t::state_t>(
                std::underlying_type_t<example_parser_container_t::state_t>{
                    128}))
            .has_value(),
        testing::Eq(false));
}

TEST(parserContainer, constructEmptyParserContainer) {
    ASSERT_NO_THROW(example_parser_container_t{});
}

TEST(parserContainer, constructEmptyParserContainerAndGetState) {
    example_parser_container_t epc{};
    ASSERT_THAT(epc.get_state(),
                testing::Eq(example_parser_container_t::state_t::not_parsed));
}

TEST(parserContainer, constructEmptyParserContainerAndChangeStateThrows) {
    example_parser_container_t epc{};
    ASSERT_THROW(
        epc.change_state(example_parser_container_t::state_t::parsed,
                         example_parser_container_t::state_t::not_parsed),
        clapp::invalid_parser_state_exception_t);
}

TEST(parserContainer,
     constructEmptyParserContainerWithoutParseAndGetExecutableViaArrowThrows) {
    example_parser_container_t epc;

    ASSERT_THROW(epc->get_executable(), clapp::not_fully_parsed_exception_t);
}

TEST(parserContainer,
     constructEmptyParserContainerWithoutParseAndGetExecutableViaStarThrows) {
    example_parser_container_t epc;

    ASSERT_THROW((*epc).get_executable(), clapp::not_fully_parsed_exception_t);
}

TEST(parserContainer, constructEmptyParserContainerAndParse) {
    constexpr const char* const argv[]{"main2", nullptr};
    example_parser_container_t epc;

    ASSERT_THAT(epc.parse(1, static_cast<const char* const*>(argv)).has_value(),
                testing::Eq(false));
}

TEST(parserContainer, constructEmptyParserContainerAndParseAndValidate) {
    constexpr const char* const argv[]{"main2", nullptr};
    example_parser_container_t epc;

    ASSERT_THAT(epc.parse_and_validate(1, static_cast<const char* const*>(argv))
                    .has_value(),
                testing::Eq(false));
}

TEST(parserContainer, constructEmptyParserContainerParseAndGetExecutable) {
    constexpr const char* const argv[]{"main2", nullptr};
    example_parser_container_t epc;

    ASSERT_THAT(epc.parse(1, static_cast<const char* const*>(argv)).has_value(),
                testing::Eq(false));
    ASSERT_THAT(epc->get_executable(), testing::StrEq("main2"));
}

TEST(parserContainer,
     constructEmptyParserContainerParseAndValidateAndGetExecutableViaArrow) {
    constexpr const char* const argv[]{"main2", nullptr};
    example_parser_container_t epc;

    ASSERT_THAT(epc.parse_and_validate(1, static_cast<const char* const*>(argv))
                    .has_value(),
                testing::Eq(false));
    ASSERT_THAT(epc->get_executable(), testing::StrEq("main2"));
}

TEST(parserContainer,
     constructEmptyParserContainerParseAndValidateAndGetExecutableViaStar) {
    constexpr const char* const argv[]{"main2", nullptr};
    example_parser_container_t epc;

    ASSERT_THAT(epc.parse_and_validate(1, static_cast<const char* const*>(argv))
                    .has_value(),
                testing::Eq(false));
    ASSERT_THAT((*epc).get_executable(), testing::StrEq("main2"));
}

TEST(parserContainer, constructEmptyParserContainerParseAndValidateThrows) {
    constexpr const char* const argv[]{"main2", nullptr};
    example_parser_container2_t epc;

    ASSERT_THROW(static_cast<void>(epc.parse_and_validate(
                     1, static_cast<const char* const*>(argv))),
                 clapp::option_param_exception_t);
}

TEST(
    parserContainer,
    constructEmptyParserContainerParseAndValidateReturnsExitAndGetExecutableThrows) {
    constexpr const char* const argv[]{"main2", "--bo", nullptr};
    example_parser_container2_t epc;

    const std::optional<clapp::value::exit_t> exit{
        epc.parse_and_validate(2, static_cast<const char* const*>(argv))};
    ASSERT_THAT(exit.has_value(), testing::Eq(true));
    ASSERT_THAT(exit.value().get_exit_code(), testing::Eq(EXIT_SUCCESS));

    ASSERT_THROW((*epc).get_executable(), clapp::not_fully_parsed_exception_t);
    ASSERT_THROW(epc->get_executable(), clapp::not_fully_parsed_exception_t);
}

TEST(parserContainer,
     constructEmptyParserContainerParseReturnsExitAndGetExecutableThrows) {
    constexpr const char* const argv[]{"main2", "--bo", nullptr};
    example_parser_container2_t epc;

    const std::optional<clapp::value::exit_t> exit{
        epc.parse(2, static_cast<const char* const*>(argv))};
    ASSERT_THAT(exit.has_value(), testing::Eq(true));
    ASSERT_THAT(exit.value().get_exit_code(), testing::Eq(EXIT_SUCCESS));

    ASSERT_THROW((*epc).get_executable(), clapp::not_fully_parsed_exception_t);
    ASSERT_THROW(epc->get_executable(), clapp::not_fully_parsed_exception_t);
}
