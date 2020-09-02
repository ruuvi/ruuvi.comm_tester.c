#!/bin/bash

find ./src -path ./src/ruuvi.endpoints.c -prune -o -iname *.c -o -iname *.cpp -o -iname *.h | xargs clang-format -i
