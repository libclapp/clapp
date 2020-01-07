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

#ifndef LIBCLAPP_PARSER_H
#define LIBCLAPP_PARSER_H

#include <functional>
#include <gsl/gsl_assert>
#include <gsl/span>
#include <map>
#include <string>
#include <variant>

namespace clapp {

inline namespace parser {
struct arg_t : public gsl::span<const char* const> {
   public:
    using base = gsl::span<const char* const>;

    arg_t(const char* const* argv, int argc);
};

class basic_parser_t {
   public:
    using arg_iterator = arg_t::const_iterator;
    using long_opt_func_t = std::function<void(const std::string_view option)>;
    using parse_func_t =
        std::function<void(arg_iterator begin, arg_iterator end)>;
    using long_opt_param_func_t = std::function<void(
        const std::string_view option, const std::string_view param)>;
    using short_opt_func_t = std::function<void(const char option)>;
    using short_opt_param_func_t =
        std::function<void(const char option, const std::string_view param)>;
    using argument_func_t =
        std::function<void(const std::string_view argument)>;
    using validate_func_t = std::function<void(void)>;

    enum class argument_type_t { single, variadic };
    enum class option_type_t { scalar, vector };
    enum class purpose_t { optional, mandatory };

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

    struct arg_conf_t {
        std::string argument;
        argument_func_t func;
        argument_type_t argument_type;
    };

    template <typename short_option_func_t, typename long_option_func_t,
              option_type_t option_type>
    struct reg_option_conf_t {
        static std::string create_option_string(
            const std::optional<char> short_option,
            const std::optional<std::string>& long_option);
        static std::string create_option_string(const std::string& long_option);
        static std::string create_option_string(const char short_option);

        using long_opt_conf_t = basic_long_opt_conf_t<long_option_func_t>;
        using short_opt_conf_t = basic_short_opt_conf_t<short_option_func_t>;
        std::optional<short_opt_conf_t> short_option;
        std::optional<long_opt_conf_t> long_option;
        std::optional<validate_func_t> validate_func;
        std::string option_string;
        std::string description;
        purpose_t purpose;
    };

    struct reg_sub_parser_conf_t {
        parse_func_t parse;
        std::string sub_parser_name;
        std::string description;
    };

    template <argument_type_t argument_type>
    struct reg_argument_conf_t {
        argument_func_t argument;
        std::string argument_name;
        std::string description;
        std::optional<validate_func_t> validate_func;
        purpose_t purpose;
    };

    using single_arg_conf_t = reg_argument_conf_t<argument_type_t::single>;
    using variadic_arg_conf_t = reg_argument_conf_t<argument_type_t::variadic>;
    using opt_conf_t = reg_option_conf_t<short_opt_func_t, long_opt_func_t,
                                         option_type_t::scalar>;
    using opt_scalar_param_conf_t =
        reg_option_conf_t<short_opt_param_func_t, long_opt_param_func_t,
                          option_type_t::scalar>;
    using opt_vector_param_conf_t =
        reg_option_conf_t<short_opt_param_func_t, long_opt_param_func_t,
                          option_type_t::vector>;

   protected:
    using long_opt_variant_t =
        std::variant<long_opt_func_t, long_opt_param_func_t>;
    using short_opt_variant_t =
        std::variant<short_opt_func_t, short_opt_param_func_t>;

    using sub_parsers_map_t = std::map<std::string, parse_func_t, std::less<>>;
    using arguments_vector_t = std::vector<arg_conf_t>;
    using optional_argument_t = std::optional<arg_conf_t>;
    using long_options_map_t =
        std::map<std::string, long_opt_variant_t, std::less<>>;
    using short_options_map_t = std::map<char, short_opt_variant_t>;
    using validate_func_vec_t = std::vector<validate_func_t>;

    struct option_description_container_t {
        std::string option_string;
        std::string description;
        option_type_t option_type;
    };

    struct sub_parser_description_container_t {
        std::string sub_parser_string;
        std::string description;
    };

    struct argument_description_container_t {
        std::string argument_string;
        std::string description;
        argument_type_t argument_type;
    };

    using option_descriptions_vec_t =
        std::vector<option_description_container_t>;
    using sub_parser_descriptions_vec_t =
        std::vector<sub_parser_description_container_t>;
    using argument_descriptions_vec_t =
        std::vector<argument_description_container_t>;

   public:
    basic_parser_t();
    virtual ~basic_parser_t();

    template <typename short_option_func_t, typename long_option_func_t>
    constexpr static bool is_param_opt();

    template <typename short_option_func_t, typename long_option_func_t,
              option_type_t option_type>
    void reg(reg_option_conf_t<short_option_func_t, long_option_func_t,
                               option_type>&& config);

    template <argument_type_t argument_type>
    void reg(reg_argument_conf_t<argument_type>&& config);

    void reg(reg_sub_parser_conf_t&& config);

    struct parse_result_t {
        arg_iterator it;
        std::optional<char> short_option;
        std::optional<std::string> long_option;
    };

    arg_iterator process_parse_result(const arg_iterator it,
                                      const parse_result_t& parse_result) const;
    void parse(arg_iterator begin, arg_iterator end);
    parse_result_t parse(std::string_view option, arg_iterator it,
                         arg_iterator end);

    void validate() const;

    std::string gen_help_msg() const;

    std::size_t get_num_processed_arguments() const;

   protected:
    sub_parsers_map_t& get_sub_parsers();
    long_options_map_t& get_long_options();
    short_options_map_t& get_short_options();
    arguments_vector_t& get_arguments();
    optional_argument_t& get_optional_argument();
    validate_func_vec_t& get_validate_functions();
    option_descriptions_vec_t& get_mandatory_option_descriptions();
    option_descriptions_vec_t& get_optional_option_descriptions();
    sub_parser_descriptions_vec_t& get_sub_parser_descriptions();
    argument_descriptions_vec_t& get_mandatory_argument_descriptions();
    argument_descriptions_vec_t& get_optional_argument_descriptions();

   private:
    parse_result_t parse_arg(const std::string_view option, arg_iterator it,
                             arg_iterator end);
    parse_result_t parse_long(const std::string_view option, arg_iterator it,
                              arg_iterator end);
    parse_result_t parse_short(const std::string_view option, arg_iterator it,
                               arg_iterator end);

    sub_parsers_map_t sub_parsers;
    long_options_map_t long_options;
    short_options_map_t short_options;
    arguments_vector_t arguments;
    optional_argument_t optional_argument;
    validate_func_vec_t validate_functions;
    option_descriptions_vec_t mandatory_option_descriptions;
    option_descriptions_vec_t optional_option_descriptions;
    sub_parser_descriptions_vec_t sub_parser_descriptions;
    argument_descriptions_vec_t mandatory_argument_descriptions;
    argument_descriptions_vec_t optional_argument_descriptions;
    std::size_t max_option_string_size{0};
    std::size_t num_processed_arguments{0};
};

}  // namespace parser

}  // namespace clapp

#include <clapp/parser.hpp>

#endif
