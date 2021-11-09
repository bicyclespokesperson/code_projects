#!/usr/bin/env bash

rm -r ./CMakeScripts
rm -r ./CMakeFiles 
rm -r ./CMakeCache.txt 
rm -r ./CTestTestfile.cmake 
rm -r ./DartConfiguration.tcl 
rm -r ./Debug 
rm -r ./Testing 
rm -rf ./_deps 
rm -r ./bin/*
rm -r ./build
rm -r ./compile_commands.json

find . -name '*.xcodeproj' -delete
find . -name '*.build' -delete