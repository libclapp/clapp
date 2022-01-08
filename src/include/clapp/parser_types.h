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

#ifndef CLAPP_PARSER_TYPES_H
#define CLAPP_PARSER_TYPES_H

#include <clapp/value.h>

#include <functional>
#include <gsl/span>
#include <optional>
#include <string>
#include <string_view>
#include <variant>

namespace clapp {
inline namespace parser {
namespace types {
struct arg_t : public gsl::span<const char* const> {
   public:
    using base = gsl::span<const char* const>;

    inline arg_t(const char* const* argv, int argc);
};

using arg_iterator_t = arg_t::iterator;
using long_opt_func_t = std::function<clapp::value::found_func_t::ret_t(
    const std::string_view option)>;
using long_opt_param_func_t = std::function<clapp::value::found_func_t::ret_t(
    const std::string_view option, const std::string_view param)>;
using short_opt_func_t =
    std::function<clapp::value::found_func_t::ret_t(const char option)>;
using short_opt_param_func_t = std::function<clapp::value::found_func_t::ret_t(
    const char option, const std::string_view param)>;
using argument_func_t = std::function<clapp::value::found_func_t::ret_t(
    const std::string_view argument)>;
using given_func_t = std::function<bool(void)>;
using validate_value_func_t = std::function<void(const std::string&)>;
using validate_func_t = std::function<void(void)>;
using validate_func_vec_t = std::vector<validate_func_t>;
using print_and_exit_func_t = std::function<clapp::value::exit_t(
    const std::string_view print_msg, int exit_code)>;

using long_opt_variant_t = std::variant<long_opt_func_t, long_opt_param_func_t>;
using short_opt_variant_t =
    std::variant<short_opt_func_t, short_opt_param_func_t>;

enum class argument_type_t { single, variadic };
enum class option_type_t { scalar, vector };
enum class logic_operator_type_t { logic_and, logic_xor };
enum class purpose_t { optional, mandatory };

static constexpr std::optional<std::string_view> to_string_view(
    purpose_t purpose) noexcept;

struct help_entry_t {
    std::string name;
    std::string description;

    bool operator==(const help_entry_t& inst) const;
    bool operator!=(const help_entry_t& inst) const;
};

using help_entry_vec_t = std::vector<help_entry_t>;

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

    [[nodiscard]] static std::string create_option_string(
        const std::vector<char>& short_option,
        const std::vector<std::string>& long_option);
    [[nodiscard]] static std::string create_option_string(
        const std::string& long_option);
    [[nodiscard]] static std::string create_option_string(char short_option);
    [[nodiscard]] std::string create_basic_option_string() const;
    [[nodiscard]] std::string create_option_string() const;
    [[nodiscard]] help_entry_t get_option_help() const;
    [[nodiscard]] long_opt_conf_vec_cit_t find_option(
        std::string_view long_option) const;
    [[nodiscard]] short_opt_conf_vec_cit_t find_option(char short_option) const;
    [[nodiscard]] bool contains_option(std::string_view long_option) const;
    [[nodiscard]] bool contains_option(char short_option) const;

    short_opt_conf_vec_t short_options;
    long_opt_conf_vec_t long_options;
    given_func_t given_func;
    validate_value_func_t validate_value_func;
    std::string option_string;
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
using variant_opt_conf_vec_t = std::vector<variant_opt_conf_t>;

struct variant_opt_conf_container_t;
using variant_opt_conf_container_ptr_vec_t =
    std::vector<variant_opt_conf_container_t*>;

struct variant_opt_conf_container_t {
    logic_operator_type_t logic_operator_type;
    variant_opt_conf_vec_t options{};
    variant_opt_conf_container_ptr_vec_t containers{};

    [[nodiscard]] std::string gen_short_option_line() const;
    [[nodiscard]] help_entry_vec_t get_option_help() const;

    [[nodiscard]] const variant_opt_conf_t* find_option(
        std::string_view long_option) const;
    [[nodiscard]] const variant_opt_conf_t* find_option(
        char short_option) const;
};

using variant_opt_conf_container_vec_t =
    std::vector<variant_opt_conf_container_t>;

template <argument_type_t arg_type>
struct basic_reg_argument_conf_t {
    [[nodiscard]] std::string create_basic_argument_string() const;
    [[nodiscard]] std::string create_argument_string() const;
    [[nodiscard]] help_entry_t get_argument_help() const;

    static constexpr argument_type_t argument_type = arg_type;
    argument_func_t argument;
    std::string argument_name;
    std::string description;
    std::optional<validate_func_t> validate_func;
    purpose_t purpose{purpose_t::mandatory};
};

using single_arg_conf_t = basic_reg_argument_conf_t<argument_type_t::single>;
using variadic_arg_conf_t =
    basic_reg_argument_conf_t<argument_type_t::variadic>;
using variant_arg_conf_t = std::variant<single_arg_conf_t, variadic_arg_conf_t>;
using variant_arg_conf_vec_t = std::vector<variant_arg_conf_t>;

template <typename short_option_func_t, typename long_option_func_t>
constexpr static bool is_param_opt();
}  // namespace types
}  // namespace parser
}  // namespace clapp

#include <clapp/parser_types.hpp>

#endif
