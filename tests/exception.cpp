#include <clapp/exception.h>
#include <gmock/gmock.h>

TEST(exception, clappExceptionT) {
    static_assert(
        std::is_base_of<std::runtime_error, clapp::clapp_exception_t>::value);

    const std::string msg{"message"};
    const std::string msg2{"message2"};

    clapp::clapp_exception_t ce1{msg.c_str()};
    clapp::clapp_exception_t ce2{msg2};

    clapp::clapp_exception_t ce3{ce1};
    clapp::clapp_exception_t ce4{std::move(ce1)};

    ce4 = ce3;
    ce3 = std::move(ce2);

    ASSERT_THAT(ce3.what(), testing::StrEq(msg2));
    ASSERT_THAT(ce4.what(), testing::StrEq(msg));
}

TEST(exception, invalidValueT) {
    static_assert(
        std::is_base_of<std::runtime_error, clapp::invalid_value_t>::value);
    static_assert(std::is_base_of<clapp::clapp_exception_t,
                                  clapp::invalid_value_t>::value);

    const std::string msg{"message"};
    const std::string msg2{"message2"};

    clapp::invalid_value_t ce1{msg.c_str()};
    clapp::invalid_value_t ce2{msg2};

    clapp::invalid_value_t ce3{ce1};
    clapp::invalid_value_t ce4{std::move(ce1)};

    ce4 = ce3;
    ce3 = std::move(ce2);

    ASSERT_THAT(ce3.what(), testing::StrEq(msg2));
    ASSERT_THAT(ce4.what(), testing::StrEq(msg));
}

TEST(exception, valueUndefinedT) {
    static_assert(
        std::is_base_of<std::runtime_error, clapp::value_undefined_t>::value);
    static_assert(std::is_base_of<clapp::clapp_exception_t,
                                  clapp::value_undefined_t>::value);

    const std::string msg{"message"};
    const std::string msg2{"message2"};

    clapp::value_undefined_t ce1{msg.c_str()};
    clapp::value_undefined_t ce2{msg2};

    clapp::value_undefined_t ce3{ce1};
    clapp::value_undefined_t ce4{std::move(ce1)};

    ce4 = ce3;
    ce3 = std::move(ce2);

    ASSERT_THAT(ce3.what(), testing::StrEq(msg2));
    ASSERT_THAT(ce4.what(), testing::StrEq(msg));
}

TEST(exception, outOfRangeT) {
    static_assert(
        std::is_base_of<std::runtime_error, clapp::out_of_range_t>::value);
    static_assert(std::is_base_of<clapp::clapp_exception_t,
                                  clapp::out_of_range_t>::value);

    const std::string msg{"message"};
    const std::string msg2{"message2"};

    clapp::out_of_range_t ce1{msg.c_str()};
    clapp::out_of_range_t ce2{msg2};

    clapp::out_of_range_t ce3{ce1};
    clapp::out_of_range_t ce4{std::move(ce1)};

    ce4 = ce3;
    ce3 = std::move(ce2);

    ASSERT_THAT(ce3.what(), testing::StrEq(msg2));
    ASSERT_THAT(ce4.what(), testing::StrEq(msg));
}

TEST(exception, pathDoesNotExistT) {
    static_assert(std::is_base_of<std::runtime_error,
                                  clapp::path_does_not_exist_t>::value);
    static_assert(std::is_base_of<clapp::clapp_exception_t,
                                  clapp::path_does_not_exist_t>::value);

    const std::string msg{"message"};
    const std::string msg2{"message2"};

    clapp::path_does_not_exist_t ce1{msg.c_str()};
    clapp::path_does_not_exist_t ce2{msg2};

    clapp::path_does_not_exist_t ce3{ce1};
    clapp::path_does_not_exist_t ce4{std::move(ce1)};

    ce4 = ce3;
    ce3 = std::move(ce2);

    ASSERT_THAT(ce3.what(), testing::StrEq(msg2));
    ASSERT_THAT(ce4.what(), testing::StrEq(msg));
}

TEST(exception, optionExceptionT) {
    static_assert(
        std::is_base_of<std::runtime_error, clapp::option_exception_t>::value);
    static_assert(std::is_base_of<clapp::clapp_exception_t,
                                  clapp::option_exception_t>::value);

    const std::string msg{"message"};
    const std::string msg2{"message2"};

    clapp::option_exception_t ce1{msg.c_str()};
    clapp::option_exception_t ce2{msg2};

    clapp::option_exception_t ce3{ce1};
    clapp::option_exception_t ce4{std::move(ce1)};

    ce4 = ce3;
    ce3 = std::move(ce2);

    ASSERT_THAT(ce3.what(), testing::StrEq(msg2));
    ASSERT_THAT(ce4.what(), testing::StrEq(msg));
}

TEST(exception, optionParamExceptionT) {
    static_assert(std::is_base_of<std::runtime_error,
                                  clapp::option_param_exception_t>::value);
    static_assert(std::is_base_of<clapp::clapp_exception_t,
                                  clapp::option_param_exception_t>::value);

    const std::string msg{"message"};
    const std::string msg2{"message2"};

    clapp::option_param_exception_t ce1{msg.c_str()};
    clapp::option_param_exception_t ce2{msg2};

    clapp::option_param_exception_t ce3{ce1};
    clapp::option_param_exception_t ce4{std::move(ce1)};

    ce4 = ce3;
    ce3 = std::move(ce2);

    ASSERT_THAT(ce3.what(), testing::StrEq(msg2));
    ASSERT_THAT(ce4.what(), testing::StrEq(msg));
}

TEST(exception, argumentExceptionT) {
    static_assert(std::is_base_of<std::runtime_error,
                                  clapp::argument_exception_t>::value);
    static_assert(std::is_base_of<clapp::clapp_exception_t,
                                  clapp::argument_exception_t>::value);

    const std::string msg{"message"};
    const std::string msg2{"message2"};

    clapp::argument_exception_t ce1{msg.c_str()};
    clapp::argument_exception_t ce2{msg2};

    clapp::argument_exception_t ce3{ce1};
    clapp::argument_exception_t ce4{std::move(ce1)};

    ce4 = ce3;
    ce3 = std::move(ce2);

    ASSERT_THAT(ce3.what(), testing::StrEq(msg2));
    ASSERT_THAT(ce4.what(), testing::StrEq(msg));
}

TEST(exception, parserExceptionT) {
    static_assert(
        std::is_base_of<std::runtime_error, clapp::parser_exception_t>::value);
    static_assert(std::is_base_of<clapp::clapp_exception_t,
                                  clapp::parser_exception_t>::value);

    const std::string msg{"message"};
    const std::string msg2{"message2"};

    clapp::parser_exception_t ce1{msg.c_str()};
    clapp::parser_exception_t ce2{msg2};

    clapp::parser_exception_t ce3{ce1};
    clapp::parser_exception_t ce4{std::move(ce1)};

    ce4 = ce3;
    ce3 = std::move(ce2);

    ASSERT_THAT(ce3.what(), testing::StrEq(msg2));
    ASSERT_THAT(ce4.what(), testing::StrEq(msg));
}

TEST(exception, subParserExceptionT) {
    static_assert(std::is_base_of<std::runtime_error,
                                  clapp::sub_parser_exception_t>::value);
    static_assert(std::is_base_of<clapp::clapp_exception_t,
                                  clapp::sub_parser_exception_t>::value);
    static_assert(std::is_base_of<clapp::parser_exception_t,
                                  clapp::sub_parser_exception_t>::value);

    const std::string msg{"message"};
    const std::string msg2{"message2"};

    clapp::sub_parser_exception_t ce1{msg.c_str()};
    clapp::sub_parser_exception_t ce2{msg2};

    clapp::sub_parser_exception_t ce3{ce1};
    clapp::sub_parser_exception_t ce4{std::move(ce1)};

    ce4 = ce3;
    ce3 = std::move(ce2);

    ASSERT_THAT(ce3.what(), testing::StrEq(msg2));
    ASSERT_THAT(ce4.what(), testing::StrEq(msg));
}

TEST(exception, mainParserExceptionT) {
    static_assert(std::is_base_of<std::runtime_error,
                                  clapp::main_parser_exception_t>::value);
    static_assert(std::is_base_of<clapp::clapp_exception_t,
                                  clapp::main_parser_exception_t>::value);
    static_assert(std::is_base_of<clapp::parser_exception_t,
                                  clapp::main_parser_exception_t>::value);

    const std::string msg{"message"};
    const std::string msg2{"message2"};

    clapp::main_parser_exception_t ce1{msg.c_str()};
    clapp::main_parser_exception_t ce2{msg2};

    clapp::main_parser_exception_t ce3{ce1};
    clapp::main_parser_exception_t ce4{std::move(ce1)};

    ce4 = ce3;
    ce3 = std::move(ce2);

    ASSERT_THAT(ce3.what(), testing::StrEq(msg2));
    ASSERT_THAT(ce4.what(), testing::StrEq(msg));
}

TEST(exception, noExecutableExceptionT) {
    static_assert(std::is_base_of<std::runtime_error,
                                  clapp::no_executable_exception_t>::value);
    static_assert(std::is_base_of<clapp::clapp_exception_t,
                                  clapp::no_executable_exception_t>::value);
    static_assert(std::is_base_of<clapp::parser_exception_t,
                                  clapp::no_executable_exception_t>::value);
    static_assert(std::is_base_of<clapp::main_parser_exception_t,
                                  clapp::no_executable_exception_t>::value);

    const std::string msg{"message"};
    const std::string msg2{"message2"};

    clapp::no_executable_exception_t ce1{msg.c_str()};
    clapp::no_executable_exception_t ce2{msg2};

    clapp::no_executable_exception_t ce3{ce1};
    clapp::no_executable_exception_t ce4{std::move(ce1)};

    ce4 = ce3;
    ce3 = std::move(ce2);

    ASSERT_THAT(ce3.what(), testing::StrEq(msg2));
    ASSERT_THAT(ce4.what(), testing::StrEq(msg));
}

TEST(exception, parserContainerExceptionT) {
    static_assert(std::is_base_of<std::runtime_error,
                                  clapp::parser_container_exception_t>::value);
    static_assert(std::is_base_of<clapp::clapp_exception_t,
                                  clapp::parser_container_exception_t>::value);
    static_assert(std::is_base_of<clapp::parser_exception_t,
                                  clapp::parser_container_exception_t>::value);

    const std::string msg{"message"};
    const std::string msg2{"message2"};

    clapp::parser_container_exception_t ce1{msg.c_str()};
    clapp::parser_container_exception_t ce2{msg2};

    clapp::parser_container_exception_t ce3{ce1};
    clapp::parser_container_exception_t ce4{std::move(ce1)};

    ce4 = ce3;
    ce3 = std::move(ce2);

    ASSERT_THAT(ce3.what(), testing::StrEq(msg2));
    ASSERT_THAT(ce4.what(), testing::StrEq(msg));
}

TEST(exception, notFullyParsedExceptionT) {
    static_assert(std::is_base_of<std::runtime_error,
                                  clapp::not_fully_parsed_exception_t>::value);
    static_assert(std::is_base_of<clapp::clapp_exception_t,
                                  clapp::not_fully_parsed_exception_t>::value);
    static_assert(std::is_base_of<clapp::parser_exception_t,
                                  clapp::not_fully_parsed_exception_t>::value);
    static_assert(std::is_base_of<clapp::parser_container_exception_t,
                                  clapp::not_fully_parsed_exception_t>::value);

    const std::string msg{"message"};
    const std::string msg2{"message2"};

    clapp::not_fully_parsed_exception_t ce1{msg.c_str()};
    clapp::not_fully_parsed_exception_t ce2{msg2};

    clapp::not_fully_parsed_exception_t ce3{ce1};
    clapp::not_fully_parsed_exception_t ce4{std::move(ce1)};

    ce4 = ce3;
    ce3 = std::move(ce2);

    ASSERT_THAT(ce3.what(), testing::StrEq(msg2));
    ASSERT_THAT(ce4.what(), testing::StrEq(msg));
}

TEST(exception, invalidParserStateExceptionT) {
    static_assert(
        std::is_base_of<std::runtime_error,
                        clapp::invalid_parser_state_exception_t>::value);
    static_assert(
        std::is_base_of<clapp::clapp_exception_t,
                        clapp::invalid_parser_state_exception_t>::value);
    static_assert(
        std::is_base_of<clapp::parser_exception_t,
                        clapp::invalid_parser_state_exception_t>::value);
    static_assert(
        std::is_base_of<clapp::parser_container_exception_t,
                        clapp::invalid_parser_state_exception_t>::value);

    const std::string msg{"message"};
    const std::string msg2{"message2"};

    clapp::invalid_parser_state_exception_t ce1{msg.c_str()};
    clapp::invalid_parser_state_exception_t ce2{msg2};

    clapp::invalid_parser_state_exception_t ce3{ce1};
    clapp::invalid_parser_state_exception_t ce4{std::move(ce1)};

    ce4 = ce3;
    ce3 = std::move(ce2);

    ASSERT_THAT(ce3.what(), testing::StrEq(msg2));
    ASSERT_THAT(ce4.what(), testing::StrEq(msg));
}
