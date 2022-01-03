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

#include <clapp/exception.h>
#include <clapp/main_parser.h>

clapp::parser::basic_main_parser_t::~basic_main_parser_t() = default;

clapp::parser::basic_main_parser_t::operator bool() const {
    return executable.has_value();
}

std::string clapp::parser::basic_main_parser_t::get_executable() const {
    if (!executable.has_value()) {
        throw no_executable_exception_t{
            "The parser does not know the executable."};
    }
    std::string ret{executable.value()};
    return ret;
}

std::optional<clapp::value::exit_t> clapp::parser::basic_main_parser_t::parse(
    int argc, const char* const* argv) {
    types::arg_t arg{argv, argc};
    return parse(arg);
}

std::optional<clapp::value::exit_t> clapp::parser::basic_main_parser_t::parse(
    const types::arg_t& arg) {
    types::arg_iterator_t ait{arg.begin()};
    executable = *ait;
    return parse(ait + 1, arg.end());
}

std::optional<clapp::value::exit_t>
clapp::parser::basic_main_parser_t::parse_and_validate(
    int argc, const char* const* argv) {
    const std::optional<clapp::value::exit_t> ret{parse(argc, argv)};
    if (ret) {
        return ret;
    }
    validate_recursive();
    return {};
}

std::string clapp::parser::basic_main_parser_t::gen_short_line_prefix() const {
    const std::string short_line{gen_short_line()};
    std::string exec{get_executable()};
    if (!short_line.empty()) {
        return exec + ' ' + short_line;
    }
    return exec;
}
