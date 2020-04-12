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

inline std::string clapp::parser::basic_sub_parser_t::get_sub_parser_name()
    const {
    return sub_parser_name;
}

inline bool clapp::parser::basic_sub_parser_t::is_active() const noexcept {
    return active;
}

constexpr clapp::parser::basic_sub_parser_t::operator bool() const noexcept {
    return active;
}

#endif
