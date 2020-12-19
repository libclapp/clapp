#include <clapp/argument.h>
#include <clapp/sub_parser.h>
#include <gmock/gmock.h>
#include "helper.h"

class argument_test_parser_t : public clapp::parser::basic_parser_t {
   public:
    ~argument_test_parser_t() override;

    using clapp::parser::basic_parser_t::purpose_t;

    using clapp::parser::basic_parser_t::argument_descriptions_vec_t;
    using clapp::parser::basic_parser_t::argument_func_t;
    using clapp::parser::basic_parser_t::argument_type_t;
    using clapp::parser::basic_parser_t::arguments_vector_t;
    using clapp::parser::basic_parser_t::validate_func_vec_t;

    using clapp::parser::basic_parser_t::get_argument_help;
    using clapp::parser::basic_parser_t::get_arguments;
    using clapp::parser::basic_parser_t::get_mandatory_argument_descriptions;
    using clapp::parser::basic_parser_t::get_optional_argument_descriptions;
    using clapp::parser::basic_parser_t::get_validate_functions;

    [[nodiscard]] std::string gen_short_line_prefix() const override;
};

template <typename ARGUMENT_T>
static std::optional<argument_test_parser_t::variant_arg_conf_t>
contains_argument(
    std::vector<argument_test_parser_t::variant_arg_conf_t>& arguments,
    ARGUMENT_T&& argument) {
    for (const auto& arg : arguments) {
        const bool found{std::visit(
            [&argument](auto&& a) -> bool {
                if (argument == a.argument_name) {
                    return true;
                }
                return false;
            },
            arg)};
        if (found) {
            return arg;
        }
    }
    return std::nullopt;
}

template <typename ARGUMENT_T, typename RESULT_LISTENER_T>
static std::optional<argument_test_parser_t::variant_arg_conf_t>
contains_argument(
    std::vector<argument_test_parser_t::variant_arg_conf_t>& arguments,
    ARGUMENT_T&& argument, RESULT_LISTENER_T&& result_listener) {
    if (arguments.size() == 0) {
        *result_listener
            << "Parser-arguments doesn't contain any arguments at all.";
        return std::nullopt;
    }
    std::optional<argument_test_parser_t::variant_arg_conf_t> ret{
        contains_argument(arguments, std::forward<ARGUMENT_T>(argument))};
    if (!ret) {
        *result_listener << "Parser-arguments doesn't contain argument '"
                         << argument << "'.";
        return std::nullopt;
    }
    return ret;
}

MATCHER_P(ContainsArgument, argument, "") {
    std::vector<clapp::parser::basic_parser_t::variant_arg_conf_t> arguments{
        arg.get_arguments()};
    std::optional<clapp::parser::basic_parser_t::variant_arg_conf_t> found_arg{
        contains_argument(arguments, argument, result_listener)};
    if (found_arg == std::nullopt) {
        return false;
    }
    *result_listener << "Found parser-argument doesn't contain argument '"
                     << argument << "'";
    return std::visit(
        [this](auto&& arg_param) -> bool {
            return arg_param.argument_name == argument;
        },
        found_arg.value());
}

MATCHER_P(ArgumentGiven, value, "") {
    *result_listener << "argument: "
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

MATCHER_P(ArgumentNotGivenDefaultValue, default_value, "") {
    *result_listener << "argument: "
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

MATCHER_P(VariadicArgumentGiven, value, "") {
    *result_listener << "argument: "
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

MATCHER(VariadicArgumentNotGiven, "") {
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

MATCHER(ArgumentNotGiven, "") {
    *result_listener << "argument: "
                     << (static_cast<bool>(arg) ? "true" : "false")
                     << ", has_value(): "
                     << (arg.has_value() ? "true" : "false")
                     << ", given(): " << (arg.given() ? "true" : "false");
    return static_cast<bool>(arg) == false && arg.has_value() == false &&
           arg.given() == false;
}

argument_test_parser_t::~argument_test_parser_t() = default;

template <typename ARG_FUNC_T>
static ARG_FUNC_T get_arg_func(argument_test_parser_t& tp,
                               const std::string& argument) {
    std::vector<argument_test_parser_t::variant_arg_conf_t> arguments{
        tp.get_arguments()};
    std::optional<argument_test_parser_t::variant_arg_conf_t> found_arg{
        contains_argument(arguments, argument)};
    if (found_arg == std::nullopt) {
        throw std::runtime_error("no argument with '" + argument +
                                 "' registered.");
    }
    return std::visit(
        [&argument](auto&& arg) -> ARG_FUNC_T {
            if (arg.argument_name == argument) {
                return arg.argument;
            }
            throw std::runtime_error(
                "found long-opt-func, but iterate failed... wtf");
        },
        found_arg.value());
}

template <typename ARGUMENT_T>
static std::optional<argument_test_parser_t::validate_func_t> get_validate_func(
    argument_test_parser_t& tp, ARGUMENT_T&& argument) {
    std::string argument_string{argument};
    std::vector<argument_test_parser_t::variant_arg_conf_t> arguments{
        tp.get_arguments()};
    std::optional<argument_test_parser_t::variant_arg_conf_t> found_arg{
        contains_argument(arguments,
                          std::forward<decltype(argument)>(argument))};
    if (found_arg == std::nullopt) {
        throw std::runtime_error("no argument with '" + argument_string +
                                 "' registered.");
    }

    return std::visit([](auto&& vf) { return vf.validate_func; },
                      found_arg.value());
}

std::string argument_test_parser_t::gen_short_line_prefix() const {
    return "arg-test-parser" + gen_short_line();
}

class simple_argument_sub_parser_t : public clapp::parser::basic_sub_parser_t {
    using clapp::parser::basic_sub_parser_t::basic_sub_parser_t;

   public:
    ~simple_argument_sub_parser_t() override;
};

simple_argument_sub_parser_t::~simple_argument_sub_parser_t() = default;

class test_argument_t : public clapp::argument::basic_argument_t<std::int32_t> {
   public:
    template <typename... Params>
    explicit test_argument_t(clapp::parser::basic_parser_t& parser,
                             Params... parameters);
    ~test_argument_t() override;

    inline explicit operator bool() const;

   private:
    void found_entry();
    static callbacks_t create_callbacks(test_argument_t* inst);
};

template <typename... Params>
test_argument_t::test_argument_t(clapp::basic_parser_t& parser,
                                 Params... parameters)
    : clapp::basic_argument_t<std::int32_t>{
          parser, std::forward<Params>(parameters)...} {}

void test_argument_t::found_entry() {
    _given = true;
    _value = _value.value() + 1;
    for (auto& found_func : _found) {
        found_func.found();
    }
}

inline test_argument_t::operator bool() const { return _value.value() != 0; }

test_argument_t::~test_argument_t() = default;

test_argument_t::callbacks_t test_argument_t::create_callbacks(
    test_argument_t* inst) {
    return callbacks_t{
        [inst](const std::string_view /*option*/) { inst->found_entry(); },
        [inst]() { return inst->given(); },
        [inst]() { return static_cast<bool>(*inst); },
        [inst]() { return inst->value(); }};
}

class argument_t : public ::testing::Test {
   protected:
    void SetUp() override {}
    void TearDown() override {}

    using int32_value_func_t = std::function<std::int32_t(void)>;
    using int32_validate_func_t = std::function<void(
        const std::int32_t&, const std::string& option_string)>;

    argument_test_parser_t tp;
    std::size_t found_func_called{0};

    inline static const std::string arg_str{"arg-str"};
    inline static constexpr const char* arg_cstr{"arg-cstr"};
    inline static const std::string desc_str{"desc-str"};
    inline static constexpr const char* desc_cstr{"opt-desc-cstr"};
    inline static constexpr const char* value_cstr{"value-cstr"};
    inline static const std::string value_str{"value-str"};
    inline static const std::string purpose_optional_str{"optional"};
    inline static const std::string purpose_mandatory_str{"mandatory"};
    inline static const char* variadic_argument_desc_restriction{
        "variadic argument"};
    inline static constexpr std::int64_t value_int64{0x12345678abcdef0LL};
    inline static constexpr std::int32_t value_int32{0x12345678};
    inline static constexpr std::int16_t value_int16{0x1234};
    inline static constexpr std::int8_t value_int8{0x12};
    inline static constexpr std::int8_t value_int8_additional{0x7e};
    inline static constexpr std::uint64_t value_uint64{0xc2345678abcdef0LL};
    inline static constexpr std::uint32_t value_uint32{0xc2345678};
    inline static constexpr std::uint16_t value_uint16{0xc234};
    inline static constexpr std::uint8_t value_uint8{0xc2};
    inline static constexpr std::uint8_t value_uint8_additional{0xff};
    inline static const clapp::fs::path value_path{"/this/is/my/path"};
    inline static constexpr const char* value_path_cstring{
        "/this/is/my/cstring/path"};
    inline static const clapp::fs::path value_path_additional{
        "/this/is/my/additional/path"};
    inline static constexpr bool value_true{true};
    inline static constexpr bool value_false{false};
    inline static constexpr std::size_t value_size{0xcf};
    inline static constexpr std::ptrdiff_t value_ptrdiff{0x1f};
    inline static const double value_double{1415.1716};
    inline static const float value_float{1415.1617f};
    inline static const float value_float_additional{12.1f};
    inline static constexpr std::chrono::nanoseconds value_ns{1213};
    inline static constexpr std::chrono::microseconds value_us{121};
    inline static constexpr std::chrono::milliseconds value_ms{12};
    inline static constexpr std::chrono::seconds value_sec{3603};
    inline static constexpr std::chrono::minutes value_min{62};
    inline static constexpr std::chrono::hours value_hours{1};
    inline static constexpr std::chrono::hours value_hours_additional{2};
};

TEST_F(argument_t, basicArgumentToShortArgumentNameThrows) {
    test_argument_t arg{tp, arg_cstr, desc_cstr};
    ASSERT_THROW(arg.value(), clapp::exception::value_undefined_t);
}

TEST_F(argument_t, basicArgumentConstructStrAndCallValueThrows) {
    test_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THROW(arg.value(), clapp::exception::value_undefined_t);
}

TEST_F(argument_t, basicArgumentConstructOptionalStrWithoutValidateFunc) {
    test_argument_t arg{tp, arg_str, desc_str,
                        argument_test_parser_t::purpose_t::optional};
    ASSERT_THAT(get_validate_func(tp, arg_str), testing::Eq(std::nullopt));
}

TEST_F(argument_t, basicArgumentConstructMandatoryStrWithValidateFunc) {
    test_argument_t arg{tp, arg_str, desc_str,
                        argument_test_parser_t::purpose_t::mandatory};
    ASSERT_THROW(get_validate_func(tp, arg_str).value()(),
                 clapp::exception::argument_exception_t);
}

TEST_F(argument_t, basicArgumentConstructStrWithEmpyArgumentNameThrows) {
    const std::string empty{};
    ASSERT_THROW((test_argument_t{tp, empty, desc_cstr}),
                 clapp::exception::argument_exception_t);
}

TEST_F(argument_t, basicArgumentConstructStrAndCallArgFunc) {
    test_argument_t arg{tp, arg_str, desc_str};
    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)(std::to_string(value_int32))));
    ASSERT_THAT(arg, ArgumentGiven(value_int32));
}

TEST_F(argument_t, basicArgumentConstructStrWithDefaultValueAndCallArgFunc) {
    test_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::optional,
        clapp::value::default_value_t<std::int32_t>{value_int16}};
    ASSERT_THAT(arg, ArgumentNotGivenDefaultValue(value_int16));
    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)(std::to_string(value_int32))));
    ASSERT_THAT(arg, ArgumentGiven(value_int32));
}

TEST_F(argument_t, basicArgumentCallFoundFunc) {
    test_argument_t arg{
        tp, arg_str, desc_str,
        clapp::value::found_func_t{[this]() { found_func_called++; }}};
    ASSERT_THAT(tp, ContainsArgument(arg_str));
    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)(std::to_string(value_uint16))));
    ASSERT_THAT(found_func_called, testing::Eq(1));
}

TEST_F(argument_t, boolArgumentConstructCStrAndCallValueThrows) {
    clapp::argument::bool_argument_t arg{tp, arg_cstr, desc_str};
    ASSERT_THAT(tp, ContainsArgument(arg_cstr));
    ASSERT_THAT(arg, ArgumentNotGiven());
    ASSERT_THROW(arg.value(), clapp::exception::value_undefined_t);
}

TEST_F(argument_t, boolArgumentConstructStrAndCallValueThrows) {
    clapp::argument::bool_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THAT(tp, ContainsArgument(arg_str));
    ASSERT_THAT(arg, ArgumentNotGiven());
    ASSERT_THROW(arg.value(), clapp::exception::value_undefined_t);
}

TEST_F(argument_t, boolArgumentConstructWithAlreadyRegisteredArgThrows) {
    clapp::argument::bool_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THROW((clapp::argument::bool_argument_t{tp, arg_str, desc_str}),
                 clapp::exception::argument_exception_t);
}

TEST_F(argument_t, boolArgumentConstructOptionalStrWithoutValidateFunc) {
    clapp::argument::bool_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::optional};
    ASSERT_THAT(get_validate_func(tp, arg_str), testing::Eq(std::nullopt));
}

TEST_F(argument_t, boolArgumentConstructMandatoryStrWithValidateFunc) {
    clapp::argument::bool_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::mandatory};
    ASSERT_THROW(get_validate_func(tp, arg_str).value()(),
                 clapp::exception::argument_exception_t);
}

TEST_F(argument_t, boolArgumentCallFoundFunc) {
    clapp::argument::bool_argument_t arg{
        tp, arg_str, desc_str,
        clapp::value::found_func_t{[this]() { found_func_called++; }}};
    ASSERT_THAT(tp, ContainsArgument(arg_str));
    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)("1")));
    ASSERT_THAT(found_func_called, testing::Eq(1));
}

TEST_F(argument_t, boolArgumentConstructWithEmpyArgumentNameThrows) {
    ASSERT_THROW((clapp::argument::bool_argument_t{tp, "", desc_cstr}),
                 clapp::exception::argument_exception_t);
}

TEST_F(argument_t, boolArgumentConstructStrAndCallArgFunc) {
    clapp::argument::bool_argument_t arg{tp, arg_str, desc_str};
    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)(std::to_string(value_true))));
    ASSERT_THAT(arg, ArgumentGiven(value_true));
}

TEST_F(argument_t, boolArgumentConstructStrWithDefaultValueAndCallArgFunc) {
    clapp::argument::bool_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::optional,
        clapp::value::default_value_t<bool>{value_false}};
    ASSERT_THAT(arg, ArgumentNotGivenDefaultValue(value_false));
    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)(std::to_string(value_true))));
    ASSERT_THAT(arg, ArgumentGiven(value_true));
}

TEST_F(argument_t, boolArgumentConstructMandatoryStrAndCallGetOptionHelp) {
    clapp::argument::bool_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THAT(tp.get_argument_help(),
                testing::ContainerEq(argument_test_parser_t::help_entry_vec_t{
                    {arg_str, desc_str + " (" + purpose_mandatory_str + ")"}}));
}

TEST_F(argument_t, boolArgumentConstructOptionalStrAndCallGetOptionHelp) {
    clapp::argument::bool_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::optional};
    ASSERT_THAT(tp.get_argument_help(),
                testing::ContainerEq(argument_test_parser_t::help_entry_vec_t{
                    {arg_str, desc_str + " (" + purpose_optional_str + ")"}}));
}

TEST_F(argument_t,
       variadicBoolArgumentConstructOptionalStrWithoutValidateFunc) {
    clapp::argument::bool_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::optional};
    ASSERT_THAT(get_validate_func(tp, arg_str), testing::Eq(std::nullopt));
}

TEST_F(argument_t, variadicBoolArgumentConstructMandatoryStrWithValidateFunc) {
    clapp::argument::bool_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::mandatory};
    ASSERT_THROW(get_validate_func(tp, arg_str).value()(),
                 clapp::exception::argument_exception_t);
}

TEST_F(argument_t,
       variadicBoolArgumentConstructStrAndCallValueReturnsEmptyVector) {
    clapp::argument::variadic_bool_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THAT(tp, ContainsArgument(arg_str));
    ASSERT_THAT(arg, VariadicArgumentNotGiven());
    ASSERT_THAT(arg.value(), testing::Eq(std::vector<bool>{}));
}

TEST_F(argument_t,
       variadicBoolArgumentConstructWithAlreadyRegisteredArgThrows) {
    clapp::argument::variadic_bool_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THROW((clapp::argument::bool_argument_t{tp, arg_str, desc_str}),
                 clapp::exception::argument_exception_t);
}

TEST_F(argument_t, variadicBoolArgumentConstructWithEmpyArgumentNameThrows) {
    ASSERT_THROW((clapp::argument::variadic_bool_argument_t{tp, "", desc_cstr}),
                 clapp::exception::argument_exception_t);
}

TEST_F(argument_t,
       variadicBoolArgumentConstructStrOptionalAndCallArgFuncMultipleTimes) {
    clapp::argument::variadic_bool_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::optional};
    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)(std::to_string(value_true))));
    ASSERT_THAT(arg, VariadicArgumentGiven(std::vector<bool>{value_true}));
    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)(std::to_string(value_false))));
    ASSERT_THAT(
        arg, VariadicArgumentGiven(std::vector<bool>{value_true, value_false}));
}

TEST_F(argument_t, variadicBoolArgumentConstructStrWithoutCallArgFunc) {
    clapp::argument::variadic_bool_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THAT(arg, VariadicArgumentNotGiven());
}

TEST_F(argument_t,
       variadicBoolArgumentConstructCStrAndCallArgFuncMultipleTimes) {
    clapp::argument::variadic_bool_argument_t arg{tp, arg_cstr, desc_str};
    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_cstr)(std::to_string(value_true))));
    ASSERT_THAT(arg, VariadicArgumentGiven(std::vector<bool>{value_true}));
    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_cstr)(std::to_string(value_false))));
    ASSERT_THAT(
        arg, VariadicArgumentGiven(std::vector<bool>{value_true, value_false}));
}

TEST_F(argument_t, variadicBoolArgumentConstructCStrWithoutCallArgFunc) {
    clapp::argument::variadic_bool_argument_t arg{tp, arg_cstr, desc_str};
    ASSERT_THAT(arg, VariadicArgumentNotGiven());
}

TEST_F(argument_t,
       variadicBoolArgumentConstructMandatoryStrAndCallGetOptionHelp) {
    clapp::argument::variadic_bool_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THAT(tp.get_argument_help(),
                testing::ContainerEq(argument_test_parser_t::help_entry_vec_t{
                    {arg_str, desc_str + " (" + purpose_mandatory_str + ", " +
                                  variadic_argument_desc_restriction + ")"}}));
}

TEST_F(argument_t, variadicBoolArgumentCallFoundFunc) {
    clapp::argument::variadic_bool_argument_t arg{
        tp, arg_str, desc_str,
        clapp::value::found_func_t{[this]() { found_func_called++; }}};
    ASSERT_THAT(tp, ContainsArgument(arg_str));

    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)("1")));
    ASSERT_THAT(found_func_called, testing::Eq(1));

    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)("true")));
    ASSERT_THAT(found_func_called, testing::Eq(2));
}

TEST_F(argument_t, stringArgumentConstructCStrAndCallValueThrows) {
    clapp::argument::string_argument_t arg{tp, arg_cstr, desc_str};
    ASSERT_THAT(tp, ContainsArgument(arg_cstr));
    ASSERT_THAT(arg, ArgumentNotGiven());
    ASSERT_THROW(arg.value(), clapp::exception::value_undefined_t);
}

TEST_F(argument_t, stringArgumentConstructStrAndCallValueThrows) {
    clapp::argument::string_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THAT(tp, ContainsArgument(arg_str));
    ASSERT_THAT(arg, ArgumentNotGiven());
    ASSERT_THROW(arg.value(), clapp::exception::value_undefined_t);
}

TEST_F(argument_t, stringArgumentConstructWithAlreadyRegisteredArgThrows) {
    clapp::argument::string_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THROW((clapp::argument::string_argument_t{tp, arg_str, desc_str}),
                 clapp::exception::argument_exception_t);
}

TEST_F(argument_t, stringArgumentConstructOptionalStrWithoutValidateFunc) {
    clapp::argument::string_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::optional};
    ASSERT_THAT(get_validate_func(tp, arg_str), testing::Eq(std::nullopt));
}

TEST_F(argument_t, stringArgumentConstructMandatoryStrWithValidateFunc) {
    clapp::argument::string_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::mandatory};
    ASSERT_THROW(get_validate_func(tp, arg_str).value()(),
                 clapp::exception::argument_exception_t);
}

TEST_F(argument_t, stringArgumentCallFoundFunc) {
    clapp::argument::string_argument_t arg{
        tp, arg_str, desc_str,
        clapp::value::found_func_t{[this]() { found_func_called++; }}};
    ASSERT_THAT(tp, ContainsArgument(arg_str));
    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)("1")));
    ASSERT_THAT(found_func_called, testing::Eq(1));
}

TEST_F(argument_t, stringArgumentConstructWithEmpyArgumentNameThrows) {
    ASSERT_THROW((clapp::argument::string_argument_t{tp, "", desc_cstr}),
                 clapp::exception::argument_exception_t);
}

TEST_F(argument_t, stringArgumentConstructStrAndCallArgFunc) {
    clapp::argument::string_argument_t arg{tp, arg_str, desc_str};
    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)(value_cstr)));
    ASSERT_THAT(arg, ArgumentGiven(value_cstr));
}

TEST_F(argument_t, stringArgumentConstructStrWithDefaultValueAndCallArgFunc) {
    clapp::argument::string_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::optional,
        clapp::value::default_value_t<std::string>{value_cstr}};
    ASSERT_THAT(arg, ArgumentNotGivenDefaultValue(value_cstr));
    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)(value_str)));
    ASSERT_THAT(arg, ArgumentGiven(value_str));
}

TEST_F(argument_t, stringArgumentConstructMandatoryStrAndCallGetOptionHelp) {
    clapp::argument::string_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THAT(tp.get_argument_help(),
                testing::ContainerEq(argument_test_parser_t::help_entry_vec_t{
                    {arg_str, desc_str + " (" + purpose_mandatory_str + ")"}}));
}

TEST_F(argument_t, stringArgumentConstructOptionalStrAndCallGetOptionHelp) {
    clapp::argument::string_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::optional};
    ASSERT_THAT(tp.get_argument_help(),
                testing::ContainerEq(argument_test_parser_t::help_entry_vec_t{
                    {arg_str, desc_str + " (" + purpose_optional_str + ")"}}));
}

TEST_F(argument_t,
       variadicStringArgumentConstructOptionalStrWithoutValidateFunc) {
    clapp::argument::string_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::optional};
    ASSERT_THAT(get_validate_func(tp, arg_str), testing::Eq(std::nullopt));
}

TEST_F(argument_t,
       variadicStringArgumentConstructMandatoryStrWithValidateFunc) {
    clapp::argument::string_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::mandatory};
    ASSERT_THROW(get_validate_func(tp, arg_str).value()(),
                 clapp::exception::argument_exception_t);
}

TEST_F(argument_t,
       variadicStringArgumentConstructStrAndCallValueReturnsEmptyVector) {
    clapp::argument::variadic_string_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THAT(tp, ContainsArgument(arg_str));
    ASSERT_THAT(arg, ArgumentNotGiven());
    ASSERT_THAT(arg.value(), testing::Eq(std::vector<std::string>{}));
}

TEST_F(argument_t,
       variadicStringArgumentConstructWithAlreadyRegisteredArgThrows) {
    clapp::argument::variadic_string_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THROW((clapp::argument::string_argument_t{tp, arg_str, desc_str}),
                 clapp::exception::argument_exception_t);
}

TEST_F(argument_t, variadicStringArgumentConstructWithEmpyArgumentNameThrows) {
    ASSERT_THROW(
        (clapp::argument::variadic_string_argument_t{tp, "", desc_cstr}),
        clapp::exception::argument_exception_t);
}

TEST_F(argument_t,
       variadicStringArgumentConstructStrOptionalAndCallArgFuncMultipleTimes) {
    clapp::argument::variadic_string_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::optional};
    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)(value_str)));
    ASSERT_THAT(arg,
                VariadicArgumentGiven(std::vector<std::string>{value_str}));
    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)(value_cstr)));
    ASSERT_THAT(arg, VariadicArgumentGiven(
                         std::vector<std::string>{value_str, value_cstr}));
}

TEST_F(argument_t, variadicStringArgumentConstructStrWithoutCallArgFunc) {
    clapp::argument::variadic_string_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THAT(arg, VariadicArgumentNotGiven());
}

TEST_F(argument_t,
       variadicStringArgumentConstructCStrAndCallArgFuncMultipleTimes) {
    clapp::argument::variadic_string_argument_t arg{tp, arg_cstr, desc_str};
    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_cstr)(value_cstr)));
    ASSERT_THAT(arg,
                VariadicArgumentGiven(std::vector<std::string>{value_cstr}));
    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_cstr)(value_str)));
    ASSERT_THAT(arg, VariadicArgumentGiven(
                         std::vector<std::string>{value_cstr, value_str}));
}

TEST_F(argument_t, variadicStringArgumentConstructCStrWithoutCallArgFunc) {
    clapp::argument::variadic_string_argument_t arg{tp, arg_cstr, desc_str};
    ASSERT_THAT(arg, VariadicArgumentNotGiven());
}

TEST_F(argument_t, variadicStringArgumentConstructStrAndCallGetOptionHelp) {
    clapp::argument::variadic_string_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THAT(tp.get_argument_help(),
                testing::ContainerEq(argument_test_parser_t::help_entry_vec_t{
                    {arg_str, desc_str + " (" + purpose_mandatory_str + ", " +
                                  variadic_argument_desc_restriction + ")"}}));
}

TEST_F(argument_t, variadicStringArgumentCallFoundFunc) {
    clapp::argument::variadic_string_argument_t arg{
        tp, arg_str, desc_str,
        clapp::value::found_func_t{[this]() { found_func_called++; }}};
    ASSERT_THAT(tp, ContainsArgument(arg_str));

    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)("1")));
    ASSERT_THAT(found_func_called, testing::Eq(1));

    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)("11")));
    ASSERT_THAT(found_func_called, testing::Eq(2));
}

TEST_F(argument_t, int64ArgumentConstructCStrAndCallValueThrows) {
    clapp::argument::int64_argument_t arg{tp, arg_cstr, desc_str};
    ASSERT_THAT(tp, ContainsArgument(arg_cstr));
    ASSERT_THAT(arg, ArgumentNotGiven());
    ASSERT_THROW(arg.value(), clapp::exception::value_undefined_t);
}

TEST_F(argument_t, int64ArgumentConstructStrAndCallValueThrows) {
    clapp::argument::int64_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THAT(tp, ContainsArgument(arg_str));
    ASSERT_THAT(arg, ArgumentNotGiven());
    ASSERT_THROW(arg.value(), clapp::exception::value_undefined_t);
}

TEST_F(argument_t, int64ArgumentConstructWithAlreadyRegisteredArgThrows) {
    clapp::argument::int64_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THROW((clapp::argument::int64_argument_t{tp, arg_str, desc_str}),
                 clapp::exception::argument_exception_t);
}

TEST_F(argument_t, int64ArgumentConstructOptionalStrWithoutValidateFunc) {
    clapp::argument::int64_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::optional};
    ASSERT_THAT(get_validate_func(tp, arg_str), testing::Eq(std::nullopt));
}

TEST_F(argument_t, int64ArgumentConstructMandatoryStrWithValidateFunc) {
    clapp::argument::int64_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::mandatory};
    ASSERT_THROW(get_validate_func(tp, arg_str).value()(),
                 clapp::exception::argument_exception_t);
}

TEST_F(argument_t, int64ArgumentCallFoundFunc) {
    clapp::argument::int64_argument_t arg{
        tp, arg_str, desc_str,
        clapp::value::found_func_t{[this]() { found_func_called++; }}};
    ASSERT_THAT(tp, ContainsArgument(arg_str));
    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)("1")));
    ASSERT_THAT(found_func_called, testing::Eq(1));
}

TEST_F(argument_t, int64ArgumentConstructWithEmpyArgumentNameThrows) {
    ASSERT_THROW((clapp::argument::int64_argument_t{tp, "", desc_cstr}),
                 clapp::exception::argument_exception_t);
}

TEST_F(argument_t, int64ArgumentConstructStrAndCallArgFunc) {
    clapp::argument::int64_argument_t arg{tp, arg_str, desc_str};
    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)(std::to_string(value_int64))));
    ASSERT_THAT(arg, ArgumentGiven(value_int64));
}

TEST_F(argument_t, int64ArgumentConstructStrWithDefaultValueAndCallArgFunc) {
    clapp::argument::int64_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::optional,
        clapp::value::default_value_t<std::int64_t>{value_int32}};
    ASSERT_THAT(arg, ArgumentNotGivenDefaultValue(value_int32));
    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)(std::to_string(value_int64))));
    ASSERT_THAT(arg, ArgumentGiven(value_int64));
}

TEST_F(argument_t, int64ArgumentConstructMandatoryStrAndCallGetOptionHelp) {
    clapp::argument::int64_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THAT(tp.get_argument_help(),
                testing::ContainerEq(argument_test_parser_t::help_entry_vec_t{
                    {arg_str, desc_str + " (" + purpose_mandatory_str + ")"}}));
}

TEST_F(argument_t, int64ArgumentConstructOptionalStrAndCallGetOptionHelp) {
    clapp::argument::int64_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::optional};
    ASSERT_THAT(tp.get_argument_help(),
                testing::ContainerEq(argument_test_parser_t::help_entry_vec_t{
                    {arg_str, desc_str + " (" + purpose_optional_str + ")"}}));
}

TEST_F(argument_t,
       variadicInt64ArgumentConstructOptionalStrWithoutValidateFunc) {
    clapp::argument::int64_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::optional};
    ASSERT_THAT(get_validate_func(tp, arg_str), testing::Eq(std::nullopt));
}

TEST_F(argument_t, variadicInt64ArgumentConstructMandatoryStrWithValidateFunc) {
    clapp::argument::int64_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::mandatory};
    ASSERT_THROW(get_validate_func(tp, arg_str).value()(),
                 clapp::exception::argument_exception_t);
}

TEST_F(argument_t,
       variadicInt64ArgumentConstructStrAndCallValueReturnsEmptyVector) {
    clapp::argument::variadic_int64_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THAT(tp, ContainsArgument(arg_str));
    ASSERT_THAT(arg, ArgumentNotGiven());
    ASSERT_THAT(arg.value(), testing::Eq(std::vector<std::int64_t>{}));
}

TEST_F(argument_t,
       variadicInt64ArgumentConstructWithAlreadyRegisteredArgThrows) {
    clapp::argument::variadic_int64_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THROW((clapp::argument::int64_argument_t{tp, arg_str, desc_str}),
                 clapp::exception::argument_exception_t);
}

TEST_F(argument_t, variadicInt64ArgumentConstructWithEmpyArgumentNameThrows) {
    ASSERT_THROW(
        (clapp::argument::variadic_int64_argument_t{tp, "", desc_cstr}),
        clapp::exception::argument_exception_t);
}

TEST_F(argument_t,
       variadicInt64ArgumentConstructStrOptionalAndCallArgFuncMultipleTimes) {
    clapp::argument::variadic_int64_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::optional};
    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)(std::to_string(value_int64))));
    ASSERT_THAT(arg,
                VariadicArgumentGiven(std::vector<std::int64_t>{value_int64}));
    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)(std::to_string(value_int16))));
    ASSERT_THAT(arg, VariadicArgumentGiven(
                         std::vector<std::int64_t>{value_int64, value_int16}));
}

TEST_F(argument_t, variadicInt64ArgumentConstructStrWithoutCallArgFunc) {
    clapp::argument::variadic_int64_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THAT(arg, VariadicArgumentNotGiven());
}

TEST_F(argument_t,
       variadicInt64ArgumentConstructStrAndCallArgFuncMultipleTimes) {
    clapp::argument::variadic_int64_argument_t arg{tp, arg_cstr, desc_str};
    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_cstr)(std::to_string(value_int32))));
    ASSERT_THAT(arg,
                VariadicArgumentGiven(std::vector<std::int64_t>{value_int32}));
    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_cstr)(std::to_string(value_int64))));
    ASSERT_THAT(arg, VariadicArgumentGiven(
                         std::vector<std::int64_t>{value_int32, value_int64}));
}

TEST_F(argument_t, variadicInt64ArgumentConstructCStrWithoutCallArgFunc) {
    clapp::argument::variadic_int64_argument_t arg{tp, arg_cstr, desc_str};
    ASSERT_THAT(arg, VariadicArgumentNotGiven());
}

TEST_F(argument_t,
       variadicInt64ArgumentConstructMandatoryStrAndCallGetOptionHelp) {
    clapp::argument::variadic_int64_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THAT(tp.get_argument_help(),
                testing::ContainerEq(argument_test_parser_t::help_entry_vec_t{
                    {arg_str, desc_str + " (" + purpose_mandatory_str + ", " +
                                  variadic_argument_desc_restriction + ")"}}));
}

TEST_F(argument_t, variadicInt64ArgumentCallFoundFunc) {
    clapp::argument::variadic_int64_argument_t arg{
        tp, arg_str, desc_str,
        clapp::value::found_func_t{[this]() { found_func_called++; }}};
    ASSERT_THAT(tp, ContainsArgument(arg_str));

    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)("1")));
    ASSERT_THAT(found_func_called, testing::Eq(1));

    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)("11")));
    ASSERT_THAT(found_func_called, testing::Eq(2));
}

TEST_F(argument_t, uint64ArgumentConstructCStrAndCallValueThrows) {
    clapp::argument::uint64_argument_t arg{tp, arg_cstr, desc_str};
    ASSERT_THAT(tp, ContainsArgument(arg_cstr));
    ASSERT_THAT(arg, ArgumentNotGiven());
    ASSERT_THROW(arg.value(), clapp::exception::value_undefined_t);
}

TEST_F(argument_t, uint64ArgumentConstructStrAndCallValueThrows) {
    clapp::argument::uint64_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THAT(tp, ContainsArgument(arg_str));
    ASSERT_THAT(arg, ArgumentNotGiven());
    ASSERT_THROW(arg.value(), clapp::exception::value_undefined_t);
}

TEST_F(argument_t, uint64ArgumentConstructWithAlreadyRegisteredArgThrows) {
    clapp::argument::uint64_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THROW((clapp::argument::uint64_argument_t{tp, arg_str, desc_str}),
                 clapp::exception::argument_exception_t);
}

TEST_F(argument_t, uint64ArgumentConstructOptionalStrWithoutValidateFunc) {
    clapp::argument::uint64_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::optional};
    ASSERT_THAT(get_validate_func(tp, arg_str), testing::Eq(std::nullopt));
}

TEST_F(argument_t, uint64ArgumentConstructMandatoryStrWithValidateFunc) {
    clapp::argument::uint64_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::mandatory};
    ASSERT_THROW(get_validate_func(tp, arg_str).value()(),
                 clapp::exception::argument_exception_t);
}

TEST_F(argument_t, uint64ArgumentCallFoundFunc) {
    clapp::argument::uint64_argument_t arg{
        tp, arg_str, desc_str,
        clapp::value::found_func_t{[this]() { found_func_called++; }}};
    ASSERT_THAT(tp, ContainsArgument(arg_str));
    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)("1")));
    ASSERT_THAT(found_func_called, testing::Eq(1));
}

TEST_F(argument_t, uint64ArgumentConstructWithEmpyArgumentNameThrows) {
    ASSERT_THROW((clapp::argument::uint64_argument_t{tp, "", desc_cstr}),
                 clapp::exception::argument_exception_t);
}

TEST_F(argument_t, uint64ArgumentConstructStrAndCallArgFunc) {
    clapp::argument::uint64_argument_t arg{tp, arg_str, desc_str};
    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)(std::to_string(value_uint64))));
    ASSERT_THAT(arg, ArgumentGiven(value_uint64));
}

TEST_F(argument_t, uint64ArgumentConstructStrWithDefaultValueAndCallArgFunc) {
    clapp::argument::uint64_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::optional,
        clapp::value::default_value_t<std::uint64_t>{value_uint32}};
    ASSERT_THAT(arg, ArgumentNotGivenDefaultValue(value_uint32));
    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)(std::to_string(value_uint64))));
    ASSERT_THAT(arg, ArgumentGiven(value_uint64));
}

TEST_F(argument_t, uint64ArgumentConstructMandatoryStrAndCallGetOptionHelp) {
    clapp::argument::uint64_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THAT(tp.get_argument_help(),
                testing::ContainerEq(argument_test_parser_t::help_entry_vec_t{
                    {arg_str, desc_str + " (" + purpose_mandatory_str + ")"}}));
}

TEST_F(argument_t, uint64ArgumentConstructOptionalStrAndCallGetOptionHelp) {
    clapp::argument::uint64_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::optional};
    ASSERT_THAT(tp.get_argument_help(),
                testing::ContainerEq(argument_test_parser_t::help_entry_vec_t{
                    {arg_str, desc_str + " (" + purpose_optional_str + ")"}}));
}

TEST_F(argument_t,
       variadicUint64ArgumentConstructOptionalStrWithoutValidateFunc) {
    clapp::argument::uint64_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::optional};
    ASSERT_THAT(get_validate_func(tp, arg_str), testing::Eq(std::nullopt));
}

TEST_F(argument_t,
       variadicUint64ArgumentConstructMandatoryStrWithValidateFunc) {
    clapp::argument::uint64_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::mandatory};
    ASSERT_THROW(get_validate_func(tp, arg_str).value()(),
                 clapp::exception::argument_exception_t);
}

TEST_F(argument_t,
       variadicUint64ArgumentConstructStrAndCallValueReturnsEmptyVector) {
    clapp::argument::variadic_uint64_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THAT(tp, ContainsArgument(arg_str));
    ASSERT_THAT(arg, ArgumentNotGiven());
    ASSERT_THAT(arg.value(), testing::Eq(std::vector<std::uint64_t>{}));
}

TEST_F(argument_t,
       variadicUint64ArgumentConstructWithAlreadyRegisteredArgThrows) {
    clapp::argument::variadic_uint64_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THROW((clapp::argument::uint64_argument_t{tp, arg_str, desc_str}),
                 clapp::exception::argument_exception_t);
}

TEST_F(argument_t, variadicUint64ArgumentConstructWithEmpyArgumentNameThrows) {
    ASSERT_THROW(
        (clapp::argument::variadic_uint64_argument_t{tp, "", desc_cstr}),
        clapp::exception::argument_exception_t);
}

TEST_F(argument_t,
       variadicUint64ArgumentConstructStrOptionalAndCallArgFuncMultipleTimes) {
    clapp::argument::variadic_uint64_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::optional};
    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)(std::to_string(value_uint64))));
    ASSERT_THAT(
        arg, VariadicArgumentGiven(std::vector<std::uint64_t>{value_uint64}));
    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)(std::to_string(value_int16))));
    ASSERT_THAT(arg, VariadicArgumentGiven(std::vector<std::uint64_t>{
                         value_uint64, value_int16}));
}

TEST_F(argument_t, variadicUint64ArgumentConstructStrWithoutCallArgFunc) {
    clapp::argument::variadic_uint64_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THAT(arg, VariadicArgumentNotGiven());
}

TEST_F(argument_t,
       variadicUint64ArgumentConstructStrAndCallArgFuncMultipleTimes) {
    clapp::argument::variadic_uint64_argument_t arg{tp, arg_cstr, desc_str};
    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_cstr)(std::to_string(value_uint32))));
    ASSERT_THAT(
        arg, VariadicArgumentGiven(std::vector<std::uint64_t>{value_uint32}));
    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_cstr)(std::to_string(value_uint64))));
    ASSERT_THAT(arg, VariadicArgumentGiven(std::vector<std::uint64_t>{
                         value_uint32, value_uint64}));
}

TEST_F(argument_t, variadicUint64ArgumentConstructCStrWithoutCallArgFunc) {
    clapp::argument::variadic_uint64_argument_t arg{tp, arg_cstr, desc_str};
    ASSERT_THAT(arg, VariadicArgumentNotGiven());
}

TEST_F(argument_t,
       variadicUint64ArgumentConstructMandatoryStrAndCallGetOptionHelp) {
    clapp::argument::variadic_uint64_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THAT(tp.get_argument_help(),
                testing::ContainerEq(argument_test_parser_t::help_entry_vec_t{
                    {arg_str, desc_str + " (" + purpose_mandatory_str + ", " +
                                  variadic_argument_desc_restriction + ")"}}));
}

TEST_F(argument_t, variadicUint64ArgumentCallFoundFunc) {
    clapp::argument::variadic_uint64_argument_t arg{
        tp, arg_str, desc_str,
        clapp::value::found_func_t{[this]() { found_func_called++; }}};
    ASSERT_THAT(tp, ContainsArgument(arg_str));

    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)("1")));
    ASSERT_THAT(found_func_called, testing::Eq(1));

    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)("11")));
    ASSERT_THAT(found_func_called, testing::Eq(2));
}

TEST_F(argument_t, int32ArgumentConstructCStrAndCallValueThrows) {
    clapp::argument::int32_argument_t arg{tp, arg_cstr, desc_str};
    ASSERT_THAT(tp, ContainsArgument(arg_cstr));
    ASSERT_THAT(arg, ArgumentNotGiven());
    ASSERT_THROW(arg.value(), clapp::exception::value_undefined_t);
}

TEST_F(argument_t, int32ArgumentConstructStrAndCallValueThrows) {
    clapp::argument::int32_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THAT(tp, ContainsArgument(arg_str));
    ASSERT_THAT(arg, ArgumentNotGiven());
    ASSERT_THROW(arg.value(), clapp::exception::value_undefined_t);
}

TEST_F(argument_t, int32ArgumentConstructWithAlreadyRegisteredArgThrows) {
    clapp::argument::int32_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THROW((clapp::argument::int32_argument_t{tp, arg_str, desc_str}),
                 clapp::exception::argument_exception_t);
}

TEST_F(argument_t, int32ArgumentConstructOptionalStrWithoutValidateFunc) {
    clapp::argument::int32_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::optional};
    ASSERT_THAT(get_validate_func(tp, arg_str), testing::Eq(std::nullopt));
}

TEST_F(argument_t, int32ArgumentConstructMandatoryStrWithValidateFunc) {
    clapp::argument::int32_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::mandatory};
    ASSERT_THROW(get_validate_func(tp, arg_str).value()(),
                 clapp::exception::argument_exception_t);
}

TEST_F(argument_t, int32ArgumentCallFoundFunc) {
    clapp::argument::int32_argument_t arg{
        tp, arg_str, desc_str,
        clapp::value::found_func_t{[this]() { found_func_called++; }}};
    ASSERT_THAT(tp, ContainsArgument(arg_str));
    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)("1")));
    ASSERT_THAT(found_func_called, testing::Eq(1));
}

TEST_F(argument_t, int32ArgumentConstructWithEmpyArgumentNameThrows) {
    ASSERT_THROW((clapp::argument::int32_argument_t{tp, "", desc_cstr}),
                 clapp::exception::argument_exception_t);
}

TEST_F(argument_t, int32ArgumentConstructStrAndCallArgFunc) {
    clapp::argument::int32_argument_t arg{tp, arg_str, desc_str};
    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)(std::to_string(value_int32))));
    ASSERT_THAT(arg, ArgumentGiven(value_int32));
}

TEST_F(argument_t, int32ArgumentConstructStrWithDefaultValueAndCallArgFunc) {
    clapp::argument::int32_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::optional,
        clapp::value::default_value_t<std::int32_t>{value_int16}};
    ASSERT_THAT(arg, ArgumentNotGivenDefaultValue(value_int16));
    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)(std::to_string(value_int32))));
    ASSERT_THAT(arg, ArgumentGiven(value_int32));
}

TEST_F(argument_t, int32ArgumentConstructMandatoryStrAndCallGetOptionHelp) {
    clapp::argument::int32_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THAT(tp.get_argument_help(),
                testing::ContainerEq(argument_test_parser_t::help_entry_vec_t{
                    {arg_str, desc_str + " (" + purpose_mandatory_str + ")"}}));
}

TEST_F(argument_t, int32ArgumentConstructOptionalStrAndCallGetOptionHelp) {
    clapp::argument::int32_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::optional};
    ASSERT_THAT(tp.get_argument_help(),
                testing::ContainerEq(argument_test_parser_t::help_entry_vec_t{
                    {arg_str, desc_str + " (" + purpose_optional_str + ")"}}));
}

TEST_F(argument_t,
       variadicInt32ArgumentConstructOptionalStrWithoutValidateFunc) {
    clapp::argument::int32_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::optional};
    ASSERT_THAT(get_validate_func(tp, arg_str), testing::Eq(std::nullopt));
}

TEST_F(argument_t, variadicInt32ArgumentConstructMandatoryStrWithValidateFunc) {
    clapp::argument::int32_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::mandatory};
    ASSERT_THROW(get_validate_func(tp, arg_str).value()(),
                 clapp::exception::argument_exception_t);
}

TEST_F(argument_t,
       variadicInt32ArgumentConstructStrAndCallValueReturnsEmptyVector) {
    clapp::argument::variadic_int32_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THAT(tp, ContainsArgument(arg_str));
    ASSERT_THAT(arg, ArgumentNotGiven());
    ASSERT_THAT(arg.value(), testing::Eq(std::vector<std::int32_t>{}));
}

TEST_F(argument_t,
       variadicInt32ArgumentConstructWithAlreadyRegisteredArgThrows) {
    clapp::argument::variadic_int32_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THROW((clapp::argument::int32_argument_t{tp, arg_str, desc_str}),
                 clapp::exception::argument_exception_t);
}

TEST_F(argument_t, variadicInt32ArgumentConstructWithEmpyArgumentNameThrows) {
    ASSERT_THROW(
        (clapp::argument::variadic_int32_argument_t{tp, "", desc_cstr}),
        clapp::exception::argument_exception_t);
}

TEST_F(argument_t,
       variadicInt32ArgumentConstructStrOptionalAndCallArgFuncMultipleTimes) {
    clapp::argument::variadic_int32_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::optional};
    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)(std::to_string(value_int32))));
    ASSERT_THAT(arg,
                VariadicArgumentGiven(std::vector<std::int32_t>{value_int32}));
    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)(std::to_string(value_int16))));
    ASSERT_THAT(arg, VariadicArgumentGiven(
                         std::vector<std::int32_t>{value_int32, value_int16}));
}

TEST_F(argument_t, variadicInt32ArgumentConstructStrWithoutCallArgFunc) {
    clapp::argument::variadic_int32_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THAT(arg, VariadicArgumentNotGiven());
}

TEST_F(argument_t,
       variadicInt32ArgumentConstructStrAndCallArgFuncMultipleTimes) {
    clapp::argument::variadic_int32_argument_t arg{tp, arg_cstr, desc_str};
    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_cstr)(std::to_string(value_int16))));
    ASSERT_THAT(arg,
                VariadicArgumentGiven(std::vector<std::int32_t>{value_int16}));
    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_cstr)(std::to_string(value_int32))));
    ASSERT_THAT(arg, VariadicArgumentGiven(
                         std::vector<std::int32_t>{value_int16, value_int32}));
}

TEST_F(argument_t, variadicInt32ArgumentConstructCStrWithoutCallArgFunc) {
    clapp::argument::variadic_int32_argument_t arg{tp, arg_cstr, desc_str};
    ASSERT_THAT(arg, VariadicArgumentNotGiven());
}

TEST_F(argument_t,
       variadicInt32ArgumentConstructMandatoryStrAndCallGetOptionHelp) {
    clapp::argument::variadic_int32_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THAT(tp.get_argument_help(),
                testing::ContainerEq(argument_test_parser_t::help_entry_vec_t{
                    {arg_str, desc_str + " (" + purpose_mandatory_str + ", " +
                                  variadic_argument_desc_restriction + ")"}}));
}

TEST_F(argument_t, variadicInt32ArgumentCallFoundFunc) {
    clapp::argument::variadic_int32_argument_t arg{
        tp, arg_str, desc_str,
        clapp::value::found_func_t{[this]() { found_func_called++; }}};
    ASSERT_THAT(tp, ContainsArgument(arg_str));

    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)("1")));
    ASSERT_THAT(found_func_called, testing::Eq(1));

    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)("11")));
    ASSERT_THAT(found_func_called, testing::Eq(2));
}

TEST_F(argument_t, uint32ArgumentConstructCStrAndCallValueThrows) {
    clapp::argument::uint32_argument_t arg{tp, arg_cstr, desc_str};
    ASSERT_THAT(tp, ContainsArgument(arg_cstr));
    ASSERT_THAT(arg, ArgumentNotGiven());
    ASSERT_THROW(arg.value(), clapp::exception::value_undefined_t);
}

TEST_F(argument_t, uint32ArgumentConstructStrAndCallValueThrows) {
    clapp::argument::uint32_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THAT(tp, ContainsArgument(arg_str));
    ASSERT_THAT(arg, ArgumentNotGiven());
    ASSERT_THROW(arg.value(), clapp::exception::value_undefined_t);
}

TEST_F(argument_t, uint32ArgumentConstructWithAlreadyRegisteredArgThrows) {
    clapp::argument::uint32_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THROW((clapp::argument::uint32_argument_t{tp, arg_str, desc_str}),
                 clapp::exception::argument_exception_t);
}

TEST_F(argument_t, uint32ArgumentConstructOptionalStrWithoutValidateFunc) {
    clapp::argument::uint32_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::optional};
    ASSERT_THAT(get_validate_func(tp, arg_str), testing::Eq(std::nullopt));
}

TEST_F(argument_t, uint32ArgumentConstructMandatoryStrWithValidateFunc) {
    clapp::argument::uint32_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::mandatory};
    ASSERT_THROW(get_validate_func(tp, arg_str).value()(),
                 clapp::exception::argument_exception_t);
}

TEST_F(argument_t, uint32ArgumentCallFoundFunc) {
    clapp::argument::uint32_argument_t arg{
        tp, arg_str, desc_str,
        clapp::value::found_func_t{[this]() { found_func_called++; }}};
    ASSERT_THAT(tp, ContainsArgument(arg_str));
    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)("1")));
    ASSERT_THAT(found_func_called, testing::Eq(1));
}

TEST_F(argument_t, uint32ArgumentConstructWithEmpyArgumentNameThrows) {
    ASSERT_THROW((clapp::argument::uint32_argument_t{tp, "", desc_cstr}),
                 clapp::exception::argument_exception_t);
}

TEST_F(argument_t, uint32ArgumentConstructStrAndCallArgFunc) {
    clapp::argument::uint32_argument_t arg{tp, arg_str, desc_str};
    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)(std::to_string(value_uint32))));
    ASSERT_THAT(arg, ArgumentGiven(value_uint32));
}

TEST_F(argument_t, uint32ArgumentConstructStrWithDefaultValueAndCallArgFunc) {
    clapp::argument::uint32_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::optional,
        clapp::value::default_value_t<std::uint32_t>{value_uint16}};
    ASSERT_THAT(arg, ArgumentNotGivenDefaultValue(value_uint16));
    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)(std::to_string(value_uint32))));
    ASSERT_THAT(arg, ArgumentGiven(value_uint32));
}

TEST_F(argument_t, uint32ArgumentConstructMandatoryStrAndCallGetOptionHelp) {
    clapp::argument::uint32_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THAT(tp.get_argument_help(),
                testing::ContainerEq(argument_test_parser_t::help_entry_vec_t{
                    {arg_str, desc_str + " (" + purpose_mandatory_str + ")"}}));
}

TEST_F(argument_t, uint32ArgumentConstructOptionalStrAndCallGetOptionHelp) {
    clapp::argument::uint32_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::optional};
    ASSERT_THAT(tp.get_argument_help(),
                testing::ContainerEq(argument_test_parser_t::help_entry_vec_t{
                    {arg_str, desc_str + " (" + purpose_optional_str + ")"}}));
}

TEST_F(argument_t,
       variadicUint32ArgumentConstructOptionalStrWithoutValidateFunc) {
    clapp::argument::uint32_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::optional};
    ASSERT_THAT(get_validate_func(tp, arg_str), testing::Eq(std::nullopt));
}

TEST_F(argument_t,
       variadicUint32ArgumentConstructMandatoryStrWithValidateFunc) {
    clapp::argument::uint32_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::mandatory};
    ASSERT_THROW(get_validate_func(tp, arg_str).value()(),
                 clapp::exception::argument_exception_t);
}

TEST_F(argument_t,
       variadicUint32ArgumentConstructStrAndCallValueReturnsEmptyVector) {
    clapp::argument::variadic_uint32_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THAT(tp, ContainsArgument(arg_str));
    ASSERT_THAT(arg, ArgumentNotGiven());
    ASSERT_THAT(arg.value(), testing::Eq(std::vector<std::uint32_t>{}));
}

TEST_F(argument_t,
       variadicUint32ArgumentConstructWithAlreadyRegisteredArgThrows) {
    clapp::argument::variadic_uint32_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THROW((clapp::argument::uint32_argument_t{tp, arg_str, desc_str}),
                 clapp::exception::argument_exception_t);
}

TEST_F(argument_t, variadicUint32ArgumentConstructWithEmpyArgumentNameThrows) {
    ASSERT_THROW(
        (clapp::argument::variadic_uint32_argument_t{tp, "", desc_cstr}),
        clapp::exception::argument_exception_t);
}

TEST_F(argument_t,
       variadicUint32ArgumentConstructStrOptionalAndCallArgFuncMultipleTimes) {
    clapp::argument::variadic_uint32_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::optional};
    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)(std::to_string(value_uint32))));
    ASSERT_THAT(
        arg, VariadicArgumentGiven(std::vector<std::uint32_t>{value_uint32}));
    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)(std::to_string(value_int16))));
    ASSERT_THAT(arg, VariadicArgumentGiven(std::vector<std::uint32_t>{
                         value_uint32, value_int16}));
}

TEST_F(argument_t, variadicUint32ArgumentConstructStrWithoutCallArgFunc) {
    clapp::argument::variadic_uint32_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THAT(arg, VariadicArgumentNotGiven());
}

TEST_F(argument_t,
       variadicUint32ArgumentConstructStrAndCallArgFuncMultipleTimes) {
    clapp::argument::variadic_uint32_argument_t arg{tp, arg_cstr, desc_str};
    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_cstr)(std::to_string(value_uint16))));
    ASSERT_THAT(
        arg, VariadicArgumentGiven(std::vector<std::uint32_t>{value_uint16}));
    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_cstr)(std::to_string(value_uint32))));
    ASSERT_THAT(arg, VariadicArgumentGiven(std::vector<std::uint32_t>{
                         value_uint16, value_uint32}));
}

TEST_F(argument_t, variadicUint32ArgumentConstructCStrWithoutCallArgFunc) {
    clapp::argument::variadic_uint32_argument_t arg{tp, arg_cstr, desc_str};
    ASSERT_THAT(arg, VariadicArgumentNotGiven());
}

TEST_F(argument_t,
       variadicUint32ArgumentConstructMandatoryStrAndCallGetOptionHelp) {
    clapp::argument::variadic_uint32_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THAT(tp.get_argument_help(),
                testing::ContainerEq(argument_test_parser_t::help_entry_vec_t{
                    {arg_str, desc_str + " (" + purpose_mandatory_str + ", " +
                                  variadic_argument_desc_restriction + ")"}}));
}

TEST_F(argument_t, variadicUint32ArgumentCallFoundFunc) {
    clapp::argument::variadic_uint32_argument_t arg{
        tp, arg_str, desc_str,
        clapp::value::found_func_t{[this]() { found_func_called++; }}};
    ASSERT_THAT(tp, ContainsArgument(arg_str));

    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)("1")));
    ASSERT_THAT(found_func_called, testing::Eq(1));

    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)("11")));
    ASSERT_THAT(found_func_called, testing::Eq(2));
}

TEST_F(argument_t, int16ArgumentConstructCStrAndCallValueThrows) {
    clapp::argument::int16_argument_t arg{tp, arg_cstr, desc_str};
    ASSERT_THAT(tp, ContainsArgument(arg_cstr));
    ASSERT_THAT(arg, ArgumentNotGiven());
    ASSERT_THROW(arg.value(), clapp::exception::value_undefined_t);
}

TEST_F(argument_t, int16ArgumentConstructStrAndCallValueThrows) {
    clapp::argument::int16_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THAT(tp, ContainsArgument(arg_str));
    ASSERT_THAT(arg, ArgumentNotGiven());
    ASSERT_THROW(arg.value(), clapp::exception::value_undefined_t);
}

TEST_F(argument_t, int16ArgumentConstructWithAlreadyRegisteredArgThrows) {
    clapp::argument::int16_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THROW((clapp::argument::int16_argument_t{tp, arg_str, desc_str}),
                 clapp::exception::argument_exception_t);
}

TEST_F(argument_t, int16ArgumentConstructOptionalStrWithoutValidateFunc) {
    clapp::argument::int16_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::optional};
    ASSERT_THAT(get_validate_func(tp, arg_str), testing::Eq(std::nullopt));
}

TEST_F(argument_t, int16ArgumentConstructMandatoryStrWithValidateFunc) {
    clapp::argument::int16_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::mandatory};
    ASSERT_THROW(get_validate_func(tp, arg_str).value()(),
                 clapp::exception::argument_exception_t);
}

TEST_F(argument_t, int16ArgumentCallFoundFunc) {
    clapp::argument::int16_argument_t arg{
        tp, arg_str, desc_str,
        clapp::value::found_func_t{[this]() { found_func_called++; }}};
    ASSERT_THAT(tp, ContainsArgument(arg_str));
    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)("1")));
    ASSERT_THAT(found_func_called, testing::Eq(1));
}

TEST_F(argument_t, int16ArgumentConstructWithEmpyArgumentNameThrows) {
    ASSERT_THROW((clapp::argument::int16_argument_t{tp, "", desc_cstr}),
                 clapp::exception::argument_exception_t);
}

TEST_F(argument_t, int16ArgumentConstructStrAndCallArgFunc) {
    clapp::argument::int16_argument_t arg{tp, arg_str, desc_str};
    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)(std::to_string(value_int16))));
    ASSERT_THAT(arg, ArgumentGiven(value_int16));
}

TEST_F(argument_t, int16ArgumentConstructStrWithDefaultValueAndCallArgFunc) {
    clapp::argument::int16_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::optional,
        clapp::value::default_value_t<std::int16_t>{value_int8}};
    ASSERT_THAT(arg, ArgumentNotGivenDefaultValue(value_int8));
    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)(std::to_string(value_int16))));
    ASSERT_THAT(arg, ArgumentGiven(value_int16));
}

TEST_F(argument_t, int16ArgumentConstructMandatoryStrAndCallGetOptionHelp) {
    clapp::argument::int16_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THAT(tp.get_argument_help(),
                testing::ContainerEq(argument_test_parser_t::help_entry_vec_t{
                    {arg_str, desc_str + " (" + purpose_mandatory_str + ")"}}));
}

TEST_F(argument_t, int16ArgumentConstructOptionalStrAndCallGetOptionHelp) {
    clapp::argument::int16_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::optional};
    ASSERT_THAT(tp.get_argument_help(),
                testing::ContainerEq(argument_test_parser_t::help_entry_vec_t{
                    {arg_str, desc_str + " (" + purpose_optional_str + ")"}}));
}

TEST_F(argument_t,
       variadicInt16ArgumentConstructOptionalStrWithoutValidateFunc) {
    clapp::argument::int16_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::optional};
    ASSERT_THAT(get_validate_func(tp, arg_str), testing::Eq(std::nullopt));
}

TEST_F(argument_t, variadicInt16ArgumentConstructMandatoryStrWithValidateFunc) {
    clapp::argument::int16_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::mandatory};
    ASSERT_THROW(get_validate_func(tp, arg_str).value()(),
                 clapp::exception::argument_exception_t);
}

TEST_F(argument_t,
       variadicInt16ArgumentConstructStrAndCallValueReturnsEmptyVector) {
    clapp::argument::variadic_int16_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THAT(tp, ContainsArgument(arg_str));
    ASSERT_THAT(arg, ArgumentNotGiven());
    ASSERT_THAT(arg.value(), testing::Eq(std::vector<std::int16_t>{}));
}

TEST_F(argument_t,
       variadicInt16ArgumentConstructWithAlreadyRegisteredArgThrows) {
    clapp::argument::variadic_int16_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THROW((clapp::argument::int16_argument_t{tp, arg_str, desc_str}),
                 clapp::exception::argument_exception_t);
}

TEST_F(argument_t, variadicInt16ArgumentConstructWithEmpyArgumentNameThrows) {
    ASSERT_THROW(
        (clapp::argument::variadic_int16_argument_t{tp, "", desc_cstr}),
        clapp::exception::argument_exception_t);
}

TEST_F(argument_t,
       variadicInt16ArgumentConstructStrOptionalAndCallArgFuncMultipleTimes) {
    clapp::argument::variadic_int16_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::optional};
    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)(std::to_string(value_int16))));
    ASSERT_THAT(arg,
                VariadicArgumentGiven(std::vector<std::int16_t>{value_int16}));
    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)(std::to_string(value_int8))));
    ASSERT_THAT(arg, VariadicArgumentGiven(
                         std::vector<std::int16_t>{value_int16, value_int8}));
}

TEST_F(argument_t, variadicInt16ArgumentConstructStrWithoutCallArgFunc) {
    clapp::argument::variadic_int16_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THAT(arg, VariadicArgumentNotGiven());
}

TEST_F(argument_t,
       variadicInt16ArgumentConstructStrAndCallArgFuncMultipleTimes) {
    clapp::argument::variadic_int16_argument_t arg{tp, arg_cstr, desc_str};
    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_cstr)(std::to_string(value_int8))));
    ASSERT_THAT(arg,
                VariadicArgumentGiven(std::vector<std::int16_t>{value_int8}));
    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_cstr)(std::to_string(value_int16))));
    ASSERT_THAT(arg, VariadicArgumentGiven(
                         std::vector<std::int16_t>{value_int8, value_int16}));
}

TEST_F(argument_t, variadicInt16ArgumentConstructCStrWithoutCallArgFunc) {
    clapp::argument::variadic_int16_argument_t arg{tp, arg_cstr, desc_str};
    ASSERT_THAT(arg, VariadicArgumentNotGiven());
}

TEST_F(argument_t,
       variadicInt16ArgumentConstructMandatoryStrAndCallGetOptionHelp) {
    clapp::argument::variadic_int16_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THAT(tp.get_argument_help(),
                testing::ContainerEq(argument_test_parser_t::help_entry_vec_t{
                    {arg_str, desc_str + " (" + purpose_mandatory_str + ", " +
                                  variadic_argument_desc_restriction + ")"}}));
}

TEST_F(argument_t, variadicInt16ArgumentCallFoundFunc) {
    clapp::argument::variadic_int16_argument_t arg{
        tp, arg_str, desc_str,
        clapp::value::found_func_t{[this]() { found_func_called++; }}};
    ASSERT_THAT(tp, ContainsArgument(arg_str));

    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)("1")));
    ASSERT_THAT(found_func_called, testing::Eq(1));

    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)("11")));
    ASSERT_THAT(found_func_called, testing::Eq(2));
}

TEST_F(argument_t, uint16ArgumentConstructCStrAndCallValueThrows) {
    clapp::argument::uint16_argument_t arg{tp, arg_cstr, desc_str};
    ASSERT_THAT(tp, ContainsArgument(arg_cstr));
    ASSERT_THAT(arg, ArgumentNotGiven());
    ASSERT_THROW(arg.value(), clapp::exception::value_undefined_t);
}

TEST_F(argument_t, uint16ArgumentConstructStrAndCallValueThrows) {
    clapp::argument::uint16_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THAT(tp, ContainsArgument(arg_str));
    ASSERT_THAT(arg, ArgumentNotGiven());
    ASSERT_THROW(arg.value(), clapp::exception::value_undefined_t);
}

TEST_F(argument_t, uint16ArgumentConstructWithAlreadyRegisteredArgThrows) {
    clapp::argument::uint16_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THROW((clapp::argument::uint16_argument_t{tp, arg_str, desc_str}),
                 clapp::exception::argument_exception_t);
}

TEST_F(argument_t, uint16ArgumentConstructOptionalStrWithoutValidateFunc) {
    clapp::argument::uint16_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::optional};
    ASSERT_THAT(get_validate_func(tp, arg_str), testing::Eq(std::nullopt));
}

TEST_F(argument_t, uint16ArgumentConstructMandatoryStrWithValidateFunc) {
    clapp::argument::uint16_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::mandatory};
    ASSERT_THROW(get_validate_func(tp, arg_str).value()(),
                 clapp::exception::argument_exception_t);
}

TEST_F(argument_t, uint16ArgumentCallFoundFunc) {
    clapp::argument::uint16_argument_t arg{
        tp, arg_str, desc_str,
        clapp::value::found_func_t{[this]() { found_func_called++; }}};
    ASSERT_THAT(tp, ContainsArgument(arg_str));
    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)("1")));
    ASSERT_THAT(found_func_called, testing::Eq(1));
}

TEST_F(argument_t, uint16ArgumentConstructWithEmpyArgumentNameThrows) {
    ASSERT_THROW((clapp::argument::uint16_argument_t{tp, "", desc_cstr}),
                 clapp::exception::argument_exception_t);
}

TEST_F(argument_t, uint16ArgumentConstructStrAndCallArgFunc) {
    clapp::argument::uint16_argument_t arg{tp, arg_str, desc_str};
    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)(std::to_string(value_uint16))));
    ASSERT_THAT(arg, ArgumentGiven(value_uint16));
}

TEST_F(argument_t, uint16ArgumentConstructStrWithDefaultValueAndCallArgFunc) {
    clapp::argument::uint16_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::optional,
        clapp::value::default_value_t<std::uint16_t>{value_uint8}};
    ASSERT_THAT(arg, ArgumentNotGivenDefaultValue(value_uint8));
    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)(std::to_string(value_uint16))));
    ASSERT_THAT(arg, ArgumentGiven(value_uint16));
}

TEST_F(argument_t, uint16ArgumentConstructMandatoryStrAndCallGetOptionHelp) {
    clapp::argument::uint16_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THAT(tp.get_argument_help(),
                testing::ContainerEq(argument_test_parser_t::help_entry_vec_t{
                    {arg_str, desc_str + " (" + purpose_mandatory_str + ")"}}));
}

TEST_F(argument_t, uint16ArgumentConstructOptionalStrAndCallGetOptionHelp) {
    clapp::argument::uint16_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::optional};
    ASSERT_THAT(tp.get_argument_help(),
                testing::ContainerEq(argument_test_parser_t::help_entry_vec_t{
                    {arg_str, desc_str + " (" + purpose_optional_str + ")"}}));
}

TEST_F(argument_t,
       variadicUint16ArgumentConstructOptionalStrWithoutValidateFunc) {
    clapp::argument::uint16_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::optional};
    ASSERT_THAT(get_validate_func(tp, arg_str), testing::Eq(std::nullopt));
}

TEST_F(argument_t,
       variadicUint16ArgumentConstructMandatoryStrWithValidateFunc) {
    clapp::argument::uint16_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::mandatory};
    ASSERT_THROW(get_validate_func(tp, arg_str).value()(),
                 clapp::exception::argument_exception_t);
}

TEST_F(argument_t,
       variadicUint16ArgumentConstructStrAndCallValueReturnsEmptyVector) {
    clapp::argument::variadic_uint16_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THAT(tp, ContainsArgument(arg_str));
    ASSERT_THAT(arg, ArgumentNotGiven());
    ASSERT_THAT(arg.value(), testing::Eq(std::vector<std::uint16_t>{}));
}

TEST_F(argument_t,
       variadicUint16ArgumentConstructWithAlreadyRegisteredArgThrows) {
    clapp::argument::variadic_uint16_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THROW((clapp::argument::uint16_argument_t{tp, arg_str, desc_str}),
                 clapp::exception::argument_exception_t);
}

TEST_F(argument_t, variadicUint16ArgumentConstructWithEmpyArgumentNameThrows) {
    ASSERT_THROW(
        (clapp::argument::variadic_uint16_argument_t{tp, "", desc_cstr}),
        clapp::exception::argument_exception_t);
}

TEST_F(argument_t,
       variadicUint16ArgumentConstructStrOptionalAndCallArgFuncMultipleTimes) {
    clapp::argument::variadic_uint16_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::optional};
    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)(std::to_string(value_uint16))));
    ASSERT_THAT(
        arg, VariadicArgumentGiven(std::vector<std::uint16_t>{value_uint16}));
    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)(std::to_string(value_int8))));
    ASSERT_THAT(arg, VariadicArgumentGiven(
                         std::vector<std::uint16_t>{value_uint16, value_int8}));
}

TEST_F(argument_t, variadicUint16ArgumentConstructStrWithoutCallArgFunc) {
    clapp::argument::variadic_uint16_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THAT(arg, VariadicArgumentNotGiven());
}

TEST_F(argument_t,
       variadicUint16ArgumentConstructStrAndCallArgFuncMultipleTimes) {
    clapp::argument::variadic_uint16_argument_t arg{tp, arg_cstr, desc_str};
    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_cstr)(std::to_string(value_uint8))));
    ASSERT_THAT(arg,
                VariadicArgumentGiven(std::vector<std::uint16_t>{value_uint8}));
    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_cstr)(std::to_string(value_uint16))));
    ASSERT_THAT(arg, VariadicArgumentGiven(std::vector<std::uint16_t>{
                         value_uint8, value_uint16}));
}

TEST_F(argument_t, variadicUint16ArgumentConstructCStrWithoutCallArgFunc) {
    clapp::argument::variadic_uint16_argument_t arg{tp, arg_cstr, desc_str};
    ASSERT_THAT(arg, VariadicArgumentNotGiven());
}

TEST_F(argument_t,
       variadicUint16ArgumentConstructMandatoryStrAndCallGetOptionHelp) {
    clapp::argument::variadic_uint16_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THAT(tp.get_argument_help(),
                testing::ContainerEq(argument_test_parser_t::help_entry_vec_t{
                    {arg_str, desc_str + " (" + purpose_mandatory_str + ", " +
                                  variadic_argument_desc_restriction + ")"}}));
}

TEST_F(argument_t, variadicUint16ArgumentCallFoundFunc) {
    clapp::argument::variadic_uint16_argument_t arg{
        tp, arg_str, desc_str,
        clapp::value::found_func_t{[this]() { found_func_called++; }}};
    ASSERT_THAT(tp, ContainsArgument(arg_str));

    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)("1")));
    ASSERT_THAT(found_func_called, testing::Eq(1));

    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)("11")));
    ASSERT_THAT(found_func_called, testing::Eq(2));
}

TEST_F(argument_t, int8ArgumentConstructCStrAndCallValueThrows) {
    clapp::argument::int8_argument_t arg{tp, arg_cstr, desc_str};
    ASSERT_THAT(tp, ContainsArgument(arg_cstr));
    ASSERT_THAT(arg, ArgumentNotGiven());
    ASSERT_THROW(arg.value(), clapp::exception::value_undefined_t);
}

TEST_F(argument_t, int8ArgumentConstructStrAndCallValueThrows) {
    clapp::argument::int8_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THAT(tp, ContainsArgument(arg_str));
    ASSERT_THAT(arg, ArgumentNotGiven());
    ASSERT_THROW(arg.value(), clapp::exception::value_undefined_t);
}

TEST_F(argument_t, int8ArgumentConstructWithAlreadyRegisteredArgThrows) {
    clapp::argument::int8_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THROW((clapp::argument::int8_argument_t{tp, arg_str, desc_str}),
                 clapp::exception::argument_exception_t);
}

TEST_F(argument_t, int8ArgumentConstructOptionalStrWithoutValidateFunc) {
    clapp::argument::int8_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::optional};
    ASSERT_THAT(get_validate_func(tp, arg_str), testing::Eq(std::nullopt));
}

TEST_F(argument_t, int8ArgumentConstructMandatoryStrWithValidateFunc) {
    clapp::argument::int8_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::mandatory};
    ASSERT_THROW(get_validate_func(tp, arg_str).value()(),
                 clapp::exception::argument_exception_t);
}

TEST_F(argument_t, int8ArgumentCallFoundFunc) {
    clapp::argument::int8_argument_t arg{
        tp, arg_str, desc_str,
        clapp::value::found_func_t{[this]() { found_func_called++; }}};
    ASSERT_THAT(tp, ContainsArgument(arg_str));
    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)("1")));
    ASSERT_THAT(found_func_called, testing::Eq(1));
}

TEST_F(argument_t, int8ArgumentConstructWithEmpyArgumentNameThrows) {
    ASSERT_THROW((clapp::argument::int8_argument_t{tp, "", desc_cstr}),
                 clapp::exception::argument_exception_t);
}

TEST_F(argument_t, int8ArgumentConstructStrAndCallArgFunc) {
    clapp::argument::int8_argument_t arg{tp, arg_str, desc_str};
    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)(std::to_string(value_int8))));
    ASSERT_THAT(arg, ArgumentGiven(value_int8));
}

TEST_F(argument_t, int8ArgumentConstructStrWithDefaultValueAndCallArgFunc) {
    clapp::argument::int8_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::optional,
        clapp::value::default_value_t<std::int8_t>{value_int8_additional}};
    ASSERT_THAT(arg, ArgumentNotGivenDefaultValue(value_int8_additional));
    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)(std::to_string(value_int8))));
    ASSERT_THAT(arg, ArgumentGiven(value_int8));
}

TEST_F(argument_t, int8ArgumentConstructMandatoryStrAndCallGetOptionHelp) {
    clapp::argument::int8_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THAT(tp.get_argument_help(),
                testing::ContainerEq(argument_test_parser_t::help_entry_vec_t{
                    {arg_str, desc_str + " (" + purpose_mandatory_str + ")"}}));
}

TEST_F(argument_t, int8ArgumentConstructOptionalStrAndCallGetOptionHelp) {
    clapp::argument::int8_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::optional};
    ASSERT_THAT(tp.get_argument_help(),
                testing::ContainerEq(argument_test_parser_t::help_entry_vec_t{
                    {arg_str, desc_str + " (" + purpose_optional_str + ")"}}));
}

TEST_F(argument_t,
       variadicInt8ArgumentConstructOptionalStrWithoutValidateFunc) {
    clapp::argument::int8_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::optional};
    ASSERT_THAT(get_validate_func(tp, arg_str), testing::Eq(std::nullopt));
}

TEST_F(argument_t, variadicInt8ArgumentConstructMandatoryStrWithValidateFunc) {
    clapp::argument::int8_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::mandatory};
    ASSERT_THROW(get_validate_func(tp, arg_str).value()(),
                 clapp::exception::argument_exception_t);
}

TEST_F(argument_t,
       variadicInt8ArgumentConstructStrAndCallValueReturnsEmptyVector) {
    clapp::argument::variadic_int8_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THAT(tp, ContainsArgument(arg_str));
    ASSERT_THAT(arg, ArgumentNotGiven());
    ASSERT_THAT(arg.value(), testing::Eq(std::vector<std::int8_t>{}));
}

TEST_F(argument_t,
       variadicInt8ArgumentConstructWithAlreadyRegisteredArgThrows) {
    clapp::argument::variadic_int8_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THROW((clapp::argument::int8_argument_t{tp, arg_str, desc_str}),
                 clapp::exception::argument_exception_t);
}

TEST_F(argument_t, variadicInt8ArgumentConstructWithEmpyArgumentNameThrows) {
    ASSERT_THROW((clapp::argument::variadic_int8_argument_t{tp, "", desc_cstr}),
                 clapp::exception::argument_exception_t);
}

TEST_F(argument_t,
       variadicInt8ArgumentConstructStrOptionalAndCallArgFuncMultipleTimes) {
    clapp::argument::variadic_int8_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::optional};
    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)(std::to_string(value_int8))));
    ASSERT_THAT(arg,
                VariadicArgumentGiven(std::vector<std::int8_t>{value_int8}));
    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)(std::to_string(value_int8_additional))));
    ASSERT_THAT(arg, VariadicArgumentGiven(std::vector<std::int8_t>{
                         value_int8, value_int8_additional}));
}

TEST_F(argument_t, variadicInt8ArgumentConstructStrWithoutCallArgFunc) {
    clapp::argument::variadic_int8_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THAT(arg, VariadicArgumentNotGiven());
}

TEST_F(argument_t,
       variadicInt8ArgumentConstructStrAndCallArgFuncMultipleTimes) {
    clapp::argument::variadic_int8_argument_t arg{tp, arg_cstr, desc_str};
    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_cstr)(std::to_string(value_int8_additional))));
    ASSERT_THAT(arg, VariadicArgumentGiven(
                         std::vector<std::int8_t>{value_int8_additional}));
    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_cstr)(std::to_string(value_int8))));
    ASSERT_THAT(arg, VariadicArgumentGiven(std::vector<std::int8_t>{
                         value_int8_additional, value_int8}));
}

TEST_F(argument_t, variadicInt8ArgumentConstructCStrWithoutCallArgFunc) {
    clapp::argument::variadic_int8_argument_t arg{tp, arg_cstr, desc_str};
    ASSERT_THAT(arg, VariadicArgumentNotGiven());
}

TEST_F(argument_t,
       variadicInt8ArgumentConstructMandatoryStrAndCallGetOptionHelp) {
    clapp::argument::variadic_int8_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THAT(tp.get_argument_help(),
                testing::ContainerEq(argument_test_parser_t::help_entry_vec_t{
                    {arg_str, desc_str + " (" + purpose_mandatory_str + ", " +
                                  variadic_argument_desc_restriction + ")"}}));
}

TEST_F(argument_t, variadicInt8ArgumentCallFoundFunc) {
    clapp::argument::variadic_int8_argument_t arg{
        tp, arg_str, desc_str,
        clapp::value::found_func_t{[this]() { found_func_called++; }}};
    ASSERT_THAT(tp, ContainsArgument(arg_str));

    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)("1")));
    ASSERT_THAT(found_func_called, testing::Eq(1));

    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)("11")));
    ASSERT_THAT(found_func_called, testing::Eq(2));
}

TEST_F(argument_t, uint8ArgumentConstructCStrAndCallValueThrows) {
    clapp::argument::uint8_argument_t arg{tp, arg_cstr, desc_str};
    ASSERT_THAT(tp, ContainsArgument(arg_cstr));
    ASSERT_THAT(arg, ArgumentNotGiven());
    ASSERT_THROW(arg.value(), clapp::exception::value_undefined_t);
}

TEST_F(argument_t, uint8ArgumentConstructStrAndCallValueThrows) {
    clapp::argument::uint8_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THAT(tp, ContainsArgument(arg_str));
    ASSERT_THAT(arg, ArgumentNotGiven());
    ASSERT_THROW(arg.value(), clapp::exception::value_undefined_t);
}

TEST_F(argument_t, uint8ArgumentConstructWithAlreadyRegisteredArgThrows) {
    clapp::argument::uint8_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THROW((clapp::argument::uint8_argument_t{tp, arg_str, desc_str}),
                 clapp::exception::argument_exception_t);
}

TEST_F(argument_t, uint8ArgumentConstructOptionalStrWithoutValidateFunc) {
    clapp::argument::uint8_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::optional};
    ASSERT_THAT(get_validate_func(tp, arg_str), testing::Eq(std::nullopt));
}

TEST_F(argument_t, uint8ArgumentConstructMandatoryStrWithValidateFunc) {
    clapp::argument::uint8_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::mandatory};
    ASSERT_THROW(get_validate_func(tp, arg_str).value()(),
                 clapp::exception::argument_exception_t);
}

TEST_F(argument_t, uint8ArgumentCallFoundFunc) {
    clapp::argument::uint8_argument_t arg{
        tp, arg_str, desc_str,
        clapp::value::found_func_t{[this]() { found_func_called++; }}};
    ASSERT_THAT(tp, ContainsArgument(arg_str));
    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)("1")));
    ASSERT_THAT(found_func_called, testing::Eq(1));
}

TEST_F(argument_t, uint8ArgumentConstructWithEmpyArgumentNameThrows) {
    ASSERT_THROW((clapp::argument::uint8_argument_t{tp, "", desc_cstr}),
                 clapp::exception::argument_exception_t);
}

TEST_F(argument_t, uint8ArgumentConstructStrAndCallArgFunc) {
    clapp::argument::uint8_argument_t arg{tp, arg_str, desc_str};
    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)(std::to_string(value_uint8))));
    ASSERT_THAT(arg, ArgumentGiven(value_uint8));
}

TEST_F(argument_t, uint8ArgumentConstructStrWithDefaultValueAndCallArgFunc) {
    clapp::argument::uint8_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::optional,
        clapp::value::default_value_t<std::uint8_t>{value_uint8_additional}};
    ASSERT_THAT(arg, ArgumentNotGivenDefaultValue(value_uint8_additional));
    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)(std::to_string(value_uint8))));
    ASSERT_THAT(arg, ArgumentGiven(value_uint8));
}

TEST_F(argument_t, uint8ArgumentConstructMandatoryStrAndCallGetOptionHelp) {
    clapp::argument::uint8_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THAT(tp.get_argument_help(),
                testing::ContainerEq(argument_test_parser_t::help_entry_vec_t{
                    {arg_str, desc_str + " (" + purpose_mandatory_str + ")"}}));
}

TEST_F(argument_t, uint8ArgumentConstructOptionalStrAndCallGetOptionHelp) {
    clapp::argument::uint8_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::optional};
    ASSERT_THAT(tp.get_argument_help(),
                testing::ContainerEq(argument_test_parser_t::help_entry_vec_t{
                    {arg_str, desc_str + " (" + purpose_optional_str + ")"}}));
}

TEST_F(argument_t,
       variadicUint8ArgumentConstructOptionalStrWithoutValidateFunc) {
    clapp::argument::uint8_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::optional};
    ASSERT_THAT(get_validate_func(tp, arg_str), testing::Eq(std::nullopt));
}

TEST_F(argument_t, variadicUint8ArgumentConstructMandatoryStrWithValidateFunc) {
    clapp::argument::uint8_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::mandatory};
    ASSERT_THROW(get_validate_func(tp, arg_str).value()(),
                 clapp::exception::argument_exception_t);
}

TEST_F(argument_t,
       variadicUint8ArgumentConstructStrAndCallValueReturnsEmptyVector) {
    clapp::argument::variadic_uint8_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THAT(tp, ContainsArgument(arg_str));
    ASSERT_THAT(arg, ArgumentNotGiven());
    ASSERT_THAT(arg.value(), testing::Eq(std::vector<std::uint8_t>{}));
}

TEST_F(argument_t,
       variadicUint8ArgumentConstructWithAlreadyRegisteredArgThrows) {
    clapp::argument::variadic_uint8_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THROW((clapp::argument::uint8_argument_t{tp, arg_str, desc_str}),
                 clapp::exception::argument_exception_t);
}

TEST_F(argument_t, variadicUint8ArgumentConstructWithEmpyArgumentNameThrows) {
    ASSERT_THROW(
        (clapp::argument::variadic_uint8_argument_t{tp, "", desc_cstr}),
        clapp::exception::argument_exception_t);
}

TEST_F(argument_t,
       variadicUint8ArgumentConstructStrOptionalAndCallArgFuncMultipleTimes) {
    clapp::argument::variadic_uint8_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::optional};
    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)(std::to_string(value_uint8))));
    ASSERT_THAT(arg,
                VariadicArgumentGiven(std::vector<std::uint8_t>{value_uint8}));
    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)(std::to_string(value_int8_additional))));
    ASSERT_THAT(arg, VariadicArgumentGiven(std::vector<std::uint8_t>{
                         value_uint8, value_int8_additional}));
}

TEST_F(argument_t, variadicUint8ArgumentConstructStrWithoutCallArgFunc) {
    clapp::argument::variadic_uint8_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THAT(arg, VariadicArgumentNotGiven());
}

TEST_F(argument_t,
       variadicUint8ArgumentConstructStrAndCallArgFuncMultipleTimes) {
    clapp::argument::variadic_uint8_argument_t arg{tp, arg_cstr, desc_str};
    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_cstr)(std::to_string(value_uint8_additional))));
    ASSERT_THAT(arg, VariadicArgumentGiven(
                         std::vector<std::uint8_t>{value_uint8_additional}));
    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_cstr)(std::to_string(value_uint8))));
    ASSERT_THAT(arg, VariadicArgumentGiven(std::vector<std::uint8_t>{
                         value_uint8_additional, value_uint8}));
}

TEST_F(argument_t, variadicUint8ArgumentConstructCStrWithoutCallArgFunc) {
    clapp::argument::variadic_uint8_argument_t arg{tp, arg_cstr, desc_str};
    ASSERT_THAT(arg, VariadicArgumentNotGiven());
}

TEST_F(argument_t,
       variadicUint8ArgumentConstructMandatoryStrAndCallGetOptionHelp) {
    clapp::argument::variadic_uint8_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THAT(tp.get_argument_help(),
                testing::ContainerEq(argument_test_parser_t::help_entry_vec_t{
                    {arg_str, desc_str + " (" + purpose_mandatory_str + ", " +
                                  variadic_argument_desc_restriction + ")"}}));
}

TEST_F(argument_t, variadicUint8ArgumentCallFoundFunc) {
    clapp::argument::variadic_uint8_argument_t arg{
        tp, arg_str, desc_str,
        clapp::value::found_func_t{[this]() { found_func_called++; }}};
    ASSERT_THAT(tp, ContainsArgument(arg_str));

    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)("1")));
    ASSERT_THAT(found_func_called, testing::Eq(1));

    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)("11")));
    ASSERT_THAT(found_func_called, testing::Eq(2));
}

TEST_F(argument_t, ptrdiffArgumentConstructCStrAndCallValueThrows) {
    clapp::argument::ptrdiff_argument_t arg{tp, arg_cstr, desc_str};
    ASSERT_THAT(tp, ContainsArgument(arg_cstr));
    ASSERT_THAT(arg, ArgumentNotGiven());
    ASSERT_THROW(arg.value(), clapp::exception::value_undefined_t);
}

TEST_F(argument_t, ptrdiffArgumentConstructStrAndCallValueThrows) {
    clapp::argument::ptrdiff_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THAT(tp, ContainsArgument(arg_str));
    ASSERT_THAT(arg, ArgumentNotGiven());
    ASSERT_THROW(arg.value(), clapp::exception::value_undefined_t);
}

TEST_F(argument_t, ptrdiffArgumentConstructWithAlreadyRegisteredArgThrows) {
    clapp::argument::ptrdiff_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THROW((clapp::argument::ptrdiff_argument_t{tp, arg_str, desc_str}),
                 clapp::exception::argument_exception_t);
}

TEST_F(argument_t, ptrdiffArgumentConstructOptionalStrWithoutValidateFunc) {
    clapp::argument::ptrdiff_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::optional};
    ASSERT_THAT(get_validate_func(tp, arg_str), testing::Eq(std::nullopt));
}

TEST_F(argument_t, ptrdiffArgumentConstructMandatoryStrWithValidateFunc) {
    clapp::argument::ptrdiff_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::mandatory};
    ASSERT_THROW(get_validate_func(tp, arg_str).value()(),
                 clapp::exception::argument_exception_t);
}

TEST_F(argument_t, ptrdiffArgumentCallFoundFunc) {
    clapp::argument::ptrdiff_argument_t arg{
        tp, arg_str, desc_str,
        clapp::value::found_func_t{[this]() { found_func_called++; }}};
    ASSERT_THAT(tp, ContainsArgument(arg_str));
    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)("1")));
    ASSERT_THAT(found_func_called, testing::Eq(1));
}

TEST_F(argument_t, ptrdiffArgumentConstructWithEmpyArgumentNameThrows) {
    ASSERT_THROW((clapp::argument::ptrdiff_argument_t{tp, "", desc_cstr}),
                 clapp::exception::argument_exception_t);
}

TEST_F(argument_t, ptrdiffArgumentConstructStrAndCallArgFunc) {
    clapp::argument::ptrdiff_argument_t arg{tp, arg_str, desc_str};
    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)(std::to_string(value_ptrdiff))));
    ASSERT_THAT(arg, ArgumentGiven(value_ptrdiff));
}

TEST_F(argument_t, ptrdiffArgumentConstructStrWithDefaultValueAndCallArgFunc) {
    clapp::argument::ptrdiff_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::optional,
        clapp::value::default_value_t<std::ptrdiff_t>{value_int32}};
    ASSERT_THAT(arg, ArgumentNotGivenDefaultValue(value_int32));
    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)(std::to_string(value_ptrdiff))));
    ASSERT_THAT(arg, ArgumentGiven(value_ptrdiff));
}

TEST_F(argument_t, ptrdiffArgumentConstructMandatoryStrAndCallGetOptionHelp) {
    clapp::argument::ptrdiff_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THAT(tp.get_argument_help(),
                testing::ContainerEq(argument_test_parser_t::help_entry_vec_t{
                    {arg_str, desc_str + " (" + purpose_mandatory_str + ")"}}));
}

TEST_F(argument_t, ptrdiffArgumentConstructOptionalStrAndCallGetOptionHelp) {
    clapp::argument::ptrdiff_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::optional};
    ASSERT_THAT(tp.get_argument_help(),
                testing::ContainerEq(argument_test_parser_t::help_entry_vec_t{
                    {arg_str, desc_str + " (" + purpose_optional_str + ")"}}));
}

TEST_F(argument_t,
       variadicPtrdiffArgumentConstructOptionalStrWithoutValidateFunc) {
    clapp::argument::ptrdiff_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::optional};
    ASSERT_THAT(get_validate_func(tp, arg_str), testing::Eq(std::nullopt));
}

TEST_F(argument_t,
       variadicPtrdiffArgumentConstructMandatoryStrWithValidateFunc) {
    clapp::argument::ptrdiff_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::mandatory};
    ASSERT_THROW(get_validate_func(tp, arg_str).value()(),
                 clapp::exception::argument_exception_t);
}

TEST_F(argument_t,
       variadicPtrdiffArgumentConstructStrAndCallValueReturnsEmptyVector) {
    clapp::argument::variadic_ptrdiff_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THAT(tp, ContainsArgument(arg_str));
    ASSERT_THAT(arg, ArgumentNotGiven());
    ASSERT_THAT(arg.value(), testing::Eq(std::vector<std::ptrdiff_t>{}));
}

TEST_F(argument_t,
       variadicPtrdiffArgumentConstructWithAlreadyRegisteredArgThrows) {
    clapp::argument::variadic_ptrdiff_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THROW((clapp::argument::ptrdiff_argument_t{tp, arg_str, desc_str}),
                 clapp::exception::argument_exception_t);
}

TEST_F(argument_t, variadicPtrdiffArgumentConstructWithEmpyArgumentNameThrows) {
    ASSERT_THROW(
        (clapp::argument::variadic_ptrdiff_argument_t{tp, "", desc_cstr}),
        clapp::exception::argument_exception_t);
}

TEST_F(argument_t,
       variadicPtrdiffArgumentConstructStrOptionalAndCallArgFuncMultipleTimes) {
    clapp::argument::variadic_ptrdiff_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::optional};
    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)(std::to_string(value_ptrdiff))));
    ASSERT_THAT(
        arg, VariadicArgumentGiven(std::vector<std::ptrdiff_t>{value_ptrdiff}));
    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)(std::to_string(value_int16))));
    ASSERT_THAT(arg, VariadicArgumentGiven(std::vector<std::ptrdiff_t>{
                         value_ptrdiff, value_int16}));
}

TEST_F(argument_t, variadicPtrdiffArgumentConstructStrWithoutCallArgFunc) {
    clapp::argument::variadic_ptrdiff_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THAT(arg, VariadicArgumentNotGiven());
}

TEST_F(argument_t,
       variadicPtrdiffArgumentConstructStrAndCallArgFuncMultipleTimes) {
    clapp::argument::variadic_ptrdiff_argument_t arg{tp, arg_cstr, desc_str};
    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_cstr)(std::to_string(value_int32))));
    ASSERT_THAT(
        arg, VariadicArgumentGiven(std::vector<std::ptrdiff_t>{value_int32}));
    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_cstr)(std::to_string(value_ptrdiff))));
    ASSERT_THAT(arg, VariadicArgumentGiven(std::vector<std::ptrdiff_t>{
                         value_int32, value_ptrdiff}));
}

TEST_F(argument_t, variadicPtrdiffArgumentConstructCStrWithoutCallArgFunc) {
    clapp::argument::variadic_ptrdiff_argument_t arg{tp, arg_cstr, desc_str};
    ASSERT_THAT(arg, VariadicArgumentNotGiven());
}

TEST_F(argument_t,
       variadicPtrdiffArgumentConstructMandatoryStrAndCallGetOptionHelp) {
    clapp::argument::variadic_ptrdiff_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THAT(tp.get_argument_help(),
                testing::ContainerEq(argument_test_parser_t::help_entry_vec_t{
                    {arg_str, desc_str + " (" + purpose_mandatory_str + ", " +
                                  variadic_argument_desc_restriction + ")"}}));
}

TEST_F(argument_t, variadicPtrdiffArgumentCallFoundFunc) {
    clapp::argument::variadic_ptrdiff_argument_t arg{
        tp, arg_str, desc_str,
        clapp::value::found_func_t{[this]() { found_func_called++; }}};
    ASSERT_THAT(tp, ContainsArgument(arg_str));

    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)("1")));
    ASSERT_THAT(found_func_called, testing::Eq(1));

    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)("11")));
    ASSERT_THAT(found_func_called, testing::Eq(2));
}

TEST_F(argument_t, sizeArgumentConstructCStrAndCallValueThrows) {
    clapp::argument::size_argument_t arg{tp, arg_cstr, desc_str};
    ASSERT_THAT(tp, ContainsArgument(arg_cstr));
    ASSERT_THAT(arg, ArgumentNotGiven());
    ASSERT_THROW(arg.value(), clapp::exception::value_undefined_t);
}

TEST_F(argument_t, sizeArgumentConstructStrAndCallValueThrows) {
    clapp::argument::size_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THAT(tp, ContainsArgument(arg_str));
    ASSERT_THAT(arg, ArgumentNotGiven());
    ASSERT_THROW(arg.value(), clapp::exception::value_undefined_t);
}

TEST_F(argument_t, sizeArgumentConstructWithAlreadyRegisteredArgThrows) {
    clapp::argument::size_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THROW((clapp::argument::size_argument_t{tp, arg_str, desc_str}),
                 clapp::exception::argument_exception_t);
}

TEST_F(argument_t, sizeArgumentConstructOptionalStrWithoutValidateFunc) {
    clapp::argument::size_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::optional};
    ASSERT_THAT(get_validate_func(tp, arg_str), testing::Eq(std::nullopt));
}

TEST_F(argument_t, sizeArgumentConstructMandatoryStrWithValidateFunc) {
    clapp::argument::size_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::mandatory};
    ASSERT_THROW(get_validate_func(tp, arg_str).value()(),
                 clapp::exception::argument_exception_t);
}

TEST_F(argument_t, sizeArgumentCallFoundFunc) {
    clapp::argument::size_argument_t arg{
        tp, arg_str, desc_str,
        clapp::value::found_func_t{[this]() { found_func_called++; }}};
    ASSERT_THAT(tp, ContainsArgument(arg_str));
    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)("1")));
    ASSERT_THAT(found_func_called, testing::Eq(1));
}

TEST_F(argument_t, sizeArgumentConstructWithEmpyArgumentNameThrows) {
    ASSERT_THROW((clapp::argument::size_argument_t{tp, "", desc_cstr}),
                 clapp::exception::argument_exception_t);
}

TEST_F(argument_t, sizeArgumentConstructStrAndCallArgFunc) {
    clapp::argument::size_argument_t arg{tp, arg_str, desc_str};
    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)(std::to_string(value_size))));
    ASSERT_THAT(arg, ArgumentGiven(value_size));
}

TEST_F(argument_t, sizeArgumentConstructStrWithDefaultValueAndCallArgFunc) {
    clapp::argument::size_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::optional,
        clapp::value::default_value_t<std::size_t>{value_uint32}};
    ASSERT_THAT(arg, ArgumentNotGivenDefaultValue(value_uint32));
    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)(std::to_string(value_size))));
    ASSERT_THAT(arg, ArgumentGiven(value_size));
}

TEST_F(argument_t, sizeArgumentConstructMandatoryStrAndCallGetOptionHelp) {
    clapp::argument::size_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THAT(tp.get_argument_help(),
                testing::ContainerEq(argument_test_parser_t::help_entry_vec_t{
                    {arg_str, desc_str + " (" + purpose_mandatory_str + ")"}}));
}

TEST_F(argument_t, sizeArgumentConstructOptionalStrAndCallGetOptionHelp) {
    clapp::argument::size_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::optional};
    ASSERT_THAT(tp.get_argument_help(),
                testing::ContainerEq(argument_test_parser_t::help_entry_vec_t{
                    {arg_str, desc_str + " (" + purpose_optional_str + ")"}}));
}

TEST_F(argument_t,
       variadicSizeArgumentConstructOptionalStrWithoutValidateFunc) {
    clapp::argument::size_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::optional};
    ASSERT_THAT(get_validate_func(tp, arg_str), testing::Eq(std::nullopt));
}

TEST_F(argument_t, variadicSizeArgumentConstructMandatoryStrWithValidateFunc) {
    clapp::argument::size_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::mandatory};
    ASSERT_THROW(get_validate_func(tp, arg_str).value()(),
                 clapp::exception::argument_exception_t);
}

TEST_F(argument_t,
       variadicSizeArgumentConstructStrAndCallValueReturnsEmptyVector) {
    clapp::argument::variadic_size_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THAT(tp, ContainsArgument(arg_str));
    ASSERT_THAT(arg, ArgumentNotGiven());
    ASSERT_THAT(arg.value(), testing::Eq(std::vector<std::size_t>{}));
}

TEST_F(argument_t,
       variadicSizeArgumentConstructWithAlreadyRegisteredArgThrows) {
    clapp::argument::variadic_size_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THROW((clapp::argument::size_argument_t{tp, arg_str, desc_str}),
                 clapp::exception::argument_exception_t);
}

TEST_F(argument_t, variadicSizeArgumentConstructWithEmpyArgumentNameThrows) {
    ASSERT_THROW((clapp::argument::variadic_size_argument_t{tp, "", desc_cstr}),
                 clapp::exception::argument_exception_t);
}

TEST_F(argument_t,
       variadicSizeArgumentConstructStrOptionalAndCallArgFuncMultipleTimes) {
    clapp::argument::variadic_size_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::optional};
    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)(std::to_string(value_size))));
    ASSERT_THAT(arg,
                VariadicArgumentGiven(std::vector<std::size_t>{value_size}));
    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)(std::to_string(value_int16))));
    ASSERT_THAT(arg, VariadicArgumentGiven(
                         std::vector<std::size_t>{value_size, value_int16}));
}

TEST_F(argument_t, variadicSizeArgumentConstructStrWithoutCallArgFunc) {
    clapp::argument::variadic_size_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THAT(arg, VariadicArgumentNotGiven());
}

TEST_F(argument_t,
       variadicSizeArgumentConstructStrAndCallArgFuncMultipleTimes) {
    clapp::argument::variadic_size_argument_t arg{tp, arg_cstr, desc_str};
    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_cstr)(std::to_string(value_uint32))));
    ASSERT_THAT(arg,
                VariadicArgumentGiven(std::vector<std::size_t>{value_uint32}));
    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_cstr)(std::to_string(value_size))));
    ASSERT_THAT(arg, VariadicArgumentGiven(
                         std::vector<std::size_t>{value_uint32, value_size}));
}

TEST_F(argument_t, variadicSizeArgumentConstructCStrWithoutCallArgFunc) {
    clapp::argument::variadic_size_argument_t arg{tp, arg_cstr, desc_str};
    ASSERT_THAT(arg, VariadicArgumentNotGiven());
}

TEST_F(argument_t,
       variadicSizeArgumentConstructMandatoryStrAndCallGetOptionHelp) {
    clapp::argument::variadic_size_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THAT(tp.get_argument_help(),
                testing::ContainerEq(argument_test_parser_t::help_entry_vec_t{
                    {arg_str, desc_str + " (" + purpose_mandatory_str + ", " +
                                  variadic_argument_desc_restriction + ")"}}));
}

TEST_F(argument_t, variadicSizeArgumentCallFoundFunc) {
    clapp::argument::variadic_size_argument_t arg{
        tp, arg_str, desc_str,
        clapp::value::found_func_t{[this]() { found_func_called++; }}};
    ASSERT_THAT(tp, ContainsArgument(arg_str));

    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)("1")));
    ASSERT_THAT(found_func_called, testing::Eq(1));

    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)("11")));
    ASSERT_THAT(found_func_called, testing::Eq(2));
}

TEST_F(argument_t, floatArgumentConstructCStrAndCallValueThrows) {
    clapp::argument::float_argument_t arg{tp, arg_cstr, desc_str};
    ASSERT_THAT(tp, ContainsArgument(arg_cstr));
    ASSERT_THAT(arg, ArgumentNotGiven());
    ASSERT_THROW(arg.value(), clapp::exception::value_undefined_t);
}

TEST_F(argument_t, floatArgumentConstructStrAndCallValueThrows) {
    clapp::argument::float_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THAT(tp, ContainsArgument(arg_str));
    ASSERT_THAT(arg, ArgumentNotGiven());
    ASSERT_THROW(arg.value(), clapp::exception::value_undefined_t);
}

TEST_F(argument_t, floatArgumentConstructWithAlreadyRegisteredArgThrows) {
    clapp::argument::float_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THROW((clapp::argument::float_argument_t{tp, arg_str, desc_str}),
                 clapp::exception::argument_exception_t);
}

TEST_F(argument_t, floatArgumentConstructOptionalStrWithoutValidateFunc) {
    clapp::argument::float_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::optional};
    ASSERT_THAT(get_validate_func(tp, arg_str), testing::Eq(std::nullopt));
}

TEST_F(argument_t, floatArgumentConstructMandatoryStrWithValidateFunc) {
    clapp::argument::float_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::mandatory};
    ASSERT_THROW(get_validate_func(tp, arg_str).value()(),
                 clapp::exception::argument_exception_t);
}

TEST_F(argument_t, floatArgumentCallFoundFunc) {
    clapp::argument::float_argument_t arg{
        tp, arg_str, desc_str,
        clapp::value::found_func_t{[this]() { found_func_called++; }}};
    ASSERT_THAT(tp, ContainsArgument(arg_str));
    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)("1")));
    ASSERT_THAT(found_func_called, testing::Eq(1));
}

TEST_F(argument_t, floatArgumentConstructWithEmpyArgumentNameThrows) {
    ASSERT_THROW((clapp::argument::float_argument_t{tp, "", desc_cstr}),
                 clapp::exception::argument_exception_t);
}

TEST_F(argument_t, floatArgumentConstructStrAndCallArgFunc) {
    clapp::argument::float_argument_t arg{tp, arg_str, desc_str};
    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)(std::to_string(value_float))));
    ASSERT_THAT(arg, ArgumentGiven(value_float));
}

TEST_F(argument_t, floatArgumentConstructStrWithDefaultValueAndCallArgFunc) {
    clapp::argument::float_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::optional,
        clapp::value::default_value_t<std::float_t>{value_float_additional}};
    ASSERT_THAT(arg, ArgumentNotGivenDefaultValue(value_float_additional));
    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)(std::to_string(value_float))));
    ASSERT_THAT(arg, ArgumentGiven(value_float));
}

TEST_F(argument_t, floatArgumentConstructMandatoryStrAndCallGetOptionHelp) {
    clapp::argument::float_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THAT(tp.get_argument_help(),
                testing::ContainerEq(argument_test_parser_t::help_entry_vec_t{
                    {arg_str, desc_str + " (" + purpose_mandatory_str + ")"}}));
}

TEST_F(argument_t, floatArgumentConstructOptionalStrAndCallGetOptionHelp) {
    clapp::argument::float_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::optional};
    ASSERT_THAT(tp.get_argument_help(),
                testing::ContainerEq(argument_test_parser_t::help_entry_vec_t{
                    {arg_str, desc_str + " (" + purpose_optional_str + ")"}}));
}

TEST_F(argument_t,
       variadicFloatArgumentConstructOptionalStrWithoutValidateFunc) {
    clapp::argument::float_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::optional};
    ASSERT_THAT(get_validate_func(tp, arg_str), testing::Eq(std::nullopt));
}

TEST_F(argument_t, variadicFloatArgumentConstructMandatoryStrWithValidateFunc) {
    clapp::argument::float_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::mandatory};
    ASSERT_THROW(get_validate_func(tp, arg_str).value()(),
                 clapp::exception::argument_exception_t);
}

TEST_F(argument_t,
       variadicFloatArgumentConstructStrAndCallValueReturnsEmptyVector) {
    clapp::argument::variadic_float_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THAT(tp, ContainsArgument(arg_str));
    ASSERT_THAT(arg, ArgumentNotGiven());
    ASSERT_THAT(arg.value(), testing::Eq(std::vector<std::float_t>{}));
}

TEST_F(argument_t,
       variadicFloatArgumentConstructWithAlreadyRegisteredArgThrows) {
    clapp::argument::variadic_float_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THROW((clapp::argument::float_argument_t{tp, arg_str, desc_str}),
                 clapp::exception::argument_exception_t);
}

TEST_F(argument_t, variadicFloatArgumentConstructWithEmpyArgumentNameThrows) {
    ASSERT_THROW(
        (clapp::argument::variadic_float_argument_t{tp, "", desc_cstr}),
        clapp::exception::argument_exception_t);
}

TEST_F(argument_t,
       variadicFloatArgumentConstructStrOptionalAndCallArgFuncMultipleTimes) {
    clapp::argument::variadic_float_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::optional};
    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)(std::to_string(value_float))));
    ASSERT_THAT(arg,
                VariadicArgumentGiven(std::vector<std::float_t>{value_float}));
    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)(std::to_string(value_float_additional))));
    ASSERT_THAT(arg, VariadicArgumentGiven(std::vector<std::float_t>{
                         value_float, value_float_additional}));
}

TEST_F(argument_t, variadicFloatArgumentConstructStrWithoutCallArgFunc) {
    clapp::argument::variadic_float_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THAT(arg, VariadicArgumentNotGiven());
}

TEST_F(argument_t,
       variadicFloatArgumentConstructStrAndCallArgFuncMultipleTimes) {
    clapp::argument::variadic_float_argument_t arg{tp, arg_cstr, desc_str};
    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_cstr)(std::to_string(value_float_additional))));
    ASSERT_THAT(arg, VariadicArgumentGiven(
                         std::vector<std::float_t>{value_float_additional}));
    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_cstr)(std::to_string(value_float))));
    ASSERT_THAT(arg, VariadicArgumentGiven(std::vector<std::float_t>{
                         value_float_additional, value_float}));
}

TEST_F(argument_t, variadicFloatArgumentConstructCStrWithoutCallArgFunc) {
    clapp::argument::variadic_float_argument_t arg{tp, arg_cstr, desc_str};
    ASSERT_THAT(arg, VariadicArgumentNotGiven());
}

TEST_F(argument_t,
       variadicFloatArgumentConstructMandatoryStrAndCallGetOptionHelp) {
    clapp::argument::variadic_float_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THAT(tp.get_argument_help(),
                testing::ContainerEq(argument_test_parser_t::help_entry_vec_t{
                    {arg_str, desc_str + " (" + purpose_mandatory_str + ", " +
                                  variadic_argument_desc_restriction + ")"}}));
}

TEST_F(argument_t, variadicFloatArgumentCallFoundFunc) {
    clapp::argument::variadic_float_argument_t arg{
        tp, arg_str, desc_str,
        clapp::value::found_func_t{[this]() { found_func_called++; }}};
    ASSERT_THAT(tp, ContainsArgument(arg_str));

    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)("1")));
    ASSERT_THAT(found_func_called, testing::Eq(1));

    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)("11")));
    ASSERT_THAT(found_func_called, testing::Eq(2));
}

TEST_F(argument_t, doubleArgumentConstructCStrAndCallValueThrows) {
    clapp::argument::double_argument_t arg{tp, arg_cstr, desc_str};
    ASSERT_THAT(tp, ContainsArgument(arg_cstr));
    ASSERT_THAT(arg, ArgumentNotGiven());
    ASSERT_THROW(arg.value(), clapp::exception::value_undefined_t);
}

TEST_F(argument_t, doubleArgumentConstructStrAndCallValueThrows) {
    clapp::argument::double_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THAT(tp, ContainsArgument(arg_str));
    ASSERT_THAT(arg, ArgumentNotGiven());
    ASSERT_THROW(arg.value(), clapp::exception::value_undefined_t);
}

TEST_F(argument_t, doubleArgumentConstructWithAlreadyRegisteredArgThrows) {
    clapp::argument::double_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THROW((clapp::argument::double_argument_t{tp, arg_str, desc_str}),
                 clapp::exception::argument_exception_t);
}

TEST_F(argument_t, doubleArgumentConstructOptionalStrWithoutValidateFunc) {
    clapp::argument::double_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::optional};
    ASSERT_THAT(get_validate_func(tp, arg_str), testing::Eq(std::nullopt));
}

TEST_F(argument_t, doubleArgumentConstructMandatoryStrWithValidateFunc) {
    clapp::argument::double_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::mandatory};
    ASSERT_THROW(get_validate_func(tp, arg_str).value()(),
                 clapp::exception::argument_exception_t);
}

TEST_F(argument_t, doubleArgumentCallFoundFunc) {
    clapp::argument::double_argument_t arg{
        tp, arg_str, desc_str,
        clapp::value::found_func_t{[this]() { found_func_called++; }}};
    ASSERT_THAT(tp, ContainsArgument(arg_str));
    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)("1")));
    ASSERT_THAT(found_func_called, testing::Eq(1));
}

TEST_F(argument_t, doubleArgumentConstructWithEmpyArgumentNameThrows) {
    ASSERT_THROW((clapp::argument::double_argument_t{tp, "", desc_cstr}),
                 clapp::exception::argument_exception_t);
}

TEST_F(argument_t, doubleArgumentConstructStrAndCallArgFunc) {
    clapp::argument::double_argument_t arg{tp, arg_str, desc_str};
    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)(std::to_string(value_double))));
    ASSERT_THAT(arg, ArgumentGiven(value_double));
}

TEST_F(argument_t, doubleArgumentConstructStrWithDefaultValueAndCallArgFunc) {
    clapp::argument::double_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::optional,
        clapp::value::default_value_t<std::double_t>{
            static_cast<std::double_t>(value_float)}};
    ASSERT_THAT(arg, ArgumentNotGivenDefaultValue(
                         static_cast<std::double_t>(value_float)));
    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)(std::to_string(value_double))));
    ASSERT_THAT(arg, ArgumentGiven(value_double));
}

TEST_F(argument_t, doubleArgumentConstructMandatoryStrAndCallGetOptionHelp) {
    clapp::argument::double_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THAT(tp.get_argument_help(),
                testing::ContainerEq(argument_test_parser_t::help_entry_vec_t{
                    {arg_str, desc_str + " (" + purpose_mandatory_str + ")"}}));
}

TEST_F(argument_t, doubleArgumentConstructOptionalStrAndCallGetOptionHelp) {
    clapp::argument::double_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::optional};
    ASSERT_THAT(tp.get_argument_help(),
                testing::ContainerEq(argument_test_parser_t::help_entry_vec_t{
                    {arg_str, desc_str + " (" + purpose_optional_str + ")"}}));
}

TEST_F(argument_t,
       variadicDoubleArgumentConstructOptionalStrWithoutValidateFunc) {
    clapp::argument::double_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::optional};
    ASSERT_THAT(get_validate_func(tp, arg_str), testing::Eq(std::nullopt));
}

TEST_F(argument_t,
       variadicDoubleArgumentConstructMandatoryStrWithValidateFunc) {
    clapp::argument::double_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::mandatory};
    ASSERT_THROW(get_validate_func(tp, arg_str).value()(),
                 clapp::exception::argument_exception_t);
}

TEST_F(argument_t,
       variadicDoubleArgumentConstructStrAndCallValueReturnsEmptyVector) {
    clapp::argument::variadic_double_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THAT(tp, ContainsArgument(arg_str));
    ASSERT_THAT(arg, ArgumentNotGiven());
    ASSERT_THAT(arg.value(), testing::Eq(std::vector<std::double_t>{}));
}

TEST_F(argument_t,
       variadicDoubleArgumentConstructWithAlreadyRegisteredArgThrows) {
    clapp::argument::variadic_double_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THROW((clapp::argument::double_argument_t{tp, arg_str, desc_str}),
                 clapp::exception::argument_exception_t);
}

TEST_F(argument_t, variadicDoubleArgumentConstructWithEmpyArgumentNameThrows) {
    ASSERT_THROW(
        (clapp::argument::variadic_double_argument_t{tp, "", desc_cstr}),
        clapp::exception::argument_exception_t);
}

TEST_F(argument_t,
       variadicDoubleArgumentConstructStrOptionalAndCallArgFuncMultipleTimes) {
    clapp::argument::variadic_double_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::optional};
    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)(std::to_string(value_double))));
    ASSERT_THAT(
        arg, VariadicArgumentGiven(std::vector<std::double_t>{value_double}));
    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)(std::to_string(value_int16))));
    ASSERT_THAT(arg, VariadicArgumentGiven(std::vector<std::double_t>{
                         value_double, value_int16}));
}

TEST_F(argument_t, variadicDoubleArgumentConstructStrWithoutCallArgFunc) {
    clapp::argument::variadic_double_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THAT(arg, VariadicArgumentNotGiven());
}

TEST_F(argument_t,
       variadicDoubleArgumentConstructStrAndCallArgFuncMultipleTimes) {
    clapp::argument::variadic_double_argument_t arg{tp, arg_cstr, desc_str};
    ASSERT_NO_THROW(
        (get_arg_func<argument_test_parser_t::argument_func_t>(tp, arg_cstr)(
            std::to_string(static_cast<std::double_t>(value_float)))));
    ASSERT_THAT(arg, VariadicArgumentGiven(std::vector<std::double_t>{
                         static_cast<std::double_t>(value_float)}));
    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_cstr)(std::to_string(value_double))));
    ASSERT_THAT(arg,
                VariadicArgumentGiven(std::vector<std::double_t>{
                    static_cast<std::double_t>(value_float), value_double}));
}

TEST_F(argument_t, variadicDoubleArgumentConstructCStrWithoutCallArgFunc) {
    clapp::argument::variadic_double_argument_t arg{tp, arg_cstr, desc_str};
    ASSERT_THAT(arg, VariadicArgumentNotGiven());
}

TEST_F(argument_t,
       variadicDoubleArgumentConstructMandatoryStrAndCallGetOptionHelp) {
    clapp::argument::variadic_double_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THAT(tp.get_argument_help(),
                testing::ContainerEq(argument_test_parser_t::help_entry_vec_t{
                    {arg_str, desc_str + " (" + purpose_mandatory_str + ", " +
                                  variadic_argument_desc_restriction + ")"}}));
}

TEST_F(argument_t, variadicDoubleArgumentCallFoundFunc) {
    clapp::argument::variadic_double_argument_t arg{
        tp, arg_str, desc_str,
        clapp::value::found_func_t{[this]() { found_func_called++; }}};
    ASSERT_THAT(tp, ContainsArgument(arg_str));

    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)("1")));
    ASSERT_THAT(found_func_called, testing::Eq(1));

    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)("11")));
    ASSERT_THAT(found_func_called, testing::Eq(2));
}

TEST_F(argument_t, nsArgumentConstructCStrAndCallValueThrows) {
    clapp::argument::ns_argument_t arg{tp, arg_cstr, desc_str};
    ASSERT_THAT(tp, ContainsArgument(arg_cstr));
    ASSERT_THAT(arg, ArgumentNotGiven());
    ASSERT_THROW(arg.value(), clapp::exception::value_undefined_t);
}

TEST_F(argument_t, nsArgumentConstructStrAndCallValueThrows) {
    clapp::argument::ns_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THAT(tp, ContainsArgument(arg_str));
    ASSERT_THAT(arg, ArgumentNotGiven());
    ASSERT_THROW(arg.value(), clapp::exception::value_undefined_t);
}

TEST_F(argument_t, nsArgumentConstructWithAlreadyRegisteredArgThrows) {
    clapp::argument::ns_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THROW((clapp::argument::ns_argument_t{tp, arg_str, desc_str}),
                 clapp::exception::argument_exception_t);
}

TEST_F(argument_t, nsArgumentConstructOptionalStrWithoutValidateFunc) {
    clapp::argument::ns_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::optional};
    ASSERT_THAT(get_validate_func(tp, arg_str), testing::Eq(std::nullopt));
}

TEST_F(argument_t, nsArgumentConstructMandatoryStrWithValidateFunc) {
    clapp::argument::ns_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::mandatory};
    ASSERT_THROW(get_validate_func(tp, arg_str).value()(),
                 clapp::exception::argument_exception_t);
}

TEST_F(argument_t, nsArgumentCallFoundFunc) {
    clapp::argument::ns_argument_t arg{
        tp, arg_str, desc_str,
        clapp::value::found_func_t{[this]() { found_func_called++; }}};
    ASSERT_THAT(tp, ContainsArgument(arg_str));
    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)("1")));
    ASSERT_THAT(found_func_called, testing::Eq(1));
}

TEST_F(argument_t, nsArgumentConstructWithEmpyArgumentNameThrows) {
    ASSERT_THROW((clapp::argument::ns_argument_t{tp, "", desc_cstr}),
                 clapp::exception::argument_exception_t);
}

TEST_F(argument_t, nsArgumentConstructStrAndCallArgFunc) {
    clapp::argument::ns_argument_t arg{tp, arg_str, desc_str};
    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)(std::to_string(value_ns.count()))));
    ASSERT_THAT(arg, ArgumentGiven(value_ns));
}

TEST_F(argument_t, nsArgumentConstructStrWithDefaultValueAndCallArgFunc) {
    clapp::argument::ns_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::optional,
        clapp::value::default_value_t<std::chrono::nanoseconds>{value_us}};
    ASSERT_THAT(arg, ArgumentNotGivenDefaultValue(value_us));
    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)(std::to_string(value_ns.count()))));
    ASSERT_THAT(arg, ArgumentGiven(value_ns));
}

TEST_F(argument_t, nsArgumentConstructMandatoryStrAndCallGetOptionHelp) {
    clapp::argument::ns_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THAT(tp.get_argument_help(),
                testing::ContainerEq(argument_test_parser_t::help_entry_vec_t{
                    {arg_str, desc_str + " (" + purpose_mandatory_str + ")"}}));
}

TEST_F(argument_t, nsArgumentConstructOptionalStrAndCallGetOptionHelp) {
    clapp::argument::ns_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::optional};
    ASSERT_THAT(tp.get_argument_help(),
                testing::ContainerEq(argument_test_parser_t::help_entry_vec_t{
                    {arg_str, desc_str + " (" + purpose_optional_str + ")"}}));
}

TEST_F(argument_t, variadicNsArgumentConstructOptionalStrWithoutValidateFunc) {
    clapp::argument::ns_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::optional};
    ASSERT_THAT(get_validate_func(tp, arg_str), testing::Eq(std::nullopt));
}

TEST_F(argument_t, variadicNsArgumentConstructMandatoryStrWithValidateFunc) {
    clapp::argument::ns_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::mandatory};
    ASSERT_THROW(get_validate_func(tp, arg_str).value()(),
                 clapp::exception::argument_exception_t);
}

TEST_F(argument_t,
       variadicNsArgumentConstructStrAndCallValueReturnsEmptyVector) {
    clapp::argument::variadic_ns_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THAT(tp, ContainsArgument(arg_str));
    ASSERT_THAT(arg, ArgumentNotGiven());
    ASSERT_THAT(arg.value(),
                testing::Eq(std::vector<std::chrono::nanoseconds>{}));
}

TEST_F(argument_t, variadicNsArgumentConstructWithAlreadyRegisteredArgThrows) {
    clapp::argument::variadic_ns_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THROW((clapp::argument::ns_argument_t{tp, arg_str, desc_str}),
                 clapp::exception::argument_exception_t);
}

TEST_F(argument_t, variadicNsArgumentConstructWithEmpyArgumentNameThrows) {
    ASSERT_THROW((clapp::argument::variadic_ns_argument_t{tp, "", desc_cstr}),
                 clapp::exception::argument_exception_t);
}

TEST_F(argument_t,
       variadicNsArgumentConstructStrOptionalAndCallArgFuncMultipleTimes) {
    clapp::argument::variadic_ns_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::optional};
    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)(std::to_string(value_ns.count()))));
    ASSERT_THAT(arg, VariadicArgumentGiven(
                         std::vector<std::chrono::nanoseconds>{value_ns}));
    ASSERT_NO_THROW(
        (get_arg_func<argument_test_parser_t::argument_func_t>(tp, arg_str)(
            std::to_string(std::chrono::nanoseconds{value_us}.count()))));
    ASSERT_THAT(arg,
                VariadicArgumentGiven(
                    std::vector<std::chrono::nanoseconds>{value_ns, value_us}));
}

TEST_F(argument_t, variadicNsArgumentConstructStrWithoutCallArgFunc) {
    clapp::argument::variadic_ns_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THAT(arg, VariadicArgumentNotGiven());
}

TEST_F(argument_t, variadicNsArgumentConstructStrAndCallArgFuncMultipleTimes) {
    clapp::argument::variadic_ns_argument_t arg{tp, arg_cstr, desc_str};
    ASSERT_NO_THROW(
        (get_arg_func<argument_test_parser_t::argument_func_t>(tp, arg_cstr)(
            std::to_string(std::chrono::nanoseconds{value_us}.count()))));
    ASSERT_THAT(arg, VariadicArgumentGiven(
                         std::vector<std::chrono::nanoseconds>{value_us}));
    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_cstr)(std::to_string(value_ns.count()))));
    ASSERT_THAT(arg,
                VariadicArgumentGiven(
                    std::vector<std::chrono::nanoseconds>{value_us, value_ns}));
}

TEST_F(argument_t, variadicNsArgumentConstructCStrWithoutCallArgFunc) {
    clapp::argument::variadic_ns_argument_t arg{tp, arg_cstr, desc_str};
    ASSERT_THAT(arg, VariadicArgumentNotGiven());
}

TEST_F(argument_t,
       variadicNsArgumentConstructMandatoryStrAndCallGetOptionHelp) {
    clapp::argument::variadic_ns_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THAT(tp.get_argument_help(),
                testing::ContainerEq(argument_test_parser_t::help_entry_vec_t{
                    {arg_str, desc_str + " (" + purpose_mandatory_str + ", " +
                                  variadic_argument_desc_restriction + ")"}}));
}

TEST_F(argument_t, variadicNsArgumentCallFoundFunc) {
    clapp::argument::variadic_ns_argument_t arg{
        tp, arg_str, desc_str,
        clapp::value::found_func_t{[this]() { found_func_called++; }}};
    ASSERT_THAT(tp, ContainsArgument(arg_str));

    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)("1")));
    ASSERT_THAT(found_func_called, testing::Eq(1));

    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)("11")));
    ASSERT_THAT(found_func_called, testing::Eq(2));
}

TEST_F(argument_t, usArgumentConstructCStrAndCallValueThrows) {
    clapp::argument::us_argument_t arg{tp, arg_cstr, desc_str};
    ASSERT_THAT(tp, ContainsArgument(arg_cstr));
    ASSERT_THAT(arg, ArgumentNotGiven());
    ASSERT_THROW(arg.value(), clapp::exception::value_undefined_t);
}

TEST_F(argument_t, usArgumentConstructStrAndCallValueThrows) {
    clapp::argument::us_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THAT(tp, ContainsArgument(arg_str));
    ASSERT_THAT(arg, ArgumentNotGiven());
    ASSERT_THROW(arg.value(), clapp::exception::value_undefined_t);
}

TEST_F(argument_t, usArgumentConstructWithAlreadyRegisteredArgThrows) {
    clapp::argument::us_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THROW((clapp::argument::us_argument_t{tp, arg_str, desc_str}),
                 clapp::exception::argument_exception_t);
}

TEST_F(argument_t, usArgumentConstructOptionalStrWithoutValidateFunc) {
    clapp::argument::us_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::optional};
    ASSERT_THAT(get_validate_func(tp, arg_str), testing::Eq(std::nullopt));
}

TEST_F(argument_t, usArgumentConstructMandatoryStrWithValidateFunc) {
    clapp::argument::us_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::mandatory};
    ASSERT_THROW(get_validate_func(tp, arg_str).value()(),
                 clapp::exception::argument_exception_t);
}

TEST_F(argument_t, usArgumentCallFoundFunc) {
    clapp::argument::us_argument_t arg{
        tp, arg_str, desc_str,
        clapp::value::found_func_t{[this]() { found_func_called++; }}};
    ASSERT_THAT(tp, ContainsArgument(arg_str));
    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)("1")));
    ASSERT_THAT(found_func_called, testing::Eq(1));
}

TEST_F(argument_t, usArgumentConstructWithEmpyArgumentNameThrows) {
    ASSERT_THROW((clapp::argument::us_argument_t{tp, "", desc_cstr}),
                 clapp::exception::argument_exception_t);
}

TEST_F(argument_t, usArgumentConstructStrAndCallArgFunc) {
    clapp::argument::us_argument_t arg{tp, arg_str, desc_str};
    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)(std::to_string(value_us.count()))));
    ASSERT_THAT(arg, ArgumentGiven(value_us));
}

TEST_F(argument_t, usArgumentConstructStrWithDefaultValueAndCallArgFunc) {
    clapp::argument::us_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::optional,
        clapp::value::default_value_t<std::chrono::microseconds>{value_ms}};
    ASSERT_THAT(arg, ArgumentNotGivenDefaultValue(value_ms));
    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)(std::to_string(value_us.count()))));
    ASSERT_THAT(arg, ArgumentGiven(value_us));
}

TEST_F(argument_t, usArgumentConstructMandatoryStrAndCallGetOptionHelp) {
    clapp::argument::us_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THAT(tp.get_argument_help(),
                testing::ContainerEq(argument_test_parser_t::help_entry_vec_t{
                    {arg_str, desc_str + " (" + purpose_mandatory_str + ")"}}));
}

TEST_F(argument_t, usArgumentConstructOptionalStrAndCallGetOptionHelp) {
    clapp::argument::us_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::optional};
    ASSERT_THAT(tp.get_argument_help(),
                testing::ContainerEq(argument_test_parser_t::help_entry_vec_t{
                    {arg_str, desc_str + " (" + purpose_optional_str + ")"}}));
}

TEST_F(argument_t, variadicUsArgumentConstructOptionalStrWithoutValidateFunc) {
    clapp::argument::us_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::optional};
    ASSERT_THAT(get_validate_func(tp, arg_str), testing::Eq(std::nullopt));
}

TEST_F(argument_t, variadicUsArgumentConstructMandatoryStrWithValidateFunc) {
    clapp::argument::us_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::mandatory};
    ASSERT_THROW(get_validate_func(tp, arg_str).value()(),
                 clapp::exception::argument_exception_t);
}

TEST_F(argument_t,
       variadicUsArgumentConstructStrAndCallValueReturnsEmptyVector) {
    clapp::argument::variadic_us_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THAT(tp, ContainsArgument(arg_str));
    ASSERT_THAT(arg, ArgumentNotGiven());
    ASSERT_THAT(arg.value(),
                testing::Eq(std::vector<std::chrono::microseconds>{}));
}

TEST_F(argument_t, variadicUsArgumentConstructWithAlreadyRegisteredArgThrows) {
    clapp::argument::variadic_us_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THROW((clapp::argument::us_argument_t{tp, arg_str, desc_str}),
                 clapp::exception::argument_exception_t);
}

TEST_F(argument_t, variadicUsArgumentConstructWithEmpyArgumentNameThrows) {
    ASSERT_THROW((clapp::argument::variadic_us_argument_t{tp, "", desc_cstr}),
                 clapp::exception::argument_exception_t);
}

TEST_F(argument_t,
       variadicUsArgumentConstructStrOptionalAndCallArgFuncMultipleTimes) {
    clapp::argument::variadic_us_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::optional};
    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)(std::to_string(value_us.count()))));
    ASSERT_THAT(arg, VariadicArgumentGiven(
                         std::vector<std::chrono::microseconds>{value_us}));
    ASSERT_NO_THROW(
        (get_arg_func<argument_test_parser_t::argument_func_t>(tp, arg_str)(
            std::to_string(std::chrono::microseconds{value_us}.count()))));
    ASSERT_THAT(
        arg, VariadicArgumentGiven(
                 std::vector<std::chrono::microseconds>{value_us, value_us}));
}

TEST_F(argument_t, variadicUsArgumentConstructStrWithoutCallArgFunc) {
    clapp::argument::variadic_us_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THAT(arg, VariadicArgumentNotGiven());
}

TEST_F(argument_t, variadicUsArgumentConstructStrAndCallArgFuncMultipleTimes) {
    clapp::argument::variadic_us_argument_t arg{tp, arg_cstr, desc_str};
    ASSERT_NO_THROW(
        (get_arg_func<argument_test_parser_t::argument_func_t>(tp, arg_cstr)(
            std::to_string(std::chrono::microseconds{value_us}.count()))));
    ASSERT_THAT(arg, VariadicArgumentGiven(
                         std::vector<std::chrono::microseconds>{value_us}));
    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_cstr)(std::to_string(value_us.count()))));
    ASSERT_THAT(
        arg, VariadicArgumentGiven(
                 std::vector<std::chrono::microseconds>{value_us, value_us}));
}

TEST_F(argument_t, variadicUsArgumentConstructCStrWithoutCallArgFunc) {
    clapp::argument::variadic_us_argument_t arg{tp, arg_cstr, desc_str};
    ASSERT_THAT(arg, VariadicArgumentNotGiven());
}

TEST_F(argument_t,
       variadicUsArgumentConstructMandatoryStrAndCallGetOptionHelp) {
    clapp::argument::variadic_us_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THAT(tp.get_argument_help(),
                testing::ContainerEq(argument_test_parser_t::help_entry_vec_t{
                    {arg_str, desc_str + " (" + purpose_mandatory_str + ", " +
                                  variadic_argument_desc_restriction + ")"}}));
}

TEST_F(argument_t, variadicUsArgumentCallFoundFunc) {
    clapp::argument::variadic_us_argument_t arg{
        tp, arg_str, desc_str,
        clapp::value::found_func_t{[this]() { found_func_called++; }}};
    ASSERT_THAT(tp, ContainsArgument(arg_str));

    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)("1")));
    ASSERT_THAT(found_func_called, testing::Eq(1));

    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)("11")));
    ASSERT_THAT(found_func_called, testing::Eq(2));
}

TEST_F(argument_t, msArgumentConstructCStrAndCallValueThrows) {
    clapp::argument::ms_argument_t arg{tp, arg_cstr, desc_str};
    ASSERT_THAT(tp, ContainsArgument(arg_cstr));
    ASSERT_THAT(arg, ArgumentNotGiven());
    ASSERT_THROW(arg.value(), clapp::exception::value_undefined_t);
}

TEST_F(argument_t, msArgumentConstructStrAndCallValueThrows) {
    clapp::argument::ms_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THAT(tp, ContainsArgument(arg_str));
    ASSERT_THAT(arg, ArgumentNotGiven());
    ASSERT_THROW(arg.value(), clapp::exception::value_undefined_t);
}

TEST_F(argument_t, msArgumentConstructWithAlreadyRegisteredArgThrows) {
    clapp::argument::ms_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THROW((clapp::argument::ms_argument_t{tp, arg_str, desc_str}),
                 clapp::exception::argument_exception_t);
}

TEST_F(argument_t, msArgumentConstructOptionalStrWithoutValidateFunc) {
    clapp::argument::ms_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::optional};
    ASSERT_THAT(get_validate_func(tp, arg_str), testing::Eq(std::nullopt));
}

TEST_F(argument_t, msArgumentConstructMandatoryStrWithValidateFunc) {
    clapp::argument::ms_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::mandatory};
    ASSERT_THROW(get_validate_func(tp, arg_str).value()(),
                 clapp::exception::argument_exception_t);
}

TEST_F(argument_t, msArgumentCallFoundFunc) {
    clapp::argument::ms_argument_t arg{
        tp, arg_str, desc_str,
        clapp::value::found_func_t{[this]() { found_func_called++; }}};
    ASSERT_THAT(tp, ContainsArgument(arg_str));
    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)("1")));
    ASSERT_THAT(found_func_called, testing::Eq(1));
}

TEST_F(argument_t, msArgumentConstructWithEmpyArgumentNameThrows) {
    ASSERT_THROW((clapp::argument::ms_argument_t{tp, "", desc_cstr}),
                 clapp::exception::argument_exception_t);
}

TEST_F(argument_t, msArgumentConstructStrAndCallArgFunc) {
    clapp::argument::ms_argument_t arg{tp, arg_str, desc_str};
    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)(std::to_string(value_ms.count()))));
    ASSERT_THAT(arg, ArgumentGiven(value_ms));
}

TEST_F(argument_t, msArgumentConstructStrWithDefaultValueAndCallArgFunc) {
    clapp::argument::ms_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::optional,
        clapp::value::default_value_t<std::chrono::milliseconds>{value_sec}};
    ASSERT_THAT(arg, ArgumentNotGivenDefaultValue(
                         std::chrono::milliseconds{value_sec}));
    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)(std::to_string(value_ms.count()))));
    ASSERT_THAT(arg, ArgumentGiven(value_ms));
}

TEST_F(argument_t, msArgumentConstructMandatoryStrAndCallGetOptionHelp) {
    clapp::argument::ms_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THAT(tp.get_argument_help(),
                testing::ContainerEq(argument_test_parser_t::help_entry_vec_t{
                    {arg_str, desc_str + " (" + purpose_mandatory_str + ")"}}));
}

TEST_F(argument_t, msArgumentConstructOptionalStrAndCallGetOptionHelp) {
    clapp::argument::ms_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::optional};
    ASSERT_THAT(tp.get_argument_help(),
                testing::ContainerEq(argument_test_parser_t::help_entry_vec_t{
                    {arg_str, desc_str + " (" + purpose_optional_str + ")"}}));
}

TEST_F(argument_t, variadicMsArgumentConstructOptionalStrWithoutValidateFunc) {
    clapp::argument::ms_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::optional};
    ASSERT_THAT(get_validate_func(tp, arg_str), testing::Eq(std::nullopt));
}

TEST_F(argument_t, variadicMsArgumentConstructMandatoryStrWithValidateFunc) {
    clapp::argument::ms_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::mandatory};
    ASSERT_THROW(get_validate_func(tp, arg_str).value()(),
                 clapp::exception::argument_exception_t);
}

TEST_F(argument_t,
       variadicMsArgumentConstructStrAndCallValueReturnsEmptyVector) {
    clapp::argument::variadic_ms_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THAT(tp, ContainsArgument(arg_str));
    ASSERT_THAT(arg, ArgumentNotGiven());
    ASSERT_THAT(arg.value(),
                testing::Eq(std::vector<std::chrono::milliseconds>{}));
}

TEST_F(argument_t, variadicMsArgumentConstructWithAlreadyRegisteredArgThrows) {
    clapp::argument::variadic_ms_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THROW((clapp::argument::ms_argument_t{tp, arg_str, desc_str}),
                 clapp::exception::argument_exception_t);
}

TEST_F(argument_t, variadicMsArgumentConstructWithEmpyArgumentNameThrows) {
    ASSERT_THROW((clapp::argument::variadic_ms_argument_t{tp, "", desc_cstr}),
                 clapp::exception::argument_exception_t);
}

TEST_F(argument_t,
       variadicMsArgumentConstructStrOptionalAndCallArgFuncMultipleTimes) {
    clapp::argument::variadic_ms_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::optional};
    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)(std::to_string(value_ms.count()))));
    ASSERT_THAT(arg, VariadicArgumentGiven(
                         std::vector<std::chrono::milliseconds>{value_ms}));
    ASSERT_NO_THROW(
        (get_arg_func<argument_test_parser_t::argument_func_t>(tp, arg_str)(
            std::to_string(std::chrono::milliseconds{value_sec}.count()))));
    ASSERT_THAT(
        arg, VariadicArgumentGiven(
                 std::vector<std::chrono::milliseconds>{value_ms, value_sec}));
}

TEST_F(argument_t, variadicMsArgumentConstructStrWithoutCallArgFunc) {
    clapp::argument::variadic_ms_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THAT(arg, VariadicArgumentNotGiven());
}

TEST_F(argument_t, variadicMsArgumentConstructStrAndCallArgFuncMultipleTimes) {
    clapp::argument::variadic_ms_argument_t arg{tp, arg_cstr, desc_str};
    ASSERT_NO_THROW(
        (get_arg_func<argument_test_parser_t::argument_func_t>(tp, arg_cstr)(
            std::to_string(std::chrono::milliseconds{value_sec}.count()))));
    ASSERT_THAT(arg, VariadicArgumentGiven(
                         std::vector<std::chrono::milliseconds>{value_sec}));
    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_cstr)(std::to_string(value_ms.count()))));
    ASSERT_THAT(
        arg, VariadicArgumentGiven(
                 std::vector<std::chrono::milliseconds>{value_sec, value_ms}));
}

TEST_F(argument_t, variadicMsArgumentConstructCStrWithoutCallArgFunc) {
    clapp::argument::variadic_ms_argument_t arg{tp, arg_cstr, desc_str};
    ASSERT_THAT(arg, VariadicArgumentNotGiven());
}

TEST_F(argument_t,
       variadicMsArgumentConstructMandatoryStrAndCallGetOptionHelp) {
    clapp::argument::variadic_ms_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THAT(tp.get_argument_help(),
                testing::ContainerEq(argument_test_parser_t::help_entry_vec_t{
                    {arg_str, desc_str + " (" + purpose_mandatory_str + ", " +
                                  variadic_argument_desc_restriction + ")"}}));
}

TEST_F(argument_t, variadicMsArgumentCallFoundFunc) {
    clapp::argument::variadic_ms_argument_t arg{
        tp, arg_str, desc_str,
        clapp::value::found_func_t{[this]() { found_func_called++; }}};
    ASSERT_THAT(tp, ContainsArgument(arg_str));

    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)("1")));
    ASSERT_THAT(found_func_called, testing::Eq(1));

    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)("11")));
    ASSERT_THAT(found_func_called, testing::Eq(2));
}

TEST_F(argument_t, secArgumentConstructCStrAndCallValueThrows) {
    clapp::argument::sec_argument_t arg{tp, arg_cstr, desc_str};
    ASSERT_THAT(tp, ContainsArgument(arg_cstr));
    ASSERT_THAT(arg, ArgumentNotGiven());
    ASSERT_THROW(arg.value(), clapp::exception::value_undefined_t);
}

TEST_F(argument_t, secArgumentConstructStrAndCallValueThrows) {
    clapp::argument::sec_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THAT(tp, ContainsArgument(arg_str));
    ASSERT_THAT(arg, ArgumentNotGiven());
    ASSERT_THROW(arg.value(), clapp::exception::value_undefined_t);
}

TEST_F(argument_t, secArgumentConstructWithAlreadyRegisteredArgThrows) {
    clapp::argument::sec_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THROW((clapp::argument::sec_argument_t{tp, arg_str, desc_str}),
                 clapp::exception::argument_exception_t);
}

TEST_F(argument_t, secArgumentConstructOptionalStrWithoutValidateFunc) {
    clapp::argument::sec_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::optional};
    ASSERT_THAT(get_validate_func(tp, arg_str), testing::Eq(std::nullopt));
}

TEST_F(argument_t, secArgumentConstructMandatoryStrWithValidateFunc) {
    clapp::argument::sec_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::mandatory};
    ASSERT_THROW(get_validate_func(tp, arg_str).value()(),
                 clapp::exception::argument_exception_t);
}

TEST_F(argument_t, secArgumentCallFoundFunc) {
    clapp::argument::sec_argument_t arg{
        tp, arg_str, desc_str,
        clapp::value::found_func_t{[this]() { found_func_called++; }}};
    ASSERT_THAT(tp, ContainsArgument(arg_str));
    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)("1")));
    ASSERT_THAT(found_func_called, testing::Eq(1));
}

TEST_F(argument_t, secArgumentConstructWithEmpyArgumentNameThrows) {
    ASSERT_THROW((clapp::argument::sec_argument_t{tp, "", desc_cstr}),
                 clapp::exception::argument_exception_t);
}

TEST_F(argument_t, secArgumentConstructStrAndCallArgFunc) {
    clapp::argument::sec_argument_t arg{tp, arg_str, desc_str};
    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)(std::to_string(value_sec.count()))));
    ASSERT_THAT(arg, ArgumentGiven(value_sec));
}

TEST_F(argument_t, secArgumentConstructStrWithDefaultValueAndCallArgFunc) {
    clapp::argument::sec_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::optional,
        clapp::value::default_value_t<std::chrono::seconds>{value_sec}};
    ASSERT_THAT(arg,
                ArgumentNotGivenDefaultValue(std::chrono::seconds{value_sec}));
    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)(std::to_string(value_sec.count()))));
    ASSERT_THAT(arg, ArgumentGiven(value_sec));
}

TEST_F(argument_t, secArgumentConstructMandatoryStrAndCallGetOptionHelp) {
    clapp::argument::sec_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THAT(tp.get_argument_help(),
                testing::ContainerEq(argument_test_parser_t::help_entry_vec_t{
                    {arg_str, desc_str + " (" + purpose_mandatory_str + ")"}}));
}

TEST_F(argument_t, secArgumentConstructOptionalStrAndCallGetOptionHelp) {
    clapp::argument::sec_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::optional};
    ASSERT_THAT(tp.get_argument_help(),
                testing::ContainerEq(argument_test_parser_t::help_entry_vec_t{
                    {arg_str, desc_str + " (" + purpose_optional_str + ")"}}));
}

TEST_F(argument_t, variadicSecArgumentConstructOptionalStrWithoutValidateFunc) {
    clapp::argument::sec_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::optional};
    ASSERT_THAT(get_validate_func(tp, arg_str), testing::Eq(std::nullopt));
}

TEST_F(argument_t, variadicSecArgumentConstructMandatoryStrWithValidateFunc) {
    clapp::argument::sec_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::mandatory};
    ASSERT_THROW(get_validate_func(tp, arg_str).value()(),
                 clapp::exception::argument_exception_t);
}

TEST_F(argument_t,
       variadicSecArgumentConstructStrAndCallValueReturnsEmptyVector) {
    clapp::argument::variadic_sec_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THAT(tp, ContainsArgument(arg_str));
    ASSERT_THAT(arg, ArgumentNotGiven());
    ASSERT_THAT(arg.value(), testing::Eq(std::vector<std::chrono::seconds>{}));
}

TEST_F(argument_t, variadicSecArgumentConstructWithAlreadyRegisteredArgThrows) {
    clapp::argument::variadic_sec_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THROW((clapp::argument::sec_argument_t{tp, arg_str, desc_str}),
                 clapp::exception::argument_exception_t);
}

TEST_F(argument_t, variadicSecArgumentConstructWithEmpyArgumentNameThrows) {
    ASSERT_THROW((clapp::argument::variadic_sec_argument_t{tp, "", desc_cstr}),
                 clapp::exception::argument_exception_t);
}

TEST_F(argument_t,
       variadicSecArgumentConstructStrOptionalAndCallArgFuncMultipleTimes) {
    clapp::argument::variadic_sec_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::optional};
    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)(std::to_string(value_sec.count()))));
    ASSERT_THAT(arg, VariadicArgumentGiven(
                         std::vector<std::chrono::seconds>{value_sec}));
    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)(std::to_string(std::chrono::seconds{value_min}.count()))));
    ASSERT_THAT(arg, VariadicArgumentGiven(std::vector<std::chrono::seconds>{
                         value_sec, value_min}));
}

TEST_F(argument_t, variadicSecArgumentConstructStrWithoutCallArgFunc) {
    clapp::argument::variadic_sec_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THAT(arg, VariadicArgumentNotGiven());
}

TEST_F(argument_t, variadicSecArgumentConstructStrAndCallArgFuncMultipleTimes) {
    clapp::argument::variadic_sec_argument_t arg{tp, arg_cstr, desc_str};
    ASSERT_NO_THROW(
        (get_arg_func<argument_test_parser_t::argument_func_t>(tp, arg_cstr)(
            std::to_string(std::chrono::seconds{value_min}.count()))));
    ASSERT_THAT(arg, VariadicArgumentGiven(
                         std::vector<std::chrono::seconds>{value_min}));
    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_cstr)(std::to_string(value_sec.count()))));
    ASSERT_THAT(
        arg, VariadicArgumentGiven(
                 std::vector<std::chrono::milliseconds>{value_min, value_sec}));
}

TEST_F(argument_t, variadicSecArgumentConstructCStrWithoutCallArgFunc) {
    clapp::argument::variadic_sec_argument_t arg{tp, arg_cstr, desc_str};
    ASSERT_THAT(arg, VariadicArgumentNotGiven());
}

TEST_F(argument_t,
       variadicSecArgumentConstructMandatoryStrAndCallGetOptionHelp) {
    clapp::argument::variadic_sec_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THAT(tp.get_argument_help(),
                testing::ContainerEq(argument_test_parser_t::help_entry_vec_t{
                    {arg_str, desc_str + " (" + purpose_mandatory_str + ", " +
                                  variadic_argument_desc_restriction + ")"}}));
}

TEST_F(argument_t, variadicSecArgumentCallFoundFunc) {
    clapp::argument::variadic_sec_argument_t arg{
        tp, arg_str, desc_str,
        clapp::value::found_func_t{[this]() { found_func_called++; }}};
    ASSERT_THAT(tp, ContainsArgument(arg_str));

    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)("1")));
    ASSERT_THAT(found_func_called, testing::Eq(1));

    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)("11")));
    ASSERT_THAT(found_func_called, testing::Eq(2));
}

TEST_F(argument_t, minArgumentConstructCStrAndCallValueThrows) {
    clapp::argument::min_argument_t arg{tp, arg_cstr, desc_str};
    ASSERT_THAT(tp, ContainsArgument(arg_cstr));
    ASSERT_THAT(arg, ArgumentNotGiven());
    ASSERT_THROW(arg.value(), clapp::exception::value_undefined_t);
}

TEST_F(argument_t, minArgumentConstructStrAndCallValueThrows) {
    clapp::argument::min_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THAT(tp, ContainsArgument(arg_str));
    ASSERT_THAT(arg, ArgumentNotGiven());
    ASSERT_THROW(arg.value(), clapp::exception::value_undefined_t);
}

TEST_F(argument_t, minArgumentConstructWithAlreadyRegisteredArgThrows) {
    clapp::argument::min_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THROW((clapp::argument::min_argument_t{tp, arg_str, desc_str}),
                 clapp::exception::argument_exception_t);
}

TEST_F(argument_t, minArgumentConstructOptionalStrWithoutValidateFunc) {
    clapp::argument::min_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::optional};
    ASSERT_THAT(get_validate_func(tp, arg_str), testing::Eq(std::nullopt));
}

TEST_F(argument_t, minArgumentConstructMandatoryStrWithValidateFunc) {
    clapp::argument::min_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::mandatory};
    ASSERT_THROW(get_validate_func(tp, arg_str).value()(),
                 clapp::exception::argument_exception_t);
}

TEST_F(argument_t, minArgumentCallFoundFunc) {
    clapp::argument::min_argument_t arg{
        tp, arg_str, desc_str,
        clapp::value::found_func_t{[this]() { found_func_called++; }}};
    ASSERT_THAT(tp, ContainsArgument(arg_str));
    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)("1")));
    ASSERT_THAT(found_func_called, testing::Eq(1));
}

TEST_F(argument_t, minArgumentConstructWithEmpyArgumentNameThrows) {
    ASSERT_THROW((clapp::argument::min_argument_t{tp, "", desc_cstr}),
                 clapp::exception::argument_exception_t);
}

TEST_F(argument_t, minArgumentConstructStrAndCallArgFunc) {
    clapp::argument::min_argument_t arg{tp, arg_str, desc_str};
    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)(std::to_string(value_min.count()))));
    ASSERT_THAT(arg, ArgumentGiven(value_min));
}

TEST_F(argument_t, minArgumentConstructStrWithDefaultValueAndCallArgFunc) {
    clapp::argument::min_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::optional,
        clapp::value::default_value_t<std::chrono::minutes>{value_min}};
    ASSERT_THAT(arg,
                ArgumentNotGivenDefaultValue(std::chrono::minutes{value_min}));
    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)(std::to_string(value_min.count()))));
    ASSERT_THAT(arg, ArgumentGiven(value_min));
}

TEST_F(argument_t, minArgumentConstructMandatoryStrAndCallGetOptionHelp) {
    clapp::argument::min_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THAT(tp.get_argument_help(),
                testing::ContainerEq(argument_test_parser_t::help_entry_vec_t{
                    {arg_str, desc_str + " (" + purpose_mandatory_str + ")"}}));
}

TEST_F(argument_t, minArgumentConstructOptionalStrAndCallGetOptionHelp) {
    clapp::argument::min_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::optional};
    ASSERT_THAT(tp.get_argument_help(),
                testing::ContainerEq(argument_test_parser_t::help_entry_vec_t{
                    {arg_str, desc_str + " (" + purpose_optional_str + ")"}}));
}

TEST_F(argument_t, variadicMinArgumentConstructOptionalStrWithoutValidateFunc) {
    clapp::argument::min_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::optional};
    ASSERT_THAT(get_validate_func(tp, arg_str), testing::Eq(std::nullopt));
}

TEST_F(argument_t, variadicMinArgumentConstructMandatoryStrWithValidateFunc) {
    clapp::argument::min_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::mandatory};
    ASSERT_THROW(get_validate_func(tp, arg_str).value()(),
                 clapp::exception::argument_exception_t);
}

TEST_F(argument_t,
       variadicMinArgumentConstructStrAndCallValueReturnsEmptyVector) {
    clapp::argument::variadic_min_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THAT(tp, ContainsArgument(arg_str));
    ASSERT_THAT(arg, ArgumentNotGiven());
    ASSERT_THAT(arg.value(), testing::Eq(std::vector<std::chrono::minutes>{}));
}

TEST_F(argument_t, variadicMinArgumentConstructWithAlreadyRegisteredArgThrows) {
    clapp::argument::variadic_min_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THROW((clapp::argument::min_argument_t{tp, arg_str, desc_str}),
                 clapp::exception::argument_exception_t);
}

TEST_F(argument_t, variadicMinArgumentConstructWithEmpyArgumentNameThrows) {
    ASSERT_THROW((clapp::argument::variadic_min_argument_t{tp, "", desc_cstr}),
                 clapp::exception::argument_exception_t);
}

TEST_F(argument_t,
       variadicMinArgumentConstructStrOptionalAndCallArgFuncMultipleTimes) {
    clapp::argument::variadic_min_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::optional};
    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)(std::to_string(value_min.count()))));
    ASSERT_THAT(arg, VariadicArgumentGiven(
                         std::vector<std::chrono::minutes>{value_min}));
    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)(std::to_string(std::chrono::minutes{value_min}.count()))));
    ASSERT_THAT(arg, VariadicArgumentGiven(std::vector<std::chrono::minutes>{
                         value_min, value_min}));
}

TEST_F(argument_t, variadicMinArgumentConstructStrWithoutCallArgFunc) {
    clapp::argument::variadic_min_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THAT(arg, VariadicArgumentNotGiven());
}

TEST_F(argument_t, variadicMinArgumentConstructStrAndCallArgFuncMultipleTimes) {
    clapp::argument::variadic_min_argument_t arg{tp, arg_cstr, desc_str};
    ASSERT_NO_THROW(
        (get_arg_func<argument_test_parser_t::argument_func_t>(tp, arg_cstr)(
            std::to_string(std::chrono::minutes{value_min}.count()))));
    ASSERT_THAT(arg, VariadicArgumentGiven(
                         std::vector<std::chrono::minutes>{value_min}));
    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_cstr)(std::to_string(value_min.count()))));
    ASSERT_THAT(arg, VariadicArgumentGiven(std::vector<std::chrono::minutes>{
                         value_min, value_min}));
}

TEST_F(argument_t, variadicMinArgumentConstructCStrWithoutCallArgFunc) {
    clapp::argument::variadic_min_argument_t arg{tp, arg_cstr, desc_str};
    ASSERT_THAT(arg, VariadicArgumentNotGiven());
}

TEST_F(argument_t,
       variadicMinArgumentConstructMandatoryStrAndCallGetOptionHelp) {
    clapp::argument::variadic_min_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THAT(tp.get_argument_help(),
                testing::ContainerEq(argument_test_parser_t::help_entry_vec_t{
                    {arg_str, desc_str + " (" + purpose_mandatory_str + ", " +
                                  variadic_argument_desc_restriction + ")"}}));
}

TEST_F(argument_t, variadicMinArgumentCallFoundFunc) {
    clapp::argument::variadic_min_argument_t arg{
        tp, arg_str, desc_str,
        clapp::value::found_func_t{[this]() { found_func_called++; }}};
    ASSERT_THAT(tp, ContainsArgument(arg_str));

    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)("1")));
    ASSERT_THAT(found_func_called, testing::Eq(1));

    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)("11")));
    ASSERT_THAT(found_func_called, testing::Eq(2));
}

TEST_F(argument_t, hoursArgumentConstructCStrAndCallValueThrows) {
    clapp::argument::hours_argument_t arg{tp, arg_cstr, desc_str};
    ASSERT_THAT(tp, ContainsArgument(arg_cstr));
    ASSERT_THAT(arg, ArgumentNotGiven());
    ASSERT_THROW(arg.value(), clapp::exception::value_undefined_t);
}

TEST_F(argument_t, hoursArgumentConstructStrAndCallValueThrows) {
    clapp::argument::hours_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THAT(tp, ContainsArgument(arg_str));
    ASSERT_THAT(arg, ArgumentNotGiven());
    ASSERT_THROW(arg.value(), clapp::exception::value_undefined_t);
}

TEST_F(argument_t, hoursArgumentConstructWithAlreadyRegisteredArgThrows) {
    clapp::argument::hours_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THROW((clapp::argument::hours_argument_t{tp, arg_str, desc_str}),
                 clapp::exception::argument_exception_t);
}

TEST_F(argument_t, hoursArgumentConstructOptionalStrWithoutValidateFunc) {
    clapp::argument::hours_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::optional};
    ASSERT_THAT(get_validate_func(tp, arg_str), testing::Eq(std::nullopt));
}

TEST_F(argument_t, hoursArgumentConstructMandatoryStrWithValidateFunc) {
    clapp::argument::hours_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::mandatory};
    ASSERT_THROW(get_validate_func(tp, arg_str).value()(),
                 clapp::exception::argument_exception_t);
}

TEST_F(argument_t, hoursArgumentCallFoundFunc) {
    clapp::argument::hours_argument_t arg{
        tp, arg_str, desc_str,
        clapp::value::found_func_t{[this]() { found_func_called++; }}};
    ASSERT_THAT(tp, ContainsArgument(arg_str));
    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)("1")));
    ASSERT_THAT(found_func_called, testing::Eq(1));
}

TEST_F(argument_t, hoursArgumentConstructWithEmpyArgumentNameThrows) {
    ASSERT_THROW((clapp::argument::hours_argument_t{tp, "", desc_cstr}),
                 clapp::exception::argument_exception_t);
}

TEST_F(argument_t, hoursArgumentConstructStrAndCallArgFunc) {
    clapp::argument::hours_argument_t arg{tp, arg_str, desc_str};
    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)(std::to_string(value_hours.count()))));
    ASSERT_THAT(arg, ArgumentGiven(value_hours));
}

TEST_F(argument_t, hoursArgumentConstructStrWithDefaultValueAndCallArgFunc) {
    clapp::argument::hours_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::optional,
        clapp::value::default_value_t<std::chrono::hours>{value_hours}};
    ASSERT_THAT(arg,
                ArgumentNotGivenDefaultValue(std::chrono::hours{value_hours}));
    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)(std::to_string(value_hours.count()))));
    ASSERT_THAT(arg, ArgumentGiven(value_hours));
}

TEST_F(argument_t, hoursArgumentConstructMandatoryStrAndCallGetOptionHelp) {
    clapp::argument::hours_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THAT(tp.get_argument_help(),
                testing::ContainerEq(argument_test_parser_t::help_entry_vec_t{
                    {arg_str, desc_str + " (" + purpose_mandatory_str + ")"}}));
}

TEST_F(argument_t, hoursArgumentConstructOptionalStrAndCallGetOptionHelp) {
    clapp::argument::hours_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::optional};
    ASSERT_THAT(tp.get_argument_help(),
                testing::ContainerEq(argument_test_parser_t::help_entry_vec_t{
                    {arg_str, desc_str + " (" + purpose_optional_str + ")"}}));
}

TEST_F(argument_t,
       variadicHoursArgumentConstructOptionalStrWithoutValidateFunc) {
    clapp::argument::hours_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::optional};
    ASSERT_THAT(get_validate_func(tp, arg_str), testing::Eq(std::nullopt));
}

TEST_F(argument_t, variadicHoursArgumentConstructMandatoryStrWithValidateFunc) {
    clapp::argument::hours_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::mandatory};
    ASSERT_THROW(get_validate_func(tp, arg_str).value()(),
                 clapp::exception::argument_exception_t);
}

TEST_F(argument_t,
       variadicHoursArgumentConstructStrAndCallValueReturnsEmptyVector) {
    clapp::argument::variadic_hours_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THAT(tp, ContainsArgument(arg_str));
    ASSERT_THAT(arg, ArgumentNotGiven());
    ASSERT_THAT(arg.value(), testing::Eq(std::vector<std::chrono::hours>{}));
}

TEST_F(argument_t,
       variadicHoursArgumentConstructWithAlreadyRegisteredArgThrows) {
    clapp::argument::variadic_hours_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THROW((clapp::argument::hours_argument_t{tp, arg_str, desc_str}),
                 clapp::exception::argument_exception_t);
}

TEST_F(argument_t, variadicHoursArgumentConstructWithEmpyArgumentNameThrows) {
    ASSERT_THROW(
        (clapp::argument::variadic_hours_argument_t{tp, "", desc_cstr}),
        clapp::exception::argument_exception_t);
}

TEST_F(argument_t,
       variadicHoursArgumentConstructStrOptionalAndCallArgFuncMultipleTimes) {
    clapp::argument::variadic_hours_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::optional};
    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)(std::to_string(value_hours.count()))));
    ASSERT_THAT(arg, VariadicArgumentGiven(
                         std::vector<std::chrono::hours>{value_hours}));
    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)(
        std::to_string(std::chrono::hours{value_hours_additional}.count()))));
    ASSERT_THAT(arg, VariadicArgumentGiven(std::vector<std::chrono::hours>{
                         value_hours, value_hours_additional}));
}

TEST_F(argument_t, variadicHoursArgumentConstructStrWithoutCallArgFunc) {
    clapp::argument::variadic_hours_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THAT(arg, VariadicArgumentNotGiven());
}

TEST_F(argument_t,
       variadicHoursArgumentConstructStrAndCallArgFuncMultipleTimes) {
    clapp::argument::variadic_hours_argument_t arg{tp, arg_cstr, desc_str};
    ASSERT_NO_THROW(
        (get_arg_func<argument_test_parser_t::argument_func_t>(tp, arg_cstr)(
            std::to_string(std::chrono::hours{value_hours}.count()))));
    ASSERT_THAT(arg, VariadicArgumentGiven(
                         std::vector<std::chrono::hours>{value_hours}));
    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_cstr)(std::to_string(value_hours.count()))));
    ASSERT_THAT(arg, VariadicArgumentGiven(std::vector<std::chrono::hours>{
                         value_hours, value_hours}));
}

TEST_F(argument_t, variadicHoursArgumentConstructCStrWithoutCallArgFunc) {
    clapp::argument::variadic_hours_argument_t arg{tp, arg_cstr, desc_str};
    ASSERT_THAT(arg, VariadicArgumentNotGiven());
}

TEST_F(argument_t,
       variadicHoursArgumentConstructMandatoryStrAndCallGetOptionHelp) {
    clapp::argument::variadic_hours_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THAT(tp.get_argument_help(),
                testing::ContainerEq(argument_test_parser_t::help_entry_vec_t{
                    {arg_str, desc_str + " (" + purpose_mandatory_str + ", " +
                                  variadic_argument_desc_restriction + ")"}}));
}

TEST_F(argument_t, variadicHoursArgumentCallFoundFunc) {
    clapp::argument::variadic_hours_argument_t arg{
        tp, arg_str, desc_str,
        clapp::value::found_func_t{[this]() { found_func_called++; }}};
    ASSERT_THAT(tp, ContainsArgument(arg_str));

    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)("1")));
    ASSERT_THAT(found_func_called, testing::Eq(1));

    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)("11")));
    ASSERT_THAT(found_func_called, testing::Eq(2));
}

#ifdef CLAPP_FS_AVAIL

TEST_F(argument_t, pathArgumentConstructCStrAndCallValueThrows) {
    clapp::argument::path_argument_t arg{tp, arg_cstr, desc_str};
    ASSERT_THAT(tp, ContainsArgument(arg_cstr));
    ASSERT_THAT(arg, ArgumentNotGiven());
    ASSERT_THROW(arg.value(), clapp::exception::value_undefined_t);
}

TEST_F(argument_t, pathArgumentConstructStrAndCallValueThrows) {
    clapp::argument::path_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THAT(tp, ContainsArgument(arg_str));
    ASSERT_THAT(arg, ArgumentNotGiven());
    ASSERT_THROW(arg.value(), clapp::exception::value_undefined_t);
}

TEST_F(argument_t, pathArgumentConstructWithAlreadyRegisteredArgThrows) {
    clapp::argument::path_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THROW((clapp::argument::path_argument_t{tp, arg_str, desc_str}),
                 clapp::exception::argument_exception_t);
}

TEST_F(argument_t, pathArgumentConstructOptionalStrWithoutValidateFunc) {
    clapp::argument::path_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::optional};
    ASSERT_THAT(get_validate_func(tp, arg_str), testing::Eq(std::nullopt));
}

TEST_F(argument_t, pathArgumentConstructMandatoryStrWithValidateFunc) {
    clapp::argument::path_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::mandatory};
    ASSERT_THROW(get_validate_func(tp, arg_str).value()(),
                 clapp::exception::argument_exception_t);
}

TEST_F(argument_t, pathArgumentCallFoundFunc) {
    clapp::argument::path_argument_t arg{
        tp, arg_str, desc_str,
        clapp::value::found_func_t{[this]() { found_func_called++; }}};
    ASSERT_THAT(tp, ContainsArgument(arg_str));
    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)(value_path.string())));
    ASSERT_THAT(found_func_called, testing::Eq(1));
}

TEST_F(argument_t, pathArgumentConstructWithEmpyArgumentNameThrows) {
    ASSERT_THROW((clapp::argument::path_argument_t{tp, "", desc_cstr}),
                 clapp::exception::argument_exception_t);
}

TEST_F(argument_t, pathArgumentConstructStrAndCallArgFunc) {
    clapp::argument::path_argument_t arg{tp, arg_str, desc_str};
    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)(value_path_cstring)));
    ASSERT_THAT(arg, ArgumentGiven(clapp::fs::path{value_path_cstring}));
}

TEST_F(argument_t, pathArgumentConstructStrWithDefaultValueAndCallArgFunc) {
    clapp::argument::path_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::optional,
        clapp::value::default_value_t<clapp::fs::path>{value_path_additional}};
    ASSERT_THAT(arg, ArgumentNotGivenDefaultValue(
                         clapp::fs::path{value_path_additional}));
    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)(value_path.string())));
    ASSERT_THAT(arg, ArgumentGiven(value_path));
}

TEST_F(argument_t, pathArgumentConstructMandatoryStrAndCallGetOptionHelp) {
    clapp::argument::path_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THAT(tp.get_argument_help(),
                testing::ContainerEq(argument_test_parser_t::help_entry_vec_t{
                    {arg_str, desc_str + " (" + purpose_mandatory_str + ")"}}));
}

TEST_F(argument_t, pathArgumentConstructOptionalStrAndCallGetOptionHelp) {
    clapp::argument::path_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::optional};
    ASSERT_THAT(tp.get_argument_help(),
                testing::ContainerEq(argument_test_parser_t::help_entry_vec_t{
                    {arg_str, desc_str + " (" + purpose_optional_str + ")"}}));
}

TEST_F(argument_t,
       variadicPathArgumentConstructOptionalStrWithoutValidateFunc) {
    clapp::argument::path_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::optional};
    ASSERT_THAT(get_validate_func(tp, arg_str), testing::Eq(std::nullopt));
}

TEST_F(argument_t, variadicPathArgumentConstructMandatoryStrWithValidateFunc) {
    clapp::argument::path_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::mandatory};
    ASSERT_THROW(get_validate_func(tp, arg_str).value()(),
                 clapp::exception::argument_exception_t);
}

TEST_F(argument_t,
       variadicPathArgumentConstructStrAndCallValueReturnsEmptyVector) {
    clapp::argument::variadic_path_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THAT(tp, ContainsArgument(arg_str));
    ASSERT_THAT(arg, ArgumentNotGiven());
    ASSERT_THAT(arg.value(), testing::Eq(std::vector<clapp::fs::path>{}));
}

TEST_F(argument_t,
       variadicPathArgumentConstructWithAlreadyRegisteredArgThrows) {
    clapp::argument::variadic_path_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THROW((clapp::argument::path_argument_t{tp, arg_str, desc_str}),
                 clapp::exception::argument_exception_t);
}

TEST_F(argument_t, variadicPathArgumentConstructWithEmpyArgumentNameThrows) {
    ASSERT_THROW((clapp::argument::variadic_path_argument_t{tp, "", desc_cstr}),
                 clapp::exception::argument_exception_t);
}

TEST_F(argument_t,
       variadicPathArgumentConstructStrOptionalAndCallArgFuncMultipleTimes) {
    clapp::argument::variadic_path_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::optional};
    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)(value_path.string())));
    ASSERT_THAT(
        arg, VariadicArgumentGiven(std::vector<clapp::fs::path>{value_path}));
    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)(clapp::fs::path{value_path}.string())));
    ASSERT_THAT(arg, VariadicArgumentGiven(
                         std::vector<clapp::fs::path>{value_path, value_path}));
}

TEST_F(argument_t, variadicPathArgumentConstructStrWithoutCallArgFunc) {
    clapp::argument::variadic_path_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THAT(arg, VariadicArgumentNotGiven());
}

TEST_F(argument_t,
       variadicPathArgumentConstructStrAndCallArgFuncMultipleTimes) {
    clapp::argument::variadic_path_argument_t arg{tp, arg_cstr, desc_str};
    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_cstr)(clapp::fs::path{value_path}.string())));
    ASSERT_THAT(
        arg, VariadicArgumentGiven(std::vector<clapp::fs::path>{value_path}));
    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_cstr)(value_path.string())));
    ASSERT_THAT(arg, VariadicArgumentGiven(
                         std::vector<clapp::fs::path>{value_path, value_path}));
}

TEST_F(argument_t, variadicPathArgumentConstructCStrWithoutCallArgFunc) {
    clapp::argument::variadic_path_argument_t arg{tp, arg_cstr, desc_str};
    ASSERT_THAT(arg, VariadicArgumentNotGiven());
}

TEST_F(argument_t,
       variadicPathArgumentConstructMandatoryStrAndCallGetOptionHelp) {
    clapp::argument::variadic_path_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THAT(tp.get_argument_help(),
                testing::ContainerEq(argument_test_parser_t::help_entry_vec_t{
                    {arg_str, desc_str + " (" + purpose_mandatory_str + ", " +
                                  variadic_argument_desc_restriction + ")"}}));
}

TEST_F(argument_t, variadicPathArgumentCallFoundFunc) {
    clapp::argument::variadic_path_argument_t arg{
        tp, arg_str, desc_str,
        clapp::value::found_func_t{[this]() { found_func_called++; }}};

    ASSERT_THAT(tp, ContainsArgument(arg_str));
    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)(value_path.string())));
    ASSERT_THAT(found_func_called, testing::Eq(1));

    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)(value_path_additional.string())));
    ASSERT_THAT(found_func_called, testing::Eq(2));
}

#endif
