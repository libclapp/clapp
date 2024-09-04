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

#ifndef CLAPP_CONTRACT_H
#define CLAPP_CONTRACT_H

#include <array>
#include <cstdlib>
#include <future>
#include <optional>
#include <source_location>
#include <string_view>
#include <type_traits>
#include <variant>

namespace clapp::contract {
[[noreturn]] inline void terminate_with_location(
    std::string_view message,
    // NOLINTNEXTLINE(fuchsia-default-arguments-declarations)
    std::source_location location = std::source_location::current());
}  // namespace clapp::contract

#ifdef CLAPP_ENABLE_CONTRACT_CHECK

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define CLAPP_CONTRACT_GENERATE_FAILURE_MESSAGE(type, cond) \
    "Checking " type " '" cond "' failed"

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define CLAPP_CONTRACT_FAILED(type, cond)       \
    ::clapp::contract::terminate_with_location( \
        CLAPP_CONTRACT_GENERATE_FAILURE_MESSAGE(type, cond))

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define CLAPP_CONTRACT_CHECK(type, cond) \
    (cond) ? static_cast<void>(0) : CLAPP_CONTRACT_FAILED(type, #cond)

#else

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define CLAPP_CONTRACT_CHECK(type, cond) \
    do {                                 \
        if (cond) {                      \
            break;                       \
        }                                \
    } while (false)

#endif

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define CLAPP_CONTRACT_ENSURES(cond) CLAPP_CONTRACT_CHECK("postcondition", cond)

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define CLAPP_CONTRACT_EXPECTS(cond) CLAPP_CONTRACT_CHECK("precondition", cond)

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define CLAPP_CONTRACT_UNREACHABLE() \
    ::clapp::contract::terminate_with_location("Reached unreachable code")

#include <clapp/contract.hpp>

#endif
