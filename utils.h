#ifndef UTILS_H
#define UTILS_H

#include <linux/limits.h>
#define SESSION_NAME " test "
#define TMUX_MAIN_COMMAND                                                      \
  " \\; split-window -h -c \\\"#{pane_current_path}\\\" "                      \
  "lazygit \\; select-pane -t 1 \\; split-window -d -c "                       \
  "\\\"#{pane_current_path}\\\" \\; select-pane -t 0 \\; "
#define SPLIT_PANE_VERTICAL " split-pane -d -c "
#define SPLIT_PANE_HORIZONTAL " split-pane -h -c "
#define SEMI_COLON_TMUX " \\; "
#define SEMI_COLON " ; "
#define LAZYGIT_COMMAND " lazygit "
#define CODE_COMMAND " \"code .\" "
#define SELECT_PANE " select-pane -t "

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
