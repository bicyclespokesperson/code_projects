#!/usr/bin/env bash

C_COMPILER="/usr/local/bin/gcc-10"
CXX_COMPILER="/usr/local/bin/g++-10"


cmake -D CMAKE_C_COMPILER="$C_COMPILER" -D CMAKE_CXX_COMPILER="$CXX_COMPILER" CMakeLists.txt

