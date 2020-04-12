cmake_minimum_required(VERSION 3.8.0)

message(STATUS "Resolving GIT Version for libClaPP")

find_package(Git)

if(NOT Git_FOUND)
  message(STATUS "Could not find git package.")
  find_program(GIT_EXECUTABLE NAMES git DOC "Git command line client")
  if(NOT GIT_EXECUTABLE)
      message(STATUS "Could not find git executable, set it to `git`.")
      set(GIT_EXECUTABLE "git")
  else()
      message(STATUS "Found git executable.")
  endif()
else()
    message(STATUS "Found git package.")
endif()

message(STATUS "trying git `${GIT_EXECUTABLE}`:")
execute_process(
  COMMAND ${GIT_EXECUTABLE} -C ${libClaPP_SOURCE_DIR} rev-parse --abbrev-ref HEAD
  WORKING_DIRECTORY "${local_dir}"
  OUTPUT_VARIABLE libClaPP_GIT_BRANCH
  ERROR_QUIET
  OUTPUT_STRIP_TRAILING_WHITESPACE
  RESULT_VARIABLE libClaPP_result_branch
)
execute_process(
  COMMAND ${GIT_EXECUTABLE} -C ${libClaPP_SOURCE_DIR} describe --dirty --broken --always
  WORKING_DIRECTORY "${local_dir}"
  OUTPUT_VARIABLE libClaPP_GIT_HASH
  ERROR_QUIET
  OUTPUT_STRIP_TRAILING_WHITESPACE
  RESULT_VARIABLE libClaPP_result_hash
)
execute_process(
  COMMAND ${GIT_EXECUTABLE} -C ${libClaPP_SOURCE_DIR} describe --dirty --broken --always --tags
  WORKING_DIRECTORY "${local_dir}"
  OUTPUT_VARIABLE libClaPP_GIT_DESCRIPTION
  ERROR_QUIET
  OUTPUT_STRIP_TRAILING_WHITESPACE
  RESULT_VARIABLE libClaPP_result_description
)
if(NOT ((libClaPP_result_hash EQUAL 0) AND (libClaPP_result_branch EQUAL 0) AND (libClaPP_result_description EQUAL 0)))
    set(libClaPP_GIT_HASH "unknown")
    set(libClaPP_GIT_BRANCH "unknown")
    set(libClaPP_GIT_DESCRIPTION "unknown")
endif()
message(STATUS "GIT branch: ${libClaPP_GIT_BRANCH}, GIT hash: ${libClaPP_GIT_HASH}, GIT description: ${libClaPP_GIT_DESCRIPTION}")
