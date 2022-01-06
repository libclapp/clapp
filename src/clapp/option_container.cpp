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
    return nullptr;
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
