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

#ifndef CLAPP_PARSER_HPP
#define CLAPP_PARSER_HPP

#include <clapp/parser.h>

#include <numeric>
#include <sstream>

inline clapp::parser::arg_t::arg_t(const char* const* argv, int argc)
    : base{gsl::span{argv, static_cast<std::size_t>(argc)}} {
    Expects(argc >= 0);
}

template <typename short_option_func_t, typename long_option_func_t>
constexpr bool clapp::parser::basic_parser_t::is_param_opt() {
    return std::is_same<short_option_func_t, short_opt_param_func_t>::value &&
           std::is_same<long_option_func_t, long_opt_param_func_t>::value;
}

template <typename short_option_func_t, typename long_option_func_t,
          clapp::parser::basic_parser_t::option_type_t option_type>
void clapp::parser::basic_parser_t::reg(
    basic_reg_option_conf_t<short_option_func_t, long_option_func_t,
                            option_type>&& config) {
    for (auto it{std::begin(config.short_options)};
         it != std::end(config.short_options); it++) {
        if (find_option(it->option) != options.end()) {
            std::stringstream string_stream;
            string_stream << "can't register option '" << it->option
                          << "', as it is already registered.";
            throw clapp::exception::option_exception_t(string_stream.str());
        }
        auto next_it{
            std::find_if(std::next(it), std::end(config.short_options),
                         [short_option = it->option](auto&& short_opt_conf) {
                             return short_opt_conf.option == short_option;
                         })};
        if (next_it != std::end(config.short_options)) {
            std::stringstream string_stream;
            string_stream << "can't register option '" << it->option
                          << "', as it is contained twice in the same config.";
            throw clapp::exception::option_exception_t(string_stream.str());
        }
    }

    for (auto it{std::begin(config.long_options)};
         it != std::end(config.long_options); it++) {
        if (find_option(it->option) != options.end()) {
            std::stringstream string_stream;
            string_stream << "can't register option '" << it->option
                          << "', as it is already registered.";
            throw clapp::exception::option_exception_t(string_stream.str());
        }
        auto next_it{
            std::find_if(std::next(it), std::end(config.long_options),
                         [long_option = it->option](auto&& long_opt_conf) {
                             return long_opt_conf.option == long_option;
                         })};
        if (next_it != std::end(config.long_options)) {
            std::stringstream string_stream;
            string_stream << "can't register option '" << it->option
                          << "', as it is contained twice in the same config.";
            throw clapp::exception::option_exception_t(string_stream.str());
        }
    }

    if (config.validate_func) {
        get_validate_functions().push_back(config.validate_func.value());
    }

    options.push_back(std::move(config));
}

template <clapp::parser::basic_parser_t::argument_type_t argument_type>
void clapp::parser::basic_parser_t::reg(
    basic_reg_argument_conf_t<argument_type>&& config) {
    if (config.argument_name.size() == 0) {
        std::stringstream ss;
        ss << "Argument name '" << config.argument_name << "' is too short.";
        throw clapp::exception::argument_exception_t{ss.str()};
    }

    const std::size_t num_arguments{arguments.size()};
    if (num_arguments > 0 && std::holds_alternative<variadic_arg_conf_t>(
                                 arguments[num_arguments - 1])) {
        std::stringstream ss;
        ss << "Can't register argument '" << config.argument_name
           << "' when variadic arguments are already registered.";
        throw clapp::exception::argument_exception_t{ss.str()};
    }

    if (config.purpose == purpose_t::mandatory) {
        if (!get_optional_argument_descriptions().empty()) {
            std::stringstream ss;
            ss << "Can't register mandatory argument '" << config.argument_name
               << "' when optional arguments are already registered.";
            throw clapp::exception::argument_exception_t{ss.str()};
        }
        get_mandatory_argument_descriptions().push_back(
            {config.argument_name, config.description, argument_type});
    } else {
        if (!get_sub_parser_descriptions().empty()) {
            std::stringstream ss;
            ss << "Can't register optional argument '" << config.argument_name
               << "' when a sub-parser is already registered.";
            throw clapp::exception::argument_exception_t{ss.str()};
        }
        get_optional_argument_descriptions().push_back(
            {config.argument_name, config.description, argument_type});
    }

    for (const auto& arg : arguments) {
        std::visit(
            [&config](auto&& a) {
                if (a.argument_name == config.argument_name) {
                    std::stringstream ss;
                    ss << "Can't register argument '" << config.argument_name
                       << "', as another argument with this name is already "
                          "registered.";
                    throw clapp::exception::argument_exception_t{ss.str()};
                }
            },
            arg);
    }

    if (config.validate_func) {
        get_validate_functions().push_back(config.validate_func.value());
    }

    arguments.push_back(config);
}

template <typename short_option_func_t, typename long_option_func_t,
          clapp::parser::basic_parser_t::option_type_t option_type>
std::string clapp::parser::basic_parser_t::basic_reg_option_conf_t<
    short_option_func_t, long_option_func_t,
    option_type>::create_option_string(const std::string& long_option) {
    const std::string prefix{"--"};
    return prefix + long_option;
}

template <typename short_option_func_t, typename long_option_func_t,
          clapp::parser::basic_parser_t::option_type_t option_type>
std::string clapp::parser::basic_parser_t::basic_reg_option_conf_t<
    short_option_func_t, long_option_func_t,
    option_type>::create_option_string(const char short_option) {
    const std::string prefix{"-"};
    return prefix + short_option;
}

template <typename short_option_func_t, typename long_option_func_t,
          clapp::parser::basic_parser_t::option_type_t option_type>
std::string clapp::parser::basic_parser_t::basic_reg_option_conf_t<
    short_option_func_t, long_option_func_t,
    option_type>::create_option_string(const std::vector<char> short_option,
                                       const std::vector<std::string>&
                                           long_option) {
    std::string option_string;
    if (!short_option.empty()) {
        option_string = std::accumulate(
            short_option.begin(), short_option.end(), std::string(),
            [](const std::string& a, const char b) -> std::string {
                const std::string opt_str{create_option_string(b)};
                const std::string sep{(a.length() > 0 ? "|" : "")};
                return a + sep + opt_str;
            });
    }
    if (!long_option.empty()) {
        option_string = std::accumulate(
            long_option.begin(), long_option.end(), option_string,
            [](const std::string& a, const std::string& b) -> std::string {
                const std::string opt_str{create_option_string(b)};
                const std::string sep{(a.length() > 0 ? "|" : "")};
                return a + sep + opt_str;
            });
    }
    return option_string;
}

template <typename short_option_func_t, typename long_option_func_t,
          clapp::parser::basic_parser_t::option_type_t option_type>
std::string clapp::parser::basic_parser_t::basic_reg_option_conf_t<
    short_option_func_t, long_option_func_t,
    option_type>::create_basic_option_string() const {
    std::string ret;
    if (!short_options.empty()) {
        ret = std::accumulate(
            short_options.begin(), short_options.end(), ret,
            [](const std::string& a,
               const basic_short_opt_conf_t<short_option_func_t>& b)
                -> std::string {
                const std::string opt_str{std::string{"-"} + b.option};
                const std::string sep{(a.length() > 0 ? "|" : "")};
                return a + sep + opt_str;
            });
    }
    if (!long_options.empty()) {
        ret = std::accumulate(
            long_options.begin(), long_options.end(), ret,
            [](const std::string& a,
               const basic_long_opt_conf_t<long_option_func_t>& b)
                -> std::string {
                const std::string opt_str{std::string{"--"} + b.option};
                const std::string sep{(a.length() > 0 ? "|" : "")};
                return a + sep + opt_str;
            });
    }
    if constexpr (is_param_opt<short_option_func_t, long_option_func_t>()) {
        ret += "=<param>";
    }
    return ret;
}

template <typename short_option_func_t, typename long_option_func_t,
          clapp::parser::basic_parser_t::option_type_t option_type>
std::string clapp::parser::basic_parser_t::basic_reg_option_conf_t<
    short_option_func_t, long_option_func_t,
    option_type>::create_option_string() const {
    std::string ret{create_basic_option_string()};
    if constexpr (option_type == option_type_t::vector) {
        ret += "...";
    }
    if (purpose == purpose_t::optional) {
        return "[" + ret + "]";
    } else {
        return ret;
    }
}

template <typename short_option_func_t, typename long_option_func_t,
          clapp::parser::basic_parser_t::option_type_t option_type>
clapp::parser::basic_parser_t::help_entry_t
clapp::parser::basic_parser_t::basic_reg_option_conf_t<
    short_option_func_t, long_option_func_t, option_type>::get_option_help()
    const {
    const help_entry_t ret{create_basic_option_string(), description};
    return ret;
}

template <typename short_option_func_t, typename long_option_func_t,
          clapp::parser::basic_parser_t::option_type_t option_type>
typename clapp::parser::basic_parser_t::basic_reg_option_conf_t<
    short_option_func_t, long_option_func_t,
    option_type>::long_opt_conf_vec_cit_t
clapp::parser::basic_parser_t::basic_reg_option_conf_t<
    short_option_func_t, long_option_func_t,
    option_type>::find_option(const std::string_view long_option) const {
    return std::find_if(std::begin(long_options), std::end(long_options),
                        [long_option](auto&& long_opt_conf) {
                            return long_opt_conf.option == long_option;
                        });
}

template <typename short_option_func_t, typename long_option_func_t,
          clapp::parser::basic_parser_t::option_type_t option_type>
typename clapp::parser::basic_parser_t::basic_reg_option_conf_t<
    short_option_func_t, long_option_func_t,
    option_type>::short_opt_conf_vec_cit_t
clapp::parser::basic_parser_t::basic_reg_option_conf_t<
    short_option_func_t, long_option_func_t,
    option_type>::find_option(const char short_option) const {
    return std::find_if(std::begin(short_options), std::end(short_options),
                        [short_option](auto&& short_opt_conf) {
                            return short_opt_conf.option == short_option;
                        });
}

template <typename short_option_func_t, typename long_option_func_t,
          clapp::parser::basic_parser_t::option_type_t option_type>
bool clapp::parser::basic_parser_t::basic_reg_option_conf_t<
    short_option_func_t, long_option_func_t,
    option_type>::contains_option(const std::string_view long_option) const {
    if (long_options.size() == 0) {
        return false;
    }
    return find_option(long_option) != std::end(long_options);
}

template <typename short_option_func_t, typename long_option_func_t,
          clapp::parser::basic_parser_t::option_type_t option_type>
bool clapp::parser::basic_parser_t::basic_reg_option_conf_t<
    short_option_func_t, long_option_func_t,
    option_type>::contains_option(const char short_option) const {
    if (short_options.size() == 0) {
        return false;
    }
    return find_option(short_option) != std::end(short_options);
}

template <clapp::parser::basic_parser_t::argument_type_t argument_type>
std::string clapp::parser::basic_parser_t::basic_reg_argument_conf_t<
    argument_type>::create_basic_argument_string() const {
    return argument_name;
}

template <clapp::parser::basic_parser_t::argument_type_t argument_type>
std::string clapp::parser::basic_parser_t::basic_reg_argument_conf_t<
    argument_type>::create_argument_string() const {
    std::string ret{"<" + create_basic_argument_string() + ">"};
    if constexpr (argument_type == argument_type_t::variadic) {
        ret += "...";
    }
    if (purpose == purpose_t::optional) {
        return "[" + ret + "]";
    } else {
        return ret;
    }
}

template <clapp::parser::basic_parser_t::argument_type_t argument_type>
clapp::parser::basic_parser_t::help_entry_t clapp::parser::basic_parser_t::
    basic_reg_argument_conf_t<argument_type>::get_argument_help() const {
    const help_entry_t ret{create_basic_argument_string(), description};
    return ret;
}

inline bool clapp::parser::basic_parser_t::is_active() const noexcept {
    return true;
}

constexpr std::optional<std::string_view>
clapp::parser::basic_parser_t::to_string_view(
    const purpose_t purpose) noexcept {
    using namespace std::literals;
    switch (purpose) {
        case purpose_t::optional:
            return "optional"sv;
        case purpose_t::mandatory:
            return "mandatory"sv;
    }
    return std::nullopt;
}

constexpr std::string_view
clapp::parser::basic_parser_t::gen_usage_prefix() noexcept {
    return "Usage:\n";
}

#endif
