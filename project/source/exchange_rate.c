#include "../include/exchange_rate.h"

double get_value(const char* str) {
  char* ptr;
  double value;
  value = strtod(str, &ptr);
  if (*ptr != '\0') {
    return -1;
  }
  return value;
}

exchange_rate_t create_rate(FILE* in, FILE* out) {
  char bank_name[100];
  char currency_from[4];
  char currency_to[4];
  double ratio;
  char value_str[100];
  char tmp[100];

  fprintf(out, "Enter bank name:\n");
  fscanf(in, "%99s", bank_name);

  fprintf(out,
          "Enter the original currency (the name length is no more than 3):\n");
  fscanf(in, "%99s", tmp);
  strncpy(currency_from, tmp, 3);
  currency_from[3] = '\0';

  fprintf(out,
          "Enter the currency to which the transfer will be made "
          "(the length of the name is no more than 3):\n");
  fscanf(in, "%99s", tmp);
  strncpy(currency_to, tmp, 4);
  currency_to[3] = '\0';

  fprintf(out, "Enter rate:\n");
  fscanf(in, "%99s", value_str);
  ratio = get_value(value_str);
  while (ratio == -1) {
    fprintf(out, "Try again to enter rate:\n");
    fscanf(in, "%99s", value_str);
    ratio = get_value(value_str);
  }

  exchange_rate_t rate;
  rate.bank_name = malloc(strlen(bank_name) + 1);
  rate.currency_from = malloc(strlen(currency_from) + 1);
  rate.currency_to = malloc(strlen(currency_to) + 1);
  snprintf(rate.bank_name, strlen(bank_name) + 1, "%s", bank_name);
  snprintf(rate.currency_from, strlen(currency_from) + 1, "%s", currency_from);
  snprintf(rate.currency_to, strlen(currency_to) + 1, "%s", currency_to);
  rate.ratio = ratio;

  return rate;
}

void print(exchange_rate_t rate, FILE* out) {
  fprintf(out, "%10s | %s ---> %s: %lf\n", rate.bank_name, rate.currency_from,
          rate.currency_to, rate.ratio);
}

bool eq_rates(exchange_rate_t rate1, exchange_rate_t rate2) {
  int bn = strcmp(rate1.bank_name, rate2.bank_name);
  int cf = strcmp(rate1.currency_from, rate2.currency_from);
  int ct = strcmp(rate1.currency_to, rate2.currency_to);
  bool r = rate1.ratio == rate2.ratio;
  return bn == 0 && cf == 0 && ct == 0 && r;
}
