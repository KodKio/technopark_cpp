#include <gtest/gtest.h>

extern "C" {
    #include "array_of_rates.h"
    #include "exchange_rate.h"
}

TEST(ExchangeRateSuite, TestGetValue) {
    const int size = 3;
    char* input[size] = {(char*)"10.0", (char*)"notNumber", (char*)"100"};
    double expected[size] = {10.0, -1, 100.0};

    for (int i = 0; i < size; i++) {
        EXPECT_EQ(get_value(input[i]), expected[i]);
    }
}

TEST(ExchangeRateSuite, TestEqRates) {
    Exchange_rate input = {(char*)"sber",(char*)"EUR",(char*)"USD",100 };
    Exchange_rate rate1 = {(char*)"sber",(char*)"EUR",(char*)"USD",100 };

    EXPECT_EQ(eq_rates(input, rate1), true);
}

TEST(ExchangeRateSuite, TestEqRatesNegative) {
    Exchange_rate input = {(char*)"sber",(char*)"EUR",(char*)"USD",100 };
    Exchange_rate rate1 = {(char*)"alf",(char*)"EUR",(char*)"USD",100 };

    EXPECT_EQ(eq_rates(input, rate1), false);
}

TEST(ExchangeRateSuite, TestCreateRateRight) {
    char* in = (char*)"alf\nRUB\nEUR\n100\n";
    FILE* istream = fmemopen(in, strlen(in), "r");

    char* expected = (char*)"Enter bank name:\n"
                            "Enter the original currency (the name length is no more than 3):\n"
                            "Enter the currency to which the transfer will be made (the length of the name is no more than 3):\n"
                            "Enter rate:\n";

    size_t size = strlen(expected) + 1;
    char out[size];
    FILE* ostream = fmemopen(out, size, "w");

    Exchange_rate result = create_rate(istream, ostream);
    Exchange_rate expected_rate = {(char*)"alf",(char*)"RUB",(char*)"EUR",100.0 };

    EXPECT_EQ(eq_rates(result, expected_rate), true);

    fclose(ostream);
    fclose(istream);
}

TEST(ExchangeRateSuite, TestCreateRateWrong) {
    char* in = (char*)"alf\nRUBS\nEURS\none\n100\n";
    FILE* istream = fmemopen(in, strlen(in), "r");

    char* expected = (char*)"Enter bank name:\n"
                            "Enter the original currency (the name length is no more than 3):\n"
                            "Enter the currency to which the transfer will be made (the length of the name is no more "
                            "than 3):\n"
                            "Enter rate:\n"
                            "Try again to enter rate:\n";

    size_t size = strlen(expected) + 1;
    char out[size];
    FILE* ostream = fmemopen(out, size, "w");

    Exchange_rate result = create_rate(istream, ostream);

    Exchange_rate expected_rate = {(char*)"alf",(char*)"RUB",(char*)"EUR",100.0 };

    EXPECT_EQ(eq_rates(result, expected_rate), true);

    fclose(ostream);
    fclose(istream);
}

TEST(ArraySuite, TestInputIntRight) {
    char* in = (char*)"99";
    FILE* istream = fmemopen(in, strlen(in), "r");

    char* out[10];
    FILE* ostream = fmemopen(out,10, "w");

    int expected = 99;
    int result = input_int(0, 110, istream, ostream);

    EXPECT_EQ(expected, result);

    fclose(istream);
    fclose(ostream);
}

TEST(ArraySuite, TestInputIntWrong) {
    char* in = (char*)"190\n99\n";
    FILE* istream = fmemopen(in, strlen(in), "r");

    size_t size = strlen("Try again:\n");
    char out[size];
    FILE* ostream = fmemopen(out, size, "w");

    int expected = 99;
    int result = input_int(0, 110, istream, ostream);

    EXPECT_EQ(expected, result);

    fclose(istream);
    fclose(ostream);
}

TEST(ArraySuite, TestGrowNotNull) {
    Array_of_rates arr = {(Exchange_rate*)malloc(sizeof(Exchange_rate) * 10),10,16 };
    grow(&arr);

    EXPECT_EQ(arr.capacity, 32);

    free(arr.array);
}

TEST(ArraySuite, TestGrowNull) {
    Array_of_rates arr = {nullptr,0,0 };
    grow(&arr);

    EXPECT_EQ(arr.capacity, 1);

    free(arr.array);
}

TEST(ArraySuite, TestAddRateFull) {
    Array_of_rates arr = {(Exchange_rate*)malloc(sizeof(Exchange_rate) * 2),2,2 };
    Exchange_rate first = {(char*)"alf",(char*)"RUB",(char*)"USD",100 };
    Exchange_rate second = {(char*)"sber",(char*)"EUR",(char*)"USD",1.1 };

    arr.array[0] = first;
    arr.array[1] = second;

    Exchange_rate added = {(char*)"tink",(char*)"RUB",(char*)"EUR",100 };

    char* in = (char*)"tink\nRUB\nEUR\n100\n";
    FILE* istream = fmemopen(in, strlen(in), "r");

    char* expected = (char*)"Enter bank name:\n"
                            "Enter the original currency (the name length is no more than 3):\n"
                            "Enter the currency to which the transfer will be made (the length of the name is no more"
                            " than 3):\n"
                            "Enter rate:\n"
                            "Rate added.\n";


    size_t size = strlen(expected) + 1;
    char out[size];
    FILE* ostream = fmemopen(out, size, "w");

    add_rate(&arr, istream, ostream);

    EXPECT_EQ(eq_rates(arr.array[arr.size - 1], added), true);
    EXPECT_EQ(arr.size, 3);
    EXPECT_EQ(arr.capacity, 4);

    free(arr.array);
    fclose(istream);
    fclose(ostream);
}

TEST(ArraySuite, TestDeleteOnIndex) {
    Array_of_rates arr = {(Exchange_rate*)malloc(sizeof(Exchange_rate) * 4),3,4 };
    Exchange_rate first = {(char*)"alf",(char*)"RUB",(char*)"USD",100 };
    Exchange_rate second = {(char*)"sber",(char*)"EUR",(char*)"USD",1.1 };
    Exchange_rate third = {(char*)"tink",(char*)"RUB",(char*)"EUR",100 };

    arr.array[0] = first;
    arr.array[1] = second;
    arr.array[2] = third;

    delete_on_index(&arr, 1);
    EXPECT_EQ(eq_rates(arr.array[1], third), true);
    EXPECT_EQ(arr.size, 2);
    EXPECT_EQ(arr.capacity, 2);

    free(arr.array);
}

TEST(ArraySuite, TestDeleteRateWrongInput) {
    Array_of_rates arr = {(Exchange_rate*)malloc(sizeof(Exchange_rate) * 4),3,4 };
    Exchange_rate first = {(char*)"alf",(char*)"RUB",(char*)"USD",100 };
    Exchange_rate second = {(char*)"sber",(char*)"EUR",(char*)"USD",1.1 };
    Exchange_rate third = {(char*)"tink",(char*)"RUB",(char*)"EUR",100 };

    arr.array[0] = first;
    arr.array[1] = second;
    arr.array[2] = third;

    char* in = (char*)"asd\n1\n";
    FILE* istream = fmemopen(in, strlen(in), "r");

    char* expected = (char*)"List of exchange rates:\n"
                            "  1)       alf | RUB ---> USD: 100.000000\n"
                            "  2)      sber | EUR ---> USD: 1.100000\n"
                            "  3)      tink | RUB ---> EUR: 100.000000\n"
                            "\nChoose rate to delete:\n"
                            "Try again:\n"
                            "Rate deleted.\n";

    size_t size = strlen(expected) + 1;
    char out[size];
    FILE* ostream = fmemopen(out, size, "w");

    EXPECT_EQ(delete_rate(&arr, istream, ostream), SUCCESS);

    free(arr.array);
    fclose(istream);
    fclose(ostream);
}

TEST(ArraySuite, TestDeleteRateRight) {
    Array_of_rates arr = {(Exchange_rate*)malloc(sizeof(Exchange_rate) * 4),3,4 };
    Exchange_rate first = {(char*)"alf",(char*)"RUB",(char*)"USD",100 };
    Exchange_rate second = {(char*)"sber",(char*)"EUR",(char*)"USD",1.1 };
    Exchange_rate third = {(char*)"tink",(char*)"RUB",(char*)"EUR",100 };

    arr.array[0] = first;
    arr.array[1] = second;
    arr.array[2] = third;

    char* in = (char*)"1\n";
    FILE* istream = fmemopen(in, strlen(in), "r");

    char* expected = (char*)"List of exchange rates:\n"
                            "  1)       alf | RUB ---> USD: 100.000000\n"
                            "  2)      sber | EUR ---> USD: 1.100000\n"
                            "  3)      tink | RUB ---> EUR: 100.000000\n"
                            "\nChoose rate to delete:\n"
                            "Rate deleted.\n";

    size_t size = strlen(expected) + 1;
    char out[size];
    FILE* ostream = fmemopen(out, size, "w");

    EXPECT_EQ(delete_rate(&arr, istream, ostream), SUCCESS);

    free(arr.array);
    fclose(istream);
    fclose(ostream);
}

TEST(ArraySuite, TestDeleteRateZeroSize) {
    Array_of_rates arr = {nullptr,0,0};

    char* in = (char*)"1\n";
    FILE* istream = fmemopen(in, strlen(in), "r");

    char* expected = (char*)"Nothing to delete.\n";

    size_t size = strlen(expected) + 1;
    char out[size];
    FILE* ostream = fmemopen(out, size, "w");

    EXPECT_EQ(delete_rate(&arr, istream, ostream), ERROR);

    free(arr.array);
    fclose(istream);
    fclose(ostream);
}

TEST(ArraySuite, TestCopyArray) {
    Array_of_rates source = {(Exchange_rate*)malloc(sizeof(Exchange_rate) * 4),3,4 };
    Exchange_rate first = {(char*)"alf",(char*)"RUB",(char*)"USD",100 };
    Exchange_rate second = {(char*)"sber",(char*)"EUR",(char*)"USD",1.1 };
    Exchange_rate third = {(char*)"tink",(char*)"RUB",(char*)"EUR",100 };

    source.array[0] = first;
    source.array[1] = second;
    source.array[2] = third;

    Array_of_rates dest = {nullptr, 0, 0};

    copy_array_of_rates(&dest, &source);

    EXPECT_EQ(dest.size, source.size);
    EXPECT_EQ(dest.capacity, source.capacity);

    for (int i = 0; i < source.size; i++) {
        EXPECT_EQ(eq_rates(dest.array[i], source.array[i]), true);
    };

    free(dest.array);
    free(source.array);
}

TEST(ArraySuite, TestCheckInArray) {
    Array_of_rates arr = {(Exchange_rate*)malloc(sizeof(Exchange_rate) * 4),3,4 };
    Exchange_rate first = {(char*)"alf",(char*)"RUB",(char*)"USD",100 };
    Exchange_rate second = {(char*)"sber",(char*)"EUR",(char*)"USD",1.1 };
    Exchange_rate third = {(char*)"tink",(char*)"RUB",(char*)"EUR",100 };

    arr.array[0] = first;
    arr.array[1] = second;
    arr.array[2] = third;

    Exchange_rate input1 = {(char*)"sber",(char*)"EUR",(char*)"USD",1.1};
    Exchange_rate input2 = {(char*)"sber",(char*)"EUR",(char*)"USD",99};

    EXPECT_EQ(check_in_array(input1, &arr), true);
    EXPECT_EQ(check_in_array(input2, &arr), false);

    free(arr.array);
}

TEST(ArraySuite, TestFind) {
    Array_of_rates arr = {(Exchange_rate*)malloc(sizeof(Exchange_rate) * 4),4,4 };
    Exchange_rate first = {(char*)"alf",(char*)"RUB",(char*)"USD",100 };
    Exchange_rate second = {(char*)"sber",(char*)"USD",(char*)"EUR",1.1 };
    Exchange_rate third = {(char*)"tink",(char*)"RUB",(char*)"JPY",100 };
    Exchange_rate fourth = {(char*)"alf",(char*)"USD",(char*)"EUR",1};

    arr.array[0] = first;
    arr.array[1] = second;
    arr.array[2] = third;
    arr.array[3] = fourth;

    char* currency_to = (char*)"EUR";
    Array_of_rates result = {nullptr, 0, 0};
    Array_of_rates colored = {(Exchange_rate*)malloc(sizeof(arr.size)), 0, arr.capacity};
    colored.array[0] = arr.array[0];
    colored.size = 1;
    double min = 100000;
    Exchange_rate rate_from = arr.array[0];

    find(&result, &colored, &arr, rate_from, currency_to, 1, &min);

    Array_of_rates expected = {(Exchange_rate*)malloc(sizeof(Exchange_rate) * 2),2,2 };
    second = {(char*)"alf",(char*)"RUB",(char*)"USD",100 };
    first = {(char*)"alf",(char*)"USD",(char*)"EUR",1 };

    expected.array[0] = first;
    expected.array[1] = second;

    EXPECT_EQ(result.size, 2);
    EXPECT_EQ(result.capacity, 2);
    for (int i = 0; i < result.size; i++) {
        EXPECT_EQ(eq_rates(result.array[i], expected.array[i]), true);
    }

    free(arr.array);
    free(expected.array);
    free(colored.array);
    free(result.array);
}

TEST(ArraySuite, TestFindRateWithWay) {
    Array_of_rates arr = {(Exchange_rate*)malloc(sizeof(Exchange_rate) * 4),4,4 };
    Exchange_rate first = {(char*)"alf",(char*)"RUB",(char*)"USD",100 };
    Exchange_rate second = {(char*)"sber",(char*)"USD",(char*)"EUR",1.1 };
    Exchange_rate third = {(char*)"tink",(char*)"RUB",(char*)"JPY",100 };
    Exchange_rate fourth = {(char*)"alf",(char*)"USD",(char*)"EUR",1};

    arr.array[0] = first;
    arr.array[1] = second;
    arr.array[2] = third;
    arr.array[3] = fourth;

    char* in = (char*)"1\n4\n";
    FILE* istream = fmemopen(in, strlen(in), "r");

    char* expected = (char*)"List of currencies from which you can transfer:\n"
                            "  1) RUB\n  2) USD\n  3) RUB\n  4) USD\n\n"
                            "Choose number of currency:\n"
                            "List of currencies that can be converted to:\n"
                            "  1) USD\n  2) EUR\n  3) JPY\n  4) EUR\n\n"
                            "Choose number of currency:\n"
                            "List of exchange rates:\n"
                            "  1)       alf | RUB ---> USD: 100.000000\n"
                            "  2)       alf | USD ---> EUR: 1.000000\n"
                            "Resulting rate = 100.000000\n";

    size_t size = strlen(expected);
    char out[size];
    FILE* ostream = fmemopen(out, size, "w");

    EXPECT_EQ(find_rate(&arr, istream, ostream), SUCCESS);

    fclose(ostream);
    fclose(istream);
    free(arr.array);

}

TEST(ArraySuite, TestFindRateWithoutWay) {
    Array_of_rates arr = {(Exchange_rate*)malloc(sizeof(Exchange_rate) * 4),4,4 };
    Exchange_rate first = {(char*)"alf",(char*)"RUB",(char*)"JPY",100 };
    Exchange_rate second = {(char*)"sber",(char*)"USD",(char*)"RUB",1.1 };
    Exchange_rate third = {(char*)"tink",(char*)"RUB",(char*)"JPY",100 };
    Exchange_rate fourth = {(char*)"alf",(char*)"USD",(char*)"EUR",1};

    arr.array[0] = first;
    arr.array[1] = second;
    arr.array[2] = third;
    arr.array[3] = fourth;

    char* in = (char*)"1\n4\n";
    FILE* istream = fmemopen(in, strlen(in), "r");

    char* expected = (char*)"List of currencies from which you can transfer:\n"
                            "  1) RUB\n  2) USD\n  3) RUB\n  4) USD\n\n"
                            "Choose number of currency:\n"
                            "List of currencies that can be converted to:\n"
                            "  1) JPY\n  2) RUB\n  3) JPY\n  4) EUR\n\n"
                            "Choose number of currency:\n"
                            "No variants to exchange.\n";
    size_t size = strlen(expected) + 1;
    char out[size];
    FILE* ostream = fmemopen(out, size, "w");

    EXPECT_EQ(find_rate(&arr, istream, ostream), ERROR);

    fclose(ostream);
    fclose(istream);
    free(arr.array);
}

TEST(ArraySuite, TestFindRateWithDifferentWays) {
    Array_of_rates arr = {(Exchange_rate*)malloc(sizeof(Exchange_rate) * 4),3,4 };
    Exchange_rate first = {(char*)"alf",(char*)"EUR",(char*)"USD",100 };
    Exchange_rate second = {(char*)"sber",(char*)"USD",(char*)"RUB",1.1 };
    Exchange_rate third = {(char*)"tink",(char*)"EUR",(char*)"USD",90 };

    arr.array[0] = first;
    arr.array[1] = second;
    arr.array[2] = third;

    char* in = (char*)"1\n2\n";
    FILE* istream = fmemopen(in, strlen(in), "r");

    char* expected = (char*)"List of currencies from which you can transfer:\n"
                            "  1) EUR\n  2) USD\n  3) EUR\n\n"
                            "Choose number of currency:\n"
                            "List of currencies that can be converted to:\n"
                            "  1) USD\n  2) RUB\n  3) USD\n\n"
                            "Choose number of currency:\n"
                            "List of exchange rates:\n"
                            "  1)      tink | EUR ---> USD: 90.000000\n"
                            "  2)      sber | USD ---> RUB: 1.100000\n"
                            "Resulting rate = 99.000000\n";
    size_t size = strlen(expected);
    char out[size];
    FILE* ostream = fmemopen(out, size, "w");

    EXPECT_EQ(find_rate(&arr, istream, ostream), SUCCESS);

    fclose(ostream);
    fclose(istream);
    free(arr.array);
}

int main(int argc, char **argv) {
    setlocale(LC_CTYPE, "rus");
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}