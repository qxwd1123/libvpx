#!/usr/bin/bash

if [[ ! -d streams ]]; then
    ln -sf ../streams ./
fi

mkdir -p build_dir
cd build_dir

CC=clang
CXX=clang++
LD=clang++
CFLAGS=-fsanitize=address,undefined
CXXFLAGS=-fsanitize=address,undefined
LDFLAGS=-fsanitize=address,undefined

if [[ ! -a vpxdec ]]; then
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
fi

make -j
