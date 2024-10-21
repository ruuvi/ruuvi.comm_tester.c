#!/bin/bash

mkdir -p cmake-build-unit-tests
cd cmake-build-unit-tests
cmake -G "Ninja" ..
ninja
ctest

