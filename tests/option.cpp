#include <clapp/option.h>
#include <gmock/gmock.h>

class option_test_parser_t : public clapp::parser::basic_parser_t {
   public:
    ~option_test_parser_t() override;
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

    [[nodiscard]] std::string gen_short_line_prefix() const override;
    void set_max_option_string_size(std::size_t max_option_size) override;
    [[nodiscard]] std::size_t get_max_option_string_size() const override;

   private:
    std::size_t max_option_string_size{0};
};

option_test_parser_t::~option_test_parser_t() = default;

std::string option_test_parser_t::gen_short_line_prefix() const {
    return "opt-test-parser" + gen_short_line();
}

void option_test_parser_t::set_max_option_string_size(
    const std::size_t max_option_size) {
    max_option_string_size = max_option_size;
}

std::size_t option_test_parser_t::get_max_option_string_size() const {
    return max_option_string_size;
}

class test_option_t : public clapp::option::basic_option_t<std::int32_t> {
   public:
    template <typename... Params>
    explicit test_option_t(clapp::parser::basic_parser_t& parser,
                           Params... parameters);
    ~test_option_t() override;

    inline explicit operator bool() const;

   private:
    void found_entry();
    static callbacks_t create_callbacks(test_option_t* inst);
};

template <typename... Params>
test_option_t::test_option_t(clapp::basic_parser_t& parser,
                             Params... parameters)
    : clapp::basic_option_t<std::int32_t>{parser, create_callbacks(this),
                                          std::forward<Params>(parameters)...} {
}

void test_option_t::found_entry() {
    _given = true;
    _value = _value.value() + 1;
    for (auto& found_func : _found) {
        found_func.found();
    }
}

inline test_option_t::operator bool() const { return _value.value() != 0; }

test_option_t::~test_option_t() = default;

test_option_t::callbacks_t test_option_t::create_callbacks(
    test_option_t* inst) {
    return callbacks_t{
        [inst](const std::string_view /*option*/) { inst->found_entry(); },
        [inst](const char /*option*/) { inst->found_entry(); },
        [inst]() { return inst->given(); },
        [inst]() { return static_cast<bool>(*inst); },
        [inst]() { return inst->value(); }};
}

TEST(option, basicOptionConstructValueThrows) {
    const std::string long_opt{"long"};
    const std::string opt_desc{"description"};
    option_test_parser_t tp;
    test_option_t opt{tp, long_opt, opt_desc};
    ASSERT_THROW(opt.value(), clapp::exception::value_undefined_t);
}

TEST(option, boolOptionConstructLongOptionTwiceThrows) {
    const std::string long_opt{"long"};
    const std::string opt_desc{"description"};
    option_test_parser_t tp;
    clapp::option::bool_option_t opt{tp, long_opt, opt_desc};
    ASSERT_THROW((clapp::option::bool_option_t{tp, long_opt, opt_desc}),
                 clapp::exception::option_exception_t);
}

TEST(option, boolOptionConstructShortOptionTwiceThrows) {
    const char short_opt{'s'};
    const std::string opt_desc{"description"};
    option_test_parser_t tp;
    clapp::option::bool_option_t opt{tp, short_opt, opt_desc};
    ASSERT_THROW((clapp::option::bool_option_t{tp, short_opt, opt_desc}),
                 clapp::exception::option_exception_t);
}

TEST(option, boolOptionConstructLongString) {
    const std::string long_opt_name{"long"};
    const std::string opt_desc{"description"};

    option_test_parser_t tp;
    clapp::option::bool_option_t opt{tp, long_opt_name, opt_desc};
    option_test_parser_t::long_options_map_t long_options{
        tp.get_long_options()};
    option_test_parser_t::short_options_map_t short_options{
        tp.get_short_options()};
    ASSERT_THAT(tp.get_mandatory_option_descriptions().size(), testing::Eq(0));
    option_test_parser_t::option_descriptions_vec_t descs{
        tp.get_optional_option_descriptions()};
    option_test_parser_t::validate_func_vec_t validate_funcs{
        tp.get_validate_functions()};
    ASSERT_THAT(validate_funcs.size(), testing::Eq(0));

    ASSERT_THAT(descs.size(), testing::Eq(1));
    ASSERT_THAT(descs[0].option_string,
                testing::StrEq(std::string{"--"} + long_opt_name));
    ASSERT_THAT(descs[0].description, testing::StrEq(opt_desc));
    ASSERT_THAT(descs[0].option_type,
                testing::Eq(option_test_parser_t::option_type_t::scalar));

    ASSERT_THAT(short_options.size(), testing::Eq(0));
    ASSERT_THAT(long_options.size(), testing::Eq(1));
    ASSERT_THAT(long_options.count(long_opt_name), testing::Eq(1));
    option_test_parser_t::long_opt_variant_t long_opt_func_variant{
        long_options.find(long_opt_name)->second};
    ASSERT_THAT(std::holds_alternative<option_test_parser_t::long_opt_func_t>(
                    long_opt_func_variant),
                testing::Eq(true));
    option_test_parser_t::long_opt_func_t long_opt_func{
        std::get<option_test_parser_t::long_opt_func_t>(long_opt_func_variant)};

    ASSERT_THAT(static_cast<bool>(opt), testing::Eq(true));
    ASSERT_THAT(opt.has_value(), testing::Eq(true));
    ASSERT_THAT(opt.given(), testing::Eq(false));
    ASSERT_THAT(opt.value(), testing::Eq(false));

    long_opt_func(long_opt_name);
    ASSERT_THAT(static_cast<bool>(opt), testing::Eq(true));
    ASSERT_THAT(opt.has_value(), testing::Eq(true));
    ASSERT_THAT(opt.given(), testing::Eq(true));
    ASSERT_THAT(opt.value(), testing::Eq(true));
}

TEST(option, boolOptionConstructLongCstring) {
    constexpr const char* long_opt_name{"long-cstring"};
    constexpr const char* opt_desc{"description"};

    option_test_parser_t tp;
    clapp::option::bool_option_t opt{tp, long_opt_name, opt_desc};
    option_test_parser_t::long_options_map_t long_options{
        tp.get_long_options()};
    option_test_parser_t::short_options_map_t short_options{
        tp.get_short_options()};
    ASSERT_THAT(tp.get_mandatory_option_descriptions().size(), testing::Eq(0));
    option_test_parser_t::option_descriptions_vec_t descs{
        tp.get_optional_option_descriptions()};
    option_test_parser_t::validate_func_vec_t validate_funcs{
        tp.get_validate_functions()};
    ASSERT_THAT(validate_funcs.size(), testing::Eq(0));

    ASSERT_THAT(descs.size(), testing::Eq(1));
    ASSERT_THAT(descs[0].option_string,
                testing::StrEq(std::string{"--"} + long_opt_name));
    ASSERT_THAT(descs[0].description, testing::StrEq(opt_desc));
    ASSERT_THAT(descs[0].option_type,
                testing::Eq(option_test_parser_t::option_type_t::scalar));

    ASSERT_THAT(short_options.size(), testing::Eq(0));
    ASSERT_THAT(long_options.size(), testing::Eq(1));
    ASSERT_THAT(long_options.count(long_opt_name), testing::Eq(1));
    option_test_parser_t::long_opt_variant_t long_opt_func_variant{
        long_options.find(long_opt_name)->second};
    ASSERT_THAT(std::holds_alternative<option_test_parser_t::long_opt_func_t>(
                    long_opt_func_variant),
                testing::Eq(true));
    option_test_parser_t::long_opt_func_t long_opt_func{
        std::get<option_test_parser_t::long_opt_func_t>(long_opt_func_variant)};

    ASSERT_THAT(static_cast<bool>(opt), testing::Eq(true));
    ASSERT_THAT(opt.has_value(), testing::Eq(true));
    ASSERT_THAT(opt.given(), testing::Eq(false));
    ASSERT_THAT(opt.value(), testing::Eq(false));

    long_opt_func(long_opt_name);
    ASSERT_THAT(static_cast<bool>(opt), testing::Eq(true));
    ASSERT_THAT(opt.has_value(), testing::Eq(true));
    ASSERT_THAT(opt.given(), testing::Eq(true));
    ASSERT_THAT(opt.value(), testing::Eq(true));
}

TEST(option, boolOptionConstructLongVec) {
    const std::string long_opt_name{"long2"};
    const std::string opt_desc{"description2"};

    option_test_parser_t tp;
    clapp::option::bool_option_t opt{
        tp, std::vector<std::string>{long_opt_name}, opt_desc};
    option_test_parser_t::long_options_map_t long_options{
        tp.get_long_options()};
    option_test_parser_t::short_options_map_t short_options{
        tp.get_short_options()};
    ASSERT_THAT(tp.get_mandatory_option_descriptions().size(), testing::Eq(0));
    option_test_parser_t::option_descriptions_vec_t descs{
        tp.get_optional_option_descriptions()};
    option_test_parser_t::validate_func_vec_t validate_funcs{
        tp.get_validate_functions()};
    ASSERT_THAT(validate_funcs.size(), testing::Eq(0));

    ASSERT_THAT(descs.size(), testing::Eq(1));
    ASSERT_THAT(descs[0].option_string,
                testing::StrEq(std::string{"--"} + long_opt_name));
    ASSERT_THAT(descs[0].description, testing::StrEq(opt_desc));
    ASSERT_THAT(descs[0].option_type,
                testing::Eq(option_test_parser_t::option_type_t::scalar));

    ASSERT_THAT(short_options.size(), testing::Eq(0));
    ASSERT_THAT(long_options.size(), testing::Eq(1));
    ASSERT_THAT(long_options.count(long_opt_name), testing::Eq(1));
    option_test_parser_t::long_opt_variant_t long_opt_func_variant{
        long_options.find(long_opt_name)->second};
    ASSERT_THAT(std::holds_alternative<option_test_parser_t::long_opt_func_t>(
                    long_opt_func_variant),
                testing::Eq(true));
    option_test_parser_t::long_opt_func_t long_opt_func{
        std::get<option_test_parser_t::long_opt_func_t>(long_opt_func_variant)};

    ASSERT_THAT(static_cast<bool>(opt), testing::Eq(true));
    ASSERT_THAT(opt.has_value(), testing::Eq(true));
    ASSERT_THAT(opt.given(), testing::Eq(false));
    ASSERT_THAT(opt.value(), testing::Eq(false));

    long_opt_func(long_opt_name);
    ASSERT_THAT(static_cast<bool>(opt), testing::Eq(true));
    ASSERT_THAT(opt.has_value(), testing::Eq(true));
    ASSERT_THAT(opt.given(), testing::Eq(true));
    ASSERT_THAT(opt.value(), testing::Eq(true));
}

TEST(option, boolOptionConstructShort) {
    const char short_opt_name{'o'};
    const std::string opt_desc{"description"};

    option_test_parser_t tp;
    clapp::option::bool_option_t opt{tp, short_opt_name, opt_desc};
    option_test_parser_t::long_options_map_t long_options{
        tp.get_long_options()};
    option_test_parser_t::short_options_map_t short_options{
        tp.get_short_options()};
    ASSERT_THAT(tp.get_mandatory_option_descriptions().size(), testing::Eq(0));
    option_test_parser_t::option_descriptions_vec_t descs{
        tp.get_optional_option_descriptions()};
    option_test_parser_t::validate_func_vec_t validate_funcs{
        tp.get_validate_functions()};
    ASSERT_THAT(validate_funcs.size(), testing::Eq(0));

    ASSERT_THAT(descs.size(), testing::Eq(1));
    ASSERT_THAT(descs[0].option_string,
                testing::StrEq(std::string{"-"} + short_opt_name));
    ASSERT_THAT(descs[0].description, testing::StrEq(opt_desc));
    ASSERT_THAT(descs[0].option_type,
                testing::Eq(option_test_parser_t::option_type_t::scalar));

    ASSERT_THAT(long_options.size(), testing::Eq(0));
    ASSERT_THAT(short_options.size(), testing::Eq(1));
    ASSERT_THAT(short_options.count(short_opt_name), testing::Eq(1));
    option_test_parser_t::short_opt_variant_t short_opt_func_variant{
        short_options.find(short_opt_name)->second};
    ASSERT_THAT(std::holds_alternative<option_test_parser_t::short_opt_func_t>(
                    short_opt_func_variant),
                testing::Eq(true));
    option_test_parser_t::short_opt_func_t short_opt_func{
        std::get<option_test_parser_t::short_opt_func_t>(
            short_opt_func_variant)};

    ASSERT_THAT(static_cast<bool>(opt), testing::Eq(true));
    ASSERT_THAT(opt.has_value(), testing::Eq(true));
    ASSERT_THAT(opt.given(), testing::Eq(false));
    ASSERT_THAT(opt.value(), testing::Eq(false));

    short_opt_func(short_opt_name);
    ASSERT_THAT(static_cast<bool>(opt), testing::Eq(true));
    ASSERT_THAT(opt.has_value(), testing::Eq(true));
    ASSERT_THAT(opt.given(), testing::Eq(true));
    ASSERT_THAT(opt.value(), testing::Eq(true));
}

TEST(option, boolOptionConstructLongAndShortWithStrings) {
    const std::string long_opt_name1{"option-name1"};
    const char short_opt_name1{'o'};
    const std::string opt_desc1{"description1"};

    const std::string long_opt_name2{"option-name2"};
    const char short_opt_name2{'n'};
    const std::string opt_desc2{"description2"};

    option_test_parser_t tp;

    clapp::option::bool_option_t opt1{tp, long_opt_name1, short_opt_name1,
                                      opt_desc1};
    clapp::option::bool_option_t opt2{
        tp, std::vector<std::string>{long_opt_name2},
        std::vector<char>{short_opt_name2}, opt_desc2};
    option_test_parser_t::long_options_map_t long_options{
        tp.get_long_options()};
    option_test_parser_t::short_options_map_t short_options{
        tp.get_short_options()};
    ASSERT_THAT(tp.get_mandatory_option_descriptions().size(), testing::Eq(0));
    option_test_parser_t::option_descriptions_vec_t descs{
        tp.get_optional_option_descriptions()};
    option_test_parser_t::validate_func_vec_t validate_funcs{
        tp.get_validate_functions()};
    ASSERT_THAT(validate_funcs.size(), testing::Eq(0));

    ASSERT_THAT(descs.size(), testing::Eq(2));
    ASSERT_THAT(descs[0].option_string,
                testing::StrEq(std::string{"-"} + short_opt_name1 + "|--" +
                               long_opt_name1));
    ASSERT_THAT(descs[0].description, testing::StrEq(opt_desc1));
    ASSERT_THAT(descs[0].option_type,
                testing::Eq(option_test_parser_t::option_type_t::scalar));
    ASSERT_THAT(descs[1].option_string,
                testing::StrEq(std::string{"-"} + short_opt_name2 + "|--" +
                               long_opt_name2));
    ASSERT_THAT(descs[1].description, testing::StrEq(opt_desc2));
    ASSERT_THAT(descs[1].option_type,
                testing::Eq(option_test_parser_t::option_type_t::scalar));

    ASSERT_THAT(long_options.size(), testing::Eq(2));
    ASSERT_THAT(long_options.count(long_opt_name1), testing::Eq(1));
    ASSERT_THAT(long_options.count(long_opt_name2), testing::Eq(1));
    option_test_parser_t::long_opt_variant_t long_opt_func_variant1{
        long_options.find(long_opt_name1)->second};
    option_test_parser_t::long_opt_variant_t long_opt_func_variant2{
        long_options.find(long_opt_name2)->second};
    ASSERT_THAT(std::holds_alternative<option_test_parser_t::long_opt_func_t>(
                    long_opt_func_variant1),
                testing::Eq(true));
    ASSERT_THAT(std::holds_alternative<option_test_parser_t::long_opt_func_t>(
                    long_opt_func_variant2),
                testing::Eq(true));
    option_test_parser_t::long_opt_func_t long_opt_func1{
        std::get<option_test_parser_t::long_opt_func_t>(
            long_opt_func_variant1)};
    option_test_parser_t::long_opt_func_t long_opt_func2{
        std::get<option_test_parser_t::long_opt_func_t>(
            long_opt_func_variant2)};

    ASSERT_THAT(short_options.size(), testing::Eq(2));
    ASSERT_THAT(short_options.count(short_opt_name1), testing::Eq(1));
    ASSERT_THAT(short_options.count(short_opt_name2), testing::Eq(1));
    option_test_parser_t::short_opt_variant_t short_opt_func_variant1{
        short_options.find(short_opt_name1)->second};
    option_test_parser_t::short_opt_variant_t short_opt_func_variant2{
        short_options.find(short_opt_name2)->second};
    ASSERT_THAT(std::holds_alternative<option_test_parser_t::short_opt_func_t>(
                    short_opt_func_variant1),
                testing::Eq(true));
    ASSERT_THAT(std::holds_alternative<option_test_parser_t::short_opt_func_t>(
                    short_opt_func_variant2),
                testing::Eq(true));
    option_test_parser_t::short_opt_func_t short_opt_func1{
        std::get<option_test_parser_t::short_opt_func_t>(
            short_opt_func_variant1)};
    option_test_parser_t::short_opt_func_t short_opt_func2{
        std::get<option_test_parser_t::short_opt_func_t>(
            short_opt_func_variant2)};

    ASSERT_THAT(static_cast<bool>(opt1), testing::Eq(true));
    ASSERT_THAT(opt1.has_value(), testing::Eq(true));
    ASSERT_THAT(opt1.given(), testing::Eq(false));
    ASSERT_THAT(opt1.value(), testing::Eq(false));

    ASSERT_THAT(static_cast<bool>(opt2), testing::Eq(true));
    ASSERT_THAT(opt2.has_value(), testing::Eq(true));
    ASSERT_THAT(opt2.given(), testing::Eq(false));
    ASSERT_THAT(opt2.value(), testing::Eq(false));

    long_opt_func1(long_opt_name1);
    ASSERT_THAT(static_cast<bool>(opt1), testing::Eq(true));
    ASSERT_THAT(opt1.has_value(), testing::Eq(true));
    ASSERT_THAT(opt1.given(), testing::Eq(true));
    ASSERT_THAT(opt1.value(), testing::Eq(true));

    short_opt_func2(short_opt_name2);
    ASSERT_THAT(static_cast<bool>(opt2), testing::Eq(true));
    ASSERT_THAT(opt2.has_value(), testing::Eq(true));
    ASSERT_THAT(opt2.given(), testing::Eq(true));
    ASSERT_THAT(opt2.value(), testing::Eq(true));
}

TEST(option, boolOptionConstructLongAndShortWithCStrings) {
    constexpr const char* long_opt_name1{"option-name-a"};
    constexpr char short_opt_name1{'a'};
    constexpr const char* opt_desc1{"description1"};

    constexpr const char* long_opt_name2{"option-name-b"};
    constexpr char short_opt_name2{'b'};
    constexpr const char* opt_desc2{"description2"};

    option_test_parser_t tp;

    clapp::option::bool_option_t opt1{tp, long_opt_name1, short_opt_name1,
                                      opt_desc1};
    clapp::option::bool_option_t opt2{
        tp, std::vector<std::string>{long_opt_name2},
        std::vector<char>{short_opt_name2}, opt_desc2};
    option_test_parser_t::long_options_map_t long_options{
        tp.get_long_options()};
    option_test_parser_t::short_options_map_t short_options{
        tp.get_short_options()};
    ASSERT_THAT(tp.get_mandatory_option_descriptions().size(), testing::Eq(0));
    option_test_parser_t::option_descriptions_vec_t descs{
        tp.get_optional_option_descriptions()};
    option_test_parser_t::validate_func_vec_t validate_funcs{
        tp.get_validate_functions()};
    ASSERT_THAT(validate_funcs.size(), testing::Eq(0));

    ASSERT_THAT(descs.size(), testing::Eq(2));
    ASSERT_THAT(descs[0].option_string,
                testing::StrEq(std::string{"-"} + short_opt_name1 + "|--" +
                               long_opt_name1));
    ASSERT_THAT(descs[0].description, testing::StrEq(opt_desc1));
    ASSERT_THAT(descs[0].option_type,
                testing::Eq(option_test_parser_t::option_type_t::scalar));
    ASSERT_THAT(descs[1].option_string,
                testing::StrEq(std::string{"-"} + short_opt_name2 + "|--" +
                               long_opt_name2));
    ASSERT_THAT(descs[1].description, testing::StrEq(opt_desc2));
    ASSERT_THAT(descs[1].option_type,
                testing::Eq(option_test_parser_t::option_type_t::scalar));

    ASSERT_THAT(long_options.size(), testing::Eq(2));
    ASSERT_THAT(long_options.count(long_opt_name1), testing::Eq(1));
    ASSERT_THAT(long_options.count(long_opt_name2), testing::Eq(1));
    option_test_parser_t::long_opt_variant_t long_opt_func_variant1{
        long_options.find(long_opt_name1)->second};
    option_test_parser_t::long_opt_variant_t long_opt_func_variant2{
        long_options.find(long_opt_name2)->second};
    ASSERT_THAT(std::holds_alternative<option_test_parser_t::long_opt_func_t>(
                    long_opt_func_variant1),
                testing::Eq(true));
    ASSERT_THAT(std::holds_alternative<option_test_parser_t::long_opt_func_t>(
                    long_opt_func_variant2),
                testing::Eq(true));
    option_test_parser_t::long_opt_func_t long_opt_func1{
        std::get<option_test_parser_t::long_opt_func_t>(
            long_opt_func_variant1)};
    option_test_parser_t::long_opt_func_t long_opt_func2{
        std::get<option_test_parser_t::long_opt_func_t>(
            long_opt_func_variant2)};

    ASSERT_THAT(short_options.size(), testing::Eq(2));
    ASSERT_THAT(short_options.count(short_opt_name1), testing::Eq(1));
    ASSERT_THAT(short_options.count(short_opt_name2), testing::Eq(1));
    option_test_parser_t::short_opt_variant_t short_opt_func_variant1{
        short_options.find(short_opt_name1)->second};
    option_test_parser_t::short_opt_variant_t short_opt_func_variant2{
        short_options.find(short_opt_name2)->second};
    ASSERT_THAT(std::holds_alternative<option_test_parser_t::short_opt_func_t>(
                    short_opt_func_variant1),
                testing::Eq(true));
    ASSERT_THAT(std::holds_alternative<option_test_parser_t::short_opt_func_t>(
                    short_opt_func_variant2),
                testing::Eq(true));
    option_test_parser_t::short_opt_func_t short_opt_func1{
        std::get<option_test_parser_t::short_opt_func_t>(
            short_opt_func_variant1)};
    option_test_parser_t::short_opt_func_t short_opt_func2{
        std::get<option_test_parser_t::short_opt_func_t>(
            short_opt_func_variant2)};

    ASSERT_THAT(static_cast<bool>(opt1), testing::Eq(true));
    ASSERT_THAT(opt1.has_value(), testing::Eq(true));
    ASSERT_THAT(opt1.given(), testing::Eq(false));
    ASSERT_THAT(opt1.value(), testing::Eq(false));

    ASSERT_THAT(static_cast<bool>(opt2), testing::Eq(true));
    ASSERT_THAT(opt2.has_value(), testing::Eq(true));
    ASSERT_THAT(opt2.given(), testing::Eq(false));
    ASSERT_THAT(opt2.value(), testing::Eq(false));

    long_opt_func1(long_opt_name1);
    ASSERT_THAT(static_cast<bool>(opt1), testing::Eq(true));
    ASSERT_THAT(opt1.has_value(), testing::Eq(true));
    ASSERT_THAT(opt1.given(), testing::Eq(true));
    ASSERT_THAT(opt1.value(), testing::Eq(true));

    short_opt_func2(short_opt_name2);
    ASSERT_THAT(static_cast<bool>(opt2), testing::Eq(true));
    ASSERT_THAT(opt2.has_value(), testing::Eq(true));
    ASSERT_THAT(opt2.given(), testing::Eq(true));
    ASSERT_THAT(opt2.value(), testing::Eq(true));
}

TEST(option, boolOptionConstructLongAndShortWithCStrings2) {
    constexpr const char* long_opt_name1{"option-name-x"};
    constexpr char short_opt_name1{'x'};
    constexpr const char* opt_desc1{"description1"};

    constexpr const char* long_opt_name2{"option-name-y"};
    constexpr char short_opt_name2{'y'};
    constexpr const char* opt_desc2{"description2"};

    option_test_parser_t tp;

    clapp::option::bool_option_t opt1{tp,
                                      std::vector<std::string>{long_opt_name1},
                                      short_opt_name1, opt_desc1};
    clapp::option::bool_option_t opt2{
        tp, long_opt_name2, std::vector<char>{short_opt_name2}, opt_desc2};
    option_test_parser_t::long_options_map_t long_options{
        tp.get_long_options()};
    option_test_parser_t::short_options_map_t short_options{
        tp.get_short_options()};
    ASSERT_THAT(tp.get_mandatory_option_descriptions().size(), testing::Eq(0));
    option_test_parser_t::option_descriptions_vec_t descs{
        tp.get_optional_option_descriptions()};
    option_test_parser_t::validate_func_vec_t validate_funcs{
        tp.get_validate_functions()};
    ASSERT_THAT(validate_funcs.size(), testing::Eq(0));

    ASSERT_THAT(descs.size(), testing::Eq(2));
    ASSERT_THAT(descs[0].option_string,
                testing::StrEq(std::string{"-"} + short_opt_name1 + "|--" +
                               long_opt_name1));
    ASSERT_THAT(descs[0].description, testing::StrEq(opt_desc1));
    ASSERT_THAT(descs[0].option_type,
                testing::Eq(option_test_parser_t::option_type_t::scalar));
    ASSERT_THAT(descs[1].option_string,
                testing::StrEq(std::string{"-"} + short_opt_name2 + "|--" +
                               long_opt_name2));
    ASSERT_THAT(descs[1].description, testing::StrEq(opt_desc2));
    ASSERT_THAT(descs[1].option_type,
                testing::Eq(option_test_parser_t::option_type_t::scalar));

    ASSERT_THAT(long_options.size(), testing::Eq(2));
    ASSERT_THAT(long_options.count(long_opt_name1), testing::Eq(1));
    ASSERT_THAT(long_options.count(long_opt_name2), testing::Eq(1));
    option_test_parser_t::long_opt_variant_t long_opt_func_variant1{
        long_options.find(long_opt_name1)->second};
    option_test_parser_t::long_opt_variant_t long_opt_func_variant2{
        long_options.find(long_opt_name2)->second};
    ASSERT_THAT(std::holds_alternative<option_test_parser_t::long_opt_func_t>(
                    long_opt_func_variant1),
                testing::Eq(true));
    ASSERT_THAT(std::holds_alternative<option_test_parser_t::long_opt_func_t>(
                    long_opt_func_variant2),
                testing::Eq(true));
    option_test_parser_t::long_opt_func_t long_opt_func1{
        std::get<option_test_parser_t::long_opt_func_t>(
            long_opt_func_variant1)};
    option_test_parser_t::long_opt_func_t long_opt_func2{
        std::get<option_test_parser_t::long_opt_func_t>(
            long_opt_func_variant2)};

    ASSERT_THAT(short_options.size(), testing::Eq(2));
    ASSERT_THAT(short_options.count(short_opt_name1), testing::Eq(1));
    ASSERT_THAT(short_options.count(short_opt_name2), testing::Eq(1));
    option_test_parser_t::short_opt_variant_t short_opt_func_variant1{
        short_options.find(short_opt_name1)->second};
    option_test_parser_t::short_opt_variant_t short_opt_func_variant2{
        short_options.find(short_opt_name2)->second};
    ASSERT_THAT(std::holds_alternative<option_test_parser_t::short_opt_func_t>(
                    short_opt_func_variant1),
                testing::Eq(true));
    ASSERT_THAT(std::holds_alternative<option_test_parser_t::short_opt_func_t>(
                    short_opt_func_variant2),
                testing::Eq(true));
    option_test_parser_t::short_opt_func_t short_opt_func1{
        std::get<option_test_parser_t::short_opt_func_t>(
            short_opt_func_variant1)};
    option_test_parser_t::short_opt_func_t short_opt_func2{
        std::get<option_test_parser_t::short_opt_func_t>(
            short_opt_func_variant2)};

    ASSERT_THAT(static_cast<bool>(opt1), testing::Eq(true));
    ASSERT_THAT(opt1.has_value(), testing::Eq(true));
    ASSERT_THAT(opt1.given(), testing::Eq(false));
    ASSERT_THAT(opt1.value(), testing::Eq(false));

    ASSERT_THAT(static_cast<bool>(opt2), testing::Eq(true));
    ASSERT_THAT(opt2.has_value(), testing::Eq(true));
    ASSERT_THAT(opt2.given(), testing::Eq(false));
    ASSERT_THAT(opt2.value(), testing::Eq(false));

    long_opt_func1(long_opt_name1);
    ASSERT_THAT(static_cast<bool>(opt1), testing::Eq(true));
    ASSERT_THAT(opt1.has_value(), testing::Eq(true));
    ASSERT_THAT(opt1.given(), testing::Eq(true));
    ASSERT_THAT(opt1.value(), testing::Eq(true));

    short_opt_func2(short_opt_name2);
    ASSERT_THAT(static_cast<bool>(opt2), testing::Eq(true));
    ASSERT_THAT(opt2.has_value(), testing::Eq(true));
    ASSERT_THAT(opt2.given(), testing::Eq(true));
    ASSERT_THAT(opt2.value(), testing::Eq(true));
}

TEST(option, boolOptionFoundFuncLong) {
    const std::string long_opt_name{"long"};
    const std::string opt_desc{"description"};

    std::stringstream ss;

    option_test_parser_t tp;
    clapp::option::bool_option_t opt{
        tp, long_opt_name, opt_desc, option_test_parser_t::purpose_t::mandatory,
        clapp::value::found_func_t{[&ss]() { ss << "this is a test"; }}};
    option_test_parser_t::short_options_map_t short_options{
        tp.get_short_options()};
    option_test_parser_t::long_options_map_t long_options{
        tp.get_long_options()};
    ASSERT_THAT(short_options.size(), testing::Eq(0));
    ASSERT_THAT(long_options.size(), testing::Eq(1));
    ASSERT_THAT(long_options.count(long_opt_name), testing::Eq(1));
    option_test_parser_t::long_opt_variant_t long_opt_func_variant{
        long_options.find(long_opt_name)->second};
    ASSERT_THAT(std::holds_alternative<option_test_parser_t::long_opt_func_t>(
                    long_opt_func_variant),
                testing::Eq(true));
    option_test_parser_t::long_opt_func_t long_opt_func{
        std::get<option_test_parser_t::long_opt_func_t>(long_opt_func_variant)};
    long_opt_func(long_opt_name);
    ASSERT_THAT(ss.str(), testing::StrEq("this is a test"));
}

TEST(option, boolOptionConstructLongMandatory) {
    const std::string long_opt_name{"long"};
    const std::string opt_desc{"description"};

    option_test_parser_t tp;
    clapp::option::bool_option_t opt{
        tp, long_opt_name, opt_desc,
        option_test_parser_t::purpose_t::mandatory};
    option_test_parser_t::long_options_map_t long_options{
        tp.get_long_options()};
    option_test_parser_t::short_options_map_t short_options{
        tp.get_short_options()};
    ASSERT_THAT(tp.get_optional_option_descriptions().size(), testing::Eq(0));
    option_test_parser_t::option_descriptions_vec_t descs{
        tp.get_mandatory_option_descriptions()};
    option_test_parser_t::validate_func_vec_t validate_funcs{
        tp.get_validate_functions()};
    ASSERT_THAT(validate_funcs.size(), testing::Eq(1));

    ASSERT_THAT(descs.size(), testing::Eq(1));
    ASSERT_THAT(descs[0].option_string,
                testing::StrEq(std::string{"--"} + long_opt_name));
    ASSERT_THAT(descs[0].description, testing::StrEq(opt_desc));
    ASSERT_THAT(descs[0].option_type,
                testing::Eq(option_test_parser_t::option_type_t::scalar));

    ASSERT_THAT(short_options.size(), testing::Eq(0));
    ASSERT_THAT(long_options.size(), testing::Eq(1));
    ASSERT_THAT(long_options.count(long_opt_name), testing::Eq(1));
    option_test_parser_t::long_opt_variant_t long_opt_func_variant{
        long_options.find(long_opt_name)->second};
    ASSERT_THAT(std::holds_alternative<option_test_parser_t::long_opt_func_t>(
                    long_opt_func_variant),
                testing::Eq(true));
    option_test_parser_t::long_opt_func_t long_opt_func{
        std::get<option_test_parser_t::long_opt_func_t>(long_opt_func_variant)};

    ASSERT_THAT(static_cast<bool>(opt), testing::Eq(true));
    ASSERT_THAT(opt.has_value(), testing::Eq(true));
    ASSERT_THAT(opt.given(), testing::Eq(false));
    ASSERT_THAT(opt.value(), testing::Eq(false));
    ASSERT_THROW(validate_funcs[0](),
                 clapp::exception::option_param_exception_t);

    long_opt_func(long_opt_name);
    ASSERT_THAT(static_cast<bool>(opt), testing::Eq(true));
    ASSERT_THAT(opt.has_value(), testing::Eq(true));
    ASSERT_THAT(opt.given(), testing::Eq(true));
    ASSERT_THAT(opt.value(), testing::Eq(true));
    ASSERT_NO_THROW(validate_funcs[0]());
}

TEST(option, helpOptionConstructLong) {
    const std::string long_opt_name{"long"};
    const char short_opt_name{'s'};
    const std::string opt_desc{"description"};

    option_test_parser_t tp;
    clapp::option::help_option_t opt{tp, long_opt_name, short_opt_name,
                                     opt_desc};
    option_test_parser_t::long_options_map_t long_options{
        tp.get_long_options()};
    option_test_parser_t::short_options_map_t short_options{
        tp.get_short_options()};
    ASSERT_THAT(tp.get_mandatory_option_descriptions().size(), testing::Eq(0));
    option_test_parser_t::option_descriptions_vec_t descs{
        tp.get_optional_option_descriptions()};
    ASSERT_THAT(tp.get_validate_functions().size(), testing::Eq(0));

    ASSERT_THAT(descs.size(), testing::Eq(1));
    ASSERT_THAT(descs[0].option_string,
                testing::StrEq(std::string{"-"} + short_opt_name + "|--" +
                               long_opt_name));
    ASSERT_THAT(descs[0].description, testing::StrEq(opt_desc));
    ASSERT_THAT(descs[0].option_type,
                testing::Eq(option_test_parser_t::option_type_t::scalar));

    ASSERT_THAT(short_options.size(), testing::Eq(1));
    ASSERT_THAT(short_options.count(short_opt_name), testing::Eq(1));
    option_test_parser_t::short_opt_variant_t short_opt_func_variant{
        short_options.find(short_opt_name)->second};
    ASSERT_THAT(long_options.size(), testing::Eq(1));
    ASSERT_THAT(long_options.count(long_opt_name), testing::Eq(1));
    option_test_parser_t::long_opt_variant_t long_opt_func_variant{
        long_options.find(long_opt_name)->second};

    ASSERT_THAT(std::holds_alternative<option_test_parser_t::long_opt_func_t>(
                    long_opt_func_variant),
                testing::Eq(true));
    option_test_parser_t::long_opt_func_t long_opt_func{
        std::get<option_test_parser_t::long_opt_func_t>(long_opt_func_variant)};

    ASSERT_THAT(std::holds_alternative<option_test_parser_t::short_opt_func_t>(
                    short_opt_func_variant),
                testing::Eq(true));
    option_test_parser_t::short_opt_func_t short_opt_func{
        std::get<option_test_parser_t::short_opt_func_t>(
            short_opt_func_variant)};

    ASSERT_THAT(static_cast<bool>(opt), testing::Eq(true));
    ASSERT_THAT(opt.has_value(), testing::Eq(true));
    ASSERT_THAT(opt.given(), testing::Eq(false));
    ASSERT_THAT(opt.value(), testing::Eq(false));

    testing::internal::CaptureStdout();
    ASSERT_EXIT(long_opt_func(long_opt_name), ::testing::ExitedWithCode(0), "");
    ASSERT_THAT(
        testing::internal::GetCapturedStdout(),
        testing::StrEq(
            "Usage:\nopt-test-parser [-s|--long]\n\n  Optional Options:\n    "
            "-s|--long description\n"));

    testing::internal::CaptureStdout();
    ASSERT_EXIT(short_opt_func(short_opt_name), ::testing::ExitedWithCode(0),
                "");
    ASSERT_THAT(
        testing::internal::GetCapturedStdout(),
        testing::StrEq(
            "Usage:\nopt-test-parser [-s|--long]\n\n  Optional Options:\n    "
            "-s|--long description\n"));
}

TEST(option, countOptionConstructLong) {
    const std::string long_opt_name{"long"};
    const std::string opt_desc{"description 2"};

    option_test_parser_t tp;
    clapp::option::count_option_t opt{tp, long_opt_name, opt_desc};
    option_test_parser_t::long_options_map_t long_options{
        tp.get_long_options()};
    option_test_parser_t::short_options_map_t short_options{
        tp.get_short_options()};
    ASSERT_THAT(tp.get_mandatory_option_descriptions().size(), testing::Eq(0));
    option_test_parser_t::option_descriptions_vec_t descs{
        tp.get_optional_option_descriptions()};
    option_test_parser_t::validate_func_vec_t validate_funcs{
        tp.get_validate_functions()};
    ASSERT_THAT(validate_funcs.size(), testing::Eq(0));

    ASSERT_THAT(descs.size(), testing::Eq(1));
    ASSERT_THAT(descs[0].option_string,
                testing::StrEq(std::string{"--"} + long_opt_name));
    ASSERT_THAT(descs[0].description, testing::StrEq(opt_desc));
    ASSERT_THAT(descs[0].option_type,
                testing::Eq(option_test_parser_t::option_type_t::scalar));

    ASSERT_THAT(short_options.size(), testing::Eq(0));
    ASSERT_THAT(long_options.size(), testing::Eq(1));
    ASSERT_THAT(long_options.count(long_opt_name), testing::Eq(1));
    option_test_parser_t::long_opt_variant_t long_opt_func_variant{
        long_options.find(long_opt_name)->second};
    ASSERT_THAT(std::holds_alternative<option_test_parser_t::long_opt_func_t>(
                    long_opt_func_variant),
                testing::Eq(true));
    option_test_parser_t::long_opt_func_t long_opt_func{
        std::get<option_test_parser_t::long_opt_func_t>(long_opt_func_variant)};

    ASSERT_THAT(static_cast<bool>(opt), testing::Eq(true));
    ASSERT_THAT(opt.has_value(), testing::Eq(true));
    ASSERT_THAT(opt.given(), testing::Eq(false));
    ASSERT_THAT(opt.value(), testing::Eq(0));

    long_opt_func(long_opt_name);
    ASSERT_THAT(static_cast<bool>(opt), testing::Eq(true));
    ASSERT_THAT(opt.has_value(), testing::Eq(true));
    ASSERT_THAT(opt.given(), testing::Eq(true));
    ASSERT_THAT(opt.value(), testing::Eq(1));

    long_opt_func(long_opt_name);
    ASSERT_THAT(static_cast<bool>(opt), testing::Eq(true));
    ASSERT_THAT(opt.has_value(), testing::Eq(true));
    ASSERT_THAT(opt.given(), testing::Eq(true));
    ASSERT_THAT(opt.value(), testing::Eq(2));
}

TEST(option, countOptionConstructShort) {
    const char short_opt_name{'i'};
    const std::string opt_desc{"description 3"};

    option_test_parser_t tp;
    clapp::option::count_option_t opt{tp, short_opt_name, opt_desc};
    option_test_parser_t::long_options_map_t long_options{
        tp.get_long_options()};
    option_test_parser_t::short_options_map_t short_options{
        tp.get_short_options()};
    ASSERT_THAT(tp.get_mandatory_option_descriptions().size(), testing::Eq(0));
    option_test_parser_t::option_descriptions_vec_t descs{
        tp.get_optional_option_descriptions()};
    option_test_parser_t::validate_func_vec_t validate_funcs{
        tp.get_validate_functions()};
    ASSERT_THAT(validate_funcs.size(), testing::Eq(0));

    ASSERT_THAT(descs.size(), testing::Eq(1));
    ASSERT_THAT(descs[0].option_string,
                testing::StrEq(std::string{"-"} + short_opt_name));
    ASSERT_THAT(descs[0].description, testing::StrEq(opt_desc));
    ASSERT_THAT(descs[0].option_type,
                testing::Eq(option_test_parser_t::option_type_t::scalar));

    ASSERT_THAT(long_options.size(), testing::Eq(0));
    ASSERT_THAT(short_options.size(), testing::Eq(1));
    ASSERT_THAT(short_options.count(short_opt_name), testing::Eq(1));
    option_test_parser_t::short_opt_variant_t short_opt_func_variant{
        short_options.find(short_opt_name)->second};
    ASSERT_THAT(std::holds_alternative<option_test_parser_t::short_opt_func_t>(
                    short_opt_func_variant),
                testing::Eq(true));
    option_test_parser_t::short_opt_func_t short_opt_func{
        std::get<option_test_parser_t::short_opt_func_t>(
            short_opt_func_variant)};

    ASSERT_THAT(static_cast<bool>(opt), testing::Eq(true));
    ASSERT_THAT(opt.has_value(), testing::Eq(true));
    ASSERT_THAT(opt.given(), testing::Eq(false));
    ASSERT_THAT(opt.value(), testing::Eq(0));

    short_opt_func(short_opt_name);
    short_opt_func(short_opt_name);
    short_opt_func(short_opt_name);
    ASSERT_THAT(static_cast<bool>(opt), testing::Eq(true));
    ASSERT_THAT(opt.has_value(), testing::Eq(true));
    ASSERT_THAT(opt.given(), testing::Eq(true));
    ASSERT_THAT(opt.value(), testing::Eq(3));
}

TEST(option, countOptionConstructLongAndShort) {
    const std::string long_opt_name1{"option-name1"};
    const char short_opt_name1{'o'};
    const std::string opt_desc1{"description1"};

    const std::string long_opt_name2{"option-name2"};
    const char short_opt_name2{'n'};
    const std::string opt_desc2{"description2"};

    option_test_parser_t tp;

    clapp::option::count_option_t opt1{tp, long_opt_name1, short_opt_name1,
                                       opt_desc1};
    clapp::option::count_option_t opt2{tp, long_opt_name2, short_opt_name2,
                                       opt_desc2};
    option_test_parser_t::long_options_map_t long_options{
        tp.get_long_options()};
    option_test_parser_t::short_options_map_t short_options{
        tp.get_short_options()};
    ASSERT_THAT(tp.get_mandatory_option_descriptions().size(), testing::Eq(0));
    option_test_parser_t::option_descriptions_vec_t descs{
        tp.get_optional_option_descriptions()};
    option_test_parser_t::validate_func_vec_t validate_funcs{
        tp.get_validate_functions()};
    ASSERT_THAT(validate_funcs.size(), testing::Eq(0));

    ASSERT_THAT(descs.size(), testing::Eq(2));
    ASSERT_THAT(descs[0].option_string,
                testing::StrEq(std::string{"-"} + short_opt_name1 + "|--" +
                               long_opt_name1));
    ASSERT_THAT(descs[0].description, testing::StrEq(opt_desc1));
    ASSERT_THAT(descs[0].option_type,
                testing::Eq(option_test_parser_t::option_type_t::scalar));
    ASSERT_THAT(descs[1].option_string,
                testing::StrEq(std::string{"-"} + short_opt_name2 + "|--" +
                               long_opt_name2));
    ASSERT_THAT(descs[1].description, testing::StrEq(opt_desc2));
    ASSERT_THAT(descs[1].option_type,
                testing::Eq(option_test_parser_t::option_type_t::scalar));

    ASSERT_THAT(long_options.size(), testing::Eq(2));
    ASSERT_THAT(long_options.count(long_opt_name1), testing::Eq(1));
    ASSERT_THAT(long_options.count(long_opt_name2), testing::Eq(1));
    option_test_parser_t::long_opt_variant_t long_opt_func_variant1{
        long_options.find(long_opt_name1)->second};
    option_test_parser_t::long_opt_variant_t long_opt_func_variant2{
        long_options.find(long_opt_name2)->second};
    ASSERT_THAT(std::holds_alternative<option_test_parser_t::long_opt_func_t>(
                    long_opt_func_variant1),
                testing::Eq(true));
    ASSERT_THAT(std::holds_alternative<option_test_parser_t::long_opt_func_t>(
                    long_opt_func_variant2),
                testing::Eq(true));
    option_test_parser_t::long_opt_func_t long_opt_func1{
        std::get<option_test_parser_t::long_opt_func_t>(
            long_opt_func_variant1)};
    option_test_parser_t::long_opt_func_t long_opt_func2{
        std::get<option_test_parser_t::long_opt_func_t>(
            long_opt_func_variant2)};

    ASSERT_THAT(short_options.size(), testing::Eq(2));
    ASSERT_THAT(short_options.count(short_opt_name1), testing::Eq(1));
    ASSERT_THAT(short_options.count(short_opt_name2), testing::Eq(1));
    option_test_parser_t::short_opt_variant_t short_opt_func_variant1{
        short_options.find(short_opt_name1)->second};
    option_test_parser_t::short_opt_variant_t short_opt_func_variant2{
        short_options.find(short_opt_name2)->second};
    ASSERT_THAT(std::holds_alternative<option_test_parser_t::short_opt_func_t>(
                    short_opt_func_variant1),
                testing::Eq(true));
    ASSERT_THAT(std::holds_alternative<option_test_parser_t::short_opt_func_t>(
                    short_opt_func_variant2),
                testing::Eq(true));
    option_test_parser_t::short_opt_func_t short_opt_func1{
        std::get<option_test_parser_t::short_opt_func_t>(
            short_opt_func_variant1)};
    option_test_parser_t::short_opt_func_t short_opt_func2{
        std::get<option_test_parser_t::short_opt_func_t>(
            short_opt_func_variant2)};

    ASSERT_THAT(static_cast<bool>(opt1), testing::Eq(true));
    ASSERT_THAT(opt1.has_value(), testing::Eq(true));
    ASSERT_THAT(opt1.given(), testing::Eq(false));
    ASSERT_THAT(opt1.value(), testing::Eq(0));

    ASSERT_THAT(static_cast<bool>(opt2), testing::Eq(true));
    ASSERT_THAT(opt2.has_value(), testing::Eq(true));
    ASSERT_THAT(opt2.given(), testing::Eq(false));
    ASSERT_THAT(opt2.value(), testing::Eq(0));

    long_opt_func1(long_opt_name1);
    short_opt_func1(short_opt_name1);
    ASSERT_THAT(static_cast<bool>(opt1), testing::Eq(true));
    ASSERT_THAT(opt1.has_value(), testing::Eq(true));
    ASSERT_THAT(opt1.given(), testing::Eq(true));
    ASSERT_THAT(opt1.value(), testing::Eq(2));

    short_opt_func2(short_opt_name2);
    long_opt_func2(long_opt_name2);
    long_opt_func2(long_opt_name2);
    short_opt_func2(short_opt_name2);
    ASSERT_THAT(static_cast<bool>(opt2), testing::Eq(true));
    ASSERT_THAT(opt2.has_value(), testing::Eq(true));
    ASSERT_THAT(opt2.given(), testing::Eq(true));
    ASSERT_THAT(opt2.value(), testing::Eq(4));
}

TEST(option, countOptionConstructShortMinMax) {
    const char short_opt_name{'i'};
    const std::string opt_desc{"description 3"};
    constexpr std::uint32_t min_value{0};
    constexpr std::uint32_t max_value{5};

    option_test_parser_t tp;
    clapp::value::min_max_value_t<std::uint32_t> min_max{min_value, max_value};
    clapp::option::count_option_t opt{tp, short_opt_name, opt_desc, min_max};
    option_test_parser_t::long_options_map_t long_options{
        tp.get_long_options()};
    option_test_parser_t::short_options_map_t short_options{
        tp.get_short_options()};
    ASSERT_THAT(tp.get_mandatory_option_descriptions().size(), testing::Eq(0));
    option_test_parser_t::option_descriptions_vec_t descs{
        tp.get_optional_option_descriptions()};
    option_test_parser_t::validate_func_vec_t validate_funcs{
        tp.get_validate_functions()};
    ASSERT_THAT(validate_funcs.size(), testing::Eq(1));

    ASSERT_THAT(descs.size(), testing::Eq(1));
    ASSERT_THAT(descs[0].option_string,
                testing::StrEq(std::string{"-"} + short_opt_name));
    ASSERT_THAT(descs[0].description,
                testing::StrEq(opt_desc + " (" +
                               min_max.append_restriction_text() + ")"));
    ASSERT_THAT(descs[0].option_type,
                testing::Eq(option_test_parser_t::option_type_t::scalar));

    ASSERT_THAT(long_options.size(), testing::Eq(0));
    ASSERT_THAT(short_options.size(), testing::Eq(1));
    ASSERT_THAT(short_options.count(short_opt_name), testing::Eq(1));
    option_test_parser_t::short_opt_variant_t short_opt_func_variant{
        short_options.find(short_opt_name)->second};
    ASSERT_THAT(std::holds_alternative<option_test_parser_t::short_opt_func_t>(
                    short_opt_func_variant),
                testing::Eq(true));
    option_test_parser_t::short_opt_func_t short_opt_func{
        std::get<option_test_parser_t::short_opt_func_t>(
            short_opt_func_variant)};

    ASSERT_THAT(static_cast<bool>(opt), testing::Eq(true));
    ASSERT_THAT(opt.has_value(), testing::Eq(true));
    ASSERT_THAT(opt.given(), testing::Eq(false));
    ASSERT_THAT(opt.value(), testing::Eq(0));
    ASSERT_NO_THROW(validate_funcs[0]());

    short_opt_func(short_opt_name);
    short_opt_func(short_opt_name);
    short_opt_func(short_opt_name);
    short_opt_func(short_opt_name);
    ASSERT_THAT(static_cast<bool>(opt), testing::Eq(true));
    ASSERT_THAT(opt.has_value(), testing::Eq(true));
    ASSERT_THAT(opt.given(), testing::Eq(true));
    ASSERT_THAT(opt.value(), testing::Eq(4));
    ASSERT_NO_THROW(validate_funcs[0]());

    short_opt_func(short_opt_name);
    ASSERT_THAT(static_cast<bool>(opt), testing::Eq(true));
    ASSERT_THAT(opt.has_value(), testing::Eq(true));
    ASSERT_THAT(opt.given(), testing::Eq(true));
    ASSERT_THAT(opt.value(), testing::Eq(5));
    ASSERT_NO_THROW(validate_funcs[0]());

    short_opt_func(short_opt_name);
    ASSERT_THAT(static_cast<bool>(opt), testing::Eq(true));
    ASSERT_THAT(opt.has_value(), testing::Eq(true));
    ASSERT_THAT(opt.given(), testing::Eq(true));
    ASSERT_THAT(opt.value(), testing::Eq(6));
    ASSERT_THROW(validate_funcs[0](), clapp::exception::out_of_range_t);
}

TEST(option, countOptionConstructLongDefault) {
    const std::string long_opt_name{"long"};
    const std::string opt_desc{"desc"};

    option_test_parser_t tp;
    clapp::value::default_value_t<std::uint32_t> default_value{4};
    clapp::option::count_option_t opt{tp, long_opt_name, opt_desc,
                                      default_value};
    option_test_parser_t::long_options_map_t long_options{
        tp.get_long_options()};
    option_test_parser_t::short_options_map_t short_options{
        tp.get_short_options()};
    ASSERT_THAT(tp.get_mandatory_option_descriptions().size(), testing::Eq(0));
    option_test_parser_t::option_descriptions_vec_t descs{
        tp.get_optional_option_descriptions()};
    option_test_parser_t::validate_func_vec_t validate_funcs{
        tp.get_validate_functions()};
    ASSERT_THAT(validate_funcs.size(), testing::Eq(0));

    ASSERT_THAT(descs.size(), testing::Eq(1));
    ASSERT_THAT(descs[0].option_string,
                testing::StrEq(std::string{"--"} + long_opt_name));
    ASSERT_THAT(descs[0].description,
                testing::StrEq(opt_desc + " (" +
                               default_value.append_restriction_text() + ")"));
    ASSERT_THAT(descs[0].option_type,
                testing::Eq(option_test_parser_t::option_type_t::scalar));

    ASSERT_THAT(short_options.size(), testing::Eq(0));
    ASSERT_THAT(long_options.size(), testing::Eq(1));
    ASSERT_THAT(long_options.count(long_opt_name), testing::Eq(1));
    option_test_parser_t::long_opt_variant_t long_opt_func_variant{
        long_options.find(long_opt_name)->second};
    ASSERT_THAT(std::holds_alternative<option_test_parser_t::long_opt_func_t>(
                    long_opt_func_variant),
                testing::Eq(true));
    option_test_parser_t::long_opt_func_t long_opt_func{
        std::get<option_test_parser_t::long_opt_func_t>(long_opt_func_variant)};

    ASSERT_THAT(static_cast<bool>(opt), testing::Eq(true));
    ASSERT_THAT(opt.has_value(), testing::Eq(true));
    ASSERT_THAT(opt.given(), testing::Eq(false));
    ASSERT_THAT(opt.value(), testing::Eq(4));

    long_opt_func(long_opt_name);
    ASSERT_THAT(static_cast<bool>(opt), testing::Eq(true));
    ASSERT_THAT(opt.has_value(), testing::Eq(true));
    ASSERT_THAT(opt.given(), testing::Eq(true));
    ASSERT_THAT(opt.value(), testing::Eq(5));
}

TEST(option, countOptionFoundFuncShort) {
    const char short_opt_name{'c'};
    const std::string opt_desc{"description"};

    std::stringstream ss;
    option_test_parser_t tp;
    clapp::option::count_option_t opt{
        tp, short_opt_name, opt_desc,
        option_test_parser_t::purpose_t::mandatory,
        clapp::value::found_func_t{[&ss]() { ss << "this is another test"; }}};
    option_test_parser_t::short_options_map_t short_options{
        tp.get_short_options()};
    option_test_parser_t::long_options_map_t long_options{
        tp.get_long_options()};
    ASSERT_THAT(long_options.size(), testing::Eq(0));
    ASSERT_THAT(short_options.size(), testing::Eq(1));
    ASSERT_THAT(short_options.count(short_opt_name), testing::Eq(1));
    option_test_parser_t::short_opt_variant_t short_opt_func_variant{
        short_options.find(short_opt_name)->second};
    ASSERT_THAT(std::holds_alternative<option_test_parser_t::short_opt_func_t>(
                    short_opt_func_variant),
                testing::Eq(true));
    option_test_parser_t::short_opt_func_t short_opt_func{
        std::get<option_test_parser_t::short_opt_func_t>(
            short_opt_func_variant)};

    ASSERT_THAT(static_cast<bool>(opt), testing::Eq(true));
    ASSERT_THAT(opt.has_value(), testing::Eq(true));
    ASSERT_THAT(opt.given(), testing::Eq(false));
    ASSERT_THAT(opt.value(), testing::Eq(0));
    ASSERT_THAT(ss.str(), testing::StrEq(""));

    short_opt_func(short_opt_name);
    ASSERT_THAT(ss.str(), testing::StrEq("this is another test"));

    ASSERT_THAT(static_cast<bool>(opt), testing::Eq(true));
    ASSERT_THAT(opt.has_value(), testing::Eq(true));
    ASSERT_THAT(opt.given(), testing::Eq(true));
    ASSERT_THAT(opt.value(), testing::Eq(1));
}

TEST(option, countOptionConstructShortMandatory) {
    const char short_opt_name{'i'};
    const std::string opt_desc{"description 3"};

    option_test_parser_t tp;
    clapp::option::count_option_t opt{
        tp, short_opt_name, opt_desc,
        option_test_parser_t::purpose_t::mandatory};
    option_test_parser_t::long_options_map_t long_options{
        tp.get_long_options()};
    option_test_parser_t::short_options_map_t short_options{
        tp.get_short_options()};
    ASSERT_THAT(tp.get_optional_option_descriptions().size(), testing::Eq(0));
    option_test_parser_t::option_descriptions_vec_t descs{
        tp.get_mandatory_option_descriptions()};
    option_test_parser_t::validate_func_vec_t validate_funcs{
        tp.get_validate_functions()};
    ASSERT_THAT(validate_funcs.size(), testing::Eq(1));

    ASSERT_THAT(descs.size(), testing::Eq(1));
    ASSERT_THAT(descs[0].option_string,
                testing::StrEq(std::string{"-"} + short_opt_name));
    ASSERT_THAT(descs[0].description, testing::StrEq(opt_desc));
    ASSERT_THAT(descs[0].option_type,
                testing::Eq(option_test_parser_t::option_type_t::scalar));

    ASSERT_THAT(long_options.size(), testing::Eq(0));
    ASSERT_THAT(short_options.size(), testing::Eq(1));
    ASSERT_THAT(short_options.count(short_opt_name), testing::Eq(1));
    option_test_parser_t::short_opt_variant_t short_opt_func_variant{
        short_options.find(short_opt_name)->second};
    ASSERT_THAT(std::holds_alternative<option_test_parser_t::short_opt_func_t>(
                    short_opt_func_variant),
                testing::Eq(true));
    option_test_parser_t::short_opt_func_t short_opt_func{
        std::get<option_test_parser_t::short_opt_func_t>(
            short_opt_func_variant)};

    ASSERT_THAT(static_cast<bool>(opt), testing::Eq(true));
    ASSERT_THAT(opt.has_value(), testing::Eq(true));
    ASSERT_THAT(opt.given(), testing::Eq(false));
    ASSERT_THAT(opt.value(), testing::Eq(0));
    ASSERT_THROW(validate_funcs[0](), clapp::exception::option_exception_t);

    short_opt_func(short_opt_name);
    short_opt_func(short_opt_name);
    short_opt_func(short_opt_name);
    ASSERT_THAT(static_cast<bool>(opt), testing::Eq(true));
    ASSERT_THAT(opt.has_value(), testing::Eq(true));
    ASSERT_THAT(opt.given(), testing::Eq(true));
    ASSERT_THAT(opt.value(), testing::Eq(3));
    ASSERT_NO_THROW(validate_funcs[0]());
}

TEST(option, stringParamOptionConstructShort) {
    const char short_opt_name{'s'};
    const std::string opt_desc{"string description"};

    option_test_parser_t tp;
    clapp::option::string_param_option_t opt{tp, short_opt_name, opt_desc};
    option_test_parser_t::long_options_map_t long_options{
        tp.get_long_options()};
    option_test_parser_t::short_options_map_t short_options{
        tp.get_short_options()};
    ASSERT_THAT(tp.get_mandatory_option_descriptions().size(), testing::Eq(0));
    option_test_parser_t::option_descriptions_vec_t descs{
        tp.get_optional_option_descriptions()};
    option_test_parser_t::validate_func_vec_t validate_funcs{
        tp.get_validate_functions()};
    ASSERT_THAT(validate_funcs.size(), testing::Eq(0));

    ASSERT_THAT(descs.size(), testing::Eq(1));
    ASSERT_THAT(descs[0].option_string,
                testing::StrEq(std::string{"-"} + short_opt_name + "=<arg>"));
    ASSERT_THAT(descs[0].description, testing::StrEq(opt_desc));
    ASSERT_THAT(descs[0].option_type,
                testing::Eq(option_test_parser_t::option_type_t::scalar));

    ASSERT_THAT(long_options.size(), testing::Eq(0));
    ASSERT_THAT(short_options.size(), testing::Eq(1));
    ASSERT_THAT(short_options.count(short_opt_name), testing::Eq(1));
    option_test_parser_t::short_opt_variant_t short_opt_func_variant{
        short_options.find(short_opt_name)->second};
    ASSERT_THAT(
        std::holds_alternative<option_test_parser_t::short_opt_param_func_t>(
            short_opt_func_variant),
        testing::Eq(true));
    option_test_parser_t::short_opt_param_func_t short_opt_func{
        std::get<option_test_parser_t::short_opt_param_func_t>(
            short_opt_func_variant)};

    ASSERT_THAT(static_cast<bool>(opt), testing::Eq(false));
    ASSERT_THAT(opt.has_value(), testing::Eq(false));
    ASSERT_THAT(opt.given(), testing::Eq(false));
    ASSERT_THROW(opt.value(), clapp::exception::value_undefined_t);

    short_opt_func(short_opt_name, "string");
    ASSERT_THAT(static_cast<bool>(opt), testing::Eq(true));
    ASSERT_THAT(opt.has_value(), testing::Eq(true));
    ASSERT_THAT(opt.given(), testing::Eq(true));
    ASSERT_THAT(opt.value(), testing::StrEq("string"));
}

TEST(option, stringParamOptionFoundFuncShort) {
    const char short_opt_name{'s'};
    const std::string opt_desc{"description"};

    std::stringstream ss;
    option_test_parser_t tp;
    clapp::option::string_param_option_t opt{
        tp, short_opt_name, opt_desc,
        option_test_parser_t::purpose_t::mandatory,
        clapp::value::found_func_t{
            [&ss]() { ss << "this is another test for params"; }}};
    option_test_parser_t::short_options_map_t short_options{
        tp.get_short_options()};
    option_test_parser_t::long_options_map_t long_options{
        tp.get_long_options()};
    ASSERT_THAT(long_options.size(), testing::Eq(0));
    ASSERT_THAT(short_options.size(), testing::Eq(1));
    ASSERT_THAT(short_options.count(short_opt_name), testing::Eq(1));
    option_test_parser_t::short_opt_variant_t short_opt_func_variant{
        short_options.find(short_opt_name)->second};
    ASSERT_THAT(
        std::holds_alternative<option_test_parser_t::short_opt_param_func_t>(
            short_opt_func_variant),
        testing::Eq(true));
    option_test_parser_t::short_opt_param_func_t short_opt_func{
        std::get<option_test_parser_t::short_opt_param_func_t>(
            short_opt_func_variant)};

    ASSERT_THAT(static_cast<bool>(opt), testing::Eq(false));
    ASSERT_THAT(opt.has_value(), testing::Eq(false));
    ASSERT_THAT(opt.given(), testing::Eq(false));
    ASSERT_THAT(ss.str(), testing::StrEq(""));
    short_opt_func(short_opt_name, "param");

    ASSERT_THAT(ss.str(), testing::StrEq("this is another test for params"));
    ASSERT_THAT(static_cast<bool>(opt), testing::Eq(true));
    ASSERT_THAT(opt.has_value(), testing::Eq(true));
    ASSERT_THAT(opt.given(), testing::Eq(true));
    ASSERT_THAT(opt.value(), testing::StrEq("param"));
}

TEST(option, stringParamOptionConstructLongMandatory) {
    const std::string long_opt_name{"string"};
    const std::string opt_desc{"string description"};

    option_test_parser_t tp;
    clapp::option::string_param_option_t opt{
        tp, long_opt_name, opt_desc,
        option_test_parser_t::purpose_t::mandatory};
    option_test_parser_t::long_options_map_t long_options{
        tp.get_long_options()};
    option_test_parser_t::short_options_map_t short_options{
        tp.get_short_options()};
    ASSERT_THAT(tp.get_optional_option_descriptions().size(), testing::Eq(0));
    option_test_parser_t::option_descriptions_vec_t descs{
        tp.get_mandatory_option_descriptions()};
    option_test_parser_t::validate_func_vec_t validate_funcs{
        tp.get_validate_functions()};
    ASSERT_THAT(validate_funcs.size(), testing::Eq(1));

    ASSERT_THAT(descs.size(), testing::Eq(1));
    ASSERT_THAT(descs[0].option_string,
                testing::StrEq(std::string{"--"} + long_opt_name + "=<arg>"));
    ASSERT_THAT(descs[0].description, testing::StrEq(opt_desc));
    ASSERT_THAT(descs[0].option_type,
                testing::Eq(option_test_parser_t::option_type_t::scalar));

    ASSERT_THAT(short_options.size(), testing::Eq(0));
    ASSERT_THAT(long_options.size(), testing::Eq(1));
    ASSERT_THAT(long_options.count(long_opt_name), testing::Eq(1));
    option_test_parser_t::long_opt_variant_t long_opt_func_variant{
        long_options.find(long_opt_name)->second};
    ASSERT_THAT(
        std::holds_alternative<option_test_parser_t::long_opt_param_func_t>(
            long_opt_func_variant),
        testing::Eq(true));
    option_test_parser_t::long_opt_param_func_t long_opt_func{
        std::get<option_test_parser_t::long_opt_param_func_t>(
            long_opt_func_variant)};

    ASSERT_THAT(static_cast<bool>(opt), testing::Eq(false));
    ASSERT_THAT(opt.has_value(), testing::Eq(false));
    ASSERT_THAT(opt.given(), testing::Eq(false));
    ASSERT_THROW(opt.value(), clapp::exception::value_undefined_t);
    ASSERT_THROW(validate_funcs[0](), clapp::exception::option_exception_t);

    long_opt_func(long_opt_name, "string");
    ASSERT_THAT(static_cast<bool>(opt), testing::Eq(true));
    ASSERT_THAT(opt.has_value(), testing::Eq(true));
    ASSERT_THAT(opt.given(), testing::Eq(true));
    ASSERT_THAT(opt.value(), testing::StrEq("string"));
    ASSERT_NO_THROW(validate_funcs[0]());
}

TEST(option, boolParamOptionConstructLongAndShortMandatory) {
    const std::string long_opt_name{"bool"};
    const char short_opt_name{'b'};
    const std::string opt_desc{"bool description"};

    option_test_parser_t tp;
    clapp::option::bool_param_option_t opt{
        tp, long_opt_name, short_opt_name, opt_desc,
        option_test_parser_t::purpose_t::mandatory};
    option_test_parser_t::long_options_map_t long_options{
        tp.get_long_options()};
    option_test_parser_t::short_options_map_t short_options{
        tp.get_short_options()};
    ASSERT_THAT(tp.get_optional_option_descriptions().size(), testing::Eq(0));
    option_test_parser_t::option_descriptions_vec_t descs{
        tp.get_mandatory_option_descriptions()};
    option_test_parser_t::validate_func_vec_t validate_funcs{
        tp.get_validate_functions()};
    ASSERT_THAT(validate_funcs.size(), testing::Eq(1));

    ASSERT_THAT(descs.size(), testing::Eq(1));
    ASSERT_THAT(descs[0].option_string,
                testing::StrEq(std::string{"-"} + short_opt_name +
                               std::string{"|--"} + long_opt_name + "=<arg>"));
    ASSERT_THAT(descs[0].option_type,
                testing::Eq(option_test_parser_t::option_type_t::scalar));

    ASSERT_THAT(short_options.size(), testing::Eq(1));
    ASSERT_THAT(short_options.count(short_opt_name), testing::Eq(1));
    ASSERT_THAT(long_options.size(), testing::Eq(1));
    ASSERT_THAT(long_options.count(long_opt_name), testing::Eq(1));

    option_test_parser_t::short_opt_variant_t short_opt_func_variant{
        short_options.find(short_opt_name)->second};
    ASSERT_THAT(
        std::holds_alternative<option_test_parser_t::short_opt_param_func_t>(
            short_opt_func_variant),
        testing::Eq(true));
    option_test_parser_t::short_opt_param_func_t short_opt_func{
        std::get<option_test_parser_t::short_opt_param_func_t>(
            short_opt_func_variant)};

    option_test_parser_t::long_opt_variant_t long_opt_func_variant{
        long_options.find(long_opt_name)->second};
    ASSERT_THAT(
        std::holds_alternative<option_test_parser_t::long_opt_param_func_t>(
            long_opt_func_variant),
        testing::Eq(true));
    option_test_parser_t::long_opt_param_func_t long_opt_func{
        std::get<option_test_parser_t::long_opt_param_func_t>(
            long_opt_func_variant)};

    ASSERT_THAT(static_cast<bool>(opt), testing::Eq(false));
    ASSERT_THAT(opt.has_value(), testing::Eq(false));
    ASSERT_THAT(opt.given(), testing::Eq(false));
    ASSERT_THROW(opt.value(), clapp::exception::value_undefined_t);
    ASSERT_THROW(validate_funcs[0](), clapp::exception::option_exception_t);

    long_opt_func(long_opt_name, "true");
    ASSERT_THAT(static_cast<bool>(opt), testing::Eq(true));
    ASSERT_THAT(opt.has_value(), testing::Eq(true));
    ASSERT_THAT(opt.given(), testing::Eq(true));
    ASSERT_THAT(opt.value(), testing::Eq(true));
    ASSERT_NO_THROW(validate_funcs[0]());

    short_opt_func(short_opt_name, "false");
    ASSERT_THAT(static_cast<bool>(opt), testing::Eq(true));
    ASSERT_THAT(opt.has_value(), testing::Eq(true));
    ASSERT_THAT(opt.given(), testing::Eq(true));
    ASSERT_THAT(opt.value(), testing::Eq(false));
    ASSERT_NO_THROW(validate_funcs[0]());

    ASSERT_THROW(long_opt_func(long_opt_name, "some-string"),
                 clapp::exception::invalid_value_t);
}

TEST(option, intParamOptionConstructLongAndShortMandatory) {
    const std::string long_opt_name{"int"};
    const char short_opt_name{'i'};
    const std::string opt_desc{"int description"};
    const int default_value{4};
    const int min_value{25};
    const int max_value{55};

    option_test_parser_t tp;
    clapp::option::int64_param_option_t opt{
        tp,
        long_opt_name,
        short_opt_name,
        opt_desc,
        option_test_parser_t::purpose_t::mandatory,
        clapp::value::default_value_t<std::int64_t>{default_value},
        clapp::value::min_max_value_t<std::int64_t>{min_value, max_value}};
    option_test_parser_t::long_options_map_t long_options{
        tp.get_long_options()};
    option_test_parser_t::short_options_map_t short_options{
        tp.get_short_options()};
    ASSERT_THAT(tp.get_optional_option_descriptions().size(), testing::Eq(0));
    option_test_parser_t::option_descriptions_vec_t descs{
        tp.get_mandatory_option_descriptions()};
    option_test_parser_t::validate_func_vec_t validate_funcs{
        tp.get_validate_functions()};
    ASSERT_THAT(validate_funcs.size(), testing::Eq(1));

    ASSERT_THAT(descs.size(), testing::Eq(1));
    ASSERT_THAT(descs[0].option_string,
                testing::StrEq(std::string{"-"} + short_opt_name +
                               std::string{"|--"} + long_opt_name + "=<arg>"));
    ASSERT_THAT(
        descs[0].description,
        testing::StrEq(
            opt_desc + " (" +
            clapp::value::default_value_t<std::int64_t>{default_value}
                .append_restriction_text() +
            ", " +
            clapp::value::min_max_value_t<std::int64_t>{min_value, max_value}
                .append_restriction_text() +
            ")"));
    ASSERT_THAT(descs[0].option_type,
                testing::Eq(option_test_parser_t::option_type_t::scalar));

    ASSERT_THAT(short_options.size(), testing::Eq(1));
    ASSERT_THAT(short_options.count(short_opt_name), testing::Eq(1));
    ASSERT_THAT(long_options.size(), testing::Eq(1));
    ASSERT_THAT(long_options.count(long_opt_name), testing::Eq(1));

    option_test_parser_t::short_opt_variant_t short_opt_func_variant{
        short_options.find(short_opt_name)->second};
    ASSERT_THAT(
        std::holds_alternative<option_test_parser_t::short_opt_param_func_t>(
            short_opt_func_variant),
        testing::Eq(true));
    option_test_parser_t::short_opt_param_func_t short_opt_func{
        std::get<option_test_parser_t::short_opt_param_func_t>(
            short_opt_func_variant)};

    option_test_parser_t::long_opt_variant_t long_opt_func_variant{
        long_options.find(long_opt_name)->second};
    ASSERT_THAT(
        std::holds_alternative<option_test_parser_t::long_opt_param_func_t>(
            long_opt_func_variant),
        testing::Eq(true));
    option_test_parser_t::long_opt_param_func_t long_opt_func{
        std::get<option_test_parser_t::long_opt_param_func_t>(
            long_opt_func_variant)};

    ASSERT_THAT(static_cast<bool>(opt), testing::Eq(true));
    ASSERT_THAT(opt.has_value(), testing::Eq(true));
    ASSERT_THAT(opt.given(), testing::Eq(false));
    ASSERT_THAT(opt.value(), testing::Eq(default_value));
    ASSERT_THROW(validate_funcs[0](), clapp::exception::option_exception_t);

    long_opt_func(long_opt_name, "123");
    ASSERT_THAT(static_cast<bool>(opt), testing::Eq(true));
    ASSERT_THAT(opt.has_value(), testing::Eq(true));
    ASSERT_THAT(opt.given(), testing::Eq(true));
    ASSERT_THAT(opt.value(), testing::Eq(123));
    ASSERT_THROW(validate_funcs[0](), clapp::exception::out_of_range_t);

    short_opt_func(short_opt_name, "0x2f");
    ASSERT_THAT(static_cast<bool>(opt), testing::Eq(true));
    ASSERT_THAT(opt.has_value(), testing::Eq(true));
    ASSERT_THAT(opt.given(), testing::Eq(true));
    ASSERT_THAT(opt.value(), testing::Eq(0x2f));
    ASSERT_NO_THROW(validate_funcs[0]());
}

TEST(option, vectorStringParamOptionConstructWithDefaultThrows) {
    const char short_opt_name{'s'};
    const std::string opt_desc{"string description"};

    option_test_parser_t tp;
    ASSERT_THROW(
        (clapp::option::vector_string_param_option_t{
            tp, short_opt_name, opt_desc,
            clapp::value::default_value_t<std::string>{"default string"}}),
        clapp::exception::option_param_exception_t);
}

TEST(option, vectorStringParamOptionConstructShort) {
    const char short_opt_name{'s'};
    const std::string opt_desc{"string description"};

    option_test_parser_t tp;
    clapp::option::vector_string_param_option_t opt{tp, short_opt_name,
                                                    opt_desc};
    option_test_parser_t::long_options_map_t long_options{
        tp.get_long_options()};
    option_test_parser_t::short_options_map_t short_options{
        tp.get_short_options()};
    ASSERT_THAT(tp.get_mandatory_option_descriptions().size(), testing::Eq(0));
    option_test_parser_t::option_descriptions_vec_t descs{
        tp.get_optional_option_descriptions()};
    option_test_parser_t::validate_func_vec_t validate_funcs{
        tp.get_validate_functions()};
    ASSERT_THAT(validate_funcs.size(), testing::Eq(0));

    ASSERT_THAT(descs.size(), testing::Eq(1));
    ASSERT_THAT(descs[0].option_string,
                testing::StrEq(std::string{"-"} + short_opt_name + "=<arg>"));
    ASSERT_THAT(descs[0].description,
                testing::StrEq(opt_desc + " (vector option)"));
    ASSERT_THAT(descs[0].option_type,
                testing::Eq(option_test_parser_t::option_type_t::vector));

    ASSERT_THAT(long_options.size(), testing::Eq(0));
    ASSERT_THAT(short_options.size(), testing::Eq(1));
    ASSERT_THAT(short_options.count(short_opt_name), testing::Eq(1));
    option_test_parser_t::short_opt_variant_t short_opt_func_variant{
        short_options.find(short_opt_name)->second};
    ASSERT_THAT(
        std::holds_alternative<option_test_parser_t::short_opt_param_func_t>(
            short_opt_func_variant),
        testing::Eq(true));
    option_test_parser_t::short_opt_param_func_t short_opt_func{
        std::get<option_test_parser_t::short_opt_param_func_t>(
            short_opt_func_variant)};

    ASSERT_THAT(static_cast<bool>(opt), testing::Eq(false));
    ASSERT_THAT(opt.has_value(), testing::Eq(false));
    ASSERT_THAT(opt.given(), testing::Eq(false));
    ASSERT_THAT(opt.value(), testing::Eq((std::vector<std::string>{})));

    short_opt_func(short_opt_name, "string");
    ASSERT_THAT(static_cast<bool>(opt), testing::Eq(true));
    ASSERT_THAT(opt.has_value(), testing::Eq(true));
    ASSERT_THAT(opt.given(), testing::Eq(true));
    std::vector<std::string> vec{opt.value()};
    ASSERT_THAT(vec.size(), testing::Eq(1));
    ASSERT_THAT(vec[0], testing::StrEq("string"));

    short_opt_func(short_opt_name, "string2");
    ASSERT_THAT(static_cast<bool>(opt), testing::Eq(true));
    ASSERT_THAT(opt.has_value(), testing::Eq(true));
    ASSERT_THAT(opt.given(), testing::Eq(true));
    vec = opt.value();
    ASSERT_THAT(vec.size(), testing::Eq(2));
    ASSERT_THAT(vec[0], testing::StrEq("string"));
    ASSERT_THAT(vec[1], testing::StrEq("string2"));
}

TEST(option, vectorStringParamOptionConstructLongMandatory) {
    const std::string long_opt_name{"string"};
    const std::string opt_desc{"string description"};

    option_test_parser_t tp;
    clapp::option::vector_string_param_option_t opt{
        tp, long_opt_name, opt_desc,
        option_test_parser_t::purpose_t::mandatory};
    option_test_parser_t::long_options_map_t long_options{
        tp.get_long_options()};
    option_test_parser_t::short_options_map_t short_options{
        tp.get_short_options()};
    ASSERT_THAT(tp.get_optional_option_descriptions().size(), testing::Eq(0));
    option_test_parser_t::option_descriptions_vec_t descs{
        tp.get_mandatory_option_descriptions()};
    option_test_parser_t::validate_func_vec_t validate_funcs{
        tp.get_validate_functions()};
    ASSERT_THAT(validate_funcs.size(), testing::Eq(1));

    ASSERT_THAT(descs.size(), testing::Eq(1));
    ASSERT_THAT(descs[0].option_string,
                testing::StrEq(std::string{"--"} + long_opt_name + "=<arg>"));
    ASSERT_THAT(descs[0].description,
                testing::StrEq(opt_desc + " (vector option)"));
    ASSERT_THAT(descs[0].option_type,
                testing::Eq(option_test_parser_t::option_type_t::vector));

    ASSERT_THAT(short_options.size(), testing::Eq(0));
    ASSERT_THAT(long_options.size(), testing::Eq(1));
    ASSERT_THAT(long_options.count(long_opt_name), testing::Eq(1));
    option_test_parser_t::long_opt_variant_t long_opt_func_variant{
        long_options.find(long_opt_name)->second};
    ASSERT_THAT(
        std::holds_alternative<option_test_parser_t::long_opt_param_func_t>(
            long_opt_func_variant),
        testing::Eq(true));
    option_test_parser_t::long_opt_param_func_t long_opt_func{
        std::get<option_test_parser_t::long_opt_param_func_t>(
            long_opt_func_variant)};

    ASSERT_THAT(static_cast<bool>(opt), testing::Eq(false));
    ASSERT_THAT(opt.has_value(), testing::Eq(false));
    ASSERT_THAT(opt.given(), testing::Eq(false));
    ASSERT_THAT(opt.value(), testing::Eq((std::vector<std::string>{})));
    ASSERT_THROW(validate_funcs[0](),
                 clapp::exception::option_param_exception_t);

    long_opt_func(long_opt_name, "string");
    ASSERT_THAT(static_cast<bool>(opt), testing::Eq(true));
    ASSERT_THAT(opt.has_value(), testing::Eq(true));
    ASSERT_THAT(opt.given(), testing::Eq(true));
    std::vector<std::string> vec{opt.value()};
    ASSERT_THAT(vec.size(), testing::Eq(1));
    ASSERT_THAT(vec[0], testing::StrEq("string"));

    long_opt_func(long_opt_name, "string2");
    ASSERT_THAT(static_cast<bool>(opt), testing::Eq(true));
    ASSERT_THAT(opt.has_value(), testing::Eq(true));
    ASSERT_THAT(opt.given(), testing::Eq(true));
    vec = opt.value();
    ASSERT_THAT(vec.size(), testing::Eq(2));
    ASSERT_THAT(vec[0], testing::StrEq("string"));
    ASSERT_THAT(vec[1], testing::StrEq("string2"));

    long_opt_func(long_opt_name, "string3");
    ASSERT_THAT(static_cast<bool>(opt), testing::Eq(true));
    ASSERT_THAT(opt.has_value(), testing::Eq(true));
    ASSERT_THAT(opt.given(), testing::Eq(true));
    vec = opt.value();
    ASSERT_THAT(vec.size(), testing::Eq(3));
    ASSERT_THAT(vec[0], testing::StrEq("string"));
    ASSERT_THAT(vec[1], testing::StrEq("string2"));
    ASSERT_THAT(vec[2], testing::StrEq("string3"));
}

TEST(option, vectorStringParamOptionFoundFuncShort) {
    const std::string long_opt_name{"long"};
    const std::string opt_desc{"description"};

    std::stringstream ss;
    option_test_parser_t tp;
    clapp::option::vector_string_param_option_t opt{
        tp, long_opt_name, opt_desc, option_test_parser_t::purpose_t::mandatory,
        clapp::value::found_func_t{
            [&ss]() { ss << "this is another test for params"; }}};
    option_test_parser_t::short_options_map_t short_options{
        tp.get_short_options()};
    option_test_parser_t::long_options_map_t long_options{
        tp.get_long_options()};
    ASSERT_THAT(short_options.size(), testing::Eq(0));
    ASSERT_THAT(long_options.size(), testing::Eq(1));
    ASSERT_THAT(long_options.count(long_opt_name), testing::Eq(1));
    option_test_parser_t::long_opt_variant_t long_opt_func_variant{
        long_options.find(long_opt_name)->second};
    ASSERT_THAT(
        std::holds_alternative<option_test_parser_t::long_opt_param_func_t>(
            long_opt_func_variant),
        testing::Eq(true));
    option_test_parser_t::long_opt_param_func_t long_opt_func{
        std::get<option_test_parser_t::long_opt_param_func_t>(
            long_opt_func_variant)};

    ASSERT_THAT(static_cast<bool>(opt), testing::Eq(false));
    ASSERT_THAT(opt.has_value(), testing::Eq(false));
    ASSERT_THAT(opt.given(), testing::Eq(false));
    ASSERT_THAT(ss.str(), testing::StrEq(""));
    ASSERT_THAT(opt.value().size(), testing::Eq(0));
    long_opt_func(long_opt_name, "param");

    ASSERT_THAT(ss.str(), testing::StrEq("this is another test for params"));
    ASSERT_THAT(static_cast<bool>(opt), testing::Eq(true));
    ASSERT_THAT(opt.has_value(), testing::Eq(true));
    ASSERT_THAT(opt.given(), testing::Eq(true));
    ASSERT_THAT(opt.value().size(), testing::Eq(1));
    ASSERT_THAT(opt.value()[0], testing::StrEq("param"));
}

TEST(option, vectorStringParamOptionConstructLongAndShortMinMax) {
    const char short_opt_name{'s'};
    const std::string long_opt_name{"long"};
    const std::string opt_desc{"int description"};
    const int min_value{10};
    const int max_value{20};
    clapp::value::min_max_value_t<std::uint8_t> min_max{min_value, max_value};

    option_test_parser_t tp;
    clapp::option::vector_uint8_param_option_t opt{
        tp, long_opt_name, short_opt_name, opt_desc, min_max};
    option_test_parser_t::long_options_map_t long_options{
        tp.get_long_options()};
    option_test_parser_t::short_options_map_t short_options{
        tp.get_short_options()};
    ASSERT_THAT(tp.get_mandatory_option_descriptions().size(), testing::Eq(0));
    option_test_parser_t::option_descriptions_vec_t descs{
        tp.get_optional_option_descriptions()};
    option_test_parser_t::validate_func_vec_t validate_funcs{
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
                testing::Eq(option_test_parser_t::option_type_t::vector));

    ASSERT_THAT(long_options.size(), testing::Eq(1));
    ASSERT_THAT(long_options.count(long_opt_name), testing::Eq(1));
    option_test_parser_t::long_opt_variant_t long_opt_func_variant{
        long_options.find(long_opt_name)->second};
    ASSERT_THAT(
        std::holds_alternative<option_test_parser_t::long_opt_param_func_t>(
            long_opt_func_variant),
        testing::Eq(true));
    option_test_parser_t::long_opt_param_func_t long_opt_func{
        std::get<option_test_parser_t::long_opt_param_func_t>(
            long_opt_func_variant)};

    ASSERT_THAT(short_options.size(), testing::Eq(1));
    ASSERT_THAT(short_options.count(short_opt_name), testing::Eq(1));
    option_test_parser_t::short_opt_variant_t short_opt_func_variant{
        short_options.find(short_opt_name)->second};
    ASSERT_THAT(
        std::holds_alternative<option_test_parser_t::short_opt_param_func_t>(
            short_opt_func_variant),
        testing::Eq(true));
    option_test_parser_t::short_opt_param_func_t short_opt_func{
        std::get<option_test_parser_t::short_opt_param_func_t>(
            short_opt_func_variant)};

    ASSERT_THAT(static_cast<bool>(opt), testing::Eq(false));
    ASSERT_THAT(opt.has_value(), testing::Eq(false));
    ASSERT_THAT(opt.given(), testing::Eq(false));
    ASSERT_THAT(opt.value(), testing::Eq((std::vector<std::uint8_t>{})));
    ASSERT_NO_THROW(validate_funcs[0]());

    short_opt_func(short_opt_name, "0x12");
    ASSERT_THAT(static_cast<bool>(opt), testing::Eq(true));
    ASSERT_THAT(opt.has_value(), testing::Eq(true));
    ASSERT_THAT(opt.given(), testing::Eq(true));
    std::vector<std::uint8_t> vec{opt.value()};
    ASSERT_THAT(vec.size(), testing::Eq(1));
    ASSERT_THAT(vec[0], testing::Eq(0x12));
    ASSERT_NO_THROW(validate_funcs[0]());

    long_opt_func(long_opt_name, "12");
    ASSERT_THAT(static_cast<bool>(opt), testing::Eq(true));
    ASSERT_THAT(opt.has_value(), testing::Eq(true));
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
    ASSERT_THROW(validate_funcs[0](), clapp::exception::out_of_range_t);
}

TEST(option, invalidLongOptionConstruct) {
    const std::string long_opt_name1{"long\n"};
    const std::string long_opt_name2{"long\t"};
    const std::string long_opt_name3{"long\r"};
    const std::string long_opt_name4{"long "};
    const std::string long_opt_name5{"long ="};
    const std::string desc{"desc1"};
    const char short_opt_name{'a'};

    option_test_parser_t tp;
    ASSERT_THROW((clapp::option::bool_option_t{tp, long_opt_name1, desc}),
                 clapp::exception::option_exception_t);
    ASSERT_THROW((clapp::option::bool_option_t{tp, long_opt_name1,
                                               short_opt_name, desc}),
                 clapp::exception::option_exception_t);

    ASSERT_THROW((clapp::option::count_option_t{tp, long_opt_name2, desc}),
                 clapp::exception::option_exception_t);
    ASSERT_THROW((clapp::option::count_option_t{tp, long_opt_name2,
                                                short_opt_name, desc}),
                 clapp::exception::option_exception_t);

    ASSERT_THROW(
        (clapp::option::string_param_option_t{tp, long_opt_name3, desc}),
        clapp::exception::option_exception_t);
    ASSERT_THROW((clapp::option::string_param_option_t{tp, long_opt_name3,
                                                       short_opt_name, desc}),
                 clapp::exception::option_exception_t);

    ASSERT_THROW(
        (clapp::option::vector_string_param_option_t{tp, long_opt_name4, desc}),
        clapp::exception::option_exception_t);
    ASSERT_THROW((clapp::option::vector_string_param_option_t{
                     tp, long_opt_name4, short_opt_name, desc}),
                 clapp::exception::option_exception_t);

    ASSERT_THROW(
        (clapp::option::int32_param_option_t{tp, long_opt_name5, desc}),
        clapp::exception::option_exception_t);
    ASSERT_THROW((clapp::option::int32_param_option_t{tp, long_opt_name5,
                                                      short_opt_name, desc}),
                 clapp::exception::option_exception_t);
}

TEST(option, invalidShortOptionConstruct) {
    const char short_opt_name1{'\n'};
    const char short_opt_name2{'\t'};
    const char short_opt_name3{'\r'};
    const char short_opt_name4{' '};
    const char short_opt_name5{'='};
    const std::string desc{"desc1"};
    const std::string long_opt_name{'a'};

    option_test_parser_t tp;
    ASSERT_THROW((clapp::option::bool_option_t{tp, short_opt_name1, desc}),
                 clapp::exception::option_exception_t);
    ASSERT_THROW((clapp::option::bool_option_t{tp, long_opt_name,
                                               short_opt_name1, desc}),
                 clapp::exception::option_exception_t);

    ASSERT_THROW((clapp::option::count_option_t{tp, short_opt_name2, desc}),
                 clapp::exception::option_exception_t);
    ASSERT_THROW((clapp::option::count_option_t{tp, long_opt_name,
                                                short_opt_name2, desc}),
                 clapp::exception::option_exception_t);

    ASSERT_THROW(
        (clapp::option::string_param_option_t{tp, short_opt_name3, desc}),
        clapp::exception::option_exception_t);
    ASSERT_THROW((clapp::option::string_param_option_t{tp, long_opt_name,
                                                       short_opt_name3, desc}),
                 clapp::exception::option_exception_t);

    ASSERT_THROW((clapp::option::vector_string_param_option_t{
                     tp, short_opt_name4, desc}),
                 clapp::exception::option_exception_t);
    ASSERT_THROW((clapp::option::vector_string_param_option_t{
                     tp, long_opt_name, short_opt_name4, desc}),
                 clapp::exception::option_exception_t);

    ASSERT_THROW(
        (clapp::option::int32_param_option_t{tp, short_opt_name5, desc}),
        clapp::exception::option_exception_t);
    ASSERT_THROW((clapp::option::int32_param_option_t{tp, long_opt_name,
                                                      short_opt_name5, desc}),
                 clapp::exception::option_exception_t);
}

TEST(option, genOptValidateFunc) {
    using int32_value_func_t = std::function<std::int32_t(void)>;
    using int32_validate_func_t = std::function<void(
        const std::int32_t&, const std::string& option_string)>;

    {
        std::optional<option_test_parser_t::validate_func_t> validate_func{
            clapp::option::gen_opt_validate_func<std::int32_t,
                                                 int32_value_func_t>(
                std::nullopt, std::nullopt, []() { return false; },
                std::vector<int32_validate_func_t>{}, "option string",
                option_test_parser_t::purpose_t::mandatory)};
        ASSERT_THAT(validate_func.has_value(), testing::Eq(true));
        ASSERT_THROW((*validate_func)(), clapp::exception::option_exception_t);
    }

    {
        std::optional<option_test_parser_t::validate_func_t> validate_func{
            clapp::option::gen_opt_validate_func<std::int32_t,
                                                 int32_value_func_t>(
                std::nullopt,
                []() -> bool { throw std::runtime_error{"unexpected call"}; },
                []() { return true; }, std::vector<int32_validate_func_t>{},
                "option string", option_test_parser_t::purpose_t::mandatory)};
        ASSERT_THAT(validate_func.has_value(), testing::Eq(true));
        ASSERT_NO_THROW((*validate_func)());
    }

    {
        constexpr std::int32_t return_value{10};
        std::optional<option_test_parser_t::validate_func_t> validate_func{
            clapp::option::gen_opt_validate_func<std::int32_t,
                                                 int32_value_func_t>(
                []() { return return_value; }, []() { return true; },
                []() { return true; },
                std::vector<int32_validate_func_t>{
                    [](const std::int32_t& value,
                       const std::string& option_string) {
                        if (option_string != "option string") {
                            throw std::runtime_error{"option_string invalid"};
                        }
                        if (value != return_value) {
                            throw std::runtime_error{"value invalid"};
                        }
                    }},
                "option string", option_test_parser_t::purpose_t::mandatory)};
        ASSERT_THAT(validate_func.has_value(), testing::Eq(true));
        ASSERT_NO_THROW((*validate_func)());
    }

    {
        std::optional<option_test_parser_t::validate_func_t> validate_func{
            clapp::option::gen_opt_validate_func<std::int32_t,
                                                 int32_value_func_t>(
                std::nullopt, std::nullopt, []() { return true; },
                std::vector<int32_validate_func_t>{
                    [](const std::int32_t& /*value*/,
                       const std::string& option_string) {
                        if (option_string != "option string") {
                            throw std::runtime_error{"option_string invalid"};
                        }
                    }},
                "option string", option_test_parser_t::purpose_t::mandatory)};
        ASSERT_THAT(validate_func.has_value(), testing::Eq(true));
        ASSERT_THROW((*validate_func)(),
                     clapp::exception::option_param_exception_t);
    }
}
