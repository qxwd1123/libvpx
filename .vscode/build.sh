#!/usr/bin/bash

export CMAKE_GENERATOR="Ninja"
# export CFLAGS=-g
# export CXXFLAGS=-g

# if [[ ! -h /aom ]]; then
#     rm -rf /aom
#     ln $(pwd) /aom
# fi
# cd /aom
mkdir -p cmbuild
cd cmbuild

../configure \
  --disable-vp8 \
  --disable-vp9-encoder \
  --disable-multithread \
  --disable-optimizations \
  --disable-docs \
  --enable-vp9-highbitdepth \
  --enable-unit-tests \
  --enable-debug \
  --enable-debug-libs \
  --disable-mmx \
  --disable-sse \
  --disable-sse2 \
  --disable-sse3 \
  --disable-ssse3 \
  --disable-sse4_1 \
  --disable-avx \
  --disable-avx2 \
  --disable-avx512

make -j12