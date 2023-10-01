#include <linux/limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define SESSION_NAME "test"
#define TMUX_MAIN_COMMAND                                                      \
  " \\; kill-pane -a -t 0\\; split-window -h "                                 \
  "lazygit \\; select-pane -t 1 \\; split-window -d \\; select-pane -t 0 \\; "

char *run_command(const char *command) {
  char buffer[128];
  char *result = NULL;
  FILE *fp = popen(command, "r");

  if (fp == NULL) {
    perror("popen");
    return NULL;
  }

  size_t total_size = 0;
  size_t buffer_size = sizeof(buffer);
  result = (char *)malloc(buffer_size + 1);

  if (result == NULL) {
    perror("malloc");
    pclose(fp);
    return NULL;
  };

  while (fgets(buffer, sizeof(buffer), fp) != NULL) {
    size_t len = strlen(buffer);
    if (total_size + len + 1 > buffer_size) {
      buffer_size *= 2;
      result = (char *)realloc(result, buffer_size);
      if (result == NULL) {
        perror("realloc");
        free(result);
        pclose(fp);
        return NULL;
      }
    }
    strcat(result, buffer);
    total_size += len;
  }

  if (pclose(fp) == 0) {
    result[buffer_size] = '\0';
    return result;
  } else {
    fprintf(stderr, "Command execution failed\n");
    return NULL;
  };
}

struct Argument {
  char project_directory[PATH_MAX];
};

int parse_flag_and_values(struct Argument *dest, char *argument_flag,
                          char *argument_value) {
  if (strcmp(argument_flag, "--project-directory") == 0) {
    char absolute_path[PATH_MAX];
    if (realpath(argument_value, absolute_path) == NULL) {
      perror("realpath");
      return 1;
    }
    strcpy(dest->project_directory, absolute_path);
    return 0;
  }
  fprintf(stderr, "Invalid argument\n");
  return 1;
}

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
  //printf("The project directory is: %s\n", arguments.project_directory);
  const char find_flags[] =
      " -name .git -type d -print | sed 's/\\/\\.git//' | fzf";
  char *command =
      (char *)malloc(strlen("find ") + strlen(arguments.project_directory) +
                     strlen(find_flags) + 1);
  strcat(command, "find ");
  strcat(command, arguments.project_directory);
  strcat(command, find_flags);
  char *selected_directory = run_command(&*command);
  if (selected_directory != NULL) {
    //printf("Output: %s\n", selected_directory);
    selected_directory[strlen(selected_directory) - 1] = '\0';
  } else {
    //printf("Error\n");
    return 1;
  }
  free(command);
  char *has_session_output = run_command("tmux has-session -t 'test'");
  char *tmux_start_command;
  if (has_session_output == NULL) {
    char temp[] = "tmux new -t " SESSION_NAME " -c ";
    tmux_start_command =
        (char *)malloc(strlen(temp) + strlen(selected_directory) +
                       strlen(" \\; new-window \"code .\"") + 1);
    strcpy(tmux_start_command, temp);
    strcat(tmux_start_command, selected_directory);
    strcat(tmux_start_command, " \\; new-window \"code .\" ");
  } else {
    char temp[] = "tmux a -t " SESSION_NAME "\\; new-window -c ";
    tmux_start_command =
        (char *)malloc(strlen(temp) + strlen(selected_directory) + 1);
    strcpy(tmux_start_command, temp);
    strcat(tmux_start_command, selected_directory);
  }
  tmux_start_command[strlen(tmux_start_command)] = '\0';
  command = (char *)malloc(strlen(tmux_start_command) +
                           strlen(TMUX_MAIN_COMMAND) + 1);
  strcpy(command, tmux_start_command);
  strcat(command, TMUX_MAIN_COMMAND);
  //printf("%s\n", command);
  run_command(command);
  free(command);
  free(selected_directory);
  free(tmux_start_command);
  return 0;
}
