#include "ui.hpp"
#include "input_mode.hpp"
#include "controller.hpp"
#include "va_analyser.hpp"
#include "va_render.hpp"
#include "va_input.hpp"

namespace va {

void handle_record_input(video_t *video, video_panel_render_info_t *data) {
    if (ImGui::IsMouseReleased(ImGuiMouseButton_Left) && data->is_focused) {
        ImVec2 min = data->min, size = data->size;

        ImVec2 mouse_pos = ImGui::GetMousePos();
        if (mouse_pos.x > min.x && mouse_pos.x < min.x + size.x &&
            mouse_pos.y > min.y && mouse_pos.y < min.y + size.y) {
            int x = mouse_pos.x - min.x;
            int y = mouse_pos.y - min.y;

            static char cmdbuf[80] = {};
            sprintf(cmdbuf, "add_record_point(%d, %d, %d, %d)", x, y, (int)size.x, (int)size.y);

            begin_controller_cmd(cmdbuf);
            finish_controller_cmd();

            const char *point_str = get_return_value<const char *>();
            add_line_to_editor(point_str);
        }
    }
}

void handle_make_axes_input(axes_t *axes, video_panel_render_info_t *panel) {
    ImVec2 min = panel->min, size = panel->size;
    ImVec2 max = ImVec2(min.x + size.x, min.y + size.y);
    auto *dl = panel->dl;

    if (!axes->is_origin_set) {
        if (ImGui::IsMouseReleased(ImGuiMouseButton_Right)) {
            ImVec2 mouse_pos = ImGui::GetMousePos();
            if (mouse_pos.x > min.x && mouse_pos.x < max.x &&
                mouse_pos.y > min.y && mouse_pos.y < max.y) {
                int x = mouse_pos.x - min.x;
                int y = mouse_pos.y - min.y;

                axes->origin = glm::vec2(x / size.x, y / size.y);
                axes->is_origin_set = 1;
            }
        }
    }
    else if (!axes->is_right_set) {
        ImVec2 mouse_pos = ImGui::GetMousePos();
        ImVec2 clamped = mouse_pos;
        clamped.x = glm::clamp(mouse_pos.x, min.x, max.x);
        clamped.y = glm::clamp(mouse_pos.y, min.y, max.y);

        ImVec2 d = ImVec2();
        dl->AddLine(
            ImVec2(axes->origin.x * size.x + min.x, axes->origin.y * size.y + min.y),
            clamped,
            ImGui::ColorConvertFloat4ToU32(ImVec4(1.0f, 1.0f, 1.0f, 1.0f)));

        if (ImGui::IsMouseReleased(ImGuiMouseButton_Right)) {
            if (mouse_pos.x > min.x && mouse_pos.x < max.x &&
                mouse_pos.y > min.y && mouse_pos.y < max.y) {
                int x = mouse_pos.x - min.x;
                int y = mouse_pos.y - min.y;

                glm::vec2 origin = axes->origin * glm::vec2(size.x, size.y);
                glm::vec2 diff = glm::vec2(x, y) - origin;
                diff /= axes->right_length;

                axes->right = diff / glm::vec2(size.x, size.y);
                axes->is_right_set = 1;
            }
        }
    }
    else {
        ImVec2 d = ImVec2();
        glm::vec2 size_glm = glm::vec2(size.x, size.y);
        glm::vec2 min_glm = glm::vec2(min.x, min.y);
        glm::vec2 origin = axes->origin * size_glm + min_glm;
        glm::vec2 right = origin + axes->right * size_glm * axes->right_length;

        dl->AddLine(
            ImVec2(origin.x, origin.y),
            ImVec2(right.x, right.y),
            ImGui::ColorConvertFloat4ToU32(ImVec4(1.0f, 1.0f, 1.0f, 1.0f)));

        pop_input_mode();
    }
}

}
