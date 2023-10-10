#include "./utils.h"
#include <linux/limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

char *run_command(const char *command) {
  char buffer[128] = "";
  char *result = NULL;
  FILE *fp = popen(command, "r");

  if (fp == NULL) {
    perror("popen");
    return NULL;
  }

  size_t total_size = 0;
  size_t buffer_size = sizeof(buffer);
  result = (char *)malloc(buffer_size + 1);
  result[0] = '\0';

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

int does_tmux_exist() {
  char *has_session_output =
      run_command("tmux has-session -t '" SESSION_NAME "'");
  if (has_session_output == NULL) {
    free(has_session_output);
    return 0;
  }
  free(has_session_output);
  return 1;
};

char *get_selected_directory(char *project_directory) {
  const char find_flags[] =
      " -name .git -type d -print | sed 's/\\/\\.git//' | fzf";
  char *command = (char *)malloc(strlen("find ") + strlen(project_directory) +
                                 strlen(find_flags) + 1);
  strcat(command, "find ");
  strcat(command, project_directory);
  strcat(command, find_flags);
  char *selected_directory = run_command(&*command);
  if (selected_directory != NULL) {
    selected_directory[strlen(selected_directory) - 1] = '\0';
  } else {
    return NULL;
  }
  free(command);
  return selected_directory;
};

int is_in_tmux_session() {
  char *output = run_command("echo $TMUX");
  printf("'%s': %ld\n", output, strlen(output));
  if (strlen(output) != 1) {
    free(output);
    return 1;
  }
  free(output);
  return 0;
}

int tmux_create_window(char *selected_directory) {
  int is_tmux_session = is_in_tmux_session();
  printf("%d\n", is_tmux_session);
  int has_session_output = does_tmux_exist();
  char *tmux_start_command;
  if (has_session_output == 0) {
    char temp[] = "tmux new -t " SESSION_NAME " -c ";
    tmux_start_command =
        (char *)malloc(strlen(temp) + strlen(selected_directory) +
                       strlen(" \\; new-window \"code .\"") + 1);
    strcpy(tmux_start_command, temp);
    strcat(tmux_start_command, selected_directory);
    strcat(tmux_start_command, " \\; new-window \"code .\" ");
  } else {
    char temp[] = "tmux a -t " SESSION_NAME "\\; new-window -c ";
    tmux_start_command = (char *)malloc(
        strlen(temp) + strlen(selected_directory) + strlen(" \"code .\" ") + 1);
    strcpy(tmux_start_command, temp);
    strcat(tmux_start_command, selected_directory);
    strcat(tmux_start_command, " \"code .\" ");
  }
  tmux_start_command[strlen(tmux_start_command)] = '\0';
  char *command = (char *)malloc(strlen(tmux_start_command) +
                                 strlen(TMUX_MAIN_COMMAND) + 1);
  strcpy(command, tmux_start_command);
  strcat(command, TMUX_MAIN_COMMAND);
  printf("%s", command);
  // run_command(command);
  free(command);
  free(tmux_start_command);
  return 0;
}
