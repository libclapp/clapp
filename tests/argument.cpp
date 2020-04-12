#include <clapp/argument.h>
#include <clapp/sub_parser.h>
#include <gmock/gmock.h>

class argument_test_parser_t : public clapp::parser::basic_parser_t {
   public:
    ~argument_test_parser_t() override;

    using clapp::parser::basic_parser_t::purpose_t;

    using clapp::parser::basic_parser_t::argument_descriptions_vec_t;
    using clapp::parser::basic_parser_t::argument_type_t;
    using clapp::parser::basic_parser_t::arguments_vector_t;
    using clapp::parser::basic_parser_t::validate_func_vec_t;

    using clapp::parser::basic_parser_t::get_arguments;
    using clapp::parser::basic_parser_t::get_mandatory_argument_descriptions;
    using clapp::parser::basic_parser_t::get_optional_argument_descriptions;
    using clapp::parser::basic_parser_t::get_validate_functions;

    [[nodiscard]] std::string gen_short_line_prefix() const override;
    void set_max_option_string_size(std::size_t max_option_size) override;
    [[nodiscard]] std::size_t get_max_option_string_size() const override;

   private:
    std::size_t max_option_string_size{0};
};

argument_test_parser_t::~argument_test_parser_t() = default;

std::string argument_test_parser_t::gen_short_line_prefix() const {
    return "arg-test-parser" + gen_short_line();
}

void argument_test_parser_t::set_max_option_string_size(
    const std::size_t max_option_size) {
    max_option_string_size = max_option_size;
}

std::size_t argument_test_parser_t::get_max_option_string_size() const {
    return max_option_string_size;
}

class simple_argument_sub_parser_t : public clapp::parser::basic_sub_parser_t {
    using clapp::parser::basic_sub_parser_t::basic_sub_parser_t;

   public:
    ~simple_argument_sub_parser_t() override;
};

simple_argument_sub_parser_t::~simple_argument_sub_parser_t() = default;

TEST(argument, basicArgumentConstructToShortArgumentNameThrows) {
    const std::string arg_name;
    const std::string arg_desc{"description"};
    argument_test_parser_t tp;
    ASSERT_THROW((clapp::argument::string_argument_t{tp, arg_name, arg_desc}),
                 clapp::exception::argument_exception_t);
}

TEST(argument,
     basicArgumentConstructVariadicArgumentAndRegisterAnotherArgumentThrows) {
    const std::string variadic_arg_name{"variadic"};
    const std::string arg_name{"arg"};
    const std::string arg_desc{"description"};
    argument_test_parser_t tp;
    clapp::argument::basic_variadic_argument_t<std::int32_t> arg{
        tp, variadic_arg_name, arg_desc};
    ASSERT_THROW((clapp::argument::string_argument_t{tp, arg_name, arg_desc}),
                 clapp::exception::argument_exception_t);
}

TEST(argument,
     basicArgumentConstructOptionalArgumentAndRegisterMandatoryArgumentThrows) {
    const std::string optional_arg_name{"variadic"};
    const std::string arg_name{"arg"};
    const std::string arg_desc{"description"};
    argument_test_parser_t tp;
    clapp::argument::basic_argument_t<std::int32_t> arg{
        tp, optional_arg_name, arg_desc,
        argument_test_parser_t::purpose_t::optional};
    ASSERT_THROW((clapp::argument::string_argument_t{tp, arg_name, arg_desc}),
                 clapp::exception::argument_exception_t);
}

TEST(argument, basicArgumentConstructSubParserAndRegisterArgumentThrows) {
    const std::string arg_name{"arg"};
    const std::string arg_desc{"description"};
    argument_test_parser_t tp;
    simple_argument_sub_parser_t sub_parser{tp, "sub-parser",
                                            "Sub parser desc"};
    ASSERT_THROW((clapp::argument::string_argument_t{
                     tp, arg_name, arg_desc,
                     argument_test_parser_t::purpose_t::optional}),
                 clapp::exception::argument_exception_t);
}

TEST(argument, basicArgumentFoundFuncLong) {
    const std::string arg_name{"arg"};
    const std::string arg_desc{"description"};

    std::stringstream ss;
    argument_test_parser_t tp;
    clapp::argument::basic_argument_t<std::string> arg{
        tp, arg_name, arg_desc,
        clapp::value::found_func_t{[&ss]() { ss << "this is a test"; }}};
    argument_test_parser_t::arguments_vector_t args{tp.get_arguments()};
    argument_test_parser_t::argument_descriptions_vec_t descs{
        tp.get_mandatory_argument_descriptions()};

    ASSERT_THAT(args.size(), testing::Eq(1));
    ASSERT_THAT(args[0].argument, testing::StrEq(arg_name));
    ASSERT_THAT(args[0].argument_type,
                testing::Eq(argument_test_parser_t::argument_type_t::single));

    args[0].func("argument-value");
    ASSERT_THAT(ss.str(), testing::StrEq("this is a test"));
}

TEST(argument, basicArgumentConstructSimple) {
    const std::string arg_name{"argument name"};
    const std::string arg_desc{"description"};
    argument_test_parser_t tp;
    clapp::argument::basic_argument_t<std::int32_t> arg{tp, arg_name, arg_desc};
    argument_test_parser_t::arguments_vector_t args{tp.get_arguments()};
    argument_test_parser_t::argument_descriptions_vec_t descs{
        tp.get_mandatory_argument_descriptions()};
    argument_test_parser_t::validate_func_vec_t validate_funcs{
        tp.get_validate_functions()};
    ASSERT_THAT(validate_funcs.size(), testing::Eq(1));

    ASSERT_THAT(descs.size(), testing::Eq(1));
    ASSERT_THAT(descs[0].argument_string, testing::StrEq(arg_name));
    ASSERT_THAT(descs[0].description, testing::StrEq(arg_desc));
    ASSERT_THAT(descs[0].argument_type,
                testing::Eq(argument_test_parser_t::argument_type_t::single));

    ASSERT_THAT(args.size(), testing::Eq(1));
    ASSERT_THAT(args[0].argument, testing::StrEq(arg_name));
    ASSERT_THAT(args[0].argument_type,
                testing::Eq(argument_test_parser_t::argument_type_t::single));

    ASSERT_THAT(static_cast<bool>(arg), testing::Eq(false));
    ASSERT_THAT(arg.given(), testing::Eq(false));
    ASSERT_THROW(arg.value(), clapp::exception::value_undefined_t);
    ASSERT_THROW(validate_funcs[0](), clapp::exception::argument_exception_t);

    args[0].func("12345");
    ASSERT_THAT(static_cast<bool>(arg), testing::Eq(true));
    ASSERT_THAT(arg.given(), testing::Eq(true));
    ASSERT_THAT(arg.value(), testing::Eq(12345));
    ASSERT_NO_THROW(validate_funcs[0]());
}

TEST(argument, basicArgumentConstructMandatory) {
    const std::string arg_name{"argument name"};
    const std::string arg_desc{"description"};
    argument_test_parser_t tp;
    clapp::argument::basic_argument_t<std::string> arg{
        tp, arg_name, arg_desc, argument_test_parser_t::purpose_t::mandatory};
    argument_test_parser_t::arguments_vector_t args{tp.get_arguments()};
    argument_test_parser_t::argument_descriptions_vec_t descs{
        tp.get_mandatory_argument_descriptions()};
    argument_test_parser_t::validate_func_vec_t validate_funcs{
        tp.get_validate_functions()};
    ASSERT_THAT(validate_funcs.size(), testing::Eq(1));

    ASSERT_THAT(descs.size(), testing::Eq(1));
    ASSERT_THAT(descs[0].argument_string, testing::StrEq(arg_name));
    ASSERT_THAT(descs[0].description, testing::StrEq(arg_desc));
    ASSERT_THAT(descs[0].argument_type,
                testing::Eq(argument_test_parser_t::argument_type_t::single));

    ASSERT_THAT(args.size(), testing::Eq(1));
    ASSERT_THAT(args[0].argument, testing::StrEq(arg_name));
    ASSERT_THAT(args[0].argument_type,
                testing::Eq(argument_test_parser_t::argument_type_t::single));

    ASSERT_THAT(static_cast<bool>(arg), testing::Eq(false));
    ASSERT_THAT(arg.given(), testing::Eq(false));
    ASSERT_THROW(arg.value(), clapp::exception::value_undefined_t);
    ASSERT_THROW(validate_funcs[0](), clapp::exception::argument_exception_t);

    args[0].func("string-arg");
    ASSERT_THAT(static_cast<bool>(arg), testing::Eq(true));
    ASSERT_THAT(arg.given(), testing::Eq(true));
    ASSERT_THAT(arg.value(), testing::StrEq("string-arg"));
    ASSERT_NO_THROW(validate_funcs[0]());
}

TEST(argument, basicArgumentConstructOptional) {
    const std::string arg_name{"argument name"};
    const std::string arg_desc{"description"};
    argument_test_parser_t tp;
    clapp::argument::basic_argument_t<std::int32_t> arg{
        tp, arg_name, arg_desc, argument_test_parser_t::purpose_t::optional};
    argument_test_parser_t::arguments_vector_t args{tp.get_arguments()};
    argument_test_parser_t::argument_descriptions_vec_t descs{
        tp.get_optional_argument_descriptions()};
    argument_test_parser_t::validate_func_vec_t validate_funcs{
        tp.get_validate_functions()};
    ASSERT_THAT(validate_funcs.size(), testing::Eq(0));

    ASSERT_THAT(descs.size(), testing::Eq(1));
    ASSERT_THAT(descs[0].argument_string, testing::StrEq(arg_name));
    ASSERT_THAT(descs[0].description, testing::StrEq(arg_desc));
    ASSERT_THAT(descs[0].argument_type,
                testing::Eq(argument_test_parser_t::argument_type_t::single));

    ASSERT_THAT(args.size(), testing::Eq(1));
    ASSERT_THAT(args[0].argument, testing::StrEq(arg_name));
    ASSERT_THAT(args[0].argument_type,
                testing::Eq(argument_test_parser_t::argument_type_t::single));

    ASSERT_THAT(static_cast<bool>(arg), testing::Eq(false));
    ASSERT_THAT(arg.given(), testing::Eq(false));
    ASSERT_THROW(arg.value(), clapp::exception::value_undefined_t);

    args[0].func("12345");
    ASSERT_THAT(static_cast<bool>(arg), testing::Eq(true));
    ASSERT_THAT(arg.given(), testing::Eq(true));
    ASSERT_THAT(arg.value(), testing::Eq(12345));
}

TEST(argument, basicArgumentConstructOptionalDefaultValue) {
    const std::string arg_name{"argument name"};
    const std::string arg_desc{"description"};
    argument_test_parser_t tp;
    const clapp::value::default_value_t<std::int8_t> default_value{10};
    clapp::argument::basic_argument_t<std::int8_t> arg{
        tp, arg_name, arg_desc, argument_test_parser_t::purpose_t::optional,
        default_value};
    argument_test_parser_t::arguments_vector_t args{tp.get_arguments()};
    argument_test_parser_t::argument_descriptions_vec_t descs{
        tp.get_optional_argument_descriptions()};
    argument_test_parser_t::validate_func_vec_t validate_funcs{
        tp.get_validate_functions()};
    ASSERT_THAT(validate_funcs.size(), testing::Eq(0));

    ASSERT_THAT(descs.size(), testing::Eq(1));
    ASSERT_THAT(descs[0].argument_string, testing::StrEq(arg_name));
    ASSERT_THAT(descs[0].description,
                testing::StrEq(arg_desc + " (" +
                               default_value.append_restriction_text() + ")"));
    ASSERT_THAT(descs[0].argument_type,
                testing::Eq(argument_test_parser_t::argument_type_t::single));

    ASSERT_THAT(args.size(), testing::Eq(1));
    ASSERT_THAT(args[0].argument, testing::StrEq(arg_name));
    ASSERT_THAT(args[0].argument_type,
                testing::Eq(argument_test_parser_t::argument_type_t::single));

    ASSERT_THAT(static_cast<bool>(arg), testing::Eq(true));
    ASSERT_THAT(arg.given(), testing::Eq(false));
    ASSERT_THAT(arg.value(), testing::Eq(default_value.default_value()));

    args[0].func("123");
    ASSERT_THAT(static_cast<bool>(arg), testing::Eq(true));
    ASSERT_THAT(arg.given(), testing::Eq(true));
    ASSERT_THAT(arg.value(), testing::Eq(123));
}

TEST(argument, basicArgumentConstructMandatoryDefaultValue) {
    const std::string arg_name{"argument name"};
    const std::string arg_desc{"desciption"};
    const std::uint32_t default_value{1123};
    argument_test_parser_t tp;
    clapp::argument::basic_argument_t<std::uint32_t> arg{
        tp, arg_name, arg_desc, argument_test_parser_t::purpose_t::optional,
        clapp::value::default_value_t<std::uint32_t>{default_value}};
    argument_test_parser_t::arguments_vector_t args{tp.get_arguments()};
    argument_test_parser_t::argument_descriptions_vec_t descs{
        tp.get_optional_argument_descriptions()};
    argument_test_parser_t::validate_func_vec_t validate_funcs{
        tp.get_validate_functions()};
    ASSERT_THAT(validate_funcs.size(), testing::Eq(0));

    ASSERT_THAT(descs.size(), testing::Eq(1));
    ASSERT_THAT(descs[0].argument_string, testing::StrEq(arg_name));
    ASSERT_THAT(descs[0].description,
                testing::StrEq(
                    arg_desc + " (" +
                    clapp::value::default_value_t<std::uint32_t>{default_value}
                        .append_restriction_text() +
                    ")"));
    ASSERT_THAT(descs[0].argument_type,
                testing::Eq(argument_test_parser_t::argument_type_t::single));

    ASSERT_THAT(args.size(), testing::Eq(1));
    ASSERT_THAT(args[0].argument, testing::StrEq(arg_name));
    ASSERT_THAT(args[0].argument_type,
                testing::Eq(argument_test_parser_t::argument_type_t::single));

    ASSERT_THAT(static_cast<bool>(arg), testing::Eq(true));
    ASSERT_THAT(arg.given(), testing::Eq(false));
    ASSERT_THAT(arg.value(), testing::Eq(default_value));

    ASSERT_THROW(args[0].func("0x123412341234"),
                 clapp::exception::out_of_range_t);
    ASSERT_THROW(args[0].func("-0x12341234"), clapp::exception::out_of_range_t);

    args[0].func("0x12341234");
    ASSERT_THAT(static_cast<bool>(arg), testing::Eq(true));
    ASSERT_THAT(arg.given(), testing::Eq(true));
    ASSERT_THAT(arg.value(), testing::Eq(0x12341234U));
}

TEST(argument, basicArgumentConstructOptionalDefaultValueMinMax) {
    const std::string arg_name{"argument name"};
    const std::string arg_desc{"desciption"};
    argument_test_parser_t tp;
    const clapp::value::default_value_t<std::int64_t> default_value{0x3};
    const clapp::value::min_max_value_t<std::int64_t> min_max{-2, 10};
    clapp::argument::basic_argument_t<std::int64_t> arg{
        tp,
        arg_name,
        arg_desc,
        argument_test_parser_t::purpose_t::optional,
        default_value,
        min_max};
    argument_test_parser_t::arguments_vector_t args{tp.get_arguments()};
    argument_test_parser_t::argument_descriptions_vec_t descs{
        tp.get_optional_argument_descriptions()};
    argument_test_parser_t::validate_func_vec_t validate_funcs{
        tp.get_validate_functions()};
    ASSERT_THAT(validate_funcs.size(), testing::Eq(1));
    ASSERT_NO_THROW(validate_funcs[0]());

    ASSERT_THAT(descs.size(), testing::Eq(1));
    ASSERT_THAT(descs[0].argument_string, testing::StrEq(arg_name));
    ASSERT_THAT(descs[0].description,
                testing::StrEq(arg_desc + " (" +
                               default_value.append_restriction_text() + ", " +
                               min_max.append_restriction_text() + ")"));
    ASSERT_THAT(descs[0].argument_type,
                testing::Eq(argument_test_parser_t::argument_type_t::single));

    ASSERT_THAT(args.size(), testing::Eq(1));
    ASSERT_THAT(args[0].argument, testing::StrEq(arg_name));
    ASSERT_THAT(args[0].argument_type,
                testing::Eq(argument_test_parser_t::argument_type_t::single));

    ASSERT_THAT(static_cast<bool>(arg), testing::Eq(true));
    ASSERT_THAT(arg.given(), testing::Eq(false));
    ASSERT_THAT(arg.value(), testing::Eq(default_value.default_value()));

    args[0].func("5");
    ASSERT_THAT(static_cast<bool>(arg), testing::Eq(true));
    ASSERT_THAT(arg.given(), testing::Eq(true));
    ASSERT_THAT(arg.value(), testing::Eq(5));
    ASSERT_NO_THROW(validate_funcs[0]());

    args[0].func("-3");
    ASSERT_THROW(validate_funcs[0](), clapp::exception::out_of_range_t);

    args[0].func("-11");
    ASSERT_THROW(validate_funcs[0](), clapp::exception::out_of_range_t);
}

TEST(argument, basicArgumentConstructMandatoryPathExists) {
    const std::string arg_name1{"arg"};
    const std::string arg_desc1{"desc"};
    argument_test_parser_t tp;
    clapp::argument::basic_argument_t<clapp::fs::path> arg{
        tp, arg_name1, arg_desc1, argument_test_parser_t::purpose_t::optional,
        clapp::value::path_exists_t{}};

    argument_test_parser_t::arguments_vector_t args{tp.get_arguments()};
    argument_test_parser_t::argument_descriptions_vec_t descs{
        tp.get_optional_argument_descriptions()};
    argument_test_parser_t::validate_func_vec_t validate_funcs{
        tp.get_validate_functions()};
    ASSERT_THAT(validate_funcs.size(), testing::Eq(1));
    ASSERT_NO_THROW(validate_funcs[0]());

    ASSERT_THAT(descs.size(), testing::Eq(1));
    ASSERT_THAT(descs[0].argument_string, testing::StrEq(arg_name1));
    ASSERT_THAT(
        descs[0].description,
        testing::StrEq(arg_desc1 + " (" +
                       clapp::value::path_exists_t{}.append_restriction_text() +
                       ")"));
    ASSERT_THAT(descs[0].argument_type,
                testing::Eq(argument_test_parser_t::argument_type_t::single));

    ASSERT_THAT(args.size(), testing::Eq(1));
    ASSERT_THAT(args[0].argument, testing::StrEq(arg_name1));
    ASSERT_THAT(args[0].argument_type,
                testing::Eq(argument_test_parser_t::argument_type_t::single));

    ASSERT_THAT(static_cast<bool>(arg), testing::Eq(false));
    ASSERT_THAT(arg.given(), testing::Eq(false));
    ASSERT_THROW(arg.value(), clapp::exception::value_undefined_t);

    args[0].func("/tmp/xxx/yyy/zz");
    ASSERT_THAT(static_cast<bool>(arg), testing::Eq(true));
    ASSERT_THAT(arg.given(), testing::Eq(true));
    ASSERT_THAT(arg.value().string(), testing::StrEq("/tmp/xxx/yyy/zz"));
    ASSERT_THROW(validate_funcs[0](), clapp::exception::path_does_not_exist_t);

    args[0].func("/tmp");
    ASSERT_THAT(static_cast<bool>(arg), testing::Eq(true));
    ASSERT_THAT(arg.given(), testing::Eq(true));
    ASSERT_THAT(arg.value().string(), testing::StrEq("/tmp"));
    ASSERT_NO_THROW(validate_funcs[0]());
}

TEST(argument, basicVariadicArgumentFoundFuncLong) {
    const std::string arg_name{"arg"};
    const std::string arg_desc{"description"};

    std::stringstream ss;
    argument_test_parser_t tp;
    clapp::argument::basic_variadic_argument_t<std::string> arg{
        tp, arg_name, arg_desc,
        clapp::value::found_func_t{[&ss]() { ss << "this is a test"; }}};
    argument_test_parser_t::arguments_vector_t args{tp.get_arguments()};
    argument_test_parser_t::argument_descriptions_vec_t descs{
        tp.get_mandatory_argument_descriptions()};

    ASSERT_THAT(args.size(), testing::Eq(1));
    ASSERT_THAT(args[0].argument, testing::StrEq(arg_name));
    ASSERT_THAT(args[0].argument_type,
                testing::Eq(argument_test_parser_t::argument_type_t::variadic));

    args[0].func("argument-value");
    args[0].func("argument-value 2");

    ASSERT_THAT(static_cast<bool>(arg), testing::Eq(true));
    ASSERT_THAT(arg.given(), testing::Eq(true));
    ASSERT_THAT(arg.value(), testing::Eq(std::vector<std::string>{
                                 "argument-value", "argument-value 2"}));
    ASSERT_THAT(ss.str(), testing::StrEq("this is a testthis is a test"));
}

TEST(argument, basicVariadicArgumentConstructSimple) {
    const std::string arg_name{"argument name"};
    const std::string arg_desc{"desc"};
    argument_test_parser_t tp;
    clapp::argument::basic_variadic_argument_t<std::int32_t> arg{tp, arg_name,
                                                                 arg_desc};
    argument_test_parser_t::arguments_vector_t args{tp.get_arguments()};
    argument_test_parser_t::argument_descriptions_vec_t descs{
        tp.get_mandatory_argument_descriptions()};
    argument_test_parser_t::validate_func_vec_t validate_funcs{
        tp.get_validate_functions()};
    ASSERT_THAT(validate_funcs.size(), testing::Eq(1));
    ASSERT_THROW(validate_funcs[0](), clapp::exception::argument_exception_t);

    ASSERT_THAT(descs.size(), testing::Eq(1));
    ASSERT_THAT(descs[0].argument_string, testing::StrEq(arg_name));
    ASSERT_THAT(
        descs[0].description,
        testing::StrEq(arg_desc + " (" +
                       clapp::basic_variadic_argument_t<
                           std::int32_t>::variadic_argument_restrictions() +
                       ")"));
    ASSERT_THAT(descs[0].argument_type,
                testing::Eq(argument_test_parser_t::argument_type_t::variadic));

    ASSERT_THAT(args.size(), testing::Eq(1));
    ASSERT_THAT(args[0].argument, testing::StrEq(arg_name));
    ASSERT_THAT(args[0].argument_type,
                testing::Eq(argument_test_parser_t::argument_type_t::variadic));

    ASSERT_THAT(static_cast<bool>(arg), testing::Eq(false));
    ASSERT_THAT(arg.given(), testing::Eq(false));
    ASSERT_THAT(arg.value(), std::vector<std::int32_t>{});

    args[0].func("12345");
    ASSERT_THAT(static_cast<bool>(arg), testing::Eq(true));
    ASSERT_THAT(arg.given(), testing::Eq(true));
    ASSERT_THAT(arg.value(), std::vector<std::int32_t>{12345});

    ASSERT_NO_THROW(validate_funcs[0]());

    args[0].func("1234");
    ASSERT_THAT(static_cast<bool>(arg), testing::Eq(true));
    ASSERT_THAT(arg.given(), testing::Eq(true));
    ASSERT_THAT(arg.value(),
                testing::Eq(std::vector<std::int32_t>{12345, 1234}));

    ASSERT_NO_THROW(validate_funcs[0]());
}

TEST(argument, basicVariadicArgumentConstructSimpleOptional) {
    const std::string arg_name{"argument name"};
    const std::string arg_desc{"desc"};
    argument_test_parser_t tp;
    clapp::argument::basic_variadic_argument_t<std::string> arg{
        tp, arg_name, arg_desc, argument_test_parser_t::purpose_t::optional};
    argument_test_parser_t::arguments_vector_t args{tp.get_arguments()};
    argument_test_parser_t::argument_descriptions_vec_t descs{
        tp.get_optional_argument_descriptions()};
    argument_test_parser_t::validate_func_vec_t validate_funcs{
        tp.get_validate_functions()};
    ASSERT_THAT(validate_funcs.size(), testing::Eq(0));

    ASSERT_THAT(descs.size(), testing::Eq(1));
    ASSERT_THAT(descs[0].argument_string, testing::StrEq(arg_name));
    ASSERT_THAT(
        descs[0].description,
        testing::StrEq(arg_desc + " (" +
                       clapp::basic_variadic_argument_t<
                           std::int32_t>::variadic_argument_restrictions() +
                       ")"));
    ASSERT_THAT(descs[0].argument_type,
                testing::Eq(argument_test_parser_t::argument_type_t::variadic));

    ASSERT_THAT(args.size(), testing::Eq(1));
    ASSERT_THAT(args[0].argument, testing::StrEq(arg_name));
    ASSERT_THAT(args[0].argument_type,
                testing::Eq(argument_test_parser_t::argument_type_t::variadic));

    ASSERT_THAT(static_cast<bool>(arg), testing::Eq(false));
    ASSERT_THAT(arg.given(), testing::Eq(false));
    ASSERT_THAT(arg.value(), std::vector<std::string>{});

    args[0].func("ccc");
    ASSERT_THAT(static_cast<bool>(arg), testing::Eq(true));
    ASSERT_THAT(arg.given(), testing::Eq(true));
    ASSERT_THAT(arg.value(), std::vector<std::string>{"ccc"});

    args[0].func("aba");
    ASSERT_THAT(static_cast<bool>(arg), testing::Eq(true));
    ASSERT_THAT(arg.given(), testing::Eq(true));
    ASSERT_THAT(arg.value(),
                testing::Eq(std::vector<std::string>{"ccc", "aba"}));
}

TEST(argument, basicVariadicArgumentConstructSimpleOptionalMinMax) {
    const std::string arg_name{"argument name"};
    const std::string arg_desc{"desc"};
    const clapp::value::min_max_value_t<std::int64_t> min_max{-2, 10};
    argument_test_parser_t tp;
    clapp::argument::basic_variadic_argument_t<std::int64_t> arg{
        tp, arg_name, arg_desc, min_max,
        argument_test_parser_t::purpose_t::optional};
    argument_test_parser_t::arguments_vector_t args{tp.get_arguments()};
    argument_test_parser_t::argument_descriptions_vec_t descs{
        tp.get_optional_argument_descriptions()};
    argument_test_parser_t::validate_func_vec_t validate_funcs{
        tp.get_validate_functions()};

    ASSERT_THAT(validate_funcs.size(), testing::Eq(1));
    ASSERT_NO_THROW(validate_funcs[0]());

    ASSERT_THAT(descs.size(), testing::Eq(1));
    ASSERT_THAT(descs[0].argument_string, testing::StrEq(arg_name));
    ASSERT_THAT(
        descs[0].description,
        testing::StrEq(arg_desc + " (" +
                       clapp::basic_variadic_argument_t<
                           std::int64_t>::variadic_argument_restrictions() +
                       ", " + min_max.append_restriction_text() + ")"));
    ASSERT_THAT(descs[0].argument_type,
                testing::Eq(argument_test_parser_t::argument_type_t::variadic));

    ASSERT_THAT(args.size(), testing::Eq(1));
    ASSERT_THAT(args[0].argument, testing::StrEq(arg_name));
    ASSERT_THAT(args[0].argument_type,
                testing::Eq(argument_test_parser_t::argument_type_t::variadic));

    ASSERT_THAT(static_cast<bool>(arg), testing::Eq(false));
    ASSERT_THAT(arg.given(), testing::Eq(false));
    ASSERT_THAT(arg.value(), std::vector<std::int64_t>{});

    args[0].func("8");
    ASSERT_THAT(static_cast<bool>(arg), testing::Eq(true));
    ASSERT_THAT(arg.given(), testing::Eq(true));
    ASSERT_THAT(arg.value(), std::vector<std::int64_t>{8});

    args[0].func("6");
    ASSERT_THAT(static_cast<bool>(arg), testing::Eq(true));
    ASSERT_THAT(arg.given(), testing::Eq(true));
    ASSERT_THAT(arg.value(), testing::Eq(std::vector<std::int64_t>{8, 6}));

    ASSERT_NO_THROW(validate_funcs[0]());

    args[0].func("12");
    ASSERT_THAT(static_cast<bool>(arg), testing::Eq(true));
    ASSERT_THAT(arg.given(), testing::Eq(true));
    ASSERT_THAT(arg.value(), testing::Eq(std::vector<std::int64_t>{8, 6, 12}));

    ASSERT_THROW(validate_funcs[0](), clapp::exception::out_of_range_t);
}

TEST(argument, basicVariadicArgumentConstructSimpleDefaultValueThrows) {
    const std::string arg_name{"argument name"};
    const std::string arg_desc{"desc"};
    const clapp::value::default_value_t<std::int8_t> default_value{10};
    argument_test_parser_t tp;
    ASSERT_THROW((clapp::argument::basic_variadic_argument_t<std::int64_t>{
                     tp, arg_name, arg_desc, default_value}),
                 clapp::exception::argument_exception_t);
}
