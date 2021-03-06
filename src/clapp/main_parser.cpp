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

clapp::parser::basic_main_parser_t::basic_main_parser_t() = default;

clapp::parser::basic_main_parser_t::~basic_main_parser_t() = default;

clapp::parser::basic_main_parser_t::operator bool() const {
    return executable.has_value();
}

std::string clapp::parser::basic_main_parser_t::get_executable() const {
    if (executable.has_value()) {
        return executable.value();
    }
    throw no_executable_exception_t{"The parser does not know the executable."};
}

void clapp::parser::basic_main_parser_t::parse(int argc,
                                               const char* const* argv) {
    arg_t arg{argv, argc};
    parse(arg);
}

void clapp::parser::basic_main_parser_t::parse(const arg_t& arg) {
    arg_iterator it{arg.cbegin()};
    executable = *it;
    parse(it + 1, arg.cend());
}

void clapp::parser::basic_main_parser_t::parse_and_validate(
    int argc, const char* const* argv) {
    parse(argc, argv);
    validate_recursive();
}

std::string clapp::parser::basic_main_parser_t::gen_short_line_prefix() const {
    return get_executable() + gen_short_line();
}

void clapp::parser::basic_main_parser_t::set_max_option_string_size(
    const std::size_t max_option_size) {
    max_option_string_size = max_option_size;
}

std::size_t clapp::parser::basic_main_parser_t::get_max_option_string_size()
    const {
    return max_option_string_size;
}
