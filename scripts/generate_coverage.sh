#!/bin/bash

pip install gcovr

ctest
cd ..
EXCLUDING_FILES=".*_deps.*|.*tests.*|project/main.c"
REPORT_FILE_NAME="coverage.xml"
gcovr -r . ./tests -o $REPORT_FILE_NAME --xml