#ifndef TECHNOPARK_CPP_STRUCTS_H
#define TECHNOPARK_CPP_STRUCTS_H

#define USER_NAME_LENGTH 30
#define USER_SURNAME_LENGTH 30
#define OBJECT_NAME_LENGTH 70
#define MAX_COUNT_OBJECTS 16000
#define SIZE_FORMAT_STRING "%zd\n"
#define USER_INFORMATION_FORMAT_STRING "%29s %29s"
#define RATE_FORMAT_STRING "%d "
#define TOP_SIZE 10
#define MIN_RATE 1
#define MAX_RATE 5
#define WITHOUT_RATE 0

typedef struct user {
    char name[USER_NAME_LENGTH];
    char surname[USER_SURNAME_LENGTH];
    int rates[MAX_COUNT_OBJECTS];
    size_t size_rates;
} user_t;

typedef struct object {
    size_t id;
    char name[OBJECT_NAME_LENGTH];
} object_t;

#endif //TECHNOPARK_CPP_STRUCTS_H
