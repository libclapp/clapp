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

#include <iostream>
#include <string>

template <typename T>
static T convert_uint(std::string_view param);
template <typename T>
static T convert_int(std::string_view param);

clapp::value::found_func_t::found_func_t(func_t&& func_arg)
    : func{std::move(func_arg)} {}

void clapp::value::found_func_t::found() { func(); }

template <>
std::string clapp::value::convert_value<std::string>(
    const std::string_view param) {
    std::string ret{param};
    return ret;
}

template <>
bool clapp::value::convert_value<bool>(const std::string_view param) {
    if (param == "TRUE" || param == "true" || param == "1") {
        return true;
    }
    if (param == "FALSE" || param == "false" || param == "0") {
        return false;
    }
    std::stringstream ss;
    ss << "convert_value: value '" << param << "' is invalid. "
       << "(valid values: TRUE, true, 1, FALSE, false, 0)";
    throw clapp::exception::invalid_value_t{ss.str()};
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

template <>
std::chrono::nanoseconds clapp::value::convert_value<std::chrono::nanoseconds>(
    const std::string_view param) {
    return std::chrono::nanoseconds{convert_uint<std::uint64_t>(param)};
}

template <>
std::chrono::microseconds
clapp::value::convert_value<std::chrono::microseconds>(
    const std::string_view param) {
    return std::chrono::microseconds{convert_uint<std::uint64_t>(param)};
}

template <>
std::chrono::milliseconds
clapp::value::convert_value<std::chrono::milliseconds>(
    const std::string_view param) {
    return std::chrono::milliseconds{convert_uint<std::uint64_t>(param)};
}

template <>
std::chrono::seconds clapp::value::convert_value<std::chrono::seconds>(
    const std::string_view param) {
    return std::chrono::seconds{convert_uint<std::uint64_t>(param)};
}

template <>
std::chrono::minutes clapp::value::convert_value<std::chrono::minutes>(
    const std::string_view param) {
    return std::chrono::minutes{convert_uint<std::uint64_t>(param)};
}

template <>
std::chrono::hours clapp::value::convert_value<std::chrono::hours>(
    const std::string_view param) {
    return std::chrono::hours{convert_uint<std::uint64_t>(param)};
}

template <>
float clapp::value::convert_value<float>(const std::string_view param) {
    try {
        return std::stof(std::string{param}, nullptr);
    } catch (std::out_of_range& e) {
        std::stringstream ss;
        ss << "convert_value: value '" << param << "' is out of float-range. ("
           << e.what() << ")";
        throw clapp::exception::out_of_range_t{ss.str()};
    } catch (std::invalid_argument& e) {
        std::stringstream ss;
        ss << "convert_value: value '" << param << "' is invalid. (" << e.what()
           << ")";
        throw clapp::exception::invalid_value_t{ss.str()};
    }
}

template <>
double clapp::value::convert_value<double>(const std::string_view param) {
    try {
        return std::stod(std::string{param}, nullptr);
    } catch (std::out_of_range& e) {
        std::stringstream ss;
        ss << "convert_value: value '" << param << "' is out of double-range. ("
           << e.what() << ")";
        throw clapp::exception::out_of_range_t{ss.str()};
    } catch (std::invalid_argument& e) {
        std::stringstream ss;
        ss << "convert_value: value '" << param << "' is invalid. (" << e.what()
           << ")";
        throw clapp::exception::invalid_value_t{ss.str()};
    }
}

#ifdef CLAPP_FS_AVAIL
template <>
clapp::fs::path clapp::value::convert_value<clapp::fs::path>(
    const std::string_view param) {
    return {param};
}

void clapp::path_exists_t::validate(const clapp::fs::path& path,
                                    const std::string& param_name) {
    if (!clapp::fs::exists(path)) {
        std::stringstream ss;
        ss << "CLI value '" << path << "' for '" << param_name
           << "' does not exist.";
        throw clapp::exception::path_does_not_exist_t(ss.str());
    }
}
#endif

template <typename T>
static T convert_uint(const std::string_view param) {
    static_assert(std::is_integral<T>::value,
                  "Integral template parameter required.");
    static_assert(std::is_unsigned<T>::value,
                  "Unsigned template parameter required.");
    try {
        const std::uint64_t value{std::stoull(std::string{param}, nullptr, 0)};

        if (value > std::numeric_limits<T>::max()) {
            std::stringstream ss;
            ss << "convert_value: value '" << value << "' is bigger than max "
               << std::numeric_limits<T>::max();
            throw clapp::exception::out_of_range_t{ss.str()};
        }
        return static_cast<T>(value);
    } catch (std::out_of_range& e) {
        std::stringstream ss;
        ss << "convert_value: value '" << param << "' is out of range. ("
           << std::numeric_limits<T>::min() << " and "
           << std::numeric_limits<T>::max() << ", " << e.what() << ")";
        throw clapp::exception::out_of_range_t{ss.str()};
    } catch (std::invalid_argument& e) {
        std::stringstream ss;
        ss << "convert_value: value '" << param << "' is invalid. (" << e.what()
           << ")";
        throw clapp::exception::invalid_value_t{ss.str()};
    }
}

template <typename T>
static T convert_int(const std::string_view param) {
    static_assert(std::is_integral<T>::value,
                  "Integral template parameter required.");
    static_assert(std::is_signed<T>::value,
                  "Signed template parameter required.");
    try {
        const std::int64_t value{std::stoll(std::string{param}, nullptr, 0)};

        if (value > std::numeric_limits<T>::max()) {
            std::stringstream ss;
            ss << "convert_value: value '" << value << "' is bigger than max "
               << std::numeric_limits<T>::max();
            throw clapp::exception::out_of_range_t{ss.str()};
        }

        if (value < std::numeric_limits<T>::min()) {
            std::stringstream ss;
            ss << "convert_value: value '" << value << "' is smaller than min "
               << std::numeric_limits<T>::max();
            throw clapp::exception::out_of_range_t{ss.str()};
        }
        return static_cast<T>(value);
    } catch (std::out_of_range& e) {
        std::stringstream ss;
        ss << "convert_value: value '" << param << "' is out of range. ("
           << std::numeric_limits<T>::min() << " and "
           << std::numeric_limits<T>::max() << ", " << e.what() << ")";
        throw clapp::exception::out_of_range_t{ss.str()};
    } catch (std::invalid_argument& e) {
        std::stringstream ss;
        ss << "convert_value: value '" << param << "' is invalid. (" << e.what()
           << ")";
        throw clapp::exception::invalid_value_t{ss.str()};
    }
}
