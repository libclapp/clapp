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

template <typename T>
class has_append_restriction_text {
   private:
    template <typename C>
    constexpr static bool test(
        [[maybe_unused]] decltype(&C::append_restriction_text) func) {
        return true;
    }
    template <typename C>
    constexpr static bool test(...) {
        return false;
    }

   public:
    constexpr static bool value{test<T>(nullptr)};
};

template <typename T>
class has_validate {
   private:
    template <typename C>
    constexpr static bool test([[maybe_unused]] decltype(&C::validate) func) {
        return true;
    }
    template <typename C>
    constexpr static bool test(...) {
        return false;
    }

   public:
    constexpr static bool value{test<T>(nullptr)};
};

template <typename T>
class has_default_value {
   private:
    template <typename C>
    constexpr static bool test(
        [[maybe_unused]] decltype(&C::default_value) func) {
        return true;
    }
    template <typename C>
    constexpr static bool test(...) {
        return false;
    }

   public:
    constexpr static bool value{test<T>(nullptr)};
};

template <typename T>
class has_found {
   private:
    template <typename C>
    constexpr static bool test([[maybe_unused]] decltype(&C::found) func) {
        return true;
    }
    template <typename C>
    constexpr static bool test(...) {
        return false;
    }

   public:
    constexpr static bool value{test<T>(nullptr)};
};

}  // namespace type_traits

}  // namespace clapp

#endif
