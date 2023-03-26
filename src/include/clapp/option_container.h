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

#ifndef CLAPP_OPTION_CONTAINER_H
#define CLAPP_OPTION_CONTAINER_H

#include <clapp/parser_types.h>

namespace clapp {

inline namespace parser {

class basic_parser_t;

class basic_option_container_t {
   public:
    explicit basic_option_container_t(
        types::logic_operator_type_t logic_operator_type);
    virtual ~basic_option_container_t();

    basic_option_container_t(const basic_option_container_t& inst);
    basic_option_container_t(basic_option_container_t&& inst) noexcept;

    basic_option_container_t& operator=(const basic_option_container_t& inst);
    basic_option_container_t& operator=(
        basic_option_container_t&& inst) noexcept;

    template <typename short_option_func_t, typename long_option_func_t,
              types::option_type_t option_type>
    void reg(types::basic_reg_option_conf_t<
             short_option_func_t, long_option_func_t, option_type>&& config);

    void reg(types::variant_opt_conf_container_t* opt_container);

    [[nodiscard]] virtual basic_parser_t& get_parser() = 0;
    [[nodiscard]] virtual const basic_parser_t& get_const_parser() const = 0;

   protected:
    [[nodiscard]] const types::variant_opt_conf_t* find_option(
        std::string_view opt) const;
    [[nodiscard]] const types::variant_opt_conf_t* find_option(char opt) const;

    [[nodiscard]] const types::variant_opt_conf_t* find_parser_option(
        std::string_view opt) const;
    [[nodiscard]] const types::variant_opt_conf_t* find_parser_option(
        char opt) const;

    [[nodiscard]] types::validate_func_vec_t& get_validate_functions();
    [[nodiscard]] const types::validate_func_vec_t& get_validate_functions()
        const;

    [[nodiscard]] types::variant_opt_conf_container_t& get_options();
    [[nodiscard]] const types::variant_opt_conf_container_t& get_options()
        const;

    void validate_options() const;

    static void validate_options_xor(
        const types::variant_opt_conf_container_t* options,
        const std::string& options_str, const std::string& xor_options_str,
        std::optional<std::vector<std::string>>& mandatory_and_options,
        std::optional<std::string>& given_xor_option,
        std::optional<std::vector<std::string>>& given_and_options,
        bool summarize_xor_options);

    static void validate_options_xor_options(
        const types::variant_opt_conf_container_t* options,
        const std::string& xor_options_str,
        std::optional<std::string>& given_xor_option);

    static void validate_options_xor_container(
        const types::variant_opt_conf_container_t* options,
        const std::string& options_str,
        std::optional<std::vector<std::string>>& mandatory_and_options,
        std::optional<std::string>& given_xor_option,
        std::optional<std::vector<std::string>>& given_and_options);

    static void validate_options_and(
        const types::variant_opt_conf_container_t* options,
        const std::string& options_str, const std::string& xor_options_str,
        std::optional<std::vector<std::string>>& mandatory_and_options,
        std::optional<std::string>& given_xor_option,
        std::optional<std::vector<std::string>>& given_and_options,
        bool allow_empty);

    static void validate_options_and_options(
        const types::variant_opt_conf_container_t* options,
        const std::string& xor_options_str,
        std::optional<std::string>& given_xor_option,
        std::optional<std::vector<std::string>>& given_and_options);

    static void validate_options_and_container(
        const types::variant_opt_conf_container_t* options,
        const std::string& options_str, const std::string& xor_options_str,
        std::optional<std::vector<std::string>>& mandatory_and_options,
        std::optional<std::string>& given_xor_option,
        std::optional<std::vector<std::string>>& given_and_options,
        bool allow_empty);

    static void append_mandatory_and_options(
        const types::variant_opt_conf_container_t* options,
        std::vector<std::string>& mandatory_and_options);

    [[nodiscard]] std::string gen_short_option_line() const;
    [[nodiscard]] types::help_entry_vec_t get_option_help() const;

   private:
    types::validate_func_vec_t validate_functions{};
    types::variant_opt_conf_container_t options;
};

class option_container_t : public basic_option_container_t {
   public:
    option_container_t(basic_option_container_t& container,
                       types::logic_operator_type_t logic_operator_type);
    ~option_container_t() override;

    [[nodiscard]] basic_parser_t& get_parser() override;
    [[nodiscard]] const basic_parser_t& get_const_parser() const override;

   private:
    basic_option_container_t& cont;  // TODO(martinw): may use shared ptr.
};

}  // namespace parser

}  // namespace clapp

#include <clapp/option_container.hpp>

#endif
