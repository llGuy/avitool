#pragma once

#include <vector>
#include <glm/glm.hpp>

#include <imgui.h>
#include "controller.hpp"

#include "va_record.hpp"

// Video analyser
namespace va {

/*
  Contains OpenCV structures
 */
struct ocv_video_t;
struct frame_t;

struct video_t {
    uint32_t frame_count;
    uint32_t width, height;
    uint32_t fps;
    float length;
    ocv_video_t *impl;
    frame_t *frame;
    bool is_loaded;
};

void init();
void render_and_tick(ImGuiID master);
void handle_settings_checkboxes();
bool should_block_output_io();

}

DECLARE_CMD_PROC(int32_t, cmd_load_file, const char *file);
DECLARE_CMD_PROC(int32_t, cmd_goto_video_frame, int frame_id);
DECLARE_CMD_PROC(int32_t, cmd_goto_video_time, int time); // Milliseconds
DECLARE_CMD_PROC(int32_t, cmd_begin_record);
DECLARE_CMD_PROC(const char *, cmd_add_record_point, int x, int y, int max_x, int max_y);
DECLARE_CMD_PROC(int32_t, cmd_end_record);
DECLARE_CMD_PROC(int32_t, cmd_make_axes, int dist);
