#include <clapp/option.h>
#include <gmock/gmock.h>

class test_parser_t : public clapp::parser::basic_parser_t {
   public:
    using clapp::parser::basic_parser_t::purpose_t;

    using clapp::parser::basic_parser_t::option_type_t;

    using clapp::parser::basic_parser_t::long_opt_func_t;
    using clapp::parser::basic_parser_t::long_opt_param_func_t;
    using clapp::parser::basic_parser_t::long_opt_variant_t;
    using clapp::parser::basic_parser_t::long_options_map_t;

    using clapp::parser::basic_parser_t::short_opt_func_t;
    using clapp::parser::basic_parser_t::short_opt_param_func_t;
    using clapp::parser::basic_parser_t::short_opt_variant_t;
    using clapp::parser::basic_parser_t::short_options_map_t;

    using clapp::parser::basic_parser_t::option_descriptions_vec_t;
    using clapp::parser::basic_parser_t::validate_func_vec_t;

    using clapp::parser::basic_parser_t::get_long_options;
    using clapp::parser::basic_parser_t::get_mandatory_option_descriptions;
    using clapp::parser::basic_parser_t::get_optional_option_descriptions;
    using clapp::parser::basic_parser_t::get_short_options;
    using clapp::parser::basic_parser_t::get_validate_functions;
};

TEST(option, bool_option_construct_long) {
    const std::string long_opt_name{"long"};
    const std::string opt_desc{"description"};

    test_parser_t tp;
    clapp::option::bool_option_t opt{tp, long_opt_name, opt_desc};
    test_parser_t::long_options_map_t long_options{tp.get_long_options()};
    test_parser_t::short_options_map_t short_options{tp.get_short_options()};
    ASSERT_THAT(tp.get_mandatory_option_descriptions().size(), testing::Eq(0));
    test_parser_t::option_descriptions_vec_t descs{
        tp.get_optional_option_descriptions()};
    test_parser_t::validate_func_vec_t validate_funcs{
        tp.get_validate_functions()};
    ASSERT_THAT(validate_funcs.size(), testing::Eq(0));

    ASSERT_THAT(descs.size(), testing::Eq(1));
    ASSERT_THAT(descs[0].option_string,
                testing::StrEq(std::string{"--"} + long_opt_name));
    ASSERT_THAT(descs[0].description, testing::StrEq(opt_desc));
    ASSERT_THAT(descs[0].option_type,
                testing::Eq(test_parser_t::option_type_t::scalar));

    ASSERT_THAT(short_options.size(), testing::Eq(0));
    ASSERT_THAT(long_options.size(), testing::Eq(1));
    ASSERT_THAT(long_options.count(long_opt_name), testing::Eq(1));
    test_parser_t::long_opt_variant_t long_opt_func_variant{
        long_options.find(long_opt_name)->second};
    ASSERT_THAT(std::holds_alternative<test_parser_t::long_opt_func_t>(
                    long_opt_func_variant),
                testing::Eq(true));
    test_parser_t::long_opt_func_t long_opt_func{
        std::get<test_parser_t::long_opt_func_t>(long_opt_func_variant)};

    ASSERT_THAT(static_cast<bool>(opt), testing::Eq(false));
    ASSERT_THAT(opt.given(), testing::Eq(false));
    ASSERT_THAT(opt.value(), testing::Eq(false));

    long_opt_func(long_opt_name);
    ASSERT_THAT(static_cast<bool>(opt), testing::Eq(true));
    ASSERT_THAT(opt.given(), testing::Eq(true));
    ASSERT_THAT(opt.value(), testing::Eq(true));
}

TEST(option, bool_option_construct_short) {
    const char short_opt_name{'o'};
    const std::string opt_desc{"description"};

    test_parser_t tp;
    clapp::option::bool_option_t opt{tp, short_opt_name, opt_desc};
    test_parser_t::long_options_map_t long_options{tp.get_long_options()};
    test_parser_t::short_options_map_t short_options{tp.get_short_options()};
    ASSERT_THAT(tp.get_mandatory_option_descriptions().size(), testing::Eq(0));
    test_parser_t::option_descriptions_vec_t descs{
        tp.get_optional_option_descriptions()};
    test_parser_t::validate_func_vec_t validate_funcs{
        tp.get_validate_functions()};
    ASSERT_THAT(validate_funcs.size(), testing::Eq(0));

    ASSERT_THAT(descs.size(), testing::Eq(1));
    ASSERT_THAT(descs[0].option_string,
                testing::StrEq(std::string{"-"} + short_opt_name));
    ASSERT_THAT(descs[0].description, testing::StrEq(opt_desc));
    ASSERT_THAT(descs[0].option_type,
                testing::Eq(test_parser_t::option_type_t::scalar));

    ASSERT_THAT(long_options.size(), testing::Eq(0));
    ASSERT_THAT(short_options.size(), testing::Eq(1));
    ASSERT_THAT(short_options.count(short_opt_name), testing::Eq(1));
    test_parser_t::short_opt_variant_t short_opt_func_variant{
        short_options.find(short_opt_name)->second};
    ASSERT_THAT(std::holds_alternative<test_parser_t::short_opt_func_t>(
                    short_opt_func_variant),
                testing::Eq(true));
    test_parser_t::short_opt_func_t short_opt_func{
        std::get<test_parser_t::short_opt_func_t>(short_opt_func_variant)};

    ASSERT_THAT(static_cast<bool>(opt), testing::Eq(false));
    ASSERT_THAT(opt.given(), testing::Eq(false));
    ASSERT_THAT(opt.value(), testing::Eq(false));

    short_opt_func(short_opt_name);
    ASSERT_THAT(static_cast<bool>(opt), testing::Eq(true));
    ASSERT_THAT(opt.given(), testing::Eq(true));
    ASSERT_THAT(opt.value(), testing::Eq(true));
}

TEST(option, bool_option_construct_long_and_short) {
    const std::string long_opt_name1{"option-name1"};
    const char short_opt_name1{'o'};
    const std::string opt_desc1{"description1"};

    const std::string long_opt_name2{"option-name2"};
    const char short_opt_name2{'n'};
    const std::string opt_desc2{"description2"};

    test_parser_t tp;

    clapp::option::bool_option_t opt1{tp, long_opt_name1, short_opt_name1,
                                      opt_desc1};
    clapp::option::bool_option_t opt2{tp, long_opt_name2, short_opt_name2,
                                      opt_desc2};
    test_parser_t::long_options_map_t long_options{tp.get_long_options()};
    test_parser_t::short_options_map_t short_options{tp.get_short_options()};
    ASSERT_THAT(tp.get_mandatory_option_descriptions().size(), testing::Eq(0));
    test_parser_t::option_descriptions_vec_t descs{
        tp.get_optional_option_descriptions()};
    test_parser_t::validate_func_vec_t validate_funcs{
        tp.get_validate_functions()};
    ASSERT_THAT(validate_funcs.size(), testing::Eq(0));

    ASSERT_THAT(descs.size(), testing::Eq(2));
    ASSERT_THAT(descs[0].option_string,
                testing::StrEq(std::string{"-"} + short_opt_name1 + "|--" +
                               long_opt_name1));
    ASSERT_THAT(descs[0].description, testing::StrEq(opt_desc1));
    ASSERT_THAT(descs[0].option_type,
                testing::Eq(test_parser_t::option_type_t::scalar));
    ASSERT_THAT(descs[1].option_string,
                testing::StrEq(std::string{"-"} + short_opt_name2 + "|--" +
                               long_opt_name2));
    ASSERT_THAT(descs[1].description, testing::StrEq(opt_desc2));
    ASSERT_THAT(descs[1].option_type,
                testing::Eq(test_parser_t::option_type_t::scalar));

    ASSERT_THAT(long_options.size(), testing::Eq(2));
    ASSERT_THAT(long_options.count(long_opt_name1), testing::Eq(1));
    ASSERT_THAT(long_options.count(long_opt_name2), testing::Eq(1));
    test_parser_t::long_opt_variant_t long_opt_func_variant1{
        long_options.find(long_opt_name1)->second};
    test_parser_t::long_opt_variant_t long_opt_func_variant2{
        long_options.find(long_opt_name2)->second};
    ASSERT_THAT(std::holds_alternative<test_parser_t::long_opt_func_t>(
                    long_opt_func_variant1),
                testing::Eq(true));
    ASSERT_THAT(std::holds_alternative<test_parser_t::long_opt_func_t>(
                    long_opt_func_variant2),
                testing::Eq(true));
    test_parser_t::long_opt_func_t long_opt_func1{
        std::get<test_parser_t::long_opt_func_t>(long_opt_func_variant1)};
    test_parser_t::long_opt_func_t long_opt_func2{
        std::get<test_parser_t::long_opt_func_t>(long_opt_func_variant2)};

    ASSERT_THAT(short_options.size(), testing::Eq(2));
    ASSERT_THAT(short_options.count(short_opt_name1), testing::Eq(1));
    ASSERT_THAT(short_options.count(short_opt_name2), testing::Eq(1));
    test_parser_t::short_opt_variant_t short_opt_func_variant1{
        short_options.find(short_opt_name1)->second};
    test_parser_t::short_opt_variant_t short_opt_func_variant2{
        short_options.find(short_opt_name2)->second};
    ASSERT_THAT(std::holds_alternative<test_parser_t::short_opt_func_t>(
                    short_opt_func_variant1),
                testing::Eq(true));
    ASSERT_THAT(std::holds_alternative<test_parser_t::short_opt_func_t>(
                    short_opt_func_variant2),
                testing::Eq(true));
    test_parser_t::short_opt_func_t short_opt_func1{
        std::get<test_parser_t::short_opt_func_t>(short_opt_func_variant1)};
    test_parser_t::short_opt_func_t short_opt_func2{
        std::get<test_parser_t::short_opt_func_t>(short_opt_func_variant2)};

    ASSERT_THAT(static_cast<bool>(opt1), testing::Eq(false));
    ASSERT_THAT(opt1.given(), testing::Eq(false));
    ASSERT_THAT(opt1.value(), testing::Eq(false));

    ASSERT_THAT(static_cast<bool>(opt2), testing::Eq(false));
    ASSERT_THAT(opt2.given(), testing::Eq(false));
    ASSERT_THAT(opt2.value(), testing::Eq(false));

    long_opt_func1(long_opt_name1);
    ASSERT_THAT(static_cast<bool>(opt1), testing::Eq(true));
    ASSERT_THAT(opt1.given(), testing::Eq(true));
    ASSERT_THAT(opt1.value(), testing::Eq(true));

    short_opt_func2(short_opt_name2);
    ASSERT_THAT(static_cast<bool>(opt2), testing::Eq(true));
    ASSERT_THAT(opt2.given(), testing::Eq(true));
    ASSERT_THAT(opt2.value(), testing::Eq(true));
}

TEST(option, bool_option_construct_long_mandatory) {
    const std::string long_opt_name{"long"};
    const std::string opt_desc{"description"};

    test_parser_t tp;
    clapp::option::bool_option_t opt{tp, long_opt_name, opt_desc,
                                     test_parser_t::purpose_t::mandatory};
    test_parser_t::long_options_map_t long_options{tp.get_long_options()};
    test_parser_t::short_options_map_t short_options{tp.get_short_options()};
    ASSERT_THAT(tp.get_optional_option_descriptions().size(), testing::Eq(0));
    test_parser_t::option_descriptions_vec_t descs{
        tp.get_mandatory_option_descriptions()};
    test_parser_t::validate_func_vec_t validate_funcs{
        tp.get_validate_functions()};
    ASSERT_THAT(validate_funcs.size(), testing::Eq(1));

    ASSERT_THAT(descs.size(), testing::Eq(1));
    ASSERT_THAT(descs[0].option_string,
                testing::StrEq(std::string{"--"} + long_opt_name));
    ASSERT_THAT(descs[0].description, testing::StrEq(opt_desc));
    ASSERT_THAT(descs[0].option_type,
                testing::Eq(test_parser_t::option_type_t::scalar));

    ASSERT_THAT(short_options.size(), testing::Eq(0));
    ASSERT_THAT(long_options.size(), testing::Eq(1));
    ASSERT_THAT(long_options.count(long_opt_name), testing::Eq(1));
    test_parser_t::long_opt_variant_t long_opt_func_variant{
        long_options.find(long_opt_name)->second};
    ASSERT_THAT(std::holds_alternative<test_parser_t::long_opt_func_t>(
                    long_opt_func_variant),
                testing::Eq(true));
    test_parser_t::long_opt_func_t long_opt_func{
        std::get<test_parser_t::long_opt_func_t>(long_opt_func_variant)};

    ASSERT_THAT(static_cast<bool>(opt), testing::Eq(false));
    ASSERT_THAT(opt.given(), testing::Eq(false));
    ASSERT_THAT(opt.value(), testing::Eq(false));
    ASSERT_THROW(validate_funcs[0](), std::exception);

    long_opt_func(long_opt_name);
    ASSERT_THAT(static_cast<bool>(opt), testing::Eq(true));
    ASSERT_THAT(opt.given(), testing::Eq(true));
    ASSERT_THAT(opt.value(), testing::Eq(true));
    ASSERT_NO_THROW(validate_funcs[0]());
}

TEST(option, count_option_construct_long) {
    const std::string long_opt_name{"long"};
    const std::string opt_desc{"description 2"};

    test_parser_t tp;
    clapp::option::count_option_t opt{tp, long_opt_name, opt_desc};
    test_parser_t::long_options_map_t long_options{tp.get_long_options()};
    test_parser_t::short_options_map_t short_options{tp.get_short_options()};
    ASSERT_THAT(tp.get_mandatory_option_descriptions().size(), testing::Eq(0));
    test_parser_t::option_descriptions_vec_t descs{
        tp.get_optional_option_descriptions()};
    test_parser_t::validate_func_vec_t validate_funcs{
        tp.get_validate_functions()};
    ASSERT_THAT(validate_funcs.size(), testing::Eq(0));

    ASSERT_THAT(descs.size(), testing::Eq(1));
    ASSERT_THAT(descs[0].option_string,
                testing::StrEq(std::string{"--"} + long_opt_name));
    ASSERT_THAT(descs[0].description, testing::StrEq(opt_desc));
    ASSERT_THAT(descs[0].option_type,
                testing::Eq(test_parser_t::option_type_t::scalar));

    ASSERT_THAT(short_options.size(), testing::Eq(0));
    ASSERT_THAT(long_options.size(), testing::Eq(1));
    ASSERT_THAT(long_options.count(long_opt_name), testing::Eq(1));
    test_parser_t::long_opt_variant_t long_opt_func_variant{
        long_options.find(long_opt_name)->second};
    ASSERT_THAT(std::holds_alternative<test_parser_t::long_opt_func_t>(
                    long_opt_func_variant),
                testing::Eq(true));
    test_parser_t::long_opt_func_t long_opt_func{
        std::get<test_parser_t::long_opt_func_t>(long_opt_func_variant)};

    ASSERT_THAT(static_cast<bool>(opt), testing::Eq(false));
    ASSERT_THAT(opt.given(), testing::Eq(false));
    ASSERT_THAT(opt.value(), testing::Eq(0));

    long_opt_func(long_opt_name);
    ASSERT_THAT(static_cast<bool>(opt), testing::Eq(true));
    ASSERT_THAT(opt.given(), testing::Eq(true));
    ASSERT_THAT(opt.value(), testing::Eq(1));

    long_opt_func(long_opt_name);
    ASSERT_THAT(static_cast<bool>(opt), testing::Eq(true));
    ASSERT_THAT(opt.given(), testing::Eq(true));
    ASSERT_THAT(opt.value(), testing::Eq(2));
}

TEST(option, count_option_construct_short) {
    const char short_opt_name{'i'};
    const std::string opt_desc{"description 3"};

    test_parser_t tp;
    clapp::option::count_option_t opt{tp, short_opt_name, opt_desc};
    test_parser_t::long_options_map_t long_options{tp.get_long_options()};
    test_parser_t::short_options_map_t short_options{tp.get_short_options()};
    ASSERT_THAT(tp.get_mandatory_option_descriptions().size(), testing::Eq(0));
    test_parser_t::option_descriptions_vec_t descs{
        tp.get_optional_option_descriptions()};
    test_parser_t::validate_func_vec_t validate_funcs{
        tp.get_validate_functions()};
    ASSERT_THAT(validate_funcs.size(), testing::Eq(0));

    ASSERT_THAT(descs.size(), testing::Eq(1));
    ASSERT_THAT(descs[0].option_string,
                testing::StrEq(std::string{"-"} + short_opt_name));
    ASSERT_THAT(descs[0].description, testing::StrEq(opt_desc));
    ASSERT_THAT(descs[0].option_type,
                testing::Eq(test_parser_t::option_type_t::scalar));

    ASSERT_THAT(long_options.size(), testing::Eq(0));
    ASSERT_THAT(short_options.size(), testing::Eq(1));
    ASSERT_THAT(short_options.count(short_opt_name), testing::Eq(1));
    test_parser_t::short_opt_variant_t short_opt_func_variant{
        short_options.find(short_opt_name)->second};
    ASSERT_THAT(std::holds_alternative<test_parser_t::short_opt_func_t>(
                    short_opt_func_variant),
                testing::Eq(true));
    test_parser_t::short_opt_func_t short_opt_func{
        std::get<test_parser_t::short_opt_func_t>(short_opt_func_variant)};

    ASSERT_THAT(static_cast<bool>(opt), testing::Eq(false));
    ASSERT_THAT(opt.given(), testing::Eq(false));
    ASSERT_THAT(opt.value(), testing::Eq(0));

    short_opt_func(short_opt_name);
    short_opt_func(short_opt_name);
    short_opt_func(short_opt_name);
    ASSERT_THAT(static_cast<bool>(opt), testing::Eq(true));
    ASSERT_THAT(opt.given(), testing::Eq(true));
    ASSERT_THAT(opt.value(), testing::Eq(3));
}

TEST(option, count_option_construct_long_and_short) {
    const std::string long_opt_name1{"option-name1"};
    const char short_opt_name1{'o'};
    const std::string opt_desc1{"description1"};

    const std::string long_opt_name2{"option-name2"};
    const char short_opt_name2{'n'};
    const std::string opt_desc2{"description2"};

    test_parser_t tp;

    clapp::option::count_option_t opt1{tp, long_opt_name1, short_opt_name1,
                                       opt_desc1};
    clapp::option::count_option_t opt2{tp, long_opt_name2, short_opt_name2,
                                       opt_desc2};
    test_parser_t::long_options_map_t long_options{tp.get_long_options()};
    test_parser_t::short_options_map_t short_options{tp.get_short_options()};
    ASSERT_THAT(tp.get_mandatory_option_descriptions().size(), testing::Eq(0));
    test_parser_t::option_descriptions_vec_t descs{
        tp.get_optional_option_descriptions()};
    test_parser_t::validate_func_vec_t validate_funcs{
        tp.get_validate_functions()};
    ASSERT_THAT(validate_funcs.size(), testing::Eq(0));

    ASSERT_THAT(descs.size(), testing::Eq(2));
    ASSERT_THAT(descs[0].option_string,
                testing::StrEq(std::string{"-"} + short_opt_name1 + "|--" +
                               long_opt_name1));
    ASSERT_THAT(descs[0].description, testing::StrEq(opt_desc1));
    ASSERT_THAT(descs[0].option_type,
                testing::Eq(test_parser_t::option_type_t::scalar));
    ASSERT_THAT(descs[1].option_string,
                testing::StrEq(std::string{"-"} + short_opt_name2 + "|--" +
                               long_opt_name2));
    ASSERT_THAT(descs[1].description, testing::StrEq(opt_desc2));
    ASSERT_THAT(descs[1].option_type,
                testing::Eq(test_parser_t::option_type_t::scalar));

    ASSERT_THAT(long_options.size(), testing::Eq(2));
    ASSERT_THAT(long_options.count(long_opt_name1), testing::Eq(1));
    ASSERT_THAT(long_options.count(long_opt_name2), testing::Eq(1));
    test_parser_t::long_opt_variant_t long_opt_func_variant1{
        long_options.find(long_opt_name1)->second};
    test_parser_t::long_opt_variant_t long_opt_func_variant2{
        long_options.find(long_opt_name2)->second};
    ASSERT_THAT(std::holds_alternative<test_parser_t::long_opt_func_t>(
                    long_opt_func_variant1),
                testing::Eq(true));
    ASSERT_THAT(std::holds_alternative<test_parser_t::long_opt_func_t>(
                    long_opt_func_variant2),
                testing::Eq(true));
    test_parser_t::long_opt_func_t long_opt_func1{
        std::get<test_parser_t::long_opt_func_t>(long_opt_func_variant1)};
    test_parser_t::long_opt_func_t long_opt_func2{
        std::get<test_parser_t::long_opt_func_t>(long_opt_func_variant2)};

    ASSERT_THAT(short_options.size(), testing::Eq(2));
    ASSERT_THAT(short_options.count(short_opt_name1), testing::Eq(1));
    ASSERT_THAT(short_options.count(short_opt_name2), testing::Eq(1));
    test_parser_t::short_opt_variant_t short_opt_func_variant1{
        short_options.find(short_opt_name1)->second};
    test_parser_t::short_opt_variant_t short_opt_func_variant2{
        short_options.find(short_opt_name2)->second};
    ASSERT_THAT(std::holds_alternative<test_parser_t::short_opt_func_t>(
                    short_opt_func_variant1),
                testing::Eq(true));
    ASSERT_THAT(std::holds_alternative<test_parser_t::short_opt_func_t>(
                    short_opt_func_variant2),
                testing::Eq(true));
    test_parser_t::short_opt_func_t short_opt_func1{
        std::get<test_parser_t::short_opt_func_t>(short_opt_func_variant1)};
    test_parser_t::short_opt_func_t short_opt_func2{
        std::get<test_parser_t::short_opt_func_t>(short_opt_func_variant2)};

    ASSERT_THAT(static_cast<bool>(opt1), testing::Eq(false));
    ASSERT_THAT(opt1.given(), testing::Eq(false));
    ASSERT_THAT(opt1.value(), testing::Eq(0));

    ASSERT_THAT(static_cast<bool>(opt2), testing::Eq(false));
    ASSERT_THAT(opt2.given(), testing::Eq(false));
    ASSERT_THAT(opt2.value(), testing::Eq(0));

    long_opt_func1(long_opt_name1);
    short_opt_func1(short_opt_name1);
    ASSERT_THAT(static_cast<bool>(opt1), testing::Eq(true));
    ASSERT_THAT(opt1.given(), testing::Eq(true));
    ASSERT_THAT(opt1.value(), testing::Eq(2));

    short_opt_func2(short_opt_name2);
    long_opt_func2(long_opt_name2);
    long_opt_func2(long_opt_name2);
    short_opt_func2(short_opt_name2);
    ASSERT_THAT(static_cast<bool>(opt2), testing::Eq(true));
    ASSERT_THAT(opt2.given(), testing::Eq(true));
    ASSERT_THAT(opt2.value(), testing::Eq(4));
}

TEST(option, count_option_construct_short_min_max) {
    const char short_opt_name{'i'};
    const std::string opt_desc{"description 3"};

    test_parser_t tp;
    clapp::value::min_max_value_t min_max{0, 5};
    clapp::option::count_option_t opt{tp, short_opt_name, opt_desc, min_max};
    test_parser_t::long_options_map_t long_options{tp.get_long_options()};
    test_parser_t::short_options_map_t short_options{tp.get_short_options()};
    ASSERT_THAT(tp.get_mandatory_option_descriptions().size(), testing::Eq(0));
    test_parser_t::option_descriptions_vec_t descs{
        tp.get_optional_option_descriptions()};
    test_parser_t::validate_func_vec_t validate_funcs{
        tp.get_validate_functions()};
    ASSERT_THAT(validate_funcs.size(), testing::Eq(1));

    ASSERT_THAT(descs.size(), testing::Eq(1));
    ASSERT_THAT(descs[0].option_string,
                testing::StrEq(std::string{"-"} + short_opt_name));
    ASSERT_THAT(descs[0].description,
                testing::StrEq(opt_desc + " (" +
                               min_max.append_restriction_text() + ")"));
    ASSERT_THAT(descs[0].option_type,
                testing::Eq(test_parser_t::option_type_t::scalar));

    ASSERT_THAT(long_options.size(), testing::Eq(0));
    ASSERT_THAT(short_options.size(), testing::Eq(1));
    ASSERT_THAT(short_options.count(short_opt_name), testing::Eq(1));
    test_parser_t::short_opt_variant_t short_opt_func_variant{
        short_options.find(short_opt_name)->second};
    ASSERT_THAT(std::holds_alternative<test_parser_t::short_opt_func_t>(
                    short_opt_func_variant),
                testing::Eq(true));
    test_parser_t::short_opt_func_t short_opt_func{
        std::get<test_parser_t::short_opt_func_t>(short_opt_func_variant)};

    ASSERT_THAT(static_cast<bool>(opt), testing::Eq(false));
    ASSERT_THAT(opt.given(), testing::Eq(false));
    ASSERT_THAT(opt.value(), testing::Eq(0));
    ASSERT_NO_THROW(validate_funcs[0]());

    short_opt_func(short_opt_name);
    short_opt_func(short_opt_name);
    short_opt_func(short_opt_name);
    short_opt_func(short_opt_name);
    ASSERT_THAT(static_cast<bool>(opt), testing::Eq(true));
    ASSERT_THAT(opt.given(), testing::Eq(true));
    ASSERT_THAT(opt.value(), testing::Eq(4));
    ASSERT_NO_THROW(validate_funcs[0]());

    short_opt_func(short_opt_name);
    ASSERT_THAT(static_cast<bool>(opt), testing::Eq(true));
    ASSERT_THAT(opt.given(), testing::Eq(true));
    ASSERT_THAT(opt.value(), testing::Eq(5));
    ASSERT_NO_THROW(validate_funcs[0]());

    short_opt_func(short_opt_name);
    ASSERT_THAT(static_cast<bool>(opt), testing::Eq(true));
    ASSERT_THAT(opt.given(), testing::Eq(true));
    ASSERT_THAT(opt.value(), testing::Eq(6));
    ASSERT_THROW(validate_funcs[0](), std::exception);
}

TEST(option, count_option_construct_long_default) {
    const std::string long_opt_name{"long"};
    const std::string opt_desc{"desc"};

    test_parser_t tp;
    clapp::value::default_value_t default_value{4};
    clapp::option::count_option_t opt{tp, long_opt_name, opt_desc,
                                      default_value};
    test_parser_t::long_options_map_t long_options{tp.get_long_options()};
    test_parser_t::short_options_map_t short_options{tp.get_short_options()};
    ASSERT_THAT(tp.get_mandatory_option_descriptions().size(), testing::Eq(0));
    test_parser_t::option_descriptions_vec_t descs{
        tp.get_optional_option_descriptions()};
    test_parser_t::validate_func_vec_t validate_funcs{
        tp.get_validate_functions()};
    ASSERT_THAT(validate_funcs.size(), testing::Eq(0));

    ASSERT_THAT(descs.size(), testing::Eq(1));
    ASSERT_THAT(descs[0].option_string,
                testing::StrEq(std::string{"--"} + long_opt_name));
    ASSERT_THAT(descs[0].description,
                testing::StrEq(opt_desc + " (" +
                               default_value.append_restriction_text() + ")"));
    ASSERT_THAT(descs[0].option_type,
                testing::Eq(test_parser_t::option_type_t::scalar));

    ASSERT_THAT(short_options.size(), testing::Eq(0));
    ASSERT_THAT(long_options.size(), testing::Eq(1));
    ASSERT_THAT(long_options.count(long_opt_name), testing::Eq(1));
    test_parser_t::long_opt_variant_t long_opt_func_variant{
        long_options.find(long_opt_name)->second};
    ASSERT_THAT(std::holds_alternative<test_parser_t::long_opt_func_t>(
                    long_opt_func_variant),
                testing::Eq(true));
    test_parser_t::long_opt_func_t long_opt_func{
        std::get<test_parser_t::long_opt_func_t>(long_opt_func_variant)};

    ASSERT_THAT(static_cast<bool>(opt), testing::Eq(true));
    ASSERT_THAT(opt.given(), testing::Eq(false));
    ASSERT_THAT(opt.value(), testing::Eq(4));

    long_opt_func(long_opt_name);
    ASSERT_THAT(static_cast<bool>(opt), testing::Eq(true));
    ASSERT_THAT(opt.given(), testing::Eq(true));
    ASSERT_THAT(opt.value(), testing::Eq(5));
}

TEST(option, count_option_construct_short_mandatory) {
    const char short_opt_name{'i'};
    const std::string opt_desc{"description 3"};

    test_parser_t tp;
    clapp::option::count_option_t opt{tp, short_opt_name, opt_desc,
                                      test_parser_t::purpose_t::mandatory};
    test_parser_t::long_options_map_t long_options{tp.get_long_options()};
    test_parser_t::short_options_map_t short_options{tp.get_short_options()};
    ASSERT_THAT(tp.get_optional_option_descriptions().size(), testing::Eq(0));
    test_parser_t::option_descriptions_vec_t descs{
        tp.get_mandatory_option_descriptions()};
    test_parser_t::validate_func_vec_t validate_funcs{
        tp.get_validate_functions()};
    ASSERT_THAT(validate_funcs.size(), testing::Eq(1));

    ASSERT_THAT(descs.size(), testing::Eq(1));
    ASSERT_THAT(descs[0].option_string,
                testing::StrEq(std::string{"-"} + short_opt_name));
    ASSERT_THAT(descs[0].description, testing::StrEq(opt_desc));
    ASSERT_THAT(descs[0].option_type,
                testing::Eq(test_parser_t::option_type_t::scalar));

    ASSERT_THAT(long_options.size(), testing::Eq(0));
    ASSERT_THAT(short_options.size(), testing::Eq(1));
    ASSERT_THAT(short_options.count(short_opt_name), testing::Eq(1));
    test_parser_t::short_opt_variant_t short_opt_func_variant{
        short_options.find(short_opt_name)->second};
    ASSERT_THAT(std::holds_alternative<test_parser_t::short_opt_func_t>(
                    short_opt_func_variant),
                testing::Eq(true));
    test_parser_t::short_opt_func_t short_opt_func{
        std::get<test_parser_t::short_opt_func_t>(short_opt_func_variant)};

    ASSERT_THAT(static_cast<bool>(opt), testing::Eq(false));
    ASSERT_THAT(opt.given(), testing::Eq(false));
    ASSERT_THAT(opt.value(), testing::Eq(0));
    ASSERT_THROW(validate_funcs[0](), std::exception);

    short_opt_func(short_opt_name);
    short_opt_func(short_opt_name);
    short_opt_func(short_opt_name);
    ASSERT_THAT(static_cast<bool>(opt), testing::Eq(true));
    ASSERT_THAT(opt.given(), testing::Eq(true));
    ASSERT_THAT(opt.value(), testing::Eq(3));
    ASSERT_NO_THROW(validate_funcs[0]());
}

TEST(option, string_param_option_construct_short) {
    const char short_opt_name{'s'};
    const std::string opt_desc{"string description"};

    test_parser_t tp;
    clapp::option::string_param_option_t opt{tp, short_opt_name, opt_desc};
    test_parser_t::long_options_map_t long_options{tp.get_long_options()};
    test_parser_t::short_options_map_t short_options{tp.get_short_options()};
    ASSERT_THAT(tp.get_mandatory_option_descriptions().size(), testing::Eq(0));
    test_parser_t::option_descriptions_vec_t descs{
        tp.get_optional_option_descriptions()};
    test_parser_t::validate_func_vec_t validate_funcs{
        tp.get_validate_functions()};
    ASSERT_THAT(validate_funcs.size(), testing::Eq(0));

    ASSERT_THAT(descs.size(), testing::Eq(1));
    ASSERT_THAT(descs[0].option_string,
                testing::StrEq(std::string{"-"} + short_opt_name + "=<arg>"));
    ASSERT_THAT(descs[0].description, testing::StrEq(opt_desc));
    ASSERT_THAT(descs[0].option_type,
                testing::Eq(test_parser_t::option_type_t::scalar));

    ASSERT_THAT(long_options.size(), testing::Eq(0));
    ASSERT_THAT(short_options.size(), testing::Eq(1));
    ASSERT_THAT(short_options.count(short_opt_name), testing::Eq(1));
    test_parser_t::short_opt_variant_t short_opt_func_variant{
        short_options.find(short_opt_name)->second};
    ASSERT_THAT(std::holds_alternative<test_parser_t::short_opt_param_func_t>(
                    short_opt_func_variant),
                testing::Eq(true));
    test_parser_t::short_opt_param_func_t short_opt_func{
        std::get<test_parser_t::short_opt_param_func_t>(
            short_opt_func_variant)};

    ASSERT_THAT(static_cast<bool>(opt), testing::Eq(false));
    ASSERT_THAT(opt.given(), testing::Eq(false));
    ASSERT_THROW(opt.value(), std::exception);

    short_opt_func(short_opt_name, "string");
    ASSERT_THAT(static_cast<bool>(opt), testing::Eq(true));
    ASSERT_THAT(opt.given(), testing::Eq(true));
    ASSERT_THAT(opt.value(), testing::StrEq("string"));
}

TEST(option, string_param_option_construct_long_mandatory) {
    const std::string long_opt_name{"string"};
    const std::string opt_desc{"string description"};

    test_parser_t tp;
    clapp::option::string_param_option_t opt{
        tp, long_opt_name, opt_desc, test_parser_t::purpose_t::mandatory};
    test_parser_t::long_options_map_t long_options{tp.get_long_options()};
    test_parser_t::short_options_map_t short_options{tp.get_short_options()};
    ASSERT_THAT(tp.get_optional_option_descriptions().size(), testing::Eq(0));
    test_parser_t::option_descriptions_vec_t descs{
        tp.get_mandatory_option_descriptions()};
    test_parser_t::validate_func_vec_t validate_funcs{
        tp.get_validate_functions()};
    ASSERT_THAT(validate_funcs.size(), testing::Eq(1));

    ASSERT_THAT(descs.size(), testing::Eq(1));
    ASSERT_THAT(descs[0].option_string,
                testing::StrEq(std::string{"--"} + long_opt_name + "=<arg>"));
    ASSERT_THAT(descs[0].description, testing::StrEq(opt_desc));
    ASSERT_THAT(descs[0].option_type,
                testing::Eq(test_parser_t::option_type_t::scalar));

    ASSERT_THAT(short_options.size(), testing::Eq(0));
    ASSERT_THAT(long_options.size(), testing::Eq(1));
    ASSERT_THAT(long_options.count(long_opt_name), testing::Eq(1));
    test_parser_t::long_opt_variant_t long_opt_func_variant{
        long_options.find(long_opt_name)->second};
    ASSERT_THAT(std::holds_alternative<test_parser_t::long_opt_param_func_t>(
                    long_opt_func_variant),
                testing::Eq(true));
    test_parser_t::long_opt_param_func_t long_opt_func{
        std::get<test_parser_t::long_opt_param_func_t>(long_opt_func_variant)};

    ASSERT_THAT(static_cast<bool>(opt), testing::Eq(false));
    ASSERT_THAT(opt.given(), testing::Eq(false));
    ASSERT_THROW(opt.value(), std::exception);
    ASSERT_THROW(validate_funcs[0](), std::exception);

    long_opt_func(long_opt_name, "string");
    ASSERT_THAT(static_cast<bool>(opt), testing::Eq(true));
    ASSERT_THAT(opt.given(), testing::Eq(true));
    ASSERT_THAT(opt.value(), testing::StrEq("string"));
    ASSERT_NO_THROW(validate_funcs[0]());
}

TEST(option, int_param_option_construct_long_and_short_mandatory) {
    const std::string long_opt_name{"int"};
    const char short_opt_name{'i'};
    const std::string opt_desc{"int description"};
    const int default_value{4};
    const int min_value{25};
    const int max_value{55};

    test_parser_t tp;
    clapp::option::int64_param_option_t opt{
        tp,
        long_opt_name,
        short_opt_name,
        opt_desc,
        test_parser_t::purpose_t::mandatory,
        clapp::value::default_value_t{default_value},
        clapp::value::min_max_value_t{min_value, max_value}};
    test_parser_t::long_options_map_t long_options{tp.get_long_options()};
    test_parser_t::short_options_map_t short_options{tp.get_short_options()};
    ASSERT_THAT(tp.get_optional_option_descriptions().size(), testing::Eq(0));
    test_parser_t::option_descriptions_vec_t descs{
        tp.get_mandatory_option_descriptions()};
    test_parser_t::validate_func_vec_t validate_funcs{
        tp.get_validate_functions()};
    ASSERT_THAT(validate_funcs.size(), testing::Eq(1));

    ASSERT_THAT(descs.size(), testing::Eq(1));
    ASSERT_THAT(descs[0].option_string,
                testing::StrEq(std::string{"-"} + short_opt_name +
                               std::string{"|--"} + long_opt_name + "=<arg>"));
    ASSERT_THAT(
        descs[0].description,
        testing::StrEq(opt_desc + " (" +
                       clapp::value::default_value_t{default_value}
                           .append_restriction_text() +
                       ", " +
                       clapp::value::min_max_value_t{min_value, max_value}
                           .append_restriction_text() +
                       ")"));
    ASSERT_THAT(descs[0].option_type,
                testing::Eq(test_parser_t::option_type_t::scalar));

    ASSERT_THAT(short_options.size(), testing::Eq(1));
    ASSERT_THAT(short_options.count(short_opt_name), testing::Eq(1));
    ASSERT_THAT(long_options.size(), testing::Eq(1));
    ASSERT_THAT(long_options.count(long_opt_name), testing::Eq(1));

    test_parser_t::short_opt_variant_t short_opt_func_variant{
        short_options.find(short_opt_name)->second};
    ASSERT_THAT(std::holds_alternative<test_parser_t::short_opt_param_func_t>(
                    short_opt_func_variant),
                testing::Eq(true));
    test_parser_t::short_opt_param_func_t short_opt_func{
        std::get<test_parser_t::short_opt_param_func_t>(
            short_opt_func_variant)};

    test_parser_t::long_opt_variant_t long_opt_func_variant{
        long_options.find(long_opt_name)->second};
    ASSERT_THAT(std::holds_alternative<test_parser_t::long_opt_param_func_t>(
                    long_opt_func_variant),
                testing::Eq(true));
    test_parser_t::long_opt_param_func_t long_opt_func{
        std::get<test_parser_t::long_opt_param_func_t>(long_opt_func_variant)};

    ASSERT_THAT(static_cast<bool>(opt), testing::Eq(true));
    ASSERT_THAT(opt.given(), testing::Eq(false));
    ASSERT_THAT(opt.value(), testing::Eq(default_value));
    ASSERT_THROW(validate_funcs[0](), std::exception);

    long_opt_func(long_opt_name, "123");
    ASSERT_THAT(static_cast<bool>(opt), testing::Eq(true));
    ASSERT_THAT(opt.given(), testing::Eq(true));
    ASSERT_THAT(opt.value(), testing::Eq(123));
    ASSERT_THROW(validate_funcs[0](), std::exception);

    short_opt_func(short_opt_name, "0x2f");
    ASSERT_THAT(static_cast<bool>(opt), testing::Eq(true));
    ASSERT_THAT(opt.given(), testing::Eq(true));
    ASSERT_THAT(opt.value(), testing::Eq(0x2f));
    ASSERT_NO_THROW(validate_funcs[0]());
}

TEST(option, vector_string_param_option_construct_with_default_throws) {
    const char short_opt_name{'s'};
    const std::string opt_desc{"string description"};

    test_parser_t tp;
    ASSERT_THROW((clapp::option::vector_string_param_option_t{
                     tp, short_opt_name, opt_desc,
                     clapp::value::default_value_t{"default string"}}),
                 std::exception);
}

TEST(option, vector_string_param_option_construct_short) {
    const char short_opt_name{'s'};
    const std::string opt_desc{"string description"};

    test_parser_t tp;
    clapp::option::vector_string_param_option_t opt{tp, short_opt_name,
                                                    opt_desc};
    test_parser_t::long_options_map_t long_options{tp.get_long_options()};
    test_parser_t::short_options_map_t short_options{tp.get_short_options()};
    ASSERT_THAT(tp.get_mandatory_option_descriptions().size(), testing::Eq(0));
    test_parser_t::option_descriptions_vec_t descs{
        tp.get_optional_option_descriptions()};
    test_parser_t::validate_func_vec_t validate_funcs{
        tp.get_validate_functions()};
    ASSERT_THAT(validate_funcs.size(), testing::Eq(0));

    ASSERT_THAT(descs.size(), testing::Eq(1));
    ASSERT_THAT(descs[0].option_string,
                testing::StrEq(std::string{"-"} + short_opt_name + "=<arg>"));
    ASSERT_THAT(descs[0].description,
                testing::StrEq(opt_desc + " (vector option)"));
    ASSERT_THAT(descs[0].option_type,
                testing::Eq(test_parser_t::option_type_t::vector));

    ASSERT_THAT(long_options.size(), testing::Eq(0));
    ASSERT_THAT(short_options.size(), testing::Eq(1));
    ASSERT_THAT(short_options.count(short_opt_name), testing::Eq(1));
    test_parser_t::short_opt_variant_t short_opt_func_variant{
        short_options.find(short_opt_name)->second};
    ASSERT_THAT(std::holds_alternative<test_parser_t::short_opt_param_func_t>(
                    short_opt_func_variant),
                testing::Eq(true));
    test_parser_t::short_opt_param_func_t short_opt_func{
        std::get<test_parser_t::short_opt_param_func_t>(
            short_opt_func_variant)};

    ASSERT_THAT(static_cast<bool>(opt), testing::Eq(false));
    ASSERT_THAT(opt.given(), testing::Eq(false));
    ASSERT_THAT(opt.value(), testing::Eq((std::vector<std::string>{})));

    short_opt_func(short_opt_name, "string");
    ASSERT_THAT(static_cast<bool>(opt), testing::Eq(true));
    ASSERT_THAT(opt.given(), testing::Eq(true));
    std::vector<std::string> vec{opt.value()};
    ASSERT_THAT(vec.size(), testing::Eq(1));
    ASSERT_THAT(vec[0], testing::StrEq("string"));

    short_opt_func(short_opt_name, "string2");
    ASSERT_THAT(static_cast<bool>(opt), testing::Eq(true));
    ASSERT_THAT(opt.given(), testing::Eq(true));
    vec = opt.value();
    ASSERT_THAT(vec.size(), testing::Eq(2));
    ASSERT_THAT(vec[0], testing::StrEq("string"));
    ASSERT_THAT(vec[1], testing::StrEq("string2"));
}

TEST(option, vector_string_param_option_construct_long_mandatory) {
    const std::string long_opt_name{"string"};
    const std::string opt_desc{"string description"};

    test_parser_t tp;
    clapp::option::vector_string_param_option_t opt{
        tp, long_opt_name, opt_desc, test_parser_t::purpose_t::mandatory};
    test_parser_t::long_options_map_t long_options{tp.get_long_options()};
    test_parser_t::short_options_map_t short_options{tp.get_short_options()};
    ASSERT_THAT(tp.get_optional_option_descriptions().size(), testing::Eq(0));
    test_parser_t::option_descriptions_vec_t descs{
        tp.get_mandatory_option_descriptions()};
    test_parser_t::validate_func_vec_t validate_funcs{
        tp.get_validate_functions()};
    ASSERT_THAT(validate_funcs.size(), testing::Eq(1));

    ASSERT_THAT(descs.size(), testing::Eq(1));
    ASSERT_THAT(descs[0].option_string,
                testing::StrEq(std::string{"--"} + long_opt_name + "=<arg>"));
    ASSERT_THAT(descs[0].description,
                testing::StrEq(opt_desc + " (vector option)"));
    ASSERT_THAT(descs[0].option_type,
                testing::Eq(test_parser_t::option_type_t::vector));

    ASSERT_THAT(short_options.size(), testing::Eq(0));
    ASSERT_THAT(long_options.size(), testing::Eq(1));
    ASSERT_THAT(long_options.count(long_opt_name), testing::Eq(1));
    test_parser_t::long_opt_variant_t long_opt_func_variant{
        long_options.find(long_opt_name)->second};
    ASSERT_THAT(std::holds_alternative<test_parser_t::long_opt_param_func_t>(
                    long_opt_func_variant),
                testing::Eq(true));
    test_parser_t::long_opt_param_func_t long_opt_func{
        std::get<test_parser_t::long_opt_param_func_t>(long_opt_func_variant)};

    ASSERT_THAT(static_cast<bool>(opt), testing::Eq(false));
    ASSERT_THAT(opt.given(), testing::Eq(false));
    ASSERT_THAT(opt.value(), testing::Eq((std::vector<std::string>{})));
    ASSERT_THROW(validate_funcs[0](), std::exception);

    long_opt_func(long_opt_name, "string");
    ASSERT_THAT(static_cast<bool>(opt), testing::Eq(true));
    ASSERT_THAT(opt.given(), testing::Eq(true));
    std::vector<std::string> vec{opt.value()};
    ASSERT_THAT(vec.size(), testing::Eq(1));
    ASSERT_THAT(vec[0], testing::StrEq("string"));

    long_opt_func(long_opt_name, "string2");
    ASSERT_THAT(static_cast<bool>(opt), testing::Eq(true));
    ASSERT_THAT(opt.given(), testing::Eq(true));
    vec = opt.value();
    ASSERT_THAT(vec.size(), testing::Eq(2));
    ASSERT_THAT(vec[0], testing::StrEq("string"));
    ASSERT_THAT(vec[1], testing::StrEq("string2"));

    long_opt_func(long_opt_name, "string3");
    ASSERT_THAT(static_cast<bool>(opt), testing::Eq(true));
    ASSERT_THAT(opt.given(), testing::Eq(true));
    vec = opt.value();
    ASSERT_THAT(vec.size(), testing::Eq(3));
    ASSERT_THAT(vec[0], testing::StrEq("string"));
    ASSERT_THAT(vec[1], testing::StrEq("string2"));
    ASSERT_THAT(vec[2], testing::StrEq("string3"));
}

TEST(option, vector_string_param_option_construct_long_and_short_min_max) {
    const char short_opt_name{'s'};
    const std::string long_opt_name{"long"};
    const std::string opt_desc{"int description"};
    clapp::value::min_max_value_t min_max{10, 20};

    test_parser_t tp;
    clapp::option::vector_uint8_param_option_t opt{
        tp, long_opt_name, short_opt_name, opt_desc, min_max};
    test_parser_t::long_options_map_t long_options{tp.get_long_options()};
    test_parser_t::short_options_map_t short_options{tp.get_short_options()};
    ASSERT_THAT(tp.get_mandatory_option_descriptions().size(), testing::Eq(0));
    test_parser_t::option_descriptions_vec_t descs{
        tp.get_optional_option_descriptions()};
    test_parser_t::validate_func_vec_t validate_funcs{
        tp.get_validate_functions()};
    ASSERT_THAT(validate_funcs.size(), testing::Eq(1));

    ASSERT_THAT(descs.size(), testing::Eq(1));
    ASSERT_THAT(descs[0].option_string,
                testing::StrEq(std::string{"-"} + short_opt_name + "|--" +
                               long_opt_name + "=<arg>"));
    ASSERT_THAT(descs[0].description,
                testing::StrEq(opt_desc + " (vector option, " +
                               min_max.append_restriction_text() + ")"));
    ASSERT_THAT(descs[0].option_type,
                testing::Eq(test_parser_t::option_type_t::vector));

    ASSERT_THAT(long_options.size(), testing::Eq(1));
    ASSERT_THAT(long_options.count(long_opt_name), testing::Eq(1));
    test_parser_t::long_opt_variant_t long_opt_func_variant{
        long_options.find(long_opt_name)->second};
    ASSERT_THAT(std::holds_alternative<test_parser_t::long_opt_param_func_t>(
                    long_opt_func_variant),
                testing::Eq(true));
    test_parser_t::long_opt_param_func_t long_opt_func{
        std::get<test_parser_t::long_opt_param_func_t>(long_opt_func_variant)};

    ASSERT_THAT(short_options.size(), testing::Eq(1));
    ASSERT_THAT(short_options.count(short_opt_name), testing::Eq(1));
    test_parser_t::short_opt_variant_t short_opt_func_variant{
        short_options.find(short_opt_name)->second};
    ASSERT_THAT(std::holds_alternative<test_parser_t::short_opt_param_func_t>(
                    short_opt_func_variant),
                testing::Eq(true));
    test_parser_t::short_opt_param_func_t short_opt_func{
        std::get<test_parser_t::short_opt_param_func_t>(
            short_opt_func_variant)};

    ASSERT_THAT(static_cast<bool>(opt), testing::Eq(false));
    ASSERT_THAT(opt.given(), testing::Eq(false));
    ASSERT_THAT(opt.value(), testing::Eq((std::vector<std::uint8_t>{})));
    ASSERT_NO_THROW(validate_funcs[0]());

    short_opt_func(short_opt_name, "0x12");
    ASSERT_THAT(static_cast<bool>(opt), testing::Eq(true));
    ASSERT_THAT(opt.given(), testing::Eq(true));
    std::vector<std::uint8_t> vec{opt.value()};
    ASSERT_THAT(vec.size(), testing::Eq(1));
    ASSERT_THAT(vec[0], testing::Eq(0x12));
    ASSERT_NO_THROW(validate_funcs[0]());

    long_opt_func(long_opt_name, "12");
    ASSERT_THAT(static_cast<bool>(opt), testing::Eq(true));
    ASSERT_THAT(opt.given(), testing::Eq(true));
    vec = opt.value();
    ASSERT_THAT(vec.size(), testing::Eq(2));
    ASSERT_THAT(vec[0], testing::Eq(0x12));
    ASSERT_THAT(vec[1], testing::Eq(12));
    ASSERT_NO_THROW(validate_funcs[0]());

    long_opt_func(long_opt_name, "1");
    ASSERT_THAT(static_cast<bool>(opt), testing::Eq(true));
    ASSERT_THAT(opt.given(), testing::Eq(true));
    vec = opt.value();
    ASSERT_THAT(vec.size(), testing::Eq(3));
    ASSERT_THAT(vec[0], testing::Eq(0x12));
    ASSERT_THAT(vec[1], testing::Eq(12));
    ASSERT_THAT(vec[2], testing::Eq(1));
    ASSERT_THROW(validate_funcs[0](), std::exception);
}

TEST(option, invalid_long_option_construct) {
    const std::string long_opt_name1{"long\n"};
    const std::string long_opt_name2{"long\t"};
    const std::string long_opt_name3{"long\r"};
    const std::string long_opt_name4{"long "};
    const std::string long_opt_name5{"long ="};
    const std::string desc{"desc1"};
    const char short_opt_name{'a'};

    test_parser_t tp;
    ASSERT_THROW((clapp::option::bool_option_t{tp, long_opt_name1, desc}),
                 std::exception);
    ASSERT_THROW((clapp::option::bool_option_t{tp, long_opt_name1,
                                               short_opt_name, desc}),
                 std::exception);

    ASSERT_THROW((clapp::option::count_option_t{tp, long_opt_name2, desc}),
                 std::exception);
    ASSERT_THROW((clapp::option::count_option_t{tp, long_opt_name2,
                                                short_opt_name, desc}),
                 std::exception);

    ASSERT_THROW(
        (clapp::option::string_param_option_t{tp, long_opt_name3, desc}),
        std::exception);
    ASSERT_THROW((clapp::option::string_param_option_t{tp, long_opt_name3,
                                                       short_opt_name, desc}),
                 std::exception);

    ASSERT_THROW(
        (clapp::option::vector_string_param_option_t{tp, long_opt_name4, desc}),
        std::exception);
    ASSERT_THROW((clapp::option::vector_string_param_option_t{
                     tp, long_opt_name4, short_opt_name, desc}),
                 std::exception);

    ASSERT_THROW(
        (clapp::option::int32_param_option_t{tp, long_opt_name5, desc}),
        std::exception);
    ASSERT_THROW((clapp::option::int32_param_option_t{tp, long_opt_name5,
                                                      short_opt_name, desc}),
                 std::exception);
}

TEST(option, invalid_short_option_construct) {
    const char short_opt_name1{'\n'};
    const char short_opt_name2{'\t'};
    const char short_opt_name3{'\r'};
    const char short_opt_name4{' '};
    const char short_opt_name5{'='};
    const std::string desc{"desc1"};
    const std::string long_opt_name{'a'};

    test_parser_t tp;
    ASSERT_THROW((clapp::option::bool_option_t{tp, short_opt_name1, desc}),
                 std::exception);
    ASSERT_THROW((clapp::option::bool_option_t{tp, long_opt_name,
                                               short_opt_name1, desc}),
                 std::exception);

    ASSERT_THROW((clapp::option::count_option_t{tp, short_opt_name2, desc}),
                 std::exception);
    ASSERT_THROW((clapp::option::count_option_t{tp, long_opt_name,
                                                short_opt_name2, desc}),
                 std::exception);

    ASSERT_THROW(
        (clapp::option::string_param_option_t{tp, short_opt_name3, desc}),
        std::exception);
    ASSERT_THROW((clapp::option::string_param_option_t{tp, long_opt_name,
                                                       short_opt_name3, desc}),
                 std::exception);

    ASSERT_THROW((clapp::option::vector_string_param_option_t{
                     tp, short_opt_name4, desc}),
                 std::exception);
    ASSERT_THROW((clapp::option::vector_string_param_option_t{
                     tp, long_opt_name, short_opt_name4, desc}),
                 std::exception);

    ASSERT_THROW(
        (clapp::option::int32_param_option_t{tp, short_opt_name5, desc}),
        std::exception);
    ASSERT_THROW((clapp::option::int32_param_option_t{tp, long_opt_name,
                                                      short_opt_name5, desc}),
                 std::exception);
}

struct mock_t {
    MOCK_METHOD(std::int32_t, value_func, ());
    MOCK_METHOD(bool, given_func, ());
    MOCK_METHOD(bool, has_value_func, ());
    MOCK_METHOD(void, validate_func,
                (const std::int32_t&, const std::string& option_string));
};

TEST(option, gen_opt_validate_func) {
    using int32_value_func_t = std::function<std::int32_t(void)>;
    using int32_validate_func_t = std::function<void(
        const std::int32_t&, const std::string& option_string)>;

    {
        mock_t mock;
        std::optional<test_parser_t::validate_func_t> validate_func{
            clapp::option::gen_opt_validate_func<std::int32_t,
                                                 int32_value_func_t>(
                std::nullopt, std::nullopt,
                [&mock]() { return mock.given_func(); },
                std::vector<int32_validate_func_t>{}, "option string",
                test_parser_t::purpose_t::mandatory)};
        ASSERT_THAT(validate_func.has_value(), testing::Eq(true));
        EXPECT_CALL(mock, given_func())
            .Times(1)
            .WillOnce(testing::Return(false));
        ASSERT_THROW((*validate_func)(), std::runtime_error);
    }

    {
        mock_t mock;
        std::optional<test_parser_t::validate_func_t> validate_func{
            clapp::option::gen_opt_validate_func<std::int32_t,
                                                 int32_value_func_t>(
                std::nullopt, [&mock]() { return mock.has_value_func(); },
                [&mock]() { return mock.given_func(); },
                std::vector<int32_validate_func_t>{}, "option string",
                test_parser_t::purpose_t::mandatory)};
        ASSERT_THAT(validate_func.has_value(), testing::Eq(true));
        EXPECT_CALL(mock, given_func())
            .Times(1)
            .WillOnce(testing::Return(true));
        ASSERT_NO_THROW((*validate_func)());
    }

    {
        mock_t mock;
        std::optional<test_parser_t::validate_func_t> validate_func{
            clapp::option::gen_opt_validate_func<std::int32_t,
                                                 int32_value_func_t>(
                [&mock]() { return mock.value_func(); },
                [&mock]() { return mock.has_value_func(); },
                [&mock]() { return mock.given_func(); },
                std::vector<int32_validate_func_t>{
                    [&mock](const std::int32_t& value,
                            const std::string& option_string) {
                        mock.validate_func(value, option_string);
                    }},
                "option string", test_parser_t::purpose_t::mandatory)};
        ASSERT_THAT(validate_func.has_value(), testing::Eq(true));
        EXPECT_CALL(mock, value_func()).Times(1).WillOnce(testing::Return(10));
        EXPECT_CALL(mock, has_value_func())
            .Times(1)
            .WillOnce(testing::Return(true));
        EXPECT_CALL(mock, given_func())
            .Times(1)
            .WillOnce(testing::Return(true));
        EXPECT_CALL(mock, validate_func(10, testing::StrEq("option string")))
            .Times(1);
        ASSERT_NO_THROW((*validate_func)());
    }

    {
        mock_t mock;
        std::optional<test_parser_t::validate_func_t> validate_func{
            clapp::option::gen_opt_validate_func<std::int32_t,
                                                 int32_value_func_t>(
                std::nullopt, std::nullopt,
                [&mock]() { return mock.given_func(); },
                std::vector<int32_validate_func_t>{
                    [&mock](const std::int32_t& value,
                            const std::string& option_string) {
                        mock.validate_func(value, option_string);
                    }},
                "option string", test_parser_t::purpose_t::mandatory)};
        ASSERT_THAT(validate_func.has_value(), testing::Eq(true));
        EXPECT_CALL(mock, given_func())
            .Times(1)
            .WillOnce(testing::Return(true));
        ASSERT_THROW((*validate_func)(), std::exception);
    }
}
