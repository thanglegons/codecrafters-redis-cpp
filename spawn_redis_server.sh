#!/bin/sh
#
# DON'T EDIT THIS!
#
# CodeCrafters uses this file to test your code. Don't make any changes here!
#
# DON'T EDIT THIS!
set -e
# vcpkg & cmake are required. 
export VCPKG_ROOT=~/Documents/workspace/vcpkg
cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake
cmake --build ./build
exec ./build/server "$@"
