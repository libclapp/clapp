#include <clapp/main_parser.h>
#include <clapp/option.h>
#include <clapp/parser.h>
#include <gmock/gmock.h>

class empty_main_parser_t : public clapp::parser::basic_main_parser_t {
};

TEST(main_parser, construct_main_parser) {
    constexpr const char* const argv[]{"main", nullptr};
    empty_main_parser_t tp;

    tp.parse(1, argv);
    ASSERT_THAT(static_cast<bool>(tp), testing::Eq(true));
    ASSERT_THAT(tp.get_executable(), testing::StrEq("main"));
}
