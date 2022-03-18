#ifndef PROJECT_INCLUDE_EXCHANGE_RATE_H_
#define PROJECT_INCLUDE_EXCHANGE_RATE_H_

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum { ERROR = 0, SUCCESS };

typedef struct exchange_rate {
  char* bank_name;
  char* currency_from;
  char* currency_to;
  double ratio;
} exchange_rate_t;

double get_value(const char* str);

exchange_rate_t create_rate(FILE* in, FILE* out);

void print(exchange_rate_t rate, FILE* out);

bool eq_rates(exchange_rate_t rate1, exchange_rate_t rate2);

#endif  // PROJECT_INCLUDE_EXCHANGE_RATE_H_
