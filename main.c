#include "./array_of_rates.h"

void run(FILE* in, FILE* out);

void do_action(int number, Array_of_rates* arr_rates, FILE* in, FILE* out);

int main() {
  run(stdin, stdout);
  return 0;
}

void run(FILE* in, FILE* out) {
  Array_of_rates arr_rates = {NULL, 0, 0};
  bool flag = true;
  while (flag) {
    fprintf(out,
            "Возможные действия:\n"
            "1. Добавить новый обменный курс\n"
            "2. Удалить обменный курс\n"
            "3. Поиск выгодного обмена\n"
            "4. Показать список курсов\n"
            "5. Выход\n\n");
    fprintf(out, "Введите номер действия:\n");
    int action_number = input_int(0, 5, in, out);
    if (action_number == 5) {
      flag = false;
      for (int i = 0; i < arr_rates.size; i++) {
        free(arr_rates.array[i].bank_name);
        free(arr_rates.array[i].currency_to);
        free(arr_rates.array[i].currency_from);
      }
      if (arr_rates.array) free(arr_rates.array);
    } else {
      do_action(action_number, &arr_rates, in, out);
    }
    fprintf(out, "\n");
  }
  printf("Пока!\n");
}

void do_action(int number, Array_of_rates* arr_rates, FILE* in, FILE* out) {
  switch (number) {
    case 1: {
      add_rate(arr_rates, in, out);
      break;
    }
    case 2: {
      int check = delete_rate(arr_rates, in, out);
      if (check == SUCCESS) {
        fprintf(out, "Rate deleted.\n");
      } else {
        fprintf(out, "Nothing to delete.\n");
      }
      break;
    }
    case 3: {
      if (arr_rates->size > 0) {
        int check = find_rate(arr_rates, in, out);
        if (!check) {
          fprintf(out, "Some thing went wrong.\n");
        }
      } else {
        fprintf(out, "No rates.\n");
      }
      break;
    }
    case 4: {
      int check = show_list(arr_rates, out);
      if (!check) {
        fprintf(out, "Nothing to show.\n");
      }
      break;
    }
    default: {
      break;
    }
  }
}
