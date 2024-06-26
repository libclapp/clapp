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

#ifndef CLAPP_OPTION_H
#define CLAPP_OPTION_H

#include <clapp/filesystem.h>
#include <clapp/parser.h>
#include <clapp/value.h>

#include <functional>
#include <optional>
#include <string>

namespace clapp {

inline namespace option {
using given_func_t = parser::types::given_func_t;
using has_value_func_t = std::function<bool(void)>;
template <typename T>
using value_func_t = std::function<T(void)>;
template <typename T>
using vector_value_func_t = std::function<std::vector<T>(void)>;

template <typename T>
struct option_callbacks_t {
    using long_opt_func_t = parser::types::long_opt_func_t;
    using short_opt_func_t = parser::types::short_opt_func_t;
    long_opt_func_t loh;
    short_opt_func_t soh;
    given_func_t given;
    has_value_func_t has_value;
    value_func_t<T> value;
};

template <typename T>
struct option_param_callbacks_t {
    using long_opt_func_t = parser::types::long_opt_param_func_t;
    using short_opt_func_t = parser::types::short_opt_param_func_t;
    long_opt_func_t loh;
    short_opt_func_t soh;
    given_func_t given;
    has_value_func_t has_value;
    value_func_t<T> value;
};

template <typename T>
struct option_vector_param_callbacks_t {
    using long_opt_func_t = parser::types::long_opt_param_func_t;
    using short_opt_func_t = parser::types::short_opt_param_func_t;
    long_opt_func_t loh;
    short_opt_func_t soh;
    given_func_t given;
    has_value_func_t has_value;
    vector_value_func_t<T> value;
};

void check_short_option(char option);
void check_long_option(std::string_view option);

template <typename T>
inline std::vector<std::string> gen_string_vec(const std::vector<T>& vec);

template <typename T>
struct opt_params_t {
    using validate_func_t =
        std::function<void(const T&, const std::string& option_string)>;
    std::vector<std::string> restrictions{};
    std::vector<validate_func_t> validate_funcs{};
    parser::types::purpose_t purpose{parser::types::purpose_t::mandatory};
    std::optional<T> default_value{};
    std::vector<clapp::value::found_func_t> found{};
};

template <typename T, typename OPT_CONF>
struct opt_conf_container_t {
    OPT_CONF opt_conf;
    std::optional<T> default_value;
    std::vector<clapp::value::found_func_t> found;
};

template <typename T>
class basic_param_option_t {
   public:
    using value_t = T;
    using callbacks_t = option_param_callbacks_t<T>;
    using opt_conf_t = parser::types::opt_scalar_param_conf_t;

    template <typename... Params>
    explicit basic_param_option_t(basic_option_container_t& container,
                                  Params... parameters);
    explicit basic_param_option_t(const basic_param_option_t&) = delete;
    explicit basic_param_option_t(basic_param_option_t&&) noexcept = delete;
    basic_param_option_t& operator=(const basic_param_option_t&) = delete;
    basic_param_option_t& operator=(basic_param_option_t&&) noexcept = delete;

    [[nodiscard]] constexpr explicit operator bool() const noexcept;
    [[nodiscard]] constexpr bool has_value() const noexcept;
    [[nodiscard]] T value() const;
    [[nodiscard]] constexpr bool given() const noexcept;

    virtual ~basic_param_option_t();

   protected:
    [[nodiscard]] clapp::value::found_func_t::ret_t found_entry(
        const std::string& option, std::string_view param);
    [[nodiscard]] static callbacks_t create_callbacks(
        basic_param_option_t<T>* inst);

    std::vector<clapp::value::found_func_t> _found{};
    std::optional<T> _value{};
    bool _given{false};
};

template <typename T>
class basic_vector_param_option_t {
   public:
    using value_t = T;
    using callbacks_t = option_vector_param_callbacks_t<T>;
    using opt_conf_t = parser::types::opt_vector_param_conf_t;

    template <typename... Params>
    explicit basic_vector_param_option_t(basic_option_container_t& container,
                                         Params... parameters);
    explicit basic_vector_param_option_t(const basic_vector_param_option_t&) =
        delete;
    explicit basic_vector_param_option_t(
        basic_vector_param_option_t&&) noexcept = delete;
    basic_vector_param_option_t& operator=(const basic_vector_param_option_t&) =
        delete;
    basic_vector_param_option_t& operator=(
        basic_vector_param_option_t&&) noexcept = delete;

    [[nodiscard]] inline explicit operator bool() const noexcept;
    [[nodiscard]] inline bool has_value() const noexcept;
    [[nodiscard]] std::vector<T> value() const;
    [[nodiscard]] constexpr bool given() const noexcept;

    virtual ~basic_vector_param_option_t();

   protected:
    [[nodiscard]] clapp::value::found_func_t::ret_t found_entry(
        const std::string& option, std::string_view param);
    [[nodiscard]] static callbacks_t create_callbacks(
        basic_vector_param_option_t<T>* inst);

    std::vector<clapp::value::found_func_t> _found{};
    std::vector<T> _value{};
    bool _given{false};
};

template <typename T, T default_value_param>
class basic_option_t {
   public:
    using value_t = T;
    using callbacks_t = option_callbacks_t<T>;
    using opt_conf_t = parser::types::opt_no_param_conf_t;

    template <typename... Params>
    basic_option_t(basic_option_container_t& container, callbacks_t&& callbacks,
                   Params&&... parameters);
    explicit basic_option_t(const basic_option_t&) = delete;
    explicit basic_option_t(basic_option_t&&) noexcept = delete;
    basic_option_t& operator=(const basic_option_t&) = delete;
    basic_option_t& operator=(basic_option_t&&) noexcept = delete;

    virtual ~basic_option_t();

    [[nodiscard]] constexpr explicit operator bool() const noexcept;
    [[nodiscard]] constexpr bool has_value() const noexcept;
    [[nodiscard]] T value() const;
    [[nodiscard]] bool given() const;

    static constexpr T default_value = default_value_param;

   protected:
    std::vector<clapp::value::found_func_t> _found{};
    T _value{default_value};
    bool _given{false};
};

class bool_option_t : public basic_option_t<bool, false> {
   public:
    template <typename... Params>
    explicit bool_option_t(basic_option_container_t& container,
                           Params... parameters);
    explicit bool_option_t(const bool_option_t&) = delete;
    explicit bool_option_t(bool_option_t&&) noexcept = delete;
    bool_option_t& operator=(const bool_option_t&) = delete;
    bool_option_t& operator=(bool_option_t&&) noexcept = delete;

    ~bool_option_t() override;

   private:
    [[nodiscard]] clapp::value::found_func_t::ret_t found_entry(
        const std::string& option);
    [[nodiscard]] static callbacks_t create_callbacks(bool_option_t* inst);
};

template <int EXIT_CODE>
class basic_help_option_t : public bool_option_t {
   public:
    static constexpr int exit_code{EXIT_CODE};

    template <typename... Params>
    explicit basic_help_option_t(basic_option_container_t& container,
                                 Params... parameters);
    explicit basic_help_option_t(const basic_help_option_t&) = delete;
    explicit basic_help_option_t(basic_help_option_t&&) noexcept = delete;
    basic_help_option_t& operator=(const basic_help_option_t&) = delete;
    basic_help_option_t& operator=(basic_help_option_t&&) noexcept = delete;

    ~basic_help_option_t() override;

    [[nodiscard]] static value::found_func_t gen_func_print_help_and_req_exit(
        basic_option_container_t& container);
};

class count_option_t : public basic_option_t<std::uint32_t, 0U> {
   public:
    template <typename... Params>
    explicit count_option_t(basic_option_container_t& container,
                            Params... parameters);
    explicit count_option_t(const count_option_t&) = delete;
    explicit count_option_t(count_option_t&&) noexcept = delete;
    count_option_t& operator=(const count_option_t&) = delete;
    count_option_t& operator=(count_option_t&&) noexcept = delete;

    ~count_option_t() override;

   private:
    [[nodiscard]] clapp::value::found_func_t::ret_t found_entry(
        const std::string& option);
    [[nodiscard]] static callbacks_t create_callbacks(count_option_t* inst);
};

template <typename T>
inline void gen_opt_conf_process_params(opt_params_t<T>& opt_params);

template <typename T, typename Param>
void gen_opt_conf_process_params(opt_params_t<T>& opt_params, Param&& param);

template <typename T, typename Param, typename... Params>
void gen_opt_conf_process_params(opt_params_t<T>& opt_params, Param&& param,
                                 Params&&... parameters);

template <typename short_option_func_t>
std::vector<parser::types::basic_short_opt_conf_t<short_option_func_t>>
gen_short_option(short_option_func_t&& sof,
                 const std::vector<char>& short_option);

template <typename long_option_func_t>
std::vector<parser::types::basic_long_opt_conf_t<long_option_func_t>>
gen_long_option(long_option_func_t&& lof,
                const std::vector<std::string>& long_option);

template <typename T, typename VALUE_FUNC>
parser::types::validate_value_func_t gen_opt_validate_value_func(
    VALUE_FUNC&& value_func, has_value_func_t&& hvf,
    std::vector<typename opt_params_t<T>::validate_func_t>&& validate_funcs);

template <typename T, typename OPT_CONF, typename CALLBACKS, typename T1,
          typename... Params>
opt_conf_container_t<T, OPT_CONF> gen_opt_conf(CALLBACKS&& callbacks,
                                               T1&& single_option,
                                               const std::string& description,
                                               Params&&... parameters);

template <typename T, typename OPT_CONF, typename CALLBACKS, typename T1,
          typename T2, typename... Params>
opt_conf_container_t<T, OPT_CONF> gen_opt_conf(CALLBACKS&& callbacks,
                                               T1&& long_option,
                                               T2&& short_option,
                                               const std::string& description,
                                               Params&&... parameters);

template <typename T, typename OPT_CONF, typename CALLBACKS, typename... Params>
opt_conf_container_t<T, OPT_CONF> gen_opt_conf1(
    CALLBACKS&& callbacks, const std::vector<std::string>& long_option,
    const std::vector<char>& short_option, const std::string& description,
    Params&&... parameters);

template <typename T, typename OPT_CONF, typename CALLBACKS>
OPT_CONF gen_opt_conf2(
    CALLBACKS&& callbacks, const std::vector<std::string>& long_option,
    const std::vector<char>& short_option,
    std::vector<typename opt_params_t<T>::validate_func_t>&& validate_funcs,
    const std::string& description, parser::types::purpose_t purpose);

using help_option_t = basic_help_option_t<EXIT_SUCCESS>;

using string_param_option_t = clapp::basic_param_option_t<std::string>;

#ifdef CLAPP_FS_AVAIL
using path_param_option_t = clapp::basic_param_option_t<clapp::fs::path>;
#endif

using bool_param_option_t = clapp::basic_param_option_t<bool>;
using int8_param_option_t = clapp::basic_param_option_t<std::int8_t>;
using uint8_param_option_t = clapp::basic_param_option_t<std::uint8_t>;
using int16_param_option_t = clapp::basic_param_option_t<std::int16_t>;
using uint16_param_option_t = clapp::basic_param_option_t<std::uint16_t>;
using int32_param_option_t = clapp::basic_param_option_t<std::int32_t>;
using uint32_param_option_t = clapp::basic_param_option_t<std::uint32_t>;
using int64_param_option_t = clapp::basic_param_option_t<std::int64_t>;
using uint64_param_option_t = clapp::basic_param_option_t<std::uint64_t>;
using ptrdiff_param_option_t = clapp::basic_param_option_t<std::ptrdiff_t>;
using size_param_option_t = clapp::basic_param_option_t<std::size_t>;
using double_param_option_t = clapp::basic_param_option_t<double>;
using float_param_option_t = clapp::basic_param_option_t<float>;
using ns_param_option_t = clapp::basic_param_option_t<std::chrono::nanoseconds>;
using us_param_option_t =
    clapp::basic_param_option_t<std::chrono::microseconds>;
using ms_param_option_t =
    clapp::basic_param_option_t<std::chrono::milliseconds>;
using sec_param_option_t = clapp::basic_param_option_t<std::chrono::seconds>;
using min_param_option_t = clapp::basic_param_option_t<std::chrono::minutes>;
using hours_param_option_t = clapp::basic_param_option_t<std::chrono::hours>;

using vector_string_param_option_t =
    clapp::basic_vector_param_option_t<std::string>;

#ifdef CLAPP_FS_AVAIL
using vector_path_param_option_t =
    clapp::basic_vector_param_option_t<clapp::fs::path>;
#endif

using vector_int8_param_option_t =
    clapp::basic_vector_param_option_t<std::int8_t>;
using vector_uint8_param_option_t =
    clapp::basic_vector_param_option_t<std::uint8_t>;
using vector_int16_param_option_t =
    clapp::basic_vector_param_option_t<std::int16_t>;
using vector_uint16_param_option_t =
    clapp::basic_vector_param_option_t<std::uint16_t>;
using vector_int32_param_option_t =
    clapp::basic_vector_param_option_t<std::int32_t>;
using vector_uint32_param_option_t =
    clapp::basic_vector_param_option_t<std::uint32_t>;
using vector_int64_param_option_t =
    clapp::basic_vector_param_option_t<std::int64_t>;
using vector_uint64_param_option_t =
    clapp::basic_vector_param_option_t<std::uint64_t>;
using vector_ptrdiff_param_option_t =
    clapp::basic_vector_param_option_t<std::ptrdiff_t>;
using vector_size_param_option_t =
    clapp::basic_vector_param_option_t<std::size_t>;
using vector_double_param_option_t = clapp::basic_vector_param_option_t<double>;
using vector_float_param_option_t = clapp::basic_vector_param_option_t<float>;
using vector_ns_param_option_t =
    clapp::basic_vector_param_option_t<std::chrono::nanoseconds>;
using vector_us_param_option_t =
    clapp::basic_vector_param_option_t<std::chrono::microseconds>;
using vector_ms_param_option_t =
    clapp::basic_vector_param_option_t<std::chrono::milliseconds>;
using vector_sec_param_option_t =
    clapp::basic_vector_param_option_t<std::chrono::seconds>;
using vector_min_param_option_t =
    clapp::basic_vector_param_option_t<std::chrono::minutes>;
using vector_hours_param_option_t =
    clapp::basic_vector_param_option_t<std::chrono::hours>;

}  // namespace option

}  // namespace clapp

#include <clapp/option.hpp>

#endif
