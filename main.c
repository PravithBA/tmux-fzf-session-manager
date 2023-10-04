#include "./utils.h"
#include <linux/limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
  printf("You have entered %d arguments:\n", argc - 1);
  struct Argument arguments;
  for (int i = 1; i < argc; i++) {
    if (argv[i][0] == '-') {
      char *argument_flag;
      char *argument_value;
      if (argv[i][0] == '-' && argv[i][1] == '-') {
        argument_flag = malloc(strlen(argv[i]) * sizeof(char));
        argument_value = malloc(strlen(argv[i + 1]) * sizeof(char));
        strcpy(argument_flag, argv[i]);
        strcpy(argument_value, argv[i + 1]);
        i++;
      } else {
        if (strlen(argv[i]) == 2) {
          argument_flag = malloc(strlen(argv[i]) * sizeof(char));
          argument_value = malloc(strlen(argv[i + 1]) * sizeof(char));
          strcpy(argument_flag, argv[i]);
          strcpy(argument_value, argv[i + 1]);
          i++;
        } else {
          argument_flag = malloc(2 * sizeof(char));
          argument_value = malloc(strlen(argv[i] + 2) * sizeof(char));
          strncpy(argument_flag, argv[i], 2);
          argument_flag[2] = '\0';
          strcpy(argument_value, argv[i] + 2);
        }
      }
      int parse_error_code =
          parse_flag_and_values(&arguments, &*argument_flag, &*argument_value);
      if (parse_error_code == 1) {
        return parse_error_code;
      }
      free(argument_flag);
      free(argument_value);
    }
  }
  if (strlen(arguments.project_directory) <= 1) {
    fprintf(stderr, "project directory not provided \n");
    return 1;
  }
  char *selected_directory =
      get_selected_directory(arguments.project_directory);
  int tmux_create_status = tmux_create_window(selected_directory);
  free(selected_directory);
  return tmux_create_status;
}
