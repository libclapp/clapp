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

#ifndef LIBCLAPP_VALUE_H
#define LIBCLAPP_VALUE_H

#include <clapp/filesystem.h>
#include <string>
#include <string_view>

namespace clapp {

inline namespace value {
template <typename T>
T convert_value(const std::string_view param);

template <typename T>
struct default_value_t {
    constexpr default_value_t(T _value);

    std::string append_restriction_text() const;

    T default_value() const;

   private:
    T value;
};

template <typename T>
class min_max_value_t {
   public:
    constexpr min_max_value_t(T _min, T _max);

    std::string append_restriction_text() const;

    void validate(const T &value, const std::string &param_name) const;

   private:
    T min;
    T max;
};

#ifdef CLAPP_FS_AVAIL
class path_exists_t {
   public:
    std::string append_restriction_text();

    void validate(const fs::path &path, const std::string &param_name) const;
};
#endif

}  // namespace value

}  // namespace clapp

#include <clapp/value.hpp>

#endif
