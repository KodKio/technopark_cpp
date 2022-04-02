#include <stdlib.h>
#include "stdio.h"

#include "simple.h"

int run(const char* films_file_name, const char* users_file_name);

int main() {
    run("../tests/data/films1000.txt", "../tests/data/users1000.txt");
    return 0;
}

int run(const char* films_file_name, const char* users_file_name) {
    FILE* objects_file = fopen(films_file_name, "r");
    FILE* users_file = fopen(users_file_name, "r");
    if (objects_file == NULL) {
        printf("Can't read file with objects\n");
        return 1;
    }
    if (users_file == NULL) {
        printf("Can't read file with users\n");
        return 1;
    }
    size_t size_objects = 0;
    size_t size_users = 0;
    object_t* objects = get_objects(objects_file, &size_objects);
    user_t* users = get_users(users_file, &size_users, size_objects);
    int* indexes;
    object_t* result;
    int user_index;
    printf("Enter number of user([0 .. %zu]):\n", size_users);
    if (scanf("%d", &user_index) != 1) {
        printf("Read error");
        return 1;
    }
    if (user_index < 0 || user_index >= (int)size_users) {
        printf("No such user");
        return 1;
    }
    indexes = get_recommendation(users[user_index], users, size_users);
    printf("%s %s:\n", users[user_index].name, users[user_index].surname);
    free(users);
    result = get_objects_by_index(indexes, TOP_SIZE, objects, size_objects);
    free(indexes);
    for (int i = 0; i < TOP_SIZE; i++) {
        printf("%s", result[i].name);
    }
    free(objects);
    fclose(objects_file);
    fclose(users_file);
    free(result);
    return 0;
}
