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

#ifndef CLAPP_OPTION_HPP
#define CLAPP_OPTION_HPP

#include <clapp/option.h>
#include <clapp/type_traits.h>
#include <clapp/value.h>

#include <numeric>
#include <sstream>

namespace clapp {
inline namespace option {
template <typename T>
basic_param_option_t<T>::~basic_param_option_t() = default;

template <typename T>
basic_vector_param_option_t<T>::~basic_vector_param_option_t() = default;

template <typename T, T default_value>
basic_option_t<T, default_value>::~basic_option_t() = default;

template <int EXIT_CODE>
basic_help_option_t<EXIT_CODE>::~basic_help_option_t() = default;
}  // namespace option
}  // namespace clapp

template <typename T>
inline void clapp::option::gen_opt_conf_process_params(
    [[maybe_unused]] opt_params_t<T>& opt_params) {}

template <typename T, typename Param>
void clapp::option::gen_opt_conf_process_params(opt_params_t<T>& opt_params,
                                                Param&& param) {
    if constexpr (has_append_restriction_text<
                      typename std::decay<Param>::type>::value) {
        opt_params.restrictions.push_back(
            std::string{param.append_restriction_text()});
    }
    if constexpr (has_default_value<typename std::decay<Param>::type>::value) {
        opt_params.default_value = param.default_value();
    }
    if constexpr (has_validate<typename std::decay<Param>::type, T>::value) {
        opt_params.validate_funcs.push_back(
            [param](const T& value, const std::string& option_string) {
                param.validate(value, option_string);
            });
    }
    if constexpr (has_found<typename std::decay<Param>::type>::value) {
        opt_params.found.push_back(param);
    }
    if constexpr (std::is_same<typename std::decay<Param>::type,
                               parser::types::purpose_t>::value) {
        opt_params.purpose = param;
    }
}

template <typename T, typename Param, typename... Params>
void clapp::option::gen_opt_conf_process_params(opt_params_t<T>& opt_params,
                                                Param&& param,
                                                Params&&... parameters) {
    gen_opt_conf_process_params(opt_params, std::forward<Param>(param));
    gen_opt_conf_process_params(opt_params,
                                std::forward<Params>(parameters)...);
}

template <typename short_option_func_t>
std::vector<clapp::parser::types::basic_short_opt_conf_t<short_option_func_t>>
clapp::option::gen_short_option(short_option_func_t&& sof,
                                const std::vector<char>& short_option) {
    std::vector<parser::types::basic_short_opt_conf_t<short_option_func_t>> ret;
    for (const auto opt : short_option) {
        check_short_option(opt);
        ret.emplace_back(
            parser::types::basic_short_opt_conf_t<short_option_func_t>{opt,
                                                                       sof});
    }
    return ret;
}

template <typename long_option_func_t>
std::vector<clapp::parser::types::basic_long_opt_conf_t<long_option_func_t>>
clapp::option::gen_long_option(long_option_func_t&& lof,
                               const std::vector<std::string>& long_option) {
    std::vector<parser::types::basic_long_opt_conf_t<long_option_func_t>> ret;
    for (const auto& opt : long_option) {
        check_long_option(opt);
        ret.emplace_back(
            parser::types::basic_long_opt_conf_t<long_option_func_t>{opt, lof});
    }
    return ret;
}

template <typename T, typename VALUE_FUNC>
clapp::parser::types::validate_value_func_t
clapp::option::gen_opt_validate_value_func(
    VALUE_FUNC&& value_func, has_value_func_t&& hvf,
    std::vector<typename opt_params_t<T>::validate_func_t>&& validate_funcs) {
    if (!validate_funcs.empty()) {
        return [value_func = std::forward<VALUE_FUNC>(value_func),
                has_value_func = std::move(hvf),
                validate_funcs = std::move(validate_funcs)](
                   const std::string& option_string) {
            if (has_value_func()) {
                if constexpr (std::is_same<VALUE_FUNC,
                                           vector_value_func_t<T>>::value) {
                    for (const auto& value : value_func()) {
                        for (const auto& func : validate_funcs) {
                            func(value, option_string);
                        }
                    }
                } else if constexpr (std::is_same<VALUE_FUNC,
                                                  value_func_t<T>>::value) {
                    const T value{value_func()};
                    for (const auto& func : validate_funcs) {
                        func(value, option_string);
                    }
                }
            }
        };
    }
    return [](const std::string& /*option_string*/) {};
}

template <typename T>
inline std::vector<std::string> clapp::option::gen_string_vec(
    const std::vector<T>& vec) {
    return std::vector<std::string>{vec.begin(), vec.end()};
}

template <>
inline std::vector<std::string> clapp::option::gen_string_vec(
    const std::vector<std::string>& vec) {
    return vec;
}

template <typename T, typename OPT_CONF, typename CALLBACKS, typename T1,
          typename... Params>
clapp::option::opt_conf_container_t<T, OPT_CONF> clapp::option::gen_opt_conf(
    CALLBACKS&& callbacks, T1&& single_option, const std::string& description,
    Params&&... parameters) {
    if constexpr (clapp::is_vector<std::decay_t<T1>>::value) {
        if constexpr (std::is_same<std::decay_t<T1>,
                                   std::vector<char>>::value) {
            return gen_opt_conf1<T, OPT_CONF>(
                std::forward<CALLBACKS>(callbacks), std::vector<std::string>{},
                std::forward<T1>(single_option), description,
                std::forward<Params>(parameters)...);
        } else {
            return gen_opt_conf1<T, OPT_CONF>(
                std::forward<CALLBACKS>(callbacks),
                gen_string_vec(std::forward<T1>(single_option)),
                std::vector<char>{}, description,
                std::forward<Params>(parameters)...);
        }
    } else if constexpr (std::is_same<std::decay_t<T1>, char>::value) {
        return gen_opt_conf1<T, OPT_CONF>(
            std::forward<CALLBACKS>(callbacks), std::vector<std::string>{},
            std::vector<char>{std::forward<T1>(single_option)}, description,
            std::forward<Params>(parameters)...);
    } else {
        return gen_opt_conf1<T, OPT_CONF>(
            std::forward<CALLBACKS>(callbacks),
            std::vector<std::string>{{std::forward<T1>(single_option)}},
            std::vector<char>{}, description,
            std::forward<Params>(parameters)...);
    }
}

template <typename T, typename OPT_CONF, typename CALLBACKS, typename T1,
          typename T2, typename... Params>
clapp::option::opt_conf_container_t<T, OPT_CONF> clapp::option::gen_opt_conf(
    CALLBACKS&& callbacks, T1&& long_option, T2&& short_option,
    const std::string& description, Params&&... parameters) {
    if constexpr (clapp::is_vector<typename std::decay<T1>::type>::value &&
                  clapp::is_vector<typename std::decay<T2>::type>::value) {
        return gen_opt_conf1<T, OPT_CONF>(
            std::forward<CALLBACKS>(callbacks),
            gen_string_vec(std::forward<T1>(long_option)),
            std::forward<T2>(short_option), description,
            std::forward<Params>(parameters)...);
    }
    if constexpr (clapp::is_vector<typename std::decay<T1>::type>::value) {
        return gen_opt_conf1<T, OPT_CONF>(
            std::forward<CALLBACKS>(callbacks),
            gen_string_vec(std::forward<T1>(long_option)),
            std::vector<char>{std::forward<T2>(short_option)}, description,
            std::forward<Params>(parameters)...);
    }
    if constexpr (clapp::is_vector<typename std::decay<T2>::type>::value) {
        return gen_opt_conf1<T, OPT_CONF>(
            std::forward<CALLBACKS>(callbacks),
            std::vector<std::string>{std::forward<T1>(long_option)},
            std::forward<T2>(short_option), description,
            std::forward<Params>(parameters)...);
    }
    return gen_opt_conf1<T, OPT_CONF>(
        std::forward<CALLBACKS>(callbacks),
        std::vector<std::string>{std::forward<T1>(long_option)},
        std::vector<char>{std::forward<T2>(short_option)}, description,
        std::forward<Params>(parameters)...);
}

template <typename T, typename OPT_CONF, typename CALLBACKS, typename... Params>
clapp::option::opt_conf_container_t<T, OPT_CONF> clapp::option::gen_opt_conf1(
    CALLBACKS&& callbacks, const std::vector<std::string>& long_option,
    const std::vector<char>& short_option, const std::string& description,
    Params&&... parameters) {
    opt_params_t<T> opt_params;
    if (std::is_same<OPT_CONF, parser::types::opt_vector_param_conf_t>::value) {
        opt_params.restrictions.push_back("vector option");
    }

    gen_opt_conf_process_params(opt_params,
                                std::forward<Params>(parameters)...);

    const std::string purpose{
        types::to_string_view(opt_params.purpose).value()};

    std::string restriction{std::accumulate(
        opt_params.restrictions.begin(), opt_params.restrictions.end(), purpose,
        [](const std::string& lhs, const std::string& rhs) -> std::string {
            const std::string sep{lhs.length() > 0 && rhs.length() > 0 ? ", "
                                                                       : ""};
            return lhs + sep + rhs;
        })};
    if (!restriction.empty()) {
        restriction = " (" + restriction + ")";
    }

    return opt_conf_container_t<T, OPT_CONF>{
        gen_opt_conf2<T, OPT_CONF>(
            std::forward<CALLBACKS>(callbacks), long_option, short_option,
            std::move(opt_params.validate_funcs), description + restriction,
            opt_params.purpose),
        opt_params.default_value, std::move(opt_params.found)};
}

template <typename T, typename OPT_CONF, typename CALLBACKS>
OPT_CONF clapp::option::gen_opt_conf2(
    CALLBACKS&& callbacks, const std::vector<std::string>& long_option,
    const std::vector<char>& short_option,
    std::vector<typename opt_params_t<T>::validate_func_t>&& validate_funcs,
    const std::string& description, parser::types::purpose_t purpose) {
    std::string option_string{
        OPT_CONF::create_option_string(short_option, long_option)};

    using short_opt_func_t = typename std::decay_t<CALLBACKS>::short_opt_func_t;
    using short_opt_conf_vec_t =
        std::vector<parser::types::basic_short_opt_conf_t<short_opt_func_t>>;
    short_opt_conf_vec_t short_options{
        gen_short_option(std::move(callbacks.soh), short_option)};

    using long_opt_func_t = typename std::decay_t<CALLBACKS>::long_opt_func_t;
    using long_opt_conf_vec_t =
        std::vector<parser::types::basic_long_opt_conf_t<long_opt_func_t>>;
    long_opt_conf_vec_t long_options{
        gen_long_option(std::move(callbacks.loh), long_option)};

    using validate_value_func_t = parser::types::validate_value_func_t;
    validate_value_func_t validate_value_func{gen_opt_validate_value_func<T>(
        std::move(callbacks.value), std::move(callbacks.has_value),
        std::move(validate_funcs))};

    return OPT_CONF{std::move(short_options),
                    std::move(long_options),
                    std::move(callbacks.given),
                    std::move(validate_value_func),
                    option_string,
                    description,
                    purpose};
}

template <typename T>
template <typename... Params>
clapp::option::basic_param_option_t<T>::basic_param_option_t(
    basic_option_container_t& container, Params... parameters) {
    opt_conf_container_t<T, opt_conf_t> conf{gen_opt_conf<T, opt_conf_t>(
        create_callbacks(this), std::forward<Params>(parameters)...)};
    container.reg(std::move(conf.opt_conf));
    _value = conf.default_value;
    _found = std::move(conf.found);
}

template <typename T>
typename clapp::option::basic_param_option_t<T>::callbacks_t
clapp::option::basic_param_option_t<T>::create_callbacks(
    basic_param_option_t<T>* inst) {
    return callbacks_t{
        [inst](const std::string_view option, const std::string_view param) {
            return inst->found_entry("--" + std::string{option}, param);
        },
        [inst](const char option, const std::string_view param) {
            return inst->found_entry("-" + std::string{option}, param);
        },
        [inst]() { return inst->given(); },
        [inst]() { return static_cast<bool>(*inst); },
        [inst]() { return inst->value(); }};
}

template <typename T>
clapp::value::found_func_t::ret_t
clapp::option::basic_param_option_t<T>::found_entry(
    const std::string& option, const std::string_view param) {
    for (auto& found_func : _found) {
        const clapp::value::found_func_t::ret_t ret{found_func.found(option)};
        if (ret) {
            return ret;
        }
    }
    _given = true;
    _value = convert_value<T>(param);
    return {};
}

template <typename T>
constexpr clapp::option::basic_param_option_t<T>::operator bool()
    const noexcept {
    return _value.has_value();
}

template <typename T>
constexpr bool clapp::option::basic_param_option_t<T>::has_value()
    const noexcept {
    return _value.has_value();
}

template <typename T>
T clapp::option::basic_param_option_t<T>::value() const {
    if (!_value) {
        throw clapp::exception::value_undefined_t{
            "Requested value is not defined."};
    }
    return T{_value.value()};
}

template <typename T>
constexpr bool clapp::option::basic_param_option_t<T>::given() const noexcept {
    return _given;
}

template <typename T>
template <typename... Params>
clapp::option::basic_vector_param_option_t<T>::basic_vector_param_option_t(
    basic_option_container_t& container, Params... parameters) {
    opt_conf_container_t<T, opt_conf_t> conf{gen_opt_conf<T, opt_conf_t>(
        create_callbacks(this), std::forward<Params>(parameters)...)};
    if (conf.default_value) {
        std::stringstream string_stream;
        string_stream << "No default value for vector based param option '"
                      << conf.opt_conf.create_basic_option_string()
                      << "' possible.";
        throw clapp::exception::option_param_exception_t{string_stream.str()};
    }
    container.reg(std::move(conf.opt_conf));
    _found = std::move(conf.found);
}

template <typename T>
typename clapp::basic_vector_param_option_t<T>::callbacks_t
clapp::option::basic_vector_param_option_t<T>::create_callbacks(
    basic_vector_param_option_t<T>* inst) {
    return callbacks_t{
        [inst](const std::string_view option, const std::string_view param) {
            return inst->found_entry("--" + std::string{option}, param);
        },
        [inst](const char option, const std::string_view param) {
            return inst->found_entry("-" + std::string{option}, param);
        },
        [inst]() { return inst->given(); },
        [inst]() { return static_cast<bool>(*inst); },
        [inst]() { return inst->value(); }};
}

template <typename T>
clapp::value::found_func_t::ret_t
clapp::option::basic_vector_param_option_t<T>::found_entry(
    const std::string& option, const std::string_view param) {
    for (auto& found_func : _found) {
        const clapp::value::found_func_t::ret_t ret{found_func.found(option)};
        if (ret) {
            return ret;
        }
    }
    _given = true;
    _value.push_back(convert_value<T>(param));
    return {};
}

template <typename T>
inline clapp::option::basic_vector_param_option_t<T>::operator bool()
    const noexcept {
    return !_value.empty();
}

template <typename T>
inline bool clapp::option::basic_vector_param_option_t<T>::has_value()
    const noexcept {
    return !_value.empty();
}

template <typename T>
std::vector<T> clapp::option::basic_vector_param_option_t<T>::value() const {
    return _value;
}

template <typename T>
constexpr bool clapp::option::basic_vector_param_option_t<T>::given()
    const noexcept {
    return _given;
}

template <typename T, T default_value>
template <typename... Params>
clapp::option::basic_option_t<T, default_value>::basic_option_t(
    basic_option_container_t& container, callbacks_t&& callbacks,
    Params&&... parameters) {
    opt_conf_container_t<T, opt_conf_t> conf{gen_opt_conf<T, opt_conf_t>(
        callbacks, std::forward<Params>(parameters)...)};
    container.reg(std::move(conf.opt_conf));
    if (conf.default_value) {
        _value = conf.default_value.value();
    }
    _found = std::move(conf.found);
}

template <typename T, T default_value>
T clapp::option::basic_option_t<T, default_value>::value() const {
    return _value;
}

template <typename T, T default_value>
bool clapp::option::basic_option_t<T, default_value>::given() const {
    return _given;
}

template <typename T, T default_value>
constexpr clapp::option::basic_option_t<T, default_value>::operator bool()
    const noexcept {
    return _value != default_value;
}

template <typename T, T default_value>
constexpr bool clapp::option::basic_option_t<T, default_value>::has_value()
    const noexcept {
    return true;
}

template <typename... Params>
clapp::option::bool_option_t::bool_option_t(basic_option_container_t& container,
                                            Params... parameters)
    : clapp::basic_option_t<bool, false>{container, create_callbacks(this),
                                         std::forward<Params>(parameters)...} {
    if (!_value) {
        _value = false;
    }
}

template <int EXIT_CODE>
template <typename... Params>
clapp::option::basic_help_option_t<EXIT_CODE>::basic_help_option_t(
    basic_option_container_t& container, Params... parameters)
    : bool_option_t{container, std::forward<Params>(parameters)...,
                    gen_func_print_help_and_req_exit(container)} {}

template <int EXIT_CODE>
clapp::value::found_func_t
clapp::option::basic_help_option_t<EXIT_CODE>::gen_func_print_help_and_req_exit(
    basic_option_container_t& container) {
    return container.get_parser().gen_func_print_help_and_req_exit(EXIT_CODE);
}

template <typename... Params>
clapp::option::count_option_t::count_option_t(
    basic_option_container_t& container, Params... parameters)
    : clapp::basic_option_t<std::uint32_t, 0U>{
          container, create_callbacks(this),
          std::forward<Params>(parameters)...} {
    if (!_value) {
        _value = 0;
    }
}

#endif
