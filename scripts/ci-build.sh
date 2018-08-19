#! /usr/bin/env sh

set -e
set -u
set -x

# Install the dependencies
sh scripts/install-deps.sh

# Compile for Debug
mkdir build-debug
cd build-debug
cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_INSTALL_PREFIX=_install ..
cmake --build . --target install
cd ..

# Compile for Release
mkdir build-release
cd build-release
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=_install ..
cmake --build . --target install
cd ..
