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

#ifndef LIBCLAPP_TYPE_TRAITS_H
#define LIBCLAPP_TYPE_TRAITS_H

#include <type_traits>

#include <iostream>
#include <type_traits>

namespace clapp {

inline namespace type_traits {
template <typename T>
class has_append_restriction_text {
   private:
    template <typename C>
    constexpr static bool test(decltype(&C::append_restriction_text)) {
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
    constexpr static bool test(decltype(&C::validate)) {
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
    constexpr static bool test(decltype(&C::default_value)) {
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
