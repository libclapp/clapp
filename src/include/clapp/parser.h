///////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2020 Martin Wölzer
//
// This code is licensed under the MIT License (MIT).
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
///////////////////////////////////////////////////////////////////////////////

#ifndef CLAPP_PARSER_H
#define CLAPP_PARSER_H

#include <clapp/value.h>
#include <functional>
#include <gsl/gsl_assert>
#include <gsl/span>
#include <map>
#include <optional>
#include <string>
#include <variant>

namespace clapp {

inline namespace parser {
struct arg_t : public gsl::span<const char* const> {
   public:
    using base = gsl::span<const char* const>;

    inline arg_t(const char* const* argv, int argc);
};

class basic_sub_parser_t;

class basic_parser_t {
   public:
    using arg_iterator = arg_t::iterator;
    using long_opt_func_t = std::function<void(const std::string_view option)>;
    using long_opt_param_func_t = std::function<void(
        const std::string_view option, const std::string_view param)>;
    using short_opt_func_t = std::function<void(const char option)>;
    using short_opt_param_func_t =
        std::function<void(const char option, const std::string_view param)>;
    using argument_func_t =
        std::function<void(const std::string_view argument)>;
    using validate_func_t = std::function<void(void)>;
    using print_and_exit_func_t = std::function<void(
        const std::string_view print_msg, std::optional<int> exit_code)>;

    enum class argument_type_t { single, variadic };
    enum class option_type_t { scalar, vector };
    enum class logic_operator_type_t { logic_and, logic_or };
    enum class purpose_t { optional, mandatory };

    struct sub_parser_line_t {
        std::string name;
        std::string description;
        basic_sub_parser_t& parser;
    };

    struct help_entry_t {
        std::string name;
        std::string description;

        bool operator==(const help_entry_t& inst) const;
        bool operator!=(const help_entry_t& inst) const;
    };

    using help_entry_vec_t = std::vector<help_entry_t>;

    struct help_contents_t {
        help_entry_vec_t arguments{};
        help_entry_vec_t options{};
        std::map<std::string, sub_parser_line_t> sub_parser{};
        std::size_t max_name_size{0};
    };

    template <typename short_option_func_t>
    struct basic_short_opt_conf_t {
        char option;
        short_option_func_t func;
    };

    template <typename long_option_func_t>
    struct basic_long_opt_conf_t {
        std::string option;
        long_option_func_t func;
    };

    template <typename short_option_func_t, typename long_option_func_t,
              option_type_t option_type>
    struct basic_reg_option_conf_t {
        using long_opt_conf_t = basic_long_opt_conf_t<long_option_func_t>;
        using short_opt_conf_t = basic_short_opt_conf_t<short_option_func_t>;
        using short_opt_conf_vec_t = std::vector<short_opt_conf_t>;
        using long_opt_conf_vec_t = std::vector<long_opt_conf_t>;
        using short_opt_conf_vec_cit_t =
            typename short_opt_conf_vec_t::const_iterator;
        using long_opt_conf_vec_cit_t =
            typename long_opt_conf_vec_t::const_iterator;

        static std::string create_option_string(
            std::vector<char> short_option,
            const std::vector<std::string>& long_option);
        static std::string create_option_string(const std::string& long_option);
        static std::string create_option_string(char short_option);
        std::string create_basic_option_string() const;
        std::string create_option_string() const;
        help_entry_t get_option_help() const;
        long_opt_conf_vec_cit_t find_option(std::string_view long_option) const;
        short_opt_conf_vec_cit_t find_option(char short_option) const;
        bool contains_option(std::string_view long_option) const;
        bool contains_option(char short_option) const;

        short_opt_conf_vec_t short_options;
        long_opt_conf_vec_t long_options;
        std::optional<validate_func_t> validate_func;
        std::string description;
        purpose_t purpose{purpose_t::optional};
    };

    using opt_no_param_conf_t =
        basic_reg_option_conf_t<short_opt_func_t, long_opt_func_t,
                                option_type_t::scalar>;
    using opt_scalar_param_conf_t =
        basic_reg_option_conf_t<short_opt_param_func_t, long_opt_param_func_t,
                                option_type_t::scalar>;
    using opt_vector_param_conf_t =
        basic_reg_option_conf_t<short_opt_param_func_t, long_opt_param_func_t,
                                option_type_t::vector>;
    using variant_opt_conf_t =
        std::variant<opt_no_param_conf_t, opt_scalar_param_conf_t,
                     opt_vector_param_conf_t>;

    template <argument_type_t arg_type>
    struct basic_reg_argument_conf_t {
        std::string create_basic_argument_string() const;
        std::string create_argument_string() const;
        help_entry_t get_argument_help() const;

        static constexpr argument_type_t argument_type = arg_type;
        argument_func_t argument;
        std::string argument_name;
        std::string description;
        std::optional<validate_func_t> validate_func;
        purpose_t purpose{purpose_t::mandatory};
    };

    using single_arg_conf_t =
        basic_reg_argument_conf_t<argument_type_t::single>;
    using variadic_arg_conf_t =
        basic_reg_argument_conf_t<argument_type_t::variadic>;
    using variant_arg_conf_t =
        std::variant<single_arg_conf_t, variadic_arg_conf_t>;

    struct reg_sub_parser_conf_t {
        basic_sub_parser_t& parser;
        std::string sub_parser_name;
        std::string description;
    };

    static inline const char* to_cstring(const purpose_t purpose) noexcept;

   protected:
    using long_opt_variant_t =
        std::variant<long_opt_func_t, long_opt_param_func_t>;
    using short_opt_variant_t =
        std::variant<short_opt_func_t, short_opt_param_func_t>;

    using sub_parsers_map_t =
        std::map<std::string, basic_sub_parser_t&, std::less<>>;
    using validate_func_vec_t = std::vector<validate_func_t>;
    using variant_opt_conf_vec_t = std::vector<variant_opt_conf_t>;
    using variant_arg_conf_vec_t = std::vector<variant_arg_conf_t>;

    struct option_description_container_t {
        std::string option_string;
        std::string description;
        option_type_t option_type;
    };

    struct sub_parser_description_container_t {
        std::string sub_parser_string;
        std::string description;
        basic_sub_parser_t& parser;
    };

    struct argument_description_container_t {
        std::string argument_string;
        std::string description;
        argument_type_t argument_type;
    };

    using option_descriptions_vec_t =
        std::vector<option_description_container_t>;
    using argument_descriptions_vec_t =
        std::vector<argument_description_container_t>;
    using sub_parser_descriptions_vec_t =
        std::vector<sub_parser_description_container_t>;

   public:
    basic_parser_t();
    explicit basic_parser_t(const basic_parser_t&) = delete;
    explicit basic_parser_t(basic_parser_t&&) noexcept = delete;
    basic_parser_t& operator=(const basic_parser_t&) = delete;
    basic_parser_t& operator=(basic_parser_t&&) noexcept = delete;

    virtual ~basic_parser_t();

    template <typename short_option_func_t, typename long_option_func_t>
    constexpr static bool is_param_opt();

    template <typename short_option_func_t, typename long_option_func_t,
              option_type_t option_type>
    void reg(basic_reg_option_conf_t<short_option_func_t, long_option_func_t,
                                     option_type>&& config);

    template <argument_type_t argument_type>
    void reg(basic_reg_argument_conf_t<argument_type>&& config);

    void reg(reg_sub_parser_conf_t&& config);

    struct parse_result_t {
        arg_iterator it;
        std::optional<char> short_option;
        std::optional<std::string> long_option;
    };

    static arg_iterator process_parse_result(
        arg_iterator it, const parse_result_t& parse_result);
    void parse(arg_iterator begin, arg_iterator end);
    parse_result_t parse(std::string_view option, arg_iterator it,
                         arg_iterator end);

    void validate() const;

    void validate_recursive() const;

    std::string gen_short_line() const;
    std::string gen_short_lines(std::size_t rec_depth) const;
    virtual std::string gen_short_line_prefix() const = 0;
    static constexpr std::string_view gen_usage_prefix() noexcept;
    virtual help_contents_t gen_detailed_help_contents() const;
    std::string gen_opt_arg_lines(const help_contents_t& help_contents,
                                  const std::size_t num_spaces) const;

    std::string gen_help_desc(std::size_t num_spaces,
                              std::size_t rec_depth) const;
    std::string gen_help_msg(std::size_t rec_depth) const;
    value::found_func_t gen_func_print_help_and_exit(int exit_code) const;

    std::size_t get_num_processed_arguments() const;

    inline virtual bool is_active() const noexcept;
    const basic_parser_t& get_active_parser() const;

    static void default_print_and_exit(const std::string_view print_msg,
                                       std::optional<int> exit_code);
    void set_print_and_exit_func(print_and_exit_func_t&& func);
    print_and_exit_func_t& get_print_and_exit_func();

    [[noreturn]] static void exit(int exit_code);

   protected:
    sub_parsers_map_t& get_sub_parsers();
    help_entry_vec_t get_option_help() const;
    help_entry_vec_t get_argument_help() const;
    variant_opt_conf_vec_t::const_iterator find_option(
        std::string_view opt) const;
    variant_opt_conf_vec_t::const_iterator find_option(char opt) const;
    variant_arg_conf_vec_t get_arguments() const;
    validate_func_vec_t& get_validate_functions();
    variant_opt_conf_vec_t get_options() const;
    sub_parser_descriptions_vec_t& get_sub_parser_descriptions();
    argument_descriptions_vec_t& get_mandatory_argument_descriptions();
    argument_descriptions_vec_t& get_optional_argument_descriptions();

   private:
    parse_result_t parse_arg(std::string_view argument, arg_iterator it,
                             arg_iterator end);
    parse_result_t parse_long(std::string_view option, arg_iterator it,
                              arg_iterator end);
    parse_result_t parse_short(std::string_view option, arg_iterator it,
                               arg_iterator end);

    sub_parsers_map_t sub_parsers{};
    validate_func_vec_t validate_functions{};
    sub_parser_descriptions_vec_t sub_parser_descriptions{};
    argument_descriptions_vec_t mandatory_argument_descriptions{};
    argument_descriptions_vec_t optional_argument_descriptions{};
    std::size_t num_processed_arguments{0};
    print_and_exit_func_t print_and_exit_func{default_print_and_exit};

    variant_opt_conf_vec_t options{};
    variant_arg_conf_vec_t arguments{};

   public:
    constexpr static std::size_t num_sub_spaces{2u};
};

}  // namespace parser

}  // namespace clapp

#include <clapp/parser.hpp>

#endif
