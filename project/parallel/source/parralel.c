#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "parallel.h"

#define PROCESS_COUNT 4

object_t* get_objects(FILE* in, size_t* size_objects) {
    if (fscanf(in, SIZE_FORMAT_STRING, size_objects) != 1) {
        return NULL;
    }
    object_t* objects = malloc(sizeof(object_t) * (*size_objects));
    if (!objects) {
        return NULL;
    }
    for (int i = 0; i < *size_objects; i++) {
        if (fgets(objects[i].name, OBJECT_NAME_LENGTH, in) == NULL) {
            free(objects);
            return NULL;
        }
        objects[i].id = i;
    }
    return objects;
}

user_t* get_k_users(FILE* in, const size_t k, const size_t size_objects) {
    if (in == NULL)
        return NULL;

    user_t *users = malloc(sizeof(user_t) * k);
    if (users == NULL)
        return NULL;

    for (int i = 0; i < k; i++) {
        users[i].size_rates = size_objects;
        if (fscanf(in, USER_INFORMATION_FORMAT_STRING, users[i].name, users[i].surname) != 2) {
            free(users);
            return NULL;
        }
        for (int j = 0; j < users[i].size_rates; j++) {
            if (fscanf(in, RATE_FORMAT_STRING, &users[i].rates[j]) != 1) {
                free(users);
                return NULL;
            }
        }
    }
    return users;
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
    for (int i = 0; i < *size_users; i++) {
        users[i].size_rates = size_objects;
        if (fscanf(in, USER_INFORMATION_FORMAT_STRING, users[i].name, users[i].surname) != 2) {
            free(users);
            return NULL;
        }
        for (int j = 0; j < users[i].size_rates; j++) {
            if (fscanf(in, RATE_FORMAT_STRING, &users[i].rates[j]) != 1) {
                free(users);
                return NULL;
            }
        }
    }
    return users;
}

double euclidean_dist(const user_t first, const user_t second) {
    if (first.size_rates != second.size_rates)
        return -1;
    double sum = 0;
    for (int i = 0; i < first.size_rates; i++) {
        sum += (first.rates[i] - second.rates[i]) * (first.rates[i] - second.rates[i]);
    }
    return sqrt(sum);
}

int users_cmp(const user_t first, const user_t second) {
    if (strcmp(first.name, second.name) != 0)
        return 0;
    if (strcmp(first.surname, second.surname) != 0)
        return 0;
    if (first.size_rates != second.size_rates)
        return 0;
    for (int i = 0; i < first.size_rates; i++)
        if (first.rates[i] != second.rates[i])
            return 0;
    return 1;
}

int not_in_array(user_t user, user_t* array, const size_t size) {
    for (int i = 0; i < size; i++) {
        if (!users_cmp(user, array[i]))
            return 0;
    }
    return 1;
}

int* get_recommendation(const user_t how, const user_t* users, const size_t size_users) {
    int* result = malloc(sizeof(int) * TOP_SIZE);
    size_t size_result = 0;

    user_t colored[TOP_SIZE - 1];
    size_t size_colored = 0;

    while (size_result < 10) {
        double min_dist = sqrt((MAX_RATE - MIN_RATE) * (MAX_RATE - MIN_RATE) * MAX_COUNT_OBJECTS);
        int min_index = -1;
        for (int i = 0; i < size_users; i++) {
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
            for (int i = 0; i < users[min_index].size_rates; i++)
                if (how.rates[i] == WITHOUT_RATE && users[min_index].rates[i] == rate) {
                    size_result++;
                    result[size_result] = i;
                }
        }
    }

    return result;
}

user_t* get_recommendation_users(const user_t who, const user_t* users, const size_t size_users) {
    size_t size = TOP_SIZE > size_users ? size_users : TOP_SIZE;
    user_t* result = malloc(sizeof(user_t) * size);
    size_t size_result = 0;

    for (int i = 0; i < size; i++) {
        double min_dist = sqrt((MAX_RATE - MIN_RATE) * (MAX_RATE - MIN_RATE) * MAX_COUNT_OBJECTS);
        int min_index = -1;
        for (int j = 0; j < size_users; j++) {
            if (!users_cmp(who, users[i])) {
                double dist = euclidean_dist(who, users[j]);
                if (dist <= min_dist && not_in_array(users[j], result, size_result)) {
                    min_index = i;
                    min_dist = dist;
                }
            }
        }
        result[size_result] = users[min_index];
        size_result++;
    }

    return result;
}

int* parallel_get_recommendation(const char* users_filename, user_t who, const size_t size_objects) {
    FILE* in = fopen(users_filename, "r");
    if (in == NULL)
        return NULL;

    size_t size_users;
    if (fscanf(in, SIZE_FORMAT_STRING, &size_users) != 1) {
        fclose(in);
        return NULL;
    }

    int fd[2];
    if (pipe(fd) < 0) {
        fclose(in);
        return NULL;
    }

    int n_per_process = (int)size_users / PROCESS_COUNT;
    int users_per_process[PROCESS_COUNT];
    for (int i = 0; i < PROCESS_COUNT - 1; i++) {
        users_per_process[i] = n_per_process;
    }
    users_per_process[PROCESS_COUNT - 1] = n_per_process + (int)size_users % PROCESS_COUNT;

    pid_t pid;
    for (int i = 0; i < PROCESS_COUNT; i++) {
        user_t* users = get_k_users(in, users_per_process[i], size_objects);
        pid = fork();
        if (pid == -1 || users == NULL) {
            fclose(in);
            close(fd[0]);
            close(fd[1]);
            return NULL;
        }
        if (pid == 0) {
            user_t* user_recommendation = get_recommendation_users(who, users, users_per_process[i]);
            size_t size_user_recommendation = sizeof(user_t) * (TOP_SIZE > size_users ? size_users : TOP_SIZE);
            write(fd[1], user_recommendation, size_user_recommendation);
            close(fd[0]);
            close(fd[1]);
            free(users);
            free(user_recommendation);
            fclose(in);
            exit(0);
        }
        free(users);
    }

    size_t size_users_final = 0;
    for (int i = 0; i < PROCESS_COUNT; i++) {
        size_users_final += users_per_process[i] < TOP_SIZE ? users_per_process[i] : TOP_SIZE;
    }
    user_t* users_final = malloc(sizeof(user_t) * size_users_final);
    int k = 0;

    for (int i = 0; i < PROCESS_COUNT; i++) {
        size_t size = users_per_process[i] < TOP_SIZE ? users_per_process[i] : TOP_SIZE;
        user_t* tmp_users = malloc(sizeof(user_t) * size);
        read(fd[0], tmp_users, sizeof(user_t) * size);
        for (int j = 0; j < size; j++) {
            users_final[k] = tmp_users[j];
            k++;
        }
        free(tmp_users);
    }

    int* result = get_recommendation(who, users_final, size_users_final);

    free(users_final);
    fclose(in);

    return result;
}

object_t* get_objects_by_index(const int* indexes, const size_t size_indexes, const object_t* objects,
                               const size_t size_objects) {
    object_t* result = malloc(sizeof(object_t) * size_indexes);
    for (int i = 0; i < size_indexes; i++) {
        if (indexes[i] >= size_objects) {
            free(result);
            return NULL;
        }
        result[i] = objects[indexes[i]];
    }
    return result;
}