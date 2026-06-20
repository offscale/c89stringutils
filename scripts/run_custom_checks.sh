#!/usr/bin/env bash
set -e

# Build
cmake -S . -B cmake-build-debug -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTING=ON
cmake --build cmake-build-debug

# Test
ctest --test-dir cmake-build-debug -C Debug

# Valgrind (on Linux)
if [ "$(uname)" = "Linux" ]; then
    valgrind --error-exitcode=1 --leak-check=full --show-leak-kinds=definite ./cmake-build-debug/bin/c89stringutils_tests
fi

# Shields
python3 scripts/update_shields.py

# Stage README.md if it was modified
