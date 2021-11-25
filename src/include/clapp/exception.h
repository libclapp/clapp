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

#ifndef CLAPP_EXCEPTION_H
#define CLAPP_EXCEPTION_H

#include <stdexcept>

namespace clapp {
inline namespace exception {
class clapp_exception_t : public std::runtime_error {
   public:
    explicit clapp_exception_t(const char* message);
    explicit clapp_exception_t(const std::string& message);
    clapp_exception_t(const clapp_exception_t&);
    clapp_exception_t& operator=(const clapp_exception_t&);
    clapp_exception_t(clapp_exception_t&&) noexcept;
    clapp_exception_t& operator=(clapp_exception_t&&) noexcept;
    ~clapp_exception_t() noexcept override;
};

class invalid_value_t : public clapp_exception_t {
   public:
    explicit invalid_value_t(const char* message);
    explicit invalid_value_t(const std::string& message);
    invalid_value_t(const invalid_value_t&);
    invalid_value_t& operator=(const invalid_value_t&);
    invalid_value_t(invalid_value_t&&) noexcept;
    invalid_value_t& operator=(invalid_value_t&&) noexcept;
    ~invalid_value_t() noexcept override;
};

class value_undefined_t : public clapp_exception_t {
   public:
    explicit value_undefined_t(const char* message);
    explicit value_undefined_t(const std::string& message);
    value_undefined_t(const value_undefined_t&);
    value_undefined_t& operator=(const value_undefined_t&);
    value_undefined_t(value_undefined_t&&) noexcept;
    value_undefined_t& operator=(value_undefined_t&&) noexcept;
    ~value_undefined_t() noexcept override;
};

class out_of_range_t : public invalid_value_t {
   public:
    explicit out_of_range_t(const char* message);
    explicit out_of_range_t(const std::string& message);
    out_of_range_t(const out_of_range_t&);
    out_of_range_t& operator=(const out_of_range_t&);
    out_of_range_t(out_of_range_t&&) noexcept;
    out_of_range_t& operator=(out_of_range_t&&) noexcept;
    ~out_of_range_t() noexcept override;
};

class path_does_not_exist_t : public invalid_value_t {
   public:
    explicit path_does_not_exist_t(const char* message);
    explicit path_does_not_exist_t(const std::string& message);
    path_does_not_exist_t(const path_does_not_exist_t&);
    path_does_not_exist_t& operator=(const path_does_not_exist_t&);
    path_does_not_exist_t(path_does_not_exist_t&&) noexcept;
    path_does_not_exist_t& operator=(path_does_not_exist_t&&) noexcept;
    ~path_does_not_exist_t() noexcept override;
};

class option_exception_t : public clapp_exception_t {
   public:
    explicit option_exception_t(const char* message);
    explicit option_exception_t(const std::string& message);
    option_exception_t(const option_exception_t&);
    option_exception_t& operator=(const option_exception_t&);
    option_exception_t(option_exception_t&&) noexcept;
    option_exception_t& operator=(option_exception_t&&) noexcept;
    ~option_exception_t() noexcept override;
};

class option_param_exception_t : public option_exception_t {
   public:
    explicit option_param_exception_t(const char* message);
    explicit option_param_exception_t(const std::string& message);
    option_param_exception_t(const option_param_exception_t&);
    option_param_exception_t& operator=(const option_param_exception_t&);
    option_param_exception_t(option_param_exception_t&&) noexcept;
    option_param_exception_t& operator=(option_param_exception_t&&) noexcept;
    ~option_param_exception_t() noexcept override;
};

class argument_exception_t : public clapp_exception_t {
   public:
    explicit argument_exception_t(const char* message);
    explicit argument_exception_t(const std::string& message);
    argument_exception_t(const argument_exception_t&);
    argument_exception_t& operator=(const argument_exception_t&);
    argument_exception_t(argument_exception_t&&) noexcept;
    argument_exception_t& operator=(argument_exception_t&&) noexcept;
    ~argument_exception_t() noexcept override;
};

class parser_exception_t : public clapp_exception_t {
   public:
    explicit parser_exception_t(const char* message);
    explicit parser_exception_t(const std::string& message);
    parser_exception_t(const parser_exception_t&);
    parser_exception_t& operator=(const parser_exception_t&);
    parser_exception_t(parser_exception_t&&) noexcept;
    parser_exception_t& operator=(parser_exception_t&&) noexcept;
    ~parser_exception_t() noexcept override;
};

class sub_parser_exception_t : public parser_exception_t {
   public:
    explicit sub_parser_exception_t(const char* message);
    explicit sub_parser_exception_t(const std::string& message);
    sub_parser_exception_t(const sub_parser_exception_t&);
    sub_parser_exception_t& operator=(const sub_parser_exception_t&);
    sub_parser_exception_t(sub_parser_exception_t&&) noexcept;
    sub_parser_exception_t& operator=(sub_parser_exception_t&&) noexcept;
    ~sub_parser_exception_t() noexcept override;
};

class main_parser_exception_t : public parser_exception_t {
   public:
    explicit main_parser_exception_t(const char* message);
    explicit main_parser_exception_t(const std::string& message);
    main_parser_exception_t(const main_parser_exception_t&);
    main_parser_exception_t& operator=(const main_parser_exception_t&);
    main_parser_exception_t(main_parser_exception_t&&) noexcept;
    main_parser_exception_t& operator=(main_parser_exception_t&&) noexcept;
    ~main_parser_exception_t() noexcept override;
};

class no_executable_exception_t : public main_parser_exception_t {
   public:
    explicit no_executable_exception_t(const char* message);
    explicit no_executable_exception_t(const std::string& message);
    no_executable_exception_t(const no_executable_exception_t&);
    no_executable_exception_t& operator=(const no_executable_exception_t&);
    no_executable_exception_t(no_executable_exception_t&&) noexcept;
    no_executable_exception_t& operator=(no_executable_exception_t&&) noexcept;
    ~no_executable_exception_t() noexcept override;
};

class parser_container_exception_t : public parser_exception_t {
   public:
    explicit parser_container_exception_t(const char* message);
    explicit parser_container_exception_t(const std::string& message);
    parser_container_exception_t(const parser_container_exception_t&);
    parser_container_exception_t& operator=(
        const parser_container_exception_t&);
    parser_container_exception_t(parser_container_exception_t&&) noexcept;
    parser_container_exception_t& operator=(
        parser_container_exception_t&&) noexcept;
    ~parser_container_exception_t() noexcept override;
};

class not_fully_parsed_exception_t : public parser_container_exception_t {
   public:
    explicit not_fully_parsed_exception_t(const char* message);
    explicit not_fully_parsed_exception_t(const std::string& message);
    not_fully_parsed_exception_t(const not_fully_parsed_exception_t&);
    not_fully_parsed_exception_t& operator=(
        const not_fully_parsed_exception_t&);
    not_fully_parsed_exception_t(not_fully_parsed_exception_t&&) noexcept;
    not_fully_parsed_exception_t& operator=(
        not_fully_parsed_exception_t&&) noexcept;
    ~not_fully_parsed_exception_t() noexcept override;
};

class invalid_parser_state_exception_t : public parser_container_exception_t {
   public:
    explicit invalid_parser_state_exception_t(const char* message);
    explicit invalid_parser_state_exception_t(const std::string& message);
    invalid_parser_state_exception_t(const invalid_parser_state_exception_t&);
    invalid_parser_state_exception_t& operator=(
        const invalid_parser_state_exception_t&);
    invalid_parser_state_exception_t(
        invalid_parser_state_exception_t&&) noexcept;
    invalid_parser_state_exception_t& operator=(
        invalid_parser_state_exception_t&&) noexcept;
    ~invalid_parser_state_exception_t() noexcept override;
};

}  // namespace exception
}  // namespace clapp

#endif
