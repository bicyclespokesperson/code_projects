#!/usr/bin/env bash

C_COMPILER="/usr/local/Cellar/llvm/13.0.0_1/bin/clang"
CXX_COMPILER="/usr/local/Cellar/llvm/13.0.0_1/bin/clang++"


cmake -D CMAKE_C_COMPILER="$C_COMPILER" -D CMAKE_CXX_COMPILER="$CXX_COMPILER" CMakeLists.txt
cmake -D CMAKE_C_COMPILER="$C_COMPILER" -D CMAKE_CXX_COMPILER="$CXX_COMPILER" CMakeLists.txt

