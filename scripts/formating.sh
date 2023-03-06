#!/bin/bash

STYLE="Google"
FILES=("../main.c" "../project/source/exchange_rate.c" \
                 "../project/include/exchange_rate.h" "../project/include/array_of_rates.h" \
                 "../project/source/array_of_rates.c" "../tests/tests.cpp")

TMP_FOLDER="tmp"
mkdir -p $TMP_FOLDER
TMP_FILE="./$TMP_FOLDER/formated_file"
touch $TMP_FILE

for file in ${FILES[*]}
do
    echo $file
    clang-format -style=$STYLE $file  > $TMP_FILE
    diff $file $TMP_FILE
done