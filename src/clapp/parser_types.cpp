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

#include <clapp/parser.h>
#include <clapp/sub_parser.h>
#include <unistd.h>

#include <iostream>
#include <sstream>
#include <utility>

bool clapp::parser::types::help_entry_t::operator==(
    const help_entry_t& inst) const {
    return name == inst.name && description == inst.description;
}

bool clapp::parser::types::help_entry_t::operator!=(
    const help_entry_t& inst) const {
    return !(*this == inst);
}

const clapp::parser::types::variant_opt_conf_t*
clapp::parser::types::variant_opt_conf_container_t::find_option(
    std::string_view long_option) const {
    for (types::variant_opt_conf_vec_t::const_iterator it{options.cbegin()};
         it != options.cend(); it++) {
        if (std::visit(
                [long_option](auto&& opt) {
                    return bool{opt.contains_option(long_option)};
                },
                *it)) {
            return &(*it);
        }
    }
    for (types::variant_opt_conf_container_ptr_vec_t::const_iterator it{
             containers.cbegin()};
         it != containers.cend(); it++) {
        Expects(*it != nullptr);
        const clapp::parser::types::variant_opt_conf_t* ret{
            (*it)->find_option(long_option)};
        if (ret != nullptr) {
            return ret;
        }
    }
    return nullptr;
}

const clapp::parser::types::variant_opt_conf_t*
clapp::parser::types::variant_opt_conf_container_t::find_option(
    char short_option) const {
    for (types::variant_opt_conf_vec_t::const_iterator it{options.cbegin()};
         it != options.end(); it++) {
        if (std::visit(
                [short_option](auto&& opt) {
                    return bool{opt.contains_option(short_option)};
                },
                *it)) {
            return &(*it);
        }
    }
    for (types::variant_opt_conf_container_ptr_vec_t::const_iterator it{
             containers.cbegin()};
         it != containers.cend(); it++) {
        Expects(*it != nullptr);
        const clapp::parser::types::variant_opt_conf_t* ret{
            (*it)->find_option(short_option)};
        if (ret != nullptr) {
            return ret;
        }
    }
    return nullptr;
}
