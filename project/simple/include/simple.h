#ifndef TECHNOPARK_CPP_FILM_H
#define TECHNOPARK_CPP_FILM_H

#include "structs.h"

object_t * get_objects(FILE* in, size_t* size_objects);

user_t* get_users(FILE* in, size_t* size_users, const size_t size_objects);

double euclidean_dist(user_t first, user_t second);

int users_cmp(user_t first, user_t second);

int not_in_array(user_t user, user_t* array, size_t size);

int* get_recommendation(user_t how, const user_t* users, size_t size_users);

object_t* get_objects_by_index(const int* indexes, size_t size_indexes, const object_t* objects,
                               size_t size_objects);

#endif //TECHNOPARK_CPP_FILM_H
