#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#include "parallel.h"

//int PROCESS_COUNT = sysconf(_SC_NPROCESSORS_ONLN);
#define PROCESS_COUNT 8

object_t* get_objects_parallel(FILE* in, size_t* size_objects) {
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

user_t* get_users_parallel(FILE* in, size_t* size_users, const size_t size_objects) {
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

double euclidean_dist_parallel(const user_t first, const user_t second) {
    if (first.size_rates != second.size_rates)
        return -1;
    double sum = 0;
    for (size_t i = 0; i < first.size_rates; ++i) {
        sum += (first.rates[i] - second.rates[i]) * (first.rates[i] - second.rates[i]);
    }
    return sqrt(sum);
}

int users_cmp_parallel(const user_t first, const user_t second) {
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

int not_in_array_parallel(user_t user, user_t* array, size_t size) {
    for (size_t i = 0; i < size; ++i) {
        if (users_cmp_parallel(user, array[i]))
            return 0;
    }
    return 1;
}

int* get_recommendation_final_parallel(user_t how, const user_t* users, size_t size_users) {
    int* result = malloc(sizeof(int) * TOP_SIZE);
    size_t size_result = 0;

    user_t colored[TOP_SIZE - 1];
    size_t size_colored = 0;

    while (size_result < 10) {
        double min_dist = sqrt((MAX_RATE - MIN_RATE) * (MAX_RATE - MIN_RATE) * MAX_COUNT_OBJECTS);
        int min_index = -1;
        for (size_t i = 0; i < size_users; ++i) {
            if (!users_cmp_parallel(how, users[i]) && strcmp(users[i].name, "") != 0) {
                double dist = euclidean_dist_parallel(how, users[i]);
                if (dist < min_dist && not_in_array_parallel(users[i], colored, size_colored) && dist != -1) {
                    min_index = (int)i;
                    min_dist = dist;
                }
            }
        }

        size_colored++;
        user_cpy_parallel(&colored[size_colored], users[min_index]);
        for (int rate = MAX_RATE; rate >= MIN_RATE; rate--) {
            for (size_t i = 0; i < users[min_index].size_rates; ++i)
                if (how.rates[i] == WITHOUT_RATE && users[min_index].rates[i] == rate) {
                    size_result++;
                    result[size_result] = (int)i;
                }
        }
    }

    return result;
}

user_t get_recommendation_user(const user_t who, const user_t* users, const size_t size_users) {
    double min_dist = sqrt((MAX_RATE - MIN_RATE) * (MAX_RATE - MIN_RATE) * MAX_COUNT_OBJECTS);
    int min_index = -1;
    for (size_t j = 0; j < size_users; ++j) {
        if (!users_cmp_parallel(who, users[j])) {
            double dist = euclidean_dist_parallel(who, users[j]);
            if (dist <= min_dist && dist != -1) {
                min_index = (int)j;
                min_dist = dist;
            }
        }
    }

    return users[min_index];
}

int* parallel_get_recommendation(
        user_t who, const user_t* all_users, const size_t size_users, const size_t size_objects) {
    int fd[2];
    if (pipe(fd) < 0) {
        return NULL;
    }

    int n_per_process = (int)size_users / PROCESS_COUNT;
    int users_per_process[PROCESS_COUNT];
    for (int i = 0; i < PROCESS_COUNT - 1; ++i) {
        users_per_process[i] = n_per_process;
    }
    users_per_process[PROCESS_COUNT - 1] = n_per_process + (int)size_users % PROCESS_COUNT;

    user_t** users = malloc(sizeof(user_t*) * PROCESS_COUNT);
    int k = 0;
    for (int i = 0; i < PROCESS_COUNT; ++i) {
        users[i] = malloc(sizeof(user_t) * users_per_process[i]);
        for (int j = k; j < k + users_per_process[i]; j++) {
            user_cpy_parallel(&users[i][j - k], all_users[j]);
        }
        k += users_per_process[i];
    }

    pid_t* pids = malloc(sizeof(pid_t) * PROCESS_COUNT);
    for (int i = 0; i < PROCESS_COUNT; ++i) {
        pids[i] = fork();
        if (pids[i] == -1) {
            close(fd[0]);
            close(fd[1]);
            exit(-1);
        }
        if (pids[i] == 0) {
            close(fd[0]);
            user_t recs = get_recommendation_user(who, users[i], users_per_process[i]);
            write(fd[1], &recs.name, USER_NAME_LENGTH);
            write(fd[1], &recs.surname, USER_SURNAME_LENGTH);
            write(fd[1], &recs.rates, sizeof(int) * size_objects);
            write(fd[1], &recs.size_rates, sizeof(size_t));
            close(fd[1]);
            exit(0);
        }
    }

    int status = 0;
    waitpid(0, &status, 0);

    for (int i = 0; i < PROCESS_COUNT; i++)
        free(users[i]);
    free(users);

    close(fd[1]);

    user_t* users_final = malloc(sizeof(user_t) * PROCESS_COUNT);

    for (size_t j = 0; j < PROCESS_COUNT; ++j) {
        read(fd[0], &users_final[j].name, USER_NAME_LENGTH);
        read(fd[0], &users_final[j].surname, USER_SURNAME_LENGTH);
        read(fd[0], &users_final[j].rates, sizeof(int) * size_objects);
        read(fd[0], &users_final[j].size_rates, sizeof(size_t));
    }

    int* result = get_recommendation_final_parallel(who, users_final, PROCESS_COUNT);

    free(users_final);
    close(fd[0]);

    return result;
}

void user_cpy_parallel(user_t* dest, user_t source) {
    strcpy(dest->name, source.name);
    strcpy(dest->surname, source.surname);
    dest->size_rates =  source.size_rates;
    for (size_t j = 0; j < dest->size_rates; ++j) {
        dest->rates[j] = source.rates[j];
    }
}

object_t* get_objects_by_index_parallel(const int* indexes, const size_t size_indexes, const object_t* objects,
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
