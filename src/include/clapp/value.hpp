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

#ifndef LIBCLAPP_VALUE_HPP
#define LIBCLAPP_VALUE_HPP

#include <clapp/value.h>
#include <gsl/gsl_assert>
#include <sstream>

template <typename T>
constexpr clapp::value::default_value_t<T>::default_value_t(T _value)
    : value{_value} {}

template <typename T>
std::string clapp::value::default_value_t<T>::append_restriction_text() const {
    std::stringstream ss;
    ss << "default-value: ";
    if constexpr (std::is_integral<T>::value) {
        ss << std::to_string(value);
    } else {
        ss << value;
    }
    return ss.str();
}

template <typename T>
T clapp::value::default_value_t<T>::default_value() const {
    return value;
}

template <typename T>
constexpr clapp::value::min_max_value_t<T>::min_max_value_t(T _min, T _max)
    : min{_min}, max{_max} {
    Expects(min < max);
}

template <typename T>
std::string clapp::value::min_max_value_t<T>::append_restriction_text() const {
    std::stringstream ss;
    if constexpr (std::is_integral<T>::value) {
        ss << "constraint: [" << std::to_string(min) << ","
           << std::to_string(max) << "]";
    } else {
        ss << "constraint: [" << min << "," << max << "]";
    }
    return ss.str();
}

template <typename T>
void clapp::value::min_max_value_t<T>::validate(
    const T &value, const std::string &param_name) const {
    if (value < min) {
        std::stringstream ss;
        ss << "CLI value for '" << param_name << "' is too low (";
        if constexpr (std::is_integral<T>::value) {
            ss << std::to_string(value) << " < " << std::to_string(min) << ")";
        } else {
            ss << value << " < " << min << ")";
        }
        throw std::out_of_range{ss.str()};
    }
    if (value > max) {
        std::stringstream ss;
        ss << "CLI value for '" << param_name << "' is too high (";
        if constexpr (std::is_integral<T>::value) {
            ss << std::to_string(value) << " < " << std::to_string(min) << ")";
        } else {
            ss << value << " > " << min << ")";
        }
        throw std::out_of_range{ss.str()};
    }
}

template <>
std::string clapp::value::convert_value<std::string>(
    const std::string_view param);

template <>
std::int8_t clapp::value::convert_value<std::int8_t>(
    const std::string_view param);

template <>
std::int16_t clapp::value::convert_value<std::int16_t>(
    const std::string_view param);

template <>
std::int32_t clapp::value::convert_value<std::int32_t>(
    const std::string_view param);

template <>
std::int64_t clapp::value::convert_value<std::int64_t>(
    const std::string_view param);

template <>
std::uint8_t clapp::value::convert_value<std::uint8_t>(
    const std::string_view param);

template <>
std::uint16_t clapp::value::convert_value<std::uint16_t>(
    const std::string_view param);

template <>
std::uint32_t clapp::value::convert_value<std::uint32_t>(
    const std::string_view param);

template <>
std::uint64_t clapp::value::convert_value<std::uint64_t>(
    const std::string_view param);

#ifdef CLAPP_FS_AVAIL
template <>
clapp::fs::path clapp::value::convert_value<clapp::fs::path>(
    const std::string_view param);
#endif

#endif
