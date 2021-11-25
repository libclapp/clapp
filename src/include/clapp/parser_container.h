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

#ifndef CLAPP_PARSER_CONTAINER_H
#define CLAPP_PARSER_CONTAINER_H

#include <clapp/value.h>

#include <atomic>
#include <optional>

namespace clapp {

inline namespace parser {

template <typename PARSER_T>
class basic_parser_container_t {
   public:
    using parser_t = PARSER_T;
    enum class state_t { not_parsed, parsing, parsed };
    static constexpr std::optional<std::string_view> to_string_view(
        state_t state) noexcept;

    template <typename... ARGS_T>
    basic_parser_container_t(ARGS_T&&...);

    template <typename... ARGS_T>
    [[nodiscard]] std::optional<clapp::value::exit_t> parse(ARGS_T&&...);

    template <typename... ARGS_T>
    [[nodiscard]] std::optional<clapp::value::exit_t> parse_and_validate(
        ARGS_T&&...);

    [[nodiscard]] parser_t* operator->();
    [[nodiscard]] parser_t& operator*();

    void change_state(state_t exp_state, state_t new_state);
    state_t get_state() const;

   private:
    std::atomic<state_t> state{state_t::not_parsed};
    parser_t parser;
};

}  // namespace parser

}  // namespace clapp

#include <clapp/parser_container.hpp>

#endif
