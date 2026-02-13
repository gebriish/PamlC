#!/usr/bin/env bash

build="bin"
bin="pamlc"
src="src/main.c"

rm -rf "$build"
mkdir -p "$build"

echo "compiling ($src) -> $build/$bin"
clang $src -o $build/$bin -std=c99 -O2 -g
