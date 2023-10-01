#include "./utils.h"
#include <linux/limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

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
