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

#ifndef CLAPP_PARSER_CONTAINER_HPP
#define CLAPP_PARSER_CONTAINER_HPP

#include <clapp/parser_container.h>

template <typename PARSER_T>
template <typename... ARGS_T>
clapp::parser::basic_parser_container_t<PARSER_T>::basic_parser_container_t(
    ARGS_T&&... args)
    : parser{std::forward<ARGS_T>(args)...} {}

template <typename PARSER_T>
constexpr std::optional<std::string_view>
clapp::parser::basic_parser_container_t<PARSER_T>::to_string_view(
    const state_t state) noexcept {
    switch (state) {
        case state_t::not_parsed:
            return "not_parsed";
        case state_t::parsing:
            return "parsing";
        case state_t::parsed:
            return "parsed";
    }
    return {};
}

template <typename PARSER_T>
typename clapp::parser::basic_parser_container_t<PARSER_T>::state_t
clapp::parser::basic_parser_container_t<PARSER_T>::get_state() const {
    return state;
}

template <typename PARSER_T>
void clapp::parser::basic_parser_container_t<PARSER_T>::change_state(
    const state_t exp_state, const state_t new_state) {
    state_t tmp{exp_state};
    if (!state.compare_exchange_strong(tmp, new_state)) {
        throw invalid_parser_state_exception_t{
            "Could not switch to state " +
            std::string{to_string_view(exp_state).value()} +
            ", as the current state is " +
            std::string{to_string_view(tmp).value()} + " and not " +
            std::string{to_string_view(exp_state).value()} +
            " which was expected."};
    }
}

template <typename PARSER_T>
template <typename... ARGS_T>
std::optional<clapp::value::exit_t>
clapp::parser::basic_parser_container_t<PARSER_T>::parse(ARGS_T&&... args) {
    change_state(state_t::not_parsed, state_t::parsing);
    const std::optional<clapp::value::exit_t> ret{
        parser.parse(std::forward<ARGS_T>(args)...)};
    if (ret.has_value()) {
        change_state(state_t::parsing, state_t::not_parsed);
    } else {
        change_state(state_t::parsing, state_t::parsed);
    }
    return ret;
}

template <typename PARSER_T>
template <typename... ARGS_T>
std::optional<clapp::value::exit_t>
clapp::parser::basic_parser_container_t<PARSER_T>::parse_and_validate(
    ARGS_T&&... args) {
    change_state(state_t::not_parsed, state_t::parsing);
    const std::optional<clapp::value::exit_t> ret{
        parser.parse_and_validate(std::forward<ARGS_T>(args)...)};
    if (ret.has_value()) {
        change_state(state_t::parsing, state_t::not_parsed);
    } else {
        change_state(state_t::parsing, state_t::parsed);
    }
    return ret;
}

template <typename PARSER_T>
PARSER_T* clapp::parser::basic_parser_container_t<PARSER_T>::operator->() {
    if (state != state_t::parsed) {
        throw not_fully_parsed_exception_t{
            "Either parse() was not executed or parse() aborted with an exit "
            "request."};
    }
    return &parser;
}

template <typename PARSER_T>
PARSER_T& clapp::parser::basic_parser_container_t<PARSER_T>::operator*() {
    if (state != state_t::parsed) {
        throw not_fully_parsed_exception_t{
            "Either parse() was not executed or parse() aborted with an exit "
            "request."};
    }
    return parser;
}

#endif
