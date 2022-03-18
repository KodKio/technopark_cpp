#!/bin/bash

STYLE="Google"
FILES=("./main.c" "./project/source/exchange_rate.c" \
                 "./project/include/exchange_rate.h" "./project/include/array_of_rates.h" \
                 "./project/source/array_of_rates.c" "./tests/tests.cpp")

TMP_FILE="./tmp/formated-file"
touch $TMP_FILE

for file in ${FORMATING_FILES[*]}
do
    echo $file
    clang-format -style=$STYLE $file  > $TMP_FILE
    diff $file $TMP_FORM_FILE
done