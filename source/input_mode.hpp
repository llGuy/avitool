#pragma once

enum class input_mode_t { NONE, RECORD, MAKE_AXES, INVALID };

using input_proc_t = void(*)(void *data);

void init_input_modes();
void bind_proc_to_input_mode(input_mode_t mode, input_proc_t proc, void *data);

void push_input_mode(input_mode_t mode);
void pop_input_mode();

input_mode_t get_top_input_mode();

void call_input_proc();
