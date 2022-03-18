#ifndef PROJECT_INCLUDE_ARRAY_OF_RATES_H_
#define PROJECT_INCLUDE_ARRAY_OF_RATES_H_

#include "./exchange_rate.h"

typedef struct Array_of_rates {
  Exchange_rate* array;
  size_t size;
  size_t capacity;
} Array_of_rates;

void grow(Array_of_rates* arr_rates);

int input_int(int l, int r, FILE* in, FILE* out);

void add_rate(Array_of_rates* arr_rates, FILE* in, FILE* out);

int delete_rate(Array_of_rates* arr_rates, FILE* in, FILE* out);

void delete_on_index(Array_of_rates* arr_rates, int index);

int find_rate(Array_of_rates* arr_rates, FILE* in, FILE* out);

void show_list_of_currency_from(Array_of_rates* arr_rates, FILE* out);

void show_list_of_currency_to(Array_of_rates* arr_rates, FILE* out);

void find(Array_of_rates* result, Array_of_rates* colored,
          Array_of_rates* arr_rates, Exchange_rate rate_from, char* to,
          double now, double* min);

bool check_in_array(Exchange_rate rate, Array_of_rates* array);

void copy_array_of_rates(Array_of_rates* dest, Array_of_rates* source);

void show_list_reversed(Array_of_rates* arr_rates, FILE* out);

void print_result_ratio(Array_of_rates* arr_rates, FILE* out);

int show_list(Array_of_rates* arr_rates, FILE* out);

#endif  // PROJECT_INCLUDE_ARRAY_OF_RATES_H_
