#include <SDL.h>
#include "utility.hpp"
#include "ui.hpp"
#include <imgui.h>
#include <glm/glm.hpp>
#include "va_analyser.hpp"
#include "va_render.hpp"
#include "va_frame.hpp"
#include "input_mode.hpp"
#include "controller.hpp"
#include "va_input.hpp"
#include "va_commands.hpp"
#include <GL/glew.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <vector>

namespace va {

static video_t current_video;
static record_settings_t record_settings;
static record_t record;
static axes_t axes_info;
static video_panel_render_info_t panel_ri;

static void s_handle_record_input(void *);
static void s_handle_make_axes_input(void *);

void init() {
    current_video.is_loaded = 0;

    bind_proc_to_input_mode(input_mode_t::RECORD, s_handle_record_input, NULL);
    bind_proc_to_input_mode(input_mode_t::MAKE_AXES, s_handle_make_axes_input, NULL);

    record_settings.xcoord = 1;
    record_settings.ycoord = 1;
    record_settings.btime = 1;
    record_settings.frame_id = 0;

    allocate_video_impl(&current_video);
    current_video.frame = flmalloc<frame_t>();

    record.points.reserve(100);
}

void render_and_tick(ImGuiID master) {
    ImGui::SetNextWindowDockID(master, ImGuiCond_FirstUseEver);
    ImGui::Begin("Video Viewer");

    auto wpos = ImGui::GetWindowPos();
    glm::vec2 window_pos = glm::vec2(wpos.x, wpos.y);
    auto wsize = ImGui::GetWindowSize();
    glm::vec2 window_size = glm::vec2(wsize.x, wsize.y);

    float window_aspect = window_size.x / window_size.y;
    float img_aspect = (float)current_video.frame->width / (float)current_video.frame->height;

    if (current_video.is_loaded) {
        glm::vec2 img_rect = render_frame(window_aspect, img_aspect, window_size, *current_video.frame);
        panel_ri = make_panel_render_info();

        render_recorded_points(panel_ri.min, panel_ri.size, record);
        video_slider(*current_video.frame, current_video.length);

        if (axes_info.is_origin_set && axes_info.is_right_set) {
            render_axes(panel_ri.min, panel_ri.size, axes_info);
        }

        if (ImGui::IsKeyDown(SDL_SCANCODE_LCTRL) && ImGui::IsKeyReleased(SDL_SCANCODE_W)) {
            cmd_goto_video_frame(current_video.frame->frame + 1);
        }

        update_axes(panel_ri.size, &axes_info);

        ImGui::Text("Frame: %d\n", current_video.frame->frame);
    }

    ImGui::End();
}

void handle_settings_checkboxes() {
    ImGui::Checkbox("Frame ID", &record_settings.frame_id);
    ImGui::Checkbox("Time", &record_settings.btime);
    ImGui::Checkbox("X Coordinate", &record_settings.xcoord);
    ImGui::Checkbox("Y Coordinate", &record_settings.ycoord);
}

bool should_block_output_io() {
    return record.is_recording;
}

static void s_handle_record_input(void *) {
    handle_record_input(&current_video, &panel_ri);
}

static void s_handle_make_axes_input(void *) {
    handle_make_axes_input(&axes_info, &panel_ri);
}

}

/*
  Definition of the command procs
 */
int32_t cmd_load_file(const char *file) {
    return va::load_file(file, &va::current_video);
}

int32_t cmd_goto_video_frame(int frame_id) {
    return va::goto_video_frame(frame_id, &va::current_video);
}

int32_t cmd_goto_video_time(int time) {
    return va::goto_video_time(time, &va::current_video);
}

int32_t cmd_begin_record() {
    return va::begin_record(va::current_video.is_loaded);
}

const char *cmd_add_record_point(int x, int y, int max_x, int max_y) {
    return va::add_record_point(
        x, y, max_x, max_y,
        &va::axes_info,
        &va::current_video,
        &va::record,
        va::record_settings);
}

int32_t cmd_end_record() {
    return va::end_record();
}

int32_t cmd_make_axes(int dist) {
    return va::make_axes(dist, &va::axes_info);
}
