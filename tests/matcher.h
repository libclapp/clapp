#ifndef CLAPP_TESTS_MATCHER_H
#define CLAPP_TESTS_MATCHER_H

#include <clapp/argument.h>
#include <clapp/option.h>
#include <gmock/gmock.h>

#include <optional>
#include <vector>

#include "helper.h"

template <typename OPTION_T>
std::optional<clapp::parser::types::variant_opt_conf_t> contains_option(
    std::vector<clapp::parser::types::variant_opt_conf_t>& options,
    OPTION_T&& option) {
    for (const auto& opt : options) {
        const bool found{std::visit(
            [&option](auto&& visited_option) {
                if constexpr (std::is_same<
                                  typename std::decay<decltype(option)>::type,
                                  char>::value) {
                    for (auto& item : visited_option.short_options) {
                        if (option == item.option) {
                            return true;
                        }
                    }
                } else {
                    for (auto& item : visited_option.long_options) {
                        if (option == item.option) {
                            return true;
                        }
                    }
                }
                return false;
            },
            opt)};
        if (found) {
            return opt;
        }
    }
    return std::nullopt;
}

template <typename OPTION_T, typename RESULT_LISTENER_T>
std::optional<clapp::parser::types::variant_opt_conf_t> contains_option(
    std::vector<clapp::parser::types::variant_opt_conf_t>& options,
    OPTION_T&& option, RESULT_LISTENER_T&& result_listener) {
    if (options.empty()) {
        *result_listener
            << "Parser-options doesn't contain any options at all.";
        return std::nullopt;
    }
    std::optional<clapp::parser::types::variant_opt_conf_t> ret{
        contains_option(options, std::forward<OPTION_T>(option))};
    if (!ret) {
        *result_listener << "Parser-options doesn't contain option '" << option
                         << "'.";
        return std::nullopt;
    }
    return ret;
}

template <typename ARGUMENT_T>
std::optional<clapp::parser::types::variant_arg_conf_t> contains_argument(
    std::vector<clapp::parser::types::variant_arg_conf_t>& arguments,
    ARGUMENT_T&& argument) {
    for (const auto& arg : arguments) {
        const bool found{std::visit(
            [&argument](auto&& visited_argument) {
                return argument == visited_argument.argument_name;
            },
            arg)};
        if (found) {
            return arg;
        }
    }
    return std::nullopt;
}

template <typename ARGUMENT_T, typename RESULT_LISTENER_T>
std::optional<clapp::parser::types::variant_arg_conf_t> contains_argument(
    std::vector<clapp::parser::types::variant_arg_conf_t>& arguments,
    ARGUMENT_T&& argument, RESULT_LISTENER_T&& result_listener) {
    if (arguments.empty()) {
        *result_listener
            << "Parser-arguments doesn't contain any arguments at all.";
        return std::nullopt;
    }
    std::optional<clapp::parser::types::variant_arg_conf_t> ret{
        contains_argument(arguments, std::forward<ARGUMENT_T>(argument))};
    if (!ret) {
        *result_listener << "Parser-arguments doesn't contain argument '"
                         << argument << "'.";
        return std::nullopt;
    }
    return ret;
}

MATCHER(BoolOptionNotGiven, "Checks, if bool option not given.") {
    *result_listener << "bool-option: "
                     << (static_cast<bool>(arg) ? "true" : "false")
                     << ", has_value(): "
                     << (arg.has_value() ? "true" : "false")
                     << ", given(): " << (arg.given() ? "true" : "false")
                     << ", value(): " << (arg.value() ? "true" : "false");
    return !static_cast<bool>(arg) && arg.has_value() && !arg.given() &&
           !arg.value();
}

MATCHER(BoolOptionGiven, "Checks, if bool option is given.") {
    *result_listener << "bool-option: "
                     << (static_cast<bool>(arg) ? "true" : "false")
                     << ", has_value(): "
                     << (arg.has_value() ? "true" : "false")
                     << ", given(): " << (arg.given() ? "true" : "false")
                     << ", value(): " << (arg.value() ? "true" : "false");
    return static_cast<bool>(arg) && arg.has_value() && arg.given() &&
           arg.value();
}

MATCHER_P(NumCountOptionGiven, count, "Checks, if num-count option is given.") {
    *result_listener << "count-option: "
                     << (static_cast<bool>(arg) ? "true" : "false")
                     << ", has_value(): "
                     << (arg.has_value() ? "true" : "false")
                     << ", given(): " << (arg.given() ? "true" : "false")
                     << ", value(): " << count << "==" << arg.value() << " = "
                     << (arg.value() == count ? "true" : "false");
    return static_cast<bool>(arg) && arg.has_value() && arg.given() &&
           arg.value() == count;
}

MATCHER_P(NumCountOptionNotGivenDefaultValue, count,
          "Checks, if num-count option is not given.") {
    *result_listener << "count-option: "
                     << (static_cast<bool>(arg) ? "true" : "false")
                     << ", has_value(): "
                     << (arg.has_value() ? "true" : "false")
                     << ", given(): " << (arg.given() ? "true" : "false")
                     << ", value(): " << count << "==" << arg.value() << " = "
                     << (arg.value() == count ? "true" : "false");
    return static_cast<bool>(arg) && arg.has_value() && !arg.given() &&
           arg.value() == count;
}

MATCHER(NumCountOptionNotGiven, "Checks, if num-count option is not given.") {
    *result_listener << "count-option: "
                     << (static_cast<bool>(arg) ? "true" : "false")
                     << ", has_value(): "
                     << (arg.has_value() ? "true" : "false")
                     << ", given(): " << (arg.given() ? "true" : "false")
                     << ", value(): " << 0 << "==" << arg.value() << " = "
                     << (arg.value() == 0 ? "true" : "false");
    return !static_cast<bool>(arg) && arg.has_value() && !arg.given() &&
           arg.value() == 0;
}

MATCHER_P(ParamOptionGiven, value, "Checks, if param option is given.") {
    *result_listener << "param-option: "
                     << (static_cast<bool>(arg) ? "true" : "false")
                     << ", has_value(): "
                     << (arg.has_value() ? "true" : "false")
                     << ", given(): " << (arg.given() ? "true" : "false");
    if (!(static_cast<bool>(arg) && arg.has_value() && arg.given())) {
        return false;
    }
    *result_listener << ", value(): " << clapp::to_string(value)
                     << "==" << clapp::to_string(arg.value()) << " = ";
    return compare_value(arg.value(), value, result_listener);
}

MATCHER_P(ParamOptionNotGivenDefaultValue, default_value,
          "Checks, if param option is not given and default value is set "
          "correctly.") {
    *result_listener << "param-option: "
                     << (static_cast<bool>(arg) ? "true" : "false")
                     << ", has_value(): "
                     << (arg.has_value() ? "true" : "false")
                     << ", given(): " << (arg.given() ? "true" : "false");
    if (!(static_cast<bool>(arg) && arg.has_value() && !arg.given())) {
        return false;
    }
    *result_listener << ", value(): " << clapp::to_string(default_value)
                     << "==" << clapp::to_string(arg.value()) << " = ";
    return compare_value(arg.value(), default_value, result_listener);
}

MATCHER(ParamOptionNotGiven, "Checks, if param option is not given.") {
    bool caught_value_undefined{false};
    try {
        static_cast<void>(arg.value());
    } catch (clapp::exception::value_undefined_t& /*exception*/) {
        caught_value_undefined = true;
    }
    *result_listener << "param-option: "
                     << (static_cast<bool>(arg) ? "true" : "false")
                     << ", has_value(): "
                     << (arg.has_value() ? "true" : "false")
                     << ", given(): " << (arg.given() ? "true" : "false")
                     << ", value() throws value_undefined = "
                     << (caught_value_undefined ? "true" : "false");
    return !static_cast<bool>(arg) && !arg.has_value() && !arg.given() &&
           caught_value_undefined;
}

MATCHER_P(VectorParamOptionGiven, value,
          "Checks, if vector param option is given.") {
    *result_listener << "param-option: "
                     << (static_cast<bool>(arg) ? "true" : "false")
                     << ", has_value(): "
                     << (arg.has_value() ? "true" : "false")
                     << ", given(): " << (arg.given() ? "true" : "false");
    if (!(static_cast<bool>(arg) && arg.has_value() && arg.given())) {
        return false;
    }
    const auto& argument{arg.value()};
    std::string val_str{
        std::accumulate(value.begin(), value.end(), std::string{},
                        [](std::string& str, const auto& itm) {
                            return str += clapp::to_string(itm) + ", ";
                        })};
    if (!val_str.empty()) {
        val_str = val_str.substr(0, val_str.size() - 2);
    }
    std::string arg_str{
        std::accumulate(argument.begin(), argument.end(), std::string{},
                        [](std::string& str, const auto& itm) {
                            return str += clapp::to_string(itm) + ", ";
                        })};
    if (!arg_str.empty()) {
        arg_str = arg_str.substr(0, arg_str.size() - 2);
    }
    *result_listener << ", value(): {" << val_str << "}=={" << arg_str
                     << "} = ";
    return compare_value_vector(argument, value, result_listener);
}

MATCHER(VectorParamOptionNotGiven,
        "Checks, if vector param option is not given.") {
    *result_listener << "vector-param-option: "
                     << (static_cast<bool>(arg) ? "true" : "false")
                     << ", has_value(): "
                     << (arg.has_value() ? "true" : "false")
                     << ", given(): " << (arg.given() ? "true" : "false");
    if (!(!static_cast<bool>(arg) && !arg.has_value() && !arg.given())) {
        return false;
    }
    *result_listener << ", size(): 0==" << clapp::to_string(arg.value().size())
                     << " = " << (arg.value().empty() ? "true" : "false");
    return arg.value().empty();
}

MATCHER_P(ContainsLongOption, option, "Checks, if long option is given.") {
    std::vector<clapp::parser::types::variant_opt_conf_t> options{
        arg.get_options().options};
    std::optional<clapp::parser::types::variant_opt_conf_t> found_opt{
        contains_option(options, option, result_listener)};
    if (found_opt == std::nullopt) {
        return false;
    }
    *result_listener << "Found parser-option doesn't contain option '" << option
                     << "'";
    return std::visit(
        [this](auto&& opt) { return opt.contains_option(option); },
        found_opt.value());
}

MATCHER_P(ContainsShortOption, option, "Checks, if short option is given.") {
    std::vector<clapp::parser::types::variant_opt_conf_t> options{
        arg.get_options().options};
    std::optional<clapp::parser::types::variant_opt_conf_t> found_opt{
        contains_option(options, option, result_listener)};
    if (found_opt == std::nullopt) {
        return false;
    }
    *result_listener << "Found parser-option doesn't contain option '" << option
                     << "'";
    return std::visit(
        [this](auto&& opt) { return opt.contains_option(option); },
        found_opt.value());
}

MATCHER_P(ContainsArgument, argument, "Checks, if argument is contained.") {
    std::vector<clapp::parser::types::variant_arg_conf_t> arguments{
        arg.get_arguments()};
    std::optional<clapp::parser::types::variant_arg_conf_t> found_arg{
        contains_argument(arguments, argument, result_listener)};
    if (found_arg == std::nullopt) {
        return false;
    }
    *result_listener << "Found parser-argument doesn't contain argument '"
                     << argument << "'";
    return std::visit(
        [this](auto&& arg_param) {
            return arg_param.argument_name == argument;
        },
        found_arg.value());
}

MATCHER_P(ArgumentGiven, value, "Checks, if argument is given.") {
    *result_listener << "argument: "
                     << (static_cast<bool>(arg) ? "true" : "false")
                     << ", has_value(): "
                     << (arg.has_value() ? "true" : "false")
                     << ", given(): " << (arg.given() ? "true" : "false");
    if (!(static_cast<bool>(arg) && arg.has_value() && arg.given())) {
        return false;
    }
    *result_listener << ", value(): " << clapp::to_string(value)
                     << "==" << clapp::to_string(arg.value()) << " = ";
    return compare_value(arg.value(), value, result_listener);
}

MATCHER_P(ArgumentNotGivenDefaultValue, default_value, std::string{}) {
    *result_listener << "argument: "
                     << (static_cast<bool>(arg) ? "true" : "false")
                     << ", has_value(): "
                     << (arg.has_value() ? "true" : "false")
                     << ", given(): " << (arg.given() ? "true" : "false");
    if (!(static_cast<bool>(arg) && arg.has_value() && !arg.given())) {
        return false;
    }
    *result_listener << ", value(): " << clapp::to_string(default_value)
                     << "==" << clapp::to_string(arg.value()) << " = ";
    return compare_value(arg.value(), default_value, result_listener);
}

MATCHER_P(VariadicArgumentGiven, value,
          "Checks, if variadic argument is given.") {
    *result_listener << "argument: "
                     << (static_cast<bool>(arg) ? "true" : "false")
                     << ", has_value(): "
                     << (arg.has_value() ? "true" : "false")
                     << ", given(): " << (arg.given() ? "true" : "false");
    if (!(static_cast<bool>(arg) && arg.has_value() && arg.given())) {
        return false;
    }
    const auto& argument{arg.value()};
    std::string val_str{
        std::accumulate(value.begin(), value.end(), std::string{},
                        [](std::string& str, const auto& itm) {
                            return str += clapp::to_string(itm) + ", ";
                        })};
    if (!val_str.empty()) {
        val_str = val_str.substr(0, val_str.size() - 2);
    }
    std::string arg_str{
        std::accumulate(argument.begin(), argument.end(), std::string{},
                        [](std::string& str, const auto& itm) {
                            return str += clapp::to_string(itm) + ", ";
                        })};
    if (!arg_str.empty()) {
        arg_str = arg_str.substr(0, arg_str.size() - 2);
    }
    *result_listener << ", value(): {" << val_str << "}=={" << arg_str
                     << "} = ";
    return compare_value_vector(argument, value, result_listener);
}

MATCHER(VariadicArgumentNotGiven,
        "Checks, if variadic argument is not given.") {
    *result_listener << "vector-param-option: "
                     << (static_cast<bool>(arg) ? "true" : "false")
                     << ", has_value(): "
                     << (arg.has_value() ? "true" : "false")
                     << ", given(): " << (arg.given() ? "true" : "false");
    if (!(!static_cast<bool>(arg) && !arg.has_value() && !arg.given())) {
        return false;
    }
    *result_listener << ", size(): 0==" << clapp::to_string(arg.value().size())
                     << " = " << (arg.value().empty() ? "true" : "false");
    return arg.value().empty();
}

MATCHER(ArgumentNotGiven, "Checks, if argument is not given.") {
    *result_listener << "argument: "
                     << (static_cast<bool>(arg) ? "true" : "false")
                     << ", has_value(): "
                     << (arg.has_value() ? "true" : "false")
                     << ", given(): " << (arg.given() ? "true" : "false");
    return !static_cast<bool>(arg) && !arg.has_value() && !arg.given();
}

#endif
