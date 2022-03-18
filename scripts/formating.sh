#!/bin/bash

STYLE="Google"
FORMATING_FILES=("./main.c" "./project/source/exchange_rate.c" \
                 "./project/include/exchange_rate.h" "./project/include/array_of_rates.h" \
                 "./project/source/array_of_rates.c" "./tests/tests.cpp")

TMP_DIR="./tmp"
LOG_FILE="./tmp/clang-formating.log"
TMP_FORM_FILE="./tmp/formated-file"
mkdir -p $TMP_DIR
touch $LOG_FILE
touch $TMP_FORM_FILE

for file in ${FORMATING_FILES[*]}
do
    echo $file
    clang-format -style=$STYLE $file > $TMP_FORM_FILE
    diff $file $TMP_FORM_FILE
done