#include <clapp/argument.h>
#include <gmock/gmock.h>

class test_parser_t : public clapp::parser::basic_parser_t {
    public:
    using clapp::parser::basic_parser_t::purpose_t;

    using clapp::parser::basic_parser_t::argument_type_t;
    using clapp::parser::basic_parser_t::arguments_vector_t;
    using clapp::parser::basic_parser_t::argument_descriptions_vec_t;
    using clapp::parser::basic_parser_t::validate_func_vec_t;

    using clapp::parser::basic_parser_t::get_arguments;
    using clapp::parser::basic_parser_t::get_mandatory_argument_descriptions;
    using clapp::parser::basic_parser_t::get_optional_argument_descriptions;
    using clapp::parser::basic_parser_t::get_validate_functions;
};

TEST(argument, basic_argument_construct_simple) {
    const std::string arg_name{"argument name"};
    const std::string arg_desc{"description"};
    test_parser_t tp;
    clapp::argument::basic_argument_t<std::int32_t> arg{tp, arg_name, arg_desc};
    test_parser_t::arguments_vector_t args{tp.get_arguments()};
    test_parser_t::argument_descriptions_vec_t descs{tp.get_mandatory_argument_descriptions()};
    test_parser_t::validate_func_vec_t validate_funcs{tp.get_validate_functions()};
    ASSERT_THAT(validate_funcs.size(), testing::Eq(1));

    ASSERT_THAT(descs.size(), testing::Eq(1));
    ASSERT_THAT(descs[0].argument_string, testing::StrEq(arg_name));
    ASSERT_THAT(descs[0].description, testing::StrEq(arg_desc));
    ASSERT_THAT(descs[0].argument_type, testing::Eq(test_parser_t::argument_type_t::single));

    ASSERT_THAT(args.size(), testing::Eq(1));
    ASSERT_THAT(args[0].argument,testing::StrEq(arg_name));
    ASSERT_THAT(args[0].argument_type, testing::Eq(test_parser_t::argument_type_t::single));

    ASSERT_THAT(static_cast<bool>(arg), testing::Eq(false));
    ASSERT_THAT(arg.given(), testing::Eq(false));
    ASSERT_THROW(arg.value(), std::exception);
    ASSERT_THROW(validate_funcs[0](), std::runtime_error);

    args[0].func("12345");
    ASSERT_THAT(static_cast<bool>(arg), testing::Eq(true));
    ASSERT_THAT(arg.given(), testing::Eq(true));
    ASSERT_THAT(arg.value(), testing::Eq(12345));
    ASSERT_NO_THROW(validate_funcs[0]());
}

TEST(argument, basic_argument_construct_mandatory) {
    const std::string arg_name{"argument name"};
    const std::string arg_desc{"description"};
    test_parser_t tp;
    clapp::argument::basic_argument_t<std::string> arg{tp, arg_name, arg_desc, 
        test_parser_t::purpose_t::mandatory};
    test_parser_t::arguments_vector_t args{tp.get_arguments()};
    test_parser_t::argument_descriptions_vec_t descs{tp.get_mandatory_argument_descriptions()};
    test_parser_t::validate_func_vec_t validate_funcs{tp.get_validate_functions()};
    ASSERT_THAT(validate_funcs.size(), testing::Eq(1));

    ASSERT_THAT(descs.size(), testing::Eq(1));
    ASSERT_THAT(descs[0].argument_string, testing::StrEq(arg_name));
    ASSERT_THAT(descs[0].description, testing::StrEq(arg_desc));
    ASSERT_THAT(descs[0].argument_type, testing::Eq(test_parser_t::argument_type_t::single));

    ASSERT_THAT(args.size(), testing::Eq(1));
    ASSERT_THAT(args[0].argument,testing::StrEq(arg_name));
    ASSERT_THAT(args[0].argument_type, testing::Eq(test_parser_t::argument_type_t::single));

    ASSERT_THAT(static_cast<bool>(arg), testing::Eq(false));
    ASSERT_THAT(arg.given(), testing::Eq(false));
    ASSERT_THROW(arg.value(), std::exception);
    ASSERT_THROW(validate_funcs[0](), std::runtime_error);

    args[0].func("string-arg");
    ASSERT_THAT(static_cast<bool>(arg), testing::Eq(true));
    ASSERT_THAT(arg.given(), testing::Eq(true));
    ASSERT_THAT(arg.value(), testing::StrEq("string-arg"));
    ASSERT_NO_THROW(validate_funcs[0]());
}

TEST(argument, basic_argument_construct_optional) {
    const std::string arg_name{"argument name"};
    const std::string arg_desc{"description"};
    test_parser_t tp;
    clapp::argument::basic_argument_t<std::int32_t> arg{tp, arg_name, arg_desc, 
        test_parser_t::purpose_t::optional};
    test_parser_t::arguments_vector_t args{tp.get_arguments()};
    test_parser_t::argument_descriptions_vec_t descs{tp.get_optional_argument_descriptions()};
    test_parser_t::validate_func_vec_t validate_funcs{tp.get_validate_functions()};
    ASSERT_THAT(validate_funcs.size(), testing::Eq(0));

    ASSERT_THAT(descs.size(), testing::Eq(1));
    ASSERT_THAT(descs[0].argument_string, testing::StrEq(arg_name));
    ASSERT_THAT(descs[0].description, testing::StrEq(arg_desc));
    ASSERT_THAT(descs[0].argument_type, testing::Eq(test_parser_t::argument_type_t::single));

    ASSERT_THAT(args.size(), testing::Eq(1));
    ASSERT_THAT(args[0].argument,testing::StrEq(arg_name));
    ASSERT_THAT(args[0].argument_type, testing::Eq(test_parser_t::argument_type_t::single));

    ASSERT_THAT(static_cast<bool>(arg), testing::Eq(false));
    ASSERT_THAT(arg.given(), testing::Eq(false));
    ASSERT_THROW(arg.value(), std::exception);

    args[0].func("12345");
    ASSERT_THAT(static_cast<bool>(arg), testing::Eq(true));
    ASSERT_THAT(arg.given(), testing::Eq(true));
    ASSERT_THAT(arg.value(), testing::Eq(12345));
}

TEST(argument, basic_argument_construct_optional_default_value) {
    const std::string arg_name{"argument name"};
    const std::string arg_desc{"description"};
    test_parser_t tp;
    const clapp::value::default_value_t<std::int8_t> default_value{10};
    clapp::argument::basic_argument_t<std::int8_t> arg{tp, arg_name, arg_desc, test_parser_t::purpose_t::optional, default_value};
    test_parser_t::arguments_vector_t args{tp.get_arguments()};
    test_parser_t::argument_descriptions_vec_t descs{tp.get_optional_argument_descriptions()};
    test_parser_t::validate_func_vec_t validate_funcs{tp.get_validate_functions()};
    ASSERT_THAT(validate_funcs.size(), testing::Eq(0));

    ASSERT_THAT(descs.size(), testing::Eq(1));
    ASSERT_THAT(descs[0].argument_string, testing::StrEq(arg_name));
    ASSERT_THAT(descs[0].description, testing::StrEq(arg_desc + " (" + default_value.append_restriction_text() + ")"));
    ASSERT_THAT(descs[0].argument_type, testing::Eq(test_parser_t::argument_type_t::single));

    ASSERT_THAT(args.size(), testing::Eq(1));
    ASSERT_THAT(args[0].argument,testing::StrEq(arg_name));
    ASSERT_THAT(args[0].argument_type, testing::Eq(test_parser_t::argument_type_t::single));

    ASSERT_THAT(static_cast<bool>(arg), testing::Eq(true));
    ASSERT_THAT(arg.given(), testing::Eq(false));
    ASSERT_THAT(arg.value(), testing::Eq(default_value.default_value()));

    args[0].func("123");
    ASSERT_THAT(static_cast<bool>(arg), testing::Eq(true));
    ASSERT_THAT(arg.given(), testing::Eq(true));
    ASSERT_THAT(arg.value(), testing::Eq(123));
}

TEST(argument, basic_argument_construct_mandatory_default_value) {
    const std::string arg_name{"argument name"};
    const std::string arg_desc{"desciption"};
    const std::uint32_t default_value{1123};
    test_parser_t tp;
    clapp::argument::basic_argument_t<std::uint32_t> arg{tp, arg_name, arg_desc, test_parser_t::purpose_t::optional, clapp::value::default_value_t<std::uint32_t>{default_value}};
    test_parser_t::arguments_vector_t args{tp.get_arguments()};
    test_parser_t::argument_descriptions_vec_t descs{tp.get_optional_argument_descriptions()};
    test_parser_t::validate_func_vec_t validate_funcs{tp.get_validate_functions()};
    ASSERT_THAT(validate_funcs.size(), testing::Eq(0));

    ASSERT_THAT(descs.size(), testing::Eq(1));
    ASSERT_THAT(descs[0].argument_string, testing::StrEq(arg_name));
    ASSERT_THAT(descs[0].description, testing::StrEq(arg_desc + " (" + clapp::value::default_value_t{default_value}.append_restriction_text() + ")"));
    ASSERT_THAT(descs[0].argument_type, testing::Eq(test_parser_t::argument_type_t::single));

    ASSERT_THAT(args.size(), testing::Eq(1));
    ASSERT_THAT(args[0].argument,testing::StrEq(arg_name));
    ASSERT_THAT(args[0].argument_type, testing::Eq(test_parser_t::argument_type_t::single));

    ASSERT_THAT(static_cast<bool>(arg), testing::Eq(true));
    ASSERT_THAT(arg.given(), testing::Eq(false));
    ASSERT_THAT(arg.value(), testing::Eq(default_value));

    ASSERT_THROW(args[0].func("0x123412341234"), std::out_of_range);
    ASSERT_THROW(args[0].func("-0x12341234"), std::out_of_range);

    args[0].func("0x12341234");
    ASSERT_THAT(static_cast<bool>(arg), testing::Eq(true));
    ASSERT_THAT(arg.given(), testing::Eq(true));
    ASSERT_THAT(arg.value(), testing::Eq(0x12341234u));
}

TEST(argument, basic_argument_construct_optional_default_value_min_max) {
    const std::string arg_name{"argument name"};
    const std::string arg_desc{"desciption"};
    test_parser_t tp;
    const clapp::value::default_value_t<std::int64_t> default_value{0x3};
    const clapp::value::min_max_value_t<std::int64_t> min_max{-2, 10};
    clapp::argument::basic_argument_t<std::int64_t> arg{tp, arg_name, arg_desc, test_parser_t::purpose_t::optional, default_value, min_max};
    test_parser_t::arguments_vector_t args{tp.get_arguments()};
    test_parser_t::argument_descriptions_vec_t descs{tp.get_optional_argument_descriptions()};
    test_parser_t::validate_func_vec_t validate_funcs{tp.get_validate_functions()};
    ASSERT_THAT(validate_funcs.size(), testing::Eq(1));
    ASSERT_NO_THROW(validate_funcs[0]());

    ASSERT_THAT(descs.size(), testing::Eq(1));
    ASSERT_THAT(descs[0].argument_string, testing::StrEq(arg_name));
    ASSERT_THAT(descs[0].description, testing::StrEq(arg_desc + " (" + default_value.append_restriction_text() + ", " + min_max.append_restriction_text() + ")"));
    ASSERT_THAT(descs[0].argument_type, testing::Eq(test_parser_t::argument_type_t::single));

    ASSERT_THAT(args.size(), testing::Eq(1));
    ASSERT_THAT(args[0].argument,testing::StrEq(arg_name));
    ASSERT_THAT(args[0].argument_type, testing::Eq(test_parser_t::argument_type_t::single));

    ASSERT_THAT(static_cast<bool>(arg), testing::Eq(true));
    ASSERT_THAT(arg.given(), testing::Eq(false));
    ASSERT_THAT(arg.value(), testing::Eq(default_value.default_value()));

    args[0].func("5");
    ASSERT_THAT(static_cast<bool>(arg), testing::Eq(true));
    ASSERT_THAT(arg.given(), testing::Eq(true));
    ASSERT_THAT(arg.value(), testing::Eq(5));
    ASSERT_NO_THROW(validate_funcs[0]());

    args[0].func("-3");
    ASSERT_THROW(validate_funcs[0](), std::out_of_range);

    args[0].func("-11");
    ASSERT_THROW(validate_funcs[0](), std::out_of_range);
}

TEST(argument, basic_argument_construct_mandatory_path_exists) {
    const std::string arg_name1{"arg"};
    const std::string arg_desc1{"desc"};
    test_parser_t tp;
    clapp::argument::basic_argument_t<clapp::fs::path> arg{tp, arg_name1, arg_desc1, test_parser_t::purpose_t::optional, clapp::value::path_exists_t{}};

    test_parser_t::arguments_vector_t args{tp.get_arguments()};
    test_parser_t::argument_descriptions_vec_t descs{tp.get_optional_argument_descriptions()};
    test_parser_t::validate_func_vec_t validate_funcs{tp.get_validate_functions()};
    ASSERT_THAT(validate_funcs.size(), testing::Eq(1));
    ASSERT_NO_THROW(validate_funcs[0]());

    ASSERT_THAT(descs.size(), testing::Eq(1));
    ASSERT_THAT(descs[0].argument_string, testing::StrEq(arg_name1));
    ASSERT_THAT(descs[0].description, testing::StrEq(arg_desc1 + " (" + clapp::value::path_exists_t{}.append_restriction_text() + ")"));
    ASSERT_THAT(descs[0].argument_type, testing::Eq(test_parser_t::argument_type_t::single));

    ASSERT_THAT(args.size(), testing::Eq(1));
    ASSERT_THAT(args[0].argument,testing::StrEq(arg_name1));
    ASSERT_THAT(args[0].argument_type, testing::Eq(test_parser_t::argument_type_t::single));

    ASSERT_THAT(static_cast<bool>(arg), testing::Eq(false));
    ASSERT_THAT(arg.given(), testing::Eq(false));
    ASSERT_THROW(arg.value(), std::bad_optional_access);

    args[0].func("/tmp/xxx/yyy/zz");
    ASSERT_THAT(static_cast<bool>(arg), testing::Eq(true));
    ASSERT_THAT(arg.given(), testing::Eq(true));
    ASSERT_THAT(arg.value().string(), testing::StrEq("/tmp/xxx/yyy/zz"));
    ASSERT_THROW(validate_funcs[0](), std::runtime_error);

    args[0].func("/tmp");
    ASSERT_THAT(static_cast<bool>(arg), testing::Eq(true));
    ASSERT_THAT(arg.given(), testing::Eq(true));
    ASSERT_THAT(arg.value().string(), testing::StrEq("/tmp"));
    ASSERT_NO_THROW(validate_funcs[0]());
}

TEST(argument, basic_variadic_argument_construct_simple) {
    const std::string arg_name{"argument name"};
    const std::string arg_desc{"desc"};
    test_parser_t tp;
    clapp::argument::basic_variadic_argument_t<std::int32_t> arg{tp, arg_name, arg_desc};
    test_parser_t::arguments_vector_t args{tp.get_arguments()};
    test_parser_t::argument_descriptions_vec_t descs{tp.get_mandatory_argument_descriptions()};
    test_parser_t::validate_func_vec_t validate_funcs{tp.get_validate_functions()};
    ASSERT_THAT(validate_funcs.size(), testing::Eq(1));
    ASSERT_THROW(validate_funcs[0](), std::runtime_error);

    ASSERT_THAT(descs.size(), testing::Eq(1));
    ASSERT_THAT(descs[0].argument_string, testing::StrEq(arg_name));
    ASSERT_THAT(descs[0].description, testing::StrEq(arg_desc + " (" + clapp::basic_variadic_argument_t<std::int32_t>::variadic_argument_restrictions() + ")"));
    ASSERT_THAT(descs[0].argument_type, testing::Eq(test_parser_t::argument_type_t::variadic));

    ASSERT_THAT(args.size(), testing::Eq(1));
    ASSERT_THAT(args[0].argument,testing::StrEq(arg_name));
    ASSERT_THAT(args[0].argument_type, testing::Eq(test_parser_t::argument_type_t::variadic));

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
    ASSERT_THAT(arg.value(), testing::Eq(std::vector<std::int32_t>{12345, 1234}));

    ASSERT_NO_THROW(validate_funcs[0]());
}

TEST(argument, basic_variadic_argument_construct_simple_optional) {
    const std::string arg_name{"argument name"};
    const std::string arg_desc{"desc"};
    test_parser_t tp;
    clapp::argument::basic_variadic_argument_t<std::string> arg{tp, arg_name, arg_desc, 
        test_parser_t::purpose_t::optional};
    test_parser_t::arguments_vector_t args{tp.get_arguments()};
    test_parser_t::argument_descriptions_vec_t descs{tp.get_optional_argument_descriptions()};
    test_parser_t::validate_func_vec_t validate_funcs{tp.get_validate_functions()};
    ASSERT_THAT(validate_funcs.size(), testing::Eq(0));

    ASSERT_THAT(descs.size(), testing::Eq(1));
    ASSERT_THAT(descs[0].argument_string, testing::StrEq(arg_name));
    ASSERT_THAT(descs[0].description, testing::StrEq(arg_desc + " (" + clapp::basic_variadic_argument_t<std::int32_t>::variadic_argument_restrictions() + ")"));
    ASSERT_THAT(descs[0].argument_type, testing::Eq(test_parser_t::argument_type_t::variadic));

    ASSERT_THAT(args.size(), testing::Eq(1));
    ASSERT_THAT(args[0].argument,testing::StrEq(arg_name));
    ASSERT_THAT(args[0].argument_type, testing::Eq(test_parser_t::argument_type_t::variadic));

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
    ASSERT_THAT(arg.value(), testing::Eq(std::vector<std::string>{"ccc", "aba"}));
}

TEST(argument, basic_variadic_argument_construct_simple_optional_min_max) {
    const std::string arg_name{"argument name"};
    const std::string arg_desc{"desc"};
    const clapp::value::min_max_value_t<std::int64_t> min_max{-2, 10};
    test_parser_t tp;
    clapp::argument::basic_variadic_argument_t<std::int64_t> arg{tp, arg_name, arg_desc, min_max, 
        test_parser_t::purpose_t::optional};
    test_parser_t::arguments_vector_t args{tp.get_arguments()};
    test_parser_t::argument_descriptions_vec_t descs{tp.get_optional_argument_descriptions()};
    test_parser_t::validate_func_vec_t validate_funcs{tp.get_validate_functions()};

    ASSERT_THAT(validate_funcs.size(), testing::Eq(1));
    ASSERT_NO_THROW(validate_funcs[0]());

    ASSERT_THAT(descs.size(), testing::Eq(1));
    ASSERT_THAT(descs[0].argument_string, testing::StrEq(arg_name));
    ASSERT_THAT(descs[0].description, testing::StrEq(arg_desc + " (" + clapp::basic_variadic_argument_t<std::int64_t>::variadic_argument_restrictions() + ", " + min_max.append_restriction_text() + ")"));
    ASSERT_THAT(descs[0].argument_type, testing::Eq(test_parser_t::argument_type_t::variadic));

    ASSERT_THAT(args.size(), testing::Eq(1));
    ASSERT_THAT(args[0].argument,testing::StrEq(arg_name));
    ASSERT_THAT(args[0].argument_type, testing::Eq(test_parser_t::argument_type_t::variadic));

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

    ASSERT_THROW(validate_funcs[0](), std::out_of_range);
}

TEST(argument, basic_variadic_argument_construct_simple_default_value_throws) {
    const std::string arg_name{"argument name"};
    const std::string arg_desc{"desc"};
    const clapp::value::default_value_t<std::int8_t> default_value{10};
    test_parser_t tp;
    ASSERT_THROW((clapp::argument::basic_variadic_argument_t<std::int64_t>{tp, arg_name, arg_desc, default_value}), std::runtime_error);
}
