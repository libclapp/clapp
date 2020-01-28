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

#include <clapp/main_parser.h>

clapp::parser::basic_main_parser_t::basic_main_parser_t() = default;

clapp::parser::basic_main_parser_t::~basic_main_parser_t() = default;

clapp::parser::basic_main_parser_t::operator bool() const {
    return executable.has_value();
}

std::string clapp::parser::basic_main_parser_t::get_executable() const {
    return executable.value();
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

std::string clapp::parser::basic_main_parser_t::gen_help_prefix() const {
    return basic_parser_t::gen_help_prefix() + get_executable();
}
