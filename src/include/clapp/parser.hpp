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

#include <sstream>

template <clapp::parser::types::argument_type_t argument_type>
void clapp::parser::basic_parser_t::reg(
    types::basic_reg_argument_conf_t<argument_type>&& config) {
    if (config.argument_name.empty()) {
        std::stringstream string_stream;
        string_stream << "Argument name must be at least one char long.";
        throw clapp::exception::argument_exception_t{string_stream.str()};
    }

    const std::size_t num_arguments{arguments.size()};
    if (num_arguments > 0 && std::holds_alternative<types::variadic_arg_conf_t>(
                                 arguments[num_arguments - 1])) {
        std::stringstream string_stream;
        string_stream << "Can't register argument '" << config.argument_name
                      << "' when variadic arguments are already registered.";
        throw clapp::exception::argument_exception_t{string_stream.str()};
    }

    if (config.purpose == types::purpose_t::mandatory) {
        if (!get_optional_argument_descriptions().empty()) {
            std::stringstream string_stream;
            string_stream
                << "Can't register mandatory argument '" << config.argument_name
                << "' when optional arguments are already registered.";
            throw clapp::exception::argument_exception_t{string_stream.str()};
        }
        get_mandatory_argument_descriptions().push_back(
            {config.argument_name, config.description, argument_type});
    } else {
        if (!get_sub_parser_descriptions().empty()) {
            std::stringstream string_stream;
            string_stream << "Can't register optional argument '"
                          << config.argument_name
                          << "' when a sub-parser is already registered.";
            throw clapp::exception::argument_exception_t{string_stream.str()};
        }
        get_optional_argument_descriptions().push_back(
            {config.argument_name, config.description, argument_type});
    }

    for (const auto& arg : arguments) {
        std::visit(
            [&config](auto&& argument) {
                if (argument.argument_name == config.argument_name) {
                    std::stringstream string_stream;
                    string_stream
                        << "Can't register argument '" << config.argument_name
                        << "', as another argument with this name is already "
                           "registered.";
                    throw clapp::exception::argument_exception_t{
                        string_stream.str()};
                }
            },
            arg);
    }

    arguments.push_back(config);
}

inline bool clapp::parser::basic_parser_t::is_active() const noexcept {
    return true;
}

constexpr std::string_view
clapp::parser::basic_parser_t::gen_usage_prefix() noexcept {
    return "Usage:\n";
}

#endif
