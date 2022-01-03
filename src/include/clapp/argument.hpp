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

#ifndef CLAPP_ARGUMENT_HPP
#define CLAPP_ARGUMENT_HPP

#include <clapp/argument.h>
#include <clapp/value.h>

#include <numeric>

namespace clapp {
inline namespace argument {
template <typename T>
basic_argument_t<T>::~basic_argument_t() = default;

}  // namespace argument
}  // namespace clapp

template <typename T>
clapp::argument::basic_argument_t<T>::basic_argument_t(
    basic_argument_t&& inst) noexcept = default;

template <typename T>
clapp::argument::basic_argument_t<T>::basic_argument_t(
    const basic_argument_t& inst) = default;

template <typename T>
clapp::argument::basic_argument_t<T>&
clapp::argument::basic_argument_t<T>::operator=(
    basic_argument_t&& inst) noexcept = default;

template <typename T>
clapp::argument::basic_argument_t<T>&
clapp::argument::basic_argument_t<T>::operator=(const basic_argument_t& inst) =
    default;

template <typename T>
inline void clapp::argument::gen_arg_conf_process_params(
    arg_params_t<T>& /*arg_params*/) {}

template <typename T, typename Param>
void clapp::argument::gen_arg_conf_process_params(arg_params_t<T>& arg_params,
                                                  Param&& param) {
    if constexpr (has_append_restriction_text<
                      typename std::decay<Param>::type>::value) {
        arg_params.restrictions.push_back(
            std::string{param.append_restriction_text()});
    }
    if constexpr (has_default_value<typename std::decay<Param>::type>::value) {
        arg_params.default_value = param.default_value();
    }
    if constexpr (has_validate<typename std::decay<Param>::type, T>::value) {
        arg_params.validate_funcs.push_back(
            [param](const T& value, const std::string& argument_name) {
                param.validate(value, argument_name);
            });
    }
    if constexpr (has_found<typename std::decay<Param>::type>::value) {
        arg_params.found.push_back(param);
    }
    if constexpr (std::is_same<typename std::decay<Param>::type,
                               parser::types::purpose_t>::value) {
        arg_params.purpose = param;
    }
}

template <typename T, typename Param, typename... Params>
void clapp::argument::gen_arg_conf_process_params(arg_params_t<T>& arg_params,
                                                  Param&& param,
                                                  Params&&... parameters) {
    gen_arg_conf_process_params(arg_params, std::forward<Param>(param));
    gen_arg_conf_process_params(arg_params,
                                std::forward<Params>(parameters)...);
}

template <typename T, typename VALUE_FUNC>
std::optional<clapp::parser::types::validate_func_t>
clapp::argument::gen_arg_validate_func(
    std::optional<VALUE_FUNC>&& vf, std::optional<has_value_func_t>&& hvf,
    std::optional<given_func_t>&& gf,
    std::vector<typename arg_params_t<T>::validate_func_t>&& validate_funcs,
    const std::string& argument_name, const parser::types::purpose_t purpose) {
    if (validate_funcs.size() > 0 ||
        purpose == parser::types::purpose_t::mandatory) {
        return [purpose, value_func = std::move(vf),
                has_value_func = std::move(hvf), given_func = std::move(gf),
                argument_name, validate_funcs = std::move(validate_funcs)]() {
            if (purpose == parser::types::purpose_t::mandatory && given_func) {
                if (!given_func.value()()) {
                    throw clapp::exception::argument_exception_t(
                        std::string{"Mandatory argument '"} + argument_name +
                        "' not given.");
                }
            }
            if (value_func) {
                if (has_value_func && has_value_func.value()()) {
                    if constexpr (std::is_same<
                                      VALUE_FUNC,
                                      variadic_value_func_t<T>>::value) {
                        const std::vector<T> values{value_func.value()()};
                        for (const auto& func : validate_funcs) {
                            if constexpr (std::is_same<T, bool>::value) {
                                for (const auto value : values) {
                                    func(value, argument_name);
                                }
                            } else {
                                for (const auto& value : values) {
                                    func(value, argument_name);
                                }
                            }
                        }
                    } else if constexpr (std::is_same<
                                             VALUE_FUNC,
                                             arg_value_func_t<T>>::value) {
                        const T value{value_func.value()()};
                        for (const auto& func : validate_funcs) {
                            func(value, argument_name);
                        }
                    }
                }
            } else {
                if (validate_funcs.size() > 0) {
                    throw clapp::exception::argument_exception_t(
                        std::string{"Cannot validate argument '"} +
                        argument_name + "' without a given value function.");
                }
            }
        };
    }
    return std::nullopt;
}

template <typename T, typename ARG_CONF, typename CALLBACKS>
ARG_CONF clapp::argument::gen_arg_conf(
    CALLBACKS&& callbacks, const std::string& argument_name,
    std::vector<typename arg_params_t<T>::validate_func_t>&& validate_funcs,
    const std::string& description, parser::types::purpose_t purpose) {
    using optional_arg_validate_func_t =
        std::optional<parser::types::validate_func_t>;
    optional_arg_validate_func_t arg_validate_func{gen_arg_validate_func<T>(
        std::move(callbacks.value), std::move(callbacks.has_value),
        std::move(callbacks.given), std::move(validate_funcs), argument_name,
        purpose)};

    return ARG_CONF{std::move(callbacks.af), argument_name, description,
                    std::move(arg_validate_func), purpose};
}

template <typename T, typename ARG_CONF, typename CALLBACKS, typename... Params>
clapp::argument::arg_conf_container_t<T, ARG_CONF>
clapp::argument::gen_arg_conf(CALLBACKS&& callbacks,
                              const std::string& argument_name,
                              const std::string& description,
                              Params&&... parameters) {
    arg_params_t<T> arg_params;
    if (std::is_same<ARG_CONF, parser::types::variadic_arg_conf_t>::value) {
        arg_params.restrictions.push_back(
            clapp::argument::basic_variadic_argument_t<
                T>::variadic_argument_restrictions());
    }
    gen_arg_conf_process_params(arg_params,
                                std::forward<Params>(parameters)...);

    const std::string purpose{
        parser::types::to_string_view(arg_params.purpose).value()};

    std::string restriction{std::accumulate(
        arg_params.restrictions.begin(), arg_params.restrictions.end(), purpose,
        [](const std::string& lhs, const std::string& rhs) -> std::string {
            const std::string separator{
                lhs.length() > 0 && rhs.length() > 0 ? ", " : ""};
            return lhs + separator + rhs;
        })};
    if (!restriction.empty()) {
        restriction = " (" + restriction + ")";
    }

    return arg_conf_container_t<T, ARG_CONF>{
        gen_arg_conf<T, ARG_CONF>(
            std::forward<CALLBACKS>(callbacks), argument_name,
            std::move(arg_params.validate_funcs), description + restriction,
            arg_params.purpose),
        arg_params.default_value, std::move(arg_params.found)};
}

template <typename T>
typename clapp::argument::basic_argument_t<T>::callbacks_t
clapp::argument::basic_argument_t<T>::create_callbacks(
    basic_argument_t<T>* inst) {
    return callbacks_t{[inst](const std::string_view argument) {
                           return inst->found_entry(argument);
                       },
                       [inst]() { return inst->given(); },
                       [inst]() { return static_cast<bool>(*inst); },
                       [inst]() { return inst->value(); }};
}

template <typename T>
template <typename... Params>
clapp::argument::basic_argument_t<T>::basic_argument_t(
    basic_parser_t& parser, const std::string& argument_name,
    const std::string& description, Params&&... parameters)
    : _argument_name{argument_name} {
    arg_conf_container_t<T, arg_conf_t> conf{gen_arg_conf<T, arg_conf_t>(
        create_callbacks(this), argument_name, description,
        std::forward<Params>(parameters)...)};
    parser.reg(std::move(conf.arg_conf));
    _value = conf.default_value;
    _found = std::move(conf.found);
}

template <typename T>
constexpr clapp::argument::basic_argument_t<T>::operator bool() const noexcept {
    return _value.has_value();
}

template <typename T>
constexpr bool clapp::argument::basic_argument_t<T>::has_value()
    const noexcept {
    return _value.has_value();
}

template <typename T>
T clapp::argument::basic_argument_t<T>::value() const {
    if (!_value) {
        throw clapp::exception::value_undefined_t{
            "Requested argument value is not defined."};
    }
    return T{_value.value()};
}

template <typename T>
constexpr bool clapp::argument::basic_argument_t<T>::given() const noexcept {
    return _given;
}

template <typename T>
clapp::value::found_func_t::ret_t
clapp::argument::basic_argument_t<T>::found_entry(
    const std::string_view argument) {
    for (auto& found_func : _found) {
        return found_func.found(_argument_name);
    }
    _given = true;
    _value = clapp::value::convert_value<T>(argument);
    return {};
}

template <typename T>
typename clapp::argument::basic_variadic_argument_t<T>::callbacks_t
clapp::argument::basic_variadic_argument_t<T>::create_callbacks(
    basic_variadic_argument_t<T>* inst) {
    return callbacks_t{[inst](const std::string_view argument) {
                           return inst->found_entry(argument);
                       },
                       [inst]() { return inst->given(); },
                       [inst]() { return static_cast<bool>(*inst); },
                       [inst]() { return inst->value(); }};
}

template <typename T>
template <typename... Params>
clapp::argument::basic_variadic_argument_t<T>::basic_variadic_argument_t(
    basic_parser_t& parser, const std::string& argument_name,
    const std::string& description, Params&&... parameters)
    : _argument_name{argument_name} {
    arg_conf_container_t<T, arg_conf_t> conf{gen_arg_conf<T, arg_conf_t>(
        create_callbacks(this), argument_name, description,
        std::forward<Params>(parameters)...)};
    if (conf.default_value) {
        std::stringstream string_stream;
        string_stream << "No default value for variadic argument '"
                      << conf.arg_conf.argument_name << "' allowed.";
        throw clapp::exception::argument_exception_t{string_stream.str()};
    }
    _found = std::move(conf.found);
    parser.reg(std::move(conf.arg_conf));
}

template <typename T>
inline clapp::argument::basic_variadic_argument_t<T>::operator bool()
    const noexcept {
    return !_value.empty();
}

template <typename T>
inline bool clapp::argument::basic_variadic_argument_t<T>::has_value()
    const noexcept {
    return !_value.empty();
}

template <typename T>
std::vector<T> clapp::argument::basic_variadic_argument_t<T>::value() const {
    return _value;
}

template <typename T>
constexpr bool clapp::argument::basic_variadic_argument_t<T>::given()
    const noexcept {
    return _given;
}

template <typename T>
clapp::value::found_func_t::ret_t
clapp::argument::basic_variadic_argument_t<T>::found_entry(
    const std::string_view argument) {
    for (auto& found_func : _found) {
        return found_func.found(_argument_name);
    }
    _given = true;
    _value.push_back(clapp::value::convert_value<T>(argument));
    return {};
}

template <typename T>
std::string clapp::argument::basic_variadic_argument_t<
    T>::variadic_argument_restrictions() {
    return std::string{"variadic argument"};
}

#endif
