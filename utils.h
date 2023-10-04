#ifndef UTILS_H
#define UTILS_H

#include <linux/limits.h>
#define SESSION_NAME "test"
#define TMUX_MAIN_COMMAND                                                      \
  " \\; kill-pane -a -t 0\\; split-window -h "                                 \
  "lazygit \\; select-pane -t 1 \\; split-window -d \\; select-pane -t 0 \\; "

struct Argument {
  char project_directory[PATH_MAX];
};
char *run_command(const char *command);
int parse_flag_and_values(struct Argument *dest, char *argument_flag,
                          char *argument_value);
int does_tmux_exist();
char *get_selected_directory(char *project_directory);
int tmux_create_window(char *selected_directory);

#endif // !UTILS_H
