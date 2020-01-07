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

#include <clapp/parser.h>
#include <iostream>
#include <sstream>
#include <utility>

clapp::parser::arg_t::arg_t(const char* const* argv, int argc)
    : base{gsl::make_span(argv, argc)} {
    Expects(argv[argc] == nullptr);
}

clapp::parser::basic_parser_t::basic_parser_t() = default;

clapp::parser::basic_parser_t::sub_parsers_map_t&
clapp::parser::basic_parser_t::get_sub_parsers() {
    return sub_parsers;
}

clapp::parser::basic_parser_t::long_options_map_t&
clapp::parser::basic_parser_t::get_long_options() {
    return long_options;
}

clapp::parser::basic_parser_t::short_options_map_t&
clapp::parser::basic_parser_t::get_short_options() {
    return short_options;
}

clapp::parser::basic_parser_t::arguments_vector_t&
clapp::parser::basic_parser_t::get_arguments() {
    return arguments;
}

std::size_t clapp::parser::basic_parser_t::get_num_processed_arguments() const {
    return num_processed_arguments;
}

clapp::parser::basic_parser_t::validate_func_vec_t&
clapp::parser::basic_parser_t::get_validate_functions() {
    return validate_functions;
}

clapp::parser::basic_parser_t::option_descriptions_vec_t&
clapp::parser::basic_parser_t::get_mandatory_option_descriptions() {
    return mandatory_option_descriptions;
}

clapp::parser::basic_parser_t::option_descriptions_vec_t&
clapp::parser::basic_parser_t::get_optional_option_descriptions() {
    return optional_option_descriptions;
}

clapp::parser::basic_parser_t::sub_parser_descriptions_vec_t&
clapp::parser::basic_parser_t::get_sub_parser_descriptions() {
    return sub_parser_descriptions;
}

clapp::parser::basic_parser_t::argument_descriptions_vec_t&
clapp::parser::basic_parser_t::get_mandatory_argument_descriptions() {
    return mandatory_argument_descriptions;
}

clapp::parser::basic_parser_t::argument_descriptions_vec_t&
clapp::parser::basic_parser_t::get_optional_argument_descriptions() {
    return optional_argument_descriptions;
}

void clapp::parser::basic_parser_t::reg(reg_sub_parser_conf_t&& config) {
    if (max_option_string_size < config.sub_parser_name.size()) {
        max_option_string_size = config.sub_parser_name.size();
    }

    if (get_optional_argument_descriptions().size() > 0) {
        std::stringstream ss;
        ss << "Can't register sub-parser '" << config.sub_parser_name
           << "' as optional arguments are already registered.";
        throw std::runtime_error(ss.str());
    }

    const std::size_t num_arguments{get_arguments().size()};
    if (num_arguments > 0 && get_arguments()[num_arguments - 1].argument_type ==
                                 argument_type_t::variadic) {
        std::stringstream ss;
        ss << "Can't register sub-parser '" << config.sub_parser_name
           << "' as a variadic argument is already registered.";
        throw std::runtime_error(ss.str());
    }

    auto found_sub_parser{get_sub_parsers().find(config.sub_parser_name)};
    if (found_sub_parser != get_sub_parsers().end()) {
        std::stringstream ss;
        ss << "Can't register sub-parser '" << config.sub_parser_name
           << "' as a sub-parser with this name is already registered.";
        throw std::runtime_error(ss.str());
    }

    get_sub_parser_descriptions().push_back(
        {config.sub_parser_name, std::move(config.description)});

    get_sub_parsers().emplace(std::move(config.sub_parser_name),
                              std::move(config.parse));
}

std::string clapp::parser::basic_parser_t::gen_help_msg() const {
    std::string arg_line;
    std::string sub_parser_desc;
    std::string mandatory_option_desc;
    std::string optional_option_desc;
    std::string mandatory_argument_desc;
    std::string optional_argument_desc;
    if (mandatory_option_descriptions.size() > 0) {
        mandatory_option_desc = "\nMandatory Options:\n";
        for (const option_description_container_t& desc_cont :
             mandatory_option_descriptions) {
            mandatory_option_desc +=
                "  " + desc_cont.option_string +
                std::string(
                    max_option_string_size + 1 - desc_cont.option_string.size(),
                    ' ') +
                desc_cont.description + "\n";
            arg_line += desc_cont.option_string;
            if (desc_cont.option_type ==
                clapp::basic_parser_t::option_type_t::vector) {
                arg_line += "...";
            }
            arg_line += " ";
        }
    }

    if (optional_option_descriptions.size() > 0) {
        optional_option_desc = "\nOptional Options:\n";
        for (const option_description_container_t& desc_cont :
             optional_option_descriptions) {
            optional_option_desc +=
                "  " + desc_cont.option_string +
                std::string(
                    max_option_string_size + 1 - desc_cont.option_string.size(),
                    ' ') +
                desc_cont.description + "\n";
            arg_line += "[" + desc_cont.option_string;
            if (desc_cont.option_type ==
                clapp::basic_parser_t::option_type_t::vector) {
                arg_line += "...";
            }
            arg_line += "] ";
        }
    }

    if (mandatory_argument_descriptions.size() > 0) {
        mandatory_argument_desc = "\nMandatory Arguments:\n";

        for (const argument_description_container_t& desc_cont :
             mandatory_argument_descriptions) {
            mandatory_argument_desc +=
                "  " + desc_cont.argument_string +
                std::string(max_option_string_size + 1 -
                                desc_cont.argument_string.size(),
                            ' ') +
                desc_cont.description + "\n";
            arg_line += "<" + desc_cont.argument_string + ">";
            if (desc_cont.argument_type ==
                clapp::basic_parser_t::argument_type_t::variadic) {
                arg_line += "...";
            }
            arg_line += " ";
        }
    }

    if (optional_argument_descriptions.size() > 0) {
        optional_argument_desc = "\nOptional Arguments:\n";

        for (const argument_description_container_t& desc_cont :
             optional_argument_descriptions) {
            optional_argument_desc +=
                "  " + desc_cont.argument_string +
                std::string(max_option_string_size + 1 -
                                desc_cont.argument_string.size(),
                            ' ') +
                desc_cont.description + "\n";

            arg_line += "[<" + desc_cont.argument_string + ">";
            if (desc_cont.argument_type ==
                clapp::basic_parser_t::argument_type_t::variadic) {
                arg_line += "...";
            }
            arg_line += "] ";
        }
    }

    if (sub_parser_descriptions.size() > 0) {
        sub_parser_desc = "\nAvailable sub-parsers:\n";

        for (const sub_parser_description_container_t& desc_cont :
             sub_parser_descriptions) {
            sub_parser_desc +=
                "  " + desc_cont.sub_parser_string +
                std::string(max_option_string_size + 1 -
                                desc_cont.sub_parser_string.size(),
                            ' ') +
                desc_cont.description + "\n";
        }
        arg_line += "<sub-parser> [sub-parser args/opts...] ";
    }

    return arg_line + "\n" + mandatory_argument_desc + optional_argument_desc +
           sub_parser_desc + mandatory_option_desc + optional_option_desc;
}

clapp::parser::basic_parser_t::arg_iterator
clapp::basic_parser_t::process_parse_result(
    const arg_iterator it,
    const clapp::basic_parser_t::parse_result_t& parse_result) const {
    if (it == parse_result.it) {
        if (parse_result.short_option) {
            std::stringstream ss;
            ss << "Invalid (sub-)parser line option '-"
               << parse_result.short_option.value() << "'";
            throw std::runtime_error(ss.str());
        } else if (parse_result.long_option) {
            std::stringstream ss;
            ss << "Invalid (sub-)parser line option '--"
               << parse_result.long_option.value() << "'";
            throw std::runtime_error(ss.str());
        } else {
            return it + 1;  // ignore unknonwn arg
        }
    } else {
        return parse_result.it;
    }
}

void clapp::parser::basic_parser_t::parse(arg_iterator begin,
                                          arg_iterator end) {
    for (arg_iterator it = begin; it != end;) {
        clapp::basic_parser_t::parse_result_t parse_result{
            parse(std::string_view{*it}, it, end)};
        it = process_parse_result(it, parse_result);
    }
}

clapp::parser::basic_parser_t::parse_result_t clapp::basic_parser_t::parse(
    std::string_view option, arg_iterator it, arg_iterator end) {
    if (option.size() >= 3 && option[0] == '-' && option[1] == '-') {
        option.remove_prefix(2);
        return parse_long(option, it, end);
    } else if (option.size() >= 2 && option[0] == '-') {
        option.remove_prefix(1);
        return parse_short(option, it, end);
    } else {
        return parse_arg(option, it, end);
    }
}

void clapp::parser::basic_parser_t::validate() const {
    for (auto& validate_func : validate_functions) {
        validate_func();
    }
}

clapp::parser::basic_parser_t::parse_result_t
clapp::parser::basic_parser_t::parse_arg(const std::string_view option,
                                         arg_iterator it, arg_iterator end) {
    const std::size_t num_arguments{get_arguments().size()};
    if (num_arguments > 0) {
        if (num_processed_arguments < num_arguments) {
            get_arguments()[num_processed_arguments].func(option);
            num_processed_arguments++;
            return parse_result_t{it + 1, std::nullopt, std::nullopt};
        }
        auto& arg{get_arguments()[get_arguments().size() - 1]};
        if (arg.argument_type == argument_type_t::variadic) {
            arg.func(option);
            num_processed_arguments++;
            return parse_result_t{it + 1, std::nullopt, std::nullopt};
        }
    }

    auto found_sub_parser{get_sub_parsers().find(option)};
    if (found_sub_parser == get_sub_parsers().end()) {
        std::stringstream ss;
        ss << "Unknown argument/sub-parser '" << option << "'.";
        throw std::runtime_error(ss.str());
    }
    found_sub_parser->second(it + 1, end);
    return parse_result_t{end, std::nullopt, std::nullopt};
}

clapp::parser::basic_parser_t::parse_result_t
clapp::parser::basic_parser_t::parse_long(const std::string_view option,
                                          arg_iterator it, arg_iterator end) {
    const std::size_t equal_index{option.find_first_of("=")};
    const std::string_view opt{option.data(),
                               std::min(equal_index, option.size())};
    auto found{get_long_options().find(opt)};
    if (found == get_long_options().end()) {
        return parse_result_t{it, std::nullopt, std::string{opt}};
    }
    if (equal_index != std::string_view::npos) {
        if (std::holds_alternative<long_opt_param_func_t>(found->second)) {
            std::get<long_opt_param_func_t>(found->second)(
                opt, std::string_view{&(option[equal_index + 1])});
            return parse_result_t{it + 1, std::nullopt, std::nullopt};
        } else {
            std::stringstream ss;
            ss << "No param allowed for option '--" << opt << "'";
            throw std::runtime_error(ss.str());
        }
    } else {
        if (std::holds_alternative<long_opt_param_func_t>(found->second)) {
            if (it + 1 != end) {
                it++;
                std::get<long_opt_param_func_t>(found->second)(
                    opt, std::string_view{*(it)});
                return parse_result_t{it + 1, std::nullopt, std::nullopt};
            } else {
                std::stringstream ss;
                ss << "No param given for option '--" << opt << "'";
                throw std::runtime_error(ss.str());
            }
        } else {
            Expects(std::holds_alternative<long_opt_func_t>(found->second));
            std::get<long_opt_func_t>(found->second)(opt);
            return parse_result_t{it + 1, std::nullopt, std::nullopt};
        }
    }
}

clapp::parser::basic_parser_t::parse_result_t
clapp::parser::basic_parser_t::parse_short(const std::string_view option,
                                           arg_iterator it, arg_iterator end) {
    const std::size_t equal_index{option.find_first_of("=")};
    for (std::size_t i{0}; i < option.size(); i++) {
        auto found{get_short_options().find(option[i])};
        if (found == get_short_options().end()) {
            return parse_result_t{it, option[i], std::nullopt};
        }
        if (equal_index == i + 1) {
            if (std::holds_alternative<short_opt_param_func_t>(found->second)) {
                std::get<short_opt_param_func_t>(found->second)(
                    option[i], std::string_view{&(option[equal_index + 1])});
                return parse_result_t{it + 1, std::nullopt, std::nullopt};
            } else {
                std::stringstream ss;
                ss << "No param allowed for option '-" << option[i] << "'";
                throw std::runtime_error(ss.str());
            }
        } else if (i == option.size() - 1) {
            if (std::holds_alternative<short_opt_param_func_t>(found->second)) {
                if (it + 1 != end) {
                    it++;
                    std::get<short_opt_param_func_t>(found->second)(
                        option[i], std::string_view{*(it)});
                    return parse_result_t{it + 1, std::nullopt, std::nullopt};
                } else {
                    std::stringstream ss;
                    ss << "No param given for option '-" << option[i] << "'";
                    throw std::runtime_error(ss.str());
                }
            } else {
                std::get<short_opt_func_t>(found->second)(option[i]);
                return parse_result_t{it + 1, std::nullopt, std::nullopt};
            }
        } else {
            if (std::holds_alternative<short_opt_param_func_t>(found->second)) {
                std::stringstream ss;
                ss << "No param given for option '-" << option[i] << "'";
                throw std::runtime_error(ss.str());
            } else {
                std::get<short_opt_func_t>(found->second)(option[i]);
            }
        }
    }
    return parse_result_t{it + 1, std::nullopt, std::nullopt};
}

clapp::parser::basic_parser_t::~basic_parser_t() = default;
