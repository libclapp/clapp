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

#ifndef CLAPP_VALUE_HPP
#define CLAPP_VALUE_HPP

#include <clapp/exception.h>
#include <clapp/type_traits.h>
#include <clapp/value.h>

#include <gsl/gsl_assert>
#include <sstream>

template <typename T>
clapp::value::default_value_t<T>::default_value_t(T _value) : value{_value} {}

template <typename T>
constexpr const char* clapp::value::get_chrono_postfix() noexcept {
    const char* postfix{""};
    if constexpr (std::is_same<T, std::chrono::nanoseconds>::value) {
        postfix = "ns";
    } else if constexpr (std::is_same<T, std::chrono::microseconds>::value) {
        postfix = "µs";
    } else if constexpr (std::is_same<T, std::chrono::milliseconds>::value) {
        postfix = "ms";
    } else if constexpr (std::is_same<T, std::chrono::seconds>::value) {
        postfix = "s";
    } else if constexpr (std::is_same<T, std::chrono::minutes>::value) {
        postfix = "m";
    } else if constexpr (std::is_same<T, std::chrono::hours>::value) {
        postfix = "h";
    }
    return postfix;
}

template <typename T>
std::string clapp::value::to_string(const T& value) {
    std::stringstream ss;
    if constexpr (std::is_integral<T>::value) {
        ss << std::to_string(value);
    } else if constexpr (is_chrono_duration<T>::value) {
        ss << value.count() << get_chrono_postfix<T>();
    } else {
        ss << value;
    }
    return ss.str();
}

template <typename T>
std::string clapp::value::default_value_t<T>::append_restriction_text() const {
    const std::string val_str{to_string(value)};
    return "default-value: " + val_str;
}

template <typename T>
T clapp::value::default_value_t<T>::default_value() const {
    return value;
}

template <typename T>
clapp::value::min_max_value_t<T>::min_max_value_t(T _min, T _max)
    : min{_min}, max{_max} {
    Expects(min < max);
}

template <typename T>
std::string clapp::value::min_max_value_t<T>::append_restriction_text() const {
    const std::string min_str{to_string(min)};
    const std::string max_str{to_string(max)};
    return "constraint: [" + min_str + "," + max_str + "]";
}

template <typename T>
void clapp::value::min_max_value_t<T>::validate(
    const T& value, const std::string& param_name) const {
    if (value < min) {
        throw clapp::exception::out_of_range_t{
            "CLI value for '" + param_name + "' is too low (" +
            to_string(value) + " < " + to_string(min) + ")"};
    }
    if (value > max) {
        throw clapp::exception::out_of_range_t{
            "CLI value for '" + param_name + "' is too high (" +
            to_string(value) + " > " + to_string(max) + ")"};
    }
}

template <typename T>
clapp::value::not_null_value_t<T>::not_null_value_t() = default;

template <typename T>
constexpr std::string_view
clapp::value::not_null_value_t<T>::append_restriction_text() noexcept {
    return "constraint: not null";
}

template <typename T>
void clapp::value::not_null_value_t<T>::validate(
    const T& value, const std::string& param_name) const {
    if (value == T{0}) {
        throw clapp::exception::out_of_range_t{"CLI value for '" + param_name +
                                               "' must not be null (" +
                                               to_string(value) + ")"};
    }
}

#ifdef CLAPP_FS_AVAIL
constexpr std::string_view
clapp::path_exists_t::append_restriction_text() noexcept {
    return "existing path";
}
#endif

template <>
std::string clapp::value::convert_value<std::string>(std::string_view param);

template <>
bool clapp::value::convert_value<bool>(std::string_view param);

template <>
std::int8_t clapp::value::convert_value<std::int8_t>(std::string_view param);

template <>
std::int16_t clapp::value::convert_value<std::int16_t>(std::string_view param);

template <>
std::int32_t clapp::value::convert_value<std::int32_t>(std::string_view param);

template <>
std::int64_t clapp::value::convert_value<std::int64_t>(std::string_view param);

template <>
std::uint8_t clapp::value::convert_value<std::uint8_t>(std::string_view param);

template <>
std::uint16_t clapp::value::convert_value<std::uint16_t>(
    std::string_view param);

template <>
std::uint32_t clapp::value::convert_value<std::uint32_t>(
    std::string_view param);

template <>
std::uint64_t clapp::value::convert_value<std::uint64_t>(
    std::string_view param);

template <>
std::chrono::nanoseconds clapp::value::convert_value<std::chrono::nanoseconds>(
    std::string_view param);

template <>
std::chrono::microseconds
clapp::value::convert_value<std::chrono::microseconds>(std::string_view param);

template <>
std::chrono::milliseconds
clapp::value::convert_value<std::chrono::milliseconds>(std::string_view param);

template <>
std::chrono::seconds clapp::value::convert_value<std::chrono::seconds>(
    std::string_view param);

template <>
std::chrono::minutes clapp::value::convert_value<std::chrono::minutes>(
    std::string_view param);

template <>
std::chrono::hours clapp::value::convert_value<std::chrono::hours>(
    std::string_view param);

template <>
float clapp::value::convert_value<float>(std::string_view param);

template <>
double clapp::value::convert_value<double>(std::string_view param);

#ifdef CLAPP_FS_AVAIL
template <>
clapp::fs::path clapp::value::convert_value<clapp::fs::path>(
    std::string_view param);
#endif

#endif
