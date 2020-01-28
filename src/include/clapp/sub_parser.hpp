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

#ifndef CLAPP_SUB_PARSER_HPP
#define CLAPP_SUB_PARSER_HPP

#include <clapp/sub_parser.h>

clapp::parser::basic_sub_parser_::operator bool() const { return active; }

std::string clapp::parser::basic_sub_parser_::get_sub_parser_name() const {
    return sub_parser_name;
}

void clapp::parser::basic_sub_parser_::sub_parse(arg_iterator begin,
                                                 arg_iterator end) {
    active = true;
    for (arg_iterator it = begin; it != end;) {
        std::string_view option{*it};
        clapp::basic_parser::parse_result_t parse_result{
            parse(option, it, end)};
        if (it == parse_result.it && parse_parent) {
            parse_result = parent_parser.parse(option, it, end);
        }
        it = process_parse_result(it, parse_result);
    }
}

clapp::parser::basic_sub_parser_::basic_sub_parser_(
    clapp::basic_parser& parser, const std::string& _sub_parser_name,
    const std::string& _description, bool _parse_parent)
    : basic_parser{},
      parent_parser{parser},
      sub_parser_name{_sub_parser_name},
      description{_description},
      parse_parent{_parse_parent} {
    parser.reg(
        reg_sub_parser_conf_t{[this](arg_iterator begin, arg_iterator end) {
                                  return this->sub_parse(begin, end);
                              },
                              sub_parser_name, description});
}

#endif
