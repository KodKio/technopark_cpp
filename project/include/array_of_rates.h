#ifndef PROJECT_INCLUDE_ARRAY_OF_RATES_H_
#define PROJECT_INCLUDE_ARRAY_OF_RATES_H_

#include "./exchange_rate.h"

typedef struct array_of_rates {
  exchange_rate_t* array;
  size_t size;
  size_t capacity;
} array_of_rates_t;

void grow(array_of_rates_t* arr_rates);

int input_int(int l, int r, FILE* in, FILE* out);

void add_rate(array_of_rates_t* arr_rates, FILE* in, FILE* out);

int delete_rate(array_of_rates_t* arr_rates, FILE* in, FILE* out);

void delete_on_index(array_of_rates_t* arr_rates, int index);

int find_rate(const array_of_rates_t* arr_rates, FILE* in, FILE* out);

void show_list_of_currency_from(const array_of_rates_t* arr_rates, FILE* out);

void show_list_of_currency_to(const array_of_rates_t* arr_rates, FILE* out);

void find(array_of_rates_t* result, array_of_rates_t* colored,
          const array_of_rates_t* arr_rates, exchange_rate_t rate_from, const char* to,
          double now, double* min);

bool check_in_array(exchange_rate_t rate, const array_of_rates_t* array);

void copy_array_of_rates(array_of_rates_t* dest, const array_of_rates_t* source);

void show_list_reversed(const array_of_rates_t* arr_rates, FILE* out);

void print_result_ratio(const array_of_rates_t* arr_rates, FILE* out);

int show_list(const array_of_rates_t* arr_rates, FILE* out);

#endif  // PROJECT_INCLUDE_ARRAY_OF_RATES_H_
