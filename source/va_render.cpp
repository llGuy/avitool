#include <SDL.h>
#include <imgui.h>
#include <glm/glm.hpp>

#include "va_frame.hpp"
#include "va_record.hpp"
#include "ctrl_cmd.hpp"
#include "ctrl_render.hpp"
#include "va_render.hpp"
#include "va_analyser.hpp"

namespace va {

glm::vec2 render_frame(
    float window_aspect,
    float img_aspect,
    const glm::vec2 &window_size,
    frame_t &frame) {
    glm::vec2 img_res = glm::vec2(0.0f);
    // Calculate pixel size of the image rect
    if (window_aspect > img_aspect) {
        img_res.y = window_size.y;
        img_res.x = img_res.y * img_aspect;
    }
    else {
        img_res.x = window_size.x;
        img_res.y = img_res.x / img_aspect;
    }

    ImGui::Image((void *)frame.texture, ImVec2(img_res.x, img_res.y));

    return img_res;
}

void render_recorded_points(
    const ImVec2 &min,
    const ImVec2 &size,
    const record_t &record) {
    auto *dl = ImGui::GetWindowDrawList();

    // Render recorded points
    for (const auto p : record.points) {
        ImVec2 relative_pos = size;
        relative_pos.x *= p.pos.x;
        relative_pos.y *= p.pos.y;

        ImVec2 position = min;
        position.x += relative_pos.x;
        position.y += relative_pos.y;

        dl->AddCircleFilled(position, 4.0f, ImGui::ColorConvertFloat4ToU32(ImVec4(1.0f, 1.0f, 1.0f, 1.0f)));
    }
}

void video_slider(const frame_t &frame, float video_length) {
    static float progress = frame.time;
    if (ImGui::SliderFloat("Time", &progress, 0.0f, video_length, "%.2fs")) {
        int time_milli = (int)(progress * 1000.0f);

        static char cmdbuf[80] = {};
        sprintf(cmdbuf, "goto_video_time(%d)", time_milli);

        ctrl::begin_cmd(cmdbuf, 0);
        ctrl::end_cmd(0);
    }
}

void render_axes(const ImVec2 &min, const ImVec2 &size, const axes_t &axes) {
    auto *dl = ImGui::GetWindowDrawList();

    glm::vec2 size_glm = glm::vec2(size.x, size.y);

    glm::vec2 origin = axes.origin * size_glm;
    glm::vec2 right = axes.right * size_glm;

    ImVec2 d = ImVec2();
    dl->AddLine(
        ImVec2(origin.x + min.x, origin.y + min.y),
        ImVec2(origin.x + right.x + min.x, origin.y + right.y + min.y),
        ImGui::ColorConvertFloat4ToU32(ImVec4(1.0f, 0.0f, 0.0f, 1.0f)), 3.0f);

    glm::vec2 perp_vec = glm::vec2(right.y, -right.x);

    d = ImVec2();
    dl->AddLine(
        ImVec2(origin.x + min.x, origin.y + min.y),
        ImVec2(origin.x + perp_vec.x + min.x, origin.y + perp_vec.y + min.y),
        ImGui::ColorConvertFloat4ToU32(ImVec4(0.0f, 1.0f, 0.0f, 1.0f)), 3.0f);
}

void update_axes(const ImVec2 &size, axes_t *axes) {
    static glm::vec2 last_mouse_pos = glm::vec2(0.0f);
    
    if (ImGui::IsKeyDown(SDL_SCANCODE_LCTRL) && ImGui::IsKeyDown(SDL_SCANCODE_LSHIFT)) {
        if (ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {
            // Start moving the axes
            auto mouse_pos = ImGui::GetMousePos();
            last_mouse_pos = glm::vec2(mouse_pos.x, mouse_pos.y);
        }
        else if (ImGui::IsMouseDown(ImGuiMouseButton_Right)) {
            auto mouse_pos = ImGui::GetMousePos();
            glm::vec2 size_glm = glm::vec2(size.x, size.y);
            glm::vec2 current_pos = glm::vec2(mouse_pos.x, mouse_pos.y);
            glm::vec2 diff = current_pos - last_mouse_pos;

            glm::vec2 new_origin = axes->origin * size_glm + diff;
            axes->origin = new_origin / size_glm;
            // axes.x2 = axes.x1 + x2_x1_diff;

            last_mouse_pos = current_pos;
        }

        ImGuiIO &io = ImGui::GetIO();
        // ImGui::Text("%f", io.MouseWheel);

        if (io.MouseWheel != 0.0f) {
            float diff = io.MouseWheel;
            glm::vec2 size_glm = glm::vec2(size.x, size.y);

            diff = io.DeltaTime * 100.0f * diff;
            float angle = glm::radians(diff);

            glm::mat2 rot = glm::mat2(
                glm::cos(angle),
                glm::sin(angle),
                -glm::sin(angle),
                glm::cos(angle));

            glm::vec2 new_right = (axes->right) * size_glm;
            new_right = rot * new_right;

            axes->right = new_right / size_glm;
        }
    }
}

video_panel_render_info_t make_panel_render_info() {
    video_panel_render_info_t render_info = {};

    render_info.dl = ImGui::GetWindowDrawList();
    render_info.min = ImGui::GetItemRectMin();
    render_info.size = ImGui::GetItemRectSize();
    render_info.is_focused = ImGui::IsWindowFocused();

    return render_info;
}

}
