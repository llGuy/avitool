#pragma once

/*
  Prepares some settings (aesthetic, input configuration, etc...)
 */
void prepare_imgui();
void tick_gui();

void print_to_controller_output(const char *str);

void begin_controller_cmd(const char *cmd, bool print = 1);
void finish_controller_cmd(bool print = 1);

void add_line_to_editor(const char *str);
