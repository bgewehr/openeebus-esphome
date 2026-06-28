#!/bin/bash
# Copyright 2025 NIBE AB
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

# Expected location of file containing files to ignore. This is located in the
# root of the repository
#
# This file can contain specific file names or match with an asterisk
#
# Example:
#   src/common/*
#   src/service/configuration.h
IGNORE_FILE=".format_diff_ignore"

# Verbose mode - can be enabled via -v flag or VERBOSE environment variable
VERBOSE="${VERBOSE:-0}"

# Print debug message if verbose mode is enabled
debug_print() {
  if [ "$VERBOSE" = "1" ]; then
    echo "[DEBUG] $*" >&2
  fi
}

# Function to check for clang-format version. Fails if not found
get_clang_format_cmd() {
  # Check if clang-format is available
  local CLANG_FORMAT_BINARY="clang-format"
  if command -v clang-format-18 >/dev/null 2>&1; then
    CLANG_FORMAT_BINARY="clang-format-18"
    debug_print "Found clang-format-18 at: $(command -v clang-format-18)"
  elif command -v clang-format >/dev/null 2>&1; then
    CLANG_FORMAT_BINARY="clang-format"
    debug_print "Found clang-format at: $(command -v clang-format)"
  else
    echo "Error: clang-format or clang-format-18 not found. Please install clang-format."
    exit 1
  fi
  debug_print "clang-format version: $($(command -v ${CLANG_FORMAT_BINARY}) --version)"

  # Check if git-clang-format is available
  if command -v git-clang-format-18 >/dev/null 2>&1; then
    debug_print "Found git-clang-format-18 at: $(command -v git-clang-format-18)"
    echo "git-clang-format-18 --binary $(command -v ${CLANG_FORMAT_BINARY})"
  elif command -v git-clang-format >/dev/null 2>&1; then
    debug_print "Found git-clang-format at: $(command -v git-clang-format)"
    echo "git-clang-format --binary $(command -v ${CLANG_FORMAT_BINARY})"
  else
    echo "Error: git-clang-format or git-clang-format-18 not found. Please install clang-format."
    exit 1
  fi
}

# returns 0 if element (first argument) does not exist in array (remaining
# arguments)
element_not_in_list() {
  local array_item
  local element

  element="$1"
  array_items="$2"

  for array_item in $array_items; do
    if [[ "$array_item" == "$element" ]]; then
      return 1
    fi
  done

  return 0
}

# Gets string of ignored files (separated by newline). Returns 1 if no ignore
# file exists
get_ignorelist() {
  if ! [[ -f "$IGNORE_FILE" ]]; then
    debug_print "No ignore file found at: $IGNORE_FILE"
    echo ""
    return 1
  fi

  debug_print "Reading ignore patterns from: $IGNORE_FILE"
  while IFS= read -r line; do
    if [[ -z "$line" ]]; then
      continue
    fi

    debug_print "Processing ignore pattern: $line"
    while IFS= read -r -d $'\0' file_to_ignore; do
      debug_print "  Ignoring file: $file_to_ignore"
      echo "$file_to_ignore"
    done < <(find . -wholename "$line" -print0)
  done <"$IGNORE_FILE"
}

# Checks if there is a list of files to ignore, and if so removes these files
# from the list
filter_ignored_files() {
  local all_diffs
  all_diffs="$1"

  local files_to_ignore
  files_to_ignore=$(get_ignorelist)

  # No ignore file - return original input
  if [[ $? -eq 1 ]]; then
    echo "$all_diffs"
    return 0
  fi

  for changed_file in $all_diffs; do
    # ignore list expects file name to have './' prefix (or find returns that)
    if element_not_in_list "./${changed_file}" "$files_to_ignore"; then
      echo "$changed_file"
    fi
  done
}

# Function to format the modified parts of source files
format_diff() {
  local branch_out_point="$1"

  debug_print "Branch out point: $branch_out_point"

  # Check if there are modified files matching the pattern
  local all_diffs
  all_diffs=$(grep -E '\.(c|cpp|h|hpp)$' code_diff)
  debug_print "Modified C/C++ files: $(echo -n "$all_diffs" | wc -l) files"

  local diffs
  diffs=$(filter_ignored_files "$all_diffs")

  if [ -z "$diffs" ]; then
    echo "No changes found"
    return 0
  fi

  debug_print "Files to check after filtering:"
  if [ "$VERBOSE" = "1" ]; then
    echo "$diffs" | while read -r file; do
      debug_print "  $file"
    done
  fi

  local clang_format_cmd
  local formatted_files

  # Run git clang-format for c, cpp, h and hpp files
  if clang_format_cmd=$(get_clang_format_cmd); then
    debug_print "Running command: $clang_format_cmd $branch_out_point <files>"
    # ${diffs} can't be quoted here in current state (can only handle source
    # files without space in the name)
    # We want error output as well, since clang-format can error out on files
    # and say that no files were changed
    formatted_files=$(2>&1 $clang_format_cmd "${branch_out_point}" ${diffs})
    debug_print "Command output: $formatted_files"
  else
    # clang_format_cmd contains error message
    formatted_files=$clang_format_cmd
  fi

  # Check if there are differences

  case "$formatted_files" in
    ""\
    |"clang-format did not modify any files"\
    |"no modified files to format"\
    )
      echo "No differences found in clang-format"
      return 0
      ;;
    *)
      if [ -n "$CI" ]; then
        {
          echo "Error: format-diff.sh found differences in formatting in the following files:"
          echo ""
          echo "$formatted_files"
          echo ""
          echo " Please run format_diff.sh locally to fix the issues."
        } | tee -a diff.md # Append output to diff.md and display it only in CI environment
      else
        echo "Error: format_diff.sh found differences in formatting in the following files:"
        echo ""
        echo "$formatted_files"
      fi
      return 1
      ;;
  esac
}

# Gets main branch to use. This looks for a .main_branch in parent project
# root directory. If file can't be found we'll return origin/master, since it's
# the most commonly used main branch
get_main_branch() {
  if ! [ -f .main_branch ]; then
    debug_print "No .main_branch file found, defaulting to origin/master"
    echo "origin/master"
    return
  fi

  local main_branch
  main_branch=$(cat .main_branch)
  debug_print "Main branch from .main_branch: $main_branch"
  echo "$main_branch"
}

# Parse command line arguments
ROOT="."
while [[ $# -gt 0 ]]; do
  case $1 in
    -v|--verbose)
      VERBOSE=1
      debug_print "Verbose mode enabled"
      shift
      ;;
    *)
      ROOT="$1"
      shift
      ;;
  esac
done

# Error out if we can't switch directory
debug_print "Changing to directory: ${ROOT}"
if ! cd "${ROOT}"; then
  echo "Could not enter directory: ${ROOT}"

  # CI expects diff.md to be present at later step. Leave an error message
  if [ -n "$CI" ]; then
    echo "Could not enter directory: ${ROOT}" >diff.md
  fi
  exit 1
fi

# Get main branch
MAIN_BRANCH=$(get_main_branch)
echo "Checking clang-format against \"${MAIN_BRANCH}\" ..."

# Find the common ancestor between the current branch and the branch-out point
debug_print "Running: git merge-base ${MAIN_BRANCH} HEAD"
if ! BRANCH_OUT_POINT=$(git merge-base "${MAIN_BRANCH}" HEAD); then
  echo "Incorrect branch ${MAIN_BRANCH}"
  if [ -n "$CI" ]; then
    echo "Incorrect main branch ${MAIN_BRANCH}" >diff.md
  fi
  exit 1
fi
debug_print "Merge base: $BRANCH_OUT_POINT"

# Generate diff of modified files against the main
debug_print "Running: git diff --name-only ${MAIN_BRANCH} --diff-filter=d"
git diff --name-only "${MAIN_BRANCH}" --diff-filter=d >code_diff
if [ "$VERBOSE" = "1" ]; then
  debug_print "Modified files:"
  while read -r file; do
    debug_print "  $file"
  done <code_diff
fi

# Call the function to format the modified parts of source files
format_diff "${BRANCH_OUT_POINT}"
RETURN_VAL=$?

# clean up
rm code_diff

exit $RETURN_VAL
