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
#include <clapp/sub_parser.h>
#include <iostream>
#include <sstream>
#include <utility>

clapp::parser::basic_parser_t::basic_parser_t() = default;

bool clapp::parser::basic_parser_t::help_entry_t::operator==(
    const help_entry_t& inst) const {
    return name == inst.name && description == inst.description;
}

bool clapp::parser::basic_parser_t::help_entry_t::operator!=(
    const help_entry_t& inst) const {
    return !(*this == inst);
}

clapp::parser::basic_parser_t::sub_parsers_map_t&
clapp::parser::basic_parser_t::get_sub_parsers() {
    return sub_parsers;
}

typename clapp::parser::basic_parser_t::variant_opt_conf_vec_t::const_iterator
clapp::parser::basic_parser_t::find_option(const std::string_view opt) const {
    for (variant_opt_conf_vec_t::const_iterator it{options.begin()};
         it != options.end(); it++) {
        if (std::visit([opt](auto&& o) { return bool{o.contains_option(opt)}; },
                       *it)) {
            return it;
        }
    }
    return options.end();
}

typename clapp::parser::basic_parser_t::variant_opt_conf_vec_t::const_iterator
clapp::parser::basic_parser_t::find_option(const char opt) const {
    for (variant_opt_conf_vec_t::const_iterator it{options.begin()};
         it != options.end(); it++) {
        if (std::visit([opt](auto&& o) { return bool{o.contains_option(opt)}; },
                       *it)) {
            return it;
        }
    }
    return options.end();
}

typename clapp::parser::basic_parser_t::help_entry_vec_t
clapp::parser::basic_parser_t::get_option_help() const {
    typename clapp::parser::basic_parser_t::help_entry_vec_t ret;
    for (const auto& option : options) {
        ret.push_back(std::visit(
            [](const auto& o) { return help_entry_t{o.get_option_help()}; },
            option));
    }
    return ret;
}

typename clapp::parser::basic_parser_t::help_entry_vec_t
clapp::parser::basic_parser_t::get_argument_help() const {
    typename clapp::parser::basic_parser_t::help_entry_vec_t ret;
    for (const auto& argument : arguments) {
        ret.push_back(std::visit(
            [](const auto& a) { return help_entry_t{a.get_argument_help()}; },
            argument));
    }
    return ret;
}

clapp::parser::basic_parser_t::variant_arg_conf_vec_t
clapp::parser::basic_parser_t::get_arguments() const {
    return arguments;
}

std::size_t clapp::parser::basic_parser_t::get_num_processed_arguments() const {
    return num_processed_arguments;
}

clapp::parser::basic_parser_t::validate_func_vec_t&
clapp::parser::basic_parser_t::get_validate_functions() {
    return validate_functions;
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

clapp::parser::basic_parser_t::variant_opt_conf_vec_t
clapp::parser::basic_parser_t::get_options() const {
    return options;
}

void clapp::parser::basic_parser_t::reg(reg_sub_parser_conf_t&& config) {
    if (!get_optional_argument_descriptions().empty()) {
        std::stringstream ss;
        ss << "Can't register sub-parser '" << config.sub_parser_name
           << "' as optional arguments are already registered.";
        throw clapp::exception::sub_parser_exception_t(ss.str());
    }

    const std::size_t num_arguments{arguments.size()};
    if (num_arguments > 0 && std::holds_alternative<variadic_arg_conf_t>(
                                 arguments.at(num_arguments - 1))) {
        std::stringstream ss;
        ss << "Can't register sub-parser '" << config.sub_parser_name
           << "' as a variadic argument is already registered.";
        throw clapp::exception::sub_parser_exception_t(ss.str());
    }

    auto found_sub_parser{get_sub_parsers().find(config.sub_parser_name)};
    if (found_sub_parser != get_sub_parsers().end()) {
        std::stringstream ss;
        ss << "Can't register sub-parser '" << config.sub_parser_name
           << "' as a sub-parser with this name is already registered.";
        throw clapp::exception::sub_parser_exception_t(ss.str());
    }

    get_sub_parser_descriptions().push_back(
        {config.sub_parser_name, std::move(config.description), config.parser});

    get_sub_parsers().emplace(std::move(config.sub_parser_name), config.parser);
}

std::string clapp::parser::basic_parser_t::gen_usage_prefix() {
    return "Usage:\n";
}

clapp::parser::basic_parser_t::help_contents_t
clapp::parser::basic_parser_t::gen_detailed_help_contents() const {
    help_contents_t ret;

    ret.options = get_option_help();
    if (!ret.options.empty()) {
        ret.max_name_size =
            std::max(ret.max_name_size,
                     std::max_element(
                         std::begin(ret.options), std::end(ret.options),
                         [](const help_entry_t& lhs, const help_entry_t& rhs) {
                             return lhs.name.size() < rhs.name.size();
                         })
                         ->name.size());
    }

    ret.arguments = get_argument_help();
    if (!ret.arguments.empty()) {
        ret.max_name_size =
            std::max(ret.max_name_size,
                     std::max_element(
                         std::begin(ret.arguments), std::end(ret.arguments),
                         [](const help_entry_t& lhs, const help_entry_t& rhs) {
                             return lhs.name.size() < rhs.name.size();
                         })
                         ->name.size());
    }

    for (const sub_parser_description_container_t& desc_cont :
         sub_parser_descriptions) {
        ret.sub_parser.emplace(
            desc_cont.sub_parser_string,
            sub_parser_line_t{desc_cont.sub_parser_string,
                              desc_cont.description, desc_cont.parser});
    }

    return ret;
}

std::string clapp::parser::basic_parser_t::gen_short_line() const {
    std::string short_line;

    for (const auto& option : options) {
        short_line +=
            " " +
            std::visit([](const auto& o) { return o.create_option_string(); },
                       option);
    }

    for (const auto& argument : arguments) {
        short_line +=
            " " +
            std::visit([](const auto& a) { return a.create_argument_string(); },
                       argument);
    }

    return short_line;
}

std::string clapp::parser::basic_parser_t::gen_opt_arg_lines(
    const clapp::parser::basic_parser_t::help_contents_t& help_contents,
    const std::size_t num_spaces) const {
    std::string ret;

    std::vector<help_entry_t> argument_help_entries{get_argument_help()};
    if (!argument_help_entries.empty()) {
        ret += "\n" + std::string(num_spaces + num_sub_spaces, ' ') +
               "Arguments:\n";
        for (const help_entry_t& entry : argument_help_entries) {
            Expects((help_contents.max_name_size + 1) > entry.name.size());
            ret +=
                "  " + std::string(num_spaces + num_sub_spaces, ' ') +
                entry.name +
                std::string(help_contents.max_name_size + 1 - entry.name.size(),
                            ' ') +
                entry.description + '\n';
        }
    }

    std::vector<help_entry_t> option_help_entries{get_option_help()};
    if (!option_help_entries.empty()) {
        ret +=
            "\n" + std::string(num_spaces + num_sub_spaces, ' ') + "Options:\n";
        for (const help_entry_t& entry : option_help_entries) {
            Expects((help_contents.max_name_size + 1) > entry.name.size());
            ret +=
                "  " + std::string(num_spaces + num_sub_spaces, ' ') +
                entry.name +
                std::string(help_contents.max_name_size + 1 - entry.name.size(),
                            ' ') +
                entry.description + '\n';
        }
    }

    return ret;
}

std::string clapp::parser::basic_parser_t::gen_help_desc(
    const std::size_t num_spaces, const std::size_t rec_depth) const {
    std::string ret{};
    clapp::parser::basic_parser_t::help_contents_t help_contents{
        gen_detailed_help_contents()};

    ret += gen_opt_arg_lines(help_contents, num_spaces);

    if (!help_contents.sub_parser.empty()) {
        ret += "\n" + std::string(num_spaces + num_sub_spaces, ' ') +
               "Subparser:\n";
        for (auto const& item : help_contents.sub_parser) {
            ret += "  " + std::string(num_spaces + num_sub_spaces, ' ') +
                   item.second.name +
                   std::string(help_contents.max_name_size + 1 -
                                   item.second.name.size(),
                               ' ') +
                   item.second.description;
            if (rec_depth > 0) {
                ret += item.second.parser.gen_help_desc(
                           num_spaces + num_sub_spaces * 2, rec_depth - 1) +
                       '\n';
            }
        }
    }
    return ret;
}

std::string clapp::parser::basic_parser_t::gen_short_lines(
    const std::size_t rec_depth) const {
    std::string short_lines{};
    if (rec_depth > 0) {
        for (const sub_parser_description_container_t& desc_cont :
             sub_parser_descriptions) {
            short_lines += desc_cont.parser.gen_short_line_prefix() + "\n";
        }
    }
    short_lines += gen_short_line_prefix() + "\n";
    return short_lines;
}

std::string clapp::parser::basic_parser_t::gen_help_msg(
    const std::size_t rec_depth) const {
    const clapp::parser::basic_parser_t& parser{get_active_parser()};
    return parser.gen_short_lines(rec_depth) +
           parser.gen_help_desc(0, rec_depth);
}

clapp::parser::basic_parser_t::arg_iterator
clapp::basic_parser_t::process_parse_result(
    const arg_iterator it,
    const clapp::basic_parser_t::parse_result_t& parse_result) {
    if (it == parse_result.it) {
        if (parse_result.short_option) {
            std::stringstream ss;
            ss << "Invalid (sub-)parser option '-"
               << parse_result.short_option.value() << "'";
            throw clapp::exception::option_exception_t(ss.str());
        }
        if (parse_result.long_option) {
            std::stringstream ss;
            ss << "Invalid (sub-)parser option '--"
               << parse_result.long_option.value() << "'";
            throw clapp::exception::option_exception_t(ss.str());
        }
        return it + 1;  // ignore unknonwn arg
    }
    return parse_result.it;
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
    }
    if (option.size() >= 2 && option[0] == '-') {
        option.remove_prefix(1);
        return parse_short(option, it, end);
    }
    return parse_arg(option, it, end);
}

void clapp::parser::basic_parser_t::validate() const {
    for (const auto& validate_func : validate_functions) {
        validate_func();
    }
}

void clapp::parser::basic_parser_t::validate_recursive() const {
    validate();
    for (const auto& elem : sub_parsers) {
        if (elem.second) {
            elem.second.validate_recursive();
        }
    }
}

clapp::parser::basic_parser_t::parse_result_t
clapp::parser::basic_parser_t::parse_arg(const std::string_view argument,
                                         arg_iterator it, arg_iterator end) {
    const std::size_t num_arguments{arguments.size()};
    if (num_arguments > 0) {
        if (num_processed_arguments < num_arguments) {
            return std::visit(
                [this, argument, it](auto&& a) {
                    a.argument(argument);
                    num_processed_arguments++;
                    return parse_result_t{it + 1, std::nullopt, std::nullopt};
                },
                arguments.at(num_processed_arguments));
        }
        const auto& arg{arguments.at(arguments.size() - 1)};
        if (std::holds_alternative<variadic_arg_conf_t>(
                arguments.at(num_arguments - 1))) {
            return std::visit(
                [this, argument, it](auto&& a) {
                    a.argument(argument);
                    num_processed_arguments++;
                    return parse_result_t{it + 1, std::nullopt, std::nullopt};
                },
                arg);
        }
    }

    auto found_sub_parser{get_sub_parsers().find(argument)};
    if (found_sub_parser == get_sub_parsers().end()) {
        std::stringstream ss;
        ss << "Unknown argument/sub-parser '" << argument << "'.";
        throw clapp::exception::clapp_exception_t(ss.str());
    }
    found_sub_parser->second.sub_parse(it + 1, end);
    return parse_result_t{end, std::nullopt, std::nullopt};
}

clapp::parser::basic_parser_t::parse_result_t
clapp::parser::basic_parser_t::parse_long(const std::string_view option,
                                          arg_iterator it, arg_iterator end) {
    const std::size_t equal_index{option.find_first_of("=")};
    const std::string_view opt{option.data(),
                               std::min(equal_index, option.size())};
    variant_opt_conf_vec_t::const_iterator found{find_option(opt)};
    if (found == options.end()) {
        return parse_result_t{it, std::nullopt, std::string{opt}};
    }
    return std::visit(
        [option, equal_index, &it, opt, end](auto&& o) {
            using current_option_type_t = std::decay_t<decltype(o)>;
            typename current_option_type_t::long_opt_conf_vec_cit_t opt_it{
                o.find_option(opt)};
            Ensures(opt_it != std::end(o.long_options));
            if (equal_index != std::string_view::npos) {
                if constexpr (std::is_same<current_option_type_t,
                                           opt_no_param_conf_t>::value) {
                    std::stringstream ss;
                    ss << "No param allowed for option '--" << opt << "'";
                    throw clapp::exception::option_param_exception_t(ss.str());
                } else {
                    opt_it->func(opt,
                                 std::string_view{&(option[equal_index + 1])});
                    return parse_result_t{it + 1, std::nullopt, std::nullopt};
                }
            }
            if constexpr (!std::is_same<current_option_type_t,
                                        opt_no_param_conf_t>::value) {
                if (it + 1 == end) {
                    std::stringstream ss;
                    ss << "No param given for option '--" << opt << "'";
                    throw clapp::exception::option_param_exception_t(ss.str());
                }
                it++;
                opt_it->func(opt, std::string_view{*it});
                return parse_result_t{it + 1, std::nullopt, std::nullopt};
            } else {
                opt_it->func(opt);
                return parse_result_t{it + 1, std::nullopt, std::nullopt};
            }
        },
        *found);
}

clapp::parser::basic_parser_t::parse_result_t
clapp::parser::basic_parser_t::parse_short(const std::string_view option,
                                           arg_iterator it, arg_iterator end) {
    Expects(!option.empty());
    const std::size_t equal_index{option.find_first_of("=")};
    for (std::size_t i{0}; i < option.size(); i++) {
        variant_opt_conf_vec_t::const_iterator found{find_option(option[i])};
        if (found == options.end()) {
            return parse_result_t{it, option[i], std::nullopt};
        }
        std::optional<parse_result_t> ret{std::visit(
            [option, i, equal_index, &it, end](auto&& o) {
                using current_option_type_t = std::decay_t<decltype(o)>;
                typename current_option_type_t::short_opt_conf_vec_cit_t opt_it{
                    o.find_option(option[i])};
                Ensures(opt_it != std::end(o.short_options));
                if (equal_index == i + 1) {
                    if constexpr (std::is_same<current_option_type_t,
                                               opt_no_param_conf_t>::value) {
                        std::stringstream ss;
                        ss << "No param allowed for option '-" << option[i]
                           << "'";
                        throw clapp::exception::option_param_exception_t(
                            ss.str());
                    } else {
                        opt_it->func(
                            option[i],
                            std::string_view{&(option[equal_index + 1])});
                        return std::make_optional(
                            parse_result_t{it + 1, std::nullopt, std::nullopt});
                    }
                }
                if (i == option.size() - 1) {
                    if constexpr (!std::is_same<current_option_type_t,
                                                opt_no_param_conf_t>::value) {
                        if (it + 1 == end) {
                            std::stringstream ss;
                            ss << "No param given for option '-" << option[i]
                               << "'";
                            throw clapp::exception::option_param_exception_t(
                                ss.str());
                        }
                        it++;
                        opt_it->func(option[i], std::string_view{*it});
                        return std::make_optional(
                            parse_result_t{it + 1, std::nullopt, std::nullopt});
                    } else {
                        opt_it->func(option[i]);
                        return std::make_optional(
                            parse_result_t{it + 1, std::nullopt, std::nullopt});
                    }
                }
                if constexpr (!std::is_same<current_option_type_t,
                                            opt_no_param_conf_t>::value) {
                    std::stringstream ss;
                    ss << "No param given for option '-" << option[i] << "'";
                    throw clapp::exception::option_param_exception_t(ss.str());
                } else {
                    opt_it->func(option[i]);
                    return std::optional<parse_result_t>{};
                }
            },
            *found)};
        if (ret) {
            return ret.value();
        }
    }
    throw clapp::exception::clapp_exception_t(
        "This exection should not be thrown.");
}

clapp::value::found_func_t
clapp::parser::basic_parser_t::gen_func_print_help_and_exit(
    const int exit_code) const {
    return found_func_t{[this, exit_code]() {
        constexpr std::size_t max_rec_depth{65535};
        print_and_exit_func(gen_usage_prefix() + gen_help_msg(max_rec_depth),
                            exit_code);
    }};
}

const clapp::parser::basic_parser_t&
clapp::parser::basic_parser_t::get_active_parser() const {
    for (auto const& item : sub_parsers) {
        if (item.second.is_active()) {
            return item.second.get_active_parser();
        }
    }
    return *this;
}

void clapp::parser::basic_parser_t::default_print_and_exit(
    const std::string_view print_msg, const std::optional<int> exit_code) {
    std::cout << print_msg;
    std::cout.flush();
    if (exit_code) {
        exit(exit_code.value());
    }
}

void clapp::parser::basic_parser_t::set_print_and_exit_func(
    print_and_exit_func_t&& func) {
    print_and_exit_func = std::move(func);
}

clapp::parser::basic_parser_t::print_and_exit_func_t&
clapp::parser::basic_parser_t::get_print_and_exit_func() {
    return print_and_exit_func;
}

clapp::parser::basic_parser_t::~basic_parser_t() = default;
