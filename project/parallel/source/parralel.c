#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "parallel.h"

#define PROCESS_COUNT 1

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
        if (users_cmp(user, array[i]))
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
            if (!users_cmp(how, users[i]) && strcmp(users[i].name, "") != 0) {
                double dist = euclidean_dist(how, users[i]);
                if (dist < min_dist && not_in_array(users[i], colored, size_colored)) {
                    min_index = i;
                    min_dist = dist;
                }
            }
        }

        size_colored++;
        user_cpy(&colored[size_colored], users[min_index]);
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
    if (!users)
        return NULL;
    printf("%s %zu\n", who.name, size_users);
    size_t size = TOP_SIZE > size_users ? size_users : TOP_SIZE;
    user_t* result = malloc(sizeof(user_t) * size);
    size_t size_result = 0;

    for (int i = 0; i < size; i++) {
        double min_dist = sqrt((MAX_RATE - MIN_RATE) * (MAX_RATE - MIN_RATE) * MAX_COUNT_OBJECTS);
        int min_index = -1;
        for (int j = 0; j < size_users; j++) {
            if (!users_cmp(who, users[j]) && not_in_array(users[j], result, size_result)) {
                double dist = euclidean_dist(who, users[j]);
                if (dist <= min_dist) {
                    min_index = j;
                    min_dist = dist;
                }
            }
        }
        user_cpy(&result[size_result], users[min_index]);

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

    user_t** users = malloc(sizeof(user_t*) * PROCESS_COUNT);
    for (int i = 0; i < PROCESS_COUNT; i++) {
        users[i] = malloc(sizeof(user_t) * users_per_process[i]);
        users[i] = get_k_users(in, users_per_process[i], size_objects);
    }

    fclose(in);

    pid_t* pids = malloc(sizeof(pid_t) * PROCESS_COUNT);
    for (int i = 0; i < PROCESS_COUNT; i++) {
        get_recommendation_users(who, users[i], users_per_process[i]);
        printf("5,");
        pids[i] = fork();
        if (pids[i] == -1) {
            close(fd[0]);
            close(fd[1]);
            exit(-1);
        }
        if (pids[i] == 0) {
            close(fd[0]);
            printf("1\n");
            user_t* recs = get_recommendation_users(who, users[i], users_per_process[i]);
            printf("4,");
            size_t size_recs = (users_per_process[i] < TOP_SIZE ? users_per_process[i] : TOP_SIZE);
            for (int j = 0; j < size_recs; j++) {
                printf("3,");
                write(fd[1], &recs[j], sizeof(user_t));
                printf("2,");
            }
            free(recs);
            close(fd[1]);
            exit(0);
        }
    }

    for (int i = 0; i < PROCESS_COUNT; ++i) {
        int status;
        waitpid(pids[i], &status, 0);
    }

    close(fd[1]);
    size_t size_users_final = 0;
    for (int i = 0; i < PROCESS_COUNT; i++) {
        size_users_final += (users_per_process[i] < TOP_SIZE ? users_per_process[i] : TOP_SIZE);
    }
    user_t* users_final = malloc(sizeof(user_t) * size_users_final);

    for (int j = 0; j < size_users_final; j++) {
        read(fd[0], &users_final[j], sizeof(user_t));
    }

    int* result = get_recommendation(who, users_final, size_users_final);

    free(users_final);
    close(fd[0]);

    int status = 0;
    wait(&status);

    return result;
}

void user_cpy(user_t* dest, const user_t source) {
    strcpy(dest->name, source.name);
    strcpy(dest->surname, source.surname);
    dest->size_rates =  source.size_rates;
    for (int j = 0; j < dest->size_rates; j++) {
        dest->rates[j] = source.rates[j];
    }
};

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