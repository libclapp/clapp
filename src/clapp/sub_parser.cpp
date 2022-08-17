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

#include <clapp/sub_parser.h>

clapp::parser::basic_sub_parser_t::~basic_sub_parser_t() = default;

void clapp::parser::basic_sub_parser_t::sub_parse(types::arg_iterator_t begin,
                                                  types::arg_iterator_t end) {
    active = true;
    for (types::arg_iterator_t it = begin; it != end;) {
        std::string_view option{*it};
        parse_result_t parse_result{parse(option, it, end)};
        it = process_parse_result(it, parse_result);
    }
}

clapp::parser::basic_sub_parser_t::basic_sub_parser_t(
    clapp::basic_parser_t& parser, std::string sub_parser_name_arg,
    std::string description_arg)
    : basic_sub_parser_t{
          parser, std::move(sub_parser_name_arg), std::move(description_arg),
          clapp::parser::types::logic_operator_type_t::logic_and} {}

clapp::parser::basic_sub_parser_t::basic_sub_parser_t(
    clapp::basic_parser_t& parser, std::string sub_parser_name_arg,
    std::string description_arg,
    const clapp::parser::types::logic_operator_type_t logic_operator_type)
    : basic_parser_t{logic_operator_type},
      parent_parser{parser},
      sub_parser_name{std::move(sub_parser_name_arg)},
      description{std::move(description_arg)} {
    set_print_and_exit_func([this](const std::string_view print_msg,
                                   const int exit_code) {
        return parent_parser.get_print_and_exit_func()(print_msg, exit_code);
    });
    parser.reg(reg_sub_parser_conf_t{*this, sub_parser_name, description});
}

std::string clapp::parser::basic_sub_parser_t::gen_short_line_prefix() const {
    const std::string short_line_prefix{parent_parser.gen_short_line_prefix()};
    const std::string short_line{gen_short_line()};
    if (!short_line.empty()) {
        return short_line_prefix + " " + sub_parser_name + " " + short_line;
    }
    return short_line_prefix + " " + sub_parser_name;
}
