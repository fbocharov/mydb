#!/bin/bash

cppcheck --force --enable=all -j 4 --language=c++ --std=c++11 .
cppcheck --force --enable=unusedFunction --language=c++ --std=c++11 .