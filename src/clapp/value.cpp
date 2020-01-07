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

#include <clapp/value.h>
#include <stdexcept>
#include <string>

template <typename T>
static T convert_uint(const std::string_view param);
template <typename T>
static T convert_int(const std::string_view param);

template <>
std::string clapp::value::convert_value<std::string>(
    const std::string_view param) {
    return std::string{param};
}

template <>
std::int8_t clapp::value::convert_value<std::int8_t>(
    const std::string_view param) {
    return convert_int<std::int8_t>(param);
}

template <>
std::int16_t clapp::value::convert_value<std::int16_t>(
    const std::string_view param) {
    return convert_int<std::int16_t>(param);
}

template <>
std::int32_t clapp::value::convert_value<std::int32_t>(
    const std::string_view param) {
    return convert_int<std::int32_t>(param);
}

template <>
std::int64_t clapp::value::convert_value<std::int64_t>(
    const std::string_view param) {
    return convert_int<std::int64_t>(param);
}

template <>
std::uint8_t clapp::value::convert_value<std::uint8_t>(
    const std::string_view param) {
    return convert_uint<std::uint8_t>(param);
}

template <>
std::uint16_t clapp::value::convert_value<std::uint16_t>(
    const std::string_view param) {
    return convert_uint<std::uint16_t>(param);
}

template <>
std::uint32_t clapp::value::convert_value<std::uint32_t>(
    const std::string_view param) {
    return convert_uint<std::uint32_t>(param);
}

template <>
std::uint64_t clapp::value::convert_value<std::uint64_t>(
    const std::string_view param) {
    return convert_uint<std::uint64_t>(param);
}

#ifdef CLAPP_FS_AVAIL
template <>
clapp::fs::path clapp::value::convert_value<clapp::fs::path>(
    const std::string_view param) {
    return {param};
}

std::string clapp::path_exists_t::append_restriction_text() {
    return "existing path";
}

void clapp::path_exists_t::validate(const clapp::fs::path &path,
                                    const std::string &param_name) const {
    if (!clapp::fs::exists(path)) {
        std::stringstream ss;
        ss << "CLI value " << path << " for '" << param_name
           << "' does not exist.";
        throw std::runtime_error(ss.str());
    }
}
#endif

template <typename T>
static T convert_uint(const std::string_view param) {
    static_assert(std::is_integral<T>::value,
                  "Integral template parameter required.");
    static_assert(std::is_unsigned<T>::value,
                  "Unsigned template parameter required.");
    const std::uint64_t value{std::stoull(std::string{param}, 0, 0)};
    if (value > std::numeric_limits<T>::max()) {
        std::stringstream ss;
        ss << "convert_value: value " << value << " is bigger than max "
           << std::numeric_limits<T>::max();
        throw std::out_of_range{ss.str()};
    }
    return static_cast<T>(value);
}

template <typename T>
static T convert_int(const std::string_view param) {
    static_assert(std::is_integral<T>::value,
                  "Integral template parameter required.");
    static_assert(std::is_signed<T>::value,
                  "Signed template parameter required.");
    const std::int64_t value{std::stoll(std::string{param}, 0, 0)};
    if (value > std::numeric_limits<T>::max()) {
        std::stringstream ss;
        ss << "convert_value: value " << value << " is bigger than max "
           << std::numeric_limits<T>::max();
        throw std::out_of_range{ss.str()};
    }
    if (value < std::numeric_limits<T>::min()) {
        std::stringstream ss;
        ss << "convert_value: value " << value << " is smaller than min "
           << std::numeric_limits<T>::max();
        throw std::out_of_range{ss.str()};
    }
    return static_cast<T>(value);
}
