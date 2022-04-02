#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "simple.h"

object_t* get_objects(FILE* in, size_t* size_objects) {
    if (in == NULL)
        return NULL;
    if (fscanf(in, SIZE_FORMAT_STRING, size_objects) != 1) {
        return NULL;
    }
    object_t* objects = malloc(sizeof(object_t) * (*size_objects));
    if (!objects) {
        return NULL;
    }
    for (size_t i = 0; i < *size_objects; ++i) {
        if (fgets(objects[i].name, OBJECT_NAME_LENGTH, in) == NULL) {
            free(objects);
            return NULL;
        }
        objects[i].id = i;
    }
    return objects;
}

user_t* get_users(FILE* in, size_t* size_users, const size_t size_objects) {
    if (in == NULL)
        return NULL;
    if (fscanf(in, SIZE_FORMAT_STRING, size_users) != 1) {
        return NULL;
    }
    user_t* users = malloc(sizeof(user_t) * (*size_users));
    if (!users) {
        return NULL;
    }
    for (size_t i = 0; i < *size_users; ++i) {
        users[i].size_rates = size_objects;
        if (fscanf(in, USER_INFORMATION_FORMAT_STRING, users[i].name, users[i].surname) != 2) {
            free(users);
            return NULL;
        }
        for (size_t j = 0; j < users[i].size_rates; ++j) {
            if (fscanf(in, RATE_FORMAT_STRING, &users[i].rates[j]) != 1) {
                free(users);
                return NULL;
            }
        }
    }
    return users;
}

double euclidean_dist(user_t first, user_t second) {
    if (first.size_rates != second.size_rates)
        return -1;
    double sum = 0;
    for (size_t i = 0; i < first.size_rates; ++i) {
        sum += (first.rates[i] - second.rates[i]) * (first.rates[i] - second.rates[i]);
    }
    return sqrt(sum);
}

int users_cmp(user_t first, user_t second) {
    if (strcmp(first.name, second.name) != 0)
        return 0;
    if (strcmp(first.surname, second.surname) != 0)
        return 0;
    if (first.size_rates != second.size_rates)
        return 0;
    for (size_t i = 0; i < first.size_rates; ++i)
        if (first.rates[i] != second.rates[i])
            return 0;
    return 1;
}

int not_in_array(user_t user, user_t* array, size_t size) {
    for (size_t i = 0; i < size; ++i) {
        if (!users_cmp(user, array[i]))
            return 0;
    }
    return 1;
}

int* get_recommendation(user_t how, const user_t* users, size_t size_users) {
    int* result = malloc(sizeof(int) * TOP_SIZE);
    size_t size_result = 0;

    user_t colored[TOP_SIZE - 1];
    size_t size_colored = 0;

    while (size_result < 10) {
        double min_dist = sqrt((MAX_RATE - MIN_RATE) * (MAX_RATE - MIN_RATE) * MAX_COUNT_OBJECTS);
        int min_index = -1;
        for (size_t i = 0; i < size_users; ++i) {
            if (!users_cmp(how, users[i])) {
                double dist = euclidean_dist(how, users[i]);
                if (dist < min_dist && not_in_array(users[i], colored, size_colored)) {
                    min_index = i;
                    min_dist = dist;
                }
            }
        }

        size_colored++;
        colored[size_colored] = users[min_index];
        for (int rate = MAX_RATE; rate >= MIN_RATE; rate--) {
            for (size_t i = 0; i < users[min_index].size_rates; ++i)
                if (how.rates[i] == WITHOUT_RATE && users[min_index].rates[i] == rate) {
                    size_result++;
                    result[size_result] = i;
                }
        }
    }

    return result;
}

object_t* get_objects_by_index(const int* indexes, const size_t size_indexes, const object_t* objects,
                               const size_t size_objects) {
    object_t* result = malloc(sizeof(object_t) * size_indexes);
    for (size_t i = 0; i < size_indexes; ++i) {
        if (indexes[i] >= (int)size_objects) {
            free(result);
            return NULL;
        }
        result[i] = objects[indexes[i]];
    }
    return result;
}
