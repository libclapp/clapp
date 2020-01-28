#include <clapp/exception.h>
#include <gmock/gmock.h>

TEST(exception, clapp_exception_t) {
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

TEST(exception, invalid_value_t) {
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

TEST(exception, value_undefined_t) {
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

TEST(exception, out_of_range_t) {
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

TEST(exception, path_does_not_exist_t) {
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

TEST(exception, option_exception_t) {
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

TEST(exception, option_param_exception_t) {
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

TEST(exception, argument_exception_t) {
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

TEST(exception, parser_exception_t) {
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

TEST(exception, sub_parser_exception_t) {
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
