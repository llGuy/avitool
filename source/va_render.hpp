#pragma once

#include <imgui.h>
#include <glm/glm.hpp>

namespace va {

struct axes_t;
struct frame_t;
struct record_t;

struct video_panel_render_info_t {
    ImDrawList *dl;
    ImVec2 min, size;
    bool is_focused;
};

video_panel_render_info_t make_panel_render_info();

/*
  Returns the pixel rect of the frame image. This will be calculated
  so as to fit in any window size.
 */
glm::vec2 render_frame(
    float window_aspect,
    float img_aspect,
    const glm::vec2 &window_size,
    frame_t &frame);

void render_recorded_points(
    const ImVec2 &min,
    const ImVec2 &size,
    const record_t &record);

void video_slider(
    const frame_t &frame,
    float video_length);

void render_axes(
    const ImVec2 &min,
    const ImVec2 &size,
    const axes_t &axes);

void update_axes(
    const ImVec2 &size,
    struct axes_t *axes);

}
