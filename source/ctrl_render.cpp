#include <imgui.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "ctrl_cmd.hpp"
#include "ctrl_render.hpp"

namespace ctrl {

constexpr uint32_t MAX_CHARS_IN_CONTROLLER_OUTPUT = 10000;

static struct {
    char cmdbuf[1000] = {};

    uint32_t char_pointer = 0;
    char *cmd_window = NULL;
    const char *prompt = "> ";
} controller_panel;

void init_controller_display() {
    controller_panel.cmd_window = new char[MAX_CHARS_IN_CONTROLLER_OUTPUT];
    memset(controller_panel.cmd_window, 0, sizeof(char) * MAX_CHARS_IN_CONTROLLER_OUTPUT);

    memcpy(controller_panel.cmd_window, controller_panel.prompt, strlen(controller_panel.prompt));
    controller_panel.char_pointer += strlen(controller_panel.prompt);
}

void log(const char *str) {
    uint32_t len = strlen(str);

    if (len + controller_panel.char_pointer < MAX_CHARS_IN_CONTROLLER_OUTPUT) {
        memcpy(controller_panel.cmd_window + controller_panel.char_pointer, str, strlen(str));
        controller_panel.char_pointer += len;
    }
    else {
        controller_panel.char_pointer = 0;
        memset(controller_panel.cmd_window, 0, sizeof(char) * MAX_CHARS_IN_CONTROLLER_OUTPUT);
    }
}

void begin_cmd(const char *cmd, bool print) {
    if (print) {
        log(cmd);
        log("\n");
    }

    submit_cmdstr(cmd);
}

void end_cmd(bool print) {
    if (print)
        log("\n> ");
}

void render_and_tick(ImGuiID master) {
    ImGui::SetNextWindowDockID(master, ImGuiCond_FirstUseEver);
    ImGui::Begin("Controller");

    if (ImGui::InputText("Command", controller_panel.cmdbuf, 1000, ImGuiInputTextFlags_EnterReturnsTrue)) {
        begin_cmd(controller_panel.cmdbuf);
        memset(controller_panel.cmdbuf, 0, sizeof(controller_panel.cmdbuf));
        end_cmd();
    }

    ImGui::BeginChild("Controller_OutputWindow", {0, 0}, false, ImGuiWindowFlags_AlwaysVerticalScrollbar | ImGuiWindowFlags_AlwaysHorizontalScrollbar);
    ImGui::TextUnformatted(controller_panel.cmd_window);
    ImGui::EndChild();

    ImGui::End();
}

}
