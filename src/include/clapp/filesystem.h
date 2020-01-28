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

#ifndef CLAPP_FILESYSTEM_H
#define CLAPP_FILESYSTEM_H

#if __has_include(<filesystem>)

#include <filesystem>
namespace clapp {
namespace fs = std::filesystem;
}  // namespace clapp

#define CLAPP_FS_AVAIL
#elif __has_include(<experimental/filesystem>)

#include <experimental/filesystem>
namespace clapp {
namespace fs = std::experimental::filesystem;
}  // namespace clapp

#define CLAPP_FS_AVAIL
#endif

#endif
