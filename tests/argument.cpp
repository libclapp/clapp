#include <clapp/argument.h>
#include <clapp/sub_parser.h>
#include <gmock/gmock.h>

#include "matcher.h"

class argument_test_parser_t : public clapp::parser::basic_parser_t {
   public:
    ~argument_test_parser_t() override;

    using purpose_t = clapp::parser::types::purpose_t;
    using argument_func_t = clapp::parser::types::argument_func_t;
    using argument_type_t = clapp::parser::types::argument_type_t;
    using argument_descriptions_vec_t =
        clapp::parser::basic_parser_t::argument_descriptions_vec_t;
    using validate_value_func_t = clapp::parser::types::validate_value_func_t;
    using given_func_t = clapp::parser::types::given_func_t;

    using clapp::parser::basic_parser_t::get_argument_help;
    using clapp::parser::basic_parser_t::get_arguments;
    using clapp::parser::basic_parser_t::get_mandatory_argument_descriptions;
    using clapp::parser::basic_parser_t::get_optional_argument_descriptions;

    [[nodiscard]] std::string gen_short_line_prefix() const override;
};

class tiny_sub_parser_t : public clapp::parser::basic_sub_parser_t {
   public:
    using clapp::parser::basic_sub_parser_t::basic_sub_parser_t;

    ~tiny_sub_parser_t() override;
};

tiny_sub_parser_t::~tiny_sub_parser_t() = default;

argument_test_parser_t::~argument_test_parser_t() = default;

template <typename ARG_FUNC_T>
static ARG_FUNC_T get_arg_func(argument_test_parser_t& atp,
                               const std::string& argument) {
    std::vector<clapp::parser::types::variant_arg_conf_t> arguments{
        atp.get_arguments()};
    std::optional<clapp::parser::types::variant_arg_conf_t> found_arg{
        contains_argument(arguments, argument)};
    if (found_arg == std::nullopt) {
        throw std::runtime_error("no argument with '" + argument +
                                 "' registered.");
    }
    return std::visit(
        [&argument](auto&& arg) {
            if (arg.argument_name == argument) {
                return ARG_FUNC_T{arg.argument};
            }
            throw std::runtime_error(
                "found long-opt-func, but iterate failed... wtf");
        },
        found_arg.value());
}

template <typename ARGUMENT_T>
static argument_test_parser_t::validate_value_func_t get_validate_value_func(
    argument_test_parser_t& atp, ARGUMENT_T&& argument) {
    std::string argument_string{argument};
    std::vector<clapp::parser::types::variant_arg_conf_t> arguments{
        atp.get_arguments()};
    std::optional<clapp::parser::types::variant_arg_conf_t> found_arg{
        contains_argument(arguments,
                          std::forward<decltype(argument)>(argument))};
    if (found_arg == std::nullopt) {
        throw std::runtime_error("no argument with '" + argument_string +
                                 "' registered.");
    }

    return std::visit(
        [](auto&& value_func) { return value_func.validate_value_func; },
        found_arg.value());
}

template <typename ARGUMENT_T>
static argument_test_parser_t::given_func_t get_given_func(
    argument_test_parser_t& atp, ARGUMENT_T&& argument) {
    std::string argument_string{argument};
    std::vector<clapp::parser::types::variant_arg_conf_t> arguments{
        atp.get_arguments()};
    std::optional<clapp::parser::types::variant_arg_conf_t> found_arg{
        contains_argument(arguments,
                          std::forward<decltype(argument)>(argument))};
    if (found_arg == std::nullopt) {
        throw std::runtime_error("no argument with '" + argument_string +
                                 "' registered.");
    }

    return std::visit([](auto&& value_func) { return value_func.given_func; },
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
    [[nodiscard]] clapp::value::found_func_t::ret_t found_entry(
        std::string_view argument);
    static callbacks_t create_callbacks(test_argument_t* inst);
};

template <typename... Params>
test_argument_t::test_argument_t(clapp::basic_parser_t& parser,
                                 Params... parameters)
    : clapp::basic_argument_t<std::int32_t>{
          parser, std::forward<Params>(parameters)...} {}

clapp::value::found_func_t::ret_t test_argument_t::found_entry(
    const std::string_view argument) {
    for (auto& found_func : _found) {
        const clapp::value::found_func_t::ret_t ret{
            found_func.found(std::string{argument})};
        if (ret) {
            return ret;
        }
    }
    _given = true;
    _value = _value.value() + 1;
    return {};
}

inline test_argument_t::operator bool() const { return _value.value() != 0; }

test_argument_t::~test_argument_t() = default;

test_argument_t::callbacks_t test_argument_t::create_callbacks(
    test_argument_t* inst) {
    return callbacks_t{[inst](const std::string_view argument) {
                           return inst->found_entry(argument);
                       },
                       [inst]() { return inst->given(); },
                       [inst]() { return static_cast<bool>(*inst); },
                       [inst]() { return inst->value(); }};
}

class argumentT : public ::testing::Test {
   protected:
    void SetUp() override {}
    void TearDown() override {}

    static bool throw_unexpected_call();

    using int32_value_func_t = std::function<std::int32_t(void)>;
    using int32_validate_func_t = std::function<void(
        const std::int32_t&, const std::string& option_string)>;

    argument_test_parser_t tp{};
    std::size_t found_func_called{0};
    std::optional<std::string> found_func_arg{};

    inline static const std::string arg_str{"arg-str"};
    inline static constexpr const char* arg_cstr{"arg-cstr"};
    inline static const std::string desc_str{"desc-str"};
    inline static constexpr const char* desc_cstr{"opt-desc-cstr"};
    inline static constexpr const char* value_cstr{"value-cstr"};
    inline static const std::string value_str{"value-str"};
    inline static const std::string purpose_optional_str{"optional"};
    inline static const std::string purpose_mandatory_str{"mandatory"};
    inline static const std::string not_null_str{"not null"};
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
    inline static const float value_float{1415.1617F};
    inline static const float value_float_additional{12.1F};
    inline static constexpr std::chrono::nanoseconds value_ns{1213};
    inline static constexpr std::chrono::microseconds value_us{121};
    inline static constexpr std::chrono::milliseconds value_ms{12};
    inline static constexpr std::chrono::seconds value_sec{3603};
    inline static constexpr std::chrono::minutes value_min{62};
    inline static constexpr std::chrono::hours value_hours{1};
    inline static constexpr std::chrono::hours value_hours_additional{2};
};

bool argumentT::throw_unexpected_call() {
    throw std::runtime_error{"unexpected call"};
}

TEST_F(argumentT, basicArgumentToShortArgumentNameThrows) {
    test_argument_t arg{tp, arg_cstr, desc_cstr};
    ASSERT_THROW(static_cast<void>(arg.value()),
                 clapp::exception::value_undefined_t);
}

TEST_F(argumentT, basicArgumentConstructStrAndCallValueThrows) {
    test_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THROW(static_cast<void>(arg.value()),
                 clapp::exception::value_undefined_t);
}

TEST_F(argumentT, basicArgumentConstructOptionalCallValidateFuncDoesNotThrow) {
    test_argument_t arg{tp, arg_str, desc_str,
                        argument_test_parser_t::purpose_t::optional};
    ASSERT_THAT(get_given_func(tp, arg_str)(), testing::Eq(false));
    ASSERT_NO_THROW(get_validate_value_func(tp, arg_str)("my-str"));
}

TEST_F(argumentT, basicArgumentConstructMandatoryCallValidateFuncDoesNotThrow) {
    test_argument_t arg{tp, arg_str, desc_str,
                        argument_test_parser_t::purpose_t::mandatory};
    ASSERT_THAT(get_given_func(tp, arg_str)(), testing::Eq(false));
    ASSERT_NO_THROW(get_validate_value_func(tp, arg_str)("arg-str"));
}

TEST_F(argumentT, basicArgumentConstructStrWithEmpyArgumentNameThrows) {
    const std::string empty{};
    ASSERT_THROW((test_argument_t{tp, empty, desc_cstr}),
                 clapp::exception::argument_exception_t);
}

TEST_F(argumentT, basicArgumentConstructStrAndCallArgFunc) {
    test_argument_t arg{tp, arg_str, desc_str};
    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)(std::to_string(value_int32))));
    ASSERT_THAT(arg, ArgumentGiven(value_int32));
}

TEST_F(argumentT, basicArgumentConstructStrWithDefaultValueAndCallArgFunc) {
    test_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::optional,
        clapp::value::default_value_t<std::int32_t>{value_int16}};
    ASSERT_THAT(arg, ArgumentNotGivenDefaultValue(value_int16));
    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)(std::to_string(value_int32))));
    ASSERT_THAT(arg, ArgumentGiven(value_int32));
}

TEST_F(argumentT, basicArgumentCallFoundFunc) {
    test_argument_t arg{
        tp, arg_str, desc_str,
        clapp::value::found_func_t{[this](const std::string& argument) {
            found_func_arg = argument;
            found_func_called++;
            return clapp::value::found_func_t::ret_t{};
        }}};
    ASSERT_THAT(tp, ContainsArgument(arg_str));
    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)(std::to_string(value_uint16))));
    ASSERT_THAT(found_func_called, testing::Eq(1));
    ASSERT_THAT(found_func_arg.value(), testing::StrEq(arg_str));
}

TEST_F(argumentT, boolArgumentConstructCStrAndCallValueThrows) {
    clapp::argument::bool_argument_t arg{tp, arg_cstr, desc_str};
    ASSERT_THAT(tp, ContainsArgument(arg_cstr));
    ASSERT_THAT(arg, ArgumentNotGiven());
    ASSERT_THROW(static_cast<void>(arg.value()),
                 clapp::exception::value_undefined_t);
}

TEST_F(argumentT, boolArgumentConstructStrAndCallValueThrows) {
    clapp::argument::bool_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THAT(tp, ContainsArgument(arg_str));
    ASSERT_THAT(arg, ArgumentNotGiven());
    ASSERT_THROW(static_cast<void>(arg.value()),
                 clapp::exception::value_undefined_t);
}

TEST_F(argumentT, boolArgumentConstructWithAlreadyRegisteredArgThrows) {
    clapp::argument::bool_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THROW((clapp::argument::bool_argument_t{tp, arg_str, desc_str}),
                 clapp::exception::argument_exception_t);
}

TEST_F(argumentT, boolArgumentConstructOptionalCallValidateFuncDoesNotThrow) {
    clapp::argument::bool_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::optional};
    ASSERT_THAT(get_given_func(tp, arg_str)(), testing::Eq(false));
    ASSERT_NO_THROW(get_validate_value_func(tp, arg_str)(arg_str));
}

TEST_F(argumentT, boolArgumentConstructMandatoryCallValidateFuncDoesNotThrow) {
    clapp::argument::bool_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::mandatory};
    ASSERT_THAT(get_given_func(tp, arg_str)(), testing::Eq(false));
    ASSERT_NO_THROW(get_validate_value_func(tp, arg_str)(arg_str));
}

TEST_F(argumentT, boolArgumentCallFoundFunc) {
    clapp::argument::bool_argument_t arg{
        tp, arg_str, desc_str,
        clapp::value::found_func_t{[this](const std::string& argument) {
            found_func_arg = argument;
            found_func_called++;
            return clapp::value::found_func_t::ret_t{};
        }}};
    ASSERT_THAT(tp, ContainsArgument(arg_str));
    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)("1")));
    ASSERT_THAT(found_func_called, testing::Eq(1));
    ASSERT_THAT(found_func_arg.value(), testing::StrEq(arg_str));
}

TEST_F(argumentT, boolArgumentConstructWithEmpyArgumentNameThrows) {
    ASSERT_THROW((clapp::argument::bool_argument_t{tp, "", desc_cstr}),
                 clapp::exception::argument_exception_t);
}

TEST_F(argumentT, boolArgumentConstructStrAndCallArgFunc) {
    clapp::argument::bool_argument_t arg{tp, arg_str, desc_str};
    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)(std::to_string(value_true))));
    ASSERT_THAT(arg, ArgumentGiven(value_true));
}

TEST_F(argumentT, boolArgumentConstructStrWithDefaultValueAndCallArgFunc) {
    clapp::argument::bool_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::optional,
        clapp::value::default_value_t<bool>{value_false}};
    ASSERT_THAT(arg, ArgumentNotGivenDefaultValue(value_false));
    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)(std::to_string(value_true))));
    ASSERT_THAT(arg, ArgumentGiven(value_true));
}

TEST_F(argumentT, boolArgumentConstructOptionalStrWithNotNullValue) {
    clapp::argument::bool_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::optional,
        clapp::value::not_null_value_t<bool>{}};

    ASSERT_THAT(get_given_func(tp, arg_str)(), testing::Eq(false));
    argument_test_parser_t::validate_value_func_t argument_validate_func{
        get_validate_value_func(tp, arg_str)};
    get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)(std::to_string(0));

    ASSERT_THAT(get_given_func(tp, arg_str)(), testing::Eq(true));
    ASSERT_THROW((argument_validate_func(arg_str)),
                 clapp::exception::out_of_range_t);
    get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)(std::to_string(1));

    ASSERT_THAT(get_given_func(tp, arg_str)(), testing::Eq(true));
    ASSERT_NO_THROW((argument_validate_func(arg_str)));
}

TEST_F(argumentT, boolArgumentConstructMandatoryStrAndCallGetOptionHelp) {
    clapp::argument::bool_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THAT(tp.get_argument_help(),
                testing::ContainerEq(clapp::parser::types::help_entry_vec_t{
                    {arg_str, desc_str + " (" + purpose_mandatory_str + ")"}}));
}

TEST_F(argumentT, boolArgumentConstructOptionalStrAndCallGetOptionHelp) {
    clapp::argument::bool_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::optional};
    ASSERT_THAT(tp.get_argument_help(),
                testing::ContainerEq(clapp::parser::types::help_entry_vec_t{
                    {arg_str, desc_str + " (" + purpose_optional_str + ")"}}));
}

TEST_F(argumentT, boolArgumentConstructMandatoryAfterOptionalThrows) {
    clapp::argument::bool_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::optional};
    ASSERT_THROW((clapp::argument::bool_argument_t{tp, arg_cstr, desc_str}),
                 clapp::exception::argument_exception_t);
}

TEST_F(argumentT, boolArgumentConstructOptionalAfterSubParserThrows) {
    const std::string sub_parser{"sub"};
    tiny_sub_parser_t sub{tp, sub_parser, desc_str};
    ASSERT_THROW((clapp::argument::bool_argument_t{
                     tp, arg_cstr, desc_str,
                     argument_test_parser_t::purpose_t::optional}),
                 clapp::exception::argument_exception_t);
}

TEST_F(argumentT,
       variadicBoolArgumentConstructOptionalCallValidateFuncDoesNotThrow) {
    clapp::argument::variadic_bool_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::optional};
    ASSERT_THAT(get_given_func(tp, arg_str)(), testing::Eq(false));
    ASSERT_NO_THROW(get_validate_value_func(tp, arg_str)(arg_str));
}

TEST_F(argumentT,
       variadicBoolArgumentConstructMandatoryCallValidateFuncDoesNotThrow) {
    clapp::argument::variadic_bool_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::mandatory};
    ASSERT_THAT(get_given_func(tp, arg_str)(), testing::Eq(false));
    ASSERT_NO_THROW(get_validate_value_func(tp, arg_str)(arg_str));
}

TEST_F(argumentT,
       variadicBoolArgumentConstructStrAndCallValueReturnsEmptyVector) {
    clapp::argument::variadic_bool_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THAT(tp, ContainsArgument(arg_str));
    ASSERT_THAT(arg, VariadicArgumentNotGiven());
    ASSERT_THAT(arg.value(), testing::Eq(std::vector<bool>{}));
}

TEST_F(argumentT, variadicBoolArgumentConstructWithAlreadyRegisteredArgThrows) {
    clapp::argument::variadic_bool_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THROW((clapp::argument::bool_argument_t{tp, arg_str, desc_str}),
                 clapp::exception::argument_exception_t);
}

TEST_F(argumentT, variadicBoolArgumentConstructWithEmpyArgumentNameThrows) {
    ASSERT_THROW((clapp::argument::variadic_bool_argument_t{tp, "", desc_cstr}),
                 clapp::exception::argument_exception_t);
}

TEST_F(argumentT,
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

TEST_F(argumentT, variadicBoolArgumentConstructStrWithoutCallArgFunc) {
    clapp::argument::variadic_bool_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THAT(arg, VariadicArgumentNotGiven());
}

TEST_F(argumentT,
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

TEST_F(argumentT, variadicBoolArgumentConstructCStrWithoutCallArgFunc) {
    clapp::argument::variadic_bool_argument_t arg{tp, arg_cstr, desc_str};
    ASSERT_THAT(arg, VariadicArgumentNotGiven());
}

TEST_F(argumentT,
       variadicBoolArgumentConstructMandatoryStrAndCallGetOptionHelp) {
    clapp::argument::variadic_bool_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THAT(tp.get_argument_help(),
                testing::ContainerEq(clapp::parser::types::help_entry_vec_t{
                    {arg_str, desc_str + " (" + purpose_mandatory_str + ", " +
                                  variadic_argument_desc_restriction + ")"}}));
}

TEST_F(argumentT, variadicBoolArgumentConstructWithNotNullValue) {
    clapp::argument::variadic_bool_argument_t arg{
        tp, arg_str, desc_str, clapp::value::not_null_value_t<bool>{}};

    ASSERT_THAT(get_given_func(tp, arg_str)(), testing::Eq(false));
    argument_test_parser_t::validate_value_func_t argument_validate_func{
        get_validate_value_func(tp, arg_str)};

    get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)(std::to_string(1));
    ASSERT_THAT(get_given_func(tp, arg_str)(), testing::Eq(true));
    ASSERT_NO_THROW((argument_validate_func(arg_str)));

    get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)(std::to_string(0));
    ASSERT_THAT(get_given_func(tp, arg_str)(), testing::Eq(true));
    ASSERT_THROW((argument_validate_func(arg_str)),
                 clapp::exception::out_of_range_t);
}

TEST_F(argumentT, variadicBoolArgumentCallFoundFunc) {
    clapp::argument::variadic_bool_argument_t arg{
        tp, arg_str, desc_str,
        clapp::value::found_func_t{[this](const std::string& argument) {
            found_func_arg = argument;
            found_func_called++;
            return clapp::value::found_func_t::ret_t{};
        }}};
    ASSERT_THAT(tp, ContainsArgument(arg_str));

    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)("1")));
    ASSERT_THAT(found_func_called, testing::Eq(1));
    ASSERT_THAT(found_func_arg.value(), testing::StrEq(arg_str));

    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)("true")));
    ASSERT_THAT(found_func_called, testing::Eq(2));
    ASSERT_THAT(found_func_arg.value(), testing::StrEq(arg_str));
}

TEST_F(argumentT,
       variadicBoolArgumentConstructStrWithMinMaxValueAndCallArgFunc) {
    constexpr bool min_value{false};
    constexpr bool max_value{true};
    constexpr std::int64_t min_value_int64{0};
    constexpr std::int64_t max_value_int64{0};
    clapp::argument::variadic_bool_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::optional,
        clapp::value::min_max_value_t<bool>{min_value, max_value}};

    ASSERT_THAT(get_given_func(tp, arg_str)(), testing::Eq(false));
    argument_test_parser_t::validate_value_func_t argument_validate_func{
        get_validate_value_func(tp, arg_str)};
    get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)(std::to_string(min_value_int64));
    ASSERT_THAT(get_given_func(tp, arg_str)(), testing::Eq(true));
    ASSERT_NO_THROW((argument_validate_func(arg_str)));
    get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)(std::to_string(max_value_int64));
    ASSERT_THAT(get_given_func(tp, arg_str)(), testing::Eq(true));
    ASSERT_NO_THROW((argument_validate_func(arg_str)));
    get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)(std::to_string(min_value_int64));
    ASSERT_THAT(get_given_func(tp, arg_str)(), testing::Eq(true));
    ASSERT_NO_THROW((argument_validate_func(arg_str)));
    get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)(std::to_string(max_value_int64));
    ASSERT_THAT(get_given_func(tp, arg_str)(), testing::Eq(true));
    ASSERT_NO_THROW((argument_validate_func(arg_str)));
}

TEST_F(argumentT, stringArgumentConstructCStrAndCallValueThrows) {
    clapp::argument::string_argument_t arg{tp, arg_cstr, desc_str};
    ASSERT_THAT(tp, ContainsArgument(arg_cstr));
    ASSERT_THAT(arg, ArgumentNotGiven());
    ASSERT_THROW(static_cast<void>(arg.value()),
                 clapp::exception::value_undefined_t);
}

TEST_F(argumentT, stringArgumentConstructStrAndCallValueThrows) {
    clapp::argument::string_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THAT(tp, ContainsArgument(arg_str));
    ASSERT_THAT(arg, ArgumentNotGiven());
    ASSERT_THROW(static_cast<void>(arg.value()),
                 clapp::exception::value_undefined_t);
}

TEST_F(argumentT, stringArgumentConstructWithAlreadyRegisteredArgThrows) {
    clapp::argument::string_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THROW((clapp::argument::string_argument_t{tp, arg_str, desc_str}),
                 clapp::exception::argument_exception_t);
}

TEST_F(argumentT, stringArgumentConstructOptionalCallValidateFuncDoesNotThrow) {
    clapp::argument::string_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::optional};
    ASSERT_THAT(get_given_func(tp, arg_str)(), testing::Eq(false));
    ASSERT_NO_THROW(get_validate_value_func(tp, arg_str)(arg_str));
}

TEST_F(argumentT,
       stringArgumentConstructMandatoryCallValidateFuncDoesNotThrow) {
    clapp::argument::string_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::mandatory};
    ASSERT_THAT(get_given_func(tp, arg_str)(), testing::Eq(false));
    ASSERT_NO_THROW(get_validate_value_func(tp, arg_str)(arg_str));
}

TEST_F(argumentT, stringArgumentCallFoundFunc) {
    clapp::argument::string_argument_t arg{
        tp, arg_str, desc_str,
        clapp::value::found_func_t{[this](const std::string& argument) {
            found_func_arg = argument;
            found_func_called++;
            return clapp::value::found_func_t::ret_t{};
        }}};
    ASSERT_THAT(tp, ContainsArgument(arg_str));
    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)("1")));
    ASSERT_THAT(found_func_called, testing::Eq(1));
    ASSERT_THAT(found_func_arg.value(), testing::StrEq(arg_str));
}

TEST_F(argumentT, stringArgumentConstructWithEmpyArgumentNameThrows) {
    ASSERT_THROW((clapp::argument::string_argument_t{tp, "", desc_cstr}),
                 clapp::exception::argument_exception_t);
}

TEST_F(argumentT, stringArgumentConstructStrAndCallArgFunc) {
    clapp::argument::string_argument_t arg{tp, arg_str, desc_str};
    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)(value_cstr)));
    ASSERT_THAT(arg, ArgumentGiven(value_cstr));
}

TEST_F(argumentT, stringArgumentConstructStrWithDefaultValueAndCallArgFunc) {
    clapp::argument::string_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::optional,
        clapp::value::default_value_t<std::string>{value_cstr}};
    ASSERT_THAT(arg, ArgumentNotGivenDefaultValue(value_cstr));
    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)(value_str)));
    ASSERT_THAT(arg, ArgumentGiven(value_str));
}

TEST_F(argumentT, stringArgumentConstructMandatoryStrAndCallGetOptionHelp) {
    clapp::argument::string_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THAT(tp.get_argument_help(),
                testing::ContainerEq(clapp::parser::types::help_entry_vec_t{
                    {arg_str, desc_str + " (" + purpose_mandatory_str + ")"}}));
}

TEST_F(argumentT, stringArgumentConstructOptionalStrAndCallGetOptionHelp) {
    clapp::argument::string_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::optional};
    ASSERT_THAT(tp.get_argument_help(),
                testing::ContainerEq(clapp::parser::types::help_entry_vec_t{
                    {arg_str, desc_str + " (" + purpose_optional_str + ")"}}));
}

TEST_F(argumentT,
       variadicStringArgumentConstructOptionalCallValidateFuncDoesNotThrow) {
    clapp::argument::string_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::optional};
    ASSERT_THAT(get_given_func(tp, arg_str)(), testing::Eq(false));
    ASSERT_NO_THROW(get_validate_value_func(tp, arg_str)(arg_str));
}

TEST_F(argumentT,
       variadicStringArgumentConstructMandatoryCallValidateFuncDoesNotThrow) {
    clapp::argument::string_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::mandatory};
    ASSERT_THAT(get_given_func(tp, arg_str)(), testing::Eq(false));
    ASSERT_NO_THROW(get_validate_value_func(tp, arg_str)(arg_str));
}

TEST_F(argumentT,
       variadicStringArgumentConstructStrAndCallValueReturnsEmptyVector) {
    clapp::argument::variadic_string_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THAT(tp, ContainsArgument(arg_str));
    ASSERT_THAT(arg, ArgumentNotGiven());
    ASSERT_THAT(arg.value(), testing::Eq(std::vector<std::string>{}));
}

TEST_F(argumentT,
       variadicStringArgumentConstructWithAlreadyRegisteredArgThrows) {
    clapp::argument::variadic_string_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THROW((clapp::argument::string_argument_t{tp, arg_str, desc_str}),
                 clapp::exception::argument_exception_t);
}

TEST_F(argumentT, variadicStringArgumentConstructWithEmpyArgumentNameThrows) {
    ASSERT_THROW(
        (clapp::argument::variadic_string_argument_t{tp, "", desc_cstr}),
        clapp::exception::argument_exception_t);
}

TEST_F(argumentT,
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

TEST_F(argumentT, variadicStringArgumentConstructStrWithoutCallArgFunc) {
    clapp::argument::variadic_string_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THAT(arg, VariadicArgumentNotGiven());
}

TEST_F(argumentT,
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

TEST_F(argumentT, variadicStringArgumentConstructCStrWithoutCallArgFunc) {
    clapp::argument::variadic_string_argument_t arg{tp, arg_cstr, desc_str};
    ASSERT_THAT(arg, VariadicArgumentNotGiven());
}

TEST_F(argumentT, variadicStringArgumentConstructStrAndCallGetOptionHelp) {
    clapp::argument::variadic_string_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THAT(tp.get_argument_help(),
                testing::ContainerEq(clapp::parser::types::help_entry_vec_t{
                    {arg_str, desc_str + " (" + purpose_mandatory_str + ", " +
                                  variadic_argument_desc_restriction + ")"}}));
}

TEST_F(argumentT, variadicStringArgumentCallFoundFunc) {
    clapp::argument::variadic_string_argument_t arg{
        tp, arg_str, desc_str,
        clapp::value::found_func_t{[this](const std::string& argument) {
            found_func_arg = argument;
            found_func_called++;
            return clapp::value::found_func_t::ret_t{};
        }}};
    ASSERT_THAT(tp, ContainsArgument(arg_str));

    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)("1")));
    ASSERT_THAT(found_func_called, testing::Eq(1));
    ASSERT_THAT(found_func_arg.value(), testing::StrEq(arg_str));

    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)("11")));
    ASSERT_THAT(found_func_called, testing::Eq(2));
    ASSERT_THAT(found_func_arg.value(), testing::StrEq(arg_str));
}

TEST_F(argumentT, int64ArgumentConstructCStrAndCallValueThrows) {
    clapp::argument::int64_argument_t arg{tp, arg_cstr, desc_str};
    ASSERT_THAT(tp, ContainsArgument(arg_cstr));
    ASSERT_THAT(arg, ArgumentNotGiven());
    ASSERT_THROW(static_cast<void>(arg.value()),
                 clapp::exception::value_undefined_t);
}

TEST_F(argumentT, int64ArgumentConstructStrAndCallValueThrows) {
    clapp::argument::int64_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THAT(tp, ContainsArgument(arg_str));
    ASSERT_THAT(arg, ArgumentNotGiven());
    ASSERT_THROW(static_cast<void>(arg.value()),
                 clapp::exception::value_undefined_t);
}

TEST_F(argumentT, int64ArgumentConstructWithAlreadyRegisteredArgThrows) {
    clapp::argument::int64_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THROW((clapp::argument::int64_argument_t{tp, arg_str, desc_str}),
                 clapp::exception::argument_exception_t);
}

TEST_F(argumentT, int64ArgumentConstructOptionalCallValidateFuncDoesNotThrow) {
    clapp::argument::int64_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::optional};
    ASSERT_THAT(get_given_func(tp, arg_str)(), testing::Eq(false));
    ASSERT_NO_THROW(get_validate_value_func(tp, arg_str)(arg_str));
}

TEST_F(argumentT, int64ArgumentConstructMandatoryCallValidateFuncDoesNotThrow) {
    clapp::argument::int64_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::mandatory};
    ASSERT_THAT(get_given_func(tp, arg_str)(), testing::Eq(false));
    ASSERT_NO_THROW(get_validate_value_func(tp, arg_str)(arg_str));
}

TEST_F(argumentT, int64ArgumentCallFoundFunc) {
    clapp::argument::int64_argument_t arg{
        tp, arg_str, desc_str,
        clapp::value::found_func_t{[this](const std::string& argument) {
            found_func_arg = argument;
            found_func_called++;
            return clapp::value::found_func_t::ret_t{};
        }}};
    ASSERT_THAT(tp, ContainsArgument(arg_str));
    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)("1")));
    ASSERT_THAT(found_func_called, testing::Eq(1));
    ASSERT_THAT(found_func_arg.value(), testing::StrEq(arg_str));
}

TEST_F(argumentT, int64ArgumentConstructWithEmpyArgumentNameThrows) {
    ASSERT_THROW((clapp::argument::int64_argument_t{tp, "", desc_cstr}),
                 clapp::exception::argument_exception_t);
}

TEST_F(argumentT, int64ArgumentConstructStrAndCallArgFunc) {
    clapp::argument::int64_argument_t arg{tp, arg_str, desc_str};
    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)(std::to_string(value_int64))));
    ASSERT_THAT(arg, ArgumentGiven(value_int64));
}

TEST_F(argumentT, int64ArgumentConstructStrWithDefaultValueAndCallArgFunc) {
    clapp::argument::int64_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::optional,
        clapp::value::default_value_t<std::int64_t>{value_int32}};
    ASSERT_THAT(arg, ArgumentNotGivenDefaultValue(value_int32));
    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)(std::to_string(value_int64))));
    ASSERT_THAT(arg, ArgumentGiven(value_int64));
}

TEST_F(argumentT, int64ArgumentConstructMandatoryStrAndCallGetOptionHelp) {
    clapp::argument::int64_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THAT(tp.get_argument_help(),
                testing::ContainerEq(clapp::parser::types::help_entry_vec_t{
                    {arg_str, desc_str + " (" + purpose_mandatory_str + ")"}}));
}

TEST_F(argumentT, int64ArgumentConstructOptionalStrAndCallGetOptionHelp) {
    clapp::argument::int64_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::optional};
    ASSERT_THAT(tp.get_argument_help(),
                testing::ContainerEq(clapp::parser::types::help_entry_vec_t{
                    {arg_str, desc_str + " (" + purpose_optional_str + ")"}}));
}

TEST_F(argumentT, int64ArgumentConstructStrWithMinMaxValueAndCallArgFunc) {
    constexpr std::int64_t min_value{-2};
    constexpr std::int64_t max_value{0x20};
    clapp::argument::int64_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::optional,
        clapp::value::min_max_value_t<std::int64_t>{min_value, max_value}};
    ASSERT_THAT(get_given_func(tp, arg_str)(), testing::Eq(false));
    argument_test_parser_t::validate_value_func_t argument_validate_func{
        get_validate_value_func(tp, arg_str)};
    get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)(std::to_string(value_int32));
    ASSERT_THAT(get_given_func(tp, arg_str)(), testing::Eq(true));
    ASSERT_THROW((argument_validate_func(arg_str)),
                 clapp::exception::out_of_range_t);
    get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)(std::to_string(value_int8));
    ASSERT_THAT(get_given_func(tp, arg_str)(), testing::Eq(true));
    ASSERT_NO_THROW((argument_validate_func(arg_str)));
    get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)(std::to_string(max_value));
    ASSERT_THAT(get_given_func(tp, arg_str)(), testing::Eq(true));
    ASSERT_NO_THROW((argument_validate_func(arg_str)));
    get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)(std::to_string(min_value));
    ASSERT_THAT(get_given_func(tp, arg_str)(), testing::Eq(true));
    ASSERT_NO_THROW((argument_validate_func(arg_str)));
    get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)(std::to_string(max_value + 1));
    ASSERT_THAT(get_given_func(tp, arg_str)(), testing::Eq(true));
    ASSERT_THROW((argument_validate_func(arg_str)),
                 clapp::exception::out_of_range_t);
    get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)(std::to_string(min_value - 1));
    ASSERT_THAT(get_given_func(tp, arg_str)(), testing::Eq(true));
    ASSERT_THROW((argument_validate_func(arg_str)),
                 clapp::exception::out_of_range_t);
}

TEST_F(argumentT, int64ArgumentConstructStrWithNotNullValue) {
    static constexpr std::int64_t not_null_value{12};
    clapp::argument::int64_argument_t arg{
        tp, arg_str, desc_str, clapp::value::not_null_value_t<std::int64_t>{}};

    ASSERT_THAT(get_given_func(tp, arg_str)(), testing::Eq(false));
    argument_test_parser_t::validate_value_func_t argument_validate_func{
        get_validate_value_func(tp, arg_str)};
    get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)(std::to_string(0));
    ASSERT_THAT(get_given_func(tp, arg_str)(), testing::Eq(true));
    ASSERT_THROW((argument_validate_func(arg_str)),
                 clapp::exception::out_of_range_t);
    get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)(std::to_string(not_null_value));
    ASSERT_NO_THROW((argument_validate_func(arg_str)));
}

TEST_F(argumentT,
       variadicInt64ArgumentConstructOptionalCallValidateFuncDoesNotThrow) {
    clapp::argument::int64_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::optional};
    ASSERT_THAT(get_given_func(tp, arg_str)(), testing::Eq(false));
    ASSERT_NO_THROW(get_validate_value_func(tp, arg_str)(arg_str));
}

TEST_F(argumentT,
       variadicInt64ArgumentConstructMandatoryCallValidateFuncDoesNotThrow) {
    clapp::argument::int64_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::mandatory};
    ASSERT_THAT(get_given_func(tp, arg_str)(), testing::Eq(false));
    ASSERT_NO_THROW(get_validate_value_func(tp, arg_str)(arg_str));
}

TEST_F(argumentT,
       variadicInt64ArgumentConstructStrAndCallValueReturnsEmptyVector) {
    clapp::argument::variadic_int64_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THAT(tp, ContainsArgument(arg_str));
    ASSERT_THAT(arg, ArgumentNotGiven());
    ASSERT_THAT(arg.value(), testing::Eq(std::vector<std::int64_t>{}));
}

TEST_F(argumentT,
       variadicInt64ArgumentConstructWithAlreadyRegisteredArgThrows) {
    clapp::argument::variadic_int64_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THROW((clapp::argument::int64_argument_t{tp, arg_str, desc_str}),
                 clapp::exception::argument_exception_t);
}

TEST_F(argumentT, variadicInt64ArgumentConstructWithEmpyArgumentNameThrows) {
    ASSERT_THROW(
        (clapp::argument::variadic_int64_argument_t{tp, "", desc_cstr}),
        clapp::exception::argument_exception_t);
}

TEST_F(argumentT,
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

TEST_F(argumentT, variadicInt64ArgumentConstructStrWithoutCallArgFunc) {
    clapp::argument::variadic_int64_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THAT(arg, VariadicArgumentNotGiven());
}

TEST_F(argumentT,
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

TEST_F(argumentT, variadicInt64ArgumentConstructCStrWithoutCallArgFunc) {
    clapp::argument::variadic_int64_argument_t arg{tp, arg_cstr, desc_str};
    ASSERT_THAT(arg, VariadicArgumentNotGiven());
}

TEST_F(argumentT,
       variadicInt64ArgumentConstructMandatoryStrAndCallGetOptionHelp) {
    clapp::argument::variadic_int64_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THAT(tp.get_argument_help(),
                testing::ContainerEq(clapp::parser::types::help_entry_vec_t{
                    {arg_str, desc_str + " (" + purpose_mandatory_str + ", " +
                                  variadic_argument_desc_restriction + ")"}}));
}

TEST_F(argumentT, variadicInt64ArgumentCallFoundFunc) {
    clapp::argument::variadic_int64_argument_t arg{
        tp, arg_str, desc_str,
        clapp::value::found_func_t{[this](const std::string& argument) {
            found_func_arg = argument;
            found_func_called++;
            return clapp::value::found_func_t::ret_t{};
        }}};
    ASSERT_THAT(tp, ContainsArgument(arg_str));

    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)("1")));
    ASSERT_THAT(found_func_called, testing::Eq(1));
    ASSERT_THAT(found_func_arg.value(), testing::StrEq(arg_str));

    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)("11")));
    ASSERT_THAT(found_func_called, testing::Eq(2));
    ASSERT_THAT(found_func_arg.value(), testing::StrEq(arg_str));
}

TEST_F(argumentT,
       variadicInt64ArgumentConstructStrWithMinMaxValueAndCallArgFunc) {
    constexpr std::int64_t min_value{-2};
    constexpr std::int64_t max_value{0x20};
    clapp::argument::variadic_int64_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::optional,
        clapp::value::min_max_value_t<std::int64_t>{min_value, max_value}};
    ASSERT_THAT(get_given_func(tp, arg_str)(), testing::Eq(false));
    argument_test_parser_t::validate_value_func_t argument_validate_func{
        get_validate_value_func(tp, arg_str)};
    get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)(std::to_string(value_int8));
    ASSERT_THAT(get_given_func(tp, arg_str)(), testing::Eq(true));
    ASSERT_NO_THROW((argument_validate_func(arg_str)));
    get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)(std::to_string(max_value));
    ASSERT_THAT(get_given_func(tp, arg_str)(), testing::Eq(true));
    ASSERT_NO_THROW((argument_validate_func(arg_str)));
    get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)(std::to_string(min_value));
    ASSERT_THAT(get_given_func(tp, arg_str)(), testing::Eq(true));
    ASSERT_NO_THROW((argument_validate_func(arg_str)));
    get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)(std::to_string(value_int32));
    ASSERT_THAT(get_given_func(tp, arg_str)(), testing::Eq(true));
    ASSERT_THROW((argument_validate_func(arg_str)),
                 clapp::exception::out_of_range_t);
}

TEST_F(argumentT, variadicInt64ArgumentConstructOptionalWithNotNullValue) {
    clapp::argument::variadic_int64_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::optional,
        clapp::value::not_null_value_t<std::int64_t>{}};

    ASSERT_THAT(get_given_func(tp, arg_str)(), testing::Eq(false));
    argument_test_parser_t::validate_value_func_t argument_validate_func{
        get_validate_value_func(tp, arg_str)};

    get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)(std::to_string(1));
    ASSERT_THAT(get_given_func(tp, arg_str)(), testing::Eq(true));
    ASSERT_NO_THROW((argument_validate_func(arg_str)));

    get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)(std::to_string(0));
    ASSERT_THAT(get_given_func(tp, arg_str)(), testing::Eq(true));
    ASSERT_THROW((argument_validate_func(arg_str)),
                 clapp::exception::out_of_range_t);
}

TEST_F(argumentT,
       variadicInt64ArgumentConstructStrWithDefaultValueAndCallArgFunc) {
    ASSERT_THROW((clapp::argument::variadic_int64_argument_t{
                     tp, arg_str, desc_str,
                     clapp::value::default_value_t<std::int64_t>{value_int32}}),
                 clapp::exception::argument_exception_t);
}

TEST_F(argumentT, uint64ArgumentConstructCStrAndCallValueThrows) {
    clapp::argument::uint64_argument_t arg{tp, arg_cstr, desc_str};
    ASSERT_THAT(tp, ContainsArgument(arg_cstr));
    ASSERT_THAT(arg, ArgumentNotGiven());
    ASSERT_THROW(static_cast<void>(arg.value()),
                 clapp::exception::value_undefined_t);
}

TEST_F(argumentT, uint64ArgumentConstructStrAndCallValueThrows) {
    clapp::argument::uint64_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THAT(tp, ContainsArgument(arg_str));
    ASSERT_THAT(arg, ArgumentNotGiven());
    ASSERT_THROW(static_cast<void>(arg.value()),
                 clapp::exception::value_undefined_t);
}

TEST_F(argumentT, uint64ArgumentConstructWithAlreadyRegisteredArgThrows) {
    clapp::argument::uint64_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THROW((clapp::argument::uint64_argument_t{tp, arg_str, desc_str}),
                 clapp::exception::argument_exception_t);
}

TEST_F(argumentT, uint64ArgumentConstructOptionalCallValidateFuncDoesNotThrow) {
    clapp::argument::uint64_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::optional};
    ASSERT_THAT(get_given_func(tp, arg_str)(), testing::Eq(false));
    ASSERT_NO_THROW(get_validate_value_func(tp, arg_str)(arg_str));
}

TEST_F(argumentT,
       uint64ArgumentConstructMandatoryCallValidateFuncDoesNotThrow) {
    clapp::argument::uint64_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::mandatory};
    ASSERT_THAT(get_given_func(tp, arg_str)(), testing::Eq(false));
    ASSERT_NO_THROW(get_validate_value_func(tp, arg_str)(arg_str));
}

TEST_F(argumentT, uint64ArgumentCallFoundFunc) {
    clapp::argument::uint64_argument_t arg{
        tp, arg_str, desc_str,
        clapp::value::found_func_t{[this](const std::string& argument) {
            found_func_arg = argument;
            found_func_called++;
            return clapp::value::found_func_t::ret_t{};
        }}};
    ASSERT_THAT(tp, ContainsArgument(arg_str));
    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)("1")));
    ASSERT_THAT(found_func_called, testing::Eq(1));
    ASSERT_THAT(found_func_arg.value(), testing::StrEq(arg_str));
}

TEST_F(argumentT, uint64ArgumentConstructWithEmpyArgumentNameThrows) {
    ASSERT_THROW((clapp::argument::uint64_argument_t{tp, "", desc_cstr}),
                 clapp::exception::argument_exception_t);
}

TEST_F(argumentT, uint64ArgumentConstructStrAndCallArgFunc) {
    clapp::argument::uint64_argument_t arg{tp, arg_str, desc_str};
    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)(std::to_string(value_uint64))));
    ASSERT_THAT(arg, ArgumentGiven(value_uint64));
}

TEST_F(argumentT, uint64ArgumentConstructStrWithDefaultValueAndCallArgFunc) {
    clapp::argument::uint64_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::optional,
        clapp::value::default_value_t<std::uint64_t>{value_uint32}};
    ASSERT_THAT(arg, ArgumentNotGivenDefaultValue(value_uint32));
    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)(std::to_string(value_uint64))));
    ASSERT_THAT(arg, ArgumentGiven(value_uint64));
}

TEST_F(argumentT, uint64ArgumentConstructMandatoryStrAndCallGetOptionHelp) {
    clapp::argument::uint64_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THAT(tp.get_argument_help(),
                testing::ContainerEq(clapp::parser::types::help_entry_vec_t{
                    {arg_str, desc_str + " (" + purpose_mandatory_str + ")"}}));
}

TEST_F(argumentT, uint64ArgumentConstructOptionalStrAndCallGetOptionHelp) {
    clapp::argument::uint64_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::optional};
    ASSERT_THAT(tp.get_argument_help(),
                testing::ContainerEq(clapp::parser::types::help_entry_vec_t{
                    {arg_str, desc_str + " (" + purpose_optional_str + ")"}}));
}

TEST_F(argumentT, uint64ArgumentConstructStrWithMinMaxValueAndCallArgFunc) {
    constexpr std::uint64_t min_value{0x80U};
    constexpr std::uint64_t max_value{0xdfffU};
    clapp::argument::uint64_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::optional,
        clapp::value::min_max_value_t<std::uint64_t>{min_value, max_value}};

    ASSERT_THAT(get_given_func(tp, arg_str)(), testing::Eq(false));
    argument_test_parser_t::validate_value_func_t argument_validate_func{
        get_validate_value_func(tp, arg_str)};
    get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)(std::to_string(value_uint32));
    ASSERT_THAT(get_given_func(tp, arg_str)(), testing::Eq(true));
    ASSERT_THROW((argument_validate_func(arg_str)),
                 clapp::exception::out_of_range_t);
    get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)(std::to_string(value_uint16));
    ASSERT_THAT(get_given_func(tp, arg_str)(), testing::Eq(true));
    ASSERT_NO_THROW((argument_validate_func(arg_str)));
    get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)(std::to_string(value_uint8));
    ASSERT_THAT(get_given_func(tp, arg_str)(), testing::Eq(true));
    ASSERT_NO_THROW((argument_validate_func(arg_str)));
    get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)(std::to_string(max_value));
    ASSERT_THAT(get_given_func(tp, arg_str)(), testing::Eq(true));
    ASSERT_NO_THROW((argument_validate_func(arg_str)));
    get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)(std::to_string(min_value));
    ASSERT_THAT(get_given_func(tp, arg_str)(), testing::Eq(true));
    ASSERT_NO_THROW((argument_validate_func(arg_str)));
    get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)(std::to_string(max_value + 1));
    ASSERT_THAT(get_given_func(tp, arg_str)(), testing::Eq(true));
    ASSERT_THROW((argument_validate_func(arg_str)),
                 clapp::exception::out_of_range_t);
    get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)(std::to_string(min_value - 1));
    ASSERT_THAT(get_given_func(tp, arg_str)(), testing::Eq(true));
    ASSERT_THROW((argument_validate_func(arg_str)),
                 clapp::exception::out_of_range_t);
}

TEST_F(argumentT,
       variadicUint64ArgumentConstructOptionalCallValidateFuncDoesNotThrow) {
    clapp::argument::uint64_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::optional};
    ASSERT_THAT(get_given_func(tp, arg_str)(), testing::Eq(false));
    ASSERT_NO_THROW(get_validate_value_func(tp, arg_str)(arg_str));
}

TEST_F(argumentT,
       variadicUint64ArgumentConstructMandatoryCallValidateFuncDoesNotThrow) {
    clapp::argument::uint64_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::mandatory};
    ASSERT_THAT(get_given_func(tp, arg_str)(), testing::Eq(false));
    ASSERT_NO_THROW(get_validate_value_func(tp, arg_str)(arg_str));
}

TEST_F(argumentT,
       variadicUint64ArgumentConstructStrAndCallValueReturnsEmptyVector) {
    clapp::argument::variadic_uint64_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THAT(tp, ContainsArgument(arg_str));
    ASSERT_THAT(arg, ArgumentNotGiven());
    ASSERT_THAT(arg.value(), testing::Eq(std::vector<std::uint64_t>{}));
}

TEST_F(argumentT,
       variadicUint64ArgumentConstructWithAlreadyRegisteredArgThrows) {
    clapp::argument::variadic_uint64_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THROW((clapp::argument::uint64_argument_t{tp, arg_str, desc_str}),
                 clapp::exception::argument_exception_t);
}

TEST_F(argumentT, variadicUint64ArgumentConstructWithEmpyArgumentNameThrows) {
    ASSERT_THROW(
        (clapp::argument::variadic_uint64_argument_t{tp, "", desc_cstr}),
        clapp::exception::argument_exception_t);
}

TEST_F(argumentT,
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

TEST_F(argumentT, variadicUint64ArgumentConstructStrWithoutCallArgFunc) {
    clapp::argument::variadic_uint64_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THAT(arg, VariadicArgumentNotGiven());
}

TEST_F(argumentT,
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

TEST_F(argumentT, variadicUint64ArgumentConstructCStrWithoutCallArgFunc) {
    clapp::argument::variadic_uint64_argument_t arg{tp, arg_cstr, desc_str};
    ASSERT_THAT(arg, VariadicArgumentNotGiven());
}

TEST_F(argumentT,
       variadicUint64ArgumentConstructMandatoryStrAndCallGetOptionHelp) {
    clapp::argument::variadic_uint64_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THAT(tp.get_argument_help(),
                testing::ContainerEq(clapp::parser::types::help_entry_vec_t{
                    {arg_str, desc_str + " (" + purpose_mandatory_str + ", " +
                                  variadic_argument_desc_restriction + ")"}}));
}

TEST_F(argumentT, variadicUint64ArgumentCallFoundFunc) {
    clapp::argument::variadic_uint64_argument_t arg{
        tp, arg_str, desc_str,
        clapp::value::found_func_t{[this](const std::string& argument) {
            found_func_arg = argument;
            found_func_called++;
            return clapp::value::found_func_t::ret_t{};
        }}};
    ASSERT_THAT(tp, ContainsArgument(arg_str));

    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)("1")));
    ASSERT_THAT(found_func_called, testing::Eq(1));
    ASSERT_THAT(found_func_arg.value(), testing::StrEq(arg_str));

    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)("11")));
    ASSERT_THAT(found_func_called, testing::Eq(2));
    ASSERT_THAT(found_func_arg.value(), testing::StrEq(arg_str));
}

TEST_F(argumentT, int32ArgumentConstructCStrAndCallValueThrows) {
    clapp::argument::int32_argument_t arg{tp, arg_cstr, desc_str};
    ASSERT_THAT(tp, ContainsArgument(arg_cstr));
    ASSERT_THAT(arg, ArgumentNotGiven());
    ASSERT_THROW(static_cast<void>(arg.value()),
                 clapp::exception::value_undefined_t);
}

TEST_F(argumentT, int32ArgumentConstructStrAndCallValueThrows) {
    clapp::argument::int32_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THAT(tp, ContainsArgument(arg_str));
    ASSERT_THAT(arg, ArgumentNotGiven());
    ASSERT_THROW(static_cast<void>(arg.value()),
                 clapp::exception::value_undefined_t);
}

TEST_F(argumentT, int32ArgumentConstructWithAlreadyRegisteredArgThrows) {
    clapp::argument::int32_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THROW((clapp::argument::int32_argument_t{tp, arg_str, desc_str}),
                 clapp::exception::argument_exception_t);
}

TEST_F(argumentT, int32ArgumentConstructOptionalCallValidateFuncDoesNotThrow) {
    clapp::argument::int32_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::optional};
    ASSERT_THAT(get_given_func(tp, arg_str)(), testing::Eq(false));
    ASSERT_NO_THROW(get_validate_value_func(tp, arg_str)(arg_str));
}

TEST_F(argumentT, int32ArgumentConstructMandatoryCallValidateFuncDoesNotThrow) {
    clapp::argument::int32_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::mandatory};
    ASSERT_THAT(get_given_func(tp, arg_str)(), testing::Eq(false));
    ASSERT_NO_THROW(get_validate_value_func(tp, arg_str)(arg_str));
}

TEST_F(argumentT, int32ArgumentCallFoundFunc) {
    clapp::argument::int32_argument_t arg{
        tp, arg_str, desc_str,
        clapp::value::found_func_t{[this](const std::string& argument) {
            found_func_arg = argument;
            found_func_called++;
            return clapp::value::found_func_t::ret_t{};
        }}};
    ASSERT_THAT(tp, ContainsArgument(arg_str));
    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)("1")));
    ASSERT_THAT(found_func_called, testing::Eq(1));
    ASSERT_THAT(found_func_arg.value(), testing::StrEq(arg_str));
}

TEST_F(argumentT, int32ArgumentConstructWithEmpyArgumentNameThrows) {
    ASSERT_THROW((clapp::argument::int32_argument_t{tp, "", desc_cstr}),
                 clapp::exception::argument_exception_t);
}

TEST_F(argumentT, int32ArgumentConstructStrAndCallArgFunc) {
    clapp::argument::int32_argument_t arg{tp, arg_str, desc_str};
    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)(std::to_string(value_int32))));
    ASSERT_THAT(arg, ArgumentGiven(value_int32));
}

TEST_F(argumentT, int32ArgumentConstructStrWithDefaultValueAndCallArgFunc) {
    clapp::argument::int32_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::optional,
        clapp::value::default_value_t<std::int32_t>{value_int16}};
    ASSERT_THAT(arg, ArgumentNotGivenDefaultValue(value_int16));
    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)(std::to_string(value_int32))));
    ASSERT_THAT(arg, ArgumentGiven(value_int32));
}

TEST_F(argumentT, int32ArgumentConstructMandatoryStrAndCallGetOptionHelp) {
    clapp::argument::int32_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THAT(tp.get_argument_help(),
                testing::ContainerEq(clapp::parser::types::help_entry_vec_t{
                    {arg_str, desc_str + " (" + purpose_mandatory_str + ")"}}));
}

TEST_F(argumentT, int32ArgumentConstructOptionalStrAndCallGetOptionHelp) {
    clapp::argument::int32_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::optional};
    ASSERT_THAT(tp.get_argument_help(),
                testing::ContainerEq(clapp::parser::types::help_entry_vec_t{
                    {arg_str, desc_str + " (" + purpose_optional_str + ")"}}));
}

TEST_F(argumentT,
       variadicInt32ArgumentConstructOptionalCallValidateFuncDoesNotThrow) {
    clapp::argument::int32_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::optional};
    ASSERT_THAT(get_given_func(tp, arg_str)(), testing::Eq(false));
    ASSERT_NO_THROW(get_validate_value_func(tp, arg_str)(arg_str));
}

TEST_F(argumentT,
       variadicInt32ArgumentConstructMandatoryCallValidateFuncDoesNotThrow) {
    clapp::argument::int32_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::mandatory};
    ASSERT_THAT(get_given_func(tp, arg_str)(), testing::Eq(false));
    ASSERT_NO_THROW(get_validate_value_func(tp, arg_str)(arg_str));
}

TEST_F(argumentT,
       variadicInt32ArgumentConstructStrAndCallValueReturnsEmptyVector) {
    clapp::argument::variadic_int32_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THAT(tp, ContainsArgument(arg_str));
    ASSERT_THAT(arg, ArgumentNotGiven());
    ASSERT_THAT(arg.value(), testing::Eq(std::vector<std::int32_t>{}));
}

TEST_F(argumentT,
       variadicInt32ArgumentConstructWithAlreadyRegisteredArgThrows) {
    clapp::argument::variadic_int32_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THROW((clapp::argument::int32_argument_t{tp, arg_str, desc_str}),
                 clapp::exception::argument_exception_t);
}

TEST_F(argumentT, variadicInt32ArgumentConstructWithEmpyArgumentNameThrows) {
    ASSERT_THROW(
        (clapp::argument::variadic_int32_argument_t{tp, "", desc_cstr}),
        clapp::exception::argument_exception_t);
}

TEST_F(argumentT,
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

TEST_F(argumentT, variadicInt32ArgumentConstructStrWithoutCallArgFunc) {
    clapp::argument::variadic_int32_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THAT(arg, VariadicArgumentNotGiven());
}

TEST_F(argumentT,
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

TEST_F(argumentT, variadicInt32ArgumentConstructCStrWithoutCallArgFunc) {
    clapp::argument::variadic_int32_argument_t arg{tp, arg_cstr, desc_str};
    ASSERT_THAT(arg, VariadicArgumentNotGiven());
}

TEST_F(argumentT,
       variadicInt32ArgumentConstructMandatoryStrAndCallGetOptionHelp) {
    clapp::argument::variadic_int32_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THAT(tp.get_argument_help(),
                testing::ContainerEq(clapp::parser::types::help_entry_vec_t{
                    {arg_str, desc_str + " (" + purpose_mandatory_str + ", " +
                                  variadic_argument_desc_restriction + ")"}}));
}

TEST_F(argumentT, variadicInt32ArgumentCallFoundFunc) {
    clapp::argument::variadic_int32_argument_t arg{
        tp, arg_str, desc_str,
        clapp::value::found_func_t{[this](const std::string& argument) {
            found_func_arg = argument;
            found_func_called++;
            return clapp::value::found_func_t::ret_t{};
        }}};
    ASSERT_THAT(tp, ContainsArgument(arg_str));

    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)("1")));
    ASSERT_THAT(found_func_called, testing::Eq(1));
    ASSERT_THAT(found_func_arg.value(), testing::StrEq(arg_str));

    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)("11")));
    ASSERT_THAT(found_func_called, testing::Eq(2));
    ASSERT_THAT(found_func_arg.value(), testing::StrEq(arg_str));
}

TEST_F(argumentT, uint32ArgumentConstructCStrAndCallValueThrows) {
    clapp::argument::uint32_argument_t arg{tp, arg_cstr, desc_str};
    ASSERT_THAT(tp, ContainsArgument(arg_cstr));
    ASSERT_THAT(arg, ArgumentNotGiven());
    ASSERT_THROW(static_cast<void>(arg.value()),
                 clapp::exception::value_undefined_t);
}

TEST_F(argumentT, uint32ArgumentConstructStrAndCallValueThrows) {
    clapp::argument::uint32_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THAT(tp, ContainsArgument(arg_str));
    ASSERT_THAT(arg, ArgumentNotGiven());
    ASSERT_THROW(static_cast<void>(arg.value()),
                 clapp::exception::value_undefined_t);
}

TEST_F(argumentT, uint32ArgumentConstructWithAlreadyRegisteredArgThrows) {
    clapp::argument::uint32_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THROW((clapp::argument::uint32_argument_t{tp, arg_str, desc_str}),
                 clapp::exception::argument_exception_t);
}

TEST_F(argumentT, uint32ArgumentConstructOptionalCallValidateFuncDoesNotThrow) {
    clapp::argument::uint32_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::optional};
    ASSERT_THAT(get_given_func(tp, arg_str)(), testing::Eq(false));
    ASSERT_NO_THROW(get_validate_value_func(tp, arg_str)(arg_str));
}

TEST_F(argumentT,
       uint32ArgumentConstructMandatoryCallValidateFuncDoesNotThrow) {
    clapp::argument::uint32_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::mandatory};
    ASSERT_THAT(get_given_func(tp, arg_str)(), testing::Eq(false));
    ASSERT_NO_THROW(get_validate_value_func(tp, arg_str)(arg_str));
}

TEST_F(argumentT, uint32ArgumentCallFoundFunc) {
    clapp::argument::uint32_argument_t arg{
        tp, arg_str, desc_str,
        clapp::value::found_func_t{[this](const std::string& argument) {
            found_func_arg = argument;
            found_func_called++;
            return clapp::value::found_func_t::ret_t{};
        }}};
    ASSERT_THAT(tp, ContainsArgument(arg_str));
    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)("1")));
    ASSERT_THAT(found_func_called, testing::Eq(1));
    ASSERT_THAT(found_func_arg.value(), testing::StrEq(arg_str));
}

TEST_F(argumentT, uint32ArgumentConstructWithEmpyArgumentNameThrows) {
    ASSERT_THROW((clapp::argument::uint32_argument_t{tp, "", desc_cstr}),
                 clapp::exception::argument_exception_t);
}

TEST_F(argumentT, uint32ArgumentConstructStrAndCallArgFunc) {
    clapp::argument::uint32_argument_t arg{tp, arg_str, desc_str};
    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)(std::to_string(value_uint32))));
    ASSERT_THAT(arg, ArgumentGiven(value_uint32));
}

TEST_F(argumentT, uint32ArgumentConstructStrWithDefaultValueAndCallArgFunc) {
    clapp::argument::uint32_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::optional,
        clapp::value::default_value_t<std::uint32_t>{value_uint16}};
    ASSERT_THAT(arg, ArgumentNotGivenDefaultValue(value_uint16));
    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)(std::to_string(value_uint32))));
    ASSERT_THAT(arg, ArgumentGiven(value_uint32));
}

TEST_F(argumentT, uint32ArgumentConstructMandatoryStrAndCallGetOptionHelp) {
    clapp::argument::uint32_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THAT(tp.get_argument_help(),
                testing::ContainerEq(clapp::parser::types::help_entry_vec_t{
                    {arg_str, desc_str + " (" + purpose_mandatory_str + ")"}}));
}

TEST_F(argumentT, uint32ArgumentConstructOptionalStrAndCallGetOptionHelp) {
    clapp::argument::uint32_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::optional};
    ASSERT_THAT(tp.get_argument_help(),
                testing::ContainerEq(clapp::parser::types::help_entry_vec_t{
                    {arg_str, desc_str + " (" + purpose_optional_str + ")"}}));
}

TEST_F(argumentT,
       variadicUint32ArgumentConstructOptionalCallValidateFuncDoesNotThrow) {
    clapp::argument::uint32_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::optional};
    ASSERT_THAT(get_given_func(tp, arg_str)(), testing::Eq(false));
    ASSERT_NO_THROW(get_validate_value_func(tp, arg_str)(arg_str));
}

TEST_F(argumentT,
       variadicUint32ArgumentConstructMandatoryCallValidateFuncDoesNotThrow) {
    clapp::argument::uint32_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::mandatory};
    ASSERT_THAT(get_given_func(tp, arg_str)(), testing::Eq(false));
    ASSERT_NO_THROW(get_validate_value_func(tp, arg_str)(arg_str));
}

TEST_F(argumentT,
       variadicUint32ArgumentConstructStrAndCallValueReturnsEmptyVector) {
    clapp::argument::variadic_uint32_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THAT(tp, ContainsArgument(arg_str));
    ASSERT_THAT(arg, ArgumentNotGiven());
    ASSERT_THAT(arg.value(), testing::Eq(std::vector<std::uint32_t>{}));
}

TEST_F(argumentT,
       variadicUint32ArgumentConstructWithAlreadyRegisteredArgThrows) {
    clapp::argument::variadic_uint32_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THROW((clapp::argument::uint32_argument_t{tp, arg_str, desc_str}),
                 clapp::exception::argument_exception_t);
}

TEST_F(argumentT, variadicUint32ArgumentConstructWithEmpyArgumentNameThrows) {
    ASSERT_THROW(
        (clapp::argument::variadic_uint32_argument_t{tp, "", desc_cstr}),
        clapp::exception::argument_exception_t);
}

TEST_F(argumentT,
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

TEST_F(argumentT, variadicUint32ArgumentConstructStrWithoutCallArgFunc) {
    clapp::argument::variadic_uint32_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THAT(arg, VariadicArgumentNotGiven());
}

TEST_F(argumentT,
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

TEST_F(argumentT, variadicUint32ArgumentConstructCStrWithoutCallArgFunc) {
    clapp::argument::variadic_uint32_argument_t arg{tp, arg_cstr, desc_str};
    ASSERT_THAT(arg, VariadicArgumentNotGiven());
}

TEST_F(argumentT,
       variadicUint32ArgumentConstructMandatoryStrAndCallGetOptionHelp) {
    clapp::argument::variadic_uint32_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THAT(tp.get_argument_help(),
                testing::ContainerEq(clapp::parser::types::help_entry_vec_t{
                    {arg_str, desc_str + " (" + purpose_mandatory_str + ", " +
                                  variadic_argument_desc_restriction + ")"}}));
}

TEST_F(argumentT, variadicUint32ArgumentCallFoundFunc) {
    clapp::argument::variadic_uint32_argument_t arg{
        tp, arg_str, desc_str,
        clapp::value::found_func_t{[this](const std::string& argument) {
            found_func_arg = argument;
            found_func_called++;
            return clapp::value::found_func_t::ret_t{};
        }}};
    ASSERT_THAT(tp, ContainsArgument(arg_str));

    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)("1")));
    ASSERT_THAT(found_func_called, testing::Eq(1));
    ASSERT_THAT(found_func_arg.value(), testing::StrEq(arg_str));

    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)("11")));
    ASSERT_THAT(found_func_called, testing::Eq(2));
    ASSERT_THAT(found_func_arg.value(), testing::StrEq(arg_str));
}

TEST_F(argumentT, int16ArgumentConstructCStrAndCallValueThrows) {
    clapp::argument::int16_argument_t arg{tp, arg_cstr, desc_str};
    ASSERT_THAT(tp, ContainsArgument(arg_cstr));
    ASSERT_THAT(arg, ArgumentNotGiven());
    ASSERT_THROW(static_cast<void>(arg.value()),
                 clapp::exception::value_undefined_t);
}

TEST_F(argumentT, int16ArgumentConstructStrAndCallValueThrows) {
    clapp::argument::int16_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THAT(tp, ContainsArgument(arg_str));
    ASSERT_THAT(arg, ArgumentNotGiven());
    ASSERT_THROW(static_cast<void>(arg.value()),
                 clapp::exception::value_undefined_t);
}

TEST_F(argumentT, int16ArgumentConstructWithAlreadyRegisteredArgThrows) {
    clapp::argument::int16_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THROW((clapp::argument::int16_argument_t{tp, arg_str, desc_str}),
                 clapp::exception::argument_exception_t);
}

TEST_F(argumentT, int16ArgumentConstructOptionalCallValidateFuncDoesNotThrow) {
    clapp::argument::int16_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::optional};
    ASSERT_THAT(get_given_func(tp, arg_str)(), testing::Eq(false));
    ASSERT_NO_THROW(get_validate_value_func(tp, arg_str)(arg_str));
}

TEST_F(argumentT, int16ArgumentConstructMandatoryCallValidateFuncDoesNotThrow) {
    clapp::argument::int16_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::mandatory};
    ASSERT_THAT(get_given_func(tp, arg_str)(), testing::Eq(false));
    ASSERT_NO_THROW(get_validate_value_func(tp, arg_str)(arg_str));
}

TEST_F(argumentT, int16ArgumentCallFoundFunc) {
    clapp::argument::int16_argument_t arg{
        tp, arg_str, desc_str,
        clapp::value::found_func_t{[this](const std::string& argument) {
            found_func_arg = argument;
            found_func_called++;
            return clapp::value::found_func_t::ret_t{};
        }}};
    ASSERT_THAT(tp, ContainsArgument(arg_str));
    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)("1")));
    ASSERT_THAT(found_func_called, testing::Eq(1));
    ASSERT_THAT(found_func_arg.value(), testing::StrEq(arg_str));
}

TEST_F(argumentT, int16ArgumentConstructWithEmpyArgumentNameThrows) {
    ASSERT_THROW((clapp::argument::int16_argument_t{tp, "", desc_cstr}),
                 clapp::exception::argument_exception_t);
}

TEST_F(argumentT, int16ArgumentConstructStrAndCallArgFunc) {
    clapp::argument::int16_argument_t arg{tp, arg_str, desc_str};
    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)(std::to_string(value_int16))));
    ASSERT_THAT(arg, ArgumentGiven(value_int16));
}

TEST_F(argumentT, int16ArgumentConstructStrWithDefaultValueAndCallArgFunc) {
    clapp::argument::int16_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::optional,
        clapp::value::default_value_t<std::int16_t>{value_int8}};
    ASSERT_THAT(arg, ArgumentNotGivenDefaultValue(value_int8));
    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)(std::to_string(value_int16))));
    ASSERT_THAT(arg, ArgumentGiven(value_int16));
}

TEST_F(argumentT, int16ArgumentConstructMandatoryStrAndCallGetOptionHelp) {
    clapp::argument::int16_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THAT(tp.get_argument_help(),
                testing::ContainerEq(clapp::parser::types::help_entry_vec_t{
                    {arg_str, desc_str + " (" + purpose_mandatory_str + ")"}}));
}

TEST_F(argumentT, int16ArgumentConstructOptionalStrAndCallGetOptionHelp) {
    clapp::argument::int16_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::optional};
    ASSERT_THAT(tp.get_argument_help(),
                testing::ContainerEq(clapp::parser::types::help_entry_vec_t{
                    {arg_str, desc_str + " (" + purpose_optional_str + ")"}}));
}

TEST_F(argumentT,
       variadicInt16ArgumentConstructOptionalCallValidateFuncDoesNotThrow) {
    clapp::argument::int16_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::optional};
    ASSERT_THAT(get_given_func(tp, arg_str)(), testing::Eq(false));
    ASSERT_NO_THROW(get_validate_value_func(tp, arg_str)(arg_str));
}

TEST_F(argumentT,
       variadicInt16ArgumentConstructMandatoryCallValidateFuncDoesNotThrow) {
    clapp::argument::int16_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::mandatory};
    ASSERT_THAT(get_given_func(tp, arg_str)(), testing::Eq(false));
    ASSERT_NO_THROW(get_validate_value_func(tp, arg_str)(arg_str));
}

TEST_F(argumentT,
       variadicInt16ArgumentConstructStrAndCallValueReturnsEmptyVector) {
    clapp::argument::variadic_int16_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THAT(tp, ContainsArgument(arg_str));
    ASSERT_THAT(arg, ArgumentNotGiven());
    ASSERT_THAT(arg.value(), testing::Eq(std::vector<std::int16_t>{}));
}

TEST_F(argumentT,
       variadicInt16ArgumentConstructWithAlreadyRegisteredArgThrows) {
    clapp::argument::variadic_int16_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THROW((clapp::argument::int16_argument_t{tp, arg_str, desc_str}),
                 clapp::exception::argument_exception_t);
}

TEST_F(argumentT, variadicInt16ArgumentConstructWithEmpyArgumentNameThrows) {
    ASSERT_THROW(
        (clapp::argument::variadic_int16_argument_t{tp, "", desc_cstr}),
        clapp::exception::argument_exception_t);
}

TEST_F(argumentT,
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

TEST_F(argumentT, variadicInt16ArgumentConstructStrWithoutCallArgFunc) {
    clapp::argument::variadic_int16_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THAT(arg, VariadicArgumentNotGiven());
}

TEST_F(argumentT,
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

TEST_F(argumentT, variadicInt16ArgumentConstructCStrWithoutCallArgFunc) {
    clapp::argument::variadic_int16_argument_t arg{tp, arg_cstr, desc_str};
    ASSERT_THAT(arg, VariadicArgumentNotGiven());
}

TEST_F(argumentT,
       variadicInt16ArgumentConstructMandatoryStrAndCallGetOptionHelp) {
    clapp::argument::variadic_int16_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THAT(tp.get_argument_help(),
                testing::ContainerEq(clapp::parser::types::help_entry_vec_t{
                    {arg_str, desc_str + " (" + purpose_mandatory_str + ", " +
                                  variadic_argument_desc_restriction + ")"}}));
}

TEST_F(argumentT, variadicInt16ArgumentCallFoundFunc) {
    clapp::argument::variadic_int16_argument_t arg{
        tp, arg_str, desc_str,
        clapp::value::found_func_t{[this](const std::string& argument) {
            found_func_arg = argument;
            found_func_called++;
            return clapp::value::found_func_t::ret_t{};
        }}};
    ASSERT_THAT(tp, ContainsArgument(arg_str));

    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)("1")));
    ASSERT_THAT(found_func_called, testing::Eq(1));
    ASSERT_THAT(found_func_arg.value(), testing::StrEq(arg_str));

    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)("11")));
    ASSERT_THAT(found_func_called, testing::Eq(2));
    ASSERT_THAT(found_func_arg.value(), testing::StrEq(arg_str));
}

TEST_F(argumentT, uint16ArgumentConstructCStrAndCallValueThrows) {
    clapp::argument::uint16_argument_t arg{tp, arg_cstr, desc_str};
    ASSERT_THAT(tp, ContainsArgument(arg_cstr));
    ASSERT_THAT(arg, ArgumentNotGiven());
    ASSERT_THROW(static_cast<void>(arg.value()),
                 clapp::exception::value_undefined_t);
}

TEST_F(argumentT, uint16ArgumentConstructStrAndCallValueThrows) {
    clapp::argument::uint16_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THAT(tp, ContainsArgument(arg_str));
    ASSERT_THAT(arg, ArgumentNotGiven());
    ASSERT_THROW(static_cast<void>(arg.value()),
                 clapp::exception::value_undefined_t);
}

TEST_F(argumentT, uint16ArgumentConstructWithAlreadyRegisteredArgThrows) {
    clapp::argument::uint16_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THROW((clapp::argument::uint16_argument_t{tp, arg_str, desc_str}),
                 clapp::exception::argument_exception_t);
}

TEST_F(argumentT, uint16ArgumentConstructOptionalCallValidateFuncDoesNotThrow) {
    clapp::argument::uint16_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::optional};
    ASSERT_THAT(get_given_func(tp, arg_str)(), testing::Eq(false));
    ASSERT_NO_THROW(get_validate_value_func(tp, arg_str)(arg_str));
}

TEST_F(argumentT,
       uint16ArgumentConstructMandatoryCallValidateFuncDoesNotThrow) {
    clapp::argument::uint16_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::mandatory};
    ASSERT_THAT(get_given_func(tp, arg_str)(), testing::Eq(false));
    ASSERT_NO_THROW(get_validate_value_func(tp, arg_str)(arg_str));
}

TEST_F(argumentT, uint16ArgumentCallFoundFunc) {
    clapp::argument::uint16_argument_t arg{
        tp, arg_str, desc_str,
        clapp::value::found_func_t{[this](const std::string& argument) {
            found_func_arg = argument;
            found_func_called++;
            return clapp::value::found_func_t::ret_t{};
        }}};
    ASSERT_THAT(tp, ContainsArgument(arg_str));
    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)("1")));
    ASSERT_THAT(found_func_called, testing::Eq(1));
    ASSERT_THAT(found_func_arg.value(), testing::StrEq(arg_str));
}

TEST_F(argumentT, uint16ArgumentConstructWithEmpyArgumentNameThrows) {
    ASSERT_THROW((clapp::argument::uint16_argument_t{tp, "", desc_cstr}),
                 clapp::exception::argument_exception_t);
}

TEST_F(argumentT, uint16ArgumentConstructStrAndCallArgFunc) {
    clapp::argument::uint16_argument_t arg{tp, arg_str, desc_str};
    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)(std::to_string(value_uint16))));
    ASSERT_THAT(arg, ArgumentGiven(value_uint16));
}

TEST_F(argumentT, uint16ArgumentConstructStrWithDefaultValueAndCallArgFunc) {
    clapp::argument::uint16_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::optional,
        clapp::value::default_value_t<std::uint16_t>{value_uint8}};
    ASSERT_THAT(arg, ArgumentNotGivenDefaultValue(value_uint8));
    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)(std::to_string(value_uint16))));
    ASSERT_THAT(arg, ArgumentGiven(value_uint16));
}

TEST_F(argumentT, uint16ArgumentConstructMandatoryStrAndCallGetOptionHelp) {
    clapp::argument::uint16_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THAT(tp.get_argument_help(),
                testing::ContainerEq(clapp::parser::types::help_entry_vec_t{
                    {arg_str, desc_str + " (" + purpose_mandatory_str + ")"}}));
}

TEST_F(argumentT, uint16ArgumentConstructOptionalStrAndCallGetOptionHelp) {
    clapp::argument::uint16_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::optional};
    ASSERT_THAT(tp.get_argument_help(),
                testing::ContainerEq(clapp::parser::types::help_entry_vec_t{
                    {arg_str, desc_str + " (" + purpose_optional_str + ")"}}));
}

TEST_F(argumentT,
       variadicUint16ArgumentConstructOptionalCallValidateFuncDoesNotThrow) {
    clapp::argument::uint16_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::optional};
    ASSERT_THAT(get_given_func(tp, arg_str)(), testing::Eq(false));
    ASSERT_NO_THROW(get_validate_value_func(tp, arg_str)(arg_str));
}

TEST_F(argumentT,
       variadicUint16ArgumentConstructMandatoryCallValidateFuncDoesNotThrow) {
    clapp::argument::uint16_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::mandatory};
    ASSERT_THAT(get_given_func(tp, arg_str)(), testing::Eq(false));
    ASSERT_NO_THROW(get_validate_value_func(tp, arg_str)(arg_str));
}

TEST_F(argumentT,
       variadicUint16ArgumentConstructStrAndCallValueReturnsEmptyVector) {
    clapp::argument::variadic_uint16_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THAT(tp, ContainsArgument(arg_str));
    ASSERT_THAT(arg, ArgumentNotGiven());
    ASSERT_THAT(arg.value(), testing::Eq(std::vector<std::uint16_t>{}));
}

TEST_F(argumentT,
       variadicUint16ArgumentConstructWithAlreadyRegisteredArgThrows) {
    clapp::argument::variadic_uint16_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THROW((clapp::argument::uint16_argument_t{tp, arg_str, desc_str}),
                 clapp::exception::argument_exception_t);
}

TEST_F(argumentT, variadicUint16ArgumentConstructWithEmpyArgumentNameThrows) {
    ASSERT_THROW(
        (clapp::argument::variadic_uint16_argument_t{tp, "", desc_cstr}),
        clapp::exception::argument_exception_t);
}

TEST_F(argumentT,
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

TEST_F(argumentT, variadicUint16ArgumentConstructStrWithoutCallArgFunc) {
    clapp::argument::variadic_uint16_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THAT(arg, VariadicArgumentNotGiven());
}

TEST_F(argumentT,
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

TEST_F(argumentT, variadicUint16ArgumentConstructCStrWithoutCallArgFunc) {
    clapp::argument::variadic_uint16_argument_t arg{tp, arg_cstr, desc_str};
    ASSERT_THAT(arg, VariadicArgumentNotGiven());
}

TEST_F(argumentT,
       variadicUint16ArgumentConstructMandatoryStrAndCallGetOptionHelp) {
    clapp::argument::variadic_uint16_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THAT(tp.get_argument_help(),
                testing::ContainerEq(clapp::parser::types::help_entry_vec_t{
                    {arg_str, desc_str + " (" + purpose_mandatory_str + ", " +
                                  variadic_argument_desc_restriction + ")"}}));
}

TEST_F(argumentT, variadicUint16ArgumentCallFoundFunc) {
    clapp::argument::variadic_uint16_argument_t arg{
        tp, arg_str, desc_str,
        clapp::value::found_func_t{[this](const std::string& argument) {
            found_func_arg = argument;
            found_func_called++;
            return clapp::value::found_func_t::ret_t{};
        }}};
    ASSERT_THAT(tp, ContainsArgument(arg_str));

    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)("1")));
    ASSERT_THAT(found_func_called, testing::Eq(1));
    ASSERT_THAT(found_func_arg.value(), testing::StrEq(arg_str));

    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)("11")));
    ASSERT_THAT(found_func_called, testing::Eq(2));
    ASSERT_THAT(found_func_arg.value(), testing::StrEq(arg_str));
}

TEST_F(argumentT, int8ArgumentConstructCStrAndCallValueThrows) {
    clapp::argument::int8_argument_t arg{tp, arg_cstr, desc_str};
    ASSERT_THAT(tp, ContainsArgument(arg_cstr));
    ASSERT_THAT(arg, ArgumentNotGiven());
    ASSERT_THROW(static_cast<void>(arg.value()),
                 clapp::exception::value_undefined_t);
}

TEST_F(argumentT, int8ArgumentConstructStrAndCallValueThrows) {
    clapp::argument::int8_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THAT(tp, ContainsArgument(arg_str));
    ASSERT_THAT(arg, ArgumentNotGiven());
    ASSERT_THROW(static_cast<void>(arg.value()),
                 clapp::exception::value_undefined_t);
}

TEST_F(argumentT, int8ArgumentConstructWithAlreadyRegisteredArgThrows) {
    clapp::argument::int8_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THROW((clapp::argument::int8_argument_t{tp, arg_str, desc_str}),
                 clapp::exception::argument_exception_t);
}

TEST_F(argumentT, int8ArgumentConstructOptionalCallValidateFuncDoesNotThrow) {
    clapp::argument::int8_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::optional};
    ASSERT_THAT(get_given_func(tp, arg_str)(), testing::Eq(false));
    ASSERT_NO_THROW(get_validate_value_func(tp, arg_str)(arg_str));
}

TEST_F(argumentT, int8ArgumentConstructMandatoryCallValidateFuncDoesNotThrow) {
    clapp::argument::int8_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::mandatory};
    ASSERT_THAT(get_given_func(tp, arg_str)(), testing::Eq(false));
    ASSERT_NO_THROW(get_validate_value_func(tp, arg_str)(arg_str));
}

TEST_F(argumentT, int8ArgumentCallFoundFunc) {
    clapp::argument::int8_argument_t arg{
        tp, arg_str, desc_str,
        clapp::value::found_func_t{[this](const std::string& argument) {
            found_func_arg = argument;
            found_func_called++;
            return clapp::value::found_func_t::ret_t{};
        }}};
    ASSERT_THAT(tp, ContainsArgument(arg_str));
    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)("1")));
    ASSERT_THAT(found_func_called, testing::Eq(1));
    ASSERT_THAT(found_func_arg.value(), testing::StrEq(arg_str));
}

TEST_F(argumentT, int8ArgumentConstructWithEmpyArgumentNameThrows) {
    ASSERT_THROW((clapp::argument::int8_argument_t{tp, "", desc_cstr}),
                 clapp::exception::argument_exception_t);
}

TEST_F(argumentT, int8ArgumentConstructStrAndCallArgFunc) {
    clapp::argument::int8_argument_t arg{tp, arg_str, desc_str};
    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)(std::to_string(value_int8))));
    ASSERT_THAT(arg, ArgumentGiven(value_int8));
}

TEST_F(argumentT, int8ArgumentConstructStrWithDefaultValueAndCallArgFunc) {
    clapp::argument::int8_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::optional,
        clapp::value::default_value_t<std::int8_t>{value_int8_additional}};
    ASSERT_THAT(arg, ArgumentNotGivenDefaultValue(value_int8_additional));
    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)(std::to_string(value_int8))));
    ASSERT_THAT(arg, ArgumentGiven(value_int8));
}

TEST_F(argumentT, int8ArgumentConstructMandatoryStrAndCallGetOptionHelp) {
    clapp::argument::int8_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THAT(tp.get_argument_help(),
                testing::ContainerEq(clapp::parser::types::help_entry_vec_t{
                    {arg_str, desc_str + " (" + purpose_mandatory_str + ")"}}));
}

TEST_F(argumentT, int8ArgumentConstructOptionalStrAndCallGetOptionHelp) {
    clapp::argument::int8_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::optional};
    ASSERT_THAT(tp.get_argument_help(),
                testing::ContainerEq(clapp::parser::types::help_entry_vec_t{
                    {arg_str, desc_str + " (" + purpose_optional_str + ")"}}));
}

TEST_F(argumentT,
       variadicInt8ArgumentConstructOptionalCallValidateFuncDoesNotThrow) {
    clapp::argument::int8_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::optional};
    ASSERT_THAT(get_given_func(tp, arg_str)(), testing::Eq(false));
    ASSERT_NO_THROW(get_validate_value_func(tp, arg_str)(arg_str));
}

TEST_F(argumentT,
       variadicInt8ArgumentConstructMandatoryCallValidateFuncDoesNotThrow) {
    clapp::argument::int8_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::mandatory};
    ASSERT_THAT(get_given_func(tp, arg_str)(), testing::Eq(false));
    ASSERT_NO_THROW(get_validate_value_func(tp, arg_str)(arg_str));
}

TEST_F(argumentT,
       variadicInt8ArgumentConstructStrAndCallValueReturnsEmptyVector) {
    clapp::argument::variadic_int8_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THAT(tp, ContainsArgument(arg_str));
    ASSERT_THAT(arg, ArgumentNotGiven());
    ASSERT_THAT(arg.value(), testing::Eq(std::vector<std::int8_t>{}));
}

TEST_F(argumentT, variadicInt8ArgumentConstructWithAlreadyRegisteredArgThrows) {
    clapp::argument::variadic_int8_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THROW((clapp::argument::int8_argument_t{tp, arg_str, desc_str}),
                 clapp::exception::argument_exception_t);
}

TEST_F(argumentT, variadicInt8ArgumentConstructWithEmpyArgumentNameThrows) {
    ASSERT_THROW((clapp::argument::variadic_int8_argument_t{tp, "", desc_cstr}),
                 clapp::exception::argument_exception_t);
}

TEST_F(argumentT,
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

TEST_F(argumentT, variadicInt8ArgumentConstructStrWithoutCallArgFunc) {
    clapp::argument::variadic_int8_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THAT(arg, VariadicArgumentNotGiven());
}

TEST_F(argumentT, variadicInt8ArgumentConstructStrAndCallArgFuncMultipleTimes) {
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

TEST_F(argumentT, variadicInt8ArgumentConstructCStrWithoutCallArgFunc) {
    clapp::argument::variadic_int8_argument_t arg{tp, arg_cstr, desc_str};
    ASSERT_THAT(arg, VariadicArgumentNotGiven());
}

TEST_F(argumentT,
       variadicInt8ArgumentConstructMandatoryStrAndCallGetOptionHelp) {
    clapp::argument::variadic_int8_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THAT(tp.get_argument_help(),
                testing::ContainerEq(clapp::parser::types::help_entry_vec_t{
                    {arg_str, desc_str + " (" + purpose_mandatory_str + ", " +
                                  variadic_argument_desc_restriction + ")"}}));
}

TEST_F(argumentT, variadicInt8ArgumentCallFoundFunc) {
    clapp::argument::variadic_int8_argument_t arg{
        tp, arg_str, desc_str,
        clapp::value::found_func_t{[this](const std::string& argument) {
            found_func_arg = argument;
            found_func_called++;
            return clapp::value::found_func_t::ret_t{};
        }}};
    ASSERT_THAT(tp, ContainsArgument(arg_str));

    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)("1")));
    ASSERT_THAT(found_func_called, testing::Eq(1));
    ASSERT_THAT(found_func_arg.value(), testing::StrEq(arg_str));

    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)("11")));
    ASSERT_THAT(found_func_called, testing::Eq(2));
    ASSERT_THAT(found_func_arg.value(), testing::StrEq(arg_str));
}

TEST_F(argumentT, uint8ArgumentConstructCStrAndCallValueThrows) {
    clapp::argument::uint8_argument_t arg{tp, arg_cstr, desc_str};
    ASSERT_THAT(tp, ContainsArgument(arg_cstr));
    ASSERT_THAT(arg, ArgumentNotGiven());
    ASSERT_THROW(static_cast<void>(arg.value()),
                 clapp::exception::value_undefined_t);
}

TEST_F(argumentT, uint8ArgumentConstructStrAndCallValueThrows) {
    clapp::argument::uint8_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THAT(tp, ContainsArgument(arg_str));
    ASSERT_THAT(arg, ArgumentNotGiven());
    ASSERT_THROW(static_cast<void>(arg.value()),
                 clapp::exception::value_undefined_t);
}

TEST_F(argumentT, uint8ArgumentConstructWithAlreadyRegisteredArgThrows) {
    clapp::argument::uint8_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THROW((clapp::argument::uint8_argument_t{tp, arg_str, desc_str}),
                 clapp::exception::argument_exception_t);
}

TEST_F(argumentT, uint8ArgumentConstructOptionalCallValidateFuncDoesNotThrow) {
    clapp::argument::uint8_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::optional};
    ASSERT_THAT(get_given_func(tp, arg_str)(), testing::Eq(false));
    ASSERT_NO_THROW(get_validate_value_func(tp, arg_str)(arg_str));
}

TEST_F(argumentT, uint8ArgumentConstructMandatoryCallValidateFuncDoesNotThrow) {
    clapp::argument::uint8_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::mandatory};
    ASSERT_THAT(get_given_func(tp, arg_str)(), testing::Eq(false));
    ASSERT_NO_THROW(get_validate_value_func(tp, arg_str)(arg_str));
}

TEST_F(argumentT, uint8ArgumentCallFoundFunc) {
    clapp::argument::uint8_argument_t arg{
        tp, arg_str, desc_str,
        clapp::value::found_func_t{[this](const std::string& argument) {
            found_func_arg = argument;
            found_func_called++;
            return clapp::value::found_func_t::ret_t{};
        }}};
    ASSERT_THAT(tp, ContainsArgument(arg_str));
    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)("1")));
    ASSERT_THAT(found_func_called, testing::Eq(1));
    ASSERT_THAT(found_func_arg.value(), testing::StrEq(arg_str));
}

TEST_F(argumentT, uint8ArgumentConstructWithEmpyArgumentNameThrows) {
    ASSERT_THROW((clapp::argument::uint8_argument_t{tp, "", desc_cstr}),
                 clapp::exception::argument_exception_t);
}

TEST_F(argumentT, uint8ArgumentConstructStrAndCallArgFunc) {
    clapp::argument::uint8_argument_t arg{tp, arg_str, desc_str};
    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)(std::to_string(value_uint8))));
    ASSERT_THAT(arg, ArgumentGiven(value_uint8));
}

TEST_F(argumentT, uint8ArgumentConstructStrWithDefaultValueAndCallArgFunc) {
    clapp::argument::uint8_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::optional,
        clapp::value::default_value_t<std::uint8_t>{value_uint8_additional}};
    ASSERT_THAT(arg, ArgumentNotGivenDefaultValue(value_uint8_additional));
    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)(std::to_string(value_uint8))));
    ASSERT_THAT(arg, ArgumentGiven(value_uint8));
}

TEST_F(argumentT, uint8ArgumentConstructMandatoryStrAndCallGetOptionHelp) {
    clapp::argument::uint8_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THAT(tp.get_argument_help(),
                testing::ContainerEq(clapp::parser::types::help_entry_vec_t{
                    {arg_str, desc_str + " (" + purpose_mandatory_str + ")"}}));
}

TEST_F(argumentT, uint8ArgumentConstructOptionalStrAndCallGetOptionHelp) {
    clapp::argument::uint8_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::optional};
    ASSERT_THAT(tp.get_argument_help(),
                testing::ContainerEq(clapp::parser::types::help_entry_vec_t{
                    {arg_str, desc_str + " (" + purpose_optional_str + ")"}}));
}

TEST_F(argumentT,
       variadicUint8ArgumentConstructOptionalCallValidateFuncDoesNotThrow) {
    clapp::argument::uint8_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::optional};
    ASSERT_THAT(get_given_func(tp, arg_str)(), testing::Eq(false));
    ASSERT_NO_THROW(get_validate_value_func(tp, arg_str)(arg_str));
}

TEST_F(argumentT,
       variadicUint8ArgumentConstructMandatoryCallValidateFuncDoesNotThrow) {
    clapp::argument::uint8_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::mandatory};
    ASSERT_THAT(get_given_func(tp, arg_str)(), testing::Eq(false));
    ASSERT_NO_THROW(get_validate_value_func(tp, arg_str)(arg_str));
}

TEST_F(argumentT,
       variadicUint8ArgumentConstructStrAndCallValueReturnsEmptyVector) {
    clapp::argument::variadic_uint8_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THAT(tp, ContainsArgument(arg_str));
    ASSERT_THAT(arg, ArgumentNotGiven());
    ASSERT_THAT(arg.value(), testing::Eq(std::vector<std::uint8_t>{}));
}

TEST_F(argumentT,
       variadicUint8ArgumentConstructWithAlreadyRegisteredArgThrows) {
    clapp::argument::variadic_uint8_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THROW((clapp::argument::uint8_argument_t{tp, arg_str, desc_str}),
                 clapp::exception::argument_exception_t);
}

TEST_F(argumentT, variadicUint8ArgumentConstructWithEmpyArgumentNameThrows) {
    ASSERT_THROW(
        (clapp::argument::variadic_uint8_argument_t{tp, "", desc_cstr}),
        clapp::exception::argument_exception_t);
}

TEST_F(argumentT,
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

TEST_F(argumentT, variadicUint8ArgumentConstructStrWithoutCallArgFunc) {
    clapp::argument::variadic_uint8_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THAT(arg, VariadicArgumentNotGiven());
}

TEST_F(argumentT,
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

TEST_F(argumentT, variadicUint8ArgumentConstructCStrWithoutCallArgFunc) {
    clapp::argument::variadic_uint8_argument_t arg{tp, arg_cstr, desc_str};
    ASSERT_THAT(arg, VariadicArgumentNotGiven());
}

TEST_F(argumentT,
       variadicUint8ArgumentConstructMandatoryStrAndCallGetOptionHelp) {
    clapp::argument::variadic_uint8_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THAT(tp.get_argument_help(),
                testing::ContainerEq(clapp::parser::types::help_entry_vec_t{
                    {arg_str, desc_str + " (" + purpose_mandatory_str + ", " +
                                  variadic_argument_desc_restriction + ")"}}));
}

TEST_F(argumentT, variadicUint8ArgumentCallFoundFunc) {
    clapp::argument::variadic_uint8_argument_t arg{
        tp, arg_str, desc_str,
        clapp::value::found_func_t{[this](const std::string& argument) {
            found_func_arg = argument;
            found_func_called++;
            return clapp::value::found_func_t::ret_t{};
        }}};
    ASSERT_THAT(tp, ContainsArgument(arg_str));

    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)("1")));
    ASSERT_THAT(found_func_called, testing::Eq(1));
    ASSERT_THAT(found_func_arg.value(), testing::StrEq(arg_str));

    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)("11")));
    ASSERT_THAT(found_func_called, testing::Eq(2));
    ASSERT_THAT(found_func_arg.value(), testing::StrEq(arg_str));
}

TEST_F(argumentT, ptrdiffArgumentConstructCStrAndCallValueThrows) {
    clapp::argument::ptrdiff_argument_t arg{tp, arg_cstr, desc_str};
    ASSERT_THAT(tp, ContainsArgument(arg_cstr));
    ASSERT_THAT(arg, ArgumentNotGiven());
    ASSERT_THROW(static_cast<void>(arg.value()),
                 clapp::exception::value_undefined_t);
}

TEST_F(argumentT, ptrdiffArgumentConstructStrAndCallValueThrows) {
    clapp::argument::ptrdiff_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THAT(tp, ContainsArgument(arg_str));
    ASSERT_THAT(arg, ArgumentNotGiven());
    ASSERT_THROW(static_cast<void>(arg.value()),
                 clapp::exception::value_undefined_t);
}

TEST_F(argumentT, ptrdiffArgumentConstructWithAlreadyRegisteredArgThrows) {
    clapp::argument::ptrdiff_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THROW((clapp::argument::ptrdiff_argument_t{tp, arg_str, desc_str}),
                 clapp::exception::argument_exception_t);
}

TEST_F(argumentT,
       ptrdiffArgumentConstructOptionalCallValidateFuncDoesNotThrow) {
    clapp::argument::ptrdiff_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::optional};
    ASSERT_THAT(get_given_func(tp, arg_str)(), testing::Eq(false));
    ASSERT_NO_THROW(get_validate_value_func(tp, arg_str)(arg_str));
}

TEST_F(argumentT,
       ptrdiffArgumentConstructMandatoryCallValidateFuncDoesNotThrow) {
    clapp::argument::ptrdiff_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::mandatory};
    ASSERT_THAT(get_given_func(tp, arg_str)(), testing::Eq(false));
    ASSERT_NO_THROW(get_validate_value_func(tp, arg_str)(arg_str));
}

TEST_F(argumentT, ptrdiffArgumentCallFoundFunc) {
    clapp::argument::ptrdiff_argument_t arg{
        tp, arg_str, desc_str,
        clapp::value::found_func_t{[this](const std::string& argument) {
            found_func_arg = argument;
            found_func_called++;
            return clapp::value::found_func_t::ret_t{};
        }}};
    ASSERT_THAT(tp, ContainsArgument(arg_str));
    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)("1")));
    ASSERT_THAT(found_func_called, testing::Eq(1));
    ASSERT_THAT(found_func_arg.value(), testing::StrEq(arg_str));
}

TEST_F(argumentT, ptrdiffArgumentConstructWithEmpyArgumentNameThrows) {
    ASSERT_THROW((clapp::argument::ptrdiff_argument_t{tp, "", desc_cstr}),
                 clapp::exception::argument_exception_t);
}

TEST_F(argumentT, ptrdiffArgumentConstructStrAndCallArgFunc) {
    clapp::argument::ptrdiff_argument_t arg{tp, arg_str, desc_str};
    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)(std::to_string(value_ptrdiff))));
    ASSERT_THAT(arg, ArgumentGiven(value_ptrdiff));
}

TEST_F(argumentT, ptrdiffArgumentConstructStrWithDefaultValueAndCallArgFunc) {
    clapp::argument::ptrdiff_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::optional,
        clapp::value::default_value_t<std::ptrdiff_t>{value_int32}};
    ASSERT_THAT(arg, ArgumentNotGivenDefaultValue(value_int32));
    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)(std::to_string(value_ptrdiff))));
    ASSERT_THAT(arg, ArgumentGiven(value_ptrdiff));
}

TEST_F(argumentT, ptrdiffArgumentConstructMandatoryStrAndCallGetOptionHelp) {
    clapp::argument::ptrdiff_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THAT(tp.get_argument_help(),
                testing::ContainerEq(clapp::parser::types::help_entry_vec_t{
                    {arg_str, desc_str + " (" + purpose_mandatory_str + ")"}}));
}

TEST_F(argumentT, ptrdiffArgumentConstructOptionalStrAndCallGetOptionHelp) {
    clapp::argument::ptrdiff_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::optional};
    ASSERT_THAT(tp.get_argument_help(),
                testing::ContainerEq(clapp::parser::types::help_entry_vec_t{
                    {arg_str, desc_str + " (" + purpose_optional_str + ")"}}));
}

TEST_F(argumentT,
       variadicPtrdiffArgumentConstructOptionalCallValidateFuncDoesNotThrow) {
    clapp::argument::ptrdiff_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::optional};
    ASSERT_THAT(get_given_func(tp, arg_str)(), testing::Eq(false));
    ASSERT_NO_THROW(get_validate_value_func(tp, arg_str)(arg_str));
}

TEST_F(argumentT,
       variadicPtrdiffArgumentConstructMandatoryCallValidateFuncDoesNotThrow) {
    clapp::argument::ptrdiff_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::mandatory};
    ASSERT_THAT(get_given_func(tp, arg_str)(), testing::Eq(false));
    ASSERT_NO_THROW(get_validate_value_func(tp, arg_str)(arg_str));
}

TEST_F(argumentT,
       variadicPtrdiffArgumentConstructStrAndCallValueReturnsEmptyVector) {
    clapp::argument::variadic_ptrdiff_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THAT(tp, ContainsArgument(arg_str));
    ASSERT_THAT(arg, ArgumentNotGiven());
    ASSERT_THAT(arg.value(), testing::Eq(std::vector<std::ptrdiff_t>{}));
}

TEST_F(argumentT,
       variadicPtrdiffArgumentConstructWithAlreadyRegisteredArgThrows) {
    clapp::argument::variadic_ptrdiff_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THROW((clapp::argument::ptrdiff_argument_t{tp, arg_str, desc_str}),
                 clapp::exception::argument_exception_t);
}

TEST_F(argumentT, variadicPtrdiffArgumentConstructWithEmpyArgumentNameThrows) {
    ASSERT_THROW(
        (clapp::argument::variadic_ptrdiff_argument_t{tp, "", desc_cstr}),
        clapp::exception::argument_exception_t);
}

TEST_F(argumentT,
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

TEST_F(argumentT, variadicPtrdiffArgumentConstructStrWithoutCallArgFunc) {
    clapp::argument::variadic_ptrdiff_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THAT(arg, VariadicArgumentNotGiven());
}

TEST_F(argumentT,
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

TEST_F(argumentT, variadicPtrdiffArgumentConstructCStrWithoutCallArgFunc) {
    clapp::argument::variadic_ptrdiff_argument_t arg{tp, arg_cstr, desc_str};
    ASSERT_THAT(arg, VariadicArgumentNotGiven());
}

TEST_F(argumentT,
       variadicPtrdiffArgumentConstructMandatoryStrAndCallGetOptionHelp) {
    clapp::argument::variadic_ptrdiff_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THAT(tp.get_argument_help(),
                testing::ContainerEq(clapp::parser::types::help_entry_vec_t{
                    {arg_str, desc_str + " (" + purpose_mandatory_str + ", " +
                                  variadic_argument_desc_restriction + ")"}}));
}

TEST_F(argumentT, variadicPtrdiffArgumentCallFoundFunc) {
    clapp::argument::variadic_ptrdiff_argument_t arg{
        tp, arg_str, desc_str,
        clapp::value::found_func_t{[this](const std::string& argument) {
            found_func_arg = argument;
            found_func_called++;
            return clapp::value::found_func_t::ret_t{};
        }}};
    ASSERT_THAT(tp, ContainsArgument(arg_str));

    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)("1")));
    ASSERT_THAT(found_func_called, testing::Eq(1));
    ASSERT_THAT(found_func_arg.value(), testing::StrEq(arg_str));

    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)("11")));
    ASSERT_THAT(found_func_called, testing::Eq(2));
    ASSERT_THAT(found_func_arg.value(), testing::StrEq(arg_str));
}

TEST_F(argumentT, sizeArgumentConstructCStrAndCallValueThrows) {
    clapp::argument::size_argument_t arg{tp, arg_cstr, desc_str};
    ASSERT_THAT(tp, ContainsArgument(arg_cstr));
    ASSERT_THAT(arg, ArgumentNotGiven());
    ASSERT_THROW(static_cast<void>(arg.value()),
                 clapp::exception::value_undefined_t);
}

TEST_F(argumentT, sizeArgumentConstructStrAndCallValueThrows) {
    clapp::argument::size_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THAT(tp, ContainsArgument(arg_str));
    ASSERT_THAT(arg, ArgumentNotGiven());
    ASSERT_THROW(static_cast<void>(arg.value()),
                 clapp::exception::value_undefined_t);
}

TEST_F(argumentT, sizeArgumentConstructWithAlreadyRegisteredArgThrows) {
    clapp::argument::size_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THROW((clapp::argument::size_argument_t{tp, arg_str, desc_str}),
                 clapp::exception::argument_exception_t);
}

TEST_F(argumentT, sizeArgumentConstructOptionalCallValidateFuncDoesNotThrow) {
    clapp::argument::size_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::optional};
    ASSERT_THAT(get_given_func(tp, arg_str)(), testing::Eq(false));
    ASSERT_NO_THROW(get_validate_value_func(tp, arg_str)(arg_str));
}

TEST_F(argumentT, sizeArgumentConstructMandatoryCallValidateFuncDoesNotThrow) {
    clapp::argument::size_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::mandatory};
    ASSERT_THAT(get_given_func(tp, arg_str)(), testing::Eq(false));
    ASSERT_NO_THROW(get_validate_value_func(tp, arg_str)(arg_str));
}

TEST_F(argumentT, sizeArgumentCallFoundFunc) {
    clapp::argument::size_argument_t arg{
        tp, arg_str, desc_str,
        clapp::value::found_func_t{[this](const std::string& argument) {
            found_func_arg = argument;
            found_func_called++;
            return clapp::value::found_func_t::ret_t{};
        }}};
    ASSERT_THAT(tp, ContainsArgument(arg_str));
    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)("1")));
    ASSERT_THAT(found_func_called, testing::Eq(1));
    ASSERT_THAT(found_func_arg.value(), testing::StrEq(arg_str));
}

TEST_F(argumentT, sizeArgumentConstructWithEmpyArgumentNameThrows) {
    ASSERT_THROW((clapp::argument::size_argument_t{tp, "", desc_cstr}),
                 clapp::exception::argument_exception_t);
}

TEST_F(argumentT, sizeArgumentConstructStrAndCallArgFunc) {
    clapp::argument::size_argument_t arg{tp, arg_str, desc_str};
    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)(std::to_string(value_size))));
    ASSERT_THAT(arg, ArgumentGiven(value_size));
}

TEST_F(argumentT, sizeArgumentConstructStrWithDefaultValueAndCallArgFunc) {
    clapp::argument::size_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::optional,
        clapp::value::default_value_t<std::size_t>{value_uint32}};
    ASSERT_THAT(arg, ArgumentNotGivenDefaultValue(value_uint32));
    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)(std::to_string(value_size))));
    ASSERT_THAT(arg, ArgumentGiven(value_size));
}

TEST_F(argumentT, sizeArgumentConstructMandatoryStrAndCallGetOptionHelp) {
    clapp::argument::size_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THAT(tp.get_argument_help(),
                testing::ContainerEq(clapp::parser::types::help_entry_vec_t{
                    {arg_str, desc_str + " (" + purpose_mandatory_str + ")"}}));
}

TEST_F(argumentT, sizeArgumentConstructOptionalStrAndCallGetOptionHelp) {
    clapp::argument::size_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::optional};
    ASSERT_THAT(tp.get_argument_help(),
                testing::ContainerEq(clapp::parser::types::help_entry_vec_t{
                    {arg_str, desc_str + " (" + purpose_optional_str + ")"}}));
}

TEST_F(argumentT,
       variadicSizeArgumentConstructOptionalCallValidateFuncDoesNotThrow) {
    clapp::argument::size_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::optional};
    ASSERT_THAT(get_given_func(tp, arg_str)(), testing::Eq(false));
    ASSERT_NO_THROW(get_validate_value_func(tp, arg_str)(arg_str));
}

TEST_F(argumentT,
       variadicSizeArgumentConstructMandatoryCallValidateFuncDoesNotThrow) {
    clapp::argument::size_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::mandatory};
    ASSERT_THAT(get_given_func(tp, arg_str)(), testing::Eq(false));
    ASSERT_NO_THROW(get_validate_value_func(tp, arg_str)(arg_str));
}

TEST_F(argumentT,
       variadicSizeArgumentConstructStrAndCallValueReturnsEmptyVector) {
    clapp::argument::variadic_size_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THAT(tp, ContainsArgument(arg_str));
    ASSERT_THAT(arg, ArgumentNotGiven());
    ASSERT_THAT(arg.value(), testing::Eq(std::vector<std::size_t>{}));
}

TEST_F(argumentT, variadicSizeArgumentConstructWithAlreadyRegisteredArgThrows) {
    clapp::argument::variadic_size_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THROW((clapp::argument::size_argument_t{tp, arg_str, desc_str}),
                 clapp::exception::argument_exception_t);
}

TEST_F(argumentT, variadicSizeArgumentConstructWithEmpyArgumentNameThrows) {
    ASSERT_THROW((clapp::argument::variadic_size_argument_t{tp, "", desc_cstr}),
                 clapp::exception::argument_exception_t);
}

TEST_F(argumentT,
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

TEST_F(argumentT, variadicSizeArgumentConstructStrWithoutCallArgFunc) {
    clapp::argument::variadic_size_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THAT(arg, VariadicArgumentNotGiven());
}

TEST_F(argumentT, variadicSizeArgumentConstructStrAndCallArgFuncMultipleTimes) {
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

TEST_F(argumentT, variadicSizeArgumentConstructCStrWithoutCallArgFunc) {
    clapp::argument::variadic_size_argument_t arg{tp, arg_cstr, desc_str};
    ASSERT_THAT(arg, VariadicArgumentNotGiven());
}

TEST_F(argumentT,
       variadicSizeArgumentConstructMandatoryStrAndCallGetOptionHelp) {
    clapp::argument::variadic_size_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THAT(tp.get_argument_help(),
                testing::ContainerEq(clapp::parser::types::help_entry_vec_t{
                    {arg_str, desc_str + " (" + purpose_mandatory_str + ", " +
                                  variadic_argument_desc_restriction + ")"}}));
}

TEST_F(argumentT, variadicSizeArgumentCallFoundFunc) {
    clapp::argument::variadic_size_argument_t arg{
        tp, arg_str, desc_str,
        clapp::value::found_func_t{[this](const std::string& argument) {
            found_func_arg = argument;
            found_func_called++;
            return clapp::value::found_func_t::ret_t{};
        }}};
    ASSERT_THAT(tp, ContainsArgument(arg_str));

    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)("1")));
    ASSERT_THAT(found_func_called, testing::Eq(1));
    ASSERT_THAT(found_func_arg.value(), testing::StrEq(arg_str));

    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)("11")));
    ASSERT_THAT(found_func_called, testing::Eq(2));
    ASSERT_THAT(found_func_arg.value(), testing::StrEq(arg_str));
}

TEST_F(argumentT, floatArgumentConstructCStrAndCallValueThrows) {
    clapp::argument::float_argument_t arg{tp, arg_cstr, desc_str};
    ASSERT_THAT(tp, ContainsArgument(arg_cstr));
    ASSERT_THAT(arg, ArgumentNotGiven());
    ASSERT_THROW(static_cast<void>(arg.value()),
                 clapp::exception::value_undefined_t);
}

TEST_F(argumentT, floatArgumentConstructStrAndCallValueThrows) {
    clapp::argument::float_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THAT(tp, ContainsArgument(arg_str));
    ASSERT_THAT(arg, ArgumentNotGiven());
    ASSERT_THROW(static_cast<void>(arg.value()),
                 clapp::exception::value_undefined_t);
}

TEST_F(argumentT, floatArgumentConstructWithAlreadyRegisteredArgThrows) {
    clapp::argument::float_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THROW((clapp::argument::float_argument_t{tp, arg_str, desc_str}),
                 clapp::exception::argument_exception_t);
}

TEST_F(argumentT, floatArgumentConstructOptionalCallValidateFuncDoesNotThrow) {
    clapp::argument::float_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::optional};
    ASSERT_THAT(get_given_func(tp, arg_str)(), testing::Eq(false));
    ASSERT_NO_THROW(get_validate_value_func(tp, arg_str)(arg_str));
}

TEST_F(argumentT, floatArgumentConstructMandatoryCallValidateFuncDoesNotThrow) {
    clapp::argument::float_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::mandatory};
    ASSERT_THAT(get_given_func(tp, arg_str)(), testing::Eq(false));
    ASSERT_NO_THROW(get_validate_value_func(tp, arg_str)(arg_str));
}

TEST_F(argumentT, floatArgumentCallFoundFunc) {
    clapp::argument::float_argument_t arg{
        tp, arg_str, desc_str,
        clapp::value::found_func_t{[this](const std::string& argument) {
            found_func_arg = argument;
            found_func_called++;
            return clapp::value::found_func_t::ret_t{};
        }}};
    ASSERT_THAT(tp, ContainsArgument(arg_str));
    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)("1")));
    ASSERT_THAT(found_func_called, testing::Eq(1));
    ASSERT_THAT(found_func_arg.value(), testing::StrEq(arg_str));
}

TEST_F(argumentT, floatArgumentConstructWithEmpyArgumentNameThrows) {
    ASSERT_THROW((clapp::argument::float_argument_t{tp, "", desc_cstr}),
                 clapp::exception::argument_exception_t);
}

TEST_F(argumentT, floatArgumentConstructStrAndCallArgFunc) {
    clapp::argument::float_argument_t arg{tp, arg_str, desc_str};
    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)(std::to_string(value_float))));
    ASSERT_THAT(arg, ArgumentGiven(value_float));
}

TEST_F(argumentT, floatArgumentConstructStrWithDefaultValueAndCallArgFunc) {
    clapp::argument::float_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::optional,
        clapp::value::default_value_t<std::float_t>{value_float_additional}};
    ASSERT_THAT(arg, ArgumentNotGivenDefaultValue(value_float_additional));
    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)(std::to_string(value_float))));
    ASSERT_THAT(arg, ArgumentGiven(value_float));
}

TEST_F(argumentT, floatArgumentConstructMandatoryStrAndCallGetOptionHelp) {
    clapp::argument::float_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THAT(tp.get_argument_help(),
                testing::ContainerEq(clapp::parser::types::help_entry_vec_t{
                    {arg_str, desc_str + " (" + purpose_mandatory_str + ")"}}));
}

TEST_F(argumentT, floatArgumentConstructOptionalStrAndCallGetOptionHelp) {
    clapp::argument::float_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::optional};
    ASSERT_THAT(tp.get_argument_help(),
                testing::ContainerEq(clapp::parser::types::help_entry_vec_t{
                    {arg_str, desc_str + " (" + purpose_optional_str + ")"}}));
}

TEST_F(argumentT,
       variadicFloatArgumentConstructOptionalCallValidateFuncDoesNotThrow) {
    clapp::argument::float_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::optional};
    ASSERT_THAT(get_given_func(tp, arg_str)(), testing::Eq(false));
    ASSERT_NO_THROW(get_validate_value_func(tp, arg_str)(arg_str));
}

TEST_F(argumentT,
       variadicFloatArgumentConstructMandatoryCallValidateFuncDoesNotThrow) {
    clapp::argument::float_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::mandatory};
    ASSERT_THAT(get_given_func(tp, arg_str)(), testing::Eq(false));
    ASSERT_NO_THROW(get_validate_value_func(tp, arg_str)(arg_str));
}

TEST_F(argumentT,
       variadicFloatArgumentConstructStrAndCallValueReturnsEmptyVector) {
    clapp::argument::variadic_float_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THAT(tp, ContainsArgument(arg_str));
    ASSERT_THAT(arg, ArgumentNotGiven());
    ASSERT_THAT(arg.value(), testing::Eq(std::vector<std::float_t>{}));
}

TEST_F(argumentT,
       variadicFloatArgumentConstructWithAlreadyRegisteredArgThrows) {
    clapp::argument::variadic_float_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THROW((clapp::argument::float_argument_t{tp, arg_str, desc_str}),
                 clapp::exception::argument_exception_t);
}

TEST_F(argumentT, variadicFloatArgumentConstructWithEmpyArgumentNameThrows) {
    ASSERT_THROW(
        (clapp::argument::variadic_float_argument_t{tp, "", desc_cstr}),
        clapp::exception::argument_exception_t);
}

TEST_F(argumentT,
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

TEST_F(argumentT, variadicFloatArgumentConstructStrWithoutCallArgFunc) {
    clapp::argument::variadic_float_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THAT(arg, VariadicArgumentNotGiven());
}

TEST_F(argumentT,
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

TEST_F(argumentT, variadicFloatArgumentConstructCStrWithoutCallArgFunc) {
    clapp::argument::variadic_float_argument_t arg{tp, arg_cstr, desc_str};
    ASSERT_THAT(arg, VariadicArgumentNotGiven());
}

TEST_F(argumentT,
       variadicFloatArgumentConstructMandatoryStrAndCallGetOptionHelp) {
    clapp::argument::variadic_float_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THAT(tp.get_argument_help(),
                testing::ContainerEq(clapp::parser::types::help_entry_vec_t{
                    {arg_str, desc_str + " (" + purpose_mandatory_str + ", " +
                                  variadic_argument_desc_restriction + ")"}}));
}

TEST_F(argumentT, variadicFloatArgumentCallFoundFunc) {
    clapp::argument::variadic_float_argument_t arg{
        tp, arg_str, desc_str,
        clapp::value::found_func_t{[this](const std::string& argument) {
            found_func_arg = argument;
            found_func_called++;
            return clapp::value::found_func_t::ret_t{};
        }}};
    ASSERT_THAT(tp, ContainsArgument(arg_str));

    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)("1")));
    ASSERT_THAT(found_func_called, testing::Eq(1));
    ASSERT_THAT(found_func_arg.value(), testing::StrEq(arg_str));

    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)("11")));
    ASSERT_THAT(found_func_called, testing::Eq(2));
    ASSERT_THAT(found_func_arg.value(), testing::StrEq(arg_str));
}

TEST_F(argumentT, doubleArgumentConstructCStrAndCallValueThrows) {
    clapp::argument::double_argument_t arg{tp, arg_cstr, desc_str};
    ASSERT_THAT(tp, ContainsArgument(arg_cstr));
    ASSERT_THAT(arg, ArgumentNotGiven());
    ASSERT_THROW(static_cast<void>(arg.value()),
                 clapp::exception::value_undefined_t);
}

TEST_F(argumentT, doubleArgumentConstructStrAndCallValueThrows) {
    clapp::argument::double_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THAT(tp, ContainsArgument(arg_str));
    ASSERT_THAT(arg, ArgumentNotGiven());
    ASSERT_THROW(static_cast<void>(arg.value()),
                 clapp::exception::value_undefined_t);
}

TEST_F(argumentT, doubleArgumentConstructWithAlreadyRegisteredArgThrows) {
    clapp::argument::double_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THROW((clapp::argument::double_argument_t{tp, arg_str, desc_str}),
                 clapp::exception::argument_exception_t);
}

TEST_F(argumentT, doubleArgumentConstructOptionalCallValidateFuncDoesNotThrow) {
    clapp::argument::double_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::optional};
    ASSERT_THAT(get_given_func(tp, arg_str)(), testing::Eq(false));
    ASSERT_NO_THROW(get_validate_value_func(tp, arg_str)(arg_str));
}

TEST_F(argumentT,
       doubleArgumentConstructMandatoryCallValidateFuncDoesNotThrow) {
    clapp::argument::double_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::mandatory};
    ASSERT_THAT(get_given_func(tp, arg_str)(), testing::Eq(false));
    ASSERT_NO_THROW(get_validate_value_func(tp, arg_str)(arg_str));
}

TEST_F(argumentT, doubleArgumentCallFoundFunc) {
    clapp::argument::double_argument_t arg{
        tp, arg_str, desc_str,
        clapp::value::found_func_t{[this](const std::string& argument) {
            found_func_arg = argument;
            found_func_called++;
            return clapp::value::found_func_t::ret_t{};
        }}};
    ASSERT_THAT(tp, ContainsArgument(arg_str));
    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)("1")));
    ASSERT_THAT(found_func_called, testing::Eq(1));
    ASSERT_THAT(found_func_arg.value(), testing::StrEq(arg_str));
}

TEST_F(argumentT, doubleArgumentConstructWithEmpyArgumentNameThrows) {
    ASSERT_THROW((clapp::argument::double_argument_t{tp, "", desc_cstr}),
                 clapp::exception::argument_exception_t);
}

TEST_F(argumentT, doubleArgumentConstructStrAndCallArgFunc) {
    clapp::argument::double_argument_t arg{tp, arg_str, desc_str};
    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)(std::to_string(value_double))));
    ASSERT_THAT(arg, ArgumentGiven(value_double));
}

TEST_F(argumentT, doubleArgumentConstructStrWithDefaultValueAndCallArgFunc) {
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

TEST_F(argumentT, doubleArgumentConstructMandatoryStrAndCallGetOptionHelp) {
    clapp::argument::double_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THAT(tp.get_argument_help(),
                testing::ContainerEq(clapp::parser::types::help_entry_vec_t{
                    {arg_str, desc_str + " (" + purpose_mandatory_str + ")"}}));
}

TEST_F(argumentT, doubleArgumentConstructOptionalStrAndCallGetOptionHelp) {
    clapp::argument::double_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::optional};
    ASSERT_THAT(tp.get_argument_help(),
                testing::ContainerEq(clapp::parser::types::help_entry_vec_t{
                    {arg_str, desc_str + " (" + purpose_optional_str + ")"}}));
}

TEST_F(argumentT,
       variadicDoubleArgumentConstructOptionalCallValidateFuncDoesNotThrow) {
    clapp::argument::double_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::optional};
    ASSERT_THAT(get_given_func(tp, arg_str)(), testing::Eq(false));
    ASSERT_NO_THROW(get_validate_value_func(tp, arg_str)(arg_str));
}

TEST_F(argumentT,
       variadicDoubleArgumentConstructMandatoryCallValidateFuncDoesNotThrow) {
    clapp::argument::double_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::mandatory};
    ASSERT_THAT(get_given_func(tp, arg_str)(), testing::Eq(false));
    ASSERT_NO_THROW(get_validate_value_func(tp, arg_str)(arg_str));
}

TEST_F(argumentT,
       variadicDoubleArgumentConstructStrAndCallValueReturnsEmptyVector) {
    clapp::argument::variadic_double_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THAT(tp, ContainsArgument(arg_str));
    ASSERT_THAT(arg, ArgumentNotGiven());
    ASSERT_THAT(arg.value(), testing::Eq(std::vector<std::double_t>{}));
}

TEST_F(argumentT,
       variadicDoubleArgumentConstructWithAlreadyRegisteredArgThrows) {
    clapp::argument::variadic_double_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THROW((clapp::argument::double_argument_t{tp, arg_str, desc_str}),
                 clapp::exception::argument_exception_t);
}

TEST_F(argumentT, variadicDoubleArgumentConstructWithEmpyArgumentNameThrows) {
    ASSERT_THROW(
        (clapp::argument::variadic_double_argument_t{tp, "", desc_cstr}),
        clapp::exception::argument_exception_t);
}

TEST_F(argumentT,
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

TEST_F(argumentT, variadicDoubleArgumentConstructStrWithoutCallArgFunc) {
    clapp::argument::variadic_double_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THAT(arg, VariadicArgumentNotGiven());
}

TEST_F(argumentT,
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

TEST_F(argumentT, variadicDoubleArgumentConstructCStrWithoutCallArgFunc) {
    clapp::argument::variadic_double_argument_t arg{tp, arg_cstr, desc_str};
    ASSERT_THAT(arg, VariadicArgumentNotGiven());
}

TEST_F(argumentT,
       variadicDoubleArgumentConstructMandatoryStrAndCallGetOptionHelp) {
    clapp::argument::variadic_double_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THAT(tp.get_argument_help(),
                testing::ContainerEq(clapp::parser::types::help_entry_vec_t{
                    {arg_str, desc_str + " (" + purpose_mandatory_str + ", " +
                                  variadic_argument_desc_restriction + ")"}}));
}

TEST_F(argumentT, variadicDoubleArgumentCallFoundFunc) {
    clapp::argument::variadic_double_argument_t arg{
        tp, arg_str, desc_str,
        clapp::value::found_func_t{[this](const std::string& argument) {
            found_func_arg = argument;
            found_func_called++;
            return clapp::value::found_func_t::ret_t{};
        }}};
    ASSERT_THAT(tp, ContainsArgument(arg_str));

    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)("1")));
    ASSERT_THAT(found_func_called, testing::Eq(1));
    ASSERT_THAT(found_func_arg.value(), testing::StrEq(arg_str));

    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)("11")));
    ASSERT_THAT(found_func_called, testing::Eq(2));
    ASSERT_THAT(found_func_arg.value(), testing::StrEq(arg_str));
}

TEST_F(argumentT, nsArgumentConstructCStrAndCallValueThrows) {
    clapp::argument::ns_argument_t arg{tp, arg_cstr, desc_str};
    ASSERT_THAT(tp, ContainsArgument(arg_cstr));
    ASSERT_THAT(arg, ArgumentNotGiven());
    ASSERT_THROW(static_cast<void>(arg.value()),
                 clapp::exception::value_undefined_t);
}

TEST_F(argumentT, nsArgumentConstructStrAndCallValueThrows) {
    clapp::argument::ns_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THAT(tp, ContainsArgument(arg_str));
    ASSERT_THAT(arg, ArgumentNotGiven());
    ASSERT_THROW(static_cast<void>(arg.value()),
                 clapp::exception::value_undefined_t);
}

TEST_F(argumentT, nsArgumentConstructWithAlreadyRegisteredArgThrows) {
    clapp::argument::ns_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THROW((clapp::argument::ns_argument_t{tp, arg_str, desc_str}),
                 clapp::exception::argument_exception_t);
}

TEST_F(argumentT, nsArgumentConstructOptionalCallValidateFuncDoesNotThrow) {
    clapp::argument::ns_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::optional};
    ASSERT_THAT(get_given_func(tp, arg_str)(), testing::Eq(false));
    ASSERT_NO_THROW(get_validate_value_func(tp, arg_str)(arg_str));
}

TEST_F(argumentT, nsArgumentConstructMandatoryCallValidateFuncDoesNotThrow) {
    clapp::argument::ns_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::mandatory};
    ASSERT_THAT(get_given_func(tp, arg_str)(), testing::Eq(false));
    ASSERT_NO_THROW(get_validate_value_func(tp, arg_str)(arg_str));
}

TEST_F(argumentT, nsArgumentCallFoundFunc) {
    clapp::argument::ns_argument_t arg{
        tp, arg_str, desc_str,
        clapp::value::found_func_t{[this](const std::string& argument) {
            found_func_arg = argument;
            found_func_called++;
            return clapp::value::found_func_t::ret_t{};
        }}};
    ASSERT_THAT(tp, ContainsArgument(arg_str));
    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)("1")));
    ASSERT_THAT(found_func_called, testing::Eq(1));
    ASSERT_THAT(found_func_arg.value(), testing::StrEq(arg_str));
}

TEST_F(argumentT, nsArgumentConstructWithEmpyArgumentNameThrows) {
    ASSERT_THROW((clapp::argument::ns_argument_t{tp, "", desc_cstr}),
                 clapp::exception::argument_exception_t);
}

TEST_F(argumentT, nsArgumentConstructStrAndCallArgFunc) {
    clapp::argument::ns_argument_t arg{tp, arg_str, desc_str};
    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)(std::to_string(value_ns.count()))));
    ASSERT_THAT(arg, ArgumentGiven(value_ns));
}

TEST_F(argumentT, nsArgumentConstructStrWithDefaultValueAndCallArgFunc) {
    clapp::argument::ns_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::optional,
        clapp::value::default_value_t<std::chrono::nanoseconds>{value_us}};
    ASSERT_THAT(arg, ArgumentNotGivenDefaultValue(value_us));
    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)(std::to_string(value_ns.count()))));
    ASSERT_THAT(arg, ArgumentGiven(value_ns));
}

TEST_F(argumentT, nsArgumentConstructMandatoryStrAndCallGetOptionHelp) {
    clapp::argument::ns_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THAT(tp.get_argument_help(),
                testing::ContainerEq(clapp::parser::types::help_entry_vec_t{
                    {arg_str, desc_str + " (" + purpose_mandatory_str + ")"}}));
}

TEST_F(argumentT, nsArgumentConstructOptionalStrAndCallGetOptionHelp) {
    clapp::argument::ns_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::optional};
    ASSERT_THAT(tp.get_argument_help(),
                testing::ContainerEq(clapp::parser::types::help_entry_vec_t{
                    {arg_str, desc_str + " (" + purpose_optional_str + ")"}}));
}

TEST_F(argumentT,
       variadicNsArgumentConstructOptionalCallValidateFuncDoesNotThrow) {
    clapp::argument::ns_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::optional};
    ASSERT_THAT(get_given_func(tp, arg_str)(), testing::Eq(false));
    ASSERT_NO_THROW(get_validate_value_func(tp, arg_str)(arg_str));
}

TEST_F(argumentT,
       variadicNsArgumentConstructMandatoryCallValidateFuncDoesNotThrow) {
    clapp::argument::ns_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::mandatory};
    ASSERT_THAT(get_given_func(tp, arg_str)(), testing::Eq(false));
    ASSERT_NO_THROW(get_validate_value_func(tp, arg_str)(arg_str));
}

TEST_F(argumentT,
       variadicNsArgumentConstructStrAndCallValueReturnsEmptyVector) {
    clapp::argument::variadic_ns_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THAT(tp, ContainsArgument(arg_str));
    ASSERT_THAT(arg, ArgumentNotGiven());
    ASSERT_THAT(arg.value(),
                testing::Eq(std::vector<std::chrono::nanoseconds>{}));
}

TEST_F(argumentT, variadicNsArgumentConstructWithAlreadyRegisteredArgThrows) {
    clapp::argument::variadic_ns_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THROW((clapp::argument::ns_argument_t{tp, arg_str, desc_str}),
                 clapp::exception::argument_exception_t);
}

TEST_F(argumentT, variadicNsArgumentConstructWithEmpyArgumentNameThrows) {
    ASSERT_THROW((clapp::argument::variadic_ns_argument_t{tp, "", desc_cstr}),
                 clapp::exception::argument_exception_t);
}

TEST_F(argumentT,
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

TEST_F(argumentT, variadicNsArgumentConstructStrWithoutCallArgFunc) {
    clapp::argument::variadic_ns_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THAT(arg, VariadicArgumentNotGiven());
}

TEST_F(argumentT, variadicNsArgumentConstructStrAndCallArgFuncMultipleTimes) {
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

TEST_F(argumentT, variadicNsArgumentConstructCStrWithoutCallArgFunc) {
    clapp::argument::variadic_ns_argument_t arg{tp, arg_cstr, desc_str};
    ASSERT_THAT(arg, VariadicArgumentNotGiven());
}

TEST_F(argumentT, variadicNsArgumentConstructMandatoryStrAndCallGetOptionHelp) {
    clapp::argument::variadic_ns_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THAT(tp.get_argument_help(),
                testing::ContainerEq(clapp::parser::types::help_entry_vec_t{
                    {arg_str, desc_str + " (" + purpose_mandatory_str + ", " +
                                  variadic_argument_desc_restriction + ")"}}));
}

TEST_F(argumentT, variadicNsArgumentCallFoundFunc) {
    clapp::argument::variadic_ns_argument_t arg{
        tp, arg_str, desc_str,
        clapp::value::found_func_t{[this](const std::string& argument) {
            found_func_arg = argument;
            found_func_called++;
            return clapp::value::found_func_t::ret_t{};
        }}};
    ASSERT_THAT(tp, ContainsArgument(arg_str));

    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)("1")));
    ASSERT_THAT(found_func_called, testing::Eq(1));
    ASSERT_THAT(found_func_arg.value(), testing::StrEq(arg_str));

    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)("11")));
    ASSERT_THAT(found_func_called, testing::Eq(2));
    ASSERT_THAT(found_func_arg.value(), testing::StrEq(arg_str));
}

TEST_F(argumentT, usArgumentConstructCStrAndCallValueThrows) {
    clapp::argument::us_argument_t arg{tp, arg_cstr, desc_str};
    ASSERT_THAT(tp, ContainsArgument(arg_cstr));
    ASSERT_THAT(arg, ArgumentNotGiven());
    ASSERT_THROW(static_cast<void>(arg.value()),
                 clapp::exception::value_undefined_t);
}

TEST_F(argumentT, usArgumentConstructStrAndCallValueThrows) {
    clapp::argument::us_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THAT(tp, ContainsArgument(arg_str));
    ASSERT_THAT(arg, ArgumentNotGiven());
    ASSERT_THROW(static_cast<void>(arg.value()),
                 clapp::exception::value_undefined_t);
}

TEST_F(argumentT, usArgumentConstructWithAlreadyRegisteredArgThrows) {
    clapp::argument::us_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THROW((clapp::argument::us_argument_t{tp, arg_str, desc_str}),
                 clapp::exception::argument_exception_t);
}

TEST_F(argumentT, usArgumentConstructOptionalCallValidateFuncDoesNotThrow) {
    clapp::argument::us_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::optional};
    ASSERT_THAT(get_given_func(tp, arg_str)(), testing::Eq(false));
    ASSERT_NO_THROW(get_validate_value_func(tp, arg_str)(arg_str));
}

TEST_F(argumentT, usArgumentConstructMandatoryCallValidateFuncDoesNotThrow) {
    clapp::argument::us_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::mandatory};
    ASSERT_THAT(get_given_func(tp, arg_str)(), testing::Eq(false));
    ASSERT_NO_THROW(get_validate_value_func(tp, arg_str)(arg_str));
}

TEST_F(argumentT, usArgumentCallFoundFunc) {
    clapp::argument::us_argument_t arg{
        tp, arg_str, desc_str,
        clapp::value::found_func_t{[this](const std::string& argument) {
            found_func_arg = argument;
            found_func_called++;
            return clapp::value::found_func_t::ret_t{};
        }}};
    ASSERT_THAT(tp, ContainsArgument(arg_str));
    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)("1")));
    ASSERT_THAT(found_func_called, testing::Eq(1));
    ASSERT_THAT(found_func_arg.value(), testing::StrEq(arg_str));
}

TEST_F(argumentT, usArgumentConstructWithEmpyArgumentNameThrows) {
    ASSERT_THROW((clapp::argument::us_argument_t{tp, "", desc_cstr}),
                 clapp::exception::argument_exception_t);
}

TEST_F(argumentT, usArgumentConstructStrAndCallArgFunc) {
    clapp::argument::us_argument_t arg{tp, arg_str, desc_str};
    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)(std::to_string(value_us.count()))));
    ASSERT_THAT(arg, ArgumentGiven(value_us));
}

TEST_F(argumentT, usArgumentConstructStrWithDefaultValueAndCallArgFunc) {
    clapp::argument::us_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::optional,
        clapp::value::default_value_t<std::chrono::microseconds>{value_ms}};
    ASSERT_THAT(arg, ArgumentNotGivenDefaultValue(value_ms));
    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)(std::to_string(value_us.count()))));
    ASSERT_THAT(arg, ArgumentGiven(value_us));
}

TEST_F(argumentT, usArgumentConstructMandatoryStrAndCallGetOptionHelp) {
    clapp::argument::us_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THAT(tp.get_argument_help(),
                testing::ContainerEq(clapp::parser::types::help_entry_vec_t{
                    {arg_str, desc_str + " (" + purpose_mandatory_str + ")"}}));
}

TEST_F(argumentT, usArgumentConstructOptionalStrAndCallGetOptionHelp) {
    clapp::argument::us_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::optional};
    ASSERT_THAT(tp.get_argument_help(),
                testing::ContainerEq(clapp::parser::types::help_entry_vec_t{
                    {arg_str, desc_str + " (" + purpose_optional_str + ")"}}));
}

TEST_F(argumentT,
       variadicUsArgumentConstructOptionalCallValidateFuncDoesNotThrow) {
    clapp::argument::us_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::optional};
    ASSERT_THAT(get_given_func(tp, arg_str)(), testing::Eq(false));
    ASSERT_NO_THROW(get_validate_value_func(tp, arg_str)(arg_str));
}

TEST_F(argumentT,
       variadicUsArgumentConstructMandatoryCallValidateFuncDoesNotThrow) {
    clapp::argument::us_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::mandatory};
    ASSERT_THAT(get_given_func(tp, arg_str)(), testing::Eq(false));
    ASSERT_NO_THROW(get_validate_value_func(tp, arg_str)(arg_str));
}

TEST_F(argumentT,
       variadicUsArgumentConstructStrAndCallValueReturnsEmptyVector) {
    clapp::argument::variadic_us_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THAT(tp, ContainsArgument(arg_str));
    ASSERT_THAT(arg, ArgumentNotGiven());
    ASSERT_THAT(arg.value(),
                testing::Eq(std::vector<std::chrono::microseconds>{}));
}

TEST_F(argumentT, variadicUsArgumentConstructWithAlreadyRegisteredArgThrows) {
    clapp::argument::variadic_us_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THROW((clapp::argument::us_argument_t{tp, arg_str, desc_str}),
                 clapp::exception::argument_exception_t);
}

TEST_F(argumentT, variadicUsArgumentConstructWithEmpyArgumentNameThrows) {
    ASSERT_THROW((clapp::argument::variadic_us_argument_t{tp, "", desc_cstr}),
                 clapp::exception::argument_exception_t);
}

TEST_F(argumentT,
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

TEST_F(argumentT, variadicUsArgumentConstructStrWithoutCallArgFunc) {
    clapp::argument::variadic_us_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THAT(arg, VariadicArgumentNotGiven());
}

TEST_F(argumentT, variadicUsArgumentConstructStrAndCallArgFuncMultipleTimes) {
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

TEST_F(argumentT, variadicUsArgumentConstructCStrWithoutCallArgFunc) {
    clapp::argument::variadic_us_argument_t arg{tp, arg_cstr, desc_str};
    ASSERT_THAT(arg, VariadicArgumentNotGiven());
}

TEST_F(argumentT, variadicUsArgumentConstructMandatoryStrAndCallGetOptionHelp) {
    clapp::argument::variadic_us_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THAT(tp.get_argument_help(),
                testing::ContainerEq(clapp::parser::types::help_entry_vec_t{
                    {arg_str, desc_str + " (" + purpose_mandatory_str + ", " +
                                  variadic_argument_desc_restriction + ")"}}));
}

TEST_F(argumentT, variadicUsArgumentCallFoundFunc) {
    clapp::argument::variadic_us_argument_t arg{
        tp, arg_str, desc_str,
        clapp::value::found_func_t{[this](const std::string& argument) {
            found_func_arg = argument;
            found_func_called++;
            return clapp::value::found_func_t::ret_t{};
        }}};
    ASSERT_THAT(tp, ContainsArgument(arg_str));

    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)("1")));
    ASSERT_THAT(found_func_called, testing::Eq(1));
    ASSERT_THAT(found_func_arg.value(), testing::StrEq(arg_str));

    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)("11")));
    ASSERT_THAT(found_func_called, testing::Eq(2));
    ASSERT_THAT(found_func_arg.value(), testing::StrEq(arg_str));
}

TEST_F(argumentT, msArgumentConstructCStrAndCallValueThrows) {
    clapp::argument::ms_argument_t arg{tp, arg_cstr, desc_str};
    ASSERT_THAT(tp, ContainsArgument(arg_cstr));
    ASSERT_THAT(arg, ArgumentNotGiven());
    ASSERT_THROW(static_cast<void>(arg.value()),
                 clapp::exception::value_undefined_t);
}

TEST_F(argumentT, msArgumentConstructStrAndCallValueThrows) {
    clapp::argument::ms_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THAT(tp, ContainsArgument(arg_str));
    ASSERT_THAT(arg, ArgumentNotGiven());
    ASSERT_THROW(static_cast<void>(arg.value()),
                 clapp::exception::value_undefined_t);
}

TEST_F(argumentT, msArgumentConstructWithAlreadyRegisteredArgThrows) {
    clapp::argument::ms_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THROW((clapp::argument::ms_argument_t{tp, arg_str, desc_str}),
                 clapp::exception::argument_exception_t);
}

TEST_F(argumentT, msArgumentConstructOptionalCallValidateFuncDoesNotThrow) {
    clapp::argument::ms_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::optional};
    ASSERT_THAT(get_given_func(tp, arg_str)(), testing::Eq(false));
    ASSERT_NO_THROW(get_validate_value_func(tp, arg_str)(arg_str));
}

TEST_F(argumentT, msArgumentConstructMandatoryCallValidateFuncDoesNotThrow) {
    clapp::argument::ms_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::mandatory};
    ASSERT_THAT(get_given_func(tp, arg_str)(), testing::Eq(false));
    ASSERT_NO_THROW(get_validate_value_func(tp, arg_str)(arg_str));
}

TEST_F(argumentT, msArgumentCallFoundFunc) {
    clapp::argument::ms_argument_t arg{
        tp, arg_str, desc_str,
        clapp::value::found_func_t{[this](const std::string& argument) {
            found_func_arg = argument;
            found_func_called++;
            return clapp::value::found_func_t::ret_t{};
        }}};
    ASSERT_THAT(tp, ContainsArgument(arg_str));
    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)("1")));
    ASSERT_THAT(found_func_called, testing::Eq(1));
    ASSERT_THAT(found_func_arg.value(), testing::StrEq(arg_str));
}

TEST_F(argumentT, msArgumentConstructWithEmpyArgumentNameThrows) {
    ASSERT_THROW((clapp::argument::ms_argument_t{tp, "", desc_cstr}),
                 clapp::exception::argument_exception_t);
}

TEST_F(argumentT, msArgumentConstructStrAndCallArgFunc) {
    clapp::argument::ms_argument_t arg{tp, arg_str, desc_str};
    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)(std::to_string(value_ms.count()))));
    ASSERT_THAT(arg, ArgumentGiven(value_ms));
}

TEST_F(argumentT, msArgumentConstructStrWithDefaultValueAndCallArgFunc) {
    clapp::argument::ms_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::optional,
        clapp::value::default_value_t<std::chrono::milliseconds>{value_sec}};
    ASSERT_THAT(arg, ArgumentNotGivenDefaultValue(
                         std::chrono::milliseconds{value_sec}));
    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)(std::to_string(value_ms.count()))));
    ASSERT_THAT(arg, ArgumentGiven(value_ms));
}

TEST_F(argumentT, msArgumentConstructMandatoryStrAndCallGetOptionHelp) {
    clapp::argument::ms_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THAT(tp.get_argument_help(),
                testing::ContainerEq(clapp::parser::types::help_entry_vec_t{
                    {arg_str, desc_str + " (" + purpose_mandatory_str + ")"}}));
}

TEST_F(argumentT, msArgumentConstructOptionalStrAndCallGetOptionHelp) {
    clapp::argument::ms_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::optional};
    ASSERT_THAT(tp.get_argument_help(),
                testing::ContainerEq(clapp::parser::types::help_entry_vec_t{
                    {arg_str, desc_str + " (" + purpose_optional_str + ")"}}));
}

TEST_F(argumentT,
       variadicMsArgumentConstructOptionalCallValidateFuncDoesNotThrow) {
    clapp::argument::ms_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::optional};
    ASSERT_THAT(get_given_func(tp, arg_str)(), testing::Eq(false));
    ASSERT_NO_THROW(get_validate_value_func(tp, arg_str)(arg_str));
}

TEST_F(argumentT,
       variadicMsArgumentConstructMandatoryCallValidateFuncDoesNotThrow) {
    clapp::argument::ms_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::mandatory};
    ASSERT_THAT(get_given_func(tp, arg_str)(), testing::Eq(false));
    ASSERT_NO_THROW(get_validate_value_func(tp, arg_str)(arg_str));
}

TEST_F(argumentT,
       variadicMsArgumentConstructStrAndCallValueReturnsEmptyVector) {
    clapp::argument::variadic_ms_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THAT(tp, ContainsArgument(arg_str));
    ASSERT_THAT(arg, ArgumentNotGiven());
    ASSERT_THAT(arg.value(),
                testing::Eq(std::vector<std::chrono::milliseconds>{}));
}

TEST_F(argumentT, variadicMsArgumentConstructWithAlreadyRegisteredArgThrows) {
    clapp::argument::variadic_ms_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THROW((clapp::argument::ms_argument_t{tp, arg_str, desc_str}),
                 clapp::exception::argument_exception_t);
}

TEST_F(argumentT, variadicMsArgumentConstructWithEmpyArgumentNameThrows) {
    ASSERT_THROW((clapp::argument::variadic_ms_argument_t{tp, "", desc_cstr}),
                 clapp::exception::argument_exception_t);
}

TEST_F(argumentT,
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

TEST_F(argumentT, variadicMsArgumentConstructStrWithoutCallArgFunc) {
    clapp::argument::variadic_ms_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THAT(arg, VariadicArgumentNotGiven());
}

TEST_F(argumentT, variadicMsArgumentConstructStrAndCallArgFuncMultipleTimes) {
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

TEST_F(argumentT, variadicMsArgumentConstructCStrWithoutCallArgFunc) {
    clapp::argument::variadic_ms_argument_t arg{tp, arg_cstr, desc_str};
    ASSERT_THAT(arg, VariadicArgumentNotGiven());
}

TEST_F(argumentT, variadicMsArgumentConstructMandatoryStrAndCallGetOptionHelp) {
    clapp::argument::variadic_ms_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THAT(tp.get_argument_help(),
                testing::ContainerEq(clapp::parser::types::help_entry_vec_t{
                    {arg_str, desc_str + " (" + purpose_mandatory_str + ", " +
                                  variadic_argument_desc_restriction + ")"}}));
}

TEST_F(argumentT, variadicMsArgumentCallFoundFunc) {
    clapp::argument::variadic_ms_argument_t arg{
        tp, arg_str, desc_str,
        clapp::value::found_func_t{[this](const std::string& argument) {
            found_func_arg = argument;
            found_func_called++;
            return clapp::value::found_func_t::ret_t{};
        }}};
    ASSERT_THAT(tp, ContainsArgument(arg_str));

    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)("1")));
    ASSERT_THAT(found_func_called, testing::Eq(1));
    ASSERT_THAT(found_func_arg.value(), testing::StrEq(arg_str));

    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)("11")));
    ASSERT_THAT(found_func_called, testing::Eq(2));
    ASSERT_THAT(found_func_arg.value(), testing::StrEq(arg_str));
}

TEST_F(argumentT, secArgumentConstructCStrAndCallValueThrows) {
    clapp::argument::sec_argument_t arg{tp, arg_cstr, desc_str};
    ASSERT_THAT(tp, ContainsArgument(arg_cstr));
    ASSERT_THAT(arg, ArgumentNotGiven());
    ASSERT_THROW(static_cast<void>(arg.value()),
                 clapp::exception::value_undefined_t);
}

TEST_F(argumentT, secArgumentConstructStrAndCallValueThrows) {
    clapp::argument::sec_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THAT(tp, ContainsArgument(arg_str));
    ASSERT_THAT(arg, ArgumentNotGiven());
    ASSERT_THROW(static_cast<void>(arg.value()),
                 clapp::exception::value_undefined_t);
}

TEST_F(argumentT, secArgumentConstructWithAlreadyRegisteredArgThrows) {
    clapp::argument::sec_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THROW((clapp::argument::sec_argument_t{tp, arg_str, desc_str}),
                 clapp::exception::argument_exception_t);
}

TEST_F(argumentT, secArgumentConstructOptionalCallValidateFuncDoesNotThrow) {
    clapp::argument::sec_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::optional};
    ASSERT_THAT(get_given_func(tp, arg_str)(), testing::Eq(false));
    ASSERT_NO_THROW(get_validate_value_func(tp, arg_str)(arg_str));
}

TEST_F(argumentT, secArgumentConstructMandatoryCallValidateFuncDoesNotThrow) {
    clapp::argument::sec_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::mandatory};
    ASSERT_THAT(get_given_func(tp, arg_str)(), testing::Eq(false));
    ASSERT_NO_THROW(get_validate_value_func(tp, arg_str)(arg_str));
}

TEST_F(argumentT, secArgumentCallFoundFunc) {
    clapp::argument::sec_argument_t arg{
        tp, arg_str, desc_str,
        clapp::value::found_func_t{[this](const std::string& argument) {
            found_func_arg = argument;
            found_func_called++;
            return clapp::value::found_func_t::ret_t{};
        }}};
    ASSERT_THAT(tp, ContainsArgument(arg_str));
    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)("1")));
    ASSERT_THAT(found_func_called, testing::Eq(1));
    ASSERT_THAT(found_func_arg.value(), testing::StrEq(arg_str));
}

TEST_F(argumentT, secArgumentConstructWithEmpyArgumentNameThrows) {
    ASSERT_THROW((clapp::argument::sec_argument_t{tp, "", desc_cstr}),
                 clapp::exception::argument_exception_t);
}

TEST_F(argumentT, secArgumentConstructStrAndCallArgFunc) {
    clapp::argument::sec_argument_t arg{tp, arg_str, desc_str};
    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)(std::to_string(value_sec.count()))));
    ASSERT_THAT(arg, ArgumentGiven(value_sec));
}

TEST_F(argumentT, secArgumentConstructStrWithDefaultValueAndCallArgFunc) {
    clapp::argument::sec_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::optional,
        clapp::value::default_value_t<std::chrono::seconds>{value_sec}};
    ASSERT_THAT(arg,
                ArgumentNotGivenDefaultValue(std::chrono::seconds{value_sec}));
    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)(std::to_string(value_sec.count()))));
    ASSERT_THAT(arg, ArgumentGiven(value_sec));
}

TEST_F(argumentT, secArgumentConstructMandatoryStrAndCallGetOptionHelp) {
    clapp::argument::sec_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THAT(tp.get_argument_help(),
                testing::ContainerEq(clapp::parser::types::help_entry_vec_t{
                    {arg_str, desc_str + " (" + purpose_mandatory_str + ")"}}));
}

TEST_F(argumentT, secArgumentConstructOptionalStrAndCallGetOptionHelp) {
    clapp::argument::sec_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::optional};
    ASSERT_THAT(tp.get_argument_help(),
                testing::ContainerEq(clapp::parser::types::help_entry_vec_t{
                    {arg_str, desc_str + " (" + purpose_optional_str + ")"}}));
}

TEST_F(argumentT,
       variadicSecArgumentConstructOptionalCallValidateFuncDoesNotThrow) {
    clapp::argument::sec_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::optional};
    ASSERT_THAT(get_given_func(tp, arg_str)(), testing::Eq(false));
    ASSERT_NO_THROW(get_validate_value_func(tp, arg_str)(arg_str));
}

TEST_F(argumentT,
       variadicSecArgumentConstructMandatoryCallValidateFuncDoesNotThrow) {
    clapp::argument::sec_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::mandatory};
    ASSERT_THAT(get_given_func(tp, arg_str)(), testing::Eq(false));
    ASSERT_NO_THROW(get_validate_value_func(tp, arg_str)(arg_str));
}

TEST_F(argumentT,
       variadicSecArgumentConstructStrAndCallValueReturnsEmptyVector) {
    clapp::argument::variadic_sec_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THAT(tp, ContainsArgument(arg_str));
    ASSERT_THAT(arg, ArgumentNotGiven());
    ASSERT_THAT(arg.value(), testing::Eq(std::vector<std::chrono::seconds>{}));
}

TEST_F(argumentT, variadicSecArgumentConstructWithAlreadyRegisteredArgThrows) {
    clapp::argument::variadic_sec_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THROW((clapp::argument::sec_argument_t{tp, arg_str, desc_str}),
                 clapp::exception::argument_exception_t);
}

TEST_F(argumentT, variadicSecArgumentConstructWithEmpyArgumentNameThrows) {
    ASSERT_THROW((clapp::argument::variadic_sec_argument_t{tp, "", desc_cstr}),
                 clapp::exception::argument_exception_t);
}

TEST_F(argumentT,
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

TEST_F(argumentT, variadicSecArgumentConstructStrWithoutCallArgFunc) {
    clapp::argument::variadic_sec_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THAT(arg, VariadicArgumentNotGiven());
}

TEST_F(argumentT, variadicSecArgumentConstructStrAndCallArgFuncMultipleTimes) {
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

TEST_F(argumentT, variadicSecArgumentConstructCStrWithoutCallArgFunc) {
    clapp::argument::variadic_sec_argument_t arg{tp, arg_cstr, desc_str};
    ASSERT_THAT(arg, VariadicArgumentNotGiven());
}

TEST_F(argumentT,
       variadicSecArgumentConstructMandatoryStrAndCallGetOptionHelp) {
    clapp::argument::variadic_sec_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THAT(tp.get_argument_help(),
                testing::ContainerEq(clapp::parser::types::help_entry_vec_t{
                    {arg_str, desc_str + " (" + purpose_mandatory_str + ", " +
                                  variadic_argument_desc_restriction + ")"}}));
}

TEST_F(argumentT, variadicSecArgumentCallFoundFunc) {
    clapp::argument::variadic_sec_argument_t arg{
        tp, arg_str, desc_str,
        clapp::value::found_func_t{[this](const std::string& argument) {
            found_func_arg = argument;
            found_func_called++;
            return clapp::value::found_func_t::ret_t{};
        }}};
    ASSERT_THAT(tp, ContainsArgument(arg_str));

    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)("1")));
    ASSERT_THAT(found_func_called, testing::Eq(1));
    ASSERT_THAT(found_func_arg.value(), testing::StrEq(arg_str));

    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)("11")));
    ASSERT_THAT(found_func_called, testing::Eq(2));
    ASSERT_THAT(found_func_arg.value(), testing::StrEq(arg_str));
}

TEST_F(argumentT, minArgumentConstructCStrAndCallValueThrows) {
    clapp::argument::min_argument_t arg{tp, arg_cstr, desc_str};
    ASSERT_THAT(tp, ContainsArgument(arg_cstr));
    ASSERT_THAT(arg, ArgumentNotGiven());
    ASSERT_THROW(static_cast<void>(arg.value()),
                 clapp::exception::value_undefined_t);
}

TEST_F(argumentT, minArgumentConstructStrAndCallValueThrows) {
    clapp::argument::min_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THAT(tp, ContainsArgument(arg_str));
    ASSERT_THAT(arg, ArgumentNotGiven());
    ASSERT_THROW(static_cast<void>(arg.value()),
                 clapp::exception::value_undefined_t);
}

TEST_F(argumentT, minArgumentConstructWithAlreadyRegisteredArgThrows) {
    clapp::argument::min_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THROW((clapp::argument::min_argument_t{tp, arg_str, desc_str}),
                 clapp::exception::argument_exception_t);
}

TEST_F(argumentT, minArgumentConstructOptionalCallValidateFuncDoesNotThrow) {
    clapp::argument::min_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::optional};
    ASSERT_THAT(get_given_func(tp, arg_str)(), testing::Eq(false));
    ASSERT_NO_THROW(get_validate_value_func(tp, arg_str)(arg_str));
}

TEST_F(argumentT, minArgumentConstructMandatoryCallValidateFuncDoesNotThrow) {
    clapp::argument::min_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::mandatory};
    ASSERT_THAT(get_given_func(tp, arg_str)(), testing::Eq(false));
    ASSERT_NO_THROW(get_validate_value_func(tp, arg_str)(arg_str));
}

TEST_F(argumentT, minArgumentCallFoundFunc) {
    clapp::argument::min_argument_t arg{
        tp, arg_str, desc_str,
        clapp::value::found_func_t{[this](const std::string& argument) {
            found_func_arg = argument;
            found_func_called++;
            return clapp::value::found_func_t::ret_t{};
        }}};
    ASSERT_THAT(tp, ContainsArgument(arg_str));
    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)("1")));
    ASSERT_THAT(found_func_called, testing::Eq(1));
    ASSERT_THAT(found_func_arg.value(), testing::StrEq(arg_str));
}

TEST_F(argumentT, minArgumentConstructWithEmpyArgumentNameThrows) {
    ASSERT_THROW((clapp::argument::min_argument_t{tp, "", desc_cstr}),
                 clapp::exception::argument_exception_t);
}

TEST_F(argumentT, minArgumentConstructStrAndCallArgFunc) {
    clapp::argument::min_argument_t arg{tp, arg_str, desc_str};
    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)(std::to_string(value_min.count()))));
    ASSERT_THAT(arg, ArgumentGiven(value_min));
}

TEST_F(argumentT, minArgumentConstructStrWithDefaultValueAndCallArgFunc) {
    clapp::argument::min_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::optional,
        clapp::value::default_value_t<std::chrono::minutes>{value_min}};
    ASSERT_THAT(arg,
                ArgumentNotGivenDefaultValue(std::chrono::minutes{value_min}));
    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)(std::to_string(value_min.count()))));
    ASSERT_THAT(arg, ArgumentGiven(value_min));
}

TEST_F(argumentT, minArgumentConstructMandatoryStrAndCallGetOptionHelp) {
    clapp::argument::min_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THAT(tp.get_argument_help(),
                testing::ContainerEq(clapp::parser::types::help_entry_vec_t{
                    {arg_str, desc_str + " (" + purpose_mandatory_str + ")"}}));
}

TEST_F(argumentT, minArgumentConstructOptionalStrAndCallGetOptionHelp) {
    clapp::argument::min_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::optional};
    ASSERT_THAT(tp.get_argument_help(),
                testing::ContainerEq(clapp::parser::types::help_entry_vec_t{
                    {arg_str, desc_str + " (" + purpose_optional_str + ")"}}));
}

TEST_F(argumentT,
       variadicMinArgumentConstructOptionalCallValidateFuncDoesNotThrow) {
    clapp::argument::min_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::optional};
    ASSERT_THAT(get_given_func(tp, arg_str)(), testing::Eq(false));
    ASSERT_NO_THROW(get_validate_value_func(tp, arg_str)(arg_str));
}

TEST_F(argumentT,
       variadicMinArgumentConstructMandatoryCallValidateFuncDoesNotThrow) {
    clapp::argument::min_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::mandatory};
    ASSERT_THAT(get_given_func(tp, arg_str)(), testing::Eq(false));
    ASSERT_NO_THROW(get_validate_value_func(tp, arg_str)(arg_str));
}

TEST_F(argumentT,
       variadicMinArgumentConstructStrAndCallValueReturnsEmptyVector) {
    clapp::argument::variadic_min_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THAT(tp, ContainsArgument(arg_str));
    ASSERT_THAT(arg, ArgumentNotGiven());
    ASSERT_THAT(arg.value(), testing::Eq(std::vector<std::chrono::minutes>{}));
}

TEST_F(argumentT, variadicMinArgumentConstructWithAlreadyRegisteredArgThrows) {
    clapp::argument::variadic_min_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THROW((clapp::argument::min_argument_t{tp, arg_str, desc_str}),
                 clapp::exception::argument_exception_t);
}

TEST_F(argumentT, variadicMinArgumentConstructWithEmpyArgumentNameThrows) {
    ASSERT_THROW((clapp::argument::variadic_min_argument_t{tp, "", desc_cstr}),
                 clapp::exception::argument_exception_t);
}

TEST_F(argumentT,
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

TEST_F(argumentT, variadicMinArgumentConstructStrWithoutCallArgFunc) {
    clapp::argument::variadic_min_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THAT(arg, VariadicArgumentNotGiven());
}

TEST_F(argumentT, variadicMinArgumentConstructStrAndCallArgFuncMultipleTimes) {
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

TEST_F(argumentT, variadicMinArgumentConstructCStrWithoutCallArgFunc) {
    clapp::argument::variadic_min_argument_t arg{tp, arg_cstr, desc_str};
    ASSERT_THAT(arg, VariadicArgumentNotGiven());
}

TEST_F(argumentT,
       variadicMinArgumentConstructMandatoryStrAndCallGetOptionHelp) {
    clapp::argument::variadic_min_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THAT(tp.get_argument_help(),
                testing::ContainerEq(clapp::parser::types::help_entry_vec_t{
                    {arg_str, desc_str + " (" + purpose_mandatory_str + ", " +
                                  variadic_argument_desc_restriction + ")"}}));
}

TEST_F(argumentT, variadicMinArgumentCallFoundFunc) {
    clapp::argument::variadic_min_argument_t arg{
        tp, arg_str, desc_str,
        clapp::value::found_func_t{[this](const std::string& argument) {
            found_func_arg = argument;
            found_func_called++;
            return clapp::value::found_func_t::ret_t{};
        }}};
    ASSERT_THAT(tp, ContainsArgument(arg_str));

    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)("1")));
    ASSERT_THAT(found_func_called, testing::Eq(1));
    ASSERT_THAT(found_func_arg.value(), testing::StrEq(arg_str));

    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)("11")));
    ASSERT_THAT(found_func_called, testing::Eq(2));
    ASSERT_THAT(found_func_arg.value(), testing::StrEq(arg_str));
}

TEST_F(argumentT, hoursArgumentConstructCStrAndCallValueThrows) {
    clapp::argument::hours_argument_t arg{tp, arg_cstr, desc_str};
    ASSERT_THAT(tp, ContainsArgument(arg_cstr));
    ASSERT_THAT(arg, ArgumentNotGiven());
    ASSERT_THROW(static_cast<void>(arg.value()),
                 clapp::exception::value_undefined_t);
}

TEST_F(argumentT, hoursArgumentConstructStrAndCallValueThrows) {
    clapp::argument::hours_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THAT(tp, ContainsArgument(arg_str));
    ASSERT_THAT(arg, ArgumentNotGiven());
    ASSERT_THROW(static_cast<void>(arg.value()),
                 clapp::exception::value_undefined_t);
}

TEST_F(argumentT, hoursArgumentConstructWithAlreadyRegisteredArgThrows) {
    clapp::argument::hours_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THROW((clapp::argument::hours_argument_t{tp, arg_str, desc_str}),
                 clapp::exception::argument_exception_t);
}

TEST_F(argumentT, hoursArgumentConstructOptionalCallValidateFuncDoesNotThrow) {
    clapp::argument::hours_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::optional};
    ASSERT_THAT(get_given_func(tp, arg_str)(), testing::Eq(false));
    ASSERT_NO_THROW(get_validate_value_func(tp, arg_str)(arg_str));
}

TEST_F(argumentT, hoursArgumentConstructMandatoryCallValidateFuncDoesNotThrow) {
    clapp::argument::hours_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::mandatory};
    ASSERT_THAT(get_given_func(tp, arg_str)(), testing::Eq(false));
    ASSERT_NO_THROW(get_validate_value_func(tp, arg_str)(arg_str));
}

TEST_F(argumentT, hoursArgumentCallFoundFunc) {
    clapp::argument::hours_argument_t arg{
        tp, arg_str, desc_str,
        clapp::value::found_func_t{[this](const std::string& argument) {
            found_func_arg = argument;
            found_func_called++;
            return clapp::value::found_func_t::ret_t{};
        }}};
    ASSERT_THAT(tp, ContainsArgument(arg_str));
    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)("1")));
    ASSERT_THAT(found_func_called, testing::Eq(1));
    ASSERT_THAT(found_func_arg.value(), testing::StrEq(arg_str));
}

TEST_F(argumentT, hoursArgumentConstructWithEmpyArgumentNameThrows) {
    ASSERT_THROW((clapp::argument::hours_argument_t{tp, "", desc_cstr}),
                 clapp::exception::argument_exception_t);
}

TEST_F(argumentT, hoursArgumentConstructStrAndCallArgFunc) {
    clapp::argument::hours_argument_t arg{tp, arg_str, desc_str};
    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)(std::to_string(value_hours.count()))));
    ASSERT_THAT(arg, ArgumentGiven(value_hours));
}

TEST_F(argumentT, hoursArgumentConstructStrWithDefaultValueAndCallArgFunc) {
    clapp::argument::hours_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::optional,
        clapp::value::default_value_t<std::chrono::hours>{value_hours}};
    ASSERT_THAT(arg,
                ArgumentNotGivenDefaultValue(std::chrono::hours{value_hours}));
    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)(std::to_string(value_hours.count()))));
    ASSERT_THAT(arg, ArgumentGiven(value_hours));
}

TEST_F(argumentT, hoursArgumentConstructMandatoryStrAndCallGetOptionHelp) {
    clapp::argument::hours_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THAT(tp.get_argument_help(),
                testing::ContainerEq(clapp::parser::types::help_entry_vec_t{
                    {arg_str, desc_str + " (" + purpose_mandatory_str + ")"}}));
}

TEST_F(argumentT, hoursArgumentConstructOptionalStrAndCallGetOptionHelp) {
    clapp::argument::hours_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::optional};
    ASSERT_THAT(tp.get_argument_help(),
                testing::ContainerEq(clapp::parser::types::help_entry_vec_t{
                    {arg_str, desc_str + " (" + purpose_optional_str + ")"}}));
}

TEST_F(argumentT,
       variadicHoursArgumentConstructOptionalCallValidateFuncDoesNotThrow) {
    clapp::argument::hours_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::optional};
    ASSERT_THAT(get_given_func(tp, arg_str)(), testing::Eq(false));
    ASSERT_NO_THROW(get_validate_value_func(tp, arg_str)(arg_str));
}

TEST_F(argumentT,
       variadicHoursArgumentConstructMandatoryCallValidateFuncDoesNotThrow) {
    clapp::argument::hours_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::mandatory};
    ASSERT_THAT(get_given_func(tp, arg_str)(), testing::Eq(false));
    ASSERT_NO_THROW(get_validate_value_func(tp, arg_str)(arg_str));
}

TEST_F(argumentT,
       variadicHoursArgumentConstructStrAndCallValueReturnsEmptyVector) {
    clapp::argument::variadic_hours_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THAT(tp, ContainsArgument(arg_str));
    ASSERT_THAT(arg, ArgumentNotGiven());
    ASSERT_THAT(arg.value(), testing::Eq(std::vector<std::chrono::hours>{}));
}

TEST_F(argumentT,
       variadicHoursArgumentConstructWithAlreadyRegisteredArgThrows) {
    clapp::argument::variadic_hours_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THROW((clapp::argument::hours_argument_t{tp, arg_str, desc_str}),
                 clapp::exception::argument_exception_t);
}

TEST_F(argumentT, variadicHoursArgumentConstructWithEmpyArgumentNameThrows) {
    ASSERT_THROW(
        (clapp::argument::variadic_hours_argument_t{tp, "", desc_cstr}),
        clapp::exception::argument_exception_t);
}

TEST_F(argumentT,
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

TEST_F(argumentT, variadicHoursArgumentConstructStrWithoutCallArgFunc) {
    clapp::argument::variadic_hours_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THAT(arg, VariadicArgumentNotGiven());
}

TEST_F(argumentT,
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

TEST_F(argumentT, variadicHoursArgumentConstructCStrWithoutCallArgFunc) {
    clapp::argument::variadic_hours_argument_t arg{tp, arg_cstr, desc_str};
    ASSERT_THAT(arg, VariadicArgumentNotGiven());
}

TEST_F(argumentT,
       variadicHoursArgumentConstructMandatoryStrAndCallGetOptionHelp) {
    clapp::argument::variadic_hours_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THAT(tp.get_argument_help(),
                testing::ContainerEq(clapp::parser::types::help_entry_vec_t{
                    {arg_str, desc_str + " (" + purpose_mandatory_str + ", " +
                                  variadic_argument_desc_restriction + ")"}}));
}

TEST_F(argumentT, variadicHoursArgumentCallFoundFunc) {
    clapp::argument::variadic_hours_argument_t arg{
        tp, arg_str, desc_str,
        clapp::value::found_func_t{[this](const std::string& argument) {
            found_func_arg = argument;
            found_func_called++;
            return clapp::value::found_func_t::ret_t{};
        }}};
    ASSERT_THAT(tp, ContainsArgument(arg_str));

    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)("1")));
    ASSERT_THAT(found_func_called, testing::Eq(1));
    ASSERT_THAT(found_func_arg.value(), testing::StrEq(arg_str));

    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)("11")));
    ASSERT_THAT(found_func_called, testing::Eq(2));
    ASSERT_THAT(found_func_arg.value(), testing::StrEq(arg_str));
}

#ifdef CLAPP_FS_AVAIL

TEST_F(argumentT, pathArgumentConstructCStrAndCallValueThrows) {
    clapp::argument::path_argument_t arg{tp, arg_cstr, desc_str};
    ASSERT_THAT(tp, ContainsArgument(arg_cstr));
    ASSERT_THAT(arg, ArgumentNotGiven());
    ASSERT_THROW(static_cast<void>(arg.value()),
                 clapp::exception::value_undefined_t);
}

TEST_F(argumentT, pathArgumentConstructStrAndCallValueThrows) {
    clapp::argument::path_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THAT(tp, ContainsArgument(arg_str));
    ASSERT_THAT(arg, ArgumentNotGiven());
    ASSERT_THROW(static_cast<void>(arg.value()),
                 clapp::exception::value_undefined_t);
}

TEST_F(argumentT, pathArgumentConstructWithAlreadyRegisteredArgThrows) {
    clapp::argument::path_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THROW((clapp::argument::path_argument_t{tp, arg_str, desc_str}),
                 clapp::exception::argument_exception_t);
}

TEST_F(argumentT, pathArgumentConstructOptionalCallValidateFuncDoesNotThrow) {
    clapp::argument::path_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::optional};
    ASSERT_THAT(get_given_func(tp, arg_str)(), testing::Eq(false));
    ASSERT_NO_THROW(get_validate_value_func(tp, arg_str)(arg_str));
}

TEST_F(argumentT, pathArgumentConstructMandatoryCallValidateFuncDoesNotThrow) {
    clapp::argument::path_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::mandatory};
    ASSERT_THAT(get_given_func(tp, arg_str)(), testing::Eq(false));
    ASSERT_NO_THROW(get_validate_value_func(tp, arg_str)(arg_str));
}

TEST_F(argumentT, pathArgumentCallFoundFunc) {
    clapp::argument::path_argument_t arg{
        tp, arg_str, desc_str,
        clapp::value::found_func_t{[this](const std::string& argument) {
            found_func_arg = argument;
            found_func_called++;
            return clapp::value::found_func_t::ret_t{};
        }}};
    ASSERT_THAT(tp, ContainsArgument(arg_str));
    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)(value_path.string())));
    ASSERT_THAT(found_func_called, testing::Eq(1));
    ASSERT_THAT(found_func_arg.value(), testing::StrEq(arg_str));
}

TEST_F(argumentT, pathArgumentConstructWithEmpyArgumentNameThrows) {
    ASSERT_THROW((clapp::argument::path_argument_t{tp, "", desc_cstr}),
                 clapp::exception::argument_exception_t);
}

TEST_F(argumentT, pathArgumentConstructStrAndCallArgFunc) {
    clapp::argument::path_argument_t arg{tp, arg_str, desc_str};
    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)(value_path_cstring)));
    ASSERT_THAT(arg, ArgumentGiven(clapp::fs::path{value_path_cstring}));
}

TEST_F(argumentT, pathArgumentConstructStrWithDefaultValueAndCallArgFunc) {
    clapp::argument::path_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::optional,
        clapp::value::default_value_t<clapp::fs::path>{value_path_additional}};
    ASSERT_THAT(arg, ArgumentNotGivenDefaultValue(
                         clapp::fs::path{value_path_additional}));
    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)(value_path.string())));
    ASSERT_THAT(arg, ArgumentGiven(value_path));
}

TEST_F(argumentT, pathArgumentConstructMandatoryStrAndCallGetOptionHelp) {
    clapp::argument::path_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THAT(tp.get_argument_help(),
                testing::ContainerEq(clapp::parser::types::help_entry_vec_t{
                    {arg_str, desc_str + " (" + purpose_mandatory_str + ")"}}));
}

TEST_F(argumentT, pathArgumentConstructOptionalStrAndCallGetOptionHelp) {
    clapp::argument::path_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::optional};
    ASSERT_THAT(tp.get_argument_help(),
                testing::ContainerEq(clapp::parser::types::help_entry_vec_t{
                    {arg_str, desc_str + " (" + purpose_optional_str + ")"}}));
}

TEST_F(argumentT,
       variadicPathArgumentConstructOptionalCallValidateFuncDoesNotThrow) {
    clapp::argument::path_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::optional};
    ASSERT_THAT(get_given_func(tp, arg_str)(), testing::Eq(false));
    ASSERT_NO_THROW(get_validate_value_func(tp, arg_str)(arg_str));
}

TEST_F(argumentT,
       variadicPathArgumentConstructMandatoryCallValidateFuncDoesNotThrow) {
    clapp::argument::path_argument_t arg{
        tp, arg_str, desc_str, argument_test_parser_t::purpose_t::mandatory};
    ASSERT_THAT(get_given_func(tp, arg_str)(), testing::Eq(false));
    ASSERT_NO_THROW(get_validate_value_func(tp, arg_str)(arg_str));
}

TEST_F(argumentT,
       variadicPathArgumentConstructStrAndCallValueReturnsEmptyVector) {
    clapp::argument::variadic_path_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THAT(tp, ContainsArgument(arg_str));
    ASSERT_THAT(arg, ArgumentNotGiven());
    ASSERT_THAT(arg.value(), testing::Eq(std::vector<clapp::fs::path>{}));
}

TEST_F(argumentT, variadicPathArgumentConstructWithAlreadyRegisteredArgThrows) {
    clapp::argument::variadic_path_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THROW((clapp::argument::path_argument_t{tp, arg_str, desc_str}),
                 clapp::exception::argument_exception_t);
}

TEST_F(argumentT, variadicPathArgumentConstructWithEmpyArgumentNameThrows) {
    ASSERT_THROW((clapp::argument::variadic_path_argument_t{tp, "", desc_cstr}),
                 clapp::exception::argument_exception_t);
}

TEST_F(argumentT,
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

TEST_F(argumentT, variadicPathArgumentConstructStrWithoutCallArgFunc) {
    clapp::argument::variadic_path_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THAT(arg, VariadicArgumentNotGiven());
}

TEST_F(argumentT, variadicPathArgumentConstructStrAndCallArgFuncMultipleTimes) {
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

TEST_F(argumentT, variadicPathArgumentConstructCStrWithoutCallArgFunc) {
    clapp::argument::variadic_path_argument_t arg{tp, arg_cstr, desc_str};
    ASSERT_THAT(arg, VariadicArgumentNotGiven());
}

TEST_F(argumentT,
       variadicPathArgumentConstructMandatoryStrAndCallGetOptionHelp) {
    clapp::argument::variadic_path_argument_t arg{tp, arg_str, desc_str};
    ASSERT_THAT(tp.get_argument_help(),
                testing::ContainerEq(clapp::parser::types::help_entry_vec_t{
                    {arg_str, desc_str + " (" + purpose_mandatory_str + ", " +
                                  variadic_argument_desc_restriction + ")"}}));
}

TEST_F(argumentT, variadicPathArgumentCallFoundFunc) {
    clapp::argument::variadic_path_argument_t arg{
        tp, arg_str, desc_str,
        clapp::value::found_func_t{[this](const std::string& argument) {
            found_func_arg = argument;
            found_func_called++;
            return clapp::value::found_func_t::ret_t{};
        }}};

    ASSERT_THAT(tp, ContainsArgument(arg_str));
    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)(value_path.string())));
    ASSERT_THAT(found_func_called, testing::Eq(1));
    ASSERT_THAT(found_func_arg.value(), testing::StrEq(arg_str));

    ASSERT_NO_THROW((get_arg_func<argument_test_parser_t::argument_func_t>(
        tp, arg_str)(value_path_additional.string())));
    ASSERT_THAT(found_func_called, testing::Eq(2));
    ASSERT_THAT(found_func_arg.value(), testing::StrEq(arg_str));
}

#endif

TEST_F(argumentT,
       genOptValidateFuncWithoutHasValueFuncAndCallValidateFuncDoesntThrow) {
    constexpr std::int32_t return_value{10};
    argument_test_parser_t::validate_value_func_t validate_func{
        clapp::argument::gen_arg_validate_value_func<std::int32_t,
                                                     int32_value_func_t>(
            []() { return return_value; }, []() { return true; },
            std::vector<int32_validate_func_t>{
                [](const std::int32_t& value,
                   const std::string& argument_string) {
                    if (argument_string != "argument string") {
                        throw std::runtime_error{"argument_string invalid"};
                    }
                    if (value != return_value) {
                        throw std::runtime_error{"value invalid"};
                    }
                }})};
    ASSERT_NO_THROW(validate_func("argument string"));
    ASSERT_THROW(validate_func("argument"), std::runtime_error);
}
