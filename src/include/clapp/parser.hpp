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
    : base{gsl::make_span(argv, argc)} {}

template <typename short_option_func_t, typename long_option_func_t>
constexpr bool clapp::parser::basic_parser_t::is_param_opt() {
    return std::is_same<short_option_func_t, short_opt_param_func_t>::value &&
           std::is_same<long_option_func_t, long_opt_param_func_t>::value;
}

template <typename short_option_func_t, typename long_option_func_t,
          clapp::parser::basic_parser_t::option_type_t option_type>
void clapp::parser::basic_parser_t::reg(
    reg_option_conf_t<short_option_func_t, long_option_func_t, option_type>&&
        config) {
    Expects(config.option_string.size() > 1);

    if (!config.short_option.empty()) {
        for (auto& so : config.short_option) {
            std::pair<short_options_map_t::iterator, bool> ret{
                get_short_options().emplace(std::move(so.option),
                                            std::move(so.func))};
            if (!ret.second) {
                std::stringstream ss;
                ss << "Can't register option '" << so.option
                   << "' as it is already registered.";
                throw clapp::exception::option_exception_t(ss.str());
            }
        }
    }
    if (!config.long_option.empty()) {
        for (auto& lo : config.long_option) {
            std::pair<long_options_map_t::iterator, bool> ret{
                get_long_options().emplace(std::move(lo.option),
                                           std::move(lo.func))};
            if (!ret.second) {
                std::stringstream ss;
                ss << "Can't register option '" << lo.option
                   << "' as it is already registered.";
                throw clapp::exception::option_exception_t(ss.str());
            }
        }
    }

    if (config.validate_func) {
        get_validate_functions().push_back(
            std::move(config.validate_func.value()));
    }

    if (is_param_opt<short_option_func_t, long_option_func_t>()) {
        config.option_string += "=<arg>";
    }

    if (max_option_string_size < config.option_string.size()) {
        max_option_string_size = config.option_string.size();
    }

    option_description_container_t desc{std::move(config.option_string),
                                        std::move(config.description),
                                        option_type};
    if (config.purpose == purpose_t::mandatory) {
        get_mandatory_option_descriptions().push_back(std::move(desc));
    } else {
        get_optional_option_descriptions().push_back(std::move(desc));
    }
}

template <clapp::parser::basic_parser_t::argument_type_t argument_type>
void clapp::parser::basic_parser_t::reg(
    reg_argument_conf_t<argument_type>&& config) {
    if (config.argument_name.size() == 0) {
        std::stringstream ss;
        ss << "Argument name '" << config.argument_name << "' is too short.";
        throw clapp::exception::argument_exception_t(ss.str());
    }

    const std::size_t num_arguments{get_arguments().size()};
    if (num_arguments > 0 && get_arguments()[num_arguments - 1].argument_type ==
                                 argument_type_t::variadic) {
        std::stringstream ss;
        ss << "Can't register argument '" << config.argument_name
           << "' when variadic arguments are already registered.";
        throw clapp::exception::argument_exception_t(ss.str());
    }

    if (max_option_string_size < config.argument_name.size()) {
        max_option_string_size = config.argument_name.size();
    }

    if (config.purpose == purpose_t::mandatory) {
        if (!get_optional_argument_descriptions().empty()) {
            std::stringstream ss;
            ss << "Can't register mandatory argument '" << config.argument_name
               << "' when optional arguments are already registered.";
            throw clapp::exception::argument_exception_t(ss.str());
        }
        get_mandatory_argument_descriptions().push_back(
            {config.argument_name, std::move(config.description),
             argument_type});
    } else {
        if (!get_sub_parser_descriptions().empty()) {
            std::stringstream ss;
            ss << "Can't register optional argument '" << config.argument_name
               << "' when a sub-parser is already registered.";
            throw clapp::exception::argument_exception_t(ss.str());
        }
        get_optional_argument_descriptions().push_back(
            {config.argument_name, std::move(config.description),
             argument_type});
    }

    get_arguments().push_back({std::move(config.argument_name),
                               std::move(config.argument), argument_type});

    if (config.validate_func) {
        get_validate_functions().push_back(
            std::move(config.validate_func.value()));
    }
}

template <typename short_option_func_t, typename long_option_func_t,
          clapp::parser::basic_parser_t::option_type_t option_type>
std::string clapp::parser::basic_parser_t::reg_option_conf_t<
    short_option_func_t, long_option_func_t,
    option_type>::create_option_string(const std::string& long_option) {
    return std::string{"--"} + long_option;
}

template <typename short_option_func_t, typename long_option_func_t,
          clapp::parser::basic_parser_t::option_type_t option_type>
std::string clapp::parser::basic_parser_t::reg_option_conf_t<
    short_option_func_t, long_option_func_t,
    option_type>::create_option_string(const char short_option) {
    return std::string{"-"} + short_option;
}

template <typename short_option_func_t, typename long_option_func_t,
          clapp::parser::basic_parser_t::option_type_t option_type>
std::string clapp::parser::basic_parser_t::reg_option_conf_t<
    short_option_func_t, long_option_func_t,
    option_type>::create_option_string(const std::vector<char> short_option,
                                       const std::vector<std::string>&
                                           long_option) {
    std::string option_string;
    if (!short_option.empty()) {
        option_string = std::accumulate(
            short_option.begin(), short_option.end(), std::string(),
            [](const std::string& a, const char b) -> std::string {
                std::string opt_str{create_option_string(b)};
                return a + (a.length() > 0 ? "|" : "") + opt_str;
            });
    }
    if (!long_option.empty()) {
        option_string = std::accumulate(
            long_option.begin(), long_option.end(), option_string,
            [](const std::string& a, const std::string& b) -> std::string {
                std::string opt_str{create_option_string(b)};
                return a + (a.length() > 0 ? "|" : "") + opt_str;
            });
    }
    return option_string;
}

#endif
