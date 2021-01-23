#pragma once

#include <imgui.h>

namespace out {

void init();
void add_line_to_editor(const char *str);
void render_and_tick(ImGuiID master);

}
