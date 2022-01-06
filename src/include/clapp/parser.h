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

#ifndef CLAPP_PARSER_H
#define CLAPP_PARSER_H

#include <clapp/option_container.h>
#include <clapp/parser_types.h>
#include <clapp/value.h>

#include <gsl/gsl_assert>
#include <gsl/span>
#include <map>
#include <optional>
#include <string>
#include <string_view>
#include <variant>

namespace clapp {

inline namespace parser {

class basic_sub_parser_t;

class basic_parser_t : public basic_option_container_t {
   public:
    using purpose_t = types::purpose_t;

    struct sub_parser_line_t {
        std::string name;
        std::string description;
        basic_sub_parser_t& parser;
    };

    struct help_contents_t {
        types::help_entry_vec_t arguments{};
        types::help_entry_vec_t options{};
        std::map<std::string, sub_parser_line_t> sub_parser{};
        std::size_t max_name_size{0};
    };

    struct reg_sub_parser_conf_t {
        basic_sub_parser_t& parser;
        std::string sub_parser_name;
        std::string description;
    };

   protected:
    using sub_parsers_map_t =
        std::map<std::string, basic_sub_parser_t&, std::less<>>;

    struct option_description_container_t {
        std::string option_string;
        std::string description;
        types::option_type_t option_type;
    };

    struct argument_description_container_t {
        std::string argument_string;
        std::string description;
        types::argument_type_t argument_type;
    };

    struct sub_parser_description_container_t {
        std::string sub_parser_string;
        std::string description;
        basic_sub_parser_t& parser;
    };

    using option_descriptions_vec_t =
        std::vector<option_description_container_t>;
    using argument_descriptions_vec_t =
        std::vector<argument_description_container_t>;
    using sub_parser_descriptions_vec_t =
        std::vector<sub_parser_description_container_t>;

   public:
    basic_parser_t();
    explicit basic_parser_t(const basic_parser_t&) = delete;
    explicit basic_parser_t(basic_parser_t&&) noexcept = delete;
    basic_parser_t& operator=(const basic_parser_t&) = delete;
    basic_parser_t& operator=(basic_parser_t&&) noexcept = delete;

    ~basic_parser_t() override;

    using basic_option_container_t::reg;

    template <types::argument_type_t argument_type>
    void reg(types::basic_reg_argument_conf_t<argument_type>&& config);
    void reg(reg_sub_parser_conf_t&& config);

    struct parse_result_t {
        types::arg_iterator_t it;
        std::optional<char> short_option;
        std::optional<std::string> long_option;
        std::optional<clapp::value::exit_t> exit;
    };

    [[nodiscard]] static types::arg_iterator_t process_parse_result(
        types::arg_iterator_t ait, const parse_result_t& parse_result);
    [[nodiscard]] std::optional<clapp::value::exit_t> parse(
        types::arg_iterator_t begin, types::arg_iterator_t end);
    [[nodiscard]] parse_result_t parse(std::string_view option,
                                       types::arg_iterator_t ait,
                                       types::arg_iterator_t end);

    void validate() const;

    void validate_recursive() const;

    [[nodiscard]] std::string gen_short_line() const;
    [[nodiscard]] std::string gen_short_lines(std::size_t rec_depth) const;
    [[nodiscard]] virtual std::string gen_short_line_prefix() const = 0;
    [[nodiscard]] static constexpr std::string_view gen_usage_prefix() noexcept;
    [[nodiscard]] virtual help_contents_t gen_detailed_help_contents() const;
    [[nodiscard]] std::string gen_opt_arg_lines(
        const help_contents_t& help_contents, std::size_t num_spaces) const;

    [[nodiscard]] std::string gen_help_desc(std::size_t num_spaces,
                                            std::size_t rec_depth) const;
    [[nodiscard]] std::string gen_help_msg(std::size_t rec_depth) const;
    [[nodiscard]] value::found_func_t gen_func_print_help_and_req_exit(
        int exit_code) const;

    [[nodiscard]] std::size_t get_num_processed_arguments() const;

    [[nodiscard]] inline virtual bool is_active() const noexcept;
    [[nodiscard]] const basic_parser_t& get_active_parser() const;

    [[nodiscard]] static exit_t default_print_and_exit(
        std::string_view print_msg, int exit_code);
    void set_print_and_exit_func(types::print_and_exit_func_t&& func);
    [[nodiscard]] types::print_and_exit_func_t& get_print_and_exit_func();

    [[nodiscard]] basic_parser_t& get_parser() override;

   protected:
    [[nodiscard]] sub_parsers_map_t& get_sub_parsers();
    [[nodiscard]] types::help_entry_vec_t get_argument_help() const;
    [[nodiscard]] types::variant_arg_conf_vec_t& get_arguments();
    [[nodiscard]] const types::variant_arg_conf_vec_t& get_arguments() const;
    [[nodiscard]] sub_parser_descriptions_vec_t& get_sub_parser_descriptions();
    [[nodiscard]] argument_descriptions_vec_t&
    get_mandatory_argument_descriptions();
    [[nodiscard]] argument_descriptions_vec_t&
    get_optional_argument_descriptions();

   private:
    [[nodiscard]] parse_result_t parse_arg(std::string_view argument,
                                           types::arg_iterator_t ait,
                                           types::arg_iterator_t end);
    [[nodiscard]] parse_result_t parse_long(std::string_view option,
                                            types::arg_iterator_t ait,
                                            types::arg_iterator_t end);
    [[nodiscard]] parse_result_t parse_short(std::string_view option,
                                             types::arg_iterator_t ait,
                                             types::arg_iterator_t end);

    sub_parsers_map_t sub_parsers{};
    sub_parser_descriptions_vec_t sub_parser_descriptions{};
    argument_descriptions_vec_t mandatory_argument_descriptions{};
    argument_descriptions_vec_t optional_argument_descriptions{};
    std::size_t num_processed_arguments{0};
    types::print_and_exit_func_t print_and_exit_func{default_print_and_exit};

    types::variant_arg_conf_vec_t arguments{};

   public:
    constexpr static std::size_t num_sub_spaces{2U};
};

}  // namespace parser

}  // namespace clapp

#include <clapp/parser.hpp>

#endif
