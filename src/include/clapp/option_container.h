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

    template <typename short_option_func_t, typename long_option_func_t,
              types::option_type_t option_type>
    void reg(types::basic_reg_option_conf_t<
             short_option_func_t, long_option_func_t, option_type>&& config);

    [[nodiscard]] virtual basic_parser_t& get_parser() = 0;

   protected:
    [[nodiscard]] const types::variant_opt_conf_t* find_option(
        std::string_view opt) const;
    [[nodiscard]] const types::variant_opt_conf_t* find_option(char opt) const;

    [[nodiscard]] types::validate_func_vec_t& get_validate_functions();
    [[nodiscard]] const types::validate_func_vec_t& get_validate_functions()
        const;

    [[nodiscard]] types::variant_opt_conf_container_t& get_options();
    [[nodiscard]] const types::variant_opt_conf_container_t& get_options()
        const;

    [[nodiscard]] std::string gen_short_option_line() const;
    [[nodiscard]] types::help_entry_vec_t get_option_help() const;

   private:
    types::validate_func_vec_t validate_functions{};
    types::variant_opt_conf_container_t options;
};

}  // namespace parser

}  // namespace clapp

#include <clapp/option_container.hpp>

#endif
