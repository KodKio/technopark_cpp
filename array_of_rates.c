#include "array_of_rates.h"

void grow(Array_of_rates* arr_rates) {
    if (arr_rates->size == 0) {
        arr_rates->capacity = 1;
        arr_rates->array = malloc(sizeof(Exchange_rate));
    } else {
        size_t new_capacity = arr_rates->capacity * 2;
        Exchange_rate* new_array = malloc(sizeof(Exchange_rate) * new_capacity);
        for (int i = 0; i < arr_rates->capacity; i++) {
            new_array[i] = arr_rates->array[i];
        }
        free(arr_rates->array);
        arr_rates->array = new_array;
        arr_rates->capacity = new_capacity;
    }
}

int input_int(int l, int r, FILE* in, FILE* out) {
    char value[100];
    fscanf(in, "%99s", value);
    int number = (int)get_value(value);
    while (number == -1 || number <= l || number > r) {
        fprintf(out, "Try again:\n");
        fscanf(in, "%99s", value);
        number = (int)get_value(value);
    }
    return number;
}

void add_rate(Array_of_rates *arr_rates, FILE* in, FILE* out) {
    if (arr_rates->size == arr_rates->capacity) {
        grow(arr_rates);
    }
    Exchange_rate new_rate = create_rate(in, out);
    arr_rates->array[arr_rates->size] = new_rate;
    arr_rates->size++;
    fprintf(out, "Rate added.\n");
}

int delete_rate(Array_of_rates *arr_rates, FILE* in, FILE* out) {
    if (arr_rates->size == 0) {
        return ERROR;
    }

    show_list(arr_rates, out);

    fprintf(out, "\nChoose rate to delete:\n");
    int number = input_int(0, (int)arr_rates->size - 1, in, out);
    number--;

    delete_on_index(arr_rates, number);

    fprintf(out, "Rate deleted.\n");
    return SUCCESS;
}

void delete_on_index(Array_of_rates* arr_rates, int index) {
    size_t new_size = arr_rates->size - 1;
    size_t new_capacity = arr_rates->capacity;
    if (arr_rates->capacity / 2 >= new_size) {
        new_capacity = arr_rates->capacity / 2;
    }

    Exchange_rate *new_arr = malloc(sizeof(Exchange_rate) * new_capacity);

    for (int i = 0; i < index; i++) {
        new_arr[i] = arr_rates->array[i];
    }

    for (int i = index + 1; i < arr_rates->size; i++) {
        new_arr[i - 1] = arr_rates->array[i];
    }

    free(arr_rates->array);
    arr_rates->array = new_arr;
    arr_rates->size = new_size;
    arr_rates->capacity = new_capacity;
}

int find_rate(Array_of_rates *arr_rates, FILE* in, FILE* out) {
    if (arr_rates->size == 0) {
        return ERROR;
    }
    fprintf(out, "List of currencies from which you can transfer:\n");
    show_list_of_currency_from(arr_rates, out);
    fprintf(out, "Choose number of currency:\n");
    int number_currency_from = input_int(0, (int)arr_rates->size, in, out);

    fprintf(out, "List of currencies that can be converted to:\n");
    show_list_of_currency_to(arr_rates, out);
    fprintf(out, "Choose number of currency:\n");
    int number_currency_to = input_int(0, (int)arr_rates->size, in, out);

    char* currency_from = arr_rates->array[number_currency_from - 1].currency_from;
    char* currency_to = arr_rates->array[number_currency_to - 1].currency_to;
    double g_min = 1000000;
    double min = 1000000;

    Array_of_rates result = {NULL, 0, 0};
    for (int i = 0; i < arr_rates->size; i++) {
        if (strcmp(currency_from, arr_rates->array[i].currency_from) == 0) {
            Array_of_rates res = {NULL, 0, 0};

            Array_of_rates colored = {malloc(sizeof(Exchange_rate) * arr_rates->size),
                                      0, arr_rates->capacity};
            colored.array[0] = arr_rates->array[i];
            colored.size = 1;

            find(&res, &colored, arr_rates, arr_rates->array[i], currency_to, arr_rates->array[i].ratio, &min);

            if (min < g_min) {
                if (result.array)
                    free(result.array);
                copy_array_of_rates(&result, &res);
                if (res.array)
                    free(res.array);
                g_min = min;
            }

            free(colored.array);
        }
    }


    if (result.array) {
        show_list_reversed(&result, out);
        print_result_ratio(&result, out);
        free(result.array);
        return SUCCESS;
    } else {
        return ERROR;
    }
}

void find(Array_of_rates *result, Array_of_rates *colored, Array_of_rates *arr_rates, Exchange_rate rate_from,
          char* to, double now, double* min) {
    if (strcmp(rate_from.currency_to, to) == 0) {
        if (now < *min) {
            *min = now;
            if (result->array)
                free(result->array);
            result->size = 1;
            result->capacity = 1;
            result->array = malloc(sizeof(Exchange_rate));
            result->array[0] = rate_from;
        }
        return;
    }
    for (int i = 0; i < arr_rates->size; i++) {
        bool cmp_cur = strcmp(rate_from.currency_to, arr_rates->array[i].currency_from) == 0;
        if (!eq_rates(arr_rates->array[i], rate_from) && cmp_cur && !check_in_array(arr_rates->array[i], colored)) {
            double new_now = now * arr_rates->array[i].ratio;
            double prev_min = *min;

            colored->array[colored->size] = arr_rates->array[i];
            colored->size++;

            find(result, colored, arr_rates, arr_rates->array[i], to, new_now, min);

            colored->size--;

            if (*min != prev_min) {
                if (result->size == result->capacity) {
                    grow(result);
                }
                result->array[result->size] = rate_from;
                result->size += 1;
            }
        }
    }
};

void copy_array_of_rates(Array_of_rates *dest, Array_of_rates *source) {
    dest->size = source->size;
    dest->capacity = source->capacity;
    dest->array = malloc(sizeof(Exchange_rate) * dest->capacity);
    for (int i = 0; i < dest->size; i++) {
        dest->array[i] = source->array[i];
    }
}

bool check_in_array(Exchange_rate rate, Array_of_rates* array) {
    for (int i = 0; i < array->size; i++) {
        if (eq_rates(rate, array->array[i])) {
            return true;
        }
    }
    return false;
}

void show_list_of_currency_from(Array_of_rates* arr_rates, FILE* out) {
    for (int i = 0; i < arr_rates->size; i++) {
        fprintf(out, "%3d) %s\n", i + 1, arr_rates->array[i].currency_from);
    }
    fprintf(out, "\n");
}

void show_list_of_currency_to(Array_of_rates* arr_rates, FILE* out) {
    for (int i = 0; i < arr_rates->size; i++) {
        fprintf(out, "%3d) %s\n", i + 1, arr_rates->array[i].currency_to);
    }
    fprintf(out, "\n");
}


void show_list_reversed(Array_of_rates *arr_rates, FILE* out) {
    fprintf(out, "List of exchange rates:\n");
    for (int i = (int)arr_rates->size - 1, j = 0; i >= 0; i--, j++) {
        fprintf(out, "%3d)", j + 1);
        print(arr_rates->array[i], out);
    }
}

void print_result_ratio(Array_of_rates* arr_rates, FILE* out) {
    double result = 1;
    for (int i = 0; i < arr_rates->size; i++) {
        result *= arr_rates->array[i].ratio;
    }
    fprintf(out, "Resulting rate = %lf\n", result);
}

int show_list(Array_of_rates *arr_rates, FILE* out) {
    if (arr_rates->size == 0) {
        return ERROR;
    }
    fprintf(out, "List of exchange rates:\n");
    for (int i = 0; i < arr_rates->size; i++) {
        fprintf(out, "%3d)", i + 1);
        print(arr_rates->array[i], out);
    }
    return SUCCESS;
}