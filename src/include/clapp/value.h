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

#ifndef CLAPP_VALUE_H
#define CLAPP_VALUE_H

#include <clapp/filesystem.h>

#include <functional>
#include <optional>
#include <string>
#include <string_view>

namespace clapp {

inline namespace value {
template <typename T>
T convert_value(std::string_view param);

template <typename T>
constexpr const char *get_chrono_postfix() noexcept;

template <typename T>
std::string to_string(const T &value);

template <typename T>
struct default_value_t {
    explicit default_value_t(T _value);

    std::string append_restriction_text() const;

    T default_value() const;

   private:
    T value;
};

template <typename T>
class min_max_value_t {
   public:
    min_max_value_t(T _min, T _max);

    std::string append_restriction_text() const;

    void validate(const T &value, const std::string &param_name) const;

   private:
    T min;
    T max;
};

template <typename T>
class not_null_value_t {
   public:
    not_null_value_t();

    static constexpr std::string_view append_restriction_text() noexcept;

    void validate(const T &value, const std::string &param_name) const;
};

class exit_t {
   public:
    [[nodiscard]] static constexpr exit_t exit(int exit_code) noexcept;

    [[nodiscard]] constexpr int get_exit_code() const noexcept;

   private:
    constexpr exit_t(int exit_code_param) noexcept;

    int exit_code;
};

class found_func_t {
   public:
    using ret_t = std::optional<exit_t>;
    using func_t = std::function<ret_t(const std::string &)>;
    explicit found_func_t(func_t &&func_arg);

    [[nodiscard]] ret_t found(const std::string &name);

   private:
    func_t func;
};

#ifdef CLAPP_FS_AVAIL
class path_exists_t {
   public:
    static constexpr std::string_view append_restriction_text() noexcept;

    static void validate(const fs::path &path, const std::string &param_name);
};
#endif

}  // namespace value

}  // namespace clapp

#include <clapp/value.hpp>

#endif
