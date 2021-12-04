0.4.0 - 2021-12-05
==================
This release contains a major interface-change (at least compared to the 0.3.x- or 0.2.x-series):
The interface of the `clapp::value::found_func_t::func_t`-callback was changed from `std::function<void()>` to `std::function<std::optional<exit_t>(const std::string &)>`.
Thus, it supports an optional exit-value which is propagated down to the `parse()`- and `parse_and_validate()`-functions and can be used to determine if the program should exit or not.
Note: also `basic_help_option_t` uses this mechanism and does not call `::exit()` or `::_exit()` anymore!
Note: if a found-callback-func returns a value, the parsing-process gets interrupted. Thus, accessing members of the parser after the parsing was interrupted can lead to undefined behavior (you may use a parser-container like `basic_parser_container_t` to prevent this).

Release 0.4.0 contains the following changes:
---------------------------------------------
- Sub-parser example ([examples/sub_parser_example.cpp](examples/sub_parser_example.cpp)) uses a new intermediate parser.
- Introduced new type `clapp::value::exit_t`.
- Updated `clapp::value::found_func_t::func_t` to use `clapp::value::exit_t` and the name of the option/argument as parameter (`std::function<std::optional<exit_t>(const std::string &)>`).
- Introduced parser-container `clapp::parser::basic_parser_container_t`: It can be used as a wrapper for a main-parser and ensures that the parser is not used if the parsing-process was interrupted by an exit-request of a found-function.
- Any calls to `::exit()` or `::_exit()` were removed (not required anymore due to the new found-callback-interface).
- Replaced internal implementation of `purpose_t`-stringification from `to_cstring()` to `to_string_view()`.
- Added support for clang-tidy-13.
- Several changes in gitlab-CI: (updated ubuntu 21.04 to 22.04, added debian 11 pipelines, added clang-11 and clang-12 to ubuntu 20.04)

0.3.1 - 2021-08-17
==================
This release uses the same interfaces (as in 0.3.0).
It only fixes the documentation and the CMake-Build.

Release 0.3.1 contains the following changes:
---------------------------------------------
- fixed `doc/doc.md`.
- fixed `README.md`.
- fixed default CMAKE build-type.

0.3.0 - 2021-07-09
==================
This release uses (nearly everywhere) the same interfaces (as in 0.2.5) but changed the implementation under the hood.
These changes are a preparation for future features of libClaPP (where options and parameters can be combined logically and grouped together).

One of these exceptions where the behavior changed are non-param-options (i.e. `basic_option_t`), which were modified slightly. Just search for `basic_option_t` in the changes below.

Release 0.3.0 contains the following changes:
---------------------------------------------
- added support for leak sanitzier.
- new Gitlab-CI-pipelines (added the following new architectures and OSs):
  * includes amd64 (debian 10, ubuntu 20.04, ubuntu 21.04)
  * includes arm64 (debian 10, ubuntu 20.04, ubuntu 21.04)
  * includes armhf (debian 10)
- added support for clang- and gcc-compilers up to version 12 (however, C++17 is still a mandatory requirement).
- moved clang-tidy build-options to `cmake/clang_tidy.cmake` (including a transition to clang-tidy-12 and several code-fixes newer clang-tidy-versions complained about).
- moved compiler build-options to `cmake/compiler_flags.cmake`.
- bump clang-format-7 to clang-format-11 (as clang-format-7 formatted code leads to
`compound-token-split-by-space` warning in clang-12).
- bump submodule `third_party/googletest` to version 1.11.0.
- bump submodule `third_party/GSL` to version 3.1.0.
- fixed documentation.
- fixed and added new tests.
- renamed test-names from snake-case to camel-case. (as suggested by google-test framework documentation)
- updated help text generation.
- type of `basic_option_t::_value` changed from `std::optional<T>` to `T`.
- `operator bool()` of an `basic_option_t` now returns `_value != default_value` instead of the value returned by `has_value()`.

0.2.5-GSL3 - 2020-11-03
=======================
- Introduced support for GSL3.

0.2.5 - 2020-11-01
==================
- Added pedantic compiler option flag switch `libClaPP_PEDANTIC_COMPILER_FLAGS` to [CMakeLists.txt](CMakeLists.txt).

0.2.4 - 2020-05-13
==================
- Added `size_[argument|param_option]_t` and `ptrdiff_[argument|param_option]_t`.

0.2.3 - 2020-05-06
==================
- Added `bool_argument_t` and `bool_param_option_t`.

0.2.2 - 2020-05-04
==================
- Fixed package-config file to contain the right version number.

0.2.1 - 2020-05-04
==================
- Fixed cmake/sanitizer.cmake to be compliant to CMake prior version 3.12.

0.2.0 - 2020-04-12
==================
- bump version to 0.2.0
- updated README.md
- added and updated several tests
- added and updated doc
- updated sample_extractor and added sample_executor
- added clang-format specification
- added support for sanitizers
- added more exhaustive gitlab-pipeline
- added more specific exceptions
- updated help-message generation in parsers (sub_parser, main_parser and basic_parser)
- minor improvements of CMakeLists.txt (e.g. added cppcheck)
- renamed full_example to large_example
- minor improvements to examples
- added test examples scripts
- added build_info_string
- unified arguments and options

0.1.0 - 2020-01-28
==================
- new exceptions based on clapp::exception::clapp_exception.
- added `parse_and_validate()` function.
- added found-func
- added help-option
- added more constexpr
- added more comments to examples
- code cleanup
- bump version to 0.1.0
- added gitlab-ci pipeline
- added double/float values
- added support for vector of short/long options
- added doc-extractor
