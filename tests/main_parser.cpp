#include <clapp/main_parser.h>
#include <clapp/option.h>
#include <clapp/parser.h>
#include <gmock/gmock.h>

class empty_main_parser_t : public clapp::parser::basic_main_parser_t {
   public:
    ~empty_main_parser_t() override;
};

empty_main_parser_t::~empty_main_parser_t() = default;

TEST(main_parser, construct_main_parser) {
    constexpr const char* const argv[]{"main", nullptr};
    empty_main_parser_t tp;

    tp.parse(1, static_cast<const char* const*>(argv));
    ASSERT_THAT(static_cast<bool>(tp), testing::Eq(true));
    ASSERT_THAT(tp.get_executable(), testing::StrEq("main"));
}

TEST(parser, construct_main_parser_and_gen_help_prefix) {
    constexpr const char* const argv[]{"main", nullptr};
    empty_main_parser_t tp;

    tp.parse(1, static_cast<const char* const*>(argv));
    ASSERT_THAT(tp.gen_help_prefix(), testing::StrEq("Usage: \nmain"));
}

TEST(parser, construct_main_parser_parse_and_validate) {
    constexpr const char* const argv[]{"main", nullptr};
    empty_main_parser_t tp;

    tp.parse_and_validate(1, static_cast<const char* const*>(argv));
}
