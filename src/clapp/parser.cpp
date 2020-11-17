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

clapp::parser::basic_parser_t::sub_parsers_map_t&
clapp::parser::basic_parser_t::get_sub_parsers() {
    return sub_parsers;
}

clapp::parser::basic_parser_t::long_options_map_t&
clapp::parser::basic_parser_t::get_long_options() {
    return long_options;
}

typename clapp::parser::basic_parser_t::help_entry_vec_t
clapp::parser::basic_parser_t::get_option_help() const {
    typename clapp::parser::basic_parser_t::help_entry_vec_t ret;
    for (auto& option : options) {
        ret.push_back(std::visit(
            [](auto&& o) -> help_entry_t { return o.get_option_help(); },
            option));
    }
    return ret;
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
    if (get_max_option_string_size() < config.sub_parser_name.size()) {
        set_max_option_string_size(config.sub_parser_name.size());
    }

    if (!get_optional_argument_descriptions().empty()) {
        std::stringstream ss;
        ss << "Can't register sub-parser '" << config.sub_parser_name
           << "' as optional arguments are already registered.";
        throw clapp::exception::sub_parser_exception_t(ss.str());
    }

    const std::size_t num_arguments{get_arguments().size()};
    if (num_arguments > 0 && get_arguments()[num_arguments - 1].argument_type ==
                                 argument_type_t::variadic) {
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

    help_entry_vec_t option_help_entries{get_option_help()};
    for (auto& entry : option_help_entries) {
        ret.options.emplace_back(
            help_line_t{"  " + entry.option_string +
                            std::string(get_max_option_string_size() + 1 -
                                            entry.option_string.size(),
                                        ' '),
                        entry.description});
    }

    for (const argument_description_container_t& desc_cont :
         mandatory_argument_descriptions) {
        ret.mandatory_arguments.emplace_back(
            help_line_t{"  " + desc_cont.argument_string +
                            std::string(get_max_option_string_size() + 1 -
                                            desc_cont.argument_string.size(),
                                        ' '),
                        desc_cont.description});
    }

    for (const argument_description_container_t& desc_cont :
         optional_argument_descriptions) {
        ret.optional_arguments.emplace_back(
            help_line_t{"  " + desc_cont.argument_string +
                            std::string(get_max_option_string_size() + 1 -
                                            desc_cont.argument_string.size(),
                                        ' '),
                        desc_cont.description});
    }

    for (const sub_parser_description_container_t& desc_cont :
         sub_parser_descriptions) {
        ret.sub_parser.emplace(
            desc_cont.sub_parser_string,
            sub_parser_line_t{
                "  " + desc_cont.sub_parser_string +
                    std::string(get_max_option_string_size() + 1 -
                                    desc_cont.sub_parser_string.size(),
                                ' '),
                desc_cont.description, desc_cont.parser});
    }

    return ret;
}

std::string clapp::parser::basic_parser_t::gen_short_line() const {
    std::string short_line;

    for (auto& option : options) {
        short_line +=
            " " + std::visit([](auto&& o) { return o.create_option_string(); },
                             option);
    }

    for (const argument_description_container_t& desc_cont :
         mandatory_argument_descriptions) {
        short_line += " <" + desc_cont.argument_string + ">";
        if (desc_cont.argument_type ==
            clapp::basic_parser_t::argument_type_t::variadic) {
            short_line += "...";
        }
    }

    for (const argument_description_container_t& desc_cont :
         optional_argument_descriptions) {
        short_line += " [<" + desc_cont.argument_string + ">";
        if (desc_cont.argument_type ==
            clapp::basic_parser_t::argument_type_t::variadic) {
            short_line += "...";
        }
        short_line += "]";
    }

    return short_line;
}

std::string clapp::parser::basic_parser_t::gen_opt_arg_lines(
    const clapp::parser::basic_parser_t::help_contents_t& help_contents,
    const std::size_t num_spaces) const {
    std::string ret;
    if (!help_contents.mandatory_arguments.empty()) {
        ret += "\n" + std::string(num_spaces + num_sub_spaces, ' ') +
               "Mandatory Arguments:\n";
        for (const help_line_t& line : help_contents.mandatory_arguments) {
            ret += std::string(num_spaces + num_sub_spaces, ' ') + line.name +
                   line.description + '\n';
        }
    }

    if (!help_contents.optional_arguments.empty()) {
        ret += "\n" + std::string(num_spaces + num_sub_spaces, ' ') +
               "Optional Arguments:\n";
        for (const help_line_t& line : help_contents.optional_arguments) {
            ret += std::string(num_spaces + num_sub_spaces, ' ') + line.name +
                   line.description + '\n';
        }
    }

    std::vector<help_entry_t> option_help_entries{get_option_help()};
    if (!option_help_entries.empty()) {
        ret +=
            "\n" + std::string(num_spaces + num_sub_spaces, ' ') + "Options:\n";
        for (const help_entry_t& entry : option_help_entries) {
            ret += "  " + std::string(num_spaces + num_sub_spaces, ' ') +
                   entry.option_string +
                   std::string(get_max_option_string_size() + 1 -
                                   entry.option_string.size(),
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

    if (!help_contents.sub_parser.empty()) {
        ret +=
            "\n" + std::string(num_spaces + num_sub_spaces, ' ') + "Subparser:";
        for (auto const& item : help_contents.sub_parser) {
            ret += '\n' + std::string(num_spaces + num_sub_spaces, ' ') +
                   item.second.name + item.second.description;
            if (rec_depth > 0) {
                ret += item.second.parser.gen_help_desc(
                           num_spaces + num_sub_spaces * 2, rec_depth - 1) +
                       '\n';
            }
        }
    }
    ret += gen_opt_arg_lines(help_contents, num_spaces);
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
    for (auto& validate_func : validate_functions) {
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
    auto found{get_long_options().find(opt)};
    if (found == get_long_options().end()) {
        return parse_result_t{it, std::nullopt, std::string{opt}};
    }
    if (equal_index != std::string_view::npos) {
        if (!std::holds_alternative<long_opt_param_func_t>(found->second)) {
            std::stringstream ss;
            ss << "No param allowed for option '--" << opt << "'";
            throw clapp::exception::option_param_exception_t(ss.str());
        }
        std::get<long_opt_param_func_t>(found->second)(
            opt, std::string_view{&(option[equal_index + 1])});
        return parse_result_t{it + 1, std::nullopt, std::nullopt};
    }
    if (std::holds_alternative<long_opt_param_func_t>(found->second)) {
        if (it + 1 == end) {
            std::stringstream ss;
            ss << "No param given for option '--" << opt << "'";
            throw clapp::exception::option_param_exception_t(ss.str());
        }

        it++;
        std::get<long_opt_param_func_t>(found->second)(opt,
                                                       std::string_view{*(it)});
        return parse_result_t{it + 1, std::nullopt, std::nullopt};
    }
    Expects(std::holds_alternative<long_opt_func_t>(found->second));
    std::get<long_opt_func_t>(found->second)(opt);
    return parse_result_t{it + 1, std::nullopt, std::nullopt};
}

clapp::parser::basic_parser_t::parse_result_t
clapp::parser::basic_parser_t::parse_short(const std::string_view option,
                                           arg_iterator it, arg_iterator end) {
    Expects(!option.empty());
    const std::size_t equal_index{option.find_first_of("=")};
    for (std::size_t i{0}; i < option.size(); i++) {
        auto found{get_short_options().find(option[i])};
        if (found == get_short_options().end()) {
            return parse_result_t{it, option[i], std::nullopt};
        }
        if (equal_index == i + 1) {
            if (!std::holds_alternative<short_opt_param_func_t>(
                    found->second)) {
                std::stringstream ss;
                ss << "No param allowed for option '-" << option[i] << "'";
                throw clapp::exception::option_param_exception_t(ss.str());
            }
            std::get<short_opt_param_func_t>(found->second)(
                option[i], std::string_view{&(option[equal_index + 1])});
            return parse_result_t{it + 1, std::nullopt, std::nullopt};
        }
        if (i == option.size() - 1) {
            if (std::holds_alternative<short_opt_param_func_t>(found->second)) {
                if (it + 1 == end) {
                    std::stringstream ss;
                    ss << "No param given for option '-" << option[i] << "'";
                    throw clapp::exception::option_param_exception_t(ss.str());
                }
                it++;
                std::get<short_opt_param_func_t>(found->second)(
                    option[i], std::string_view{*(it)});
                return parse_result_t{it + 1, std::nullopt, std::nullopt};
            }
            std::get<short_opt_func_t>(found->second)(option[i]);
            return parse_result_t{it + 1, std::nullopt, std::nullopt};
        }
        if (std::holds_alternative<short_opt_param_func_t>(found->second)) {
            std::stringstream ss;
            ss << "No param given for option '-" << option[i] << "'";
            throw clapp::exception::option_param_exception_t(ss.str());
        }
        std::get<short_opt_func_t>(found->second)(option[i]);
    }
    throw clapp::exception::clapp_exception_t(
        "This exection should not be thrown.");
}

clapp::value::found_func_t
clapp::parser::basic_parser_t::gen_func_print_help_and_exit(
    const int exit_code) const {
    return found_func_t{[this, exit_code]() {
        constexpr std::size_t max_rec_depth{65535};
        std::cout << gen_usage_prefix() + gen_help_msg(max_rec_depth);
        exit(exit_code);
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

clapp::parser::basic_parser_t::~basic_parser_t() = default;
