#! /usr/bin/env sh

set -e
set -u

mkdir -p .deps
cd .deps

###############################################################################
# Flatbuffers
###############################################################################

FB_RELEASE="1.9.0"
FB_TAR="v$FB_RELEASE.tar.gz"
FB_URL="https://github.com/google/flatbuffers/archive/$FB_TAR"
FB_DIR="flatbuffers-$FB_RELEASE"

if [ -d "$FB_DIR" ]; then
   rm -rf "$FB_DIR"
fi

wget "$FB_URL"
tar -xf "$FB_TAR"
rm "$FB_TAR"

cd "$FB_DIR"
mkdir build
cd build
cmake \
   -DCMAKE_BUILD_TYPE=Release \
   -DCMAKE_INSTALL_PREFIX=../../_sysroot \
   -DFLATBUFFERS_BUILD_FLATC=OFF \
   -DFLATBUFFERS_BUILD_FLATHASH=OFF \
   -DFLATBUFFERS_BUILD_TESTS=OFF \
   -DFLATBUFFERS_BUILD_FLATLIB=ON \
   -DFLATBUFFERS_BUILD_SHAREDLIB=OFF \
   -DFLATBUFFERS_CODE_COVERAGE=OFF \
   ..
cmake --build . --target install

cd ../..


###############################################################################
# Mstch
###############################################################################

MS_RELEASE="1.0.2"
MS_TAR="$MS_RELEASE.tar.gz"
MS_URL="https://github.com/no1msd/mstch/archive/$MS_TAR"
MS_DIR="mstch-$MS_RELEASE"

if [ -d "$MS_DIR" ]; then
   rm -rf "$MS_DIR"
fi

wget "$MS_URL"
tar -xf "$MS_TAR"
rm "$MS_TAR"

cd "$MS_DIR"
mkdir build
cd build
cmake \
   -DCMAKE_BUILD_TYPE=Release \
   -DCMAKE_INSTALL_PREFIX=../../_sysroot \
   -DWITH_BENCHMARK=OFF \
   -DWITH_UNIT_TESTS=OFF \
   ..
cmake --build . --target install

cd ../..
