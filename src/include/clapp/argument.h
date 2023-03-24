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

#ifndef CLAPP_ARGUMENT_H
#define CLAPP_ARGUMENT_H

#include <clapp/parser.h>
#include <clapp/type_traits.h>
#include <clapp/value.h>

#include <functional>
#include <optional>
#include <string>
#include <vector>

namespace clapp {

inline namespace argument {
using given_func_t = std::function<bool(void)>;
using has_value_func_t = std::function<bool(void)>;
template <typename T>
using arg_value_func_t = std::function<T(void)>;
template <typename T>
using variadic_value_func_t = std::function<std::vector<T>(void)>;

template <typename T>
struct argument_callbacks_t {
    using argument_func_t = basic_parser_t::argument_func_t;
    argument_func_t af;
    std::optional<given_func_t> given;
    std::optional<has_value_func_t> has_value;
    std::optional<arg_value_func_t<T>> value;
};

template <typename T>
struct variadic_argument_callbacks_t {
    using argument_func_t = basic_parser_t::argument_func_t;
    argument_func_t af;
    std::optional<given_func_t> given;
    std::optional<has_value_func_t> has_value;
    std::optional<variadic_value_func_t<T>> value;
};

template <typename T>
struct arg_params_t {
    using validate_func_t =
        std::function<void(const T&, const std::string& option_string)>;
    std::vector<std::string> restrictions{};
    std::vector<validate_func_t> validate_funcs{};
    basic_parser_t::purpose_t purpose{basic_parser_t::purpose_t::mandatory};
    std::optional<T> default_value{};
    std::vector<clapp::value::found_func_t> found{};
};

template <typename T, typename ARG_CONF>
struct arg_conf_container_t {
    ARG_CONF arg_conf;
    std::optional<T> default_value;
    std::vector<clapp::value::found_func_t> found;
};

template <typename T>
inline void gen_arg_conf_process_params(arg_params_t<T>& arg_params);

template <typename T, typename Param>
void gen_arg_conf_process_params(arg_params_t<T>& arg_params, Param&& param);

template <typename T, typename Param, typename... Params>
void gen_arg_conf_process_params(arg_params_t<T>& arg_params, Param&& param,
                                 Params&&... parameters);

template <typename T, typename VALUE_FUNC>
std::optional<basic_parser_t::validate_func_t> gen_arg_validate_func(
    std::optional<VALUE_FUNC>&& vf, std::optional<has_value_func_t>&& hvf,
    std::optional<given_func_t>&& gf,
    std::vector<typename arg_params_t<T>::validate_func_t>&& validate_funcs,
    const std::string& argument_name, const basic_parser_t::purpose_t purpose);

template <typename T, typename ARG_CONF, typename CALLBACKS>
ARG_CONF gen_arg_conf(
    CALLBACKS&& callbacks, const std::string& argument_name,
    std::vector<typename arg_params_t<T>::validate_func_t>&& validate_funcs,
    const std::string& description, basic_parser_t::purpose_t purpose);

template <typename T, typename ARG_CONF, typename CALLBACKS, typename... Params>
arg_conf_container_t<T, ARG_CONF> gen_arg_conf(CALLBACKS&& callbacks,
                                               const std::string& argument_name,
                                               const std::string& description,
                                               Params&&... parameters);

template <typename T>
class basic_argument_t {
   public:
    using value_t = T;
    using callbacks_t = argument_callbacks_t<T>;
    using arg_conf_t = basic_parser_t::single_arg_conf_t;

    template <typename... Params>
    basic_argument_t(basic_parser_t& parser, const std::string& argument_name,
                     const std::string& description, Params&&... parameters);
    [[nodiscard]] constexpr explicit operator bool() const noexcept;
    [[nodiscard]] constexpr bool has_value() const noexcept;
    [[nodiscard]] T value() const;
    [[nodiscard]] constexpr bool given() const noexcept;

    virtual ~basic_argument_t();

   protected:
    void validate() const;
    [[nodiscard]] clapp::value::found_func_t::ret_t found_entry(
        const std::string_view argument);
    [[nodiscard]] static callbacks_t create_callbacks(
        basic_argument_t<T>* inst);

    const std::string _argument_name;
    std::vector<clapp::value::found_func_t> _found{};
    std::optional<T> _value{};
    bool _given{false};
};

template <typename T>
class basic_variadic_argument_t {
   public:
    using value_t = T;
    using callbacks_t = variadic_argument_callbacks_t<T>;
    using arg_conf_t = basic_parser_t::variadic_arg_conf_t;

    template <typename... Params>
    basic_variadic_argument_t(basic_parser_t& parser,
                              const std::string& argument_name,
                              const std::string& description,
                              Params&&... parameters);
    [[nodiscard]] inline explicit operator bool() const noexcept;
    [[nodiscard]] inline bool has_value() const noexcept;
    [[nodiscard]] std::vector<T> value() const;
    [[nodiscard]] constexpr bool given() const noexcept;

    [[nodiscard]] static std::string variadic_argument_restrictions();

   protected:
    void validate() const;
    [[nodiscard]] clapp::value::found_func_t::ret_t found_entry(
        const std::string_view argument);
    [[nodiscard]] static callbacks_t create_callbacks(
        basic_variadic_argument_t<T>* inst);

    const std::string _argument_name;
    std::vector<clapp::value::found_func_t> _found{};
    std::vector<T> _value{};
    bool _given{false};
};

using bool_argument_t = clapp::basic_argument_t<bool>;
using string_argument_t = clapp::basic_argument_t<std::string>;
#ifdef CLAPP_FS_AVAIL
using path_argument_t = clapp::basic_argument_t<clapp::fs::path>;
#endif
using int8_argument_t = clapp::basic_argument_t<std::int8_t>;
using uint8_argument_t = clapp::basic_argument_t<std::uint8_t>;
using int16_argument_t = clapp::basic_argument_t<std::int16_t>;
using uint16_argument_t = clapp::basic_argument_t<std::uint16_t>;
using int32_argument_t = clapp::basic_argument_t<std::int32_t>;
using uint32_argument_t = clapp::basic_argument_t<std::uint32_t>;
using int64_argument_t = clapp::basic_argument_t<std::int64_t>;
using uint64_argument_t = clapp::basic_argument_t<std::uint64_t>;
using ptrdiff_argument_t = clapp::basic_argument_t<std::ptrdiff_t>;
using size_argument_t = clapp::basic_argument_t<std::size_t>;
using double_argument_t = clapp::basic_argument_t<double>;
using float_argument_t = clapp::basic_argument_t<float>;
using ns_argument_t = clapp::basic_argument_t<std::chrono::nanoseconds>;
using us_argument_t = clapp::basic_argument_t<std::chrono::microseconds>;
using ms_argument_t = clapp::basic_argument_t<std::chrono::milliseconds>;
using sec_argument_t = clapp::basic_argument_t<std::chrono::seconds>;
using min_argument_t = clapp::basic_argument_t<std::chrono::minutes>;
using hours_argument_t = clapp::basic_argument_t<std::chrono::hours>;

using variadic_bool_argument_t = clapp::basic_variadic_argument_t<bool>;
using variadic_string_argument_t =
    clapp::basic_variadic_argument_t<std::string>;
#ifdef CLAPP_FS_AVAIL
using variadic_path_argument_t =
    clapp::basic_variadic_argument_t<clapp::fs::path>;
#endif
using variadic_int8_argument_t = clapp::basic_variadic_argument_t<std::int8_t>;
using variadic_uint8_argument_t =
    clapp::basic_variadic_argument_t<std::uint8_t>;
using variadic_int16_argument_t =
    clapp::basic_variadic_argument_t<std::int16_t>;
using variadic_uint16_argument_t =
    clapp::basic_variadic_argument_t<std::uint16_t>;
using variadic_int32_argument_t =
    clapp::basic_variadic_argument_t<std::int32_t>;
using variadic_uint32_argument_t =
    clapp::basic_variadic_argument_t<std::uint32_t>;
using variadic_int64_argument_t =
    clapp::basic_variadic_argument_t<std::int64_t>;
using variadic_uint64_argument_t =
    clapp::basic_variadic_argument_t<std::uint64_t>;
using variadic_ptrdiff_argument_t =
    clapp::basic_variadic_argument_t<std::ptrdiff_t>;
using variadic_size_argument_t = clapp::basic_variadic_argument_t<std::size_t>;
using variadic_double_argument_t = clapp::basic_variadic_argument_t<double>;
using variadic_float_argument_t = clapp::basic_variadic_argument_t<float>;
using variadic_ns_argument_t =
    clapp::basic_variadic_argument_t<std::chrono::nanoseconds>;
using variadic_us_argument_t =
    clapp::basic_variadic_argument_t<std::chrono::microseconds>;
using variadic_ms_argument_t =
    clapp::basic_variadic_argument_t<std::chrono::milliseconds>;
using variadic_sec_argument_t =
    clapp::basic_variadic_argument_t<std::chrono::seconds>;
using variadic_min_argument_t =
    clapp::basic_variadic_argument_t<std::chrono::minutes>;
using variadic_hours_argument_t =
    clapp::basic_variadic_argument_t<std::chrono::hours>;

}  // namespace argument

}  // namespace clapp

#include <clapp/argument.hpp>

#endif
