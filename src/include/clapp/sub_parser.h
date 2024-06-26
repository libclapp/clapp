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

#ifndef CLAPP_SUB_PARSER_H
#define CLAPP_SUB_PARSER_H

#include <clapp/parser.h>

namespace clapp {

inline namespace parser {
class basic_sub_parser_t : public basic_parser_t {
   public:
    basic_sub_parser_t(
        basic_parser_t& parser, std::string sub_parser_name_arg,
        std::string description_arg,
        clapp::parser::types::logic_operator_type_t logic_operator_type);

    basic_sub_parser_t(basic_parser_t& parser, std::string sub_parser_name_arg,
                       std::string description_arg);

    ~basic_sub_parser_t() override;

    // TODO(martinw): enable, if basic_parser has implemented this too
    basic_sub_parser_t(const basic_sub_parser_t& inst) = delete;
    basic_sub_parser_t(basic_sub_parser_t&& inst) noexcept = delete;
    basic_sub_parser_t& operator=(const basic_sub_parser_t& inst) = delete;
    basic_sub_parser_t& operator=(basic_sub_parser_t&& inst) noexcept = delete;

    [[nodiscard]] inline bool is_active() const noexcept override;
    [[nodiscard]] constexpr explicit operator bool() const noexcept;

    [[nodiscard]] inline std::string get_sub_parser_name() const;
    parse_result_t sub_parse(types::arg_iterator_t begin,
                             types::arg_iterator_t end);

    [[nodiscard]] std::string gen_short_line_prefix() const override;

   private:
    basic_parser_t& parent_parser;
    std::string sub_parser_name;
    std::string description;
    bool active{false};
};

}  // namespace parser

}  // namespace clapp

#include <clapp/sub_parser.hpp>

#endif
