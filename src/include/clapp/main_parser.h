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

#ifndef CLAPP_MAIN_PARSER_H
#define CLAPP_MAIN_PARSER_H

#include <clapp/parser.h>

namespace clapp {

inline namespace parser {
class basic_main_parser_t : public basic_parser_t {
   public:
    using basic_parser_t::basic_parser_t;
    ~basic_main_parser_t() override;

    // TODO(martinw): enable, if basic_parser has implemented this too
    basic_main_parser_t(const basic_main_parser_t& inst) = delete;
    basic_main_parser_t(basic_main_parser_t&& inst) noexcept = delete;
    basic_main_parser_t& operator=(const basic_main_parser_t& inst) = delete;
    basic_main_parser_t& operator=(basic_main_parser_t&& inst) noexcept =
        delete;

    using basic_parser_t::parse;
    [[nodiscard]] std::optional<clapp::value::exit_t> parse(
        int argc, const char* const* argv);
    [[nodiscard]] std::optional<clapp::value::exit_t> parse(
        const types::arg_t& arg);

    [[nodiscard]] std::optional<clapp::value::exit_t> parse_and_validate(
        int argc, const char* const* argv);

    [[nodiscard]] explicit operator bool() const;
    [[nodiscard]] std::string get_executable() const;

    [[nodiscard]] std::string gen_short_line_prefix() const override;

   private:
    std::optional<std::string> executable{};
};

}  // namespace parser

}  // namespace clapp

#endif
