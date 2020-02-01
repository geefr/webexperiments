#!/usr/bin/env sh
set -x

rm -rf ./build
mkdir -p ./build

# Fetch options
# enable fetches, enable waitable fetches on all threads
# PTHREADS support requires SharedArrayBuffer support in browser. Disabled in many as of ~Sep 2019 due to Spectre vulns
# -s USE_PTHREADS=1
fetchOpts="-s FETCH=1"

# OpenGL options
glOpts="-s USE_WEBGL2=1 -s USE_SDL=2"

# Language and optimisation options
# Exception catches will be disabled at higher optimisation levels by default, turn them back on but try not to abuse them due to inherent performance issues
# ALLOW_MEMORY_GROWTH allows the heap to grow if needed, if not present we'll abort on large allocations (obviously need to optimise/test/limit caches and such)
langOpts="-O3 -std=c++14 -s DISABLE_EXCEPTION_CATCHING=0 -s ALLOW_MEMORY_GROWTH=1"

em++ ${fetchOpts} ${glOpts} ${langOpts} src/*.cpp -I../common/glm/ -o build/client.html

