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

#ifndef CLAPP_PARSER_TYPES_HPP
#define CLAPP_PARSER_TYPES_HPP

#include <clapp/parser_types.h>

#include <numeric>

inline clapp::parser::types::arg_t::arg_t(const char* const* argv, int argc)
    : base{gsl::span{argv, static_cast<std::size_t>(argc)}} {
    Expects(argc >= 0);
}

constexpr std::optional<std::string_view> clapp::parser::types::to_string_view(
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

template <typename short_option_func_t, typename long_option_func_t,
          clapp::parser::types::option_type_t option_type>
std::string clapp::parser::types::basic_reg_option_conf_t<
    short_option_func_t, long_option_func_t,
    option_type>::create_option_string(const std::string& long_option) {
    const std::string prefix{"--"};
    return prefix + long_option;
}

template <typename short_option_func_t, typename long_option_func_t,
          clapp::parser::types::option_type_t option_type>
std::string clapp::parser::types::basic_reg_option_conf_t<
    short_option_func_t, long_option_func_t,
    option_type>::create_option_string(const char short_option) {
    const std::string prefix{"-"};
    return prefix + short_option;
}

template <typename short_option_func_t, typename long_option_func_t,
          clapp::parser::types::option_type_t option_type>
std::string clapp::parser::types::basic_reg_option_conf_t<
    short_option_func_t, long_option_func_t,
    option_type>::create_option_string(const std::vector<char>& short_option,
                                       const std::vector<std::string>&
                                           long_option) {
    std::string option_string;
    if (!short_option.empty()) {
        option_string = std::accumulate(
            short_option.begin(), short_option.end(), std::string(),
            [](const std::string& first, const char second) -> std::string {
                const std::string opt_str{create_option_string(second)};
                const std::string sep{(first.length() > 0 ? "|" : "")};
                return first + sep + opt_str;
            });
    }
    if (!long_option.empty()) {
        option_string = std::accumulate(
            long_option.begin(), long_option.end(), option_string,
            [](const std::string& first,
               const std::string& second) -> std::string {
                const std::string opt_str{create_option_string(second)};
                const std::string sep{(first.length() > 0 ? "|" : "")};
                return first + sep + opt_str;
            });
    }
    return option_string;
}

template <typename short_option_func_t, typename long_option_func_t,
          clapp::parser::types::option_type_t option_type>
std::string clapp::parser::types::basic_reg_option_conf_t<
    short_option_func_t, long_option_func_t,
    option_type>::create_basic_option_string() const {
    std::string ret;
    if (!short_options.empty()) {
        ret = std::accumulate(
            short_options.begin(), short_options.end(), ret,
            [](const std::string& first,
               const basic_short_opt_conf_t<short_option_func_t>& second)
                -> std::string {
                const std::string opt_str{std::string{"-"} + second.option};
                const std::string sep{(first.length() > 0 ? "|" : "")};
                return first + sep + opt_str;
            });
    }
    if (!long_options.empty()) {
        ret = std::accumulate(
            long_options.begin(), long_options.end(), ret,
            [](const std::string& first,
               const basic_long_opt_conf_t<long_option_func_t>& second)
                -> std::string {
                const std::string opt_str{std::string{"--"} + second.option};
                const std::string sep{(first.length() > 0 ? "|" : "")};
                return first + sep + opt_str;
            });
    }
    if constexpr (is_param_opt<short_option_func_t, long_option_func_t>()) {
        ret += "=<param>";
    }
    return ret;
}

template <typename short_option_func_t, typename long_option_func_t,
          clapp::parser::types::option_type_t option_type>
std::string clapp::parser::types::basic_reg_option_conf_t<
    short_option_func_t, long_option_func_t,
    option_type>::create_option_string() const {
    std::string ret{create_basic_option_string()};
    if constexpr (option_type == option_type_t::vector) {
        ret += "...";
    }
    if (purpose == purpose_t::optional) {
        return "[" + ret + "]";
    }
    return ret;
}

template <typename short_option_func_t, typename long_option_func_t,
          clapp::parser::types::option_type_t option_type>
clapp::parser::types::help_entry_t
clapp::parser::types::basic_reg_option_conf_t<
    short_option_func_t, long_option_func_t, option_type>::get_option_help()
    const {
    return help_entry_t{create_basic_option_string(), description};
}

template <typename short_option_func_t, typename long_option_func_t,
          clapp::parser::types::option_type_t option_type>
typename clapp::parser::types::basic_reg_option_conf_t<
    short_option_func_t, long_option_func_t,
    option_type>::long_opt_conf_vec_cit_t
clapp::parser::types::basic_reg_option_conf_t<
    short_option_func_t, long_option_func_t,
    option_type>::find_option(const std::string_view long_option) const {
    return std::find_if(std::begin(long_options), std::end(long_options),
                        [long_option](auto&& long_opt_conf) {
                            return long_opt_conf.option == long_option;
                        });
}

template <typename short_option_func_t, typename long_option_func_t,
          clapp::parser::types::option_type_t option_type>
typename clapp::parser::types::basic_reg_option_conf_t<
    short_option_func_t, long_option_func_t,
    option_type>::short_opt_conf_vec_cit_t
clapp::parser::types::basic_reg_option_conf_t<
    short_option_func_t, long_option_func_t,
    option_type>::find_option(const char short_option) const {
    return std::find_if(std::begin(short_options), std::end(short_options),
                        [short_option](auto&& short_opt_conf) {
                            return short_opt_conf.option == short_option;
                        });
}

template <typename short_option_func_t, typename long_option_func_t,
          clapp::parser::types::option_type_t option_type>
bool clapp::parser::types::basic_reg_option_conf_t<
    short_option_func_t, long_option_func_t,
    option_type>::contains_option(const std::string_view long_option) const {
    if (long_options.empty()) {
        return false;
    }
    return find_option(long_option) != std::end(long_options);
}

template <typename short_option_func_t, typename long_option_func_t,
          clapp::parser::types::option_type_t option_type>
bool clapp::parser::types::basic_reg_option_conf_t<
    short_option_func_t, long_option_func_t,
    option_type>::contains_option(const char short_option) const {
    if (short_options.empty()) {
        return false;
    }
    return find_option(short_option) != std::end(short_options);
}

template <clapp::parser::types::argument_type_t argument_type>
std::string clapp::parser::types::basic_reg_argument_conf_t<
    argument_type>::create_basic_argument_string() const {
    return argument_name;
}

template <clapp::parser::types::argument_type_t argument_type>
std::string clapp::parser::types::basic_reg_argument_conf_t<
    argument_type>::create_argument_string() const {
    std::string ret{"<" + create_basic_argument_string() + ">"};
    if constexpr (argument_type == argument_type_t::variadic) {
        ret += "...";
    }
    if (purpose == purpose_t::optional) {
        return "[" + ret + "]";
    }
    return ret;
}

template <clapp::parser::types::argument_type_t argument_type>
clapp::parser::types::help_entry_t clapp::parser::types::
    basic_reg_argument_conf_t<argument_type>::get_argument_help() const {
    return help_entry_t{create_basic_argument_string(), description};
}

template <typename short_option_func_t, typename long_option_func_t>
constexpr bool clapp::parser::types::is_param_opt() {
    return std::is_same<short_option_func_t, short_opt_param_func_t>::value &&
           std::is_same<long_option_func_t, long_opt_param_func_t>::value;
}

#endif
