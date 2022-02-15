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
        throw clapp::exception::option_exception_t{ss.str()};
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
            throw clapp::exception::option_exception_t{ss.str()};
        }
        default:
            return;
    }
}

clapp::option::bool_option_t::callbacks_t
clapp::option::bool_option_t::create_callbacks(bool_option_t* inst) {
    callbacks_t callbacks{
        [inst](const std::string_view option) {
            const clapp::value::found_func_t::ret_t ret{
                inst->found_entry("--" + std::string{option})};
            return ret;
        },
        [inst](const char option) {
            const clapp::value::found_func_t::ret_t ret{
                inst->found_entry("-" + std::string{option})};
            return ret;
        },
        [inst]() { return inst->given(); },
        [inst]() { return static_cast<bool>(*inst); },
        [inst]() { return inst->value(); }};
    return callbacks;
}

clapp::value::found_func_t::ret_t clapp::option::bool_option_t::found_entry(
    const std::string& option) {
    for (auto& found_func : _found) {
        const clapp::value::found_func_t::ret_t ret{found_func.found(option)};
        if (ret) {
            return ret;
        }
    }
    _given = true;
    _value = true;
    return {};
}

clapp::option::bool_option_t::~bool_option_t() = default;

clapp::option::count_option_t::callbacks_t
clapp::option::count_option_t::create_callbacks(count_option_t* inst) {
    callbacks_t callbacks{
        [inst](const std::string_view option) {
            const clapp::value::found_func_t::ret_t ret{
                inst->found_entry("--" + std::string{option})};
            return ret;
        },
        [inst](const char option) {
            const clapp::value::found_func_t::ret_t ret{
                inst->found_entry("-" + std::string{option})};
            return ret;
        },
        [inst]() { return inst->given(); },
        [inst]() { return static_cast<bool>(*inst); },
        [inst]() { return inst->value(); }};
    return callbacks;
}

clapp::value::found_func_t::ret_t clapp::option::count_option_t::found_entry(
    const std::string& option) {
    for (auto& found_func : _found) {
        const clapp::value::found_func_t::ret_t ret{found_func.found(option)};
        if (ret) {
            return ret;
        }
    }
    _given = true;
    _value = _value + 1;
    return {};
}

clapp::option::count_option_t::~count_option_t() = default;
