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

#ifndef LIBCLAPP_CLI_OPTION_HPP
#define LIBCLAPP_CLI_OPTION_HPP

#include <clapp/option.h>
#include <clapp/type_traits.h>
#include <clapp/value.h>
#include <numeric>
#include <sstream>

template <typename T>
inline void clapp::option::gen_opt_conf_process_params(opt_params_t<T>&) {}

template <typename T, typename Param>
void clapp::option::gen_opt_conf_process_params(opt_params_t<T>& opt_params,
                                                Param&& param) {
    if constexpr (has_append_restriction_text<
                      typename std::decay<Param>::type>::value) {
        opt_params.restrictions.push_back(param.append_restriction_text());
    }
    if constexpr (has_default_value<typename std::decay<Param>::type>::value) {
        opt_params.default_value = param.default_value();
    }
    if constexpr (has_validate<typename std::decay<Param>::type>::value) {
        opt_params.validate_funcs.push_back(
            [param](const T& value, const std::string& option_string) {
                param.validate(value, option_string);
            });
    }
    if constexpr (std::is_same<typename std::decay<Param>::type,
                               basic_parser_t::purpose_t>::value) {
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
std::optional<
    clapp::basic_parser_t::basic_short_opt_conf_t<short_option_func_t>>
clapp::option::gen_short_option(short_option_func_t&& sof,
                                const std::optional<char> short_option) {
    if (short_option) {
        check_short_option(short_option.value());
        return basic_parser_t::basic_short_opt_conf_t<short_option_func_t>{
            short_option.value(), std::forward<short_option_func_t>(sof)};
    }
    return std::nullopt;
}

template <typename long_option_func_t>
std::optional<clapp::basic_parser_t::basic_long_opt_conf_t<long_option_func_t>>
clapp::option::gen_long_option(long_option_func_t&& lof,
                               const std::optional<std::string>& long_option) {
    if (long_option) {
        check_long_option(long_option.value());
        return basic_parser_t::basic_long_opt_conf_t<long_option_func_t>{
            long_option.value(), std::forward<long_option_func_t>(lof)};
    }
    return std::nullopt;
}

template <typename T, typename VALUE_FUNC>
std::optional<clapp::basic_parser_t::validate_func_t>
clapp::option::gen_opt_validate_func(
    std::optional<VALUE_FUNC>&& vf, std::optional<has_value_func_t>&& hvf,
    std::optional<given_func_t>&& gf,
    std::vector<typename opt_params_t<T>::validate_func_t>&& validate_funcs,
    const std::string& option_string, const basic_parser_t::purpose_t purpose) {
    if (validate_funcs.size() > 0 ||
        purpose == basic_parser_t::purpose_t::mandatory) {
        return [purpose, value_func = std::move(vf),
                has_value_func = std::move(hvf), given_func = std::move(gf),
                option_string, validate_funcs = std::move(validate_funcs)]() {
            if (purpose == basic_parser_t::purpose_t::mandatory && given_func) {
                if (!given_func.value()()) {
                    throw std::runtime_error(
                        std::string{"Mandatory parameter for option '"} +
                        option_string + "' not given.");
                }
            }
            if (value_func && has_value_func) {
                if (has_value_func.value()()) {
                    if constexpr (std::is_same<VALUE_FUNC,
                                               vector_value_func_t<T>>::value) {
                        const std::vector<T> values{value_func.value()()};
                        for (auto& value : values) {
                            for (auto& func : validate_funcs) {
                                func(value, option_string);
                            }
                        }
                    } else if constexpr (std::is_same<VALUE_FUNC,
                                                      value_func_t<T>>::value) {
                        const T value{value_func.value()()};
                        for (auto& func : validate_funcs) {
                            func(value, option_string);
                        }
                    }
                }
            } else {
                if (validate_funcs.size() > 0) {
                    throw std::runtime_error(
                        std::string{"Cannot validate option '"} +
                        option_string +
                        "' without a given value and has_value function.");
                }
            }
        };
    }
    return std::nullopt;
}

template <typename T, typename OPT_CONF, typename CALLBACKS>
OPT_CONF clapp::option::gen_opt_conf(
    CALLBACKS&& callbacks, const std::optional<std::string>& long_option,
    const std::optional<char> short_option,
    std::vector<typename opt_params_t<T>::validate_func_t>&& validate_funcs,
    const std::string& description, basic_parser_t::purpose_t purpose) {
    std::string option_string{
        OPT_CONF::create_option_string(short_option, long_option)};
    return OPT_CONF{gen_short_option(std::move(callbacks.soh), short_option),
                    gen_long_option(std::move(callbacks.loh), long_option),
                    gen_opt_validate_func<T>(std::move(callbacks.value),
                                             std::move(callbacks.has_value),
                                             std::move(callbacks.given),
                                             std::move(validate_funcs),
                                             option_string, purpose),
                    std::move(option_string),
                    description,
                    purpose};
}

template <typename T, typename OPT_CONF, typename CALLBACKS, typename... Params>
clapp::option::opt_conf_container_t<T, OPT_CONF> clapp::option::gen_opt_conf(
    CALLBACKS&& callbacks, const std::optional<std::string>& long_option,
    const std::optional<char> short_option, const std::string& description,
    Params&&... parameters) {
    opt_params_t<T> opt_params;
    if (std::is_same<
            OPT_CONF,
            basic_parser_t::basic_parser_t::opt_vector_param_conf_t>::value) {
        opt_params.restrictions.push_back("vector option");
    }

    gen_opt_conf_process_params(opt_params,
                                std::forward<Params>(parameters)...);

    std::string restriction{std::accumulate(
        opt_params.restrictions.begin(), opt_params.restrictions.end(),
        std::string(),
        [](const std::string& a, const std::string& b) -> std::string {
            return a + (a.length() > 0 && b.length() > 0 ? ", " : "") + b;
        })};
    if (restriction.size() > 0) {
        restriction = " (" + restriction + ")";
    }

    return opt_conf_container_t<T, OPT_CONF>{
        gen_opt_conf<T, OPT_CONF>(
            std::forward<CALLBACKS>(callbacks), long_option, short_option,
            std::move(opt_params.validate_funcs), description + restriction,
            opt_params.purpose),
        opt_params.default_value};
}

template <typename T, typename OPT_CONF, typename CALLBACKS, typename... Params>
clapp::option::opt_conf_container_t<T, OPT_CONF> clapp::option::gen_opt_conf(
    CALLBACKS&& callbacks, const char short_option, Params&&... parameters) {
    return gen_opt_conf<T, OPT_CONF>(
        std::forward<CALLBACKS>(callbacks), std::nullopt,
        std::make_optional(short_option), std::forward<Params>(parameters)...);
}

template <typename T, typename OPT_CONF, typename CALLBACKS, typename... Params>
clapp::option::opt_conf_container_t<T, OPT_CONF> clapp::option::gen_opt_conf(
    CALLBACKS&& callbacks, const std::string& long_option,
    Params&&... parameters) {
    return gen_opt_conf<T, OPT_CONF>(
        std::forward<CALLBACKS>(callbacks), std::make_optional(long_option),
        std::nullopt, std::forward<Params>(parameters)...);
}

template <typename T, typename OPT_CONF, typename CALLBACKS, typename... Params>
clapp::option::opt_conf_container_t<T, OPT_CONF> clapp::option::gen_opt_conf(
    CALLBACKS&& callbacks, const std::string& long_option,
    const char short_option, Params&&... parameters) {
    return gen_opt_conf<T, OPT_CONF>(
        std::forward<CALLBACKS>(callbacks), std::make_optional(long_option),
        std::make_optional(short_option), std::forward<Params>(parameters)...);
}

template <typename T>
template <typename... Params>
clapp::option::basic_param_option_t<T>::basic_param_option_t(
    clapp::basic_parser_t& parser, Params... parameters) {
    opt_conf_container_t<T, opt_conf_t> conf{gen_opt_conf<T, opt_conf_t>(
        create_callbacks(this), std::forward<Params>(parameters)...)};
    parser.reg(std::move(conf.opt_conf));
    _value = conf.default_value;
}

template <typename T>
clapp::option::basic_param_option_t<T>::~basic_param_option_t() = default;

template <typename T>
typename clapp::option::basic_param_option_t<T>::callbacks_t
clapp::option::basic_param_option_t<T>::create_callbacks(
    basic_param_option_t<T>* inst) {
    return callbacks_t{
        [inst](const std::string_view /*option*/,
               const std::string_view param) { inst->found_entry(param); },
        [inst](const char /*option*/, const std::string_view param) {
            inst->found_entry(param);
        },
        [inst]() { return inst->given(); },
        [inst]() { return static_cast<bool>(*inst); },
        [inst]() { return inst->value(); }};
}

template <typename T>
void clapp::option::basic_param_option_t<T>::found_entry(
    const std::string_view param) {
    _given = true;
    _value = convert_value<T>(param);
}

template <typename T>
clapp::option::basic_param_option_t<T>::operator bool() const {
    return _value.has_value();
}

template <typename T>
T clapp::option::basic_param_option_t<T>::value() const {
    return _value.value();
}

template <typename T>
bool clapp::option::basic_param_option_t<T>::given() const {
    return _given;
}

template <typename T>
template <typename... Params>
clapp::option::basic_vector_param_option_t<T>::basic_vector_param_option_t(
    clapp::basic_parser_t& parser, Params... parameters) {
    opt_conf_container_t<T, opt_conf_t> conf{gen_opt_conf<T, opt_conf_t>(
        create_callbacks(this), std::forward<Params>(parameters)...)};
    if (conf.default_value) {
        std::stringstream ss;
        ss << "No default value for vector based param option '"
           << conf.opt_conf.option_string << "' possible.";
        throw std::runtime_error(ss.str());
    }
    parser.reg(std::move(conf.opt_conf));
}

template <typename T>
clapp::option::basic_vector_param_option_t<T>::~basic_vector_param_option_t() =
    default;

template <typename T>
typename clapp::basic_vector_param_option_t<T>::callbacks_t
clapp::option::basic_vector_param_option_t<T>::create_callbacks(
    basic_vector_param_option_t<T>* inst) {
    return callbacks_t{
        [inst](const std::string_view /*option*/,
               const std::string_view param) { inst->found_entry(param); },
        [inst](const char /*option*/, const std::string_view param) {
            inst->found_entry(param);
        },
        [inst]() { return inst->given(); },
        [inst]() { return static_cast<bool>(*inst); },
        [inst]() { return inst->value(); }};
}

template <typename T>
void clapp::option::basic_vector_param_option_t<T>::found_entry(
    const std::string_view param) {
    _given = true;
    _value.push_back(convert_value<T>(param));
}

template <typename T>
clapp::option::basic_vector_param_option_t<T>::operator bool() const {
    return _value.size() > 0;
}

template <typename T>
std::vector<T> clapp::option::basic_vector_param_option_t<T>::value() const {
    return _value;
}

template <typename T>
bool clapp::option::basic_vector_param_option_t<T>::given() const {
    return _given;
}

template <typename T>
template <typename... Params>
clapp::option::basic_option_t<T>::basic_option_t(clapp::basic_parser_t& parser,
                                                 callbacks_t&& callbacks,
                                                 Params&&... parameters) {
    opt_conf_container_t<T, opt_conf_t> conf{gen_opt_conf<T, opt_conf_t>(
        callbacks, std::forward<Params>(parameters)...)};
    parser.reg(std::move(conf.opt_conf));
    _value = conf.default_value;
}

template <typename T>
clapp::option::basic_option_t<T>::~basic_option_t() = default;

template <typename T>
T clapp::option::basic_option_t<T>::value() const {
    return _value.value();
}

template <typename T>
bool clapp::option::basic_option_t<T>::given() const {
    return _given;
}

template <typename... Params>
clapp::option::bool_option_t::bool_option_t(clapp::basic_parser_t& parser,
                                            Params... parameters)
    : clapp::basic_option_t<bool>{parser, create_callbacks(this),
                                  std::forward<Params>(parameters)...} {
    if (!_value) {
        _value = false;
    }
}

template <typename... Params>
clapp::option::count_option_t::count_option_t(clapp::basic_parser_t& parser,
                                              Params... parameters)
    : clapp::basic_option_t<std::uint32_t>{
          parser, create_callbacks(this), std::forward<Params>(parameters)...} {
    if (!_value) {
        _value = 0;
    }
}

#endif
