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

#ifndef LIBCLAPP_CLI_OPTION_H
#define LIBCLAPP_CLI_OPTION_H

#include <clapp/filesystem.h>
#include <clapp/parser.h>
#include <functional>
#include <optional>
#include <string>

namespace clapp {

inline namespace option {
using given_func_t = std::function<bool(void)>;
using has_value_func_t = std::function<bool(void)>;
template <typename T>
using value_func_t = std::function<T(void)>;
template <typename T>
using vector_value_func_t = std::function<std::vector<T>(void)>;

template <typename T>
struct option_callbacks_t {
    basic_parser_t::long_opt_func_t loh;
    basic_parser_t::short_opt_func_t soh;
    std::optional<given_func_t> given;
    std::optional<has_value_func_t> has_value;
    std::optional<value_func_t<T>> value;
};

template <typename T>
struct option_param_callbacks_t {
    basic_parser_t::long_opt_param_func_t loh;
    basic_parser_t::short_opt_param_func_t soh;
    std::optional<given_func_t> given;
    std::optional<has_value_func_t> has_value;
    std::optional<value_func_t<T>> value;
};

template <typename T>
struct option_vector_param_callbacks_t {
    basic_parser_t::long_opt_param_func_t loh;
    basic_parser_t::short_opt_param_func_t soh;
    std::optional<given_func_t> given;
    std::optional<has_value_func_t> has_value;
    std::optional<vector_value_func_t<T>> value;
};

void check_short_option(const char option);
void check_long_option(const std::string_view option);

template <typename T>
struct opt_params_t {
    using validate_func_t =
        std::function<void(const T&, const std::string& option_string)>;
    std::vector<std::string> restrictions;
    std::vector<validate_func_t> validate_funcs;
    basic_parser_t::purpose_t purpose{basic_parser_t::purpose_t::optional};
    std::optional<T> default_value;
};

template <typename T, typename OPT_CONF>
struct opt_conf_container_t {
    OPT_CONF opt_conf;
    std::optional<T> default_value;
};

template <typename T>
class basic_param_option_t {
   public:
    using callbacks_t = option_param_callbacks_t<T>;
    using opt_conf_t = basic_parser_t::opt_scalar_param_conf_t;

    template <typename... Params>
    basic_param_option_t(basic_parser_t& parser, Params... parameters);

    explicit operator bool() const;
    T value() const;
    bool given() const;

    virtual ~basic_param_option_t();

   protected:
    void found_entry(const std::string_view param);
    static callbacks_t create_callbacks(basic_param_option_t<T>* inst);

    std::optional<T> _value;
    bool _given{false};
};

template <typename T>
class basic_vector_param_option_t {
   public:
    using callbacks_t = option_vector_param_callbacks_t<T>;
    using opt_conf_t = basic_parser_t::opt_vector_param_conf_t;

    template <typename... Params>
    basic_vector_param_option_t(basic_parser_t& parser, Params... parameters);

    explicit operator bool() const;
    std::vector<T> value() const;
    bool given() const;

    virtual ~basic_vector_param_option_t();

   protected:
    void found_entry(const std::string_view param);
    static callbacks_t create_callbacks(basic_vector_param_option_t<T>* inst);

    std::vector<T> _value;
    bool _given{false};
};

template <typename T>
class basic_option_t {
   public:
    using callbacks_t = option_callbacks_t<T>;
    using opt_conf_t = basic_parser_t::opt_conf_t;

    template <typename... Params>
    basic_option_t(basic_parser_t& parser, callbacks_t&& callbacks,
                   Params&&... parameters);

    virtual ~basic_option_t();

    T value() const;
    bool given() const;

   protected:
    std::optional<T> _value;
    bool _given{false};
};

class bool_option_t final : public basic_option_t<bool> {
   public:
    template <typename... Params>
    bool_option_t(basic_parser_t& parser, Params... parameters);

    ~bool_option_t() override;

    explicit operator bool() const;

   private:
    void found_entry();
    static callbacks_t create_callbacks(bool_option_t* inst);
};

class count_option_t final : public basic_option_t<std::uint32_t> {
   public:
    template <typename... Params>
    count_option_t(basic_parser_t& parser, Params... parameters);

    ~count_option_t() override;

    explicit operator bool() const;

   private:
    void found_entry();
    static callbacks_t create_callbacks(count_option_t* inst);
};

template <typename T>
inline void gen_opt_conf_process_params(opt_params_t<T>&);

template <typename T, typename Param>
void gen_opt_conf_process_params(opt_params_t<T>& opt_params, Param&& param);

template <typename T, typename Param, typename... Params>
void gen_opt_conf_process_params(opt_params_t<T>& opt_params, Param&& param,
                                 Params&&... parameters);

template <typename short_option_func_t>
std::optional<basic_parser_t::basic_short_opt_conf_t<short_option_func_t>>
gen_short_option(short_option_func_t&& sof,
                 const std::optional<char> short_option);

template <typename long_option_func_t>
std::optional<basic_parser_t::basic_long_opt_conf_t<long_option_func_t>>
gen_long_option(long_option_func_t&& lof,
                const std::optional<std::string>& long_option);

template <typename T, typename VALUE_FUNC>
std::optional<basic_parser_t::validate_func_t> gen_opt_validate_func(
    std::optional<VALUE_FUNC>&& vf, std::optional<has_value_func_t>&& hvf,
    std::optional<given_func_t>&& gf,
    std::vector<typename opt_params_t<T>::validate_func_t>&& validate_funcs,
    const std::string& option_string, const basic_parser_t::purpose_t purpose);

template <typename T, typename OPT_CONF, typename CALLBACKS>
OPT_CONF gen_opt_conf(
    CALLBACKS&& callbacks, const std::optional<std::string>& long_option,
    const std::optional<char> short_option,
    std::vector<typename opt_params_t<T>::validate_func_t>&& validate_funcs,
    const std::string& description, basic_parser_t::purpose_t purpose);

template <typename T, typename OPT_CONF, typename CALLBACKS, typename... Params>
opt_conf_container_t<T, OPT_CONF> gen_opt_conf(
    CALLBACKS&& callbacks, const std::optional<std::string>& long_option,
    const std::optional<char> short_option, const std::string& description,
    Params&&... parameters);

template <typename T, typename OPT_CONF, typename CALLBACKS, typename... Params>
opt_conf_container_t<T, OPT_CONF> gen_opt_conf(CALLBACKS&& callbacks,
                                               const char short_option,
                                               Params&&... parameters);

template <typename T, typename OPT_CONF, typename CALLBACKS, typename... Params>
opt_conf_container_t<T, OPT_CONF> gen_opt_conf(CALLBACKS&& callbacks,
                                               const std::string& long_option,
                                               Params&&... parameters);

template <typename T, typename OPT_CONF, typename CALLBACKS, typename... Params>
opt_conf_container_t<T, OPT_CONF> gen_opt_conf(CALLBACKS&& callbacks,
                                               const std::string& long_option,
                                               const char short_option,
                                               Params&&... parameters);

using string_param_option_t = clapp::basic_param_option_t<std::string>;

#ifdef CLAPP_FS_AVAIL
using path_param_option_t = clapp::basic_param_option_t<clapp::fs::path>;
#endif

using int8_param_option_t = clapp::basic_param_option_t<std::int8_t>;
using uint8_param_option_t = clapp::basic_param_option_t<std::uint8_t>;
using int16_param_option_t = clapp::basic_param_option_t<std::int16_t>;
using uint16_param_option_t = clapp::basic_param_option_t<std::uint16_t>;
using int32_param_option_t = clapp::basic_param_option_t<std::int32_t>;
using uint32_param_option_t = clapp::basic_param_option_t<std::uint32_t>;
using int64_param_option_t = clapp::basic_param_option_t<std::int64_t>;
using uint64_param_option_t = clapp::basic_param_option_t<std::uint64_t>;

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

}  // namespace option

}  // namespace clapp

#include <clapp/option.hpp>

#endif
