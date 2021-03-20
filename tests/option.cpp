#include <clapp/option.h>
#include <gmock/gmock.h>
#include "helper.h"

class option_test_parser_t : public clapp::parser::basic_parser_t {
   public:
    ~option_test_parser_t() override;
    using clapp::parser::basic_parser_t::purpose_t;

    using clapp::parser::basic_parser_t::option_type_t;

    using clapp::parser::basic_parser_t::long_opt_func_t;
    using clapp::parser::basic_parser_t::long_opt_param_func_t;
    using clapp::parser::basic_parser_t::long_opt_variant_t;

    using clapp::parser::basic_parser_t::short_opt_func_t;
    using clapp::parser::basic_parser_t::short_opt_param_func_t;
    using clapp::parser::basic_parser_t::short_opt_variant_t;

    using clapp::parser::basic_parser_t::option_descriptions_vec_t;
    using clapp::parser::basic_parser_t::validate_func_vec_t;

    using clapp::parser::basic_parser_t::get_option_help;
    using clapp::parser::basic_parser_t::get_options;
    using clapp::parser::basic_parser_t::get_validate_functions;

    [[nodiscard]] std::string gen_short_line_prefix() const override;
};

template <typename OPTION_T>
static std::optional<option_test_parser_t::variant_opt_conf_t> contains_option(
    std::vector<option_test_parser_t::variant_opt_conf_t>& options,
    OPTION_T&& option) {
    for (const auto& opt : options) {
        const bool found{std::visit(
            [&option](auto&& o) {
                if constexpr (std::is_same<
                                  typename std::decay<decltype(option)>::type,
                                  char>::value) {
                    for (auto& item : o.short_options) {
                        if (option == item.option) {
                            return true;
                        }
                    }
                } else {
                    for (auto& item : o.long_options) {
                        if (option == item.option) {
                            return true;
                        }
                    }
                }
                return false;
            },
            opt)};
        if (found) {
            return opt;
        }
    }
    return std::nullopt;
}

template <typename OPTION_T, typename RESULT_LISTENER_T>
static std::optional<option_test_parser_t::variant_opt_conf_t> contains_option(
    std::vector<option_test_parser_t::variant_opt_conf_t>& options,
    OPTION_T&& option, RESULT_LISTENER_T&& result_listener) {
    if (options.empty()) {
        *result_listener
            << "Parser-options doesn't contain any options at all.";
        return std::nullopt;
    }
    std::optional<option_test_parser_t::variant_opt_conf_t> ret{
        contains_option(options, std::forward<OPTION_T>(option))};
    if (!ret) {
        *result_listener << "Parser-options doesn't contain option '" << option
                         << "'.";
        return std::nullopt;
    }
    return ret;
}

MATCHER(BoolOptionNotGiven, "Checks, if bool option not given.") {
    *result_listener << "bool-option: "
                     << (static_cast<bool>(arg) ? "true" : "false")
                     << ", has_value(): "
                     << (arg.has_value() ? "true" : "false")
                     << ", given(): " << (arg.given() ? "true" : "false")
                     << ", value(): " << (arg.value() ? "true" : "false");
    return static_cast<bool>(arg) && arg.has_value() && !arg.given() &&
           !arg.value();
}

MATCHER(BoolOptionGiven, "Checks, if bool option is given.") {
    *result_listener << "bool-option: "
                     << (static_cast<bool>(arg) ? "true" : "false")
                     << ", has_value(): "
                     << (arg.has_value() ? "true" : "false")
                     << ", given(): " << (arg.given() ? "true" : "false")
                     << ", value(): " << (arg.value() ? "true" : "false");
    return static_cast<bool>(arg) && arg.has_value() && arg.given() &&
           arg.value();
}

MATCHER_P(NumCountOptionGiven, count, "Checks, if num-count option is given.") {
    *result_listener << "count-option: "
                     << (static_cast<bool>(arg) ? "true" : "false")
                     << ", has_value(): "
                     << (arg.has_value() ? "true" : "false")
                     << ", given(): " << (arg.given() ? "true" : "false")
                     << ", value(): " << count << "==" << arg.value() << " = "
                     << (arg.value() == count ? "true" : "false");
    return static_cast<bool>(arg) && arg.has_value() && arg.given() &&
           arg.value() == count;
}

MATCHER_P(NumCountOptionNotGiven, count,
          "Checks, if num-count option is not given.") {
    *result_listener << "count-option: "
                     << (static_cast<bool>(arg) ? "true" : "false")
                     << ", has_value(): "
                     << (arg.has_value() ? "true" : "false")
                     << ", given(): " << (arg.given() ? "true" : "false")
                     << ", value(): " << count << "==" << arg.value() << " = "
                     << (arg.value() == count ? "true" : "false");
    return static_cast<bool>(arg) && arg.has_value() && !arg.given() &&
           arg.value() == count;
}

MATCHER_P(ParamOptionGiven, value, "Checks, if param option is given.") {
    *result_listener << "param-option: "
                     << (static_cast<bool>(arg) ? "true" : "false")
                     << ", has_value(): "
                     << (arg.has_value() ? "true" : "false")
                     << ", given(): " << (arg.given() ? "true" : "false");
    if (!(static_cast<bool>(arg) && arg.has_value() && arg.given())) {
        return false;
    }
    *result_listener << ", value(): " << clapp::to_string(value)
                     << "==" << clapp::to_string(arg.value()) << " = ";
    return compare_value(arg.value(), value, result_listener);
}

MATCHER_P(ParamOptionNotGivenDefaultValue, default_value,
          "Checks, if param option is not given and default value is set "
          "correctly.") {
    *result_listener << "param-option: "
                     << (static_cast<bool>(arg) ? "true" : "false")
                     << ", has_value(): "
                     << (arg.has_value() ? "true" : "false")
                     << ", given(): " << (arg.given() ? "true" : "false");
    if (!(static_cast<bool>(arg) && arg.has_value() && !arg.given())) {
        return false;
    }
    *result_listener << ", value(): " << clapp::to_string(default_value)
                     << "==" << clapp::to_string(arg.value()) << " = ";
    return compare_value(arg.value(), default_value, result_listener);
}

MATCHER(ParamOptionNotGiven, "Checks, if param option is not given.") {
    bool caught_value_undefined{false};
    try {
        arg.value();
    } catch (clapp::exception::value_undefined_t& /*e*/) {
        caught_value_undefined = true;
    }
    *result_listener << "param-option: "
                     << (static_cast<bool>(arg) ? "true" : "false")
                     << ", has_value(): "
                     << (arg.has_value() ? "true" : "false")
                     << ", given(): " << (arg.given() ? "true" : "false")
                     << ", value() throws value_undefined = "
                     << (caught_value_undefined ? "true" : "false");
    return !static_cast<bool>(arg) && !arg.has_value() && !arg.given() &&
           caught_value_undefined;
}

MATCHER_P(VectorParamOptionGiven, value,
          "Checks, if vector param option is given.") {
    *result_listener << "param-option: "
                     << (static_cast<bool>(arg) ? "true" : "false")
                     << ", has_value(): "
                     << (arg.has_value() ? "true" : "false")
                     << ", given(): " << (arg.given() ? "true" : "false");
    if (!(static_cast<bool>(arg) && arg.has_value() && arg.given())) {
        return false;
    }
    const auto& a{arg.value()};
    std::string val_str{
        std::accumulate(value.begin(), value.end(), std::string{},
                        [](std::string& s, const auto& itm) {
                            return s += clapp::to_string(itm) + ", ";
                        })};
    if (!val_str.empty()) {
        val_str = val_str.substr(0, val_str.size() - 2);
    }
    std::string arg_str{std::accumulate(
        a.begin(), a.end(), std::string{}, [](std::string& s, const auto& itm) {
            return s += clapp::to_string(itm) + ", ";
        })};
    if (!arg_str.empty()) {
        arg_str = arg_str.substr(0, arg_str.size() - 2);
    }
    *result_listener << ", value(): {" << val_str << "}=={" << arg_str
                     << "} = ";
    return compare_value_vector(a, value, result_listener);
}

MATCHER(VectorParamOptionNotGiven,
        "Checks, if vector param option is not given.") {
    *result_listener << "vector-param-option: "
                     << (static_cast<bool>(arg) ? "true" : "false")
                     << ", has_value(): "
                     << (arg.has_value() ? "true" : "false")
                     << ", given(): " << (arg.given() ? "true" : "false");
    if (!(!static_cast<bool>(arg) && !arg.has_value() && !arg.given())) {
        return false;
    }
    *result_listener << ", size(): 0==" << clapp::to_string(arg.value().size())
                     << " = " << (arg.value().empty() ? "true" : "false");
    return arg.value().empty();
}

MATCHER_P(ContainsLongOption, option, "Checks, if long option is given.") {
    std::vector<option_test_parser_t::variant_opt_conf_t> options{
        arg.get_options()};
    std::optional<option_test_parser_t::variant_opt_conf_t> found_opt{
        contains_option(options, option, result_listener)};
    if (found_opt == std::nullopt) {
        return false;
    }
    *result_listener << "Found parser-option doesn't contain option '" << option
                     << "'";
    return std::visit(
        [this](auto&& opt) { return opt.contains_option(option); },
        found_opt.value());
}

MATCHER_P(ContainsShortOption, option, "Checks, if short option is given.") {
    std::vector<option_test_parser_t::variant_opt_conf_t> options{
        arg.get_options()};
    std::optional<option_test_parser_t::variant_opt_conf_t> found_opt{
        contains_option(options, option, result_listener)};
    if (found_opt == std::nullopt) {
        return false;
    }
    *result_listener << "Found parser-option doesn't contain option '" << option
                     << "'";
    return std::visit(
        [this](auto&& opt) { return opt.contains_option(option); },
        found_opt.value());
}

template <typename LONG_OPT_FUNC_T, typename OPT_T>
LONG_OPT_FUNC_T process_long_option(OPT_T&& option,
                                    const std::string& long_opt_name) {
    for (auto& opt : option.long_options) {
        if (opt.option == long_opt_name) {
            if constexpr (std::is_same<LONG_OPT_FUNC_T,
                                       decltype(opt.func)>::value) {
                return opt.func;
            } else {
                throw std::runtime_error("unexpected long_options-func-type");
            }
        }
    }
    throw std::runtime_error("found long-opt-func, but iterate failed... wtf");
}

template <typename LONG_OPT_FUNC_T>
static LONG_OPT_FUNC_T get_long_opt_func(option_test_parser_t& tp,
                                         const std::string& long_opt_name) {
    std::vector<option_test_parser_t::variant_opt_conf_t> options{
        tp.get_options()};
    std::optional<option_test_parser_t::variant_opt_conf_t> found_opt{
        contains_option(options, long_opt_name)};
    if (found_opt == std::nullopt) {
        throw std::runtime_error("no option with '" + long_opt_name +
                                 "' registered.");
    }
    return std::visit(
        [&long_opt_name](auto&& option) {
            return process_long_option<LONG_OPT_FUNC_T>(
                std::forward<decltype(option)>(option), long_opt_name);
        },
        found_opt.value());
}

template <typename SHORT_OPT_FUNC_T, typename OPT_T>
SHORT_OPT_FUNC_T process_short_option(OPT_T&& option,
                                      const char short_opt_name) {
    for (auto& opt : option.short_options) {
        if (opt.option == short_opt_name) {
            if constexpr (std::is_same<SHORT_OPT_FUNC_T,
                                       decltype(opt.func)>::value) {
                return opt.func;
            } else {
                throw std::runtime_error("unexpected short_options-func-type");
            }
        }
    }
    throw std::runtime_error("found long-opt-func, but iterate failed... wtf");
}

template <typename SHORT_OPT_FUNC_T>
static SHORT_OPT_FUNC_T get_short_opt_func(option_test_parser_t& tp,
                                           const char short_opt_name) {
    std::vector<option_test_parser_t::variant_opt_conf_t> options{
        tp.get_options()};
    std::optional<option_test_parser_t::variant_opt_conf_t> found_opt{
        contains_option(options, short_opt_name)};
    if (found_opt == std::nullopt) {
        throw std::runtime_error("no option with '" +
                                 std::string{short_opt_name} + "' registered.");
    }
    return std::visit(
        [&short_opt_name](auto&& option) {
            return process_short_option<SHORT_OPT_FUNC_T>(
                std::forward<decltype(option)>(option), short_opt_name);
        },
        found_opt.value());
}

template <typename OPTION_T>
static std::optional<option_test_parser_t::validate_func_t> get_validate_func(
    option_test_parser_t& tp, OPTION_T&& option) {
    std::string option_string{option};
    std::vector<option_test_parser_t::variant_opt_conf_t> options{
        tp.get_options()};
    std::optional<option_test_parser_t::variant_opt_conf_t> found_opt{
        contains_option(options, std::forward<decltype(option)>(option))};
    if (found_opt == std::nullopt) {
        throw std::runtime_error("no option with '" + option_string +
                                 "' registered.");
    }

    return std::visit([](auto&& vf) { return vf.validate_func; },
                      found_opt.value());
}

option_test_parser_t::~option_test_parser_t() = default;

std::string option_test_parser_t::gen_short_line_prefix() const {
    return "opt-test-parser" + gen_short_line();
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

class optionT : public ::testing::Test {
   protected:
    void SetUp() override {}
    void TearDown() override {}

    static bool throw_unexpected_call();

    using int32_value_func_t = std::function<std::int32_t(void)>;
    using int32_validate_func_t = std::function<void(
        const std::int32_t&, const std::string& option_string)>;

    option_test_parser_t tp;
    std::size_t found_func_called{0};

    inline static const char* param_opt_postfix{"=<param>"};
    inline static const char* vector_opt_desc_restriction{"vector option"};
    inline static const char short_opt{'s'};
    inline static const std::string long_opt_str{"long-str"};
    inline static constexpr const char* long_opt_cstr{"long-cstr"};
    inline static const std::string opt_desc_str{"opt-desc-str"};
    inline static constexpr const char* opt_desc_cstr{"opt-desc-cstr"};
    inline static constexpr const char* value_cstr{"value-cstr"};
    inline static const std::string value_str{"value-str"};
    inline static constexpr const char* value_cstr_path{"/value/cstr/path"};
    inline static const std::string value_str_path{"/value/str/path"};
    inline static const std::string purpose_optional_str{"optional"};
    inline static const std::string purpose_mandatory_str{"mandatory"};
    inline static constexpr std::int64_t value_int64{0x12345678abcdef0LL};
    inline static constexpr std::int32_t value_int32{0x12345678};
    inline static constexpr std::int16_t value_int16{0x1234};
    inline static constexpr std::int8_t value_int8{0x12};
    inline static constexpr std::uint64_t value_uint64{0xc2345678abcdef0LL};
    inline static constexpr std::uint32_t value_uint32{0xc2345678};
    inline static constexpr std::uint16_t value_uint16{0xc234};
    inline static constexpr std::uint8_t value_uint8{0xc2};
    inline static constexpr bool value_bool{true};
    inline static constexpr std::size_t value_size{0xcf};
    inline static constexpr std::ptrdiff_t value_ptrdiff{0x1f};
    inline static const double value_double{1415.1716};
    inline static const float value_float{1415.1617F};
    inline static constexpr std::chrono::nanoseconds value_ns{1213};
    inline static constexpr std::chrono::microseconds value_us{121};
    inline static constexpr std::chrono::milliseconds value_ms{12};
    inline static constexpr std::chrono::seconds value_sec{3603};
    inline static constexpr std::chrono::minutes value_min{62};
    inline static constexpr std::chrono::hours value_hours{1};
};

bool optionT::throw_unexpected_call() {
    throw std::runtime_error{"unexpected call"};
}

TEST_F(optionT, basicOptionConstructLongAndCallValueThrows) {
    test_option_t opt{tp, long_opt_str, opt_desc_str};
    ASSERT_THROW(opt.value(), clapp::exception::value_undefined_t);
}

TEST_F(optionT, basicOptionConstructShortAndCallValueThrows) {
    test_option_t opt{tp, short_opt, opt_desc_str};
    ASSERT_THROW(opt.value(), clapp::exception::value_undefined_t);
}

TEST_F(optionT, boolOptionConstructShort) {
    clapp::option::bool_option_t opt{tp, short_opt, opt_desc_str};
    ASSERT_THAT(tp, ContainsShortOption(short_opt));
    ASSERT_THAT(opt, BoolOptionNotGiven());
}

TEST_F(optionT, boolOptionConstructLongString) {
    clapp::option::bool_option_t opt{tp, long_opt_str, opt_desc_str};
    ASSERT_THAT(tp, ContainsLongOption(long_opt_str));
    ASSERT_THAT(opt, BoolOptionNotGiven());
}

TEST_F(optionT, boolOptionConstructLongCString) {
    clapp::option::bool_option_t opt{tp, long_opt_cstr, opt_desc_cstr};
    ASSERT_THAT(tp, ContainsLongOption(long_opt_cstr));
    ASSERT_THAT(opt, BoolOptionNotGiven());
}

TEST_F(optionT, boolOptionConstructShortVec) {
    clapp::option::bool_option_t opt{tp, std::vector<char>{short_opt},
                                     opt_desc_str};
    ASSERT_THAT(tp, ContainsShortOption(short_opt));
    ASSERT_THAT(opt, BoolOptionNotGiven());
}

TEST_F(optionT, boolOptionConstructLongStringVec) {
    clapp::option::bool_option_t opt{
        tp, std::vector<std::string>{long_opt_str, long_opt_cstr},
        opt_desc_str};
    ASSERT_THAT(tp, ContainsLongOption(long_opt_str));
    ASSERT_THAT(tp, ContainsLongOption(long_opt_cstr));
    ASSERT_THAT(opt, BoolOptionNotGiven());
}

TEST_F(optionT, boolOptionConstructLongStringVecTwiceThrows) {
    clapp::option::bool_option_t opt{tp, long_opt_str, opt_desc_str};
    ASSERT_THROW((clapp::option::bool_option_t{tp, long_opt_str, opt_desc_str}),
                 clapp::exception::option_exception_t);
}

TEST_F(optionT, boolOptionConstructLongStringVecWithSameOptionThrows) {
    ASSERT_THROW((clapp::option::bool_option_t{
                     tp, std::vector<std::string>{long_opt_str, long_opt_str},
                     opt_desc_str}),
                 clapp::exception::option_exception_t);
}

TEST_F(optionT, boolOptionConstructLongStringVecAndCallGetOptionHelp) {
    clapp::option::bool_option_t opt{
        tp, std::vector<std::string>{long_opt_str, long_opt_cstr},
        opt_desc_str};
    ASSERT_THAT(tp.get_option_help(),
                testing::ContainerEq(option_test_parser_t::help_entry_vec_t{
                    {"--" + long_opt_str + "|--" + long_opt_cstr,
                     opt_desc_str + " (" + purpose_optional_str + ")"}}));
}

TEST_F(optionT, boolOptionConstructShortCharVecWithSameOptionThrows) {
    ASSERT_THROW(
        (clapp::option::bool_option_t{
            tp, std::vector<char>{short_opt, short_opt}, opt_desc_str}),
        clapp::exception::option_exception_t);
}

TEST_F(optionT, boolOptionConstructShortCharVecTwiceThrows) {
    clapp::option::bool_option_t opt{tp, std::vector<char>{short_opt},
                                     opt_desc_str};
    ASSERT_THROW((clapp::option::bool_option_t{tp, std::vector<char>{short_opt},
                                               opt_desc_str}),
                 clapp::exception::option_exception_t);
}

TEST_F(optionT, boolOptionConstructShortVecAndCallGetOptionHelp) {
    clapp::option::bool_option_t opt{
        tp, std::vector<char>{short_opt}, opt_desc_str,
        clapp::basic_parser_t::purpose_t::mandatory};
    ASSERT_THAT(tp.get_option_help(),
                testing::ContainerEq(option_test_parser_t::help_entry_vec_t{
                    {std::string{"-"} + short_opt,
                     opt_desc_str + " (" + purpose_mandatory_str + ")"}}));
}

TEST_F(optionT,
       boolOptionConstructLongStringVecAndShortVecAndCallGetOptionHelp) {
    clapp::option::bool_option_t opt{
        tp, std::vector<std::string>{long_opt_str, long_opt_cstr},
        std::vector<char>{short_opt}, opt_desc_str};
    ASSERT_THAT(tp.get_option_help(),
                testing::ContainerEq(option_test_parser_t::help_entry_vec_t{
                    {std::string{"-"} + short_opt + "|--" + long_opt_str +
                         "|--" + long_opt_cstr,
                     opt_desc_str + " (" + purpose_optional_str + ")"}}));
}

TEST_F(optionT, boolOptionConstructLongStringAndShortAndCallGetOptionHelp) {
    clapp::option::bool_option_t opt{
        tp, long_opt_str, short_opt, opt_desc_str,
        clapp::basic_parser_t::purpose_t::mandatory};
    ASSERT_THAT(tp.get_option_help(),
                testing::ContainerEq(option_test_parser_t::help_entry_vec_t{
                    {std::string{"-"} + short_opt + "|--" + long_opt_str,
                     opt_desc_str + " (" + purpose_mandatory_str + ")"}}));
}

TEST_F(optionT, boolOptionConstructLongCStringAndShortAndCallGetOptionHelp) {
    clapp::option::bool_option_t opt{tp, long_opt_cstr, short_opt,
                                     opt_desc_str};
    ASSERT_THAT(tp.get_option_help(),
                testing::ContainerEq(option_test_parser_t::help_entry_vec_t{
                    {std::string{"-"} + short_opt + "|--" + long_opt_cstr,
                     opt_desc_str + " (" + purpose_optional_str + ")"}}));
}

TEST_F(optionT, boolOptionConstructLongStringVecAndCallLongOptFunc) {
    clapp::option::bool_option_t opt{tp, std::vector<std::string>{long_opt_str},
                                     opt_desc_str};
    ASSERT_THAT(opt, BoolOptionNotGiven());

    get_long_opt_func<option_test_parser_t::long_opt_func_t>(
        tp, long_opt_str)(long_opt_str);
    ASSERT_THAT(opt, BoolOptionGiven());
}

TEST_F(optionT, boolOptionConstructShortVecAndCallShortOptFunc) {
    clapp::option::bool_option_t opt{tp, std::vector<char>{short_opt},
                                     opt_desc_str};
    ASSERT_THAT(opt, BoolOptionNotGiven());
    get_short_opt_func<option_test_parser_t::short_opt_func_t>(
        tp, short_opt)(short_opt);
    ASSERT_THAT(opt, BoolOptionGiven());
}

TEST_F(optionT, boolOptionConstructLongStringAndShortAndCallLongOptFunc) {
    clapp::option::bool_option_t opt{tp, long_opt_str, short_opt, opt_desc_str};
    ASSERT_THAT(opt, BoolOptionNotGiven());
    get_long_opt_func<option_test_parser_t::long_opt_func_t>(
        tp, long_opt_str)(long_opt_str);
    ASSERT_THAT(opt, BoolOptionGiven());
}

TEST_F(optionT, boolOptionConstructLongStringAndShortAndCallShortOptFunc) {
    clapp::option::bool_option_t opt{tp, long_opt_str, short_opt, opt_desc_str};
    ASSERT_THAT(opt, BoolOptionNotGiven());
    get_short_opt_func<option_test_parser_t::short_opt_func_t>(
        tp, short_opt)(short_opt);
    ASSERT_THAT(opt, BoolOptionGiven());
}

TEST_F(optionT, twoBoolOptionsConstructAndCallShortOptAndLongOptFunc) {
    clapp::option::bool_option_t opt1{tp, long_opt_str, short_opt,
                                      opt_desc_str};
    clapp::option::bool_option_t opt2{
        tp, std::vector<const char*>{long_opt_cstr}, opt_desc_str};
    ASSERT_THAT(opt1, BoolOptionNotGiven());
    get_short_opt_func<option_test_parser_t::short_opt_func_t>(
        tp, short_opt)(short_opt);
    ASSERT_THAT(opt1, BoolOptionGiven());
    ASSERT_THAT(opt2, BoolOptionNotGiven());
    get_long_opt_func<option_test_parser_t::long_opt_func_t>(
        tp, long_opt_cstr)(long_opt_cstr);
    ASSERT_THAT(opt2, BoolOptionGiven());
}

TEST_F(optionT, boolOptionConstructLongStringCallLongOptFuncCallsFoundFunc) {
    clapp::option::bool_option_t opt{
        tp, long_opt_str, opt_desc_str,
        clapp::value::found_func_t{[this]() { found_func_called++; }}};
    get_long_opt_func<option_test_parser_t::long_opt_func_t>(
        tp, long_opt_str)(long_opt_str);

    constexpr std::uint32_t expected_found_func_called{1U};
    ASSERT_THAT(found_func_called, testing::Eq(expected_found_func_called));
}

TEST_F(optionT, boolOptionConstructShortOptionalNoValidateFunc) {
    clapp::option::bool_option_t opt{tp, short_opt, opt_desc_str,
                                     option_test_parser_t::purpose_t::optional};
    ASSERT_THAT(get_validate_func(tp, short_opt), testing::Eq(std::nullopt));
}

TEST_F(optionT, boolOptionConstructLongOptionalNoValidateFunc) {
    clapp::option::bool_option_t opt{tp, long_opt_str, opt_desc_str,
                                     option_test_parser_t::purpose_t::optional};
    ASSERT_THAT(get_validate_func(tp, long_opt_str), testing::Eq(std::nullopt));
}

TEST_F(optionT, boolOptionConstructMandatoryLongCallValidateFunc) {
    clapp::option::bool_option_t opt{
        tp, long_opt_cstr, opt_desc_str,
        option_test_parser_t::purpose_t::mandatory};
    std::optional<option_test_parser_t::validate_func_t> option_validate_func{
        get_validate_func(tp, long_opt_cstr)};
    ASSERT_THAT(option_validate_func, testing::Ne(std::nullopt));
    ASSERT_THROW(option_validate_func.value()(),
                 clapp::exception::option_param_exception_t);
    get_long_opt_func<option_test_parser_t::long_opt_func_t>(
        tp, long_opt_cstr)(long_opt_cstr);
    ASSERT_NO_THROW(option_validate_func.value()());
}

TEST_F(optionT,
       helpOptionConstructLongStringVecAndShortVecAndCallGetOptionHelp) {
    clapp::option::help_option_t opt{
        tp, std::vector<std::string>{long_opt_str, long_opt_cstr},
        std::vector<char>{short_opt}, opt_desc_str};
    ASSERT_THAT(tp.get_option_help(),
                testing::ContainerEq(option_test_parser_t::help_entry_vec_t{
                    {std::string{"-"} + short_opt + "|--" + long_opt_str +
                         "|--" + long_opt_cstr,
                     opt_desc_str + " (" + purpose_optional_str + ")"}}));
}

TEST_F(optionT, helpOptionConstructLongStringAndShortAndCallGetOptionHelp) {
    clapp::option::help_option_t opt{
        tp, long_opt_str, short_opt, opt_desc_str,
        clapp::basic_parser_t::purpose_t::mandatory};
    ASSERT_THAT(tp.get_option_help(),
                testing::ContainerEq(option_test_parser_t::help_entry_vec_t{
                    {std::string{"-"} + short_opt + "|--" + long_opt_str,
                     opt_desc_str + " (" + purpose_mandatory_str + ")"}}));
}

TEST_F(optionT, helpOptionConstructLongCStringAndShortAndCallGetOptionHelp) {
    clapp::option::help_option_t opt{tp, long_opt_cstr, short_opt,
                                     opt_desc_str};
    ASSERT_THAT(tp.get_option_help(),
                testing::ContainerEq(option_test_parser_t::help_entry_vec_t{
                    {std::string{"-"} + short_opt + "|--" + long_opt_cstr,
                     opt_desc_str + " (" + purpose_optional_str + ")"}}));
}

TEST_F(optionT, helpOptionConstructLongStringCallLongOptFunc) {
    clapp::option::help_option_t opt{tp, long_opt_str, opt_desc_str};
    testing::internal::CaptureStdout();
    ASSERT_EXIT((get_long_opt_func<option_test_parser_t::long_opt_func_t>(
                    tp, long_opt_str)(long_opt_str)),
                ::testing::ExitedWithCode(0), "");
    ASSERT_THAT(testing::internal::GetCapturedStdout(),
                testing::HasSubstr("Usage:\n"));
}

TEST_F(optionT, helpOptionConstructLongStringCallShortOptFunc) {
    clapp::option::help_option_t opt{tp, short_opt, opt_desc_str};
    testing::internal::CaptureStdout();
    ASSERT_EXIT((get_short_opt_func<option_test_parser_t::short_opt_func_t>(
                    tp, short_opt)(short_opt)),
                ::testing::ExitedWithCode(0), "");
    ASSERT_THAT(testing::internal::GetCapturedStdout(),
                testing::HasSubstr("Usage:\n"));
}

TEST_F(optionT, helpOptionConstructAndCallShortOptAndLongOptFunc) {
    clapp::option::help_option_t opt{
        tp, std::vector<std::string>{long_opt_cstr}, short_opt, opt_desc_str};
    testing::internal::CaptureStdout();
    ASSERT_EXIT((get_short_opt_func<option_test_parser_t::short_opt_func_t>(
                    tp, short_opt)(short_opt)),
                ::testing::ExitedWithCode(0), "");
    ASSERT_THAT(testing::internal::GetCapturedStdout(),
                testing::HasSubstr("Usage:\n"));
    testing::internal::CaptureStdout();
    ASSERT_EXIT((get_long_opt_func<option_test_parser_t::long_opt_func_t>(
                    tp, long_opt_cstr)(long_opt_cstr)),
                ::testing::ExitedWithCode(0), "");
    ASSERT_THAT(testing::internal::GetCapturedStdout(),
                testing::HasSubstr("Usage:\n"));
}

TEST_F(optionT,
       countOptionConstructLongStringVecAndShortVecAndCallGetOptionHelp) {
    clapp::option::count_option_t opt{
        tp, std::vector<std::string>{long_opt_str, long_opt_cstr},
        std::vector<char>{short_opt}, opt_desc_str};
    ASSERT_THAT(tp.get_option_help(),
                testing::ContainerEq(option_test_parser_t::help_entry_vec_t{
                    {std::string{"-"} + short_opt + "|--" + long_opt_str +
                         "|--" + long_opt_cstr,
                     opt_desc_str + " (" + purpose_optional_str + ")"}}));
}

TEST_F(optionT, countOptionConstructLongStringAndShortAndCallGetOptionHelp) {
    clapp::option::count_option_t opt{
        tp, long_opt_str, short_opt, opt_desc_str,
        clapp::basic_parser_t::purpose_t::mandatory};
    ASSERT_THAT(tp.get_option_help(),
                testing::ContainerEq(option_test_parser_t::help_entry_vec_t{
                    {std::string{"-"} + short_opt + "|--" + long_opt_str,
                     opt_desc_str + " (" + purpose_mandatory_str + ")"}}));
}

TEST_F(optionT, countOptionConstructLongCStringAndShortAndCallGetOptionHelp) {
    clapp::option::count_option_t opt{tp, long_opt_cstr, short_opt,
                                      opt_desc_str};
    ASSERT_THAT(tp.get_option_help(),
                testing::ContainerEq(option_test_parser_t::help_entry_vec_t{
                    {std::string{"-"} + short_opt + "|--" + long_opt_cstr,
                     opt_desc_str + " (" + purpose_optional_str + ")"}}));
}

TEST_F(optionT, countOptionConstructLongStringCallLongOptFuncTwice) {
    clapp::option::count_option_t opt{tp, long_opt_str, opt_desc_str};
    get_long_opt_func<option_test_parser_t::long_opt_func_t>(
        tp, long_opt_str)(long_opt_str);
    get_long_opt_func<option_test_parser_t::long_opt_func_t>(
        tp, long_opt_str)(long_opt_str);

    constexpr std::uint32_t num_count_options{2U};
    ASSERT_THAT(opt, NumCountOptionGiven(num_count_options));
}

TEST_F(optionT, countOptionConstructLongStringCallShortOptFuncOnce) {
    clapp::option::count_option_t opt{tp, short_opt, opt_desc_str};
    get_short_opt_func<option_test_parser_t::short_opt_func_t>(
        tp, short_opt)(short_opt);

    constexpr std::uint32_t num_count_options{1U};
    ASSERT_THAT(opt, NumCountOptionGiven(num_count_options));
}

TEST_F(
    optionT,
    countOptionConstructLongStringVecAndShortAndCallLongOptFuncAndShortOptFunc) {
    clapp::option::count_option_t opt{
        tp, std::vector<std::string>{long_opt_str}, short_opt, opt_desc_str};
    get_long_opt_func<option_test_parser_t::long_opt_func_t>(
        tp, long_opt_str)(long_opt_str);
    get_short_opt_func<option_test_parser_t::short_opt_func_t>(
        tp, short_opt)(short_opt);

    constexpr std::uint32_t num_count_options{2U};
    ASSERT_THAT(opt, NumCountOptionGiven(num_count_options));
}

TEST_F(
    optionT,
    countOptionConstructLongStringVecAndShortWithMinMaxConstraintAndCallLongOptFuncAndShortOptFunc) {
    constexpr std::uint32_t min_value{0U};
    constexpr std::uint32_t max_value{5U};
    clapp::option::count_option_t opt{
        tp, std::vector<std::string>{long_opt_str}, short_opt, opt_desc_str,
        clapp::value::min_max_value_t<std::uint32_t>{min_value, max_value}};

    std::optional<option_test_parser_t::validate_func_t> option_validate_func{
        get_validate_func(tp, long_opt_str)};
    ASSERT_THAT(option_validate_func, testing::Ne(std::nullopt));

    get_long_opt_func<option_test_parser_t::long_opt_func_t>(
        tp, long_opt_str)(long_opt_str);
    get_long_opt_func<option_test_parser_t::long_opt_func_t>(
        tp, long_opt_str)(long_opt_str);
    get_long_opt_func<option_test_parser_t::long_opt_func_t>(
        tp, long_opt_str)(long_opt_str);
    get_short_opt_func<option_test_parser_t::short_opt_func_t>(
        tp, short_opt)(short_opt);

    constexpr std::uint32_t num_count_options_first{4U};
    ASSERT_THAT(opt, NumCountOptionGiven(num_count_options_first));
    ASSERT_NO_THROW((option_validate_func.value()()));
    get_short_opt_func<option_test_parser_t::short_opt_func_t>(
        tp, short_opt)(short_opt);

    constexpr std::uint32_t num_count_options_second{5U};
    ASSERT_THAT(opt, NumCountOptionGiven(num_count_options_second));
    ASSERT_NO_THROW((option_validate_func.value()()));
    ASSERT_NO_THROW((get_long_opt_func<option_test_parser_t::long_opt_func_t>(
        tp, long_opt_str)(long_opt_str)));

    constexpr std::uint32_t num_count_options_third{6U};
    ASSERT_THAT(opt, NumCountOptionGiven(num_count_options_third));
    ASSERT_THROW((option_validate_func.value()()),
                 clapp::exception::out_of_range_t);
}

TEST_F(
    optionT,
    countOptionConstructLongStringAndShortWithDefaultValueAndCallLongOptFunc) {
    constexpr std::uint32_t default_value{5U};
    clapp::option::count_option_t opt{
        tp, long_opt_cstr, short_opt, opt_desc_str,
        clapp::value::default_value_t<std::uint32_t>{default_value}};
    ASSERT_THAT(opt, NumCountOptionNotGiven(default_value));
    ASSERT_NO_THROW((get_long_opt_func<option_test_parser_t::long_opt_func_t>(
        tp, long_opt_cstr)(long_opt_cstr)));
    constexpr std::uint32_t num_count_options{6U};
    ASSERT_THAT(opt, NumCountOptionGiven(num_count_options));
}

TEST_F(optionT, countOptionConstructLongStringCallLongOptFuncCallsFoundFunc) {
    clapp::option::count_option_t opt{
        tp, long_opt_str, opt_desc_str,
        clapp::value::found_func_t{[this]() { found_func_called++; }}};
    get_long_opt_func<option_test_parser_t::long_opt_func_t>(
        tp, long_opt_str)(long_opt_str);

    constexpr std::uint32_t expected_found_func_called{1U};
    ASSERT_THAT(found_func_called, testing::Eq(expected_found_func_called));
}

TEST_F(optionT, countOptionConstructMandatoryLongCallValidateFunc) {
    clapp::option::count_option_t opt{
        tp, long_opt_cstr, opt_desc_str,
        option_test_parser_t::purpose_t::mandatory};
    std::optional<option_test_parser_t::validate_func_t> option_validate_func{
        get_validate_func(tp, long_opt_cstr)};
    ASSERT_THAT(option_validate_func, testing::Ne(std::nullopt));
    ASSERT_THROW(option_validate_func.value()(),
                 clapp::exception::option_param_exception_t);
    get_long_opt_func<option_test_parser_t::long_opt_func_t>(
        tp, long_opt_cstr)(long_opt_cstr);
    ASSERT_NO_THROW(option_validate_func.value()());
}

TEST_F(optionT, stringParamOptionConstructShort) {
    clapp::option::string_param_option_t opt{tp, short_opt, opt_desc_str};
    ASSERT_THAT(tp, ContainsShortOption(short_opt));
    ASSERT_THAT(opt, ParamOptionNotGiven());
}

TEST_F(optionT, stringParamOptionConstructLongString) {
    clapp::option::string_param_option_t opt{tp, long_opt_str, opt_desc_str};
    ASSERT_THAT(tp, ContainsLongOption(long_opt_str));
    ASSERT_THAT(opt, ParamOptionNotGiven());
}

TEST_F(optionT, stringParamOptionConstructLongCString) {
    clapp::option::string_param_option_t opt{tp, long_opt_cstr, opt_desc_cstr};
    ASSERT_THAT(tp, ContainsLongOption(long_opt_cstr));
    ASSERT_THAT(opt, ParamOptionNotGiven());
}

TEST_F(optionT, stringParamOptionConstructShortVec) {
    clapp::option::string_param_option_t opt{tp, std::vector<char>{short_opt},
                                             opt_desc_str};
    ASSERT_THAT(tp, ContainsShortOption(short_opt));
    ASSERT_THAT(opt, ParamOptionNotGiven());
}

TEST_F(optionT, stringParamOptionConstructLongStringVec) {
    clapp::option::string_param_option_t opt{
        tp, std::vector<std::string>{long_opt_str, long_opt_cstr},
        opt_desc_str};
    ASSERT_THAT(tp, ContainsLongOption(long_opt_str));
    ASSERT_THAT(tp, ContainsLongOption(long_opt_cstr));
    ASSERT_THAT(opt, ParamOptionNotGiven());
}

TEST_F(optionT, stringParamOptionConstructLongOptionTwiceThrows) {
    clapp::option::string_param_option_t opt{tp, long_opt_str, opt_desc_str};
    ASSERT_THROW(
        (clapp::option::string_param_option_t{tp, long_opt_str, opt_desc_str}),
        clapp::exception::option_exception_t);
}

TEST_F(optionT, stringParamOptionConstructShortOptionTwiceThrows) {
    clapp::option::string_param_option_t opt{tp, short_opt, opt_desc_str};
    ASSERT_THROW(
        (clapp::option::string_param_option_t{tp, short_opt, opt_desc_str}),
        clapp::exception::option_exception_t);
}

TEST_F(optionT, stringParamOptionConstructLongStringVecAndCallGetOptionHelp) {
    clapp::option::string_param_option_t opt{
        tp, std::vector<std::string>{long_opt_str, long_opt_cstr},
        opt_desc_str};
    ASSERT_THAT(
        tp.get_option_help(),
        testing::ContainerEq(option_test_parser_t::help_entry_vec_t{
            {"--" + long_opt_str + "|--" + long_opt_cstr + param_opt_postfix,
             opt_desc_str + " (" + purpose_optional_str + ")"}}));
}

TEST_F(optionT, stringParamOptionConstructShortVecAndCallGetOptionHelp) {
    clapp::option::string_param_option_t opt{tp, std::vector<char>{short_opt},
                                             opt_desc_str};
    ASSERT_THAT(tp.get_option_help(),
                testing::ContainerEq(option_test_parser_t::help_entry_vec_t{
                    {std::string{"-"} + short_opt + param_opt_postfix,
                     opt_desc_str + " (" + purpose_optional_str + ")"}}));
}

TEST_F(optionT,
       stringParamOptionConstructLongStringVecAndShortVecAndCallGetOptionHelp) {
    clapp::option::string_param_option_t opt{
        tp, std::vector<std::string>{long_opt_str, long_opt_cstr},
        std::vector<char>{short_opt}, opt_desc_str};
    ASSERT_THAT(tp.get_option_help(),
                testing::ContainerEq(option_test_parser_t::help_entry_vec_t{
                    {std::string{"-"} + short_opt + "|--" + long_opt_str +
                         "|--" + long_opt_cstr + param_opt_postfix,
                     opt_desc_str + " (" + purpose_optional_str + ")"}}));
}

TEST_F(optionT,
       stringParamOptionConstructLongStringAndShortAndCallGetOptionHelp) {
    clapp::option::string_param_option_t opt{
        tp, long_opt_str, short_opt, opt_desc_str,
        clapp::basic_parser_t::purpose_t::mandatory};
    ASSERT_THAT(tp.get_option_help(),
                testing::ContainerEq(option_test_parser_t::help_entry_vec_t{
                    {std::string{"-"} + short_opt + "|--" + long_opt_str +
                         param_opt_postfix,
                     opt_desc_str + " (" + purpose_mandatory_str + ")"}}));
}

TEST_F(optionT,
       stringParamOptionConstructLongCStringAndShortAndCallGetOptionHelp) {
    clapp::option::string_param_option_t opt{tp, long_opt_cstr, short_opt,
                                             opt_desc_str};
    ASSERT_THAT(tp.get_option_help(),
                testing::ContainerEq(option_test_parser_t::help_entry_vec_t{
                    {std::string{"-"} + short_opt + "|--" + long_opt_cstr +
                         param_opt_postfix,
                     opt_desc_str + " (" + purpose_optional_str + ")"}}));
}

TEST_F(optionT, stringParamOptionConstructLongStringAndCallLongOpt) {
    clapp::option::string_param_option_t opt{tp, long_opt_str, opt_desc_str};
    ASSERT_THAT(opt, ParamOptionNotGiven());
    ASSERT_NO_THROW(
        (get_long_opt_func<option_test_parser_t::long_opt_param_func_t>(
            tp, long_opt_str)(long_opt_str, value_cstr)));
    ASSERT_THAT(opt, ParamOptionGiven(value_cstr));
}

TEST_F(optionT, stringParamOptionConstructShortAndCallShortOpt) {
    clapp::option::string_param_option_t opt{tp, short_opt, opt_desc_str};
    ASSERT_THAT(opt, ParamOptionNotGiven());
    ASSERT_NO_THROW(
        (get_short_opt_func<option_test_parser_t::short_opt_param_func_t>(
            tp, short_opt)(short_opt, value_str)));
    ASSERT_THAT(opt, ParamOptionGiven(value_str));
}

TEST_F(
    optionT,
    stringParamOptionConstructLongStringAndShortAndCallShortOptCallsFoundFunc) {
    clapp::option::string_param_option_t opt{
        tp, long_opt_str, short_opt, opt_desc_str,
        clapp::value::found_func_t{[this]() { found_func_called++; }}};
    ASSERT_NO_THROW(
        (get_short_opt_func<option_test_parser_t::short_opt_param_func_t>(
            tp, short_opt)(short_opt, value_str)));

    constexpr std::uint32_t expected_found_func_called{1U};
    ASSERT_THAT(found_func_called, testing::Eq(expected_found_func_called));
}

TEST_F(optionT,
       stringParamOptionConstructMandatoryLongCStringAndCallValidateFunc) {
    clapp::option::string_param_option_t opt{
        tp, long_opt_cstr, opt_desc_str,
        option_test_parser_t::purpose_t::mandatory};
    std::optional<option_test_parser_t::validate_func_t> option_validate_func{
        get_validate_func(tp, long_opt_cstr)};
    ASSERT_THAT(option_validate_func, testing::Ne(std::nullopt));
    ASSERT_THROW(option_validate_func.value()(),
                 clapp::exception::option_param_exception_t);
    get_long_opt_func<option_test_parser_t::long_opt_param_func_t>(
        tp, long_opt_cstr)(long_opt_cstr, value_str);
    ASSERT_NO_THROW(option_validate_func.value()());
}

TEST_F(
    optionT,
    stringParamOptionConstructLongStringAndShortWithDefaultValueAndCallLongOptFunc) {
    clapp::option::string_param_option_t opt{
        tp, long_opt_str, short_opt, opt_desc_str,
        clapp::value::default_value_t<std::string>{value_cstr}};
    ASSERT_THAT(opt, ParamOptionNotGivenDefaultValue(std::string{value_cstr}));
    ASSERT_NO_THROW(
        (get_long_opt_func<option_test_parser_t::long_opt_param_func_t>(
            tp, long_opt_str)(long_opt_str, value_str)));
    ASSERT_THAT(opt, ParamOptionGiven(value_str));
}

#ifdef CLAPP_FS_AVAIL
TEST_F(optionT, pathParamOptionConstructShort) {
    clapp::option::path_param_option_t opt{tp, short_opt, opt_desc_str};
    ASSERT_THAT(tp, ContainsShortOption(short_opt));
    ASSERT_THAT(opt, ParamOptionNotGiven());
}

TEST_F(optionT, pathParamOptionConstructLongString) {
    clapp::option::path_param_option_t opt{tp, long_opt_str, opt_desc_str};
    ASSERT_THAT(tp, ContainsLongOption(long_opt_str));
    ASSERT_THAT(opt, ParamOptionNotGiven());
}

TEST_F(optionT, pathParamOptionConstructLongCString) {
    clapp::option::path_param_option_t opt{tp, long_opt_cstr, opt_desc_cstr};
    ASSERT_THAT(tp, ContainsLongOption(long_opt_cstr));
    ASSERT_THAT(opt, ParamOptionNotGiven());
}

TEST_F(optionT, pathParamOptionConstructShortVec) {
    clapp::option::path_param_option_t opt{tp, std::vector<char>{short_opt},
                                           opt_desc_str};
    ASSERT_THAT(tp, ContainsShortOption(short_opt));
    ASSERT_THAT(opt, ParamOptionNotGiven());
}

TEST_F(optionT, pathParamOptionConstructLongStringVec) {
    clapp::option::path_param_option_t opt{
        tp, std::vector<std::string>{long_opt_str, long_opt_cstr},
        opt_desc_str};
    ASSERT_THAT(tp, ContainsLongOption(long_opt_str));
    ASSERT_THAT(tp, ContainsLongOption(long_opt_cstr));
    ASSERT_THAT(opt, ParamOptionNotGiven());
}

TEST_F(optionT, pathParamOptionConstructLongOptionTwiceThrows) {
    clapp::option::path_param_option_t opt{tp, long_opt_str, opt_desc_str};
    ASSERT_THROW(
        (clapp::option::path_param_option_t{tp, long_opt_str, opt_desc_str}),
        clapp::exception::option_exception_t);
}

TEST_F(optionT, pathParamOptionConstructShortOptionTwiceThrows) {
    clapp::option::path_param_option_t opt{tp, short_opt, opt_desc_str};
    ASSERT_THROW(
        (clapp::option::path_param_option_t{tp, short_opt, opt_desc_str}),
        clapp::exception::option_exception_t);
}

TEST_F(optionT, pathParamOptionConstructLongStringVecAndCallGetOptionHelp) {
    clapp::option::path_param_option_t opt{
        tp, std::vector<std::string>{long_opt_str, long_opt_cstr},
        opt_desc_str};
    ASSERT_THAT(
        tp.get_option_help(),
        testing::ContainerEq(option_test_parser_t::help_entry_vec_t{
            {"--" + long_opt_str + "|--" + long_opt_cstr + param_opt_postfix,
             opt_desc_str + " (" + purpose_optional_str + ")"}}));
}

TEST_F(optionT, pathParamOptionConstructShortVecAndCallGetOptionHelp) {
    clapp::option::path_param_option_t opt{tp, std::vector<char>{short_opt},
                                           opt_desc_str};
    ASSERT_THAT(tp.get_option_help(),
                testing::ContainerEq(option_test_parser_t::help_entry_vec_t{
                    {std::string{"-"} + short_opt + param_opt_postfix,
                     opt_desc_str + " (" + purpose_optional_str + ")"}}));
}

TEST_F(optionT,
       pathParamOptionConstructLongStringVecAndShortVecAndCallGetOptionHelp) {
    clapp::option::path_param_option_t opt{
        tp, std::vector<std::string>{long_opt_str, long_opt_cstr},
        std::vector<char>{short_opt}, opt_desc_str};
    ASSERT_THAT(tp.get_option_help(),
                testing::ContainerEq(option_test_parser_t::help_entry_vec_t{
                    {std::string{"-"} + short_opt + "|--" + long_opt_str +
                         "|--" + long_opt_cstr + param_opt_postfix,
                     opt_desc_str + " (" + purpose_optional_str + ")"}}));
}

TEST_F(optionT,
       pathParamOptionConstructLongStringAndShortAndCallGetOptionHelp) {
    clapp::option::path_param_option_t opt{
        tp, long_opt_str, short_opt, opt_desc_str,
        clapp::basic_parser_t::purpose_t::mandatory};
    ASSERT_THAT(tp.get_option_help(),
                testing::ContainerEq(option_test_parser_t::help_entry_vec_t{
                    {std::string{"-"} + short_opt + "|--" + long_opt_str +
                         param_opt_postfix,
                     opt_desc_str + " (" + purpose_mandatory_str + ")"}}));
}

TEST_F(optionT,
       pathParamOptionConstructLongCStringAndShortAndCallGetOptionHelp) {
    clapp::option::path_param_option_t opt{tp, long_opt_cstr, short_opt,
                                           opt_desc_str};
    ASSERT_THAT(tp.get_option_help(),
                testing::ContainerEq(option_test_parser_t::help_entry_vec_t{
                    {std::string{"-"} + short_opt + "|--" + long_opt_cstr +
                         param_opt_postfix,
                     opt_desc_str + " (" + purpose_optional_str + ")"}}));
}

TEST_F(optionT, pathParamOptionConstructLongStringAndCallLongOpt) {
    clapp::option::path_param_option_t opt{tp, long_opt_str, opt_desc_str};
    ASSERT_THAT(opt, ParamOptionNotGiven());
    ASSERT_NO_THROW(
        (get_long_opt_func<option_test_parser_t::long_opt_param_func_t>(
            tp, long_opt_str)(long_opt_str, value_cstr_path)));
    ASSERT_THAT(opt, ParamOptionGiven(value_cstr_path));
}

TEST_F(optionT, pathParamOptionConstructShortAndCallShortOpt) {
    clapp::option::path_param_option_t opt{tp, short_opt, opt_desc_str};
    ASSERT_THAT(opt, ParamOptionNotGiven());
    ASSERT_NO_THROW(
        (get_short_opt_func<option_test_parser_t::short_opt_param_func_t>(
            tp, short_opt)(short_opt, value_str_path)));
    ASSERT_THAT(opt, ParamOptionGiven(value_str_path));
}

TEST_F(
    optionT,
    pathParamOptionConstructLongStringAndShortAndCallShortOptCallsFoundFunc) {
    clapp::option::path_param_option_t opt{
        tp, long_opt_str, short_opt, opt_desc_str,
        clapp::value::found_func_t{[this]() { found_func_called++; }}};
    ASSERT_NO_THROW(
        (get_short_opt_func<option_test_parser_t::short_opt_param_func_t>(
            tp, short_opt)(short_opt, value_str_path)));

    constexpr std::uint32_t expected_found_func_called{1U};
    ASSERT_THAT(found_func_called, testing::Eq(expected_found_func_called));
}

TEST_F(optionT,
       pathParamOptionConstructMandatoryLongCStringAndCallValidateFunc) {
    clapp::option::path_param_option_t opt{
        tp, long_opt_cstr, opt_desc_str,
        option_test_parser_t::purpose_t::mandatory};
    std::optional<option_test_parser_t::validate_func_t> option_validate_func{
        get_validate_func(tp, long_opt_cstr)};
    ASSERT_THAT(option_validate_func, testing::Ne(std::nullopt));
    ASSERT_THROW(option_validate_func.value()(),
                 clapp::exception::option_param_exception_t);
    get_long_opt_func<option_test_parser_t::long_opt_param_func_t>(
        tp, long_opt_cstr)(long_opt_cstr, value_str_path);
    ASSERT_NO_THROW(option_validate_func.value()());
}

TEST_F(
    optionT,
    pathParamOptionConstructLongStringAndShortWithDefaultValueAndCallLongOptFunc) {
    clapp::option::path_param_option_t opt{
        tp, long_opt_str, short_opt, opt_desc_str,
        clapp::value::default_value_t<std::string>{value_cstr_path}};
    ASSERT_THAT(opt,
                ParamOptionNotGivenDefaultValue(std::string{value_cstr_path}));
    ASSERT_NO_THROW(
        (get_long_opt_func<option_test_parser_t::long_opt_param_func_t>(
            tp, long_opt_str)(long_opt_str, value_str_path)));
    ASSERT_THAT(opt, ParamOptionGiven(value_str_path));
}
#endif

TEST_F(optionT, int64ParamOptionConstructShort) {
    clapp::option::int64_param_option_t opt{tp, short_opt, opt_desc_str};
    ASSERT_THAT(tp, ContainsShortOption(short_opt));
    ASSERT_THAT(opt, ParamOptionNotGiven());
}

TEST_F(optionT, int64ParamOptionConstructLongString) {
    clapp::option::int64_param_option_t opt{tp, long_opt_str, opt_desc_str};
    ASSERT_THAT(tp, ContainsLongOption(long_opt_str));
    ASSERT_THAT(opt, ParamOptionNotGiven());
}

TEST_F(optionT, int64ParamOptionConstructLongCString) {
    clapp::option::int64_param_option_t opt{tp, long_opt_cstr, opt_desc_cstr};
    ASSERT_THAT(tp, ContainsLongOption(long_opt_cstr));
    ASSERT_THAT(opt, ParamOptionNotGiven());
}

TEST_F(optionT, int64ParamOptionConstructShortVec) {
    clapp::option::int64_param_option_t opt{tp, std::vector<char>{short_opt},
                                            opt_desc_str};
    ASSERT_THAT(tp, ContainsShortOption(short_opt));
    ASSERT_THAT(opt, ParamOptionNotGiven());
}

TEST_F(optionT, int64ParamOptionConstructLongStringVec) {
    clapp::option::int64_param_option_t opt{
        tp, std::vector<std::string>{long_opt_str, long_opt_cstr},
        opt_desc_str};
    ASSERT_THAT(tp, ContainsLongOption(long_opt_str));
    ASSERT_THAT(tp, ContainsLongOption(long_opt_cstr));
    ASSERT_THAT(opt, ParamOptionNotGiven());
}

TEST_F(optionT, int64ParamOptionConstructLongOptionTwiceThrows) {
    clapp::option::int64_param_option_t opt{tp, long_opt_str, opt_desc_str};
    ASSERT_THROW(
        (clapp::option::int64_param_option_t{tp, long_opt_str, opt_desc_str}),
        clapp::exception::option_exception_t);
}

TEST_F(optionT, int64ParamOptionConstructShortOptionTwiceThrows) {
    clapp::option::int64_param_option_t opt{tp, short_opt, opt_desc_str};
    ASSERT_THROW(
        (clapp::option::int64_param_option_t{tp, short_opt, opt_desc_str}),
        clapp::exception::option_exception_t);
}

TEST_F(optionT, int64ParamOptionConstructLongStringVecAndCallGetOptionHelp) {
    clapp::option::int64_param_option_t opt{
        tp, std::vector<std::string>{long_opt_str, long_opt_cstr},
        opt_desc_str};
    ASSERT_THAT(
        tp.get_option_help(),
        testing::ContainerEq(option_test_parser_t::help_entry_vec_t{
            {"--" + long_opt_str + "|--" + long_opt_cstr + param_opt_postfix,
             opt_desc_str + " (" + purpose_optional_str + ")"}}));
}

TEST_F(optionT, int64ParamOptionConstructShortVecAndCallGetOptionHelp) {
    clapp::option::int64_param_option_t opt{tp, std::vector<char>{short_opt},
                                            opt_desc_str};
    ASSERT_THAT(tp.get_option_help(),
                testing::ContainerEq(option_test_parser_t::help_entry_vec_t{
                    {std::string{"-"} + short_opt + param_opt_postfix,
                     opt_desc_str + " (" + purpose_optional_str + ")"}}));
}

TEST_F(optionT,
       int64ParamOptionConstructLongStringVecAndShortVecAndCallGetOptionHelp) {
    clapp::option::int64_param_option_t opt{
        tp, std::vector<std::string>{long_opt_str, long_opt_cstr},
        std::vector<char>{short_opt}, opt_desc_str};
    ASSERT_THAT(tp.get_option_help(),
                testing::ContainerEq(option_test_parser_t::help_entry_vec_t{
                    {std::string{"-"} + short_opt + "|--" + long_opt_str +
                         "|--" + long_opt_cstr + param_opt_postfix,
                     opt_desc_str + " (" + purpose_optional_str + ")"}}));
}

TEST_F(optionT,
       int64ParamOptionConstructLongStringAndShortAndCallGetOptionHelp) {
    clapp::option::int64_param_option_t opt{
        tp, long_opt_str, short_opt, opt_desc_str,
        clapp::basic_parser_t::purpose_t::mandatory};
    ASSERT_THAT(tp.get_option_help(),
                testing::ContainerEq(option_test_parser_t::help_entry_vec_t{
                    {std::string{"-"} + short_opt + "|--" + long_opt_str +
                         param_opt_postfix,
                     opt_desc_str + " (" + purpose_mandatory_str + ")"}}));
}

TEST_F(optionT,
       int64ParamOptionConstructLongCStringAndShortAndCallGetOptionHelp) {
    clapp::option::int64_param_option_t opt{tp, long_opt_cstr, short_opt,
                                            opt_desc_str};
    ASSERT_THAT(tp.get_option_help(),
                testing::ContainerEq(option_test_parser_t::help_entry_vec_t{
                    {std::string{"-"} + short_opt + "|--" + long_opt_cstr +
                         param_opt_postfix,
                     opt_desc_str + " (" + purpose_optional_str + ")"}}));
}

TEST_F(optionT,
       int64ParamOptionConstructLongStringVecAndShortAndCallGetOptionHelp) {
    constexpr std::int64_t min_value{5};
    constexpr std::int64_t max_value{0x17};
    clapp::option::int64_param_option_t opt{
        tp, std::vector<std::string>{long_opt_str}, short_opt, opt_desc_str,
        clapp::value::min_max_value_t<std::int64_t>{min_value, max_value}};
    ASSERT_THAT(tp.get_option_help(),
                testing::ContainerEq(option_test_parser_t::help_entry_vec_t{
                    {std::string{"-"} + short_opt + "|--" + long_opt_str +
                         param_opt_postfix,
                     opt_desc_str + " (" + purpose_optional_str +
                         ", constraint: [" + std::to_string(min_value) + "," +
                         std::to_string(max_value) + "])"}}));
}

TEST_F(optionT, int64ParamOptionConstructLongStringAndCallLongOpt) {
    clapp::option::int64_param_option_t opt{tp, long_opt_str, opt_desc_str};
    ASSERT_THAT(opt, ParamOptionNotGiven());
    ASSERT_NO_THROW(
        (get_long_opt_func<option_test_parser_t::long_opt_param_func_t>(
            tp, long_opt_str)(long_opt_str, std::to_string(value_int64))));
    ASSERT_THAT(opt, ParamOptionGiven(value_int64));
}

TEST_F(optionT, int64ParamOptionConstructShortAndCallShortOpt) {
    clapp::option::int64_param_option_t opt{tp, short_opt, opt_desc_str};
    ASSERT_THAT(opt, ParamOptionNotGiven());
    ASSERT_NO_THROW(
        (get_short_opt_func<option_test_parser_t::short_opt_param_func_t>(
            tp, short_opt)(short_opt, std::to_string(value_int64))));
    ASSERT_THAT(opt, ParamOptionGiven(value_int64));
}

TEST_F(
    optionT,
    int64ParamOptionConstructLongStringAndShortAndCallShortOptCallsFoundFunc) {
    clapp::option::int64_param_option_t opt{
        tp, long_opt_str, short_opt, opt_desc_str,
        clapp::value::found_func_t{[this]() { found_func_called++; }}};
    ASSERT_NO_THROW(
        (get_short_opt_func<option_test_parser_t::short_opt_param_func_t>(
            tp, short_opt)(short_opt, std::to_string(value_int64))));

    constexpr std::uint32_t expected_found_func_called{1U};
    ASSERT_THAT(found_func_called, testing::Eq(expected_found_func_called));
}

TEST_F(
    optionT,
    int64ParamOptionConstructShortAndCallShortOptFuncWithInvalidValueThrows) {
    clapp::option::int64_param_option_t opt{tp, short_opt, opt_desc_str};
    ASSERT_THROW(
        (get_short_opt_func<option_test_parser_t::short_opt_param_func_t>(
            tp, short_opt)(short_opt, value_str)),
        clapp::exception::invalid_value_t);
}

TEST_F(optionT, int64ParamOptionConstructShortOptionalNoValidateFunc) {
    clapp::option::int64_param_option_t opt{
        tp, short_opt, opt_desc_str, option_test_parser_t::purpose_t::optional};
    ASSERT_THAT(get_validate_func(tp, short_opt), testing::Eq(std::nullopt));
}

TEST_F(optionT, int64ParamOptionConstructLongOptionalNoValidateFunc) {
    clapp::option::int64_param_option_t opt{
        tp, long_opt_str, opt_desc_str,
        option_test_parser_t::purpose_t::optional};
    ASSERT_THAT(get_validate_func(tp, long_opt_str), testing::Eq(std::nullopt));
}

TEST_F(optionT, int64ParamOptionConstructMandatoryShortAndCallValidateFunc) {
    clapp::option::int64_param_option_t opt{
        tp, short_opt, opt_desc_str,
        option_test_parser_t::purpose_t::mandatory};
    std::optional<option_test_parser_t::validate_func_t> option_validate_func{
        get_validate_func(tp, short_opt)};
    ASSERT_THAT(option_validate_func, testing::Ne(std::nullopt));
    ASSERT_THROW(option_validate_func.value()(),
                 clapp::exception::option_param_exception_t);
    ASSERT_NO_THROW(
        (get_short_opt_func<option_test_parser_t::short_opt_param_func_t>(
            tp, short_opt)(short_opt, std::to_string(value_int64))));
    ASSERT_NO_THROW(option_validate_func.value()());
}

TEST_F(
    optionT,
    int64ParamOptionConstructLongStringVecAndShortWithMinMaxConstraintAndCallLongOptFuncAndShortOptFunc) {
    constexpr std::int64_t min_value{0};
    constexpr std::int64_t max_value{0x20};
    clapp::option::int64_param_option_t opt{
        tp, std::vector<std::string>{long_opt_str}, short_opt, opt_desc_str,
        clapp::value::min_max_value_t<std::int64_t>{min_value, max_value}};
    std::optional<option_test_parser_t::validate_func_t> option_validate_func{
        get_validate_func(tp, long_opt_str)};
    ASSERT_THAT(option_validate_func, testing::Ne(std::nullopt));
    get_long_opt_func<option_test_parser_t::long_opt_param_func_t>(
        tp, long_opt_str)(long_opt_str, std::to_string(value_int32));
    ASSERT_THROW((option_validate_func.value()()),
                 clapp::exception::out_of_range_t);
    get_long_opt_func<option_test_parser_t::long_opt_param_func_t>(
        tp, long_opt_str)(long_opt_str, std::to_string(value_int8));
    ASSERT_NO_THROW((option_validate_func.value()()));
    get_long_opt_func<option_test_parser_t::long_opt_param_func_t>(
        tp, long_opt_str)(long_opt_str, std::to_string(max_value));
    ASSERT_NO_THROW((option_validate_func.value()()));
    get_long_opt_func<option_test_parser_t::long_opt_param_func_t>(
        tp, long_opt_str)(long_opt_str, std::to_string(min_value));
    ASSERT_NO_THROW((option_validate_func.value()()));
    get_long_opt_func<option_test_parser_t::long_opt_param_func_t>(
        tp, long_opt_str)(long_opt_str, std::to_string(max_value + 1));
    ASSERT_THROW((option_validate_func.value()()),
                 clapp::exception::out_of_range_t);
    get_long_opt_func<option_test_parser_t::long_opt_param_func_t>(
        tp, long_opt_str)(long_opt_str, std::to_string(min_value - 1));
    ASSERT_THROW((option_validate_func.value()()),
                 clapp::exception::out_of_range_t);
}

TEST_F(
    optionT,
    int64ParamOptionConstructLongStringAndShortWithDefaultValueAndCallShortOptFunc) {
    clapp::option::int64_param_option_t opt{
        tp, long_opt_str, short_opt, opt_desc_str,
        clapp::value::default_value_t<std::int64_t>{value_int32}};
    ASSERT_THAT(opt, ParamOptionNotGivenDefaultValue(value_int32));
    ASSERT_NO_THROW(
        (get_short_opt_func<option_test_parser_t::short_opt_param_func_t>(
            tp, short_opt)(short_opt, std::to_string(value_int64))));
    ASSERT_THAT(opt, ParamOptionGiven(value_int64));
}

TEST_F(optionT, int32ParamOptionConstructLongStringAndShortAndCallShortOpt) {
    clapp::option::int32_param_option_t opt{tp, long_opt_str, short_opt,
                                            opt_desc_str};
    ASSERT_THAT(opt, ParamOptionNotGiven());
    ASSERT_NO_THROW(
        (get_short_opt_func<option_test_parser_t::short_opt_param_func_t>(
            tp, short_opt)(short_opt, std::to_string(value_int32))));
    ASSERT_THAT(opt, ParamOptionGiven(value_int32));
}

TEST_F(optionT, int16ParamOptionConstructLongStringAndShortAndCallShortOpt) {
    clapp::option::int16_param_option_t opt{tp, long_opt_str, short_opt,
                                            opt_desc_str};
    ASSERT_THAT(opt, ParamOptionNotGiven());
    ASSERT_NO_THROW(
        (get_short_opt_func<option_test_parser_t::short_opt_param_func_t>(
            tp, short_opt)(short_opt, std::to_string(value_int16))));
    ASSERT_THAT(opt, ParamOptionGiven(value_int16));
}

TEST_F(optionT, int8ParamOptionConstructLongStringAndShortAndCallShortOpt) {
    clapp::option::int8_param_option_t opt{tp, long_opt_str, short_opt,
                                           opt_desc_str};
    ASSERT_THAT(opt, ParamOptionNotGiven());
    ASSERT_NO_THROW(
        (get_short_opt_func<option_test_parser_t::short_opt_param_func_t>(
            tp, short_opt)(short_opt, std::to_string(value_int8))));
    ASSERT_THAT(opt, ParamOptionGiven(value_int8));
}

TEST_F(
    optionT,
    uint64ParamOptionConstructLongStringAndShortWithDefaultValueAndCallShortOptFunc) {
    clapp::option::uint64_param_option_t opt{
        tp, long_opt_str, short_opt, opt_desc_str,
        clapp::value::default_value_t<std::uint64_t>{value_uint32}};
    ASSERT_THAT(opt, ParamOptionNotGivenDefaultValue(value_uint32));
    ASSERT_NO_THROW(
        (get_short_opt_func<option_test_parser_t::short_opt_param_func_t>(
            tp, short_opt)(short_opt, std::to_string(value_uint64))));
    ASSERT_THAT(opt, ParamOptionGiven(value_uint64));
}

TEST_F(optionT, uint32ParamOptionConstructLongStringAndShortAndCallShortOpt) {
    clapp::option::uint32_param_option_t opt{tp, long_opt_str, short_opt,
                                             opt_desc_str};
    ASSERT_THAT(opt, ParamOptionNotGiven());
    ASSERT_NO_THROW(
        (get_short_opt_func<option_test_parser_t::short_opt_param_func_t>(
            tp, short_opt)(short_opt, std::to_string(value_uint32))));
    ASSERT_THAT(opt, ParamOptionGiven(value_uint32));
}

TEST_F(optionT, uint16ParamOptionConstructLongStringAndShortAndCallShortOpt) {
    clapp::option::uint16_param_option_t opt{tp, long_opt_str, short_opt,
                                             opt_desc_str};
    ASSERT_THAT(opt, ParamOptionNotGiven());
    ASSERT_NO_THROW(
        (get_short_opt_func<option_test_parser_t::short_opt_param_func_t>(
            tp, short_opt)(short_opt, std::to_string(value_uint16))));
    ASSERT_THAT(opt, ParamOptionGiven(value_uint16));
}

TEST_F(optionT, uint8ParamOptionConstructLongStringAndShortAndCallShortOpt) {
    clapp::option::uint8_param_option_t opt{tp, long_opt_str, short_opt,
                                            opt_desc_str};
    ASSERT_THAT(opt, ParamOptionNotGiven());
    ASSERT_NO_THROW(
        (get_short_opt_func<option_test_parser_t::short_opt_param_func_t>(
            tp, short_opt)(short_opt, std::to_string(value_uint8))));
    ASSERT_THAT(opt, ParamOptionGiven(value_uint8));
}

TEST_F(optionT, boolParamOptionConstructLongStringAndShortAndCallShortOpt) {
    clapp::option::bool_param_option_t opt{tp, long_opt_str, short_opt,
                                           opt_desc_str};
    ASSERT_THAT(opt, ParamOptionNotGiven());
    ASSERT_NO_THROW(
        (get_short_opt_func<option_test_parser_t::short_opt_param_func_t>(
            tp, short_opt)(short_opt, std::to_string(value_bool))));
    ASSERT_THAT(opt, ParamOptionGiven(value_bool));
}

TEST_F(optionT, sizeParamOptionConstructLongStringAndShortAndCallShortOpt) {
    clapp::option::size_param_option_t opt{tp, long_opt_str, short_opt,
                                           opt_desc_str};
    ASSERT_THAT(opt, ParamOptionNotGiven());
    ASSERT_NO_THROW(
        (get_short_opt_func<option_test_parser_t::short_opt_param_func_t>(
            tp, short_opt)(short_opt, std::to_string(value_size))));
    ASSERT_THAT(opt, ParamOptionGiven(value_size));
}

TEST_F(optionT, ptrdiffParamOptionConstructLongStringAndShortAndCallShortOpt) {
    clapp::option::ptrdiff_param_option_t opt{tp, long_opt_str, short_opt,
                                              opt_desc_str};
    ASSERT_THAT(opt, ParamOptionNotGiven());
    ASSERT_NO_THROW(
        (get_short_opt_func<option_test_parser_t::short_opt_param_func_t>(
            tp, short_opt)(short_opt, std::to_string(value_ptrdiff))));
    ASSERT_THAT(opt, ParamOptionGiven(value_ptrdiff));
}

TEST_F(optionT, doubleParamOptionConstructLongStringAndShortAndCallShortOpt) {
    clapp::option::double_param_option_t opt{tp, long_opt_str, short_opt,
                                             opt_desc_str};
    ASSERT_THAT(opt, ParamOptionNotGiven());
    ASSERT_NO_THROW(
        (get_short_opt_func<option_test_parser_t::short_opt_param_func_t>(
            tp, short_opt)(short_opt, std::to_string(value_double))));
    ASSERT_THAT(opt, ParamOptionGiven(value_double));
}

TEST_F(optionT, floatParamOptionConstructLongStringAndShortAndCallShortOpt) {
    clapp::option::float_param_option_t opt{tp, long_opt_str, short_opt,
                                            opt_desc_str};
    ASSERT_THAT(opt, ParamOptionNotGiven());
    ASSERT_NO_THROW(
        (get_short_opt_func<option_test_parser_t::short_opt_param_func_t>(
            tp, short_opt)(short_opt, std::to_string(value_float))));
    ASSERT_THAT(opt, ParamOptionGiven(value_float));
}

TEST_F(optionT, nsParamOptionConstructLongStringAndShortAndCallShortOpt) {
    clapp::option::ns_param_option_t opt{tp, long_opt_str, short_opt,
                                         opt_desc_str};
    ASSERT_THAT(opt, ParamOptionNotGiven());
    ASSERT_NO_THROW(
        (get_short_opt_func<option_test_parser_t::short_opt_param_func_t>(
            tp, short_opt)(short_opt, std::to_string(value_ns.count()))));
    ASSERT_THAT(opt, ParamOptionGiven(value_ns));
}

TEST_F(optionT, usParamOptionConstructLongStringAndShortAndCallShortOpt) {
    clapp::option::us_param_option_t opt{tp, long_opt_str, short_opt,
                                         opt_desc_str};
    ASSERT_THAT(opt, ParamOptionNotGiven());
    ASSERT_NO_THROW(
        (get_short_opt_func<option_test_parser_t::short_opt_param_func_t>(
            tp, short_opt)(short_opt, std::to_string(value_us.count()))));
    ASSERT_THAT(opt, ParamOptionGiven(value_us));
}

TEST_F(optionT, msParamOptionConstructLongStringAndShortAndCallShortOpt) {
    clapp::option::ms_param_option_t opt{tp, long_opt_str, short_opt,
                                         opt_desc_str};
    ASSERT_THAT(opt, ParamOptionNotGiven());
    ASSERT_NO_THROW(
        (get_short_opt_func<option_test_parser_t::short_opt_param_func_t>(
            tp, short_opt)(short_opt, std::to_string(value_ms.count()))));
    ASSERT_THAT(opt, ParamOptionGiven(value_ms));
}

TEST_F(optionT, secParamOptionConstructLongStringAndShortAndCallShortOpt) {
    clapp::option::sec_param_option_t opt{tp, long_opt_str, short_opt,
                                          opt_desc_str};
    ASSERT_THAT(opt, ParamOptionNotGiven());
    ASSERT_NO_THROW(
        (get_short_opt_func<option_test_parser_t::short_opt_param_func_t>(
            tp, short_opt)(short_opt, std::to_string(value_sec.count()))));
    ASSERT_THAT(opt, ParamOptionGiven(value_sec));
}

TEST_F(optionT, minParamOptionConstructLongStringAndShortAndCallShortOpt) {
    clapp::option::min_param_option_t opt{tp, long_opt_str, short_opt,
                                          opt_desc_str};
    ASSERT_THAT(opt, ParamOptionNotGiven());
    ASSERT_NO_THROW(
        (get_short_opt_func<option_test_parser_t::short_opt_param_func_t>(
            tp, short_opt)(short_opt, std::to_string(value_min.count()))));
    ASSERT_THAT(opt, ParamOptionGiven(value_min));
}

TEST_F(optionT, hoursParamOptionConstructLongStringAndShortAndCallShortOpt) {
    clapp::option::hours_param_option_t opt{tp, long_opt_str, short_opt,
                                            opt_desc_str};
    ASSERT_THAT(opt, ParamOptionNotGiven());
    ASSERT_NO_THROW(
        (get_short_opt_func<option_test_parser_t::short_opt_param_func_t>(
            tp, short_opt)(short_opt, std::to_string(value_hours.count()))));
    ASSERT_THAT(opt, ParamOptionGiven(value_hours));
}

TEST_F(
    optionT,
    vectorStringParamOptionConstructLongStringAndShortWithDefaultValueThrows) {
    ASSERT_THROW((clapp::option::vector_string_param_option_t{
                     tp, long_opt_str, short_opt, opt_desc_str,
                     clapp::value::default_value_t<std::string>{value_cstr}}),
                 clapp::exception::option_param_exception_t);
}

TEST_F(optionT, vectorStringParamOptionConstructShort) {
    clapp::option::vector_string_param_option_t opt{tp, short_opt,
                                                    opt_desc_str};
    ASSERT_THAT(tp, ContainsShortOption(short_opt));
    ASSERT_THAT(opt, VectorParamOptionNotGiven());
}

TEST_F(optionT, vectorStringParamOptionConstructLongString) {
    clapp::option::vector_string_param_option_t opt{tp, long_opt_str,
                                                    opt_desc_str};
    ASSERT_THAT(tp, ContainsLongOption(long_opt_str));
    ASSERT_THAT(opt, VectorParamOptionNotGiven());
}

TEST_F(optionT, vectorStringParamOptionConstructLongCString) {
    clapp::option::vector_string_param_option_t opt{tp, long_opt_cstr,
                                                    opt_desc_cstr};
    ASSERT_THAT(tp, ContainsLongOption(long_opt_cstr));
    ASSERT_THAT(opt, VectorParamOptionNotGiven());
}

TEST_F(optionT, vectorStringParamOptionConstructShortVec) {
    clapp::option::vector_string_param_option_t opt{
        tp, std::vector<char>{short_opt}, opt_desc_str};
    ASSERT_THAT(tp, ContainsShortOption(short_opt));
    ASSERT_THAT(opt, VectorParamOptionNotGiven());
}

TEST_F(optionT, vectorStringParamOptionConstructLongStringVec) {
    clapp::option::vector_string_param_option_t opt{
        tp, std::vector<std::string>{long_opt_str, long_opt_cstr},
        opt_desc_str};
    ASSERT_THAT(tp, ContainsLongOption(long_opt_str));
    ASSERT_THAT(tp, ContainsLongOption(long_opt_cstr));
    ASSERT_THAT(opt, VectorParamOptionNotGiven());
}

TEST_F(optionT, vectorStringParamOptionConstructLongOptionTwiceThrows) {
    clapp::option::vector_string_param_option_t opt{tp, long_opt_str,
                                                    opt_desc_str};
    ASSERT_THROW((clapp::option::vector_string_param_option_t{tp, long_opt_str,
                                                              opt_desc_str}),
                 clapp::exception::option_exception_t);
}

TEST_F(optionT, vectorStringParamOptionConstructShortOptionTwiceThrows) {
    clapp::option::vector_string_param_option_t opt{tp, short_opt,
                                                    opt_desc_str};
    ASSERT_THROW((clapp::option::vector_string_param_option_t{tp, short_opt,
                                                              opt_desc_str}),
                 clapp::exception::option_exception_t);
}

TEST_F(optionT,
       vectorStringParamOptionConstructLongStringVecAndCallGetOptionHelp) {
    clapp::option::vector_string_param_option_t opt{
        tp, std::vector<std::string>{long_opt_str, long_opt_cstr},
        opt_desc_str};
    ASSERT_THAT(
        tp.get_option_help(),
        testing::ContainerEq(option_test_parser_t::help_entry_vec_t{
            {"--" + long_opt_str + "|--" + long_opt_cstr + param_opt_postfix,
             opt_desc_str + " (" + purpose_optional_str + ", " +
                 vector_opt_desc_restriction + ")"}}));
}

TEST_F(optionT, vectorStringParamOptionConstructShortVecAndCallGetOptionHelp) {
    clapp::option::vector_string_param_option_t opt{
        tp, std::vector<char>{short_opt}, opt_desc_str};
    ASSERT_THAT(tp.get_option_help(),
                testing::ContainerEq(option_test_parser_t::help_entry_vec_t{
                    {std::string{"-"} + short_opt + param_opt_postfix,
                     opt_desc_str + " (" + purpose_optional_str + ", " +
                         vector_opt_desc_restriction + ")"}}));
}

TEST_F(
    optionT,
    vectorStringParamOptionConstructLongStringVecAndShortVecAndCallGetOptionHelp) {
    clapp::option::vector_string_param_option_t opt{
        tp, std::vector<std::string>{long_opt_str, long_opt_cstr},
        std::vector<char>{short_opt}, opt_desc_str,
        clapp::basic_parser_t::purpose_t::mandatory};
    ASSERT_THAT(tp.get_option_help(),
                testing::ContainerEq(option_test_parser_t::help_entry_vec_t{
                    {std::string{"-"} + short_opt + "|--" + long_opt_str +
                         "|--" + long_opt_cstr + param_opt_postfix,
                     opt_desc_str + " (" + purpose_mandatory_str + ", " +
                         vector_opt_desc_restriction + ")"}}));
}

TEST_F(optionT,
       vectorStringParamOptionConstructLongStringAndShortAndCallGetOptionHelp) {
    clapp::option::vector_string_param_option_t opt{tp, long_opt_str, short_opt,
                                                    opt_desc_str};
    ASSERT_THAT(tp.get_option_help(),
                testing::ContainerEq(option_test_parser_t::help_entry_vec_t{
                    {std::string{"-"} + short_opt + "|--" + long_opt_str +
                         param_opt_postfix,
                     opt_desc_str + " (" + purpose_optional_str + ", " +
                         vector_opt_desc_restriction + ")"}}));
}

TEST_F(
    optionT,
    vectorStringParamOptionConstructLongCStringAndShortAndCallGetOptionHelp) {
    clapp::option::vector_string_param_option_t opt{tp, long_opt_cstr,
                                                    short_opt, opt_desc_str};
    ASSERT_THAT(tp.get_option_help(),
                testing::ContainerEq(option_test_parser_t::help_entry_vec_t{
                    {std::string{"-"} + short_opt + "|--" + long_opt_cstr +
                         param_opt_postfix,
                     opt_desc_str + " (" + purpose_optional_str + ", " +
                         vector_opt_desc_restriction + ")"}}));
}

TEST_F(optionT, vectorStringParamOptionConstructLongStringAndCallLongOpt) {
    clapp::option::vector_string_param_option_t opt{tp, long_opt_str,
                                                    opt_desc_str};
    ASSERT_THAT(opt, VectorParamOptionNotGiven());
    ASSERT_NO_THROW(
        (get_long_opt_func<option_test_parser_t::long_opt_param_func_t>(
            tp, long_opt_str)(long_opt_str, value_cstr)));
    ASSERT_THAT(opt, VectorParamOptionGiven(
                         std::vector<std::string>{std::string{value_cstr}}));
}

TEST_F(optionT, vectorStringParamOptionConstructShortAndCallShortOpt) {
    clapp::option::vector_string_param_option_t opt{tp, short_opt,
                                                    opt_desc_str};
    ASSERT_THAT(opt, VectorParamOptionNotGiven());
    ASSERT_NO_THROW(
        (get_short_opt_func<option_test_parser_t::short_opt_param_func_t>(
            tp, short_opt)(short_opt, value_str)));
    ASSERT_THAT(opt,
                VectorParamOptionGiven(std::vector<std::string>{value_str}));
}

TEST_F(
    optionT,
    vectorStringParamOptionConstructShortAndLongCStringAndCallShortAndLongCStringOpt) {
    clapp::option::vector_string_param_option_t opt{tp, long_opt_cstr,
                                                    short_opt, opt_desc_str};
    ASSERT_THAT(opt, VectorParamOptionNotGiven());
    ASSERT_NO_THROW(
        (get_short_opt_func<option_test_parser_t::short_opt_param_func_t>(
            tp, short_opt)(short_opt, value_str)));
    ASSERT_NO_THROW(
        (get_long_opt_func<option_test_parser_t::long_opt_param_func_t>(
            tp, long_opt_cstr)(long_opt_cstr, value_cstr)));
    ASSERT_THAT(opt, VectorParamOptionGiven(std::vector<std::string>{
                         value_str, std::string{value_cstr}}));
}

TEST_F(
    optionT,
    vectorStringParamOptionConstructShortLongStringAndLongCStringAndCallShortLongStringAndLongCStringOpt) {
    clapp::option::vector_string_param_option_t opt{
        tp, std::vector<std::string>{long_opt_str, long_opt_cstr}, short_opt,
        opt_desc_str};
    ASSERT_THAT(opt, VectorParamOptionNotGiven());
    ASSERT_NO_THROW(
        (get_short_opt_func<option_test_parser_t::short_opt_param_func_t>(
            tp, short_opt)(short_opt, value_str)));
    ASSERT_NO_THROW(
        (get_long_opt_func<option_test_parser_t::long_opt_param_func_t>(
            tp, long_opt_str)(long_opt_str, value_cstr)));
    ASSERT_NO_THROW(
        (get_long_opt_func<option_test_parser_t::long_opt_param_func_t>(
            tp, long_opt_cstr)(long_opt_cstr, value_cstr)));
    ASSERT_NO_THROW(
        (get_long_opt_func<option_test_parser_t::long_opt_param_func_t>(
            tp, long_opt_str)(long_opt_str, value_str)));
    ASSERT_THAT(opt, VectorParamOptionGiven(std::vector<std::string>{
                         value_str, std::string{value_cstr},
                         std::string{value_cstr}, value_str}));
}

TEST_F(
    optionT,
    vectorStringParamOptionConstructLongStringAndShortAndCallShortOptCallsFoundFunc) {
    clapp::option::vector_string_param_option_t opt{
        tp, long_opt_str, short_opt, opt_desc_str,
        clapp::value::found_func_t{[this]() { found_func_called++; }}};
    ASSERT_NO_THROW(
        (get_short_opt_func<option_test_parser_t::short_opt_param_func_t>(
            tp, short_opt)(short_opt, value_str)));

    constexpr std::uint32_t expected_found_func_called{1U};
    ASSERT_THAT(found_func_called, testing::Eq(expected_found_func_called));
}

TEST_F(
    optionT,
    vectorStringParamOptionConstructMandatoryLongCStringAndCallValidateFunc) {
    clapp::option::vector_string_param_option_t opt{
        tp, long_opt_cstr, opt_desc_str,
        option_test_parser_t::purpose_t::mandatory};
    std::optional<option_test_parser_t::validate_func_t> option_validate_func{
        get_validate_func(tp, long_opt_cstr)};
    ASSERT_THAT(option_validate_func, testing::Ne(std::nullopt));
    ASSERT_THROW(option_validate_func.value()(),
                 clapp::exception::option_param_exception_t);
    get_long_opt_func<option_test_parser_t::long_opt_param_func_t>(
        tp, long_opt_cstr)(long_opt_cstr, value_str);
    ASSERT_NO_THROW(option_validate_func.value()());
}

#ifdef CLAPP_FS_AVAIL
TEST_F(optionT,
       vectorPathParamOptionConstructLongStringAndShortWithDefaultValueThrows) {
    ASSERT_THROW(
        (clapp::option::vector_path_param_option_t{
            tp, long_opt_str, short_opt, opt_desc_str,
            clapp::value::default_value_t<clapp::fs::path>{value_cstr_path}}),
        clapp::exception::option_param_exception_t);
}

TEST_F(optionT, vectorPathParamOptionConstructShort) {
    clapp::option::vector_path_param_option_t opt{tp, short_opt, opt_desc_str};
    ASSERT_THAT(tp, ContainsShortOption(short_opt));
    ASSERT_THAT(opt, VectorParamOptionNotGiven());
}

TEST_F(optionT, vectorPathParamOptionConstructLongString) {
    clapp::option::vector_path_param_option_t opt{tp, long_opt_str,
                                                  opt_desc_str};
    ASSERT_THAT(tp, ContainsLongOption(long_opt_str));
    ASSERT_THAT(opt, VectorParamOptionNotGiven());
}

TEST_F(optionT, vectorPathParamOptionConstructLongCString) {
    clapp::option::vector_path_param_option_t opt{tp, long_opt_cstr,
                                                  opt_desc_cstr};
    ASSERT_THAT(tp, ContainsLongOption(long_opt_cstr));
    ASSERT_THAT(opt, VectorParamOptionNotGiven());
}

TEST_F(optionT, vectorPathParamOptionConstructShortVec) {
    clapp::option::vector_path_param_option_t opt{
        tp, std::vector<char>{short_opt}, opt_desc_str};
    ASSERT_THAT(tp, ContainsShortOption(short_opt));
    ASSERT_THAT(opt, VectorParamOptionNotGiven());
}

TEST_F(optionT, vectorPathParamOptionConstructLongStringVec) {
    clapp::option::vector_path_param_option_t opt{
        tp, std::vector<std::string>{long_opt_str, long_opt_cstr},
        opt_desc_str};
    ASSERT_THAT(tp, ContainsLongOption(long_opt_str));
    ASSERT_THAT(tp, ContainsLongOption(long_opt_cstr));
    ASSERT_THAT(opt, VectorParamOptionNotGiven());
}

TEST_F(optionT, vectorPathParamOptionConstructLongOptionTwiceThrows) {
    clapp::option::vector_path_param_option_t opt{tp, long_opt_str,
                                                  opt_desc_str};
    ASSERT_THROW((clapp::option::vector_path_param_option_t{tp, long_opt_str,
                                                            opt_desc_str}),
                 clapp::exception::option_exception_t);
}

TEST_F(optionT, vectorPathParamOptionConstructShortOptionTwiceThrows) {
    clapp::option::vector_path_param_option_t opt{tp, short_opt, opt_desc_str};
    ASSERT_THROW((clapp::option::vector_path_param_option_t{tp, short_opt,
                                                            opt_desc_str}),
                 clapp::exception::option_exception_t);
}

TEST_F(optionT,
       vectorPathParamOptionConstructLongStringVecAndCallGetOptionHelp) {
    clapp::option::vector_path_param_option_t opt{
        tp, std::vector<std::string>{long_opt_str, long_opt_cstr},
        opt_desc_str};
    ASSERT_THAT(
        tp.get_option_help(),
        testing::ContainerEq(option_test_parser_t::help_entry_vec_t{
            {"--" + long_opt_str + "|--" + long_opt_cstr + param_opt_postfix,
             opt_desc_str + " (" + purpose_optional_str + ", " +
                 vector_opt_desc_restriction + ")"}}));
}

TEST_F(optionT, vectorPathParamOptionConstructShortVecAndCallGetOptionHelp) {
    clapp::option::vector_path_param_option_t opt{
        tp, std::vector<char>{short_opt}, opt_desc_str};
    ASSERT_THAT(tp.get_option_help(),
                testing::ContainerEq(option_test_parser_t::help_entry_vec_t{
                    {std::string{"-"} + short_opt + param_opt_postfix,
                     opt_desc_str + " (" + purpose_optional_str + ", " +
                         vector_opt_desc_restriction + ")"}}));
}

TEST_F(
    optionT,
    vectorPathParamOptionConstructLongStringVecAndShortVecAndCallGetOptionHelp) {
    clapp::option::vector_path_param_option_t opt{
        tp, std::vector<std::string>{long_opt_str, long_opt_cstr},
        std::vector<char>{short_opt}, opt_desc_str,
        clapp::basic_parser_t::purpose_t::mandatory};
    ASSERT_THAT(tp.get_option_help(),
                testing::ContainerEq(option_test_parser_t::help_entry_vec_t{
                    {std::string{"-"} + short_opt + "|--" + long_opt_str +
                         "|--" + long_opt_cstr + param_opt_postfix,
                     opt_desc_str + " (" + purpose_mandatory_str + ", " +
                         vector_opt_desc_restriction + ")"}}));
}

TEST_F(optionT,
       vectorPathParamOptionConstructLongStringAndShortAndCallGetOptionHelp) {
    clapp::option::vector_path_param_option_t opt{tp, long_opt_str, short_opt,
                                                  opt_desc_str};
    ASSERT_THAT(tp.get_option_help(),
                testing::ContainerEq(option_test_parser_t::help_entry_vec_t{
                    {std::string{"-"} + short_opt + "|--" + long_opt_str +
                         param_opt_postfix,
                     opt_desc_str + " (" + purpose_optional_str + ", " +
                         vector_opt_desc_restriction + ")"}}));
}

TEST_F(optionT,
       vectorPathParamOptionConstructLongCStringAndShortAndCallGetOptionHelp) {
    clapp::option::vector_path_param_option_t opt{tp, long_opt_cstr, short_opt,
                                                  opt_desc_str};
    ASSERT_THAT(tp.get_option_help(),
                testing::ContainerEq(option_test_parser_t::help_entry_vec_t{
                    {std::string{"-"} + short_opt + "|--" + long_opt_cstr +
                         param_opt_postfix,
                     opt_desc_str + " (" + purpose_optional_str + ", " +
                         vector_opt_desc_restriction + ")"}}));
}

TEST_F(optionT, vectorPathParamOptionConstructLongStringAndCallLongOpt) {
    clapp::option::vector_path_param_option_t opt{tp, long_opt_str,
                                                  opt_desc_str};
    ASSERT_THAT(opt, VectorParamOptionNotGiven());
    ASSERT_NO_THROW(
        (get_long_opt_func<option_test_parser_t::long_opt_param_func_t>(
            tp, long_opt_str)(long_opt_str, value_cstr_path)));
    ASSERT_THAT(opt, VectorParamOptionGiven(std::vector<std::string>{
                         std::string{value_cstr_path}}));
}

TEST_F(optionT, vectorPathParamOptionConstructShortAndCallShortOpt) {
    clapp::option::vector_path_param_option_t opt{tp, short_opt, opt_desc_str};
    ASSERT_THAT(opt, VectorParamOptionNotGiven());
    ASSERT_NO_THROW(
        (get_short_opt_func<option_test_parser_t::short_opt_param_func_t>(
            tp, short_opt)(short_opt, value_str_path)));
    ASSERT_THAT(
        opt, VectorParamOptionGiven(std::vector<std::string>{value_str_path}));
}

TEST_F(
    optionT,
    vectorPathParamOptionConstructShortAndLongCStringAndCallShortAndLongCStringOpt) {
    clapp::option::vector_path_param_option_t opt{tp, long_opt_cstr, short_opt,
                                                  opt_desc_str};
    ASSERT_THAT(opt, VectorParamOptionNotGiven());
    ASSERT_NO_THROW(
        (get_short_opt_func<option_test_parser_t::short_opt_param_func_t>(
            tp, short_opt)(short_opt, value_str_path)));
    ASSERT_NO_THROW(
        (get_long_opt_func<option_test_parser_t::long_opt_param_func_t>(
            tp, long_opt_cstr)(long_opt_cstr, value_cstr_path)));
    ASSERT_THAT(opt, VectorParamOptionGiven(std::vector<std::string>{
                         value_str_path, std::string{value_cstr_path}}));
}

TEST_F(
    optionT,
    vectorPathParamOptionConstructShortLongStringAndLongCStringAndCallShortLongStringAndLongCStringOpt) {
    clapp::option::vector_path_param_option_t opt{
        tp, std::vector<std::string>{long_opt_str, long_opt_cstr}, short_opt,
        opt_desc_str};
    ASSERT_THAT(opt, VectorParamOptionNotGiven());
    ASSERT_NO_THROW(
        (get_short_opt_func<option_test_parser_t::short_opt_param_func_t>(
            tp, short_opt)(short_opt, value_str_path)));
    ASSERT_NO_THROW(
        (get_long_opt_func<option_test_parser_t::long_opt_param_func_t>(
            tp, long_opt_str)(long_opt_str, value_cstr_path)));
    ASSERT_NO_THROW(
        (get_long_opt_func<option_test_parser_t::long_opt_param_func_t>(
            tp, long_opt_cstr)(long_opt_cstr, value_cstr_path)));
    ASSERT_NO_THROW(
        (get_long_opt_func<option_test_parser_t::long_opt_param_func_t>(
            tp, long_opt_str)(long_opt_str, value_str_path)));
    ASSERT_THAT(opt, VectorParamOptionGiven(std::vector<std::string>{
                         value_str_path, std::string{value_cstr_path},
                         std::string{value_cstr_path}, value_str_path}));
}

TEST_F(
    optionT,
    vectorPathParamOptionConstructLongStringAndShortAndCallShortOptCallsFoundFunc) {
    clapp::option::vector_path_param_option_t opt{
        tp, long_opt_str, short_opt, opt_desc_str,
        clapp::value::found_func_t{[this]() { found_func_called++; }}};
    ASSERT_NO_THROW(
        (get_short_opt_func<option_test_parser_t::short_opt_param_func_t>(
            tp, short_opt)(short_opt, value_str_path)));

    constexpr std::uint32_t expected_found_func_called{1U};
    ASSERT_THAT(found_func_called, testing::Eq(expected_found_func_called));
}

TEST_F(optionT,
       vectorPathParamOptionConstructMandatoryLongCStringAndCallValidateFunc) {
    clapp::option::vector_path_param_option_t opt{
        tp, long_opt_cstr, opt_desc_str,
        option_test_parser_t::purpose_t::mandatory};
    std::optional<option_test_parser_t::validate_func_t> option_validate_func{
        get_validate_func(tp, long_opt_cstr)};
    ASSERT_THAT(option_validate_func, testing::Ne(std::nullopt));
    ASSERT_THROW(option_validate_func.value()(),
                 clapp::exception::option_param_exception_t);
    get_long_opt_func<option_test_parser_t::long_opt_param_func_t>(
        tp, long_opt_cstr)(long_opt_cstr, value_str_path);
    ASSERT_NO_THROW(option_validate_func.value()());
}
#endif

TEST_F(
    optionT,
    vectorInt64ParamOptionConstructLongStringAndShortWithDefaultValueThrows) {
    ASSERT_THROW((clapp::option::vector_int64_param_option_t{
                     tp, long_opt_str, short_opt, opt_desc_str,
                     clapp::value::default_value_t<std::int64_t>{value_int64}}),
                 clapp::exception::option_param_exception_t);
}

TEST_F(optionT, vectorInt64ParamOptionConstructShort) {
    clapp::option::vector_int64_param_option_t opt{tp, short_opt, opt_desc_str};
    ASSERT_THAT(tp, ContainsShortOption(short_opt));
    ASSERT_THAT(opt, VectorParamOptionNotGiven());
}

TEST_F(optionT, vectorInt64ParamOptionConstructLongString) {
    clapp::option::vector_int64_param_option_t opt{tp, long_opt_str,
                                                   opt_desc_str};
    ASSERT_THAT(tp, ContainsLongOption(long_opt_str));
    ASSERT_THAT(opt, VectorParamOptionNotGiven());
}

TEST_F(optionT, vectorInt64ParamOptionConstructLongCString) {
    clapp::option::vector_int64_param_option_t opt{tp, long_opt_cstr,
                                                   opt_desc_cstr};
    ASSERT_THAT(tp, ContainsLongOption(long_opt_cstr));
    ASSERT_THAT(opt, VectorParamOptionNotGiven());
}

TEST_F(optionT, vectorInt64ParamOptionConstructShortVec) {
    clapp::option::vector_int64_param_option_t opt{
        tp, std::vector<char>{short_opt}, opt_desc_str};
    ASSERT_THAT(tp, ContainsShortOption(short_opt));
    ASSERT_THAT(opt, VectorParamOptionNotGiven());
}

TEST_F(optionT, vectorInt64ParamOptionConstructLongStringVec) {
    clapp::option::vector_int64_param_option_t opt{
        tp, std::vector<std::string>{long_opt_str, long_opt_cstr},
        opt_desc_str};
    ASSERT_THAT(tp, ContainsLongOption(long_opt_str));
    ASSERT_THAT(tp, ContainsLongOption(long_opt_cstr));
    ASSERT_THAT(opt, VectorParamOptionNotGiven());
}

TEST_F(optionT, vectorInt64ParamOptionConstructLongOptionTwiceThrows) {
    clapp::option::vector_int64_param_option_t opt{tp, long_opt_str,
                                                   opt_desc_str};
    ASSERT_THROW((clapp::option::vector_int64_param_option_t{tp, long_opt_str,
                                                             opt_desc_str}),
                 clapp::exception::option_exception_t);
}

TEST_F(optionT, vectorInt64ParamOptionConstructShortOptionTwiceThrows) {
    clapp::option::vector_int64_param_option_t opt{tp, short_opt, opt_desc_str};
    ASSERT_THROW((clapp::option::vector_int64_param_option_t{tp, short_opt,
                                                             opt_desc_str}),
                 clapp::exception::option_exception_t);
}

TEST_F(optionT,
       vectorInt64ParamOptionConstructLongStringVecAndCallGetOptionHelp) {
    clapp::option::vector_int64_param_option_t opt{
        tp, std::vector<std::string>{long_opt_str, long_opt_cstr},
        opt_desc_str};
    ASSERT_THAT(
        tp.get_option_help(),
        testing::ContainerEq(option_test_parser_t::help_entry_vec_t{
            {"--" + long_opt_str + "|--" + long_opt_cstr + param_opt_postfix,
             opt_desc_str + " (" + purpose_optional_str + ", " +
                 vector_opt_desc_restriction + ")"}}));
}

TEST_F(optionT, vectorInt64ParamOptionConstructShortVecAndCallGetOptionHelp) {
    clapp::option::vector_int64_param_option_t opt{
        tp, std::vector<char>{short_opt}, opt_desc_str};
    ASSERT_THAT(tp.get_option_help(),
                testing::ContainerEq(option_test_parser_t::help_entry_vec_t{
                    {std::string{"-"} + short_opt + param_opt_postfix,
                     opt_desc_str + " (" + purpose_optional_str + ", " +
                         vector_opt_desc_restriction + ")"}}));
}

TEST_F(
    optionT,
    vectorInt64ParamOptionConstructLongStringVecAndShortVecAndCallGetOptionHelp) {
    clapp::option::vector_int64_param_option_t opt{
        tp, std::vector<std::string>{long_opt_str, long_opt_cstr},
        std::vector<char>{short_opt}, opt_desc_str,
        clapp::basic_parser_t::purpose_t::mandatory};
    ASSERT_THAT(tp.get_option_help(),
                testing::ContainerEq(option_test_parser_t::help_entry_vec_t{
                    {std::string{"-"} + short_opt + "|--" + long_opt_str +
                         "|--" + long_opt_cstr + param_opt_postfix,
                     opt_desc_str + " (" + purpose_mandatory_str + ", " +
                         vector_opt_desc_restriction + ")"}}));
}

TEST_F(optionT,
       vectorInt64ParamOptionConstructLongStringAndShortAndCallGetOptionHelp) {
    clapp::option::vector_int64_param_option_t opt{tp, long_opt_str, short_opt,
                                                   opt_desc_str};
    ASSERT_THAT(tp.get_option_help(),
                testing::ContainerEq(option_test_parser_t::help_entry_vec_t{
                    {std::string{"-"} + short_opt + "|--" + long_opt_str +
                         param_opt_postfix,
                     opt_desc_str + " (" + purpose_optional_str + ", " +
                         vector_opt_desc_restriction + ")"}}));
}

TEST_F(optionT,
       vectorInt64ParamOptionConstructLongCStringAndShortAndCallGetOptionHelp) {
    clapp::option::vector_int64_param_option_t opt{tp, long_opt_cstr, short_opt,
                                                   opt_desc_str};
    ASSERT_THAT(tp.get_option_help(),
                testing::ContainerEq(option_test_parser_t::help_entry_vec_t{
                    {std::string{"-"} + short_opt + "|--" + long_opt_cstr +
                         param_opt_postfix,
                     opt_desc_str + " (" + purpose_optional_str + ", " +
                         vector_opt_desc_restriction + ")"}}));
}

TEST_F(
    optionT,
    vectorInt64ParamOptionConstructLongStringVecAndShortAndCallGetOptionHelp) {
    constexpr std::int64_t min_value{5};
    constexpr std::int64_t max_value{0x17};
    clapp::option::vector_int64_param_option_t opt{
        tp, std::vector<std::string>{long_opt_str}, short_opt, opt_desc_str,
        clapp::value::min_max_value_t<std::int64_t>{min_value, max_value}};
    ASSERT_THAT(tp.get_option_help(),
                testing::ContainerEq(option_test_parser_t::help_entry_vec_t{
                    {std::string{"-"} + short_opt + "|--" + long_opt_str +
                         param_opt_postfix,
                     opt_desc_str + " (" + purpose_optional_str + ", " +
                         vector_opt_desc_restriction + ", " + "constraint: [" +
                         std::to_string(min_value) + "," +
                         std::to_string(max_value) + "])"}}));
}

TEST_F(optionT, vectorInt64ParamOptionConstructLongStringAndCallLongOpt) {
    clapp::option::vector_int64_param_option_t opt{tp, long_opt_str,
                                                   opt_desc_str};
    ASSERT_THAT(opt, VectorParamOptionNotGiven());
    ASSERT_NO_THROW(
        (get_long_opt_func<option_test_parser_t::long_opt_param_func_t>(
            tp, long_opt_str)(long_opt_str, std::to_string(value_int64))));
    ASSERT_THAT(opt,
                VectorParamOptionGiven(std::vector<std::int64_t>{value_int64}));
}

TEST_F(optionT, vectorInt64ParamOptionConstructShortAndCallShortOpt) {
    clapp::option::vector_int64_param_option_t opt{tp, short_opt, opt_desc_str};
    ASSERT_THAT(opt, VectorParamOptionNotGiven());
    ASSERT_NO_THROW(
        (get_short_opt_func<option_test_parser_t::short_opt_param_func_t>(
            tp, short_opt)(short_opt, std::to_string(value_int64))));
    ASSERT_THAT(opt,
                VectorParamOptionGiven(std::vector<std::int64_t>{value_int64}));
}

TEST_F(
    optionT,
    vectorInt64ParamOptionConstructLongStringAndShortAndCallShortOptCallsFoundFunc) {
    clapp::option::vector_int64_param_option_t opt{
        tp, long_opt_str, short_opt, opt_desc_str,
        clapp::value::found_func_t{[this]() { found_func_called++; }}};

    ASSERT_NO_THROW(
        (get_short_opt_func<option_test_parser_t::short_opt_param_func_t>(
            tp, short_opt)(short_opt, std::to_string(value_int64))));
    ASSERT_NO_THROW(
        (get_short_opt_func<option_test_parser_t::short_opt_param_func_t>(
            tp, short_opt)(short_opt, std::to_string(value_int32))));

    constexpr std::uint32_t expected_found_func_called{2U};
    ASSERT_THAT(found_func_called, testing::Eq(expected_found_func_called));
}

TEST_F(
    optionT,
    vectorInt64ParamOptionConstructShortAndCallShortOptFuncWithInvalidValueThrows) {
    clapp::option::vector_int64_param_option_t opt{tp, short_opt, opt_desc_str};
    ASSERT_THROW(
        (get_short_opt_func<option_test_parser_t::short_opt_param_func_t>(
            tp, short_opt)(short_opt, value_str)),
        clapp::exception::invalid_value_t);
}

TEST_F(optionT,
       vectorInt64ParamOptionConstructMandatoryShortAndCallValidateFunc) {
    clapp::option::vector_int64_param_option_t opt{
        tp, short_opt, opt_desc_str,
        option_test_parser_t::purpose_t::mandatory};
    std::optional<option_test_parser_t::validate_func_t> option_validate_func{
        get_validate_func(tp, short_opt)};
    ASSERT_THAT(option_validate_func, testing::Ne(std::nullopt));
    ASSERT_THROW(option_validate_func.value()(),
                 clapp::exception::option_param_exception_t);
    ASSERT_NO_THROW(
        (get_short_opt_func<option_test_parser_t::short_opt_param_func_t>(
            tp, short_opt)(short_opt, std::to_string(value_int64))));
    ASSERT_NO_THROW(option_validate_func.value()());
}

TEST_F(
    optionT,
    vectorInt64ParamOptionConstructLongStringVecAndShortWithMinMaxConstraintAndCallLongOptFuncAndShortOptFunc) {
    constexpr std::int64_t min_value{0};
    constexpr std::int64_t max_value{0x20};
    clapp::option::vector_int64_param_option_t opt{
        tp, std::vector<std::string>{long_opt_str}, short_opt, opt_desc_str,
        clapp::value::min_max_value_t<std::int64_t>{min_value, max_value}};
    std::optional<option_test_parser_t::validate_func_t> option_validate_func{
        get_validate_func(tp, long_opt_str)};
    ASSERT_THAT(option_validate_func, testing::Ne(std::nullopt));
    get_long_opt_func<option_test_parser_t::long_opt_param_func_t>(
        tp, long_opt_str)(long_opt_str, std::to_string(value_int32));
    ASSERT_THROW((option_validate_func.value()()),
                 clapp::exception::out_of_range_t);
}

TEST_F(optionT,
       vectorInt64ParamOptionConstructLongStringAndShortAndCallShortOptFunc) {
    clapp::option::vector_int64_param_option_t opt{tp, long_opt_str, short_opt,
                                                   opt_desc_str};
    ASSERT_THAT(opt, VectorParamOptionNotGiven());
    ASSERT_NO_THROW(
        (get_short_opt_func<option_test_parser_t::short_opt_param_func_t>(
            tp, short_opt)(short_opt, std::to_string(value_int64))));
    ASSERT_THAT(opt,
                VectorParamOptionGiven(std::vector<std::int64_t>{value_int64}));
}

TEST_F(optionT,
       vectorInt32ParamOptionConstructLongStringAndShortAndCallShortOpt) {
    clapp::option::vector_int32_param_option_t opt{tp, long_opt_str, short_opt,
                                                   opt_desc_str};
    ASSERT_THAT(opt, VectorParamOptionNotGiven());
    ASSERT_NO_THROW(
        (get_short_opt_func<option_test_parser_t::short_opt_param_func_t>(
            tp, short_opt)(short_opt, std::to_string(value_int32))));
    ASSERT_THAT(opt,
                VectorParamOptionGiven(std::vector<std::int32_t>{value_int32}));
}

TEST_F(optionT,
       vectorInt16ParamOptionConstructLongStringAndShortAndCallShortOpt) {
    clapp::option::vector_int16_param_option_t opt{tp, long_opt_str, short_opt,
                                                   opt_desc_str};
    ASSERT_THAT(opt, VectorParamOptionNotGiven());
    ASSERT_NO_THROW(
        (get_short_opt_func<option_test_parser_t::short_opt_param_func_t>(
            tp, short_opt)(short_opt, std::to_string(value_int16))));
    ASSERT_THAT(opt,
                VectorParamOptionGiven(std::vector<std::int16_t>{value_int16}));
}

TEST_F(optionT,
       vectorInt8ParamOptionConstructLongStringAndShortAndCallShortOpt) {
    clapp::option::vector_int8_param_option_t opt{tp, long_opt_str, short_opt,
                                                  opt_desc_str};
    ASSERT_THAT(opt, VectorParamOptionNotGiven());
    ASSERT_NO_THROW(
        (get_short_opt_func<option_test_parser_t::short_opt_param_func_t>(
            tp, short_opt)(short_opt, std::to_string(value_int8))));
    ASSERT_THAT(opt,
                VectorParamOptionGiven(std::vector<std::int8_t>{value_int8}));
}

TEST_F(optionT,
       vectorUint64ParamOptionConstructLongStringAndShortAndCallShortOptFunc) {
    clapp::option::vector_uint64_param_option_t opt{tp, long_opt_str, short_opt,
                                                    opt_desc_str};
    ASSERT_THAT(opt, VectorParamOptionNotGiven());
    ASSERT_NO_THROW(
        (get_short_opt_func<option_test_parser_t::short_opt_param_func_t>(
            tp, short_opt)(short_opt, std::to_string(value_uint64))));
    ASSERT_THAT(
        opt, VectorParamOptionGiven(std::vector<std::uint64_t>{value_uint64}));
}

TEST_F(optionT,
       vectorUint32ParamOptionConstructLongStringAndShortAndCallShortOpt) {
    clapp::option::vector_uint32_param_option_t opt{tp, long_opt_str, short_opt,
                                                    opt_desc_str};
    ASSERT_THAT(opt, VectorParamOptionNotGiven());
    ASSERT_NO_THROW(
        (get_short_opt_func<option_test_parser_t::short_opt_param_func_t>(
            tp, short_opt)(short_opt, std::to_string(value_uint32))));
    ASSERT_THAT(
        opt, VectorParamOptionGiven(std::vector<std::uint32_t>{value_uint32}));
}

TEST_F(optionT,
       vectorUint16ParamOptionConstructLongStringAndShortAndCallShortOpt) {
    clapp::option::vector_uint16_param_option_t opt{tp, long_opt_str, short_opt,
                                                    opt_desc_str};
    ASSERT_THAT(opt, VectorParamOptionNotGiven());
    ASSERT_NO_THROW(
        (get_short_opt_func<option_test_parser_t::short_opt_param_func_t>(
            tp, short_opt)(short_opt, std::to_string(value_uint16))));
    ASSERT_THAT(
        opt, VectorParamOptionGiven(std::vector<std::uint16_t>{value_uint16}));
}

TEST_F(optionT,
       vectorUint8ParamOptionConstructLongStringAndShortAndCallShortOpt) {
    clapp::option::vector_uint8_param_option_t opt{tp, long_opt_str, short_opt,
                                                   opt_desc_str};
    ASSERT_THAT(opt, VectorParamOptionNotGiven());
    ASSERT_NO_THROW(
        (get_short_opt_func<option_test_parser_t::short_opt_param_func_t>(
            tp, short_opt)(short_opt, std::to_string(value_uint8))));
    ASSERT_THAT(opt,
                VectorParamOptionGiven(std::vector<std::uint8_t>{value_uint8}));
}

TEST_F(optionT,
       vectorSizeParamOptionConstructLongStringAndShortAndCallShortOpt) {
    clapp::option::vector_size_param_option_t opt{tp, long_opt_str, short_opt,
                                                  opt_desc_str};
    ASSERT_THAT(opt, VectorParamOptionNotGiven());
    ASSERT_NO_THROW(
        (get_short_opt_func<option_test_parser_t::short_opt_param_func_t>(
            tp, short_opt)(short_opt, std::to_string(value_size))));
    ASSERT_THAT(opt,
                VectorParamOptionGiven(std::vector<std::size_t>{value_size}));
}

TEST_F(optionT,
       vectorPtrdiffParamOptionConstructLongStringAndShortAndCallShortOpt) {
    clapp::option::vector_ptrdiff_param_option_t opt{tp, long_opt_str,
                                                     short_opt, opt_desc_str};
    ASSERT_THAT(opt, VectorParamOptionNotGiven());
    ASSERT_NO_THROW(
        (get_short_opt_func<option_test_parser_t::short_opt_param_func_t>(
            tp, short_opt)(short_opt, std::to_string(value_ptrdiff))));
    ASSERT_THAT(opt, VectorParamOptionGiven(
                         std::vector<std::ptrdiff_t>{value_ptrdiff}));
}

TEST_F(optionT,
       vectorDoubleParamOptionConstructLongStringAndShortAndCallShortOpt) {
    clapp::option::vector_double_param_option_t opt{tp, long_opt_str, short_opt,
                                                    opt_desc_str};
    ASSERT_THAT(opt, VectorParamOptionNotGiven());
    ASSERT_NO_THROW(
        (get_short_opt_func<option_test_parser_t::short_opt_param_func_t>(
            tp, short_opt)(short_opt, std::to_string(value_double))));
    ASSERT_THAT(opt, VectorParamOptionGiven(std::vector<double>{value_double}));
}

TEST_F(optionT,
       vectorFloatParamOptionConstructLongStringAndShortAndCallShortOpt) {
    clapp::option::vector_float_param_option_t opt{tp, long_opt_str, short_opt,
                                                   opt_desc_str};
    ASSERT_THAT(opt, VectorParamOptionNotGiven());
    ASSERT_NO_THROW(
        (get_short_opt_func<option_test_parser_t::short_opt_param_func_t>(
            tp, short_opt)(short_opt, std::to_string(value_float))));
    ASSERT_THAT(opt, VectorParamOptionGiven(std::vector<float>{value_float}));
}

TEST_F(optionT, vectorNsParamOptionConstructLongStringAndShortAndCallShortOpt) {
    clapp::option::vector_ns_param_option_t opt{tp, long_opt_str, short_opt,
                                                opt_desc_str};
    ASSERT_THAT(opt, VectorParamOptionNotGiven());
    ASSERT_NO_THROW(
        (get_short_opt_func<option_test_parser_t::short_opt_param_func_t>(
            tp, short_opt)(short_opt, std::to_string(value_ns.count()))));
    ASSERT_THAT(opt, VectorParamOptionGiven(
                         std::vector<std::chrono::nanoseconds>{value_ns}));
}

TEST_F(optionT, vectorUsParamOptionConstructLongStringAndShortAndCallShortOpt) {
    clapp::option::vector_us_param_option_t opt{tp, long_opt_str, short_opt,
                                                opt_desc_str};
    ASSERT_THAT(opt, VectorParamOptionNotGiven());
    ASSERT_NO_THROW(
        (get_short_opt_func<option_test_parser_t::short_opt_param_func_t>(
            tp, short_opt)(short_opt, std::to_string(value_us.count()))));
    ASSERT_THAT(opt, VectorParamOptionGiven(
                         std::vector<std::chrono::microseconds>{value_us}));
}

TEST_F(optionT, vectorMsParamOptionConstructLongStringAndShortAndCallShortOpt) {
    clapp::option::vector_ms_param_option_t opt{tp, long_opt_str, short_opt,
                                                opt_desc_str};
    ASSERT_THAT(opt, VectorParamOptionNotGiven());
    ASSERT_NO_THROW(
        (get_short_opt_func<option_test_parser_t::short_opt_param_func_t>(
            tp, short_opt)(short_opt, std::to_string(value_ms.count()))));
    ASSERT_THAT(opt, VectorParamOptionGiven(
                         std::vector<std::chrono::milliseconds>{value_ms}));
}

TEST_F(optionT,
       vectorSecParamOptionConstructLongStringAndShortAndCallShortOpt) {
    clapp::option::vector_sec_param_option_t opt{tp, long_opt_str, short_opt,
                                                 opt_desc_str};
    ASSERT_THAT(opt, VectorParamOptionNotGiven());
    ASSERT_NO_THROW(
        (get_short_opt_func<option_test_parser_t::short_opt_param_func_t>(
            tp, short_opt)(short_opt, std::to_string(value_sec.count()))));
    ASSERT_THAT(opt, VectorParamOptionGiven(
                         std::vector<std::chrono::seconds>{value_sec}));
}

TEST_F(optionT,
       vectorMinParamOptionConstructLongStringAndShortAndCallShortOpt) {
    clapp::option::vector_min_param_option_t opt{tp, long_opt_str, short_opt,
                                                 opt_desc_str};
    ASSERT_THAT(opt, VectorParamOptionNotGiven());
    ASSERT_NO_THROW(
        (get_short_opt_func<option_test_parser_t::short_opt_param_func_t>(
            tp, short_opt)(short_opt, std::to_string(value_min.count()))));
    ASSERT_THAT(opt, VectorParamOptionGiven(
                         std::vector<std::chrono::minutes>{value_min}));
}

TEST_F(optionT,
       vectorHoursParamOptionConstructLongStringAndShortAndCallShortOpt) {
    clapp::option::vector_hours_param_option_t opt{tp, long_opt_str, short_opt,
                                                   opt_desc_str};
    ASSERT_THAT(opt, VectorParamOptionNotGiven());
    ASSERT_NO_THROW(
        (get_short_opt_func<option_test_parser_t::short_opt_param_func_t>(
            tp, short_opt)(short_opt, std::to_string(value_hours.count()))));
    ASSERT_THAT(opt, VectorParamOptionGiven(
                         std::vector<std::chrono::hours>{value_hours}));
}

TEST_F(optionT, invalidLongOptionConstruct) {
    const std::string long_opt1{"long\n"};
    const std::string long_opt2{"long\t"};
    const std::string long_opt3{"long\r"};
    const std::string long_opt4{"long "};
    const std::string long_opt5{"long ="};

    ASSERT_THROW((clapp::option::bool_option_t{tp, long_opt1, opt_desc_str}),
                 clapp::exception::option_exception_t);
    ASSERT_THROW(
        (clapp::option::bool_option_t{tp, long_opt1, short_opt, opt_desc_str}),
        clapp::exception::option_exception_t);

    ASSERT_THROW((clapp::option::bool_option_t{tp, long_opt2, opt_desc_str}),
                 clapp::exception::option_exception_t);
    ASSERT_THROW(
        (clapp::option::bool_option_t{tp, long_opt2, short_opt, opt_desc_str}),
        clapp::exception::option_exception_t);

    ASSERT_THROW((clapp::option::bool_option_t{tp, long_opt3, opt_desc_str}),
                 clapp::exception::option_exception_t);
    ASSERT_THROW(
        (clapp::option::bool_option_t{tp, long_opt3, short_opt, opt_desc_str}),
        clapp::exception::option_exception_t);

    ASSERT_THROW((clapp::option::vector_string_param_option_t{tp, long_opt4,
                                                              opt_desc_str}),
                 clapp::exception::option_exception_t);
    ASSERT_THROW((clapp::option::vector_string_param_option_t{
                     tp, long_opt4, short_opt, opt_desc_str}),
                 clapp::exception::option_exception_t);

    ASSERT_THROW(
        (clapp::option::int32_param_option_t{tp, long_opt5, opt_desc_str}),
        clapp::exception::option_exception_t);
    ASSERT_THROW((clapp::option::int32_param_option_t{tp, long_opt5, short_opt,
                                                      opt_desc_str}),
                 clapp::exception::option_exception_t);
}

TEST_F(optionT, invalidShortOptionConstruct) {
    const char short_opt1{'\n'};
    const char short_opt2{'\t'};
    const char short_opt3{'\r'};
    const char short_opt4{' '};
    const char short_opt5{'='};

    ASSERT_THROW((clapp::option::bool_option_t{tp, short_opt1, opt_desc_str}),
                 clapp::exception::option_exception_t);
    ASSERT_THROW((clapp::option::bool_option_t{tp, long_opt_str, short_opt1,
                                               opt_desc_str}),
                 clapp::exception::option_exception_t);

    ASSERT_THROW((clapp::option::count_option_t{tp, short_opt2, opt_desc_str}),
                 clapp::exception::option_exception_t);
    ASSERT_THROW((clapp::option::count_option_t{tp, long_opt_str, short_opt2,
                                                opt_desc_str}),
                 clapp::exception::option_exception_t);

    ASSERT_THROW(
        (clapp::option::string_param_option_t{tp, short_opt3, opt_desc_str}),
        clapp::exception::option_exception_t);
    ASSERT_THROW((clapp::option::string_param_option_t{
                     tp, long_opt_str, short_opt3, opt_desc_str}),
                 clapp::exception::option_exception_t);

    ASSERT_THROW((clapp::option::vector_string_param_option_t{tp, short_opt4,
                                                              opt_desc_str}),
                 clapp::exception::option_exception_t);
    ASSERT_THROW((clapp::option::vector_string_param_option_t{
                     tp, long_opt_str, short_opt4, opt_desc_str}),
                 clapp::exception::option_exception_t);

    ASSERT_THROW(
        (clapp::option::int32_param_option_t{tp, short_opt5, opt_desc_str}),
        clapp::exception::option_exception_t);
    ASSERT_THROW((clapp::option::int32_param_option_t{
                     tp, long_opt_str, short_opt5, opt_desc_str}),
                 clapp::exception::option_exception_t);
}

TEST_F(optionT, genOptValidateFuncAndCallValidateFuncThrows) {
    std::optional<option_test_parser_t::validate_func_t> validate_func{
        clapp::option::gen_opt_validate_func<std::int32_t, int32_value_func_t>(
            std::nullopt, []() { return throw_unexpected_call(); },
            []() { return false; }, std::vector<int32_validate_func_t>{},
            "option string", option_test_parser_t::purpose_t::mandatory)};
    ASSERT_THAT(validate_func.has_value(), testing::Eq(true));
    ASSERT_THROW((*validate_func)(), clapp::exception::option_exception_t);
}

TEST_F(optionT, genOptValidateFuncAndCallValidateFuncDoesntThrow) {
    std::optional<option_test_parser_t::validate_func_t> validate_func{
        clapp::option::gen_opt_validate_func<std::int32_t, int32_value_func_t>(
            std::nullopt, []() { return throw_unexpected_call(); },
            []() { return true; }, std::vector<int32_validate_func_t>{},
            "option string", option_test_parser_t::purpose_t::mandatory)};
    ASSERT_THAT(validate_func.has_value(), testing::Eq(true));
    ASSERT_NO_THROW((*validate_func)());
}

TEST_F(optionT, genOptValidateFuncWithoutValueFuncAndCallValidateFuncThrows) {
    constexpr std::int32_t return_value{10};
    std::optional<option_test_parser_t::validate_func_t> validate_func{
        clapp::option::gen_opt_validate_func<std::int32_t, int32_value_func_t>(
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

TEST(option, genOptValidateFuncAndCallValidateFuncThrows) {
    using int32_value_func_t = std::function<std::int32_t(void)>;
    using int32_validate_func_t = std::function<void(
        const std::int32_t&, const std::string& option_string)>;
    std::optional<option_test_parser_t::validate_func_t> validate_func{
        clapp::option::gen_opt_validate_func<std::int32_t, int32_value_func_t>(
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

TEST_F(optionT,
       genOptValidateFuncWithoutHasValueFuncAndCallValidateFuncDoesntThrow) {
    constexpr std::int32_t return_value{10};
    std::optional<option_test_parser_t::validate_func_t> validate_func{
        clapp::option::gen_opt_validate_func<std::int32_t, int32_value_func_t>(
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
