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

#include <clapp/option_container.h>
#include <clapp/parser.h>

clapp::parser::basic_option_container_t::~basic_option_container_t() = default;

clapp::parser::basic_option_container_t::basic_option_container_t(
    const types::logic_operator_type_t logic_operator_type)
    : options{logic_operator_type} {}

const clapp::parser::types::variant_opt_conf_t*
clapp::parser::basic_option_container_t::find_option(
    const std::string_view opt) const {
    for (types::variant_opt_conf_vec_t::const_iterator it{
             options.options.begin()};
         it != options.options.end(); it++) {
        if (std::visit(
                [opt](auto&& option) {
                    return bool{option.contains_option(opt)};
                },
                *it)) {
            return &(*it);
        }
    }
    for (types::variant_opt_conf_container_ptr_vec_t::const_iterator it{
             options.containers.cbegin()};
         it != options.containers.cend(); it++) {
        Expects(*it != nullptr);

        const clapp::parser::types::variant_opt_conf_t* ret{
            (*it)->find_option(opt)};
        if (ret != nullptr) {
            return ret;
        }
    }
    return nullptr;
}

const clapp::parser::types::variant_opt_conf_t*
clapp::parser::basic_option_container_t::find_option(const char opt) const {
    for (types::variant_opt_conf_vec_t::const_iterator it{
             options.options.begin()};
         it != options.options.end(); it++) {
        if (std::visit(
                [opt](auto&& option) {
                    return bool{option.contains_option(opt)};
                },
                *it)) {
            return &(*it);
        }
    }
    for (types::variant_opt_conf_container_ptr_vec_t::const_iterator it{
             options.containers.cbegin()};
         it != options.containers.cend(); it++) {
        Expects(*it != nullptr);

        const clapp::parser::types::variant_opt_conf_t* ret{
            (*it)->find_option(opt)};
        if (ret != nullptr) {
            return ret;
        }
    }
    return nullptr;
}

const clapp::parser::types::variant_opt_conf_t*
clapp::parser::basic_option_container_t::find_parser_option(
    const std::string_view opt) const {
    return get_const_parser().find_option(opt);
}

const clapp::parser::types::variant_opt_conf_t*
clapp::parser::basic_option_container_t::find_parser_option(
    const char opt) const {
    return get_const_parser().find_option(opt);
}

clapp::parser::types::variant_opt_conf_container_t&
clapp::parser::basic_option_container_t::get_options() {
    return options;
}

const clapp::parser::types::variant_opt_conf_container_t&
clapp::parser::basic_option_container_t::get_options() const {
    return options;
}

clapp::parser::types::validate_func_vec_t&
clapp::parser::basic_option_container_t::get_validate_functions() {
    return validate_functions;
}

const clapp::parser::types::validate_func_vec_t&
clapp::parser::basic_option_container_t::get_validate_functions() const {
    return validate_functions;
}

std::string clapp::parser::basic_option_container_t::gen_short_option_line()
    const {
    return options.gen_short_option_line();
}

clapp::parser::types::help_entry_vec_t
clapp::parser::basic_option_container_t::get_option_help() const {
    return options.get_option_help();
}

void clapp::parser::basic_option_container_t::reg(
    types::variant_opt_conf_container_t* opt_container) {
    if (opt_container != nullptr) {
        options.containers.push_back(opt_container);
    }
}

void clapp::parser::basic_option_container_t::validate_options() const {
    std::optional<std::vector<std::string>> exclusive_or_options;
    std::optional<std::vector<std::string>> mandatory_and_options;
    if (options.logic_operator_type ==
        types::logic_operator_type_t::logic_xor) {
        std::optional<std::string> given_xor_option;
        std::optional<std::vector<std::string>> given_and_options;
        validate_options_xor(&options, exclusive_or_options,
                             mandatory_and_options, given_xor_option,
                             given_and_options, false);
    } else {
        Expects(options.logic_operator_type ==
                types::logic_operator_type_t::logic_and);
        std::optional<std::string> given_xor_option;
        std::optional<std::vector<std::string>> given_and_options;
        validate_options_and(&options, mandatory_and_options,
                             exclusive_or_options, given_xor_option,
                             given_and_options, false);
    }
}

void clapp::parser::basic_option_container_t::validate_options_xor(
    const types::variant_opt_conf_container_t* options,
    std::optional<std::vector<std::string>>& exclusive_or_options,
    std::optional<std::vector<std::string>>& mandatory_and_options,
    std::optional<std::string>& given_xor_option,
    std::optional<std::vector<std::string>>& given_and_options,
    bool summarize_xor_options) {

    if (!exclusive_or_options.has_value()) {
        exclusive_or_options.emplace(std::vector<std::string>{});
        append_exclusive_or_options(options, exclusive_or_options.value());
    }

    Expects(exclusive_or_options.has_value());
    for (const auto& option : options->options) {
        std::visit(
            [&exclusive_or_options, &given_xor_option](auto&& opt) {
                if (opt.given_func()) {
                    if (given_xor_option.has_value()) {
                        const std::string exclusive_or_options_str{
                            std::accumulate(
                                exclusive_or_options.value().begin(),
                                exclusive_or_options.value().end(),
                                std::string{},
                                [](std::string& lhs, const std::string& rhs) {
                                    return lhs.empty()
                                               ? ("'" + rhs + "'")
                                               : (lhs + ", '" + rhs + "'");
                                })};

                        throw clapp::exception::option_param_exception_t{
                            std::string{"Mutually exclusive option '"} +
                            opt.option_string + "' given, but option '" +
                            given_xor_option.value() +
                            "' was also given! The following options " +
                            exclusive_or_options_str +
                            " are mutual exclusive."};
                    }
                    given_xor_option = opt.option_string;
                    opt.validate_value_func(opt.option_string);
                }
            },
            option);
    }
    for (types::variant_opt_conf_container_ptr_vec_t::const_iterator it{
             options->containers.cbegin()};
         it != options->containers.cend(); it++) {
        Expects(*it != nullptr);
        if ((*it)->logic_operator_type ==
            types::logic_operator_type_t::logic_and) {
            std::optional<std::vector<std::string>> new_mandatory_and_options;
            std::optional<std::vector<std::string>> new_given_and_options;
            clapp::parser::option_container_t::validate_options_and(
                (*it), new_mandatory_and_options, exclusive_or_options,
                given_xor_option, new_given_and_options, true);
            std::cout << "validate_options_xor(): new given and options: "
                      << new_given_and_options.value().size() << std::endl;
            if (new_given_and_options.has_value() &&
                !new_given_and_options.value().empty()) {
                const std::string xor_option_str{std::accumulate(
                    new_given_and_options.value().begin(),
                    new_given_and_options.value().end(), std::string{},
                    [](std::string& lhs, const std::string& rhs) {
                        return lhs.empty() ? ("'" + rhs + "'")
                                           : (lhs + ", '" + rhs + "'");
                    })};
                if (given_xor_option.has_value()) {
                    throw clapp::exception::option_param_exception_t{
                        std::string{"Mutually exclusive option '"} +
                        xor_option_str + "' given, but option '" +
                        given_xor_option.value() +
                        "' was also given!"};  // TODO: Generate text the
                                               // following options are mutual
                                               // exclusive
                } else {
                    given_xor_option = xor_option_str;
                }
            }
        } else if ((*it)->logic_operator_type ==
                   types::logic_operator_type_t::logic_xor) {
            clapp::parser::option_container_t::validate_options_xor(
                (*it), exclusive_or_options, mandatory_and_options,
                given_xor_option, given_and_options, false);
        } else {
            Expects(false);
        }
    }
    if (summarize_xor_options) {
        const std::string exclusive_or_options_str{
            std::accumulate(exclusive_or_options.value().begin(),
                            exclusive_or_options.value().end(), std::string{},
                            [](std::string& lhs, const std::string& rhs) {
                                return lhs.empty() ? rhs : (lhs + " | " + rhs);
                            })};
        given_and_options.value().push_back(exclusive_or_options_str);
    }
}

void clapp::parser::basic_option_container_t::validate_options_and(
    const types::variant_opt_conf_container_t* options,
    std::optional<std::vector<std::string>>& mandatory_and_options,
    std::optional<std::vector<std::string>>& exclusive_or_options,
    std::optional<std::string>& given_xor_option,
    std::optional<std::vector<std::string>>& given_and_options,
    const bool allow_empty) {
    if (!mandatory_and_options.has_value()) {
        mandatory_and_options.emplace(std::vector<std::string>{});
        append_mandatory_and_options(options, mandatory_and_options.value());
    }

    bool final_check{false};
    if (!given_and_options.has_value()) {
        given_and_options.emplace(std::vector<std::string>{});
        final_check = true;
    }

    Expects(mandatory_and_options.has_value());
    for (const auto& option : options->options) {
        std::visit(
            [&given_and_options, &given_xor_option,
             &xor_options_str](auto&& opt) {
                if (opt.purpose == parser::types::purpose_t::mandatory) {
                    if (opt.given_func()) {
                        if (given_xor_option) {
                            Expects(!xor_options_str.empty());
                            throw clapp::exception::option_exception_t{
                                std::string{"Mutually exclusive option '"} +
                                opt.option_string + "' given, but option '" +
                                given_xor_option.value() +
                                "' was also given! The following options " +
                                xor_options_str + " are mutual exclusive."};
                        }
                        given_and_options.value().push_back(opt.option_string);
                    }
                }
                opt.validate_value_func(opt.option_string);
            },
            option);
    }
    for (types::variant_opt_conf_container_ptr_vec_t::const_iterator it{
             options->containers.cbegin()};
         it != options->containers.cend(); it++) {
        Expects(*it != nullptr);
        if ((*it)->logic_operator_type ==
            types::logic_operator_type_t::logic_and) {
            clapp::parser::option_container_t::validate_options_and(
                (*it), mandatory_and_options, exclusive_or_options,
                given_xor_option, given_and_options, allow_empty);
        } else if ((*it)->logic_operator_type ==
                   types::logic_operator_type_t::logic_xor) {
            std::optional<std::vector<std::string>> my_exclusive_or_options;
            std::optional<std::string> my_given_xor_option;
            std::optional<std::vector<std::string>> my_given_and_options{
                std::vector<std::string>{}};

            clapp::parser::option_container_t::validate_options_xor(
                (*it), my_exclusive_or_options, mandatory_and_options,
                my_given_xor_option, my_given_and_options, true);

            Expects(my_given_and_options.has_value());
            if (my_given_xor_option.has_value()) {
                given_and_options.value().push_back(std::accumulate(
                    my_given_and_options.value().begin(),
                    my_given_and_options.value().end(), std::string{},
                    [](std::string& lhs, const std::string& rhs) {
                        return lhs.empty() ? ("'" + rhs + "'")
                                           : (lhs + ", '" + rhs + "'");
                    }));
            }
        } else {
            Expects(false);
        }
    }

    if (final_check) {
        if (allow_empty && given_and_options.value().empty()) {
            return;
        }
        if (given_and_options.value().size() ==
            mandatory_and_options.value().size()) {
            return;
        }
        const std::string mandatory_and_options_str{
            std::accumulate(mandatory_and_options.value().begin(),
                            mandatory_and_options.value().end(), std::string{},
                            [](std::string& lhs, const std::string& rhs) {
                                return lhs.empty() ? ("'" + rhs + "'")
                                                   : (lhs + ", '" + rhs + "'");
                            })};
        const std::string given_and_options_str{std::accumulate(
            given_and_options.value().begin(), given_and_options.value().end(),
            std::string{}, [](std::string& lhs, const std::string& rhs) {
                return lhs.empty() ? ("'" + rhs + "'")
                                   : (lhs + ", '" + rhs + "'");
            })};
        throw clapp::exception::option_param_exception_t{
            std::string{"Only the following mandatory options "} +
            given_and_options_str +
            " were given, but at least the following options are required: " +
            mandatory_and_options_str};
    }
}

void clapp::parser::basic_option_container_t::append_mandatory_and_options(
    const types::variant_opt_conf_container_t* options,
    std::vector<std::string>& mandatory_and_options) {
    for (const auto& option : options->options) {
        std::visit(
            [&mandatory_and_options](auto&& opt) {
                if (opt.purpose == parser::types::purpose_t::mandatory) {
                    mandatory_and_options.push_back(opt.option_string);
                }
            },
            option);
    }
    for (types::variant_opt_conf_container_ptr_vec_t::const_iterator it{
             options->containers.cbegin()};
         it != options->containers.cend(); it++) {
        Expects(*it != nullptr);
        if ((*it)->logic_operator_type ==
            types::logic_operator_type_t::logic_and) {
            clapp::parser::option_container_t::append_mandatory_and_options(
                (*it), mandatory_and_options);
        } else if ((*it)->logic_operator_type ==
                   types::logic_operator_type_t::logic_xor) {
            std::vector<std::string> exclusive_or_options;
            clapp::parser::option_container_t::append_exclusive_or_options(
                (*it), exclusive_or_options);
            if (exclusive_or_options.size() == 1) {
                mandatory_and_options.push_back(exclusive_or_options.front());
            } else {
                const std::string exclusive_or_options_str{std::accumulate(
                    exclusive_or_options.begin(), exclusive_or_options.end(),
                    std::string{},
                    [](std::string& lhs, const std::string& rhs) {
                        return lhs.empty() ? rhs : lhs + " | " + rhs;
                    })};
                mandatory_and_options.push_back(
                    " ( " + exclusive_or_options_str + " ) ");
            }
        }
    }
}

void clapp::parser::basic_option_container_t::append_exclusive_or_options(
    const types::variant_opt_conf_container_t* options,
    std::vector<std::string>& exclusive_or_options) {
    for (const auto& option : options->options) {
        std::visit(
            [&exclusive_or_options](auto&& o) {
                exclusive_or_options.push_back(o.option_string);
            },
            option);
    }
    for (types::variant_opt_conf_container_ptr_vec_t::const_iterator it{
             options->containers.cbegin()};
         it != options->containers.cend(); it++) {
        Expects(*it != nullptr);
        if ((*it)->logic_operator_type ==
            types::logic_operator_type_t::logic_xor) {
            clapp::parser::option_container_t::append_exclusive_or_options(
                (*it), exclusive_or_options);
        } else if ((*it)->logic_operator_type ==
                   types::logic_operator_type_t::logic_and) {
            std::vector<std::string> mandatory_and_options;
            clapp::parser::option_container_t::append_mandatory_and_options(
                (*it), mandatory_and_options);
            if (mandatory_and_options.size() == 1) {
                exclusive_or_options.push_back(mandatory_and_options.front());
            } else {
                const std::string mandatory_and_options_str{std::accumulate(
                    mandatory_and_options.begin(), mandatory_and_options.end(),
                    std::string{},
                    [](std::string& lhs, const std::string& rhs) {
                        return lhs.empty() ? rhs : lhs + " " + rhs;
                    })};
                exclusive_or_options.push_back(
                    " ( " + mandatory_and_options_str + " ) ");
            }
        }
    }
}

clapp::parser::option_container_t::option_container_t(
    basic_option_container_t& container,
    const types::logic_operator_type_t logic_operator_type)
    : basic_option_container_t{logic_operator_type}, cont{container} {
    container.reg(&get_options());
}

clapp::parser::option_container_t::~option_container_t() = default;

clapp::parser::basic_parser_t& clapp::parser::option_container_t::get_parser() {
    return cont.get_parser();
}

const clapp::parser::basic_parser_t&
clapp::parser::option_container_t::get_const_parser() const {
    return cont.get_const_parser();
}
