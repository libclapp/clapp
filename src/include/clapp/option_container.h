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

class basic_option_container_t {
   public:
    basic_option_container_t();
    virtual ~basic_option_container_t();

    template <typename short_option_func_t, typename long_option_func_t,
              types::option_type_t option_type>
    void reg(types::basic_reg_option_conf_t<
             short_option_func_t, long_option_func_t, option_type>&& config);

   protected:
    [[nodiscard]] const types::variant_opt_conf_t* find_option(
        std::string_view opt) const;
    [[nodiscard]] const types::variant_opt_conf_t* find_option(char opt) const;

    [[nodiscard]] types::validate_func_vec_t&
    get_validate_functions();  // todo: move to validate_func_container
    [[nodiscard]] const types::validate_func_vec_t& get_validate_functions()
        const;

    [[nodiscard]] types::variant_opt_conf_vec_t&
    get_options();  // todo: move to option_container
    [[nodiscard]] const types::variant_opt_conf_vec_t& get_options() const;

   private:
    types::validate_func_vec_t validate_functions{};
    types::variant_opt_conf_vec_t options{};
};

}  // namespace parser

}  // namespace clapp

#include <clapp/option_container.hpp>

#endif
