#include <stack>
#include <stdint.h>
#include "app_context.hpp"

namespace app {

static std::stack<input_mode_t> modes;

struct input_mode_info_t {
    input_proc_t proc;
    void *data;
};

static input_mode_info_t procs[(uint32_t)input_mode_t::INVALID];

static void s_none(void *data) {
    // Input proc for NONE does nothing
}

void init_input_modes() {
    bind_proc_to_input_mode(input_mode_t::NONE, s_none, NULL);
    modes.push(input_mode_t::NONE);
}

void bind_proc_to_input_mode(input_mode_t mode, input_proc_t proc, void *data) {
    procs[(uint32_t)mode] = {proc, data};
}

void push_input_mode(input_mode_t mode) {
    modes.push(mode);
}

void pop_input_mode() {
    // Make sure NONE stays at the bottom
    if (modes.size() > 1)
        modes.pop();
}

void call_input_proc() {
    input_mode_t mode = modes.top();
    auto *info = &procs[(uint32_t)mode];
    info->proc(info->data);
}

input_mode_t get_top_input_mode() {
    return modes.top();
}

}
