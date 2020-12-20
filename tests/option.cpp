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
            [&option](auto&& o) -> bool {
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
    if (options.size() == 0) {
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

MATCHER(BoolOptionNotGiven, "") {
    *result_listener << "bool-option: "
                     << (static_cast<bool>(arg) ? "true" : "false")
                     << ", has_value(): "
                     << (arg.has_value() ? "true" : "false")
                     << ", given(): " << (arg.given() ? "true" : "false")
                     << ", value(): " << (arg.value() ? "true" : "false");
    return static_cast<bool>(arg) == true && arg.has_value() == true &&
           arg.given() == false && arg.value() == false;
}

MATCHER(BoolOptionGiven, "") {
    *result_listener << "bool-option: "
                     << (static_cast<bool>(arg) ? "true" : "false")
                     << ", has_value(): "
                     << (arg.has_value() ? "true" : "false")
                     << ", given(): " << (arg.given() ? "true" : "false")
                     << ", value(): " << (arg.value() ? "true" : "false");
    return static_cast<bool>(arg) == true && arg.has_value() == true &&
           arg.given() == true && arg.value() == true;
}

MATCHER_P(NumCountOptionGiven, count, "") {
    *result_listener << "count-option: "
                     << (static_cast<bool>(arg) ? "true" : "false")
                     << ", has_value(): "
                     << (arg.has_value() ? "true" : "false")
                     << ", given(): " << (arg.given() ? "true" : "false")
                     << ", value(): " << count << "==" << arg.value() << " = "
                     << (arg.value() == count ? "true" : "false");
    return static_cast<bool>(arg) == true && arg.has_value() == true &&
           arg.given() == true && arg.value() == count;
}

MATCHER_P(NumCountOptionNotGiven, count, "") {
    *result_listener << "count-option: "
                     << (static_cast<bool>(arg) ? "true" : "false")
                     << ", has_value(): "
                     << (arg.has_value() ? "true" : "false")
                     << ", given(): " << (arg.given() ? "true" : "false")
                     << ", value(): " << count << "==" << arg.value() << " = "
                     << (arg.value() == count ? "true" : "false");
    return static_cast<bool>(arg) == true && arg.has_value() == true &&
           arg.given() == false && arg.value() == count;
}

template <typename ARG, typename T>
bool compare_value(const ARG& arg, T value,
                   ::testing::MatchResultListener* result_listener) {
    if constexpr (std::is_same<T, double>::value) {
        ::testing::Matcher<double> matcher{
            ::testing::DoubleNear(value, 0.0001)};
        ::testing::StringMatchResultListener smrl;
        if (!matcher.MatchAndExplain(arg, &smrl)) {
            *result_listener << "false (" << smrl.str() << ")";
            return false;
        }
        *result_listener << "true (approx)";
        return true;
    } else if constexpr (std::is_same<T, float>::value) {
        ::testing::Matcher<float> matcher{::testing::FloatNear(value, 0.01f)};
        ::testing::StringMatchResultListener smrl;
        if (!matcher.MatchAndExplain(arg, &smrl)) {
            *result_listener << "false (" << smrl.str() << ")";
            return false;
        }
        *result_listener << "true (approx)";
        return true;
    } else {
        *result_listener << (arg == value ? "true" : "false");
        return arg == value;
    }
}

template <typename ARG, typename T>
bool compare_value_vector(const std::vector<ARG>& arg, std::vector<T> value,
                          ::testing::MatchResultListener* result_listener) {
    if (arg.size() != value.size()) {
        *result_listener
            << "false (actual and expected vectors do not match in size (" +
                   std::to_string(arg.size()) + " vs " +
                   std::to_string(arg.size()) + ")).";
        return false;
    }
    for (std::size_t i{0}; i < arg.size(); i++) {
        if (!compare_value(arg[i], value[i], result_listener)) {
            return false;
        }
    }
    return true;
}

MATCHER_P(ParamOptionGiven, value, "") {
    *result_listener << "param-option: "
                     << (static_cast<bool>(arg) ? "true" : "false")
                     << ", has_value(): "
                     << (arg.has_value() ? "true" : "false")
                     << ", given(): " << (arg.given() ? "true" : "false");
    if (!(static_cast<bool>(arg) == true && arg.has_value() == true &&
          arg.given() == true)) {
        return false;
    }
    *result_listener << ", value(): " << clapp::to_string(value)
                     << "==" << clapp::to_string(arg.value()) << " = ";
    return compare_value(arg.value(), value, result_listener);
}

MATCHER_P(ParamOptionNotGivenDefaultValue, default_value, "") {
    *result_listener << "param-option: "
                     << (static_cast<bool>(arg) ? "true" : "false")
                     << ", has_value(): "
                     << (arg.has_value() ? "true" : "false")
                     << ", given(): " << (arg.given() ? "true" : "false");
    if (!(static_cast<bool>(arg) == true && arg.has_value() == true &&
          arg.given() == false)) {
        return false;
    }
    *result_listener << ", value(): " << clapp::to_string(default_value)
                     << "==" << clapp::to_string(arg.value()) << " = ";
    return compare_value(arg.value(), default_value, result_listener);
}

MATCHER(ParamOptionNotGiven, "") {
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
    return static_cast<bool>(arg) == false && arg.has_value() == false &&
           arg.given() == false && caught_value_undefined;
}

MATCHER_P(VectorParamOptionGiven, value, "") {
    *result_listener << "param-option: "
                     << (static_cast<bool>(arg) ? "true" : "false")
                     << ", has_value(): "
                     << (arg.has_value() ? "true" : "false")
                     << ", given(): " << (arg.given() ? "true" : "false");
    if (!(static_cast<bool>(arg) == true && arg.has_value() == true &&
          arg.given() == true)) {
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

MATCHER(VectorParamOptionNotGiven, "") {
    *result_listener << "vector-param-option: "
                     << (static_cast<bool>(arg) ? "true" : "false")
                     << ", has_value(): "
                     << (arg.has_value() ? "true" : "false")
                     << ", given(): " << (arg.given() ? "true" : "false");
    if (!(static_cast<bool>(arg) == false && arg.has_value() == false &&
          arg.given() == false)) {
        return false;
    }
    *result_listener << ", size(): 0==" << clapp::to_string(arg.value().size())
                     << " = " << (arg.value().size() == 0 ? "true" : "false");
    return arg.value().size() == 0;
}

MATCHER_P(ContainsLongOption, option, "") {
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
        [this](auto&& opt) -> bool { return opt.contains_option(option); },
        found_opt.value());
}

MATCHER_P(ContainsShortOption, option, "") {
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
        [this](auto&& opt) -> bool { return opt.contains_option(option); },
        found_opt.value());
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
        [&long_opt_name](auto&& option) -> LONG_OPT_FUNC_T {
            for (auto& opt : option.long_options) {
                if (opt.option == long_opt_name) {
                    if constexpr (std::is_same<LONG_OPT_FUNC_T,
                                               decltype(opt.func)>::value) {
                        return opt.func;
                    } else {
                        throw std::runtime_error(
                            "unexpected long_options-func-type");
                    }
                }
            }
            throw std::runtime_error(
                "found long-opt-func, but iterate failed... wtf");
        },
        found_opt.value());
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
        [&short_opt_name](auto&& option) -> SHORT_OPT_FUNC_T {
            for (auto& opt : option.short_options) {
                if (opt.option == short_opt_name) {
                    if constexpr (std::is_same<SHORT_OPT_FUNC_T,
                                               decltype(opt.func)>::value) {
                        return opt.func;
                    } else {
                        throw std::runtime_error(
                            "unexpected short_options-func-type");
                    }
                }
            }
            throw std::runtime_error(
                "found short-opt-func, but iterate failed... wtf");
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

class option_t : public ::testing::Test {
   protected:
    void SetUp() override {}
    void TearDown() override {}

    using int32_value_func_t = std::function<std::int32_t(void)>;
    using int32_validate_func_t = std::function<void(
        const std::int32_t&, const std::string& option_string)>;

    option_test_parser_t tp;
    inline static const char* param_opt_postfix{"=<param>"};
    inline static const char* vector_opt_desc_restriction{"vector option"};
    inline static const char short_opt{'s'};
    inline static const std::string long_opt_str{"long-str"};
    inline static constexpr const char* long_opt_cstr{"long-cstr"};
    inline static const std::string opt_desc_str{"opt-desc-str"};
    inline static constexpr const char* opt_desc_cstr{"opt-desc-cstr"};
    inline static constexpr const char* value_cstr{"value-cstr"};
    inline static const std::string value_str{"value-str"};
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
    inline static const float value_float{1415.1617f};
    inline static constexpr std::chrono::nanoseconds value_ns{1213};
    inline static constexpr std::chrono::microseconds value_us{121};
    inline static constexpr std::chrono::milliseconds value_ms{12};
    inline static constexpr std::chrono::seconds value_sec{3603};
    inline static constexpr std::chrono::minutes value_min{62};
    inline static constexpr std::chrono::hours value_hours{1};
};

TEST_F(option_t, basicOptionConstructLongAndCallValueThrows) {
    test_option_t opt{tp, long_opt_str, opt_desc_str};
    ASSERT_THROW(opt.value(), clapp::exception::value_undefined_t);
}

TEST_F(option_t, basicOptionConstructShortAndCallValueThrows) {
    test_option_t opt{tp, short_opt, opt_desc_str};
    ASSERT_THROW(opt.value(), clapp::exception::value_undefined_t);
}

TEST_F(option_t, boolOptionConstructShort) {
    clapp::option::bool_option_t opt{tp, short_opt, opt_desc_str};
    ASSERT_THAT(tp, ContainsShortOption(short_opt));
    ASSERT_THAT(opt, BoolOptionNotGiven());
}

TEST_F(option_t, boolOptionConstructLongString) {
    clapp::option::bool_option_t opt{tp, long_opt_str, opt_desc_str};
    ASSERT_THAT(tp, ContainsLongOption(long_opt_str));
    ASSERT_THAT(opt, BoolOptionNotGiven());
}

TEST_F(option_t, boolOptionConstructLongCString) {
    clapp::option::bool_option_t opt{tp, long_opt_cstr, opt_desc_cstr};
    ASSERT_THAT(tp, ContainsLongOption(long_opt_cstr));
    ASSERT_THAT(opt, BoolOptionNotGiven());
}

TEST_F(option_t, boolOptionConstructShortVec) {
    clapp::option::bool_option_t opt{tp, std::vector<char>{short_opt},
                                     opt_desc_str};
    ASSERT_THAT(tp, ContainsShortOption(short_opt));
    ASSERT_THAT(opt, BoolOptionNotGiven());
}

TEST_F(option_t, boolOptionConstructLongStringVec) {
    clapp::option::bool_option_t opt{
        tp, std::vector<std::string>{long_opt_str, long_opt_cstr},
        opt_desc_str};
    ASSERT_THAT(tp, ContainsLongOption(long_opt_str));
    ASSERT_THAT(tp, ContainsLongOption(long_opt_cstr));
    ASSERT_THAT(opt, BoolOptionNotGiven());
}

TEST_F(option_t, boolOptionConstructLongStringVecTwiceThrows) {
    clapp::option::bool_option_t opt{tp, long_opt_str, opt_desc_str};
    ASSERT_THROW((clapp::option::bool_option_t{tp, long_opt_str, opt_desc_str}),
                 clapp::exception::option_exception_t);
}

TEST_F(option_t, boolOptionConstructLongStringVecWithSameOptionThrows) {
    ASSERT_THROW((clapp::option::bool_option_t{
                     tp, std::vector<std::string>{long_opt_str, long_opt_str},
                     opt_desc_str}),
                 clapp::exception::option_exception_t);
}

TEST_F(option_t, boolOptionConstructLongStringVecAndCallGetOptionHelp) {
    clapp::option::bool_option_t opt{
        tp, std::vector<std::string>{long_opt_str, long_opt_cstr},
        opt_desc_str};
    ASSERT_THAT(tp.get_option_help(),
                testing::ContainerEq(option_test_parser_t::help_entry_vec_t{
                    {"--" + long_opt_str + "|--" + long_opt_cstr,
                     opt_desc_str + " (" + purpose_optional_str + ")"}}));
}

TEST_F(option_t, boolOptionConstructShortCharVecWithSameOptionThrows) {
    ASSERT_THROW(
        (clapp::option::bool_option_t{
            tp, std::vector<char>{short_opt, short_opt}, opt_desc_str}),
        clapp::exception::option_exception_t);
}

TEST_F(option_t, boolOptionConstructShortCharVecTwiceThrows) {
    clapp::option::bool_option_t opt{tp, std::vector<char>{short_opt},
                                     opt_desc_str};
    ASSERT_THROW((clapp::option::bool_option_t{tp, std::vector<char>{short_opt},
                                               opt_desc_str}),
                 clapp::exception::option_exception_t);
}

TEST_F(option_t, boolOptionConstructShortVecAndCallGetOptionHelp) {
    clapp::option::bool_option_t opt{
        tp, std::vector<char>{short_opt}, opt_desc_str,
        clapp::basic_parser_t::purpose_t::mandatory};
    ASSERT_THAT(tp.get_option_help(),
                testing::ContainerEq(option_test_parser_t::help_entry_vec_t{
                    {std::string{"-"} + short_opt,
                     opt_desc_str + " (" + purpose_mandatory_str + ")"}}));
}

TEST_F(option_t,
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

TEST_F(option_t, boolOptionConstructLongStringAndShortAndCallGetOptionHelp) {
    clapp::option::bool_option_t opt{
        tp, long_opt_str, short_opt, opt_desc_str,
        clapp::basic_parser_t::purpose_t::mandatory};
    ASSERT_THAT(tp.get_option_help(),
                testing::ContainerEq(option_test_parser_t::help_entry_vec_t{
                    {std::string{"-"} + short_opt + "|--" + long_opt_str,
                     opt_desc_str + " (" + purpose_mandatory_str + ")"}}));
}

TEST_F(option_t, boolOptionConstructLongCStringAndShortAndCallGetOptionHelp) {
    clapp::option::bool_option_t opt{tp, long_opt_cstr, short_opt,
                                     opt_desc_str};
    ASSERT_THAT(tp.get_option_help(),
                testing::ContainerEq(option_test_parser_t::help_entry_vec_t{
                    {std::string{"-"} + short_opt + "|--" + long_opt_cstr,
                     opt_desc_str + " (" + purpose_optional_str + ")"}}));
}

TEST_F(option_t, boolOptionConstructLongStringVecAndCallLongOptFunc) {
    clapp::option::bool_option_t opt{tp, std::vector<std::string>{long_opt_str},
                                     opt_desc_str};
    ASSERT_THAT(opt, BoolOptionNotGiven());

    get_long_opt_func<option_test_parser_t::long_opt_func_t>(
        tp, long_opt_str)(long_opt_str);
    ASSERT_THAT(opt, BoolOptionGiven());
}

TEST_F(option_t, boolOptionConstructShortVecAndCallShortOptFunc) {
    clapp::option::bool_option_t opt{tp, std::vector<char>{short_opt},
                                     opt_desc_str};
    ASSERT_THAT(opt, BoolOptionNotGiven());
    get_short_opt_func<option_test_parser_t::short_opt_func_t>(
        tp, short_opt)(short_opt);
    ASSERT_THAT(opt, BoolOptionGiven());
}

TEST_F(option_t, boolOptionConstructLongStringAndShortAndCallLongOptFunc) {
    clapp::option::bool_option_t opt{tp, long_opt_str, short_opt, opt_desc_str};
    ASSERT_THAT(opt, BoolOptionNotGiven());
    get_long_opt_func<option_test_parser_t::long_opt_func_t>(
        tp, long_opt_str)(long_opt_str);
    ASSERT_THAT(opt, BoolOptionGiven());
}

TEST_F(option_t, boolOptionConstructLongStringAndShortAndCallShortOptFunc) {
    clapp::option::bool_option_t opt{tp, long_opt_str, short_opt, opt_desc_str};
    ASSERT_THAT(opt, BoolOptionNotGiven());
    get_short_opt_func<option_test_parser_t::short_opt_func_t>(
        tp, short_opt)(short_opt);
    ASSERT_THAT(opt, BoolOptionGiven());
}

TEST_F(option_t, twoBoolOptionsConstructAndCallShortOptAndLongOptFunc) {
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

TEST_F(option_t, boolOptionConstructLongStringCallLongOptFuncCallsFoundFunc) {
    std::stringstream ss;
    clapp::option::bool_option_t opt{
        tp, long_opt_str, opt_desc_str,
        clapp::value::found_func_t{[&ss]() { ss << "this is a test"; }}};
    ASSERT_THAT(ss.str().size(), testing::Eq(0));
    get_long_opt_func<option_test_parser_t::long_opt_func_t>(
        tp, long_opt_str)(long_opt_str);
    ASSERT_THAT(ss.str(), testing::StrEq("this is a test"));
}

TEST_F(option_t, boolOptionConstructShortOptionalNoValidateFunc) {
    clapp::option::bool_option_t opt{tp, short_opt, opt_desc_str,
                                     option_test_parser_t::purpose_t::optional};
    ASSERT_THAT(get_validate_func(tp, short_opt), testing::Eq(std::nullopt));
}

TEST_F(option_t, boolOptionConstructLongOptionalNoValidateFunc) {
    clapp::option::bool_option_t opt{tp, long_opt_str, opt_desc_str,
                                     option_test_parser_t::purpose_t::optional};
    ASSERT_THAT(get_validate_func(tp, long_opt_str), testing::Eq(std::nullopt));
}

TEST_F(option_t, boolOptionConstructMandatoryLongCallValidateFunc) {
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

TEST_F(option_t,
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

TEST_F(option_t, helpOptionConstructLongStringAndShortAndCallGetOptionHelp) {
    clapp::option::help_option_t opt{
        tp, long_opt_str, short_opt, opt_desc_str,
        clapp::basic_parser_t::purpose_t::mandatory};
    ASSERT_THAT(tp.get_option_help(),
                testing::ContainerEq(option_test_parser_t::help_entry_vec_t{
                    {std::string{"-"} + short_opt + "|--" + long_opt_str,
                     opt_desc_str + " (" + purpose_mandatory_str + ")"}}));
}

TEST_F(option_t, helpOptionConstructLongCStringAndShortAndCallGetOptionHelp) {
    clapp::option::help_option_t opt{tp, long_opt_cstr, short_opt,
                                     opt_desc_str};
    ASSERT_THAT(tp.get_option_help(),
                testing::ContainerEq(option_test_parser_t::help_entry_vec_t{
                    {std::string{"-"} + short_opt + "|--" + long_opt_cstr,
                     opt_desc_str + " (" + purpose_optional_str + ")"}}));
}

TEST_F(option_t, helpOptionConstructLongStringCallLongOptFunc) {
    clapp::option::help_option_t opt{tp, long_opt_str, opt_desc_str};
    testing::internal::CaptureStdout();
    ASSERT_EXIT((get_long_opt_func<option_test_parser_t::long_opt_func_t>(
                    tp, long_opt_str)(long_opt_str)),
                ::testing::ExitedWithCode(0), "");
    ASSERT_THAT(testing::internal::GetCapturedStdout(),
                testing::HasSubstr("Usage:\n"));
}

TEST_F(option_t, helpOptionConstructLongStringCallShortOptFunc) {
    clapp::option::help_option_t opt{tp, short_opt, opt_desc_str};
    testing::internal::CaptureStdout();
    ASSERT_EXIT((get_short_opt_func<option_test_parser_t::short_opt_func_t>(
                    tp, short_opt)(short_opt)),
                ::testing::ExitedWithCode(0), "");
    ASSERT_THAT(testing::internal::GetCapturedStdout(),
                testing::HasSubstr("Usage:\n"));
}

TEST_F(option_t, helpOptionConstructAndCallShortOptAndLongOptFunc) {
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

TEST_F(option_t,
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

TEST_F(option_t, countOptionConstructLongStringAndShortAndCallGetOptionHelp) {
    clapp::option::count_option_t opt{
        tp, long_opt_str, short_opt, opt_desc_str,
        clapp::basic_parser_t::purpose_t::mandatory};
    ASSERT_THAT(tp.get_option_help(),
                testing::ContainerEq(option_test_parser_t::help_entry_vec_t{
                    {std::string{"-"} + short_opt + "|--" + long_opt_str,
                     opt_desc_str + " (" + purpose_mandatory_str + ")"}}));
}

TEST_F(option_t, countOptionConstructLongCStringAndShortAndCallGetOptionHelp) {
    clapp::option::count_option_t opt{tp, long_opt_cstr, short_opt,
                                      opt_desc_str};
    ASSERT_THAT(tp.get_option_help(),
                testing::ContainerEq(option_test_parser_t::help_entry_vec_t{
                    {std::string{"-"} + short_opt + "|--" + long_opt_cstr,
                     opt_desc_str + " (" + purpose_optional_str + ")"}}));
}

TEST_F(option_t, countOptionConstructLongStringCallLongOptFuncTwice) {
    clapp::option::count_option_t opt{tp, long_opt_str, opt_desc_str};
    get_long_opt_func<option_test_parser_t::long_opt_func_t>(
        tp, long_opt_str)(long_opt_str);
    get_long_opt_func<option_test_parser_t::long_opt_func_t>(
        tp, long_opt_str)(long_opt_str);
    ASSERT_THAT(opt, NumCountOptionGiven(2u));
}

TEST_F(option_t, countOptionConstructLongStringCallShortOptFuncOnce) {
    clapp::option::count_option_t opt{tp, short_opt, opt_desc_str};
    get_short_opt_func<option_test_parser_t::short_opt_func_t>(
        tp, short_opt)(short_opt);
    ASSERT_THAT(opt, NumCountOptionGiven(1u));
}

TEST_F(
    option_t,
    countOptionConstructLongStringVecAndShortAndCallLongOptFuncAndShortOptFunc) {
    clapp::option::count_option_t opt{
        tp, std::vector<std::string>{long_opt_str}, short_opt, opt_desc_str};
    get_long_opt_func<option_test_parser_t::long_opt_func_t>(
        tp, long_opt_str)(long_opt_str);
    get_short_opt_func<option_test_parser_t::short_opt_func_t>(
        tp, short_opt)(short_opt);
    ASSERT_THAT(opt, NumCountOptionGiven(2u));
}

TEST_F(
    option_t,
    countOptionConstructLongStringVecAndShortWithMinMaxConstraintAndCallLongOptFuncAndShortOptFunc) {
    constexpr std::uint32_t min_value{0};
    constexpr std::uint32_t max_value{5};
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
    ASSERT_THAT(opt, NumCountOptionGiven(4u));
    ASSERT_NO_THROW((option_validate_func.value()()));
    get_short_opt_func<option_test_parser_t::short_opt_func_t>(
        tp, short_opt)(short_opt);
    ASSERT_THAT(opt, NumCountOptionGiven(5u));
    ASSERT_NO_THROW((option_validate_func.value()()));
    ASSERT_NO_THROW((get_long_opt_func<option_test_parser_t::long_opt_func_t>(
        tp, long_opt_str)(long_opt_str)));
    ASSERT_THAT(opt, NumCountOptionGiven(6u));
    ASSERT_THROW((option_validate_func.value()()),
                 clapp::exception::out_of_range_t);
}

TEST_F(
    option_t,
    countOptionConstructLongStringAndShortWithDefaultValueAndCallLongOptFunc) {
    constexpr std::uint32_t default_value{5};
    clapp::option::count_option_t opt{
        tp, long_opt_cstr, short_opt, opt_desc_str,
        clapp::value::default_value_t<std::uint32_t>{default_value}};
    ASSERT_THAT(opt, NumCountOptionNotGiven(default_value));
    ASSERT_NO_THROW((get_long_opt_func<option_test_parser_t::long_opt_func_t>(
        tp, long_opt_cstr)(long_opt_cstr)));
    ASSERT_THAT(opt, NumCountOptionGiven(6u));
}

TEST_F(option_t, countOptionConstructLongStringCallLongOptFuncCallsFoundFunc) {
    std::stringstream ss;
    clapp::option::count_option_t opt{
        tp, long_opt_str, opt_desc_str,
        clapp::value::found_func_t{[&ss]() { ss << "this is a test"; }}};
    ASSERT_THAT(ss.str().size(), testing::Eq(0));
    get_long_opt_func<option_test_parser_t::long_opt_func_t>(
        tp, long_opt_str)(long_opt_str);
    ASSERT_THAT(ss.str(), testing::StrEq("this is a test"));
}

TEST_F(option_t, countOptionConstructMandatoryLongCallValidateFunc) {
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

TEST_F(option_t, stringParamOptionConstructShort) {
    clapp::option::string_param_option_t opt{tp, short_opt, opt_desc_str};
    ASSERT_THAT(tp, ContainsShortOption(short_opt));
    ASSERT_THAT(opt, ParamOptionNotGiven());
}

TEST_F(option_t, stringParamOptionConstructLongString) {
    clapp::option::string_param_option_t opt{tp, long_opt_str, opt_desc_str};
    ASSERT_THAT(tp, ContainsLongOption(long_opt_str));
    ASSERT_THAT(opt, ParamOptionNotGiven());
}

TEST_F(option_t, stringParamOptionConstructLongCString) {
    clapp::option::string_param_option_t opt{tp, long_opt_cstr, opt_desc_cstr};
    ASSERT_THAT(tp, ContainsLongOption(long_opt_cstr));
    ASSERT_THAT(opt, ParamOptionNotGiven());
}

TEST_F(option_t, stringParamOptionConstructShortVec) {
    clapp::option::string_param_option_t opt{tp, std::vector<char>{short_opt},
                                             opt_desc_str};
    ASSERT_THAT(tp, ContainsShortOption(short_opt));
    ASSERT_THAT(opt, ParamOptionNotGiven());
}

TEST_F(option_t, stringParamOptionConstructLongStringVec) {
    clapp::option::string_param_option_t opt{
        tp, std::vector<std::string>{long_opt_str, long_opt_cstr},
        opt_desc_str};
    ASSERT_THAT(tp, ContainsLongOption(long_opt_str));
    ASSERT_THAT(tp, ContainsLongOption(long_opt_cstr));
    ASSERT_THAT(opt, ParamOptionNotGiven());
}

TEST_F(option_t, stringParamOptionConstructLongOptionTwiceThrows) {
    clapp::option::string_param_option_t opt{tp, long_opt_str, opt_desc_str};
    ASSERT_THROW(
        (clapp::option::string_param_option_t{tp, long_opt_str, opt_desc_str}),
        clapp::exception::option_exception_t);
}

TEST_F(option_t, stringParamOptionConstructShortOptionTwiceThrows) {
    clapp::option::string_param_option_t opt{tp, short_opt, opt_desc_str};
    ASSERT_THROW(
        (clapp::option::string_param_option_t{tp, short_opt, opt_desc_str}),
        clapp::exception::option_exception_t);
}

TEST_F(option_t, stringParamOptionConstructLongStringVecAndCallGetOptionHelp) {
    clapp::option::string_param_option_t opt{
        tp, std::vector<std::string>{long_opt_str, long_opt_cstr},
        opt_desc_str};
    ASSERT_THAT(
        tp.get_option_help(),
        testing::ContainerEq(option_test_parser_t::help_entry_vec_t{
            {"--" + long_opt_str + "|--" + long_opt_cstr + param_opt_postfix,
             opt_desc_str + " (" + purpose_optional_str + ")"}}));
}

TEST_F(option_t, stringParamOptionConstructShortVecAndCallGetOptionHelp) {
    clapp::option::string_param_option_t opt{tp, std::vector<char>{short_opt},
                                             opt_desc_str};
    ASSERT_THAT(tp.get_option_help(),
                testing::ContainerEq(option_test_parser_t::help_entry_vec_t{
                    {std::string{"-"} + short_opt + param_opt_postfix,
                     opt_desc_str + " (" + purpose_optional_str + ")"}}));
}

TEST_F(option_t,
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

TEST_F(option_t,
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

TEST_F(option_t,
       stringParamOptionConstructLongCStringAndShortAndCallGetOptionHelp) {
    clapp::option::string_param_option_t opt{tp, long_opt_cstr, short_opt,
                                             opt_desc_str};
    ASSERT_THAT(tp.get_option_help(),
                testing::ContainerEq(option_test_parser_t::help_entry_vec_t{
                    {std::string{"-"} + short_opt + "|--" + long_opt_cstr +
                         param_opt_postfix,
                     opt_desc_str + " (" + purpose_optional_str + ")"}}));
}

TEST_F(option_t, stringParamOptionConstructLongStringAndCallLongOpt) {
    clapp::option::string_param_option_t opt{tp, long_opt_str, opt_desc_str};
    ASSERT_THAT(opt, ParamOptionNotGiven());
    ASSERT_NO_THROW(
        (get_long_opt_func<option_test_parser_t::long_opt_param_func_t>(
            tp, long_opt_str)(long_opt_str, value_cstr)));
    ASSERT_THAT(opt, ParamOptionGiven(value_cstr));
}

TEST_F(option_t, stringParamOptionConstructShortAndCallShortOpt) {
    clapp::option::string_param_option_t opt{tp, short_opt, opt_desc_str};
    ASSERT_THAT(opt, ParamOptionNotGiven());
    ASSERT_NO_THROW(
        (get_short_opt_func<option_test_parser_t::short_opt_param_func_t>(
            tp, short_opt)(short_opt, value_str)));
    ASSERT_THAT(opt, ParamOptionGiven(value_str));
}

TEST_F(
    option_t,
    stringParamOptionConstructLongStringAndShortAndCallShortOptCallsFoundFunc) {
    std::stringstream ss;
    clapp::option::string_param_option_t opt{
        tp, long_opt_str, short_opt, opt_desc_str,
        clapp::value::found_func_t{[&ss]() { ss << "this is a test"; }}};
    ASSERT_THAT(ss.str().size(), testing::Eq(0));
    ASSERT_NO_THROW(
        (get_short_opt_func<option_test_parser_t::short_opt_param_func_t>(
            tp, short_opt)(short_opt, value_str)));
    ASSERT_THAT(ss.str(), testing::StrEq("this is a test"));
}

TEST_F(option_t,
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
    option_t,
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

TEST_F(option_t, int64ParamOptionConstructShort) {
    clapp::option::int64_param_option_t opt{tp, short_opt, opt_desc_str};
    ASSERT_THAT(tp, ContainsShortOption(short_opt));
    ASSERT_THAT(opt, ParamOptionNotGiven());
}

TEST_F(option_t, int64ParamOptionConstructLongString) {
    clapp::option::int64_param_option_t opt{tp, long_opt_str, opt_desc_str};
    ASSERT_THAT(tp, ContainsLongOption(long_opt_str));
    ASSERT_THAT(opt, ParamOptionNotGiven());
}

TEST_F(option_t, int64ParamOptionConstructLongCString) {
    clapp::option::int64_param_option_t opt{tp, long_opt_cstr, opt_desc_cstr};
    ASSERT_THAT(tp, ContainsLongOption(long_opt_cstr));
    ASSERT_THAT(opt, ParamOptionNotGiven());
}

TEST_F(option_t, int64ParamOptionConstructShortVec) {
    clapp::option::int64_param_option_t opt{tp, std::vector<char>{short_opt},
                                            opt_desc_str};
    ASSERT_THAT(tp, ContainsShortOption(short_opt));
    ASSERT_THAT(opt, ParamOptionNotGiven());
}

TEST_F(option_t, int64ParamOptionConstructLongStringVec) {
    clapp::option::int64_param_option_t opt{
        tp, std::vector<std::string>{long_opt_str, long_opt_cstr},
        opt_desc_str};
    ASSERT_THAT(tp, ContainsLongOption(long_opt_str));
    ASSERT_THAT(tp, ContainsLongOption(long_opt_cstr));
    ASSERT_THAT(opt, ParamOptionNotGiven());
}

TEST_F(option_t, int64ParamOptionConstructLongOptionTwiceThrows) {
    clapp::option::int64_param_option_t opt{tp, long_opt_str, opt_desc_str};
    ASSERT_THROW(
        (clapp::option::int64_param_option_t{tp, long_opt_str, opt_desc_str}),
        clapp::exception::option_exception_t);
}

TEST_F(option_t, int64ParamOptionConstructShortOptionTwiceThrows) {
    clapp::option::int64_param_option_t opt{tp, short_opt, opt_desc_str};
    ASSERT_THROW(
        (clapp::option::int64_param_option_t{tp, short_opt, opt_desc_str}),
        clapp::exception::option_exception_t);
}

TEST_F(option_t, int64ParamOptionConstructLongStringVecAndCallGetOptionHelp) {
    clapp::option::int64_param_option_t opt{
        tp, std::vector<std::string>{long_opt_str, long_opt_cstr},
        opt_desc_str};
    ASSERT_THAT(
        tp.get_option_help(),
        testing::ContainerEq(option_test_parser_t::help_entry_vec_t{
            {"--" + long_opt_str + "|--" + long_opt_cstr + param_opt_postfix,
             opt_desc_str + " (" + purpose_optional_str + ")"}}));
}

TEST_F(option_t, int64ParamOptionConstructShortVecAndCallGetOptionHelp) {
    clapp::option::int64_param_option_t opt{tp, std::vector<char>{short_opt},
                                            opt_desc_str};
    ASSERT_THAT(tp.get_option_help(),
                testing::ContainerEq(option_test_parser_t::help_entry_vec_t{
                    {std::string{"-"} + short_opt + param_opt_postfix,
                     opt_desc_str + " (" + purpose_optional_str + ")"}}));
}

TEST_F(option_t,
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

TEST_F(option_t,
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

TEST_F(option_t,
       int64ParamOptionConstructLongCStringAndShortAndCallGetOptionHelp) {
    clapp::option::int64_param_option_t opt{tp, long_opt_cstr, short_opt,
                                            opt_desc_str};
    ASSERT_THAT(tp.get_option_help(),
                testing::ContainerEq(option_test_parser_t::help_entry_vec_t{
                    {std::string{"-"} + short_opt + "|--" + long_opt_cstr +
                         param_opt_postfix,
                     opt_desc_str + " (" + purpose_optional_str + ")"}}));
}

TEST_F(option_t,
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

TEST_F(option_t, int64ParamOptionConstructLongStringAndCallLongOpt) {
    clapp::option::int64_param_option_t opt{tp, long_opt_str, opt_desc_str};
    ASSERT_THAT(opt, ParamOptionNotGiven());
    ASSERT_NO_THROW(
        (get_long_opt_func<option_test_parser_t::long_opt_param_func_t>(
            tp, long_opt_str)(long_opt_str, std::to_string(value_int64))));
    ASSERT_THAT(opt, ParamOptionGiven(value_int64));
}

TEST_F(option_t, int64ParamOptionConstructShortAndCallShortOpt) {
    clapp::option::int64_param_option_t opt{tp, short_opt, opt_desc_str};
    ASSERT_THAT(opt, ParamOptionNotGiven());
    ASSERT_NO_THROW(
        (get_short_opt_func<option_test_parser_t::short_opt_param_func_t>(
            tp, short_opt)(short_opt, std::to_string(value_int64))));
    ASSERT_THAT(opt, ParamOptionGiven(value_int64));
}

TEST_F(
    option_t,
    int64ParamOptionConstructLongStringAndShortAndCallShortOptCallsFoundFunc) {
    std::stringstream ss;
    clapp::option::int64_param_option_t opt{
        tp, long_opt_str, short_opt, opt_desc_str,
        clapp::value::found_func_t{[&ss]() { ss << "this is a test"; }}};
    ASSERT_THAT(ss.str().size(), testing::Eq(0));
    ASSERT_NO_THROW(
        (get_short_opt_func<option_test_parser_t::short_opt_param_func_t>(
            tp, short_opt)(short_opt, std::to_string(value_int64))));
    ASSERT_THAT(ss.str(), testing::StrEq("this is a test"));
}

TEST_F(
    option_t,
    int64ParamOptionConstructShortAndCallShortOptFuncWithInvalidValueThrows) {
    clapp::option::int64_param_option_t opt{tp, short_opt, opt_desc_str};
    ASSERT_THROW(
        (get_short_opt_func<option_test_parser_t::short_opt_param_func_t>(
            tp, short_opt)(short_opt, value_str)),
        clapp::exception::invalid_value_t);
}

TEST_F(option_t, int64ParamOptionConstructShortOptionalNoValidateFunc) {
    clapp::option::int64_param_option_t opt{
        tp, short_opt, opt_desc_str, option_test_parser_t::purpose_t::optional};
    ASSERT_THAT(get_validate_func(tp, short_opt), testing::Eq(std::nullopt));
}

TEST_F(option_t, int64ParamOptionConstructLongOptionalNoValidateFunc) {
    clapp::option::int64_param_option_t opt{
        tp, long_opt_str, opt_desc_str,
        option_test_parser_t::purpose_t::optional};
    ASSERT_THAT(get_validate_func(tp, long_opt_str), testing::Eq(std::nullopt));
}

TEST_F(option_t, int64ParamOptionConstructMandatoryShortAndCallValidateFunc) {
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
    option_t,
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
    option_t,
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

TEST_F(option_t, int32ParamOptionConstructLongStringAndShortAndCallShortOpt) {
    clapp::option::int32_param_option_t opt{tp, long_opt_str, short_opt,
                                            opt_desc_str};
    ASSERT_THAT(opt, ParamOptionNotGiven());
    ASSERT_NO_THROW(
        (get_short_opt_func<option_test_parser_t::short_opt_param_func_t>(
            tp, short_opt)(short_opt, std::to_string(value_int32))));
    ASSERT_THAT(opt, ParamOptionGiven(value_int32));
}

TEST_F(option_t, int16ParamOptionConstructLongStringAndShortAndCallShortOpt) {
    clapp::option::int16_param_option_t opt{tp, long_opt_str, short_opt,
                                            opt_desc_str};
    ASSERT_THAT(opt, ParamOptionNotGiven());
    ASSERT_NO_THROW(
        (get_short_opt_func<option_test_parser_t::short_opt_param_func_t>(
            tp, short_opt)(short_opt, std::to_string(value_int16))));
    ASSERT_THAT(opt, ParamOptionGiven(value_int16));
}

TEST_F(option_t, int8ParamOptionConstructLongStringAndShortAndCallShortOpt) {
    clapp::option::int8_param_option_t opt{tp, long_opt_str, short_opt,
                                           opt_desc_str};
    ASSERT_THAT(opt, ParamOptionNotGiven());
    ASSERT_NO_THROW(
        (get_short_opt_func<option_test_parser_t::short_opt_param_func_t>(
            tp, short_opt)(short_opt, std::to_string(value_int8))));
    ASSERT_THAT(opt, ParamOptionGiven(value_int8));
}

TEST_F(
    option_t,
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

TEST_F(option_t, uint32ParamOptionConstructLongStringAndShortAndCallShortOpt) {
    clapp::option::uint32_param_option_t opt{tp, long_opt_str, short_opt,
                                             opt_desc_str};
    ASSERT_THAT(opt, ParamOptionNotGiven());
    ASSERT_NO_THROW(
        (get_short_opt_func<option_test_parser_t::short_opt_param_func_t>(
            tp, short_opt)(short_opt, std::to_string(value_uint32))));
    ASSERT_THAT(opt, ParamOptionGiven(value_uint32));
}

TEST_F(option_t, uint16ParamOptionConstructLongStringAndShortAndCallShortOpt) {
    clapp::option::uint16_param_option_t opt{tp, long_opt_str, short_opt,
                                             opt_desc_str};
    ASSERT_THAT(opt, ParamOptionNotGiven());
    ASSERT_NO_THROW(
        (get_short_opt_func<option_test_parser_t::short_opt_param_func_t>(
            tp, short_opt)(short_opt, std::to_string(value_uint16))));
    ASSERT_THAT(opt, ParamOptionGiven(value_uint16));
}

TEST_F(option_t, uint8ParamOptionConstructLongStringAndShortAndCallShortOpt) {
    clapp::option::uint8_param_option_t opt{tp, long_opt_str, short_opt,
                                            opt_desc_str};
    ASSERT_THAT(opt, ParamOptionNotGiven());
    ASSERT_NO_THROW(
        (get_short_opt_func<option_test_parser_t::short_opt_param_func_t>(
            tp, short_opt)(short_opt, std::to_string(value_uint8))));
    ASSERT_THAT(opt, ParamOptionGiven(value_uint8));
}

TEST_F(option_t, boolParamOptionConstructLongStringAndShortAndCallShortOpt) {
    clapp::option::bool_param_option_t opt{tp, long_opt_str, short_opt,
                                           opt_desc_str};
    ASSERT_THAT(opt, ParamOptionNotGiven());
    ASSERT_NO_THROW(
        (get_short_opt_func<option_test_parser_t::short_opt_param_func_t>(
            tp, short_opt)(short_opt, std::to_string(value_bool))));
    ASSERT_THAT(opt, ParamOptionGiven(value_bool));
}

TEST_F(option_t, sizeParamOptionConstructLongStringAndShortAndCallShortOpt) {
    clapp::option::size_param_option_t opt{tp, long_opt_str, short_opt,
                                           opt_desc_str};
    ASSERT_THAT(opt, ParamOptionNotGiven());
    ASSERT_NO_THROW(
        (get_short_opt_func<option_test_parser_t::short_opt_param_func_t>(
            tp, short_opt)(short_opt, std::to_string(value_size))));
    ASSERT_THAT(opt, ParamOptionGiven(value_size));
}

TEST_F(option_t, ptrdiffParamOptionConstructLongStringAndShortAndCallShortOpt) {
    clapp::option::ptrdiff_param_option_t opt{tp, long_opt_str, short_opt,
                                              opt_desc_str};
    ASSERT_THAT(opt, ParamOptionNotGiven());
    ASSERT_NO_THROW(
        (get_short_opt_func<option_test_parser_t::short_opt_param_func_t>(
            tp, short_opt)(short_opt, std::to_string(value_ptrdiff))));
    ASSERT_THAT(opt, ParamOptionGiven(value_ptrdiff));
}

TEST_F(option_t, doubleParamOptionConstructLongStringAndShortAndCallShortOpt) {
    clapp::option::double_param_option_t opt{tp, long_opt_str, short_opt,
                                             opt_desc_str};
    ASSERT_THAT(opt, ParamOptionNotGiven());
    ASSERT_NO_THROW(
        (get_short_opt_func<option_test_parser_t::short_opt_param_func_t>(
            tp, short_opt)(short_opt, std::to_string(value_double))));
    ASSERT_THAT(opt, ParamOptionGiven(value_double));
}

TEST_F(option_t, floatParamOptionConstructLongStringAndShortAndCallShortOpt) {
    clapp::option::float_param_option_t opt{tp, long_opt_str, short_opt,
                                            opt_desc_str};
    ASSERT_THAT(opt, ParamOptionNotGiven());
    ASSERT_NO_THROW(
        (get_short_opt_func<option_test_parser_t::short_opt_param_func_t>(
            tp, short_opt)(short_opt, std::to_string(value_float))));
    ASSERT_THAT(opt, ParamOptionGiven(value_float));
}

TEST_F(option_t, nsParamOptionConstructLongStringAndShortAndCallShortOpt) {
    clapp::option::ns_param_option_t opt{tp, long_opt_str, short_opt,
                                         opt_desc_str};
    ASSERT_THAT(opt, ParamOptionNotGiven());
    ASSERT_NO_THROW(
        (get_short_opt_func<option_test_parser_t::short_opt_param_func_t>(
            tp, short_opt)(short_opt, std::to_string(value_ns.count()))));
    ASSERT_THAT(opt, ParamOptionGiven(value_ns));
}

TEST_F(option_t, usParamOptionConstructLongStringAndShortAndCallShortOpt) {
    clapp::option::us_param_option_t opt{tp, long_opt_str, short_opt,
                                         opt_desc_str};
    ASSERT_THAT(opt, ParamOptionNotGiven());
    ASSERT_NO_THROW(
        (get_short_opt_func<option_test_parser_t::short_opt_param_func_t>(
            tp, short_opt)(short_opt, std::to_string(value_us.count()))));
    ASSERT_THAT(opt, ParamOptionGiven(value_us));
}

TEST_F(option_t, msParamOptionConstructLongStringAndShortAndCallShortOpt) {
    clapp::option::ms_param_option_t opt{tp, long_opt_str, short_opt,
                                         opt_desc_str};
    ASSERT_THAT(opt, ParamOptionNotGiven());
    ASSERT_NO_THROW(
        (get_short_opt_func<option_test_parser_t::short_opt_param_func_t>(
            tp, short_opt)(short_opt, std::to_string(value_ms.count()))));
    ASSERT_THAT(opt, ParamOptionGiven(value_ms));
}

TEST_F(option_t, secParamOptionConstructLongStringAndShortAndCallShortOpt) {
    clapp::option::sec_param_option_t opt{tp, long_opt_str, short_opt,
                                          opt_desc_str};
    ASSERT_THAT(opt, ParamOptionNotGiven());
    ASSERT_NO_THROW(
        (get_short_opt_func<option_test_parser_t::short_opt_param_func_t>(
            tp, short_opt)(short_opt, std::to_string(value_sec.count()))));
    ASSERT_THAT(opt, ParamOptionGiven(value_sec));
}

TEST_F(option_t, minParamOptionConstructLongStringAndShortAndCallShortOpt) {
    clapp::option::min_param_option_t opt{tp, long_opt_str, short_opt,
                                          opt_desc_str};
    ASSERT_THAT(opt, ParamOptionNotGiven());
    ASSERT_NO_THROW(
        (get_short_opt_func<option_test_parser_t::short_opt_param_func_t>(
            tp, short_opt)(short_opt, std::to_string(value_min.count()))));
    ASSERT_THAT(opt, ParamOptionGiven(value_min));
}

TEST_F(option_t, hoursParamOptionConstructLongStringAndShortAndCallShortOpt) {
    clapp::option::hours_param_option_t opt{tp, long_opt_str, short_opt,
                                            opt_desc_str};
    ASSERT_THAT(opt, ParamOptionNotGiven());
    ASSERT_NO_THROW(
        (get_short_opt_func<option_test_parser_t::short_opt_param_func_t>(
            tp, short_opt)(short_opt, std::to_string(value_hours.count()))));
    ASSERT_THAT(opt, ParamOptionGiven(value_hours));
}

TEST_F(
    option_t,
    vectorStringParamOptionConstructLongStringAndShortWithDefaultValueThrows) {
    ASSERT_THROW((clapp::option::vector_string_param_option_t{
                     tp, long_opt_str, short_opt, opt_desc_str,
                     clapp::value::default_value_t<std::string>{value_cstr}}),
                 clapp::exception::option_param_exception_t);
}

TEST_F(option_t, vectorStringParamOptionConstructShort) {
    clapp::option::vector_string_param_option_t opt{tp, short_opt,
                                                    opt_desc_str};
    ASSERT_THAT(tp, ContainsShortOption(short_opt));
    ASSERT_THAT(opt, VectorParamOptionNotGiven());
}

TEST_F(option_t, vectorStringParamOptionConstructLongString) {
    clapp::option::vector_string_param_option_t opt{tp, long_opt_str,
                                                    opt_desc_str};
    ASSERT_THAT(tp, ContainsLongOption(long_opt_str));
    ASSERT_THAT(opt, VectorParamOptionNotGiven());
}

TEST_F(option_t, vectorStringParamOptionConstructLongCString) {
    clapp::option::vector_string_param_option_t opt{tp, long_opt_cstr,
                                                    opt_desc_cstr};
    ASSERT_THAT(tp, ContainsLongOption(long_opt_cstr));
    ASSERT_THAT(opt, VectorParamOptionNotGiven());
}

TEST_F(option_t, vectorStringParamOptionConstructShortVec) {
    clapp::option::vector_string_param_option_t opt{
        tp, std::vector<char>{short_opt}, opt_desc_str};
    ASSERT_THAT(tp, ContainsShortOption(short_opt));
    ASSERT_THAT(opt, VectorParamOptionNotGiven());
}

TEST_F(option_t, vectorStringParamOptionConstructLongStringVec) {
    clapp::option::vector_string_param_option_t opt{
        tp, std::vector<std::string>{long_opt_str, long_opt_cstr},
        opt_desc_str};
    ASSERT_THAT(tp, ContainsLongOption(long_opt_str));
    ASSERT_THAT(tp, ContainsLongOption(long_opt_cstr));
    ASSERT_THAT(opt, VectorParamOptionNotGiven());
}

TEST_F(option_t, vectorStringParamOptionConstructLongOptionTwiceThrows) {
    clapp::option::vector_string_param_option_t opt{tp, long_opt_str,
                                                    opt_desc_str};
    ASSERT_THROW((clapp::option::vector_string_param_option_t{tp, long_opt_str,
                                                              opt_desc_str}),
                 clapp::exception::option_exception_t);
}

TEST_F(option_t, vectorStringParamOptionConstructShortOptionTwiceThrows) {
    clapp::option::vector_string_param_option_t opt{tp, short_opt,
                                                    opt_desc_str};
    ASSERT_THROW((clapp::option::vector_string_param_option_t{tp, short_opt,
                                                              opt_desc_str}),
                 clapp::exception::option_exception_t);
}

TEST_F(option_t,
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

TEST_F(option_t, vectorStringParamOptionConstructShortVecAndCallGetOptionHelp) {
    clapp::option::vector_string_param_option_t opt{
        tp, std::vector<char>{short_opt}, opt_desc_str};
    ASSERT_THAT(tp.get_option_help(),
                testing::ContainerEq(option_test_parser_t::help_entry_vec_t{
                    {std::string{"-"} + short_opt + param_opt_postfix,
                     opt_desc_str + " (" + purpose_optional_str + ", " +
                         vector_opt_desc_restriction + ")"}}));
}

TEST_F(
    option_t,
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

TEST_F(option_t,
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
    option_t,
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

TEST_F(option_t, vectorStringParamOptionConstructLongStringAndCallLongOpt) {
    clapp::option::vector_string_param_option_t opt{tp, long_opt_str,
                                                    opt_desc_str};
    ASSERT_THAT(opt, VectorParamOptionNotGiven());
    ASSERT_NO_THROW(
        (get_long_opt_func<option_test_parser_t::long_opt_param_func_t>(
            tp, long_opt_str)(long_opt_str, value_cstr)));
    ASSERT_THAT(opt, VectorParamOptionGiven(
                         std::vector<std::string>{std::string{value_cstr}}));
}

TEST_F(option_t, vectorStringParamOptionConstructShortAndCallShortOpt) {
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
    option_t,
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
    option_t,
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
    option_t,
    vectorStringParamOptionConstructLongStringAndShortAndCallShortOptCallsFoundFunc) {
    std::stringstream ss;
    clapp::option::vector_string_param_option_t opt{
        tp, long_opt_str, short_opt, opt_desc_str,
        clapp::value::found_func_t{[&ss]() { ss << "this is a test"; }}};
    ASSERT_THAT(ss.str().size(), testing::Eq(0));
    ASSERT_NO_THROW(
        (get_short_opt_func<option_test_parser_t::short_opt_param_func_t>(
            tp, short_opt)(short_opt, value_str)));
    ASSERT_THAT(ss.str(), testing::StrEq("this is a test"));
}

TEST_F(
    option_t,
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

TEST_F(
    option_t,
    vectorInt64ParamOptionConstructLongStringAndShortWithDefaultValueThrows) {
    ASSERT_THROW((clapp::option::vector_int64_param_option_t{
                     tp, long_opt_str, short_opt, opt_desc_str,
                     clapp::value::default_value_t<std::int64_t>{value_int64}}),
                 clapp::exception::option_param_exception_t);
}

TEST_F(option_t, vectorInt64ParamOptionConstructShort) {
    clapp::option::vector_int64_param_option_t opt{tp, short_opt, opt_desc_str};
    ASSERT_THAT(tp, ContainsShortOption(short_opt));
    ASSERT_THAT(opt, VectorParamOptionNotGiven());
}

TEST_F(option_t, vectorInt64ParamOptionConstructLongString) {
    clapp::option::vector_int64_param_option_t opt{tp, long_opt_str,
                                                   opt_desc_str};
    ASSERT_THAT(tp, ContainsLongOption(long_opt_str));
    ASSERT_THAT(opt, VectorParamOptionNotGiven());
}

TEST_F(option_t, vectorInt64ParamOptionConstructLongCString) {
    clapp::option::vector_int64_param_option_t opt{tp, long_opt_cstr,
                                                   opt_desc_cstr};
    ASSERT_THAT(tp, ContainsLongOption(long_opt_cstr));
    ASSERT_THAT(opt, VectorParamOptionNotGiven());
}

TEST_F(option_t, vectorInt64ParamOptionConstructShortVec) {
    clapp::option::vector_int64_param_option_t opt{
        tp, std::vector<char>{short_opt}, opt_desc_str};
    ASSERT_THAT(tp, ContainsShortOption(short_opt));
    ASSERT_THAT(opt, VectorParamOptionNotGiven());
}

TEST_F(option_t, vectorInt64ParamOptionConstructLongStringVec) {
    clapp::option::vector_int64_param_option_t opt{
        tp, std::vector<std::string>{long_opt_str, long_opt_cstr},
        opt_desc_str};
    ASSERT_THAT(tp, ContainsLongOption(long_opt_str));
    ASSERT_THAT(tp, ContainsLongOption(long_opt_cstr));
    ASSERT_THAT(opt, VectorParamOptionNotGiven());
}

TEST_F(option_t, vectorInt64ParamOptionConstructLongOptionTwiceThrows) {
    clapp::option::vector_int64_param_option_t opt{tp, long_opt_str,
                                                   opt_desc_str};
    ASSERT_THROW((clapp::option::vector_int64_param_option_t{tp, long_opt_str,
                                                             opt_desc_str}),
                 clapp::exception::option_exception_t);
}

TEST_F(option_t, vectorInt64ParamOptionConstructShortOptionTwiceThrows) {
    clapp::option::vector_int64_param_option_t opt{tp, short_opt, opt_desc_str};
    ASSERT_THROW((clapp::option::vector_int64_param_option_t{tp, short_opt,
                                                             opt_desc_str}),
                 clapp::exception::option_exception_t);
}

TEST_F(option_t,
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

TEST_F(option_t, vectorInt64ParamOptionConstructShortVecAndCallGetOptionHelp) {
    clapp::option::vector_int64_param_option_t opt{
        tp, std::vector<char>{short_opt}, opt_desc_str};
    ASSERT_THAT(tp.get_option_help(),
                testing::ContainerEq(option_test_parser_t::help_entry_vec_t{
                    {std::string{"-"} + short_opt + param_opt_postfix,
                     opt_desc_str + " (" + purpose_optional_str + ", " +
                         vector_opt_desc_restriction + ")"}}));
}

TEST_F(
    option_t,
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

TEST_F(option_t,
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

TEST_F(option_t,
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
    option_t,
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

TEST_F(option_t, vectorInt64ParamOptionConstructLongStringAndCallLongOpt) {
    clapp::option::vector_int64_param_option_t opt{tp, long_opt_str,
                                                   opt_desc_str};
    ASSERT_THAT(opt, VectorParamOptionNotGiven());
    ASSERT_NO_THROW(
        (get_long_opt_func<option_test_parser_t::long_opt_param_func_t>(
            tp, long_opt_str)(long_opt_str, std::to_string(value_int64))));
    ASSERT_THAT(opt,
                VectorParamOptionGiven(std::vector<std::int64_t>{value_int64}));
}

TEST_F(option_t, vectorInt64ParamOptionConstructShortAndCallShortOpt) {
    clapp::option::vector_int64_param_option_t opt{tp, short_opt, opt_desc_str};
    ASSERT_THAT(opt, VectorParamOptionNotGiven());
    ASSERT_NO_THROW(
        (get_short_opt_func<option_test_parser_t::short_opt_param_func_t>(
            tp, short_opt)(short_opt, std::to_string(value_int64))));
    ASSERT_THAT(opt,
                VectorParamOptionGiven(std::vector<std::int64_t>{value_int64}));
}

TEST_F(
    option_t,
    vectorInt64ParamOptionConstructLongStringAndShortAndCallShortOptCallsFoundFunc) {
    std::stringstream ss;
    clapp::option::vector_int64_param_option_t opt{
        tp, long_opt_str, short_opt, opt_desc_str,
        clapp::value::found_func_t{[&ss]() { ss << "testaa"; }}};
    ASSERT_THAT(ss.str().size(), testing::Eq(0));
    ASSERT_NO_THROW(
        (get_short_opt_func<option_test_parser_t::short_opt_param_func_t>(
            tp, short_opt)(short_opt, std::to_string(value_int64))));
    ASSERT_NO_THROW(
        (get_short_opt_func<option_test_parser_t::short_opt_param_func_t>(
            tp, short_opt)(short_opt, std::to_string(value_int32))));
    ASSERT_THAT(ss.str(), testing::StrEq("testaatestaa"));
}

TEST_F(
    option_t,
    vectorInt64ParamOptionConstructShortAndCallShortOptFuncWithInvalidValueThrows) {
    clapp::option::vector_int64_param_option_t opt{tp, short_opt, opt_desc_str};
    ASSERT_THROW(
        (get_short_opt_func<option_test_parser_t::short_opt_param_func_t>(
            tp, short_opt)(short_opt, value_str)),
        clapp::exception::invalid_value_t);
}

TEST_F(option_t,
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
    option_t,
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

TEST_F(option_t,
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

TEST_F(option_t,
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

TEST_F(option_t,
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

TEST_F(option_t,
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

TEST_F(option_t,
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

TEST_F(option_t,
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

TEST_F(option_t,
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

TEST_F(option_t,
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

TEST_F(option_t,
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

TEST_F(option_t,
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

TEST_F(option_t,
       vectorDoubleParamOptionConstructLongStringAndShortAndCallShortOpt) {
    clapp::option::vector_double_param_option_t opt{tp, long_opt_str, short_opt,
                                                    opt_desc_str};
    ASSERT_THAT(opt, VectorParamOptionNotGiven());
    ASSERT_NO_THROW(
        (get_short_opt_func<option_test_parser_t::short_opt_param_func_t>(
            tp, short_opt)(short_opt, std::to_string(value_double))));
    ASSERT_THAT(opt, VectorParamOptionGiven(std::vector<double>{value_double}));
}

TEST_F(option_t,
       vectorFloatParamOptionConstructLongStringAndShortAndCallShortOpt) {
    clapp::option::vector_float_param_option_t opt{tp, long_opt_str, short_opt,
                                                   opt_desc_str};
    ASSERT_THAT(opt, VectorParamOptionNotGiven());
    ASSERT_NO_THROW(
        (get_short_opt_func<option_test_parser_t::short_opt_param_func_t>(
            tp, short_opt)(short_opt, std::to_string(value_float))));
    ASSERT_THAT(opt, VectorParamOptionGiven(std::vector<float>{value_float}));
}

TEST_F(option_t,
       vectorNsParamOptionConstructLongStringAndShortAndCallShortOpt) {
    clapp::option::vector_ns_param_option_t opt{tp, long_opt_str, short_opt,
                                                opt_desc_str};
    ASSERT_THAT(opt, VectorParamOptionNotGiven());
    ASSERT_NO_THROW(
        (get_short_opt_func<option_test_parser_t::short_opt_param_func_t>(
            tp, short_opt)(short_opt, std::to_string(value_ns.count()))));
    ASSERT_THAT(opt, VectorParamOptionGiven(
                         std::vector<std::chrono::nanoseconds>{value_ns}));
}

TEST_F(option_t,
       vectorUsParamOptionConstructLongStringAndShortAndCallShortOpt) {
    clapp::option::vector_us_param_option_t opt{tp, long_opt_str, short_opt,
                                                opt_desc_str};
    ASSERT_THAT(opt, VectorParamOptionNotGiven());
    ASSERT_NO_THROW(
        (get_short_opt_func<option_test_parser_t::short_opt_param_func_t>(
            tp, short_opt)(short_opt, std::to_string(value_us.count()))));
    ASSERT_THAT(opt, VectorParamOptionGiven(
                         std::vector<std::chrono::microseconds>{value_us}));
}

TEST_F(option_t,
       vectorMsParamOptionConstructLongStringAndShortAndCallShortOpt) {
    clapp::option::vector_ms_param_option_t opt{tp, long_opt_str, short_opt,
                                                opt_desc_str};
    ASSERT_THAT(opt, VectorParamOptionNotGiven());
    ASSERT_NO_THROW(
        (get_short_opt_func<option_test_parser_t::short_opt_param_func_t>(
            tp, short_opt)(short_opt, std::to_string(value_ms.count()))));
    ASSERT_THAT(opt, VectorParamOptionGiven(
                         std::vector<std::chrono::milliseconds>{value_ms}));
}

TEST_F(option_t,
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

TEST_F(option_t,
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

TEST_F(option_t,
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

TEST_F(option_t, invalidLongOptionConstruct) {
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

TEST_F(option_t, invalidShortOptionConstruct) {
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

TEST_F(option_t, genOptValidateFuncAndCallValidateFuncThrows) {
    std::optional<option_test_parser_t::validate_func_t> validate_func{
        clapp::option::gen_opt_validate_func<std::int32_t, int32_value_func_t>(
            std::nullopt,
            []() -> bool { throw std::runtime_error{"unexpected call"}; },
            []() { return false; }, std::vector<int32_validate_func_t>{},
            "option string", option_test_parser_t::purpose_t::mandatory)};
    ASSERT_THAT(validate_func.has_value(), testing::Eq(true));
    ASSERT_THROW((*validate_func)(), clapp::exception::option_exception_t);
}

TEST_F(option_t, genOpftValidateFuncAndCallValidateFuncDoesntThrow) {
    std::optional<option_test_parser_t::validate_func_t> validate_func{
        clapp::option::gen_opt_validate_func<std::int32_t, int32_value_func_t>(
            std::nullopt,
            []() -> bool { throw std::runtime_error{"unexpected call"}; },
            []() { return true; }, std::vector<int32_validate_func_t>{},
            "option string", option_test_parser_t::purpose_t::mandatory)};
    ASSERT_THAT(validate_func.has_value(), testing::Eq(true));
    ASSERT_NO_THROW((*validate_func)());
}

TEST_F(option_t, genOptValidateFuncWithoutValueFuncAndCallValidateFuncThrows) {
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

TEST_F(option_t,
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
