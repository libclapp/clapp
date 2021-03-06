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

#include <clapp/option.h>

void clapp::option::check_long_option(const std::string_view option) {
    if (option.find_first_of("\n\t\r =") != std::string::npos) {
        std::stringstream ss;
        ss << "Whitespaces and equalsigns are not allowed in long options: "
              "option='"
           << option << "'";
        throw clapp::exception::option_exception_t(ss.str());
    }
}

void clapp::option::check_short_option(const char option) {
    switch (option) {
        case '\n':
        case '\t':
        case '\r':
        case ' ':
        case '=': {
            std::stringstream ss;
            ss << "Whitespaces and equalsigns are not allowed in short "
                  "options: option='"
               << option << "'";
            throw clapp::exception::option_exception_t(ss.str());
        }
        default:
            return;
    }
}

clapp::option::bool_option_t::callbacks_t
clapp::option::bool_option_t::create_callbacks(bool_option_t* inst) {
    return callbacks_t{
        [inst](const std::string_view /*option*/) { inst->found_entry(); },
        [inst](const char /*option*/) { inst->found_entry(); },
        [inst]() { return inst->given(); },
        [inst]() { return static_cast<bool>(*inst); },
        [inst]() { return inst->value(); }};
}

void clapp::option::bool_option_t::found_entry() {
    _given = true;
    _value = true;
    for (auto& found_func : _found) {
        found_func.found();
    }
}

clapp::option::bool_option_t::~bool_option_t() = default;

clapp::option::count_option_t::callbacks_t
clapp::option::count_option_t::create_callbacks(count_option_t* inst) {
    return callbacks_t{
        [inst](const std::string_view /*option*/) { inst->found_entry(); },
        [inst](const char /*option*/) { inst->found_entry(); },
        [inst]() { return inst->given(); },
        [inst]() { return static_cast<bool>(*inst); },
        [inst]() { return inst->value(); }};
}

void clapp::option::count_option_t::found_entry() {
    _given = true;
    _value = _value.value() + 1;
    for (auto& found_func : _found) {
        found_func.found();
    }
}

clapp::option::count_option_t::~count_option_t() = default;
