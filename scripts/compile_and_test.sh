#!/bin/sh
set -e
cd "$(git rev-parse --show-toplevel)"
mkdir -p cmake_build
cd cmake_build
cmake ..
cmake --build .
ctest
