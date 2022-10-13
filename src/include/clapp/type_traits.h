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

#ifndef CLAPP_TYPE_TRAITS_H
#define CLAPP_TYPE_TRAITS_H

#include <chrono>
#include <iostream>
#include <type_traits>
#include <vector>

namespace clapp {

inline namespace type_traits {
template <typename T>
struct is_chrono_duration : std::false_type {};

template <typename Rep, typename Period>
struct is_chrono_duration<std::chrono::duration<Rep, Period>> : std::true_type {
};

template <typename T>
struct is_vector : std::false_type {};

template <typename T, typename A>
struct is_vector<std::vector<T, A>> : std::true_type {};

template <typename T, typename = void>
struct has_append_restriction_text : std::false_type {};

template <typename T>
struct has_append_restriction_text<
    T, decltype(void(std::declval<T &>().append_restriction_text()))>
    : std::true_type {};

template <typename T, typename V, typename = void>
struct has_validate : std::false_type {};

template <typename T, typename V>
struct has_validate<
    T, V, decltype(void(std::declval<T &>().validate(V{}, std::string{})))>
    : std::true_type {};

template <typename T, typename = void>
struct has_default_value : std::false_type {};

template <typename T>
struct has_default_value<T, decltype(void(std::declval<T &>().default_value()))>
    : std::true_type {};

template <typename T, typename = void>
struct has_found : std::false_type {};

template <typename T>
struct has_found<T, decltype(void(std::declval<T &>().found(std::string{})))>
    : std::true_type {};

}  // namespace type_traits

}  // namespace clapp

#endif
