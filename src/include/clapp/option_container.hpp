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

#ifndef CLAPP_OPTION_CONTAINER_HPP
#define CLAPP_OPTION_CONTAINER_HPP

#include <clapp/option_container.h>

template <typename short_option_func_t, typename long_option_func_t,
          clapp::parser::types::option_type_t option_type>
void clapp::parser::basic_option_container_t::reg(
    types::basic_reg_option_conf_t<short_option_func_t, long_option_func_t,
                                   option_type>&& config) {
    for (auto it{std::begin(config.short_options)};
         it != std::end(config.short_options); it++) {
        if (find_option(it->option) != nullptr) {
            std::stringstream string_stream;
            string_stream << "can't register option '" << it->option
                          << "', as it is already registered.";
            throw clapp::exception::option_exception_t{string_stream.str()};
        }
        auto next_it{
            std::find_if(std::next(it), std::end(config.short_options),
                         [short_option = it->option](auto&& short_opt_conf) {
                             return short_opt_conf.option == short_option;
                         })};
        if (next_it != std::end(config.short_options)) {
            std::stringstream string_stream;
            string_stream << "can't register option '" << it->option
                          << "', as it is contained twice in the same config.";
            throw clapp::exception::option_exception_t{string_stream.str()};
        }
    }

    for (auto it{std::begin(config.long_options)};
         it != std::end(config.long_options); it++) {
        if (find_option(it->option) != nullptr) {
            std::stringstream string_stream;
            string_stream << "can't register option '" << it->option
                          << "', as it is already registered.";
            throw clapp::exception::option_exception_t{string_stream.str()};
        }
        auto next_it{
            std::find_if(std::next(it), std::end(config.long_options),
                         [long_option = it->option](auto&& long_opt_conf) {
                             return long_opt_conf.option == long_option;
                         })};
        if (next_it != std::end(config.long_options)) {
            std::stringstream string_stream;
            string_stream << "can't register option '" << it->option
                          << "', as it is contained twice in the same config.";
            throw clapp::exception::option_exception_t{string_stream.str()};
        }
    }

    if (config.validate_func) {
        get_validate_functions().push_back(config.validate_func.value());
    }

    options.options.push_back(std::move(config));
}

#endif
