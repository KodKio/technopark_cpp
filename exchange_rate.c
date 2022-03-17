#include "exchange_rate.h"

double get_value(const char* str) {
    char *ptr;
    double value;
    value = strtod(str, &ptr);
    if (*ptr != '\0') {
        return -1;
    }
    return value;
}

Exchange_rate create_rate(FILE* in, FILE* out) {
    char bank_name[100];
    char currency_from[4];
    char currency_to[4];
    double ratio;
    char value_str[100];
    char tmp[100];

    fprintf(out, "Enter bank name:\n");
    fscanf(in, "%99s", bank_name);

    fprintf(out, "Enter the original currency (the name length is no more than 3):\n");
    fscanf(in, "%99s", tmp);
    strncpy(currency_from, tmp, 3);
    currency_from[3] = '\0';

    fprintf(out, "Enter the currency to which the transfer will be made (the length of the name is no more than 3):\n");
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

    Exchange_rate rate;
    rate.bank_name = malloc(strlen(bank_name));
    rate.currency_from = malloc(strlen(currency_from));
    rate.currency_to = malloc(strlen(currency_to));
    strcpy(rate.bank_name, bank_name);
    strcpy(rate.currency_from, currency_from);
    strcpy(rate.currency_to, currency_to);
    rate.ratio = ratio;

    return rate;
}

void print(Exchange_rate rate, FILE* out) {
    fprintf(out, "%10s | %s ---> %s: %lf\n", rate.bank_name, rate.currency_from, rate.currency_to, rate.ratio);
}

bool eq_rates(Exchange_rate rate1, Exchange_rate rate2) {
    int bn = strcmp(rate1.bank_name, rate2.bank_name);
    int cf = strcmp(rate1.currency_from, rate2.currency_from);
    int ct = strcmp(rate1.currency_to, rate2.currency_to);
    bool r = rate1.ratio == rate2.ratio;
    return bn == 0 && cf == 0 && ct == 0 && r;
}