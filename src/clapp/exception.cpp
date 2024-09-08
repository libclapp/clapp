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

clapp::clapp_exception_t::clapp_exception_t(const char* message)
    : std::runtime_error(message) {}
clapp::clapp_exception_t::clapp_exception_t(const std::string& message)
    : std::runtime_error(message) {}
clapp::clapp_exception_t::clapp_exception_t(const clapp_exception_t&) = default;
clapp::clapp_exception_t& clapp::clapp_exception_t::operator=(
    const clapp_exception_t&) = default;
clapp::clapp_exception_t::clapp_exception_t(clapp_exception_t&&) noexcept =
    default;
clapp::clapp_exception_t& clapp::clapp_exception_t::operator=(
    clapp_exception_t&&) noexcept = default;
clapp::clapp_exception_t::~clapp_exception_t() noexcept = default;

clapp::invalid_value_t::invalid_value_t(const char* message)
    : clapp_exception_t(message) {}
clapp::invalid_value_t::invalid_value_t(const std::string& message)
    : clapp_exception_t(message) {}
clapp::invalid_value_t::invalid_value_t(const invalid_value_t&) = default;
clapp::invalid_value_t& clapp::invalid_value_t::operator=(
    const invalid_value_t&) = default;
clapp::invalid_value_t::invalid_value_t(invalid_value_t&&) noexcept = default;
clapp::invalid_value_t& clapp::invalid_value_t::operator=(
    invalid_value_t&&) noexcept = default;
clapp::invalid_value_t::~invalid_value_t() noexcept = default;

clapp::value_undefined_t::value_undefined_t(const char* message)
    : clapp_exception_t(message) {}
clapp::value_undefined_t::value_undefined_t(const std::string& message)
    : clapp_exception_t(message) {}
clapp::value_undefined_t::value_undefined_t(const value_undefined_t&) = default;
clapp::value_undefined_t& clapp::value_undefined_t::operator=(
    const value_undefined_t&) = default;
clapp::value_undefined_t::value_undefined_t(value_undefined_t&&) noexcept =
    default;
clapp::value_undefined_t& clapp::value_undefined_t::operator=(
    value_undefined_t&&) noexcept = default;
clapp::value_undefined_t::~value_undefined_t() noexcept = default;

clapp::out_of_range_t::out_of_range_t(const char* message)
    : invalid_value_t(message) {}
clapp::out_of_range_t::out_of_range_t(const std::string& message)
    : invalid_value_t(message) {}
clapp::out_of_range_t::out_of_range_t(const out_of_range_t&) = default;
clapp::out_of_range_t& clapp::out_of_range_t::operator=(const out_of_range_t&) =
    default;
clapp::out_of_range_t::out_of_range_t(out_of_range_t&&) noexcept = default;
clapp::out_of_range_t& clapp::out_of_range_t::operator=(
    out_of_range_t&&) noexcept = default;
clapp::out_of_range_t::~out_of_range_t() noexcept = default;

clapp::path_does_not_exist_t::path_does_not_exist_t(const char* message)
    : invalid_value_t(message) {}
clapp::path_does_not_exist_t::path_does_not_exist_t(const std::string& message)
    : invalid_value_t(message) {}
clapp::path_does_not_exist_t::path_does_not_exist_t(
    const path_does_not_exist_t&) = default;
clapp::path_does_not_exist_t& clapp::path_does_not_exist_t::operator=(
    const path_does_not_exist_t&) = default;
clapp::path_does_not_exist_t::path_does_not_exist_t(
    path_does_not_exist_t&&) noexcept = default;
clapp::path_does_not_exist_t& clapp::path_does_not_exist_t::operator=(
    path_does_not_exist_t&&) noexcept = default;
clapp::path_does_not_exist_t::~path_does_not_exist_t() noexcept = default;

clapp::option_exception_t::option_exception_t(const char* message)
    : clapp_exception_t(message) {}
clapp::option_exception_t::option_exception_t(const std::string& message)
    : clapp_exception_t(message) {}
clapp::option_exception_t::option_exception_t(const option_exception_t&) =
    default;
clapp::option_exception_t& clapp::option_exception_t::operator=(
    const option_exception_t&) = default;
clapp::option_exception_t::option_exception_t(option_exception_t&&) noexcept =
    default;
clapp::option_exception_t& clapp::option_exception_t::operator=(
    option_exception_t&&) noexcept = default;
clapp::option_exception_t::~option_exception_t() noexcept = default;

clapp::option_param_exception_t::option_param_exception_t(const char* message)
    : option_exception_t(message) {}
clapp::option_param_exception_t::option_param_exception_t(
    const std::string& message)
    : option_exception_t(message) {}
clapp::option_param_exception_t::option_param_exception_t(
    const option_param_exception_t&) = default;
clapp::option_param_exception_t& clapp::option_param_exception_t::operator=(
    const option_param_exception_t&) = default;
clapp::option_param_exception_t::option_param_exception_t(
    option_param_exception_t&&) noexcept = default;
clapp::option_param_exception_t& clapp::option_param_exception_t::operator=(
    option_param_exception_t&&) noexcept = default;
clapp::option_param_exception_t::~option_param_exception_t() noexcept = default;

clapp::argument_exception_t::argument_exception_t(const char* message)
    : clapp_exception_t(message) {}
clapp::argument_exception_t::argument_exception_t(const std::string& message)
    : clapp_exception_t(message) {}
clapp::argument_exception_t::argument_exception_t(const argument_exception_t&) =
    default;
clapp::argument_exception_t& clapp::argument_exception_t::operator=(
    const argument_exception_t&) = default;
clapp::argument_exception_t::argument_exception_t(
    argument_exception_t&&) noexcept = default;
clapp::argument_exception_t& clapp::argument_exception_t::operator=(
    argument_exception_t&&) noexcept = default;
clapp::argument_exception_t::~argument_exception_t() noexcept = default;

clapp::parser_exception_t::parser_exception_t(const char* message)
    : clapp_exception_t(message) {}
clapp::parser_exception_t::parser_exception_t(const std::string& message)
    : clapp_exception_t(message) {}
clapp::parser_exception_t::parser_exception_t(const parser_exception_t&) =
    default;
clapp::parser_exception_t& clapp::parser_exception_t::operator=(
    const parser_exception_t&) = default;
clapp::parser_exception_t::parser_exception_t(parser_exception_t&&) noexcept =
    default;
clapp::parser_exception_t& clapp::parser_exception_t::operator=(
    parser_exception_t&&) noexcept = default;
clapp::parser_exception_t::~parser_exception_t() noexcept = default;

clapp::sub_parser_exception_t::sub_parser_exception_t(const char* message)
    : parser_exception_t(message) {}
clapp::sub_parser_exception_t::sub_parser_exception_t(
    const std::string& message)
    : parser_exception_t(message) {}
clapp::sub_parser_exception_t::sub_parser_exception_t(
    const sub_parser_exception_t&) = default;
clapp::sub_parser_exception_t& clapp::sub_parser_exception_t::operator=(
    const sub_parser_exception_t&) = default;
clapp::sub_parser_exception_t::sub_parser_exception_t(
    sub_parser_exception_t&&) noexcept = default;
clapp::sub_parser_exception_t& clapp::sub_parser_exception_t::operator=(
    sub_parser_exception_t&&) noexcept = default;
clapp::sub_parser_exception_t::~sub_parser_exception_t() noexcept = default;

clapp::main_parser_exception_t::main_parser_exception_t(const char* message)
    : parser_exception_t(message) {}
clapp::main_parser_exception_t::main_parser_exception_t(
    const std::string& message)
    : parser_exception_t(message) {}
clapp::main_parser_exception_t::main_parser_exception_t(
    const main_parser_exception_t&) = default;
clapp::main_parser_exception_t& clapp::main_parser_exception_t::operator=(
    const main_parser_exception_t&) = default;
clapp::main_parser_exception_t::main_parser_exception_t(
    main_parser_exception_t&&) noexcept = default;
clapp::main_parser_exception_t& clapp::main_parser_exception_t::operator=(
    main_parser_exception_t&&) noexcept = default;
clapp::main_parser_exception_t::~main_parser_exception_t() noexcept = default;

clapp::no_executable_exception_t::no_executable_exception_t(const char* message)
    : main_parser_exception_t(message) {}
clapp::no_executable_exception_t::no_executable_exception_t(
    const std::string& message)
    : main_parser_exception_t(message) {}
clapp::no_executable_exception_t::no_executable_exception_t(
    const no_executable_exception_t&) = default;
clapp::no_executable_exception_t& clapp::no_executable_exception_t::operator=(
    const no_executable_exception_t&) = default;
clapp::no_executable_exception_t::no_executable_exception_t(
    no_executable_exception_t&&) noexcept = default;
clapp::no_executable_exception_t& clapp::no_executable_exception_t::operator=(
    no_executable_exception_t&&) noexcept = default;
clapp::no_executable_exception_t::~no_executable_exception_t() noexcept =
    default;

clapp::parser_container_exception_t::parser_container_exception_t(
    const char* message)
    : parser_exception_t(message) {}
clapp::parser_container_exception_t::parser_container_exception_t(
    const std::string& message)
    : parser_exception_t(message) {}
clapp::parser_container_exception_t::parser_container_exception_t(
    const parser_container_exception_t&) = default;
clapp::parser_container_exception_t&
clapp::parser_container_exception_t::operator=(
    const parser_container_exception_t&) = default;
clapp::parser_container_exception_t::parser_container_exception_t(
    parser_container_exception_t&&) noexcept = default;
clapp::parser_container_exception_t&
clapp::parser_container_exception_t::operator=(
    parser_container_exception_t&&) noexcept = default;
clapp::parser_container_exception_t::~parser_container_exception_t() noexcept =
    default;

clapp::not_fully_parsed_exception_t::not_fully_parsed_exception_t(
    const char* message)
    : parser_container_exception_t(message) {}
clapp::not_fully_parsed_exception_t::not_fully_parsed_exception_t(
    const std::string& message)
    : parser_container_exception_t(message) {}
clapp::not_fully_parsed_exception_t::not_fully_parsed_exception_t(
    const not_fully_parsed_exception_t&) = default;
clapp::not_fully_parsed_exception_t&
clapp::not_fully_parsed_exception_t::operator=(
    const not_fully_parsed_exception_t&) = default;
clapp::not_fully_parsed_exception_t::not_fully_parsed_exception_t(
    not_fully_parsed_exception_t&&) noexcept = default;
clapp::not_fully_parsed_exception_t&
clapp::not_fully_parsed_exception_t::operator=(
    not_fully_parsed_exception_t&&) noexcept = default;
clapp::not_fully_parsed_exception_t::~not_fully_parsed_exception_t() noexcept =
    default;

clapp::invalid_parser_state_exception_t::invalid_parser_state_exception_t(
    const char* message)
    : parser_container_exception_t(message) {}
clapp::invalid_parser_state_exception_t::invalid_parser_state_exception_t(
    const std::string& message)
    : parser_container_exception_t(message) {}
clapp::invalid_parser_state_exception_t::invalid_parser_state_exception_t(
    const invalid_parser_state_exception_t&) = default;
clapp::invalid_parser_state_exception_t&
clapp::invalid_parser_state_exception_t::operator=(
    const invalid_parser_state_exception_t&) = default;
clapp::invalid_parser_state_exception_t::invalid_parser_state_exception_t(
    invalid_parser_state_exception_t&&) noexcept = default;
clapp::invalid_parser_state_exception_t&
clapp::invalid_parser_state_exception_t::operator=(
    invalid_parser_state_exception_t&&) noexcept = default;
clapp::invalid_parser_state_exception_t::
    ~invalid_parser_state_exception_t() noexcept = default;
