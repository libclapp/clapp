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

void clapp::parser::basic_sub_parser_t::sub_parse(arg_iterator begin,
                                                  arg_iterator end) {
    active = true;
    for (arg_iterator it = begin; it != end;) {
        std::string_view option{*it};
        parse_result_t parse_result{parse(option, it, end)};
        it = process_parse_result(it, parse_result);
    }
}

clapp::parser::basic_sub_parser_t::basic_sub_parser_t(
    clapp::basic_parser_t& parser, std::string sub_parser_name_arg,
    std::string description_arg)
    : parent_parser{parser},
      sub_parser_name{std::move(sub_parser_name_arg)},
      description{std::move(description_arg)} {
    parser.reg(reg_sub_parser_conf_t{*this, sub_parser_name, description});
}

std::string clapp::parser::basic_sub_parser_t::gen_short_line_prefix() const {
    return parent_parser.gen_short_line_prefix() + " " + sub_parser_name +
           gen_short_line();
}

void clapp::parser::basic_sub_parser_t::set_max_option_string_size(
    const std::size_t max_option_size) {
    parent_parser.set_max_option_string_size(max_option_size +
                                             num_sub_spaces * 2);
}

std::size_t clapp::parser::basic_sub_parser_t::get_max_option_string_size()
    const {
    return parent_parser.get_max_option_string_size() - num_sub_spaces * 2;
}
