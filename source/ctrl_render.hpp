#pragma once

#include <imgui.h>

namespace ctrl {

void init_controller_display();
void log(const char *str);
void begin_cmd(const char *cmd, bool should_print = 1);
void end_cmd(bool should_print = 1);
void render_and_tick(ImGuiID master);

}
