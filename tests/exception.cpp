#include <clapp/exception.h>
#include <gmock/gmock.h>

TEST(exception, clappExceptionT) {
    static_assert(std::is_base_of<std::runtime_error,
                                  clapp::exception::clapp_exception_t>::value);

    const std::string msg{"message"};
    const std::string msg2{"message2"};

    clapp::exception::clapp_exception_t ce1{msg.c_str()};
    clapp::exception::clapp_exception_t ce2{msg2};

    clapp::exception::clapp_exception_t ce3{ce1};
    clapp::exception::clapp_exception_t ce4{std::move(ce1)};

    ce4 = ce3;
    ce3 = std::move(ce2);

    ASSERT_THAT(ce3.what(), testing::StrEq(msg2));
    ASSERT_THAT(ce4.what(), testing::StrEq(msg));
}

TEST(exception, invalidValueT) {
    static_assert(std::is_base_of<std::runtime_error,
                                  clapp::exception::invalid_value_t>::value);
    static_assert(std::is_base_of<clapp::exception::clapp_exception_t,
                                  clapp::exception::invalid_value_t>::value);

    const std::string msg{"message"};
    const std::string msg2{"message2"};

    clapp::exception::invalid_value_t ce1{msg.c_str()};
    clapp::exception::invalid_value_t ce2{msg2};

    clapp::exception::invalid_value_t ce3{ce1};
    clapp::exception::invalid_value_t ce4{std::move(ce1)};

    ce4 = ce3;
    ce3 = std::move(ce2);

    ASSERT_THAT(ce3.what(), testing::StrEq(msg2));
    ASSERT_THAT(ce4.what(), testing::StrEq(msg));
}

TEST(exception, valueUndefinedT) {
    static_assert(std::is_base_of<std::runtime_error,
                                  clapp::exception::value_undefined_t>::value);
    static_assert(std::is_base_of<clapp::exception::clapp_exception_t,
                                  clapp::exception::value_undefined_t>::value);

    const std::string msg{"message"};
    const std::string msg2{"message2"};

    clapp::exception::value_undefined_t ce1{msg.c_str()};
    clapp::exception::value_undefined_t ce2{msg2};

    clapp::exception::value_undefined_t ce3{ce1};
    clapp::exception::value_undefined_t ce4{std::move(ce1)};

    ce4 = ce3;
    ce3 = std::move(ce2);

    ASSERT_THAT(ce3.what(), testing::StrEq(msg2));
    ASSERT_THAT(ce4.what(), testing::StrEq(msg));
}

TEST(exception, outOfRangeT) {
    static_assert(std::is_base_of<std::runtime_error,
                                  clapp::exception::out_of_range_t>::value);
    static_assert(std::is_base_of<clapp::exception::clapp_exception_t,
                                  clapp::exception::out_of_range_t>::value);

    const std::string msg{"message"};
    const std::string msg2{"message2"};

    clapp::exception::out_of_range_t ce1{msg.c_str()};
    clapp::exception::out_of_range_t ce2{msg2};

    clapp::exception::out_of_range_t ce3{ce1};
    clapp::exception::out_of_range_t ce4{std::move(ce1)};

    ce4 = ce3;
    ce3 = std::move(ce2);

    ASSERT_THAT(ce3.what(), testing::StrEq(msg2));
    ASSERT_THAT(ce4.what(), testing::StrEq(msg));
}

TEST(exception, pathDoesNotExistT) {
    static_assert(
        std::is_base_of<std::runtime_error,
                        clapp::exception::path_does_not_exist_t>::value);
    static_assert(
        std::is_base_of<clapp::exception::clapp_exception_t,
                        clapp::exception::path_does_not_exist_t>::value);

    const std::string msg{"message"};
    const std::string msg2{"message2"};

    clapp::exception::path_does_not_exist_t ce1{msg.c_str()};
    clapp::exception::path_does_not_exist_t ce2{msg2};

    clapp::exception::path_does_not_exist_t ce3{ce1};
    clapp::exception::path_does_not_exist_t ce4{std::move(ce1)};

    ce4 = ce3;
    ce3 = std::move(ce2);

    ASSERT_THAT(ce3.what(), testing::StrEq(msg2));
    ASSERT_THAT(ce4.what(), testing::StrEq(msg));
}

TEST(exception, optionExceptionT) {
    static_assert(std::is_base_of<std::runtime_error,
                                  clapp::exception::option_exception_t>::value);
    static_assert(std::is_base_of<clapp::exception::clapp_exception_t,
                                  clapp::exception::option_exception_t>::value);

    const std::string msg{"message"};
    const std::string msg2{"message2"};

    clapp::exception::option_exception_t ce1{msg.c_str()};
    clapp::exception::option_exception_t ce2{msg2};

    clapp::exception::option_exception_t ce3{ce1};
    clapp::exception::option_exception_t ce4{std::move(ce1)};

    ce4 = ce3;
    ce3 = std::move(ce2);

    ASSERT_THAT(ce3.what(), testing::StrEq(msg2));
    ASSERT_THAT(ce4.what(), testing::StrEq(msg));
}

TEST(exception, optionParamExceptionT) {
    static_assert(
        std::is_base_of<std::runtime_error,
                        clapp::exception::option_param_exception_t>::value);
    static_assert(
        std::is_base_of<clapp::exception::clapp_exception_t,
                        clapp::exception::option_param_exception_t>::value);

    const std::string msg{"message"};
    const std::string msg2{"message2"};

    clapp::exception::option_param_exception_t ce1{msg.c_str()};
    clapp::exception::option_param_exception_t ce2{msg2};

    clapp::exception::option_param_exception_t ce3{ce1};
    clapp::exception::option_param_exception_t ce4{std::move(ce1)};

    ce4 = ce3;
    ce3 = std::move(ce2);

    ASSERT_THAT(ce3.what(), testing::StrEq(msg2));
    ASSERT_THAT(ce4.what(), testing::StrEq(msg));
}

TEST(exception, argumentExceptionT) {
    static_assert(
        std::is_base_of<std::runtime_error,
                        clapp::exception::argument_exception_t>::value);
    static_assert(
        std::is_base_of<clapp::exception::clapp_exception_t,
                        clapp::exception::argument_exception_t>::value);

    const std::string msg{"message"};
    const std::string msg2{"message2"};

    clapp::exception::argument_exception_t ce1{msg.c_str()};
    clapp::exception::argument_exception_t ce2{msg2};

    clapp::exception::argument_exception_t ce3{ce1};
    clapp::exception::argument_exception_t ce4{std::move(ce1)};

    ce4 = ce3;
    ce3 = std::move(ce2);

    ASSERT_THAT(ce3.what(), testing::StrEq(msg2));
    ASSERT_THAT(ce4.what(), testing::StrEq(msg));
}

TEST(exception, parserExceptionT) {
    static_assert(std::is_base_of<std::runtime_error,
                                  clapp::exception::parser_exception_t>::value);
    static_assert(std::is_base_of<clapp::exception::clapp_exception_t,
                                  clapp::exception::parser_exception_t>::value);

    const std::string msg{"message"};
    const std::string msg2{"message2"};

    clapp::exception::parser_exception_t ce1{msg.c_str()};
    clapp::exception::parser_exception_t ce2{msg2};

    clapp::exception::parser_exception_t ce3{ce1};
    clapp::exception::parser_exception_t ce4{std::move(ce1)};

    ce4 = ce3;
    ce3 = std::move(ce2);

    ASSERT_THAT(ce3.what(), testing::StrEq(msg2));
    ASSERT_THAT(ce4.what(), testing::StrEq(msg));
}

TEST(exception, subParserExceptionT) {
    static_assert(
        std::is_base_of<std::runtime_error,
                        clapp::exception::sub_parser_exception_t>::value);
    static_assert(
        std::is_base_of<clapp::exception::clapp_exception_t,
                        clapp::exception::sub_parser_exception_t>::value);
    static_assert(
        std::is_base_of<clapp::exception::parser_exception_t,
                        clapp::exception::sub_parser_exception_t>::value);

    const std::string msg{"message"};
    const std::string msg2{"message2"};

    clapp::exception::sub_parser_exception_t ce1{msg.c_str()};
    clapp::exception::sub_parser_exception_t ce2{msg2};

    clapp::exception::sub_parser_exception_t ce3{ce1};
    clapp::exception::sub_parser_exception_t ce4{std::move(ce1)};

    ce4 = ce3;
    ce3 = std::move(ce2);

    ASSERT_THAT(ce3.what(), testing::StrEq(msg2));
    ASSERT_THAT(ce4.what(), testing::StrEq(msg));
}

TEST(exception, mainParserExceptionT) {
    static_assert(
        std::is_base_of<std::runtime_error,
                        clapp::exception::main_parser_exception_t>::value);
    static_assert(
        std::is_base_of<clapp::exception::clapp_exception_t,
                        clapp::exception::main_parser_exception_t>::value);
    static_assert(
        std::is_base_of<clapp::exception::parser_exception_t,
                        clapp::exception::main_parser_exception_t>::value);

    const std::string msg{"message"};
    const std::string msg2{"message2"};

    clapp::exception::main_parser_exception_t ce1{msg.c_str()};
    clapp::exception::main_parser_exception_t ce2{msg2};

    clapp::exception::main_parser_exception_t ce3{ce1};
    clapp::exception::main_parser_exception_t ce4{std::move(ce1)};

    ce4 = ce3;
    ce3 = std::move(ce2);

    ASSERT_THAT(ce3.what(), testing::StrEq(msg2));
    ASSERT_THAT(ce4.what(), testing::StrEq(msg));
}

TEST(exception, noExecutableExceptionT) {
    static_assert(
        std::is_base_of<std::runtime_error,
                        clapp::exception::no_executable_exception_t>::value);
    static_assert(
        std::is_base_of<clapp::exception::clapp_exception_t,
                        clapp::exception::no_executable_exception_t>::value);
    static_assert(
        std::is_base_of<clapp::exception::parser_exception_t,
                        clapp::exception::no_executable_exception_t>::value);
    static_assert(
        std::is_base_of<clapp::exception::main_parser_exception_t,
                        clapp::exception::no_executable_exception_t>::value);

    const std::string msg{"message"};
    const std::string msg2{"message2"};

    clapp::exception::no_executable_exception_t ce1{msg.c_str()};
    clapp::exception::no_executable_exception_t ce2{msg2};

    clapp::exception::no_executable_exception_t ce3{ce1};
    clapp::exception::no_executable_exception_t ce4{std::move(ce1)};

    ce4 = ce3;
    ce3 = std::move(ce2);

    ASSERT_THAT(ce3.what(), testing::StrEq(msg2));
    ASSERT_THAT(ce4.what(), testing::StrEq(msg));
}
