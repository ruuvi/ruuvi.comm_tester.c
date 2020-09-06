#!/bin/bash

find ./src -iname *.c -o -iname *.cpp -o -iname *.h | xargs clang-format -i
