#include <clapp/argument.h>
#include <clapp/main_parser.h>
#include <clapp/option.h>
#include <clapp/parser.h>
#include <clapp/sub_parser.h>
#include <gmock/gmock.h>

#include "matcher.h"

class print_and_exit_t {
   public:
    MOCK_METHOD2(print_and_exit,
                 clapp::value::exit_t(const std::string_view print_msg,
                                      int exit_code));
};

template <class T, size_t N>
inline clapp::parser::types::arg_t parser_make_arg_t(T (&arg)[N]) {
    return clapp::parser::types::arg_t{static_cast<const char* const*>(arg),
                                       N - 1};
}

class empty_basic_parser_t : public clapp::basic_parser_t {
   public:
    using clapp::basic_parser_t::basic_parser_t;
    ~empty_basic_parser_t() override;
    [[nodiscard]] std::string gen_short_line_prefix() const override;
};

empty_basic_parser_t::~empty_basic_parser_t() = default;

std::string empty_basic_parser_t::gen_short_line_prefix() const {
    return "empty-basic-parser" + gen_short_line();
}

class simple_test_parser_t : public clapp::basic_parser_t {
   public:
    using clapp::basic_parser_t::basic_parser_t;
    ~simple_test_parser_t() override;

    static constexpr std::int32_t min_int{10};
    static constexpr std::int32_t max_int{200};

    clapp::option::bool_option_t bool_option{
        *this, "bool", 'b', "Bool option.",
        clapp::parser::types::purpose_t::optional};

    clapp::option::int32_param_option_t int_option{
        *this,
        "int",
        'i',
        "Int option.",
        clapp::value::min_max_value_t<std::int32_t>{min_int, max_int},
        clapp::parser::types::purpose_t::optional};

    clapp::argument::string_argument_t string_arg{*this, "arg-name",
                                                  "Arg desc"};

    clapp::argument::variadic_string_argument_t variadic_string_arg{
        *this, "variadic-arg-name", "Variadic arg desc",
        clapp::parser::types::purpose_t::optional};

    [[nodiscard]] std::string gen_short_line_prefix() const override;
};

simple_test_parser_t::~simple_test_parser_t() = default;

std::string simple_test_parser_t::gen_short_line_prefix() const {
    return "simple-test-parser " + gen_short_line();
}

class simple_test_parser2_t : public clapp::basic_parser_t {
   public:
    using clapp::basic_parser_t::basic_parser_t;
    ~simple_test_parser2_t() override;

    clapp::option::count_option_t count_option{
        *this, "count", 'c', "Count option.",
        clapp::parser::types::purpose_t::mandatory};

    clapp::argument::string_argument_t string_arg{
        *this, "arg-name", "Arg desc",
        clapp::parser::types::purpose_t::optional};

    [[nodiscard]] std::string gen_short_line_prefix() const override;
};

simple_test_parser2_t::~simple_test_parser2_t() = default;

[[nodiscard]] std::string simple_test_parser2_t::gen_short_line_prefix() const {
    return "simple-test-parser2 " + gen_short_line();
}

class simple_test_parser3_t : public clapp::basic_parser_t {
   public:
    using clapp::basic_parser_t::basic_parser_t;
    ~simple_test_parser3_t() override;

    static constexpr std::int32_t min_int{10};
    static constexpr std::int32_t max_int{200};

    clapp::option::vector_int64_param_option_t int_option{
        *this,
        "int",
        'i',
        "Int option.",
        clapp::value::min_max_value_t<std::int64_t>{min_int, max_int},
        clapp::parser::types::purpose_t::mandatory};

    clapp::option::vector_string_param_option_t string_option{
        *this, "str", std::vector<char>{'s', 'o'}, "String option.",
        clapp::parser::types::purpose_t::optional};

    clapp::argument::variadic_string_argument_t variadic_string_arg{
        *this, "variadic-arg-name", "Variadic arg desc",
        clapp::parser::types::purpose_t::mandatory};

    [[nodiscard]] std::string gen_short_line_prefix() const override;
};

simple_test_parser3_t::~simple_test_parser3_t() = default;

std::string simple_test_parser3_t::gen_short_line_prefix() const {
    return "simple-test-parser3 " + gen_short_line();
}

class simple_test_parser4_t : public clapp::basic_parser_t {
   public:
    using clapp::basic_parser_t::basic_parser_t;
    ~simple_test_parser4_t() override;

    static constexpr std::int32_t min_int{10};
    static constexpr std::int32_t max_int{200};

    static clapp::value::found_func_t::ret_t found_cb(
        const std::string_view option) {
        std::cout << "found cb: " << option << std::endl;
        return clapp::value::exit_t::exit(EXIT_SUCCESS);
    }

    clapp::option::bool_option_t bool_opt{
        *this,
        "bo",
        'b',
        "Arg desc",
        clapp::parser::types::purpose_t::optional,
        clapp::value::found_func_t{found_cb}};

    clapp::option::string_param_option_t string_opt{
        *this,
        "opt",
        'o',
        "Arg desc",
        clapp::parser::types::purpose_t::optional,
        clapp::value::found_func_t{found_cb}};

    clapp::argument::string_argument_t string_arg{
        *this, "arg-name", "Arg desc",
        clapp::parser::types::purpose_t::optional,
        clapp::value::found_func_t{found_cb}};

    [[nodiscard]] std::string gen_short_line_prefix() const override;
};

simple_test_parser4_t::~simple_test_parser4_t() = default;

std::string simple_test_parser4_t::gen_short_line_prefix() const {
    return "simple-test-parser4 " + gen_short_line();
}

class simple_test_parser5_t : public clapp::basic_parser_t {
   public:
    using clapp::basic_parser_t::basic_parser_t;
    ~simple_test_parser5_t() override;

    static constexpr std::int32_t min_int{10};
    static constexpr std::int32_t max_int{200};

    static clapp::value::found_func_t::ret_t found_cb(
        const std::string_view option) {
        static std::size_t cnt{0U};
        std::cout << "found cb: " << option << std::endl;
        cnt++;
        if (cnt == 1U) {
            return clapp::value::found_func_t::ret_t{};
        }
        return clapp::value::exit_t::exit(EXIT_FAILURE);
    }

    clapp::argument::string_argument_t string_arg{*this, "arg-name",
                                                  "Arg desc"};

    clapp::argument::variadic_string_argument_t var_string_arg{
        *this, "var-arg-name", "Var arg desc",
        clapp::parser::types::purpose_t::optional,
        clapp::value::found_func_t{found_cb}};

    [[nodiscard]] std::string gen_short_line_prefix() const override;
};

simple_test_parser5_t::~simple_test_parser5_t() = default;

std::string simple_test_parser5_t::gen_short_line_prefix() const {
    return "simple-test-parser5 " + gen_short_line();
}

class mandatory_options_test_parser_t : public clapp::basic_parser_t {
   public:
    using clapp::basic_parser_t::basic_parser_t;
    ~mandatory_options_test_parser_t() override;

    static constexpr std::int32_t min_int{10};
    static constexpr std::int32_t max_int{200};

    clapp::option::bool_option_t bool_option{*this, "bool", 'b',
                                             "Bool option."};

    clapp::option::string_param_option_t string_option{*this, "string", 's',
                                                       "String option."};

    clapp::option::int32_param_option_t int_option{
        *this, "int", 'i', "Int option.",
        clapp::value::min_max_value_t<std::int32_t>{min_int, max_int}};

    [[nodiscard]] std::string gen_short_line_prefix() const override;
};

mandatory_options_test_parser_t::~mandatory_options_test_parser_t() = default;

std::string mandatory_options_test_parser_t::gen_short_line_prefix() const {
    return "mandatory-options-test-parser " + gen_short_line();
}

class sub_parser_container_t : public clapp::basic_parser_t {
   public:
    using clapp::basic_parser_t::basic_parser_t;
    ~sub_parser_container_t() override;

    [[nodiscard]] std::string gen_short_line_prefix() const override;

    clapp::option::bool_option_t bool_option{
        *this, "bool", 'b', "Bool option.",
        clapp::parser::types::purpose_t::optional};

    clapp::option::bool_option_t bool_option2{
        *this, "second", '2', "Second bool option.",
        clapp::parser::types::purpose_t::optional};

    clapp::argument::string_argument_t string_arg{*this, "arg-name",
                                                  "Arg desc"};

    class simple_sub_parser_t : public clapp::parser::basic_sub_parser_t {
       public:
        ~simple_sub_parser_t() override;
        clapp::option::bool_option_t bool_option{
            *this, "bool", 'b', "Bool option.",
            clapp::parser::types::purpose_t::optional};

        using clapp::parser::basic_sub_parser_t::basic_sub_parser_t;
        clapp::option::string_param_option_t string_option{
            *this, "string", 's', "String option.",
            clapp::parser::types::purpose_t::optional};

        clapp::argument::string_argument_t string_arg{
            *this, "sub-arg-name", "Sub arg desc",
            clapp::parser::types::purpose_t::optional};
    };

    simple_sub_parser_t sub_parser{*this, "sub-parser", "Sub parser desc"};
};

sub_parser_container_t::simple_sub_parser_t::~simple_sub_parser_t() = default;

sub_parser_container_t::~sub_parser_container_t() = default;

std::string sub_parser_container_t::gen_short_line_prefix() const {
    return "sub_parser_container " + gen_short_line();
}

class help_parser_t : public clapp::basic_parser_t {
   public:
    using clapp::basic_parser_t::basic_parser_t;
    ~help_parser_t() override;

    clapp::option::help_option_t help_option{
        *this, std::vector<std::string>{"h", "help"},
        std::vector<char>{'h', '?'}, "Print help and exit.",
        clapp::parser::types::purpose_t::optional};

    [[nodiscard]] std::string gen_short_line_prefix() const override;
};

help_parser_t::~help_parser_t() = default;

std::string help_parser_t::gen_short_line_prefix() const {
    return "help-parser " + gen_short_line();
}

class container_parser_t : public clapp::basic_parser_t {
   public:
    using clapp::basic_parser_t::basic_parser_t;
    ~container_parser_t() override;

    class container_t : public clapp::option_container_t {
       public:
        using clapp::option_container_t::option_container_t;
        ~container_t() override;

        static constexpr std::int32_t min_int{10};
        static constexpr std::int32_t max_int{200};

        clapp::option::bool_option_t bool_option{
            *this, "bool", 'b', "Bool option.",
            clapp::parser::types::purpose_t::mandatory};

        clapp::option::int32_param_option_t int_option{
            *this,
            "int",
            'i',
            "Int option.",
            clapp::value::min_max_value_t<std::int32_t>{min_int, max_int},
            clapp::parser::types::purpose_t::mandatory};
    };

    container_t container{
        *this, clapp::parser::types::logic_operator_type_t::logic_xor};

    [[nodiscard]] std::string gen_short_line_prefix() const override;
};

container_parser_t::container_t::~container_t() = default;

container_parser_t::~container_parser_t() = default;

std::string container_parser_t::gen_short_line_prefix() const {
    return "container-parser " + gen_short_line();
}

class nested_container_parser_t : public clapp::basic_parser_t {
   public:
    using clapp::basic_parser_t::basic_parser_t;
    ~nested_container_parser_t() override;

    class container_t : public clapp::option_container_t {
       public:
        using clapp::option_container_t::option_container_t;
        ~container_t() override;

        class test_container_t : public clapp::option_container_t {
           public:
            using clapp::option_container_t::option_container_t;
            ~test_container_t() override;

            clapp::option::int64_param_option_t int_option{
                *this, "int", 'i', "Int option.",
                clapp::parser::types::purpose_t::mandatory};

            clapp::option::uint64_param_option_t uint_option{
                *this, "uint", 'u', "Unsigned int option.",
                clapp::parser::types::purpose_t::mandatory};
        };

        class test_container2_t : public clapp::option_container_t {
           public:
            using clapp::option_container_t::option_container_t;
            ~test_container2_t() override;

            clapp::option::string_param_option_t string_option{
                *this, "str", 's', "String option.",
                clapp::parser::types::purpose_t::mandatory};

            // TODO: ensure that xor-containers don't contain optional options
            // TODO: change default purpose to mandatory (don't create a new
            // `auto`-purpose_t)

            clapp::option::bool_option_t bool_option{
                *this, "bool", 'b', "Bool option.",
                clapp::parser::types::purpose_t::mandatory};
        };

        test_container_t toc{
            *this, clapp::parser::types::logic_operator_type_t::logic_xor};

        test_container2_t toc2{
            *this, clapp::parser::types::logic_operator_type_t::logic_and};

        clapp::option::string_param_option_t cont_string_option{
            *this, "cstr", 'c', "Cont String option.",
            clapp::parser::types::purpose_t::mandatory};
    };

    container_t container{
        *this, clapp::parser::types::logic_operator_type_t::logic_xor};

    [[nodiscard]] std::string gen_short_line_prefix() const override;
};

nested_container_parser_t::container_t::test_container_t::~test_container_t() =
    default;

nested_container_parser_t::container_t::test_container2_t::
    ~test_container2_t() = default;

nested_container_parser_t::container_t::~container_t() = default;

nested_container_parser_t::~nested_container_parser_t() = default;

std::string nested_container_parser_t::gen_short_line_prefix() const {
    return "container-parser " + gen_short_line();
}

class nested_container_parser2_t : public clapp::basic_parser_t {
   public:
    using clapp::basic_parser_t::basic_parser_t;
    ~nested_container_parser2_t() override;

    class container_t : public clapp::option_container_t {
       public:
        using clapp::option_container_t::option_container_t;
        ~container_t() override;

        class test_container_t : public clapp::option_container_t {
           public:
            using clapp::option_container_t::option_container_t;
            ~test_container_t() override;

            clapp::option::int64_param_option_t int_option{
                *this, "int", 'i', "Int option.",
                clapp::parser::types::purpose_t::mandatory};

            clapp::option::uint64_param_option_t uint_option{
                *this, "uint", 'u', "Unsigned int option.",
                clapp::parser::types::purpose_t::mandatory};
        };

        class test_container2_t : public clapp::option_container_t {
           public:
            using clapp::option_container_t::option_container_t;
            ~test_container2_t() override;

            clapp::option::string_param_option_t string_option{
                *this, "str", 's', "String option.",
                clapp::parser::types::purpose_t::mandatory};
            // TODO: ensure that xor-containers don't contain optional options
            // TODO: change default purpose to mandatory (don't create a new
            // `auto`-purpose_t)

            clapp::option::bool_option_t bool_option{
                *this, "bool", 'b', "Bool option.",
                clapp::parser::types::purpose_t::mandatory};
        };

        test_container_t toc{
            *this, clapp::parser::types::logic_operator_type_t::logic_and};

        test_container2_t toc2{
            *this, clapp::parser::types::logic_operator_type_t::logic_xor};

        clapp::option::string_param_option_t cont_string_option{
            *this, "cstr", 'c', "Cont String option.",
            clapp::parser::types::purpose_t::mandatory};
    };

    container_t container{
        *this, clapp::parser::types::logic_operator_type_t::logic_and};

    [[nodiscard]] std::string gen_short_line_prefix() const override;
};

nested_container_parser2_t::container_t::test_container_t::~test_container_t() =
    default;

nested_container_parser2_t::container_t::test_container2_t::
    ~test_container2_t() = default;

nested_container_parser2_t::container_t::~container_t() = default;

nested_container_parser2_t::~nested_container_parser2_t() = default;

std::string nested_container_parser2_t::gen_short_line_prefix() const {
    return "container-parser " + gen_short_line();
}

class super_nested_container_parser_t : public clapp::basic_parser_t {
   public:
    using clapp::basic_parser_t::basic_parser_t;
    ~super_nested_container_parser_t() override;

    class container_t : public clapp::option_container_t {
       public:
        using clapp::option_container_t::option_container_t;
        ~container_t() override;

        class test_container_t : public clapp::option_container_t {
           public:
            using clapp::option_container_t::option_container_t;
            ~test_container_t() override;

            class test_sub_int_container_t : public clapp::option_container_t {
               public:
                using clapp::option_container_t::option_container_t;
                ~test_sub_int_container_t() override;

                clapp::option::int64_param_option_t sub_int_option{
                    *this, "sub-int", "Sub int option.",
                    clapp::parser::types::purpose_t::mandatory};

                clapp::option::int64_param_option_t sub_int_option2{
                    *this, "sub-int2", "Sub int option2.",
                    clapp::parser::types::purpose_t::mandatory};
            };

            class test_sub_uint_container_t : public clapp::option_container_t {
               public:
                using clapp::option_container_t::option_container_t;
                ~test_sub_uint_container_t() override;

                clapp::option::uint64_param_option_t sub_uint_option{
                    *this, "sub-uint", "Sub unsigned int option.",
                    clapp::parser::types::purpose_t::mandatory};

                clapp::option::uint64_param_option_t sub_uint_option2{
                    *this, "sub-uint2", "Sub unsigned int option2.",
                    clapp::parser::types::purpose_t::mandatory};
            };

            clapp::option::int64_param_option_t int_option{
                *this, "int", 'i', "Int option.",
                clapp::parser::types::purpose_t::mandatory};

            clapp::option::uint64_param_option_t uint_option{
                *this, "uint", 'u', "Unsigned int option.",
                clapp::parser::types::purpose_t::mandatory};

            test_sub_int_container_t tsic{
                *this, clapp::parser::types::logic_operator_type_t::logic_and};

            test_sub_uint_container_t tsuc{
                *this, clapp::parser::types::logic_operator_type_t::logic_xor};
        };

        class test_container2_t : public clapp::option_container_t {
           public:
            using clapp::option_container_t::option_container_t;
            ~test_container2_t() override;

            class test_int_container_t : public clapp::option_container_t {
               public:
                using clapp::option_container_t::option_container_t;
                ~test_int_container_t() override;

                clapp::option::int64_param_option_t int1{
                    *this, "int1", "Int option1.",
                    clapp::parser::types::purpose_t::mandatory};

                clapp::option::int64_param_option_t int2{
                    *this, "int2", "Int option2.",
                    clapp::parser::types::purpose_t::mandatory};
            };

            class test_uint_container_t : public clapp::option_container_t {
               public:
                using clapp::option_container_t::option_container_t;
                ~test_uint_container_t() override;

                clapp::option::uint64_param_option_t uint1{
                    *this, "uint1", "Unsigned int option1.",
                    clapp::parser::types::purpose_t::mandatory};

                clapp::option::uint64_param_option_t uint2{
                    *this, "uint2", "Unsigned int option2.",
                    clapp::parser::types::purpose_t::mandatory};
            };

            clapp::option::string_param_option_t string_option{
                *this, "str", 's', "String option.",
                clapp::parser::types::purpose_t::mandatory};

            clapp::option::bool_option_t bool_option{
                *this, "bool", 'b', "Bool option.",
                clapp::parser::types::purpose_t::mandatory};

            test_int_container_t tsic{
                *this, clapp::parser::types::logic_operator_type_t::logic_and};

            test_uint_container_t tsuc{
                *this, clapp::parser::types::logic_operator_type_t::logic_and};
        };

        test_container_t toc{
            *this, clapp::parser::types::logic_operator_type_t::logic_and};

        test_container2_t toc2{
            *this, clapp::parser::types::logic_operator_type_t::logic_xor};

        clapp::option::string_param_option_t cont_string_option{
            *this, "cstr", 'c', "Cont String option.",
            clapp::parser::types::purpose_t::mandatory};
    };

    container_t container{
        *this, clapp::parser::types::logic_operator_type_t::logic_xor};

    [[nodiscard]] std::string gen_short_line_prefix() const override;
};

super_nested_container_parser_t::container_t::test_container_t::
    ~test_container_t() = default;

super_nested_container_parser_t::container_t::test_container_t::
    test_sub_int_container_t::~test_sub_int_container_t() = default;

super_nested_container_parser_t::container_t::test_container_t::
    test_sub_uint_container_t::~test_sub_uint_container_t() = default;

super_nested_container_parser_t::container_t::test_container2_t::
    ~test_container2_t() = default;

super_nested_container_parser_t::container_t::test_container2_t::
    test_int_container_t::~test_int_container_t() = default;

super_nested_container_parser_t::container_t::test_container2_t::
    test_uint_container_t::~test_uint_container_t() = default;

super_nested_container_parser_t::container_t::~container_t() = default;

super_nested_container_parser_t::~super_nested_container_parser_t() = default;

std::string super_nested_container_parser_t::gen_short_line_prefix() const {
    return "container-parser " + gen_short_line();
}

class invalid_nested_container_parser_t : public clapp::basic_parser_t {
   public:
    using clapp::basic_parser_t::basic_parser_t;
    ~invalid_nested_container_parser_t() override;

    class container_t : public clapp::option_container_t {
       public:
        using clapp::option_container_t::option_container_t;
        ~container_t() override;

        class test_container_t : public clapp::option_container_t {
           public:
            using clapp::option_container_t::option_container_t;
            ~test_container_t() override;

            clapp::option::int64_param_option_t int_option{
                *this, "int", 'i', "Int option.",
                clapp::parser::types::purpose_t::mandatory};
        };

        class test_container2_t : public clapp::option_container_t {
           public:
            using clapp::option_container_t::option_container_t;
            ~test_container2_t() override;

            clapp::option::int64_param_option_t int_option{
                *this, "int", 'i', "Int option.",
                clapp::parser::types::purpose_t::mandatory};
        };

        test_container_t toc{
            *this, clapp::parser::types::logic_operator_type_t::logic_xor};

        test_container2_t toc2{
            *this, clapp::parser::types::logic_operator_type_t::logic_and};
    };

    container_t container{
        *this, clapp::parser::types::logic_operator_type_t::logic_xor};

    [[nodiscard]] std::string gen_short_line_prefix() const override;
};

invalid_nested_container_parser_t::container_t::test_container_t::
    ~test_container_t() = default;

invalid_nested_container_parser_t::container_t::test_container2_t::
    ~test_container2_t() = default;

invalid_nested_container_parser_t::container_t::~container_t() = default;

invalid_nested_container_parser_t::~invalid_nested_container_parser_t() =
    default;

std::string invalid_nested_container_parser_t::gen_short_line_prefix() const {
    return "container-parser " + gen_short_line();
}

class invalid_nested_container_parser2_t : public clapp::basic_parser_t {
   public:
    using clapp::basic_parser_t::basic_parser_t;
    ~invalid_nested_container_parser2_t() override;

    class test_container_t : public clapp::option_container_t {
       public:
        using clapp::option_container_t::option_container_t;
        ~test_container_t() override;

        clapp::option::int64_param_option_t int_option{
            *this, "int", 'i', "Int option.",
            clapp::parser::types::purpose_t::mandatory};
    };

    class test_container2_t : public clapp::option_container_t {
       public:
        using clapp::option_container_t::option_container_t;
        ~test_container2_t() override;

        clapp::option::int64_param_option_t int_option{
            *this, "int", 'i', "Int option.",
            clapp::parser::types::purpose_t::mandatory};
    };

    test_container_t toc{
        *this, clapp::parser::types::logic_operator_type_t::logic_xor};

    test_container2_t toc2{
        *this, clapp::parser::types::logic_operator_type_t::logic_and};

    [[nodiscard]] std::string gen_short_line_prefix() const override;
};

invalid_nested_container_parser2_t::test_container_t::~test_container_t() =
    default;

invalid_nested_container_parser2_t::test_container2_t::~test_container2_t() =
    default;

invalid_nested_container_parser2_t::~invalid_nested_container_parser2_t() =
    default;

std::string invalid_nested_container_parser2_t::gen_short_line_prefix() const {
    return "invalid-container-parser " + gen_short_line();
}

class invalid_nested_container_parser3_t : public clapp::basic_parser_t {
   public:
    using clapp::basic_parser_t::basic_parser_t;
    ~invalid_nested_container_parser3_t() override;

    class test_container_t : public clapp::option_container_t {
       public:
        using clapp::option_container_t::option_container_t;
        ~test_container_t() override;

        clapp::option::int64_param_option_t int_option{
            *this, "int", 'i', "Int option.",
            clapp::parser::types::purpose_t::mandatory};
    };

    test_container_t toc{
        *this, clapp::parser::types::logic_operator_type_t::logic_xor};

    clapp::option::int64_param_option_t int_option{
        *this, "int", 'i', "Int option.",
        clapp::parser::types::purpose_t::mandatory};

    [[nodiscard]] std::string gen_short_line_prefix() const override;
};

invalid_nested_container_parser3_t::test_container_t::~test_container_t() =
    default;

invalid_nested_container_parser3_t::~invalid_nested_container_parser3_t() =
    default;

std::string invalid_nested_container_parser3_t::gen_short_line_prefix() const {
    return "invalid-container-parser " + gen_short_line();
}

TEST(parser, constructEmptyBasicParserDoesNotThrow) {
    ASSERT_NO_THROW(empty_basic_parser_t{});
}

TEST(parser, constructEmptyBasicParserAndCallGetParser) {
    empty_basic_parser_t ebp;
    ASSERT_THAT(&(ebp.get_parser()), testing::Eq(&ebp));
}

TEST(parser, constructEmptyBasicParserAndCallGetConstParser) {
    empty_basic_parser_t ebp;
    ASSERT_THAT(&(ebp.get_const_parser()), testing::Eq(&ebp));
}

TEST(parser, constructEmptyBasicParserAndParseEmptyArguments) {
    constexpr const char* const argv[]{nullptr};
    const clapp::parser::types::arg_t arg{parser_make_arg_t(argv)};
    empty_basic_parser_t ebp;
    ASSERT_THAT(ebp.parse(arg.begin(), arg.end()).has_value(),
                testing::Eq(false));
}

TEST(parser, constructEmptyBasicParserAndParseAndValidateEmptyArguments) {
    constexpr const char* const argv[]{nullptr};
    const clapp::parser::types::arg_t arg{parser_make_arg_t(argv)};
    empty_basic_parser_t ebp;
    ASSERT_THAT(ebp.parse(arg.begin(), arg.end()).has_value(),
                testing::Eq(false));
    ASSERT_NO_THROW(ebp.validate());
}

TEST(parser, constructEmptyBasicParserAndGenUsagePrefix) {
    empty_basic_parser_t ebp;
    ASSERT_THAT(ebp.gen_usage_prefix(), testing::StrEq("Usage:\n"));
}

TEST(parser, constructEmptyBasicParserAndGenHelpMessage) {
    empty_basic_parser_t ebp;
    ASSERT_THAT(ebp.gen_help_msg(255), testing::StrEq("empty-basic-parser\n"));
}

TEST(parser, constructEmptyBasicParserIsActiveIsTrue) {
    empty_basic_parser_t ebp;
    ASSERT_THAT(ebp.is_active(), testing::Eq(true));
}

TEST(parser, constructEmptyBasicParserGetActiveReturnsThisRef) {
    empty_basic_parser_t ebp;
    ASSERT_THAT(&ebp.get_active_parser(), testing::Eq(&ebp));
}

TEST(parser, callDefaultPrintAndExitWithStringAndExitCode) {
    const std::string text{"text"};
    static constexpr int exit_code{2};
    testing::internal::CaptureStdout();
    const clapp::value::exit_t ret{
        empty_basic_parser_t::default_print_and_exit(text, exit_code)};
    const std::string output{testing::internal::GetCapturedStdout()};
    ASSERT_THAT(output, testing::StrEq(text));
    ASSERT_THAT(ret.get_exit_code(), ::testing::Eq(exit_code));
}

static constexpr const char help_str[] =
    "Usage:\nhelp-parser [-h|-?|--h|--help]\n\n  Options:\n    "
    "-h|-?|--h|--help Print help and exit. (optional)\n";

TEST(parser, constructHelpParserAndParseEmptyArguments) {
    constexpr const char* const argv[]{nullptr};
    const clapp::parser::types::arg_t arg{parser_make_arg_t(argv)};
    help_parser_t help_parser;
    ASSERT_THAT(help_parser.parse(arg.begin(), arg.end()).has_value(),
                testing::Eq(false));
}

TEST(parser, constructHelpParserAndParseHelpOptionLong1) {
    constexpr const char* const argv[]{"--help", nullptr};
    const clapp::parser::types::arg_t arg{parser_make_arg_t(argv)};
    help_parser_t help_parser;
    static constexpr int exit_code{EXIT_SUCCESS};
    testing::internal::CaptureStdout();
    const clapp::value::found_func_t::ret_t ret{
        help_parser.parse(arg.begin(), arg.end())};
    std::string output = testing::internal::GetCapturedStdout();
    ASSERT_THAT(output, testing::StrEq(help_str));
    ASSERT_THAT(ret.has_value(), testing::Eq(true));
    ASSERT_THAT(ret.value().get_exit_code(), testing::Eq(exit_code));
}

TEST(parser, constructHelpParserAndParseHelpOptionLong2) {
    constexpr const char* const argv[]{"--h", nullptr};
    const clapp::parser::types::arg_t arg{parser_make_arg_t(argv)};
    help_parser_t help_parser;
    static constexpr int exit_code{EXIT_SUCCESS};
    testing::internal::CaptureStdout();
    const clapp::value::found_func_t::ret_t ret{
        help_parser.parse(arg.begin(), arg.end())};
    std::string output = testing::internal::GetCapturedStdout();
    ASSERT_THAT(output, testing::StrEq(help_str));
    ASSERT_THAT(ret.has_value(), testing::Eq(true));
    ASSERT_THAT(ret.value().get_exit_code(), testing::Eq(exit_code));
}

TEST(parser, constructHelpParserAndParseHelpOptionShort1) {
    constexpr const char* const argv[]{"-h", nullptr};
    const clapp::parser::types::arg_t arg{parser_make_arg_t(argv)};
    help_parser_t help_parser;
    static constexpr int exit_code{EXIT_SUCCESS};
    testing::internal::CaptureStdout();
    const clapp::value::found_func_t::ret_t ret{
        help_parser.parse(arg.begin(), arg.end())};
    std::string output = testing::internal::GetCapturedStdout();
    ASSERT_THAT(output, testing::StrEq(help_str));
    ASSERT_THAT(ret.has_value(), testing::Eq(true));
    ASSERT_THAT(ret.value().get_exit_code(), testing::Eq(exit_code));
}

TEST(parser, constructHelpParserAndParseHelpOptionShort2) {
    constexpr const char* const argv[]{"-?", nullptr};
    const clapp::parser::types::arg_t arg{parser_make_arg_t(argv)};
    help_parser_t help_parser;
    static constexpr int exit_code{EXIT_SUCCESS};
    testing::internal::CaptureStdout();
    const clapp::value::found_func_t::ret_t ret{
        help_parser.parse(arg.begin(), arg.end())};
    std::string output = testing::internal::GetCapturedStdout();
    ASSERT_THAT(output, testing::StrEq(help_str));
    ASSERT_THAT(ret.has_value(), testing::Eq(true));
    ASSERT_THAT(ret.value().get_exit_code(), testing::Eq(exit_code));
}

TEST(parser, constructSimpleTestParser4AndParseArgument) {
    constexpr const char* const argv[]{"myarg", nullptr};
    const clapp::parser::types::arg_t arg{parser_make_arg_t(argv)};
    simple_test_parser4_t stp;
    static constexpr int exit_code{EXIT_SUCCESS};
    testing::internal::CaptureStdout();
    const clapp::value::found_func_t::ret_t ret{
        stp.parse(arg.begin(), arg.end())};
    std::string output{testing::internal::GetCapturedStdout()};
    ASSERT_THAT(output, testing::StrEq("found cb: arg-name\n"));
    ASSERT_THAT(ret.has_value(), testing::Eq(true));
    ASSERT_THAT(ret.value().get_exit_code(), testing::Eq(exit_code));
}

TEST(parser, constructSimpleTestParser4AndParseLongOpt) {
    constexpr const char* const argv[]{"--opt", "string", nullptr};
    const clapp::parser::types::arg_t arg{parser_make_arg_t(argv)};
    simple_test_parser4_t stp;
    static constexpr int exit_code{EXIT_SUCCESS};
    testing::internal::CaptureStdout();
    const clapp::value::found_func_t::ret_t ret{
        stp.parse(arg.begin(), arg.end())};
    std::string output{testing::internal::GetCapturedStdout()};
    ASSERT_THAT(output, testing::StrEq("found cb: --opt\n"));
    ASSERT_THAT(ret.has_value(), testing::Eq(true));
    ASSERT_THAT(ret.value().get_exit_code(), testing::Eq(exit_code));
}

TEST(parser, constructSimpleTestParser4AndParseShortOpt) {
    constexpr const char* const argv[]{"-o", "string", nullptr};
    const clapp::parser::types::arg_t arg{parser_make_arg_t(argv)};
    simple_test_parser4_t stp;
    static constexpr int exit_code{EXIT_SUCCESS};
    testing::internal::CaptureStdout();
    const clapp::value::found_func_t::ret_t ret{
        stp.parse(arg.begin(), arg.end())};
    std::string output{testing::internal::GetCapturedStdout()};
    ASSERT_THAT(output, testing::StrEq("found cb: -o\n"));
    ASSERT_THAT(ret.has_value(), testing::Eq(true));
    ASSERT_THAT(ret.value().get_exit_code(), testing::Eq(exit_code));
}

TEST(parser, constructSimpleTestParser4AndParseShortOpt2) {
    constexpr const char* const argv[]{"-bo", "string", nullptr};
    const clapp::parser::types::arg_t arg{parser_make_arg_t(argv)};
    simple_test_parser4_t stp;
    static constexpr int exit_code{EXIT_SUCCESS};
    testing::internal::CaptureStdout();
    const clapp::value::found_func_t::ret_t ret{
        stp.parse(arg.begin(), arg.end())};
    std::string output{testing::internal::GetCapturedStdout()};
    ASSERT_THAT(output, testing::StrEq("found cb: -b\n"));
    ASSERT_THAT(ret.has_value(), testing::Eq(true));
    ASSERT_THAT(ret.value().get_exit_code(), testing::Eq(exit_code));
}

TEST(parser, constructSimpleTestParser5AndParseArgument) {
    constexpr const char* const argv[]{"myarg", "myarg2", "myarg3", nullptr};
    const clapp::parser::types::arg_t arg{parser_make_arg_t(argv)};
    simple_test_parser5_t stp;
    static constexpr int exit_code{EXIT_FAILURE};
    testing::internal::CaptureStdout();
    const clapp::value::found_func_t::ret_t ret{
        stp.parse(arg.begin(), arg.end())};
    std::string output{testing::internal::GetCapturedStdout()};
    ASSERT_THAT(
        output,
        testing::StrEq("found cb: var-arg-name\nfound cb: var-arg-name\n"));
    ASSERT_THAT(ret.has_value(), testing::Eq(true));
    ASSERT_THAT(ret.value().get_exit_code(), testing::Eq(exit_code));
}

TEST(parser,
     constructMandatoryOptionsTestParserLogicAndCallParseAndValidateRecursive) {
    constexpr const char* const argv[]{"-b", "-s=test", "-i=21", nullptr};
    const clapp::parser::types::arg_t arg{parser_make_arg_t(argv)};
    mandatory_options_test_parser_t motp{
        clapp::parser::types::logic_operator_type_t::logic_and};
    const clapp::value::found_func_t::ret_t ret{
        motp.parse(arg.begin(), arg.end())};
    ASSERT_THAT(ret.has_value(), testing::Eq(false));
    ASSERT_NO_THROW(motp.validate_recursive());
}

TEST(
    parser,
    constructMandatoryOptionsTestParserLogicXorCallParseBoolAndValidateRecursive) {
    constexpr const char* const argv[]{"-b", nullptr};
    const clapp::parser::types::arg_t arg{parser_make_arg_t(argv)};
    mandatory_options_test_parser_t motp{
        clapp::parser::types::logic_operator_type_t::logic_xor};
    const clapp::value::found_func_t::ret_t ret{
        motp.parse(arg.begin(), arg.end())};
    ASSERT_THAT(ret.has_value(), testing::Eq(false));
    ASSERT_NO_THROW(motp.validate_recursive());
}

TEST(
    parser,
    constructMandatoryOptionsTestParserLogicXorCallParseStrAndValidateRecursive) {
    constexpr const char* const argv[]{"-s=test", nullptr};
    const clapp::parser::types::arg_t arg{parser_make_arg_t(argv)};
    mandatory_options_test_parser_t motp{
        clapp::parser::types::logic_operator_type_t::logic_xor};
    const clapp::value::found_func_t::ret_t ret{
        motp.parse(arg.begin(), arg.end())};
    ASSERT_THAT(ret.has_value(), testing::Eq(false));
    ASSERT_NO_THROW(motp.validate_recursive());
}

TEST(
    parser,
    constructMandatoryOptionsTestParserLogicXorCallParseIntAndValidateRecursive) {
    constexpr const char* const argv[]{"-i=11", nullptr};
    const clapp::parser::types::arg_t arg{parser_make_arg_t(argv)};
    mandatory_options_test_parser_t motp{
        clapp::parser::types::logic_operator_type_t::logic_xor};
    const clapp::value::found_func_t::ret_t ret{
        motp.parse(arg.begin(), arg.end())};
    ASSERT_THAT(ret.has_value(), testing::Eq(false));
    ASSERT_NO_THROW(motp.validate_recursive());
}

TEST(parser, setAndCallPrintAndExitWithStringAndExitCode) {
    print_and_exit_t pae{};
    simple_test_parser_t stp;
    stp.set_print_and_exit_func(
        [&pae](const std::string_view text, const int exit_code) {
            return pae.print_and_exit(text, exit_code);
        });

    static constexpr std::string_view text{"text-string"};
    static constexpr int exit_code{10};
    static constexpr clapp::value::exit_t stop{
        clapp::value::exit_t::exit(exit_code)};
    EXPECT_CALL(pae, print_and_exit(text, exit_code))
        .Times(1)
        .WillOnce(testing::Return(stop));
    const clapp::value::exit_t ret{
        stp.get_print_and_exit_func()(text, exit_code)};
    ASSERT_THAT(ret.get_exit_code(), testing::Eq(exit_code));
}

TEST(parser, genFuncPrintHelpAndExit) {
    empty_basic_parser_t ebp;
    static constexpr int ret_val{EXIT_SUCCESS};
    clapp::value::found_func_t found_func{
        ebp.gen_func_print_help_and_req_exit(ret_val)};
    testing::internal::CaptureStdout();
    const clapp::value::found_func_t::ret_t ret{found_func.found("name")};
    ASSERT_THAT(ret.has_value(), testing::Eq(true));
    ASSERT_THAT(ret.value().get_exit_code(), testing::Eq(ret_val));
    std::string output{testing::internal::GetCapturedStdout()};
    ASSERT_THAT(output, testing::StrEq("Usage:\nempty-basic-parser\n"));
}

TEST(parser, constructSimpleTestParserAndGenHelpMessage) {
    simple_test_parser_t stp;
    ASSERT_THAT(
        stp.gen_help_msg(255),
        testing::StrEq(
            "simple-test-parser [-b|--bool] [-i|--int=<param>] <arg-name> "
            "[<variadic-arg-name>...]\n\n  Arguments:\n    "
            "arg-name          Arg desc (mandatory)\n    "
            "variadic-arg-name Variadic arg desc (optional, variadic "
            "argument)\n\n  Options:\n    -b|--bool         "
            "Bool option. (optional)\n    -i|--int=<param>  Int option. "
            "(optional, constraint: [10,200])\n"));
}

TEST(parser, constructSimpleTestParser2AndGenHelpMessage) {
    simple_test_parser2_t stp;
    ASSERT_THAT(
        stp.gen_help_msg(255),
        testing::StrEq("simple-test-parser2 -c|--count [<arg-name>]\n\n  "
                       "Arguments:\n    arg-name   Arg desc (optional)\n\n  "
                       "Options:\n    -c|--count Count option. (mandatory)\n"));
}

TEST(parser, constructSimpleTestParser3AndGenHelpMessage) {
    simple_test_parser3_t stp;
    ASSERT_THAT(
        stp.gen_help_msg(255),
        testing::StrEq(
            "simple-test-parser3 -i|--int=<param>... [-s|-o|--str=<param>...] "
            "<variadic-arg-name>...\n\n  Arguments:\n    "
            "variadic-arg-name   Variadic arg desc (mandatory, variadic "
            "argument)\n\n  Options:\n    -i|--int=<param>    Int option. "
            "(mandatory, vector option, constraint: [10,200])\n  "
            "  -s|-o|--str=<param> String "
            "option. (optional, vector option)\n"));
}

TEST(parser, constructSimpleTestParser4AndGenHelpMessage) {
    simple_test_parser4_t stp;
    ASSERT_THAT(
        stp.gen_help_msg(255),
        testing::StrEq(
            "simple-test-parser4 [-b|--bo] [-o|--opt=<param>] [<arg-name>]\n\n "
            " Arguments:\n    arg-name         Arg desc (optional)\n\n  "
            "Options:\n    -b|--bo          Arg desc (optional)\n    "
            "-o|--opt=<param> Arg desc (optional)\n"));
}

TEST(parser, constructSimpleTestParser5AndGenHelpMessage) {
    simple_test_parser5_t stp;
    ASSERT_THAT(
        stp.gen_help_msg(255),
        testing::StrEq(
            "simple-test-parser5 <arg-name> [<var-arg-name>...]\n\n  "
            "Arguments:\n    arg-name     Arg desc (mandatory)\n    "
            "var-arg-name Var arg desc (optional, variadic argument)\n"));
}

TEST(parser, constructMandatoryOptionsTestParserLogicAndAndGenHelpMessage) {
    mandatory_options_test_parser_t motp{
        clapp::parser::types::logic_operator_type_t::logic_and};
    ASSERT_THAT(
        motp.gen_help_msg(255),
        testing::StrEq("mandatory-options-test-parser -b|--bool "
                       "-s|--string=<param> -i|--int=<param>\n\n  "
                       "Options:\n    -b|--bool           Bool option. "
                       "(mandatory)\n    -s|--string=<param> String option. "
                       "(mandatory)\n    -i|--int=<param>    Int option. "
                       "(mandatory, constraint: [10,200])\n"));
}

TEST(parser, constructMandatoryOptionsTestParserLogicAndXorGenHelpMessage) {
    mandatory_options_test_parser_t motp{
        clapp::parser::types::logic_operator_type_t::logic_xor};
    ASSERT_THAT(
        motp.gen_help_msg(255),
        testing::StrEq("mandatory-options-test-parser -b|--bool | "
                       "-s|--string=<param> | -i|--int=<param>\n\n  "
                       "Options:\n    -b|--bool           Bool option. "
                       "(mandatory)\n    -s|--string=<param> String option. "
                       "(mandatory)\n    -i|--int=<param>    Int option. "
                       "(mandatory, constraint: [10,200])\n"));
}

TEST(parser, constructSubParserContainerAndGenHelpMessage) {
    sub_parser_container_t spc;
    ASSERT_THAT(
        spc.gen_help_msg(255),
        testing::StrEq(
            "sub_parser_container [-b|--bool] [-2|--second] <arg-name> "
            "sub-parser [-b|--bool] [-s|--string=<param>] [<sub-arg-name>]\n"
            "sub_parser_container [-b|--bool] [-2|--second] <arg-name>\n\n"
            "  Arguments:\n    arg-name    "
            "Arg desc (mandatory)\n\n  Options:\n    -b|--bool   "
            "Bool option. (optional)\n    -2|--second Second bool "
            "option. (optional)\n\n"
            "  Subparser:\n    sub-parser  Sub "
            "parser desc\n      Arguments:\n        sub-arg-name      "
            "  Sub arg desc (optional)\n\n      Options:\n        -b|--bool    "
            "       Bool option. (optional)\n        -s|--string=<param> "
            "String "
            "option. (optional)\n\n"));
}

TEST(parser, constructSubParserContainerAndGenSubParserHelpMessage) {
    sub_parser_container_t spc;
    ASSERT_THAT(
        spc.sub_parser.gen_help_msg(255),
        testing::StrEq(
            "sub_parser_container [-b|--bool] [-2|--second] <arg-name> "
            "sub-parser [-b|--bool] [-s|--string=<param>] [<sub-arg-name>]\n\n "
            " "
            "Arguments:\n    sub-arg-name        Sub arg "
            "desc (optional)\n\n  Options:\n    -b|--bool           "
            "Bool option. (optional)\n    -s|--string=<param> String "
            "option. (optional)\n"));
}

TEST(parser, constructEmptyBasicParserAndParseUnknownArgumentsThrows) {
    constexpr const char* const argv[]{"unknown-argument", nullptr};
    const clapp::parser::types::arg_t arg{parser_make_arg_t(argv)};
    empty_basic_parser_t ebp;
    ASSERT_THROW(static_cast<void>(ebp.parse(arg.begin(), arg.end())),
                 clapp::exception::clapp_exception_t);
}

TEST(parser, constructEmptyBasicParserAndParseUnknownLongOptionThrows) {
    constexpr const char* const argv[]{"--long-option", nullptr};
    const clapp::parser::types::arg_t arg{parser_make_arg_t(argv)};
    empty_basic_parser_t ebp;
    ASSERT_THROW(static_cast<void>(ebp.parse(arg.begin(), arg.end())),
                 clapp::exception::option_exception_t);
}

TEST(parser, constructEmptyBasicParserAndParseUnknownShortOptionThrows) {
    constexpr const char* const argv[]{"-s", nullptr};
    const clapp::parser::types::arg_t arg{parser_make_arg_t(argv)};
    empty_basic_parser_t ebp;
    ASSERT_THROW(static_cast<void>(ebp.parse(arg.begin(), arg.end())),
                 clapp::exception::option_exception_t);
}

TEST(parser, constructSimpleTestParserAndParseUnknownLongOptionThrows) {
    constexpr const char* const argv[]{"--long-option", nullptr};
    const clapp::parser::types::arg_t arg{parser_make_arg_t(argv)};
    simple_test_parser_t stp;
    ASSERT_THROW(static_cast<void>(stp.parse(arg.begin(), arg.end())),
                 clapp::exception::option_exception_t);
}

TEST(parser, constructSimpleTestParserAndParseUnknownShortOptionThrows) {
    constexpr const char* const argv[]{"-s", nullptr};
    const clapp::parser::types::arg_t arg{parser_make_arg_t(argv)};
    simple_test_parser_t stp;
    ASSERT_THROW(static_cast<void>(stp.parse(arg.begin(), arg.end())),
                 clapp::exception::option_exception_t);
}

TEST(parser, constructSimpleTestParserAndParseLongBoolOptionWithParamThrows) {
    constexpr const char* const argv[]{"--bool=param", nullptr};
    const clapp::parser::types::arg_t arg{parser_make_arg_t(argv)};
    simple_test_parser_t stp;
    ASSERT_THROW(static_cast<void>(stp.parse(arg.begin(), arg.end())),
                 clapp::exception::option_param_exception_t);
}

TEST(parser, constructSimpleTestParserAndParseShortOptionWithParamThrows) {
    constexpr const char* const argv[]{"-b=param", nullptr};
    const clapp::parser::types::arg_t arg{parser_make_arg_t(argv)};
    simple_test_parser_t stp;
    ASSERT_THROW(static_cast<void>(stp.parse(arg.begin(), arg.end())),
                 clapp::exception::option_param_exception_t);
}

TEST(parser, constructSimpleTestParserAndParseLongIntOptionWithoutParamThrows) {
    constexpr const char* const argv[]{"--int", nullptr};
    const clapp::parser::types::arg_t arg{parser_make_arg_t(argv)};
    simple_test_parser_t stp;
    ASSERT_THROW(static_cast<void>(stp.parse(arg.begin(), arg.end())),
                 clapp::exception::option_param_exception_t);
}

TEST(parser,
     constructSimpleTestParserAndParseShortIntOptionWithoutParamThrows) {
    constexpr const char* const argv[]{"-i", nullptr};
    const clapp::parser::types::arg_t arg{parser_make_arg_t(argv)};
    simple_test_parser_t stp;
    ASSERT_THROW(static_cast<void>(stp.parse(arg.begin(), arg.end())),
                 clapp::exception::option_param_exception_t);
}

TEST(parser,
     constructSimpleTestParserAndParseShortIntOptionWithoutParamThrows2) {
    constexpr const char* const argv[]{"-ib", nullptr};
    const clapp::parser::types::arg_t arg{parser_make_arg_t(argv)};
    simple_test_parser_t stp;
    ASSERT_THROW(static_cast<void>(stp.parse(arg.begin(), arg.end())),
                 clapp::exception::option_param_exception_t);
}

TEST(parser,
     constructSimpleTestParserParseWithoutArgumentAndValidateRecursiveThrows) {
    constexpr const char* const argv[]{nullptr};
    const clapp::parser::types::arg_t arg{parser_make_arg_t(argv)};
    simple_test_parser_t stp;
    ASSERT_THAT(stp.parse(arg.begin(), arg.end()).has_value(),
                testing::Eq(false));

    ASSERT_THAT(stp.int_option, ParamOptionNotGiven());
    ASSERT_THAT(stp.bool_option, BoolOptionNotGiven());
    ASSERT_THAT(stp.string_arg, ArgumentNotGiven());
    ASSERT_THAT(stp.variadic_string_arg, VariadicArgumentNotGiven());

    ASSERT_THAT(stp.get_num_processed_arguments(), testing::Eq(0));

    ASSERT_THROW(stp.validate_recursive(),
                 clapp::exception::argument_exception_t);
}

TEST(parser, constructSimpleTestParserParseArgumentAndValidateRecursive) {
    constexpr const char* const argv[]{"argument", nullptr};
    const clapp::parser::types::arg_t arg{parser_make_arg_t(argv)};
    simple_test_parser_t stp;
    ASSERT_THAT(stp.parse(arg.begin(), arg.end()).has_value(),
                testing::Eq(false));

    ASSERT_THAT(stp.int_option, ParamOptionNotGiven());
    ASSERT_THAT(stp.bool_option, BoolOptionNotGiven());
    ASSERT_THAT(stp.string_arg, ArgumentGiven("argument"));
    ASSERT_THAT(stp.variadic_string_arg, VariadicArgumentNotGiven());

    ASSERT_THAT(stp.get_num_processed_arguments(), testing::Eq(1));

    ASSERT_NO_THROW(stp.validate_recursive());
}

TEST(
    parser,
    constructSimpleTestParserParseArgumentAndShortOptionWithoutParamsAndValidateRecursive) {
    constexpr const char* const argv[]{"-b", "arg", nullptr};
    const clapp::parser::types::arg_t arg{parser_make_arg_t(argv)};
    simple_test_parser_t stp;
    ASSERT_THAT(stp.parse(arg.begin(), arg.end()).has_value(),
                testing::Eq(false));

    ASSERT_THAT(stp.int_option, ParamOptionNotGiven());
    ASSERT_THAT(stp.bool_option, BoolOptionGiven());
    ASSERT_THAT(stp.string_arg, ArgumentGiven("arg"));
    ASSERT_THAT(stp.variadic_string_arg, VariadicArgumentNotGiven());

    ASSERT_THAT(stp.get_num_processed_arguments(), testing::Eq(1));

    ASSERT_NO_THROW(stp.validate_recursive());
}

TEST(parser,
     constructSimpleTestParserParseArgumentAndLongOptionWithParamAndValidate) {
    constexpr const char* const argv[]{"--int", "123", "arg", nullptr};
    const clapp::parser::types::arg_t arg{parser_make_arg_t(argv)};
    simple_test_parser_t stp;
    ASSERT_THAT(stp.parse(arg.begin(), arg.end()).has_value(),
                testing::Eq(false));

    ASSERT_THAT(stp.int_option, ParamOptionGiven(123));
    ASSERT_THAT(stp.bool_option, BoolOptionNotGiven());
    ASSERT_THAT(stp.string_arg, ArgumentGiven("arg"));
    ASSERT_THAT(stp.variadic_string_arg, VariadicArgumentNotGiven());

    ASSERT_THAT(stp.get_num_processed_arguments(), testing::Eq(1));

    ASSERT_NO_THROW(stp.validate_recursive());
}

TEST(
    parser,
    constructSimpleTestParserParseArgumentVariadicArgumentAndLongOptionWithParamAndValidate) {
    constexpr const char* const argv[]{"--int=123", "arg", "varg0", "varg1",
                                       nullptr};
    const clapp::parser::types::arg_t arg{parser_make_arg_t(argv)};
    simple_test_parser_t stp;
    ASSERT_THAT(stp.parse(arg.begin(), arg.end()).has_value(),
                testing::Eq(false));

    ASSERT_THAT(stp.int_option, ParamOptionGiven(123));
    ASSERT_THAT(stp.bool_option, BoolOptionNotGiven());
    ASSERT_THAT(stp.string_arg, ArgumentGiven("arg"));
    ASSERT_THAT(
        stp.variadic_string_arg,
        VariadicArgumentGiven(std::vector<std::string>{"varg0", "varg1"}));

    ASSERT_THAT(stp.get_num_processed_arguments(), testing::Eq(3));

    ASSERT_NO_THROW(stp.validate_recursive());
}

TEST(parser, constructSimpleTestParserParseArgumentAndShortOptionsAndValidate) {
    constexpr const char* const argv[]{"-bi=123", "arg", nullptr};
    const clapp::parser::types::arg_t arg{parser_make_arg_t(argv)};
    simple_test_parser_t stp;
    ASSERT_THAT(stp.parse(arg.begin(), arg.end()).has_value(),
                testing::Eq(false));

    ASSERT_THAT(stp.int_option, ParamOptionGiven(123));
    ASSERT_THAT(stp.bool_option, BoolOptionGiven());
    ASSERT_THAT(stp.string_arg, ArgumentGiven("arg"));
    ASSERT_THAT(stp.variadic_string_arg, VariadicArgumentNotGiven());

    ASSERT_THAT(stp.get_num_processed_arguments(), testing::Eq(1));

    ASSERT_NO_THROW(stp.validate_recursive());
}

TEST(parser, constructSimpleTestParser2AndParseLongCountOptionWithParamThrows) {
    constexpr const char* const argv[]{"--count=param", nullptr};
    const clapp::parser::types::arg_t arg{parser_make_arg_t(argv)};
    simple_test_parser2_t stp;

    ASSERT_THAT(stp.count_option, NumCountOptionNotGiven());
    ASSERT_THAT(stp.string_arg, ArgumentNotGiven());

    ASSERT_THROW(static_cast<void>(stp.parse(arg.begin(), arg.end())),
                 clapp::option_param_exception_t);
}

TEST(parser, constructSimpleTestParser2ParseWithoutMandatoryOptionThrows) {
    constexpr const char* const argv[]{nullptr};
    const clapp::parser::types::arg_t arg{parser_make_arg_t(argv)};
    simple_test_parser2_t stp;
    ASSERT_THAT(stp.parse(arg.begin(), arg.end()).has_value(),
                testing::Eq(false));

    ASSERT_THAT(stp.count_option, NumCountOptionNotGiven());
    ASSERT_THAT(stp.string_arg, ArgumentNotGiven());

    ASSERT_THROW(stp.validate(), clapp::option_param_exception_t);
}

TEST(parser, constructSimpleTestParser2ParseOptionWithoutParamAndValidate) {
    constexpr const char* const argv[]{"-c", nullptr};
    const clapp::parser::types::arg_t arg{parser_make_arg_t(argv)};
    simple_test_parser2_t stp;
    ASSERT_THAT(stp.parse(arg.begin(), arg.end()).has_value(),
                testing::Eq(false));

    ASSERT_THAT(stp.count_option, NumCountOptionGiven(1U));
    ASSERT_THAT(stp.string_arg, ArgumentNotGiven());

    ASSERT_NO_THROW(stp.validate());
}

TEST(
    parser,
    constructSimpleTestParser2ParseOptionWithoutParamOptionalArgumentAndValidate) {
    constexpr const char* const argv[]{"-c", "opt-arg", nullptr};
    const clapp::parser::types::arg_t arg{parser_make_arg_t(argv)};
    simple_test_parser2_t stp;
    ASSERT_THAT(stp.parse(arg.begin(), arg.end()).has_value(),
                testing::Eq(false));

    ASSERT_THAT(stp.count_option, NumCountOptionGiven(1U));
    ASSERT_THAT(stp.string_arg, ArgumentGiven("opt-arg"));

    ASSERT_NO_THROW(stp.validate());
}

TEST(parser, constructSubParserContainerParseSubparserAndValidate) {
    constexpr const char* const argv[]{"string-arg", "sub-parser", "-bs=param",
                                       nullptr};
    const clapp::parser::types::arg_t arg{parser_make_arg_t(argv)};
    sub_parser_container_t spc;
    ASSERT_THAT(spc.parse(arg.begin(), arg.end()).has_value(),
                testing::Eq(false));

    ASSERT_THAT(spc.string_arg, ArgumentGiven("string-arg"));
    ASSERT_THAT(spc.bool_option, BoolOptionNotGiven());
    ASSERT_THAT(spc.bool_option2, BoolOptionNotGiven());
    ASSERT_THAT(spc.sub_parser.bool_option, BoolOptionGiven());
    ASSERT_THAT(spc.sub_parser.string_option, ParamOptionGiven("param"));

    ASSERT_NO_THROW(spc.validate());
}

TEST(parser, constructSubParserContainerParseSubparserAndValidate2) {
    constexpr const char* const argv[]{
        "string-arg", "-b", "-2", "sub-parser", "-s", "param", nullptr};
    const clapp::parser::types::arg_t arg{parser_make_arg_t(argv)};
    sub_parser_container_t spc;
    ASSERT_THAT(spc.parse(arg.begin(), arg.end()).has_value(),
                testing::Eq(false));

    ASSERT_THAT(spc.string_arg, ArgumentGiven("string-arg"));
    ASSERT_THAT(spc.bool_option, BoolOptionGiven());
    ASSERT_THAT(spc.bool_option2, BoolOptionGiven());
    ASSERT_THAT(spc.sub_parser.bool_option, BoolOptionNotGiven());
    ASSERT_THAT(spc.sub_parser.string_option, ParamOptionGiven("param"));

    ASSERT_NO_THROW(spc.validate());
}

TEST(parser, constructContainerParserAndGenHelpMessage) {
    container_parser_t parser;
    ASSERT_THAT(
        parser.gen_help_msg(255),
        testing::StrEq("container-parser ( -b|--bool | -i|--int=<param> )\n\n "
                       " Options:\n    -b|--bool        Bool option. "
                       "(mandatory)\n    -i|--int=<param> Int option. "
                       "(mandatory, constraint: [10,200])\n"));
}

TEST(parser, constructContainerParserAndParseBoolOptionDoesNotThrow) {
    constexpr const char* const argv[]{"-b", nullptr};
    const clapp::parser::types::arg_t arg{parser_make_arg_t(argv)};
    container_parser_t parser;
    ASSERT_THAT(parser.parse(arg.begin(), arg.end()).has_value(),
                testing::Eq(false));

    ASSERT_THAT(parser.container.bool_option, BoolOptionGiven());
    ASSERT_THAT(parser.container.int_option, ParamOptionNotGiven());

    ASSERT_NO_THROW(parser.validate_recursive());
}

TEST(parser, constructContainerParserAndParseIntOptionDoesNotThrow) {
    constexpr const char* const argv[]{"-i", "56", nullptr};
    const clapp::parser::types::arg_t arg{parser_make_arg_t(argv)};
    container_parser_t parser;
    ASSERT_THAT(parser.parse(arg.begin(), arg.end()).has_value(),
                testing::Eq(false));

    ASSERT_THAT(parser.container.bool_option, BoolOptionNotGiven());
    ASSERT_THAT(parser.container.int_option, ParamOptionGiven(56));

    ASSERT_NO_THROW(parser.validate_recursive());
}

TEST(parser, constructContainerParserAndParseEmptyOptionThrows) {
    constexpr const char* const argv[]{nullptr};
    const clapp::parser::types::arg_t arg{parser_make_arg_t(argv)};
    container_parser_t parser;
    ASSERT_THAT(parser.parse(arg.begin(), arg.end()).has_value(),
                testing::Eq(false));

    ASSERT_THAT(parser.container.bool_option, BoolOptionNotGiven());
    ASSERT_THAT(parser.container.int_option, ParamOptionNotGiven());

    ASSERT_THROW(parser.validate_recursive(),
                 clapp::exception::option_param_exception_t);
}

TEST(parser, constructNestedContainerParserAndGenHelpMessage) {
    nested_container_parser_t ncp;
    ASSERT_THAT(
        ncp.gen_help_msg(255),
        testing::StrEq(
            "container-parser ( -c|--cstr=<param> | ( -i|--int=<param> | "
            "-u|--uint=<param> ) | ( -s|--str=<param> -b|--bool ) )\n\n  "
            "Options:\n    -c|--cstr=<param> Cont String option. (mandatory)\n "
            "   -i|--int=<param>  Int option. (mandatory)\n    "
            "-u|--uint=<param> Unsigned int option. (mandatory)\n    "
            "-s|--str=<param>  String option. (mandatory)\n    -b|--bool       "
            "  Bool option. (mandatory)\n"));
}

TEST(parser,
     constructNestedContainerParserAndParseEmptyOptionValidateRecursiveThrows) {
    constexpr const char* const argv[]{nullptr};
    const clapp::parser::types::arg_t arg{parser_make_arg_t(argv)};
    nested_container_parser_t ncp;
    ASSERT_THAT(ncp.parse(arg.begin(), arg.end()).has_value(),
                testing::Eq(false));

    ASSERT_THAT(ncp.container.cont_string_option, ParamOptionNotGiven());
    ASSERT_THAT(ncp.container.toc.int_option, ParamOptionNotGiven());
    ASSERT_THAT(ncp.container.toc.uint_option, ParamOptionNotGiven());
    ASSERT_THAT(ncp.container.toc2.string_option, ParamOptionNotGiven());
    ASSERT_THAT(ncp.container.toc2.bool_option, BoolOptionNotGiven());

    ASSERT_THROW(ncp.validate_recursive(),
                 clapp::exception::option_param_exception_t);
}

TEST(
    parser,
    constructNestedContainerParserAndParseCStrOptionValidateRecursiveDoesNotThrow) {
    constexpr const char* const argv[]{"-c", "test", nullptr};
    const clapp::parser::types::arg_t arg{parser_make_arg_t(argv)};
    nested_container_parser_t ncp;
    ASSERT_THAT(ncp.parse(arg.begin(), arg.end()).has_value(),
                testing::Eq(false));

    ASSERT_THAT(ncp.container.cont_string_option, ParamOptionGiven("test"));
    ASSERT_THAT(ncp.container.toc.int_option, ParamOptionNotGiven());
    ASSERT_THAT(ncp.container.toc.uint_option, ParamOptionNotGiven());
    ASSERT_THAT(ncp.container.toc2.string_option, ParamOptionNotGiven());
    ASSERT_THAT(ncp.container.toc2.bool_option, BoolOptionNotGiven());

    ASSERT_NO_THROW(ncp.validate_recursive());
}

TEST(
    parser,
    constructNestedContainerParserAndParseIntOptionValidateRecursiveDoesNotThrow) {
    constexpr const char* const argv[]{"-i", "123", nullptr};
    const clapp::parser::types::arg_t arg{parser_make_arg_t(argv)};
    nested_container_parser_t ncp;
    ASSERT_THAT(ncp.parse(arg.begin(), arg.end()).has_value(),
                testing::Eq(false));

    ASSERT_THAT(ncp.container.cont_string_option, ParamOptionNotGiven());
    ASSERT_THAT(ncp.container.toc.int_option, ParamOptionGiven(123));
    ASSERT_THAT(ncp.container.toc.uint_option, ParamOptionNotGiven());
    ASSERT_THAT(ncp.container.toc2.string_option, ParamOptionNotGiven());
    ASSERT_THAT(ncp.container.toc2.bool_option, BoolOptionNotGiven());

    ASSERT_NO_THROW(ncp.validate_recursive());
}

TEST(
    parser,
    constructNestedContainerParserAndParseUIntOptionValidateRecursiveDoesNotThrow) {
    constexpr const char* const argv[]{"-u", "123", nullptr};
    const clapp::parser::types::arg_t arg{parser_make_arg_t(argv)};
    nested_container_parser_t ncp;
    ASSERT_THAT(ncp.parse(arg.begin(), arg.end()).has_value(),
                testing::Eq(false));

    ASSERT_THAT(ncp.container.cont_string_option, ParamOptionNotGiven());
    ASSERT_THAT(ncp.container.toc.int_option, ParamOptionNotGiven());
    ASSERT_THAT(ncp.container.toc.uint_option, ParamOptionGiven(123U));
    ASSERT_THAT(ncp.container.toc2.string_option, ParamOptionNotGiven());
    ASSERT_THAT(ncp.container.toc2.bool_option, BoolOptionNotGiven());

    ASSERT_NO_THROW(ncp.validate_recursive());
}

TEST(
    parser,
    constructNestedContainerParserAndParseIntAndUIntOptionValidateRecursiveThrows) {
    constexpr const char* const argv[]{"-i=123", "-u=234", nullptr};
    const clapp::parser::types::arg_t arg{parser_make_arg_t(argv)};
    nested_container_parser_t ncp;
    ASSERT_THAT(ncp.parse(arg.begin(), arg.end()).has_value(),
                testing::Eq(false));

    ASSERT_THAT(ncp.container.cont_string_option, ParamOptionNotGiven());
    ASSERT_THAT(ncp.container.toc.int_option, ParamOptionGiven(123));
    ASSERT_THAT(ncp.container.toc.uint_option, ParamOptionGiven(234U));
    ASSERT_THAT(ncp.container.toc2.string_option, ParamOptionNotGiven());
    ASSERT_THAT(ncp.container.toc2.bool_option, BoolOptionNotGiven());

    ASSERT_THROW(ncp.validate_recursive(),
                 clapp::exception::option_param_exception_t);
}

TEST(
    parser,
    constructNestedContainerParserAndParseCStrAndUIntOptionValidateRecursiveThrows) {
    constexpr const char* const argv[]{"-c=test", "-u=123", nullptr};
    const clapp::parser::types::arg_t arg{parser_make_arg_t(argv)};
    nested_container_parser_t ncp;
    ASSERT_THAT(ncp.parse(arg.begin(), arg.end()).has_value(),
                testing::Eq(false));

    ASSERT_THAT(ncp.container.cont_string_option, ParamOptionGiven("test"));
    ASSERT_THAT(ncp.container.toc.int_option, ParamOptionNotGiven());
    ASSERT_THAT(ncp.container.toc.uint_option, ParamOptionGiven(123U));
    ASSERT_THAT(ncp.container.toc2.string_option, ParamOptionNotGiven());
    ASSERT_THAT(ncp.container.toc2.bool_option, BoolOptionNotGiven());

    ASSERT_THROW(ncp.validate_recursive(),
                 clapp::exception::option_param_exception_t);
}

TEST(
    parser,
    constructNestedContainerParserAndParseCStrAndIntOptionValidateRecursiveThrows) {
    constexpr const char* const argv[]{"-c=test", "-i=123", nullptr};
    const clapp::parser::types::arg_t arg{parser_make_arg_t(argv)};
    nested_container_parser_t ncp;
    ASSERT_THAT(ncp.parse(arg.begin(), arg.end()).has_value(),
                testing::Eq(false));

    ASSERT_THAT(ncp.container.cont_string_option, ParamOptionGiven("test"));
    ASSERT_THAT(ncp.container.toc.int_option, ParamOptionGiven(123));
    ASSERT_THAT(ncp.container.toc.uint_option, ParamOptionNotGiven());
    ASSERT_THAT(ncp.container.toc2.string_option, ParamOptionNotGiven());
    ASSERT_THAT(ncp.container.toc2.bool_option, BoolOptionNotGiven());

    ASSERT_THROW(ncp.validate_recursive(),
                 clapp::exception::option_param_exception_t);
}

TEST(
    parser,
    constructNestedContainerParserAndParseCStrAndBoolOptionValidateRecursiveThrows) {
    constexpr const char* const argv[]{"-c=test", "--bool", nullptr};
    const clapp::parser::types::arg_t arg{parser_make_arg_t(argv)};
    nested_container_parser_t ncp;
    ASSERT_THAT(ncp.parse(arg.begin(), arg.end()).has_value(),
                testing::Eq(false));

    ASSERT_THAT(ncp.container.cont_string_option, ParamOptionGiven("test"));
    ASSERT_THAT(ncp.container.toc.int_option, ParamOptionNotGiven());
    ASSERT_THAT(ncp.container.toc.uint_option, ParamOptionNotGiven());
    ASSERT_THAT(ncp.container.toc2.string_option, ParamOptionNotGiven());
    ASSERT_THAT(ncp.container.toc2.bool_option, BoolOptionGiven());

    ASSERT_THROW(ncp.validate_recursive(),
                 clapp::exception::option_param_exception_t);
}

TEST(
    parser,
    constructNestedContainerParserAndParseBoolAndCStrOptionValidateRecursiveThrows) {
    constexpr const char* const argv[]{"--bool", "-c=test", nullptr};
    const clapp::parser::types::arg_t arg{parser_make_arg_t(argv)};
    nested_container_parser_t ncp;
    ASSERT_THAT(ncp.parse(arg.begin(), arg.end()).has_value(),
                testing::Eq(false));

    ASSERT_THAT(ncp.container.cont_string_option, ParamOptionGiven("test"));
    ASSERT_THAT(ncp.container.toc.int_option, ParamOptionNotGiven());
    ASSERT_THAT(ncp.container.toc.uint_option, ParamOptionNotGiven());
    ASSERT_THAT(ncp.container.toc2.string_option, ParamOptionNotGiven());
    ASSERT_THAT(ncp.container.toc2.bool_option, BoolOptionGiven());

    ASSERT_THROW(ncp.validate_recursive(),
                 clapp::exception::option_param_exception_t);
}

TEST(
    parser,
    constructNestedContainerParserAndParseCStrBoolAndStrOptionValidateRecursiveDoesNotThrow) {
    constexpr const char* const argv[]{"-b", "-s", "str", nullptr};
    const clapp::parser::types::arg_t arg{parser_make_arg_t(argv)};
    nested_container_parser_t ncp;
    ASSERT_THAT(ncp.parse(arg.begin(), arg.end()).has_value(),
                testing::Eq(false));

    ASSERT_THAT(ncp.container.cont_string_option, ParamOptionNotGiven());
    ASSERT_THAT(ncp.container.toc.int_option, ParamOptionNotGiven());
    ASSERT_THAT(ncp.container.toc.uint_option, ParamOptionNotGiven());
    ASSERT_THAT(ncp.container.toc2.string_option, ParamOptionGiven("str"));
    ASSERT_THAT(ncp.container.toc2.bool_option, BoolOptionGiven());

    ASSERT_NO_THROW(ncp.validate_recursive());
}

TEST(
    parser,
    constructNestedContainerParserAndParseCStrAndBoolShortOptionValidateRecursiveThrows) {
    constexpr const char* const argv[]{"--int=123", "-b", nullptr};
    const clapp::parser::types::arg_t arg{parser_make_arg_t(argv)};
    nested_container_parser_t ncp;

    ASSERT_THAT(ncp.parse(arg.begin(), arg.end()).has_value(),
                testing::Eq(false));

    ASSERT_THAT(ncp.container.cont_string_option, ParamOptionNotGiven());
    ASSERT_THAT(ncp.container.toc.int_option, ParamOptionGiven(123));
    ASSERT_THAT(ncp.container.toc.uint_option, ParamOptionNotGiven());
    ASSERT_THAT(ncp.container.toc2.string_option, ParamOptionNotGiven());
    ASSERT_THAT(ncp.container.toc2.bool_option, BoolOptionGiven());

    ASSERT_THROW(ncp.validate_recursive(),
                 clapp::exception::option_param_exception_t);
}

TEST(
    parser,
    constructNestedContainerParserAndParseCStrAndBoolLongOptionValidateRecursiveThrows) {
    constexpr const char* const argv[]{"--uint", "123", "--bool", nullptr};
    const clapp::parser::types::arg_t arg{parser_make_arg_t(argv)};
    nested_container_parser_t ncp;

    ASSERT_THAT(ncp.parse(arg.begin(), arg.end()).has_value(),
                testing::Eq(false));

    ASSERT_THAT(ncp.container.cont_string_option, ParamOptionNotGiven());
    ASSERT_THAT(ncp.container.toc.int_option, ParamOptionNotGiven());
    ASSERT_THAT(ncp.container.toc.uint_option, ParamOptionGiven(123U));
    ASSERT_THAT(ncp.container.toc2.string_option, ParamOptionNotGiven());
    ASSERT_THAT(ncp.container.toc2.bool_option, BoolOptionGiven());

    ASSERT_THROW(ncp.validate_recursive(),
                 clapp::exception::option_param_exception_t);
}

TEST(
    parser,
    constructNestedContainerParserAndParseBoolLongOptionValidateRecursiveThrows) {
    constexpr const char* const argv[]{"--bool", nullptr};
    const clapp::parser::types::arg_t arg{parser_make_arg_t(argv)};
    nested_container_parser_t ncp;

    ASSERT_THAT(ncp.parse(arg.begin(), arg.end()).has_value(),
                testing::Eq(false));

    ASSERT_THAT(ncp.container.cont_string_option, ParamOptionNotGiven());
    ASSERT_THAT(ncp.container.toc.int_option, ParamOptionNotGiven());
    ASSERT_THAT(ncp.container.toc.uint_option, ParamOptionNotGiven());
    ASSERT_THAT(ncp.container.toc2.string_option, ParamOptionNotGiven());
    ASSERT_THAT(ncp.container.toc2.bool_option, BoolOptionGiven());

    ASSERT_THROW(ncp.validate_recursive(),
                 clapp::exception::option_param_exception_t);
}

TEST(
    parser,
    constructNestedContainerParserAndParseStringLongOptionValidateRecursiveThrows) {
    constexpr const char* const argv[]{"--str", "string", nullptr};
    const clapp::parser::types::arg_t arg{parser_make_arg_t(argv)};
    nested_container_parser_t ncp;

    ASSERT_THAT(ncp.parse(arg.begin(), arg.end()).has_value(),
                testing::Eq(false));

    ASSERT_THAT(ncp.container.cont_string_option, ParamOptionNotGiven());
    ASSERT_THAT(ncp.container.toc.int_option, ParamOptionNotGiven());
    ASSERT_THAT(ncp.container.toc.uint_option, ParamOptionNotGiven());
    ASSERT_THAT(ncp.container.toc2.string_option, ParamOptionGiven("string"));
    ASSERT_THAT(ncp.container.toc2.bool_option, BoolOptionNotGiven());

    ASSERT_THROW(ncp.validate_recursive(),
                 clapp::exception::option_param_exception_t);
}

TEST(parser, constructNestedContainerParser2AndGenHelpMessage) {
    nested_container_parser2_t ncp;
    ASSERT_THAT(
        ncp.gen_help_msg(255),
        testing::StrEq(
            "container-parser ( -c|--cstr=<param> ( -i|--int=<param> "
            "-u|--uint=<param> ) ( -s|--str=<param> | -b|--bool ) )\n\n  "
            "Options:\n    -c|--cstr=<param> Cont String option. (mandatory)\n "
            "   -i|--int=<param>  Int option. (mandatory)\n    "
            "-u|--uint=<param> Unsigned int option. (mandatory)\n    "
            "-s|--str=<param>  String option. (mandatory)\n    -b|--bool       "
            "  Bool option. (mandatory)\n"));
}

TEST(
    parser,
    constructNestedContainerParser2AndParseEmptyOptionValidateRecursiveThrows) {
    constexpr const char* const argv[]{nullptr};
    const clapp::parser::types::arg_t arg{parser_make_arg_t(argv)};
    nested_container_parser2_t ncp;
    ASSERT_THAT(ncp.parse(arg.begin(), arg.end()).has_value(),
                testing::Eq(false));

    ASSERT_THAT(ncp.container.cont_string_option, ParamOptionNotGiven());
    ASSERT_THAT(ncp.container.toc.int_option, ParamOptionNotGiven());
    ASSERT_THAT(ncp.container.toc.uint_option, ParamOptionNotGiven());
    ASSERT_THAT(ncp.container.toc2.string_option, ParamOptionNotGiven());
    ASSERT_THAT(ncp.container.toc2.bool_option, BoolOptionNotGiven());

    ASSERT_THROW(ncp.validate_recursive(),
                 clapp::exception::option_param_exception_t);
}

TEST(
    parser,
    constructNestedContainerParser2AndParseCStrIntUintStrOptionValidateRecursiveDoesNotThrow) {
    constexpr const char* const argv[]{"-c",  "test", "-i",  "12",   "-u",
                                       "123", "-s",   "str", nullptr};
    const clapp::parser::types::arg_t arg{parser_make_arg_t(argv)};
    nested_container_parser2_t ncp;
    ASSERT_THAT(ncp.parse(arg.begin(), arg.end()).has_value(),
                testing::Eq(false));

    ASSERT_THAT(ncp.container.cont_string_option, ParamOptionGiven("test"));
    ASSERT_THAT(ncp.container.toc.int_option, ParamOptionGiven(12));
    ASSERT_THAT(ncp.container.toc.uint_option, ParamOptionGiven(123U));
    ASSERT_THAT(ncp.container.toc2.string_option, ParamOptionGiven("str"));
    ASSERT_THAT(ncp.container.toc2.bool_option, BoolOptionNotGiven());

    ASSERT_NO_THROW(ncp.validate_recursive());
}

TEST(
    parser,
    constructNestedContainerParser2AndParseCStrIntUintBoolOptionValidateRecursiveDoesNotThrow) {
    constexpr const char* const argv[]{"--cstr", "t",  "--int", "123",
                                       "--uint", "12", "-b",    nullptr};
    const clapp::parser::types::arg_t arg{parser_make_arg_t(argv)};
    nested_container_parser2_t ncp;
    ASSERT_THAT(ncp.parse(arg.begin(), arg.end()).has_value(),
                testing::Eq(false));

    ASSERT_THAT(ncp.container.cont_string_option, ParamOptionGiven("t"));
    ASSERT_THAT(ncp.container.toc.int_option, ParamOptionGiven(123));
    ASSERT_THAT(ncp.container.toc.uint_option, ParamOptionGiven(12U));
    ASSERT_THAT(ncp.container.toc2.string_option, ParamOptionNotGiven());
    ASSERT_THAT(ncp.container.toc2.bool_option, BoolOptionGiven());

    ASSERT_NO_THROW(ncp.validate_recursive());
}

TEST(
    parser,
    constructNestedContainerParser2AndParseCStrIntUintOptionValidateRecursiveThrows) {
    constexpr const char* const argv[]{"--cstr", "te",  "--int", "1234",
                                       "--uint", "123", nullptr};
    const clapp::parser::types::arg_t arg{parser_make_arg_t(argv)};
    nested_container_parser_t ncp;
    ASSERT_THAT(ncp.parse(arg.begin(), arg.end()).has_value(),
                testing::Eq(false));

    ASSERT_THAT(ncp.container.cont_string_option, ParamOptionGiven("te"));
    ASSERT_THAT(ncp.container.toc.int_option, ParamOptionGiven(1234));
    ASSERT_THAT(ncp.container.toc.uint_option, ParamOptionGiven(123U));
    ASSERT_THAT(ncp.container.toc2.string_option, ParamOptionNotGiven());
    ASSERT_THAT(ncp.container.toc2.bool_option, BoolOptionNotGiven());

    ASSERT_THROW(ncp.validate_recursive(),
                 clapp::exception::option_param_exception_t);
}

TEST(
    parser,
    constructNestedContainerParser2AndParseCStrIntBoolOptionValidateRecursiveThrows) {
    constexpr const char* const argv[]{"--cstr", "t",  "--int",
                                       "123",    "-b", nullptr};
    const clapp::parser::types::arg_t arg{parser_make_arg_t(argv)};
    nested_container_parser2_t ncp;
    ASSERT_THAT(ncp.parse(arg.begin(), arg.end()).has_value(),
                testing::Eq(false));

    ASSERT_THAT(ncp.container.cont_string_option, ParamOptionGiven("t"));
    ASSERT_THAT(ncp.container.toc.int_option, ParamOptionGiven(123));
    ASSERT_THAT(ncp.container.toc.uint_option, ParamOptionNotGiven());
    ASSERT_THAT(ncp.container.toc2.string_option, ParamOptionNotGiven());
    ASSERT_THAT(ncp.container.toc2.bool_option, BoolOptionGiven());

    ASSERT_THROW(ncp.validate_recursive(),
                 clapp::exception::option_param_exception_t);
}

TEST(
    parser,
    constructNestedContainerParser2AndParseCStrUintBoolOptionValidateRecursiveThrows) {
    constexpr const char* const argv[]{"--cstr", "test", "--uint",
                                       "123",    "-b",   nullptr};
    const clapp::parser::types::arg_t arg{parser_make_arg_t(argv)};
    nested_container_parser2_t ncp;
    ASSERT_THAT(ncp.parse(arg.begin(), arg.end()).has_value(),
                testing::Eq(false));

    ASSERT_THAT(ncp.container.cont_string_option, ParamOptionGiven("test"));
    ASSERT_THAT(ncp.container.toc.int_option, ParamOptionNotGiven());
    ASSERT_THAT(ncp.container.toc.uint_option, ParamOptionGiven(123U));
    ASSERT_THAT(ncp.container.toc2.string_option, ParamOptionNotGiven());
    ASSERT_THAT(ncp.container.toc2.bool_option, BoolOptionGiven());

    ASSERT_THROW(ncp.validate_recursive(),
                 clapp::exception::option_param_exception_t);
}

TEST(
    parser,
    constructNestedContainerParser2AndParseIntUintStrOptionValidateRecursiveThrows) {
    constexpr const char* const argv[]{"-i", "12",  "-u",   "123",
                                       "-s", "str", nullptr};
    const clapp::parser::types::arg_t arg{parser_make_arg_t(argv)};
    nested_container_parser2_t ncp;
    ASSERT_THAT(ncp.parse(arg.begin(), arg.end()).has_value(),
                testing::Eq(false));

    ASSERT_THAT(ncp.container.cont_string_option, ParamOptionNotGiven());
    ASSERT_THAT(ncp.container.toc.int_option, ParamOptionGiven(12));
    ASSERT_THAT(ncp.container.toc.uint_option, ParamOptionGiven(123U));
    ASSERT_THAT(ncp.container.toc2.string_option, ParamOptionGiven("str"));
    ASSERT_THAT(ncp.container.toc2.bool_option, BoolOptionNotGiven());

    ASSERT_THROW(ncp.validate_recursive(),
                 clapp::exception::option_param_exception_t);
}

TEST(parser, constructSuperNestedContainerParserAndGenHelpMessage) {
    super_nested_container_parser_t sncp;
    ASSERT_THAT(
        sncp.gen_help_msg(255),
        testing::StrEq(
            "container-parser ( -c|--cstr=<param> | ( -i|--int=<param> "
            "-u|--uint=<param> ( --sub-int=<param> --sub-int2=<param> ) ( "
            "--sub-uint=<param> | --sub-uint2=<param> ) ) | ( -s|--str=<param> "
            "| -b|--bool | ( --int1=<param> --int2=<param> ) | ( "
            "--uint1=<param> --uint2=<param> ) ) )\n\n  Options:\n    "
            "-c|--cstr=<param>   Cont String option. (mandatory)\n    "
            "-i|--int=<param>    Int option. (mandatory)\n    "
            "-u|--uint=<param>   Unsigned int option. (mandatory)\n    "
            "--sub-int=<param>   Sub int option. (mandatory)\n    "
            "--sub-int2=<param>  Sub int option2. (mandatory)\n    "
            "--sub-uint=<param>  Sub unsigned int option. (mandatory)\n    "
            "--sub-uint2=<param> Sub unsigned int option2. (mandatory)\n    "
            "-s|--str=<param>    String option. (mandatory)\n    -b|--bool     "
            "      Bool option. (mandatory)\n    --int1=<param>      Int "
            "option1. (mandatory)\n    --int2=<param>      Int option2. "
            "(mandatory)\n    --uint1=<param>     Unsigned int option1. "
            "(mandatory)\n    --uint2=<param>     Unsigned int option2. "
            "(mandatory)\n"));
}

TEST(
    parser,
    constructSuperNestedContainerParserAndParseEmptyOptionValidateRecursiveThrows) {
    constexpr const char* const argv[]{nullptr};
    const clapp::parser::types::arg_t arg{parser_make_arg_t(argv)};
    super_nested_container_parser_t sncp;
    ASSERT_THAT(sncp.parse(arg.begin(), arg.end()).has_value(),
                testing::Eq(false));

    ASSERT_THAT(sncp.container.cont_string_option, ParamOptionNotGiven());
    ASSERT_THAT(sncp.container.toc.int_option, ParamOptionNotGiven());
    ASSERT_THAT(sncp.container.toc.uint_option, ParamOptionNotGiven());
    ASSERT_THAT(sncp.container.toc.tsic.sub_int_option, ParamOptionNotGiven());
    ASSERT_THAT(sncp.container.toc.tsic.sub_int_option2, ParamOptionNotGiven());
    ASSERT_THAT(sncp.container.toc.tsuc.sub_uint_option, ParamOptionNotGiven());
    ASSERT_THAT(sncp.container.toc.tsuc.sub_uint_option2,
                ParamOptionNotGiven());
    ASSERT_THAT(sncp.container.toc2.string_option, ParamOptionNotGiven());
    ASSERT_THAT(sncp.container.toc2.bool_option, BoolOptionNotGiven());
    ASSERT_THAT(sncp.container.toc2.tsic.int1, ParamOptionNotGiven());
    ASSERT_THAT(sncp.container.toc2.tsic.int2, ParamOptionNotGiven());
    ASSERT_THAT(sncp.container.toc2.tsuc.uint1, ParamOptionNotGiven());
    ASSERT_THAT(sncp.container.toc2.tsuc.uint2, ParamOptionNotGiven());

    ASSERT_THROW(sncp.validate_recursive(),
                 clapp::exception::option_param_exception_t);
}

TEST(
    parser,
    constructSuperNestedContainerParserAndParseCStrOptionValidateRecursiveDoesNotThrow) {
    constexpr const char* const argv[]{"--cstr", "te", nullptr};
    const clapp::parser::types::arg_t arg{parser_make_arg_t(argv)};
    super_nested_container_parser_t sncp;
    ASSERT_THAT(sncp.parse(arg.begin(), arg.end()).has_value(),
                testing::Eq(false));

    ASSERT_THAT(sncp.container.cont_string_option, ParamOptionGiven("te"));
    ASSERT_THAT(sncp.container.toc.int_option, ParamOptionNotGiven());
    ASSERT_THAT(sncp.container.toc.uint_option, ParamOptionNotGiven());
    ASSERT_THAT(sncp.container.toc.tsic.sub_int_option, ParamOptionNotGiven());
    ASSERT_THAT(sncp.container.toc.tsic.sub_int_option2, ParamOptionNotGiven());
    ASSERT_THAT(sncp.container.toc.tsuc.sub_uint_option, ParamOptionNotGiven());
    ASSERT_THAT(sncp.container.toc.tsuc.sub_uint_option2,
                ParamOptionNotGiven());
    ASSERT_THAT(sncp.container.toc2.string_option, ParamOptionNotGiven());
    ASSERT_THAT(sncp.container.toc2.bool_option, BoolOptionNotGiven());
    ASSERT_THAT(sncp.container.toc2.tsic.int1, ParamOptionNotGiven());
    ASSERT_THAT(sncp.container.toc2.tsic.int2, ParamOptionNotGiven());
    ASSERT_THAT(sncp.container.toc2.tsuc.uint1, ParamOptionNotGiven());
    ASSERT_THAT(sncp.container.toc2.tsuc.uint2, ParamOptionNotGiven());

    ASSERT_NO_THROW(sncp.validate_recursive());
}

TEST(
    parser,
    constructSuperNestedContainerParserAndParseIntUintSubIntSubInt2SubUintOptionValidateRecursiveDoesNotThrow) {
    constexpr const char* const argv[]{"-i",         "1", "-u",         "2",
                                       "--sub-int",  "3", "--sub-int2", "4",
                                       "--sub-uint", "5", nullptr};
    const clapp::parser::types::arg_t arg{parser_make_arg_t(argv)};
    super_nested_container_parser_t sncp;
    ASSERT_THAT(sncp.parse(arg.begin(), arg.end()).has_value(),
                testing::Eq(false));

    ASSERT_THAT(sncp.container.cont_string_option, ParamOptionNotGiven());
    ASSERT_THAT(sncp.container.toc.int_option, ParamOptionGiven(1));
    ASSERT_THAT(sncp.container.toc.uint_option, ParamOptionGiven(2U));
    ASSERT_THAT(sncp.container.toc.tsic.sub_int_option, ParamOptionGiven(3));
    ASSERT_THAT(sncp.container.toc.tsic.sub_int_option2, ParamOptionGiven(4));
    ASSERT_THAT(sncp.container.toc.tsuc.sub_uint_option, ParamOptionGiven(5U));
    ASSERT_THAT(sncp.container.toc.tsuc.sub_uint_option2,
                ParamOptionNotGiven());
    ASSERT_THAT(sncp.container.toc2.string_option, ParamOptionNotGiven());
    ASSERT_THAT(sncp.container.toc2.bool_option, BoolOptionNotGiven());
    ASSERT_THAT(sncp.container.toc2.tsic.int1, ParamOptionNotGiven());
    ASSERT_THAT(sncp.container.toc2.tsic.int2, ParamOptionNotGiven());
    ASSERT_THAT(sncp.container.toc2.tsuc.uint1, ParamOptionNotGiven());
    ASSERT_THAT(sncp.container.toc2.tsuc.uint2, ParamOptionNotGiven());

    ASSERT_NO_THROW(sncp.validate_recursive());
}

TEST(
    parser,
    constructSuperNestedContainerParserAndParseIntUintSubIntSubInt2SubUint2OptionValidateRecursiveDoesNotThrow) {
    constexpr const char* const argv[]{"-i",          "1", "-u",         "2",
                                       "--sub-int",   "3", "--sub-int2", "4",
                                       "--sub-uint2", "5", nullptr};
    const clapp::parser::types::arg_t arg{parser_make_arg_t(argv)};
    super_nested_container_parser_t sncp;
    ASSERT_THAT(sncp.parse(arg.begin(), arg.end()).has_value(),
                testing::Eq(false));

    ASSERT_THAT(sncp.container.cont_string_option, ParamOptionNotGiven());
    ASSERT_THAT(sncp.container.toc.int_option, ParamOptionGiven(1));
    ASSERT_THAT(sncp.container.toc.uint_option, ParamOptionGiven(2U));
    ASSERT_THAT(sncp.container.toc.tsic.sub_int_option, ParamOptionGiven(3));
    ASSERT_THAT(sncp.container.toc.tsic.sub_int_option2, ParamOptionGiven(4));
    ASSERT_THAT(sncp.container.toc.tsuc.sub_uint_option, ParamOptionNotGiven());
    ASSERT_THAT(sncp.container.toc.tsuc.sub_uint_option2, ParamOptionGiven(5U));
    ASSERT_THAT(sncp.container.toc2.string_option, ParamOptionNotGiven());
    ASSERT_THAT(sncp.container.toc2.bool_option, BoolOptionNotGiven());
    ASSERT_THAT(sncp.container.toc2.tsic.int1, ParamOptionNotGiven());
    ASSERT_THAT(sncp.container.toc2.tsic.int2, ParamOptionNotGiven());
    ASSERT_THAT(sncp.container.toc2.tsuc.uint1, ParamOptionNotGiven());
    ASSERT_THAT(sncp.container.toc2.tsuc.uint2, ParamOptionNotGiven());

    ASSERT_NO_THROW(sncp.validate_recursive());
}

TEST(
    parser,
    constructSuperNestedContainerParserAndParseIntUintSubIntSubInt2SubUintSubUint2OptionValidateRecursiveThrows) {
    constexpr const char* const argv[]{"-i",         "1", "-u",          "2",
                                       "--sub-int",  "3", "--sub-int2",  "4",
                                       "--sub-uint", "5", "--sub-uint2", "6",
                                       nullptr};
    const clapp::parser::types::arg_t arg{parser_make_arg_t(argv)};
    super_nested_container_parser_t sncp;
    ASSERT_THAT(sncp.parse(arg.begin(), arg.end()).has_value(),
                testing::Eq(false));

    ASSERT_THAT(sncp.container.cont_string_option, ParamOptionNotGiven());
    ASSERT_THAT(sncp.container.toc.int_option, ParamOptionGiven(1));
    ASSERT_THAT(sncp.container.toc.uint_option, ParamOptionGiven(2U));
    ASSERT_THAT(sncp.container.toc.tsic.sub_int_option, ParamOptionGiven(3));
    ASSERT_THAT(sncp.container.toc.tsic.sub_int_option2, ParamOptionGiven(4));
    ASSERT_THAT(sncp.container.toc.tsuc.sub_uint_option, ParamOptionGiven(5U));
    ASSERT_THAT(sncp.container.toc.tsuc.sub_uint_option2, ParamOptionGiven(6U));
    ASSERT_THAT(sncp.container.toc2.string_option, ParamOptionNotGiven());
    ASSERT_THAT(sncp.container.toc2.bool_option, BoolOptionNotGiven());
    ASSERT_THAT(sncp.container.toc2.tsic.int1, ParamOptionNotGiven());
    ASSERT_THAT(sncp.container.toc2.tsic.int2, ParamOptionNotGiven());
    ASSERT_THAT(sncp.container.toc2.tsuc.uint1, ParamOptionNotGiven());
    ASSERT_THAT(sncp.container.toc2.tsuc.uint2, ParamOptionNotGiven());

    ASSERT_THROW(sncp.validate_recursive(),
                 clapp::exception::option_param_exception_t);
}

TEST(
    parser,
    constructSuperNestedContainerParserAndParseIntUintSubIntSubUintSubUint2OptionValidateRecursiveThrows) {
    constexpr const char* const argv[]{"--int",      "1",         "--uint",
                                       "2",          "--sub-int", "3",
                                       "--sub-uint", "4",         nullptr};
    const clapp::parser::types::arg_t arg{parser_make_arg_t(argv)};
    super_nested_container_parser_t sncp;
    ASSERT_THAT(sncp.parse(arg.begin(), arg.end()).has_value(),
                testing::Eq(false));

    ASSERT_THAT(sncp.container.cont_string_option, ParamOptionNotGiven());
    ASSERT_THAT(sncp.container.toc.int_option, ParamOptionGiven(1));
    ASSERT_THAT(sncp.container.toc.uint_option, ParamOptionGiven(2U));
    ASSERT_THAT(sncp.container.toc.tsic.sub_int_option, ParamOptionGiven(3));
    ASSERT_THAT(sncp.container.toc.tsic.sub_int_option2, ParamOptionNotGiven());
    ASSERT_THAT(sncp.container.toc.tsuc.sub_uint_option, ParamOptionGiven(4U));
    ASSERT_THAT(sncp.container.toc.tsuc.sub_uint_option2,
                ParamOptionNotGiven());
    ASSERT_THAT(sncp.container.toc2.string_option, ParamOptionNotGiven());
    ASSERT_THAT(sncp.container.toc2.bool_option, BoolOptionNotGiven());
    ASSERT_THAT(sncp.container.toc2.tsic.int1, ParamOptionNotGiven());
    ASSERT_THAT(sncp.container.toc2.tsic.int2, ParamOptionNotGiven());
    ASSERT_THAT(sncp.container.toc2.tsuc.uint1, ParamOptionNotGiven());
    ASSERT_THAT(sncp.container.toc2.tsuc.uint2, ParamOptionNotGiven());

    ASSERT_THROW(sncp.validate_recursive(),
                 clapp::exception::option_param_exception_t);
}

TEST(
    parser,
    constructSuperNestedContainerParserAndParseIntSubIntSubInt2SubUint2OptionValidateRecursiveThrows) {
    constexpr const char* const argv[]{"-i",          "1",          "--sub-int",
                                       "2",           "--sub-int2", "3",
                                       "--sub-uint2", "4",          nullptr};
    const clapp::parser::types::arg_t arg{parser_make_arg_t(argv)};
    super_nested_container_parser_t sncp;
    ASSERT_THAT(sncp.parse(arg.begin(), arg.end()).has_value(),
                testing::Eq(false));

    ASSERT_THAT(sncp.container.cont_string_option, ParamOptionNotGiven());
    ASSERT_THAT(sncp.container.toc.int_option, ParamOptionGiven(1));
    ASSERT_THAT(sncp.container.toc.uint_option, ParamOptionNotGiven());
    ASSERT_THAT(sncp.container.toc.tsic.sub_int_option, ParamOptionGiven(2));
    ASSERT_THAT(sncp.container.toc.tsic.sub_int_option2, ParamOptionGiven(3));
    ASSERT_THAT(sncp.container.toc.tsuc.sub_uint_option, ParamOptionNotGiven());
    ASSERT_THAT(sncp.container.toc.tsuc.sub_uint_option2, ParamOptionGiven(4U));
    ASSERT_THAT(sncp.container.toc2.string_option, ParamOptionNotGiven());
    ASSERT_THAT(sncp.container.toc2.bool_option, BoolOptionNotGiven());
    ASSERT_THAT(sncp.container.toc2.tsic.int1, ParamOptionNotGiven());
    ASSERT_THAT(sncp.container.toc2.tsic.int2, ParamOptionNotGiven());
    ASSERT_THAT(sncp.container.toc2.tsuc.uint1, ParamOptionNotGiven());
    ASSERT_THAT(sncp.container.toc2.tsuc.uint2, ParamOptionNotGiven());

    ASSERT_THROW(sncp.validate_recursive(),
                 clapp::exception::option_param_exception_t);
}

TEST(
    parser,
    constructSuperNestedContainerParserAndParseStringOptionValidateRecursiveDoesNotThrow) {
    constexpr const char* const argv[]{"-s", "string", nullptr};
    const clapp::parser::types::arg_t arg{parser_make_arg_t(argv)};
    super_nested_container_parser_t sncp;
    ASSERT_THAT(sncp.parse(arg.begin(), arg.end()).has_value(),
                testing::Eq(false));

    ASSERT_THAT(sncp.container.cont_string_option, ParamOptionNotGiven());
    ASSERT_THAT(sncp.container.toc.int_option, ParamOptionNotGiven());
    ASSERT_THAT(sncp.container.toc.uint_option, ParamOptionNotGiven());
    ASSERT_THAT(sncp.container.toc.tsic.sub_int_option, ParamOptionNotGiven());
    ASSERT_THAT(sncp.container.toc.tsic.sub_int_option2, ParamOptionNotGiven());
    ASSERT_THAT(sncp.container.toc.tsuc.sub_uint_option, ParamOptionNotGiven());
    ASSERT_THAT(sncp.container.toc.tsuc.sub_uint_option2,
                ParamOptionNotGiven());
    ASSERT_THAT(sncp.container.toc2.string_option, ParamOptionGiven("string"));
    ASSERT_THAT(sncp.container.toc2.bool_option, BoolOptionNotGiven());
    ASSERT_THAT(sncp.container.toc2.tsic.int1, ParamOptionNotGiven());
    ASSERT_THAT(sncp.container.toc2.tsic.int2, ParamOptionNotGiven());
    ASSERT_THAT(sncp.container.toc2.tsuc.uint1, ParamOptionNotGiven());
    ASSERT_THAT(sncp.container.toc2.tsuc.uint2, ParamOptionNotGiven());

    ASSERT_NO_THROW(sncp.validate_recursive());
}

TEST(
    parser,
    constructSuperNestedContainerParserAndParseBoolOptionValidateRecursiveDoesNotThrow) {
    constexpr const char* const argv[]{"-b", nullptr};
    const clapp::parser::types::arg_t arg{parser_make_arg_t(argv)};
    super_nested_container_parser_t sncp;
    ASSERT_THAT(sncp.parse(arg.begin(), arg.end()).has_value(),
                testing::Eq(false));

    ASSERT_THAT(sncp.container.cont_string_option, ParamOptionNotGiven());
    ASSERT_THAT(sncp.container.toc.int_option, ParamOptionNotGiven());
    ASSERT_THAT(sncp.container.toc.uint_option, ParamOptionNotGiven());
    ASSERT_THAT(sncp.container.toc.tsic.sub_int_option, ParamOptionNotGiven());
    ASSERT_THAT(sncp.container.toc.tsic.sub_int_option2, ParamOptionNotGiven());
    ASSERT_THAT(sncp.container.toc.tsuc.sub_uint_option, ParamOptionNotGiven());
    ASSERT_THAT(sncp.container.toc.tsuc.sub_uint_option2,
                ParamOptionNotGiven());
    ASSERT_THAT(sncp.container.toc2.string_option, ParamOptionNotGiven());
    ASSERT_THAT(sncp.container.toc2.bool_option, BoolOptionGiven());
    ASSERT_THAT(sncp.container.toc2.tsic.int1, ParamOptionNotGiven());
    ASSERT_THAT(sncp.container.toc2.tsic.int2, ParamOptionNotGiven());
    ASSERT_THAT(sncp.container.toc2.tsuc.uint1, ParamOptionNotGiven());
    ASSERT_THAT(sncp.container.toc2.tsuc.uint2, ParamOptionNotGiven());

    ASSERT_NO_THROW(sncp.validate_recursive());
}

TEST(
    parser,
    constructSuperNestedContainerParserAndParseInt1Int2OptionValidateRecursiveDoesNotThrow) {
    constexpr const char* const argv[]{"--int1=1", "--int2=2", nullptr};
    const clapp::parser::types::arg_t arg{parser_make_arg_t(argv)};
    super_nested_container_parser_t sncp;
    ASSERT_THAT(sncp.parse(arg.begin(), arg.end()).has_value(),
                testing::Eq(false));

    ASSERT_THAT(sncp.container.cont_string_option, ParamOptionNotGiven());
    ASSERT_THAT(sncp.container.toc.int_option, ParamOptionNotGiven());
    ASSERT_THAT(sncp.container.toc.uint_option, ParamOptionNotGiven());
    ASSERT_THAT(sncp.container.toc.tsic.sub_int_option, ParamOptionNotGiven());
    ASSERT_THAT(sncp.container.toc.tsic.sub_int_option2, ParamOptionNotGiven());
    ASSERT_THAT(sncp.container.toc.tsuc.sub_uint_option, ParamOptionNotGiven());
    ASSERT_THAT(sncp.container.toc.tsuc.sub_uint_option2,
                ParamOptionNotGiven());
    ASSERT_THAT(sncp.container.toc2.string_option, ParamOptionNotGiven());
    ASSERT_THAT(sncp.container.toc2.bool_option, BoolOptionNotGiven());
    ASSERT_THAT(sncp.container.toc2.tsic.int1, ParamOptionGiven(1));
    ASSERT_THAT(sncp.container.toc2.tsic.int2, ParamOptionGiven(2));
    ASSERT_THAT(sncp.container.toc2.tsuc.uint1, ParamOptionNotGiven());
    ASSERT_THAT(sncp.container.toc2.tsuc.uint2, ParamOptionNotGiven());

    ASSERT_NO_THROW(sncp.validate_recursive());
}

TEST(
    parser,
    constructSuperNestedContainerParserAndParseUint1Uint2OptionValidateRecursiveDoesNotThrow) {
    constexpr const char* const argv[]{"--uint1=1", "--uint2=2", nullptr};
    const clapp::parser::types::arg_t arg{parser_make_arg_t(argv)};
    super_nested_container_parser_t sncp;
    ASSERT_THAT(sncp.parse(arg.begin(), arg.end()).has_value(),
                testing::Eq(false));

    ASSERT_THAT(sncp.container.cont_string_option, ParamOptionNotGiven());
    ASSERT_THAT(sncp.container.toc.int_option, ParamOptionNotGiven());
    ASSERT_THAT(sncp.container.toc.uint_option, ParamOptionNotGiven());
    ASSERT_THAT(sncp.container.toc.tsic.sub_int_option, ParamOptionNotGiven());
    ASSERT_THAT(sncp.container.toc.tsic.sub_int_option2, ParamOptionNotGiven());
    ASSERT_THAT(sncp.container.toc.tsuc.sub_uint_option, ParamOptionNotGiven());
    ASSERT_THAT(sncp.container.toc.tsuc.sub_uint_option2,
                ParamOptionNotGiven());
    ASSERT_THAT(sncp.container.toc2.string_option, ParamOptionNotGiven());
    ASSERT_THAT(sncp.container.toc2.bool_option, BoolOptionNotGiven());
    ASSERT_THAT(sncp.container.toc2.tsic.int1, ParamOptionNotGiven());
    ASSERT_THAT(sncp.container.toc2.tsic.int2, ParamOptionNotGiven());
    ASSERT_THAT(sncp.container.toc2.tsuc.uint1, ParamOptionGiven(1U));
    ASSERT_THAT(sncp.container.toc2.tsuc.uint2, ParamOptionGiven(2U));

    ASSERT_NO_THROW(sncp.validate_recursive());
}

TEST(
    parser,
    constructSuperNestedContainerParserAndParseInt1Int2Uint1Uint2OptionValidateRecursiveThrows) {
    constexpr const char* const argv[]{"--int1=1", "--int2=2", "--uint1=3",
                                       "--uint2=4", nullptr};
    const clapp::parser::types::arg_t arg{parser_make_arg_t(argv)};
    super_nested_container_parser_t sncp;
    ASSERT_THAT(sncp.parse(arg.begin(), arg.end()).has_value(),
                testing::Eq(false));

    ASSERT_THAT(sncp.container.cont_string_option, ParamOptionNotGiven());
    ASSERT_THAT(sncp.container.toc.int_option, ParamOptionNotGiven());
    ASSERT_THAT(sncp.container.toc.uint_option, ParamOptionNotGiven());
    ASSERT_THAT(sncp.container.toc.tsic.sub_int_option, ParamOptionNotGiven());
    ASSERT_THAT(sncp.container.toc.tsic.sub_int_option2, ParamOptionNotGiven());
    ASSERT_THAT(sncp.container.toc.tsuc.sub_uint_option, ParamOptionNotGiven());
    ASSERT_THAT(sncp.container.toc.tsuc.sub_uint_option2,
                ParamOptionNotGiven());
    ASSERT_THAT(sncp.container.toc2.string_option, ParamOptionNotGiven());
    ASSERT_THAT(sncp.container.toc2.bool_option, BoolOptionNotGiven());
    ASSERT_THAT(sncp.container.toc2.tsic.int1, ParamOptionGiven(1));
    ASSERT_THAT(sncp.container.toc2.tsic.int2, ParamOptionGiven(2));
    ASSERT_THAT(sncp.container.toc2.tsuc.uint1, ParamOptionGiven(3U));
    ASSERT_THAT(sncp.container.toc2.tsuc.uint2, ParamOptionGiven(4U));

    ASSERT_THROW(sncp.validate_recursive(),
                 clapp::exception::option_param_exception_t);
}

TEST(
    parser,
    constructSuperNestedContainerParserAndParseBoolInt1Int2OptionValidateRecursiveThrows) {
    constexpr const char* const argv[]{"--bool", "--int1=1", "--int2=2",
                                       nullptr};
    const clapp::parser::types::arg_t arg{parser_make_arg_t(argv)};
    super_nested_container_parser_t sncp;
    ASSERT_THAT(sncp.parse(arg.begin(), arg.end()).has_value(),
                testing::Eq(false));

    ASSERT_THAT(sncp.container.cont_string_option, ParamOptionNotGiven());
    ASSERT_THAT(sncp.container.toc.int_option, ParamOptionNotGiven());
    ASSERT_THAT(sncp.container.toc.uint_option, ParamOptionNotGiven());
    ASSERT_THAT(sncp.container.toc.tsic.sub_int_option, ParamOptionNotGiven());
    ASSERT_THAT(sncp.container.toc.tsic.sub_int_option2, ParamOptionNotGiven());
    ASSERT_THAT(sncp.container.toc.tsuc.sub_uint_option, ParamOptionNotGiven());
    ASSERT_THAT(sncp.container.toc.tsuc.sub_uint_option2,
                ParamOptionNotGiven());
    ASSERT_THAT(sncp.container.toc2.string_option, ParamOptionNotGiven());
    ASSERT_THAT(sncp.container.toc2.bool_option, BoolOptionGiven());
    ASSERT_THAT(sncp.container.toc2.tsic.int1, ParamOptionGiven(1));
    ASSERT_THAT(sncp.container.toc2.tsic.int2, ParamOptionGiven(2));
    ASSERT_THAT(sncp.container.toc2.tsuc.uint1, ParamOptionNotGiven());
    ASSERT_THAT(sncp.container.toc2.tsuc.uint2, ParamOptionNotGiven());

    ASSERT_THROW(sncp.validate_recursive(),
                 clapp::exception::option_param_exception_t);
}

TEST(
    parser,
    constructSuperNestedContainerParserAndParseStringUint1Uint2OptionValidateRecursiveThrows) {
    constexpr const char* const argv[]{"-s=str", "--uint1=1", "--uint2=2",
                                       nullptr};
    const clapp::parser::types::arg_t arg{parser_make_arg_t(argv)};
    super_nested_container_parser_t sncp;
    ASSERT_THAT(sncp.parse(arg.begin(), arg.end()).has_value(),
                testing::Eq(false));

    ASSERT_THAT(sncp.container.cont_string_option, ParamOptionNotGiven());
    ASSERT_THAT(sncp.container.toc.int_option, ParamOptionNotGiven());
    ASSERT_THAT(sncp.container.toc.uint_option, ParamOptionNotGiven());
    ASSERT_THAT(sncp.container.toc.tsic.sub_int_option, ParamOptionNotGiven());
    ASSERT_THAT(sncp.container.toc.tsic.sub_int_option2, ParamOptionNotGiven());
    ASSERT_THAT(sncp.container.toc.tsuc.sub_uint_option, ParamOptionNotGiven());
    ASSERT_THAT(sncp.container.toc.tsuc.sub_uint_option2,
                ParamOptionNotGiven());
    ASSERT_THAT(sncp.container.toc2.string_option, ParamOptionGiven("str"));
    ASSERT_THAT(sncp.container.toc2.bool_option, BoolOptionNotGiven());
    ASSERT_THAT(sncp.container.toc2.tsic.int1, ParamOptionNotGiven());
    ASSERT_THAT(sncp.container.toc2.tsic.int2, ParamOptionNotGiven());
    ASSERT_THAT(sncp.container.toc2.tsuc.uint1, ParamOptionGiven(1U));
    ASSERT_THAT(sncp.container.toc2.tsuc.uint2, ParamOptionGiven(2U));

    ASSERT_THROW(sncp.validate_recursive(),
                 clapp::exception::option_param_exception_t);
}

TEST(
    parser,
    constructSuperNestedContainerParserAndParseStringBoolOptionValidateRecursiveThrows) {
    constexpr const char* const argv[]{"-s=string", "--bool", nullptr};
    const clapp::parser::types::arg_t arg{parser_make_arg_t(argv)};
    super_nested_container_parser_t sncp;
    ASSERT_THAT(sncp.parse(arg.begin(), arg.end()).has_value(),
                testing::Eq(false));

    ASSERT_THAT(sncp.container.cont_string_option, ParamOptionNotGiven());
    ASSERT_THAT(sncp.container.toc.int_option, ParamOptionNotGiven());
    ASSERT_THAT(sncp.container.toc.uint_option, ParamOptionNotGiven());
    ASSERT_THAT(sncp.container.toc.tsic.sub_int_option, ParamOptionNotGiven());
    ASSERT_THAT(sncp.container.toc.tsic.sub_int_option2, ParamOptionNotGiven());
    ASSERT_THAT(sncp.container.toc.tsuc.sub_uint_option, ParamOptionNotGiven());
    ASSERT_THAT(sncp.container.toc.tsuc.sub_uint_option2,
                ParamOptionNotGiven());
    ASSERT_THAT(sncp.container.toc2.string_option, ParamOptionGiven("string"));
    ASSERT_THAT(sncp.container.toc2.bool_option, BoolOptionGiven());
    ASSERT_THAT(sncp.container.toc2.tsic.int1, ParamOptionNotGiven());
    ASSERT_THAT(sncp.container.toc2.tsic.int2, ParamOptionNotGiven());
    ASSERT_THAT(sncp.container.toc2.tsuc.uint1, ParamOptionNotGiven());
    ASSERT_THAT(sncp.container.toc2.tsuc.uint2, ParamOptionNotGiven());

    ASSERT_THROW(sncp.validate_recursive(),
                 clapp::exception::option_param_exception_t);
}

TEST(parser, constructInvalidNestedContainerParserThrows) {
    ASSERT_THROW(invalid_nested_container_parser_t{},
                 clapp::exception::option_exception_t);
}

TEST(parser, constructInvalidNestedContainerParser2Throws) {
    ASSERT_THROW(invalid_nested_container_parser2_t{},
                 clapp::exception::option_exception_t);
}

TEST(parser, constructInvalidNestedContainerParser3Throws) {
    ASSERT_THROW(invalid_nested_container_parser3_t{},
                 clapp::exception::option_exception_t);
}
