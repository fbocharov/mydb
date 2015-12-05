#!/bin/bash

BASE_FLAGS="--force --language=c++ --std=c++11 -i ./src/sql"

cppcheck -j 4 --enable=all $BASE_FLAGS .
cppcheck --enable=unusedFunction $BASE_FLAGS .