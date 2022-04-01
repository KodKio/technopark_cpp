#include <gtest/gtest.h>
#include <cmath>

extern "C" {
    #include "structs.h"
    #include "parallel.h"
    #include "simple.h"
}

TEST(GeneralSuite, TestGetObjects) {
    FILE* in = fopen("../../tests/data/films1000.txt", "r");
    size_t size_objects = 0;
    object_t *objects = get_objects(in, &size_objects);

    EXPECT_EQ(size_objects, 1000);
    rewind(in);

    object_t *objects_parallel = get_objects_parallel(in, &size_objects);

    EXPECT_EQ(size_objects, 1000);

    EXPECT_STREQ(objects_parallel[0].name, "La Antena\n");
    EXPECT_STREQ(objects[0].name, "La Antena\n");

    EXPECT_STREQ(objects_parallel[999].name, "Scott Walker: 30 Century Man\n");
    EXPECT_STREQ(objects[999].name, "Scott Walker: 30 Century Man\n");

    free(objects);
    free(objects_parallel);
}

TEST(GeneralSuite, TestGetUsers) {
    FILE* in = fopen("../../tests/data/users1000.txt", "r");
    size_t size_object = 1000;

    size_t size_users = 0;
    user_t *users = get_users(in, &size_users, size_object);

    EXPECT_EQ(size_users, 1500);
    rewind(in);

    user_t *users_parallel = get_users_parallel(in, &size_users, size_object);

    EXPECT_EQ(size_users, 1500);

    EXPECT_STREQ(users_parallel[0].name, "Gene");
    EXPECT_STREQ(users[0].name, "Gene");

    EXPECT_STREQ(users_parallel[999].name, "Dieter");
    EXPECT_STREQ(users[999].name, "Dieter");

    free(users_parallel);
    free(users);
}

TEST(GeneralSuite, TestGetRecsUsers) {
    char filename[] = "../../tests/data/users1000.txt";
    FILE *in = fopen(filename, "r");

    size_t size_objects = 1000;
    size_t size_users = 0;

    user_t* users = get_users_parallel(in, &size_users, size_objects);

    int *res_parallel = parallel_get_recommendation(users[0], users, size_users, size_objects);
    int *res = get_recommendation(users[0], users, size_users);

    for (int i = 0; i < TOP_SIZE; i++) {
        EXPECT_EQ(res[i], res_parallel[i]);
    }

    fclose(in);
    free(res);
    free(res_parallel);
    free(users);
}

TEST(GeneralSuite, TestGetObjectsByIndex) {
    int index[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    const char *expected[] = {
            "La Antena\n",
            "Elementary Particles\n",
            "It's Winter\n",
            "Kirikou and the Wild Beasts\n",
            "Padre Nuestro\n",
            "The Perfume of the Lady in Black\n",
            "Riviera\n",
            "Someone Else's Happiness\n",
            "Young Yakuza\n",
            "The Return\n"
    };

    FILE* in = fopen("../../tests/data/films1000.txt", "r");
    size_t size_objects = 0;

    object_t* objects = get_objects(in, &size_objects);

    object_t* result = get_objects_by_index(index, 10, objects, size_objects);

    for (int i = 0; i < 10; i++) {
        EXPECT_STREQ(expected[i], result[i].name);
    }

    free(result);

    result = get_objects_by_index_parallel(index, 10, objects, size_objects);

    for (int i = 0; i < 10; i++) {
        EXPECT_STREQ(expected[i], result[i].name);
    }

    free(result);

}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}