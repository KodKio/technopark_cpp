#ifndef PROJECT_INCLUDE_EXCHANGE_RATE_H_
#define PROJECT_INCLUDE_EXCHANGE_RATE_H_

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum { ERROR = 0, SUCCESS };

typedef struct Exchange_rate {
  char* bank_name;
  char* currency_from;
  char* currency_to;
  double ratio;
} Exchange_rate;

double get_value(const char* str);

Exchange_rate create_rate(FILE* in, FILE* out);

void print(Exchange_rate rate, FILE* out);

bool eq_rates(Exchange_rate rate1, Exchange_rate rate2);

#endif  // PROJECT_INCLUDE_EXCHANGE_RATE_H_
