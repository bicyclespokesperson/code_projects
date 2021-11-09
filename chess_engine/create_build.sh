#!/usr/bin/env bash

C_COMPILER="/usr/local/Cellar/llvm/13.0.0_1/bin/clang"
CXX_COMPILER="/usr/local/Cellar/llvm/13.0.0_1/bin/clang++"

BUILD_TYPE="DEBUG"
#BUILD_TYPE="RELEASE"

#cmake -GXcode CMakeLists.txt
cmake -D CMAKE_C_COMPILER="$C_COMPILER" -D CMAKE_CXX_COMPILER="$CXX_COMPILER" -D CMAKE_BUILD_TYPE="$BUILD_TYPE" CMakeLists.txt

