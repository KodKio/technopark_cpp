#!/bin/bash

STYLE="Google"
FORMATING_FILES=("./main.c" "./exchange_rates.c" \
                 "./exchange_rates.h" "./array_of_rates.h" \
                 "./array_of_rates.c" "./tests.cpp")
for file in ${FORMATING_FILES[*]}
do
    echo $file
    clang-format -i -style=$STYLE $file
done