#pragma once

#include <vector>
#include <glm/glm.hpp>

#include <imgui.h>
#include "controller.hpp"

struct record_settings_t {
    bool frame_id;
    bool btime;
    bool xcoord;
    bool ycoord;
};

extern record_settings_t g_record_settings;

struct frame_t {
    uint32_t frame;
    float time;

    uint32_t texture;
    uint32_t width, height;

    float video_length;
};

struct axes_t {
    int dist;

    glm::vec2 x_axis;
    glm::vec2 y_axis;

    glm::vec2 x1, x2;

    union {
        struct {
            uint8_t is_being_made: 1;
            uint8_t x1_is_set: 1;
            uint8_t x2_is_set: 1;
        };
        uint8_t flags;
    };

    glm::vec2 current_mouse_pos;
};

struct im_video_panel_info_t {
    ImVec2 min, max, size;
    bool is_focused;
};

struct im_axes_info_t {
    axes_t *axes;
    im_video_panel_info_t *panel;
    ImDrawList *dl;
};

struct record_point_t {
    glm::vec2 pos; // From 0->1
    glm::vec2 axis_space_pos;
};

struct record_t{
    std::vector<record_point_t> points;
    bool is_recording = 0;
};

void init_video_viewer();
void render_panel_video_viewer(ImGuiID master);
void begin_playing_video();
void stop_playing_video();
void tick_video_player();
bool should_block_output_io();

DECLARE_CMD_PROC(int32_t, cmd_load_file, const char *file);
DECLARE_CMD_PROC(int32_t, cmd_goto_video_frame, int frame_id);
DECLARE_CMD_PROC(int32_t, cmd_goto_video_time, int time); // Milliseconds
DECLARE_CMD_PROC(int32_t, cmd_begin_record);
DECLARE_CMD_PROC(const char *, cmd_add_record_point, int x, int y, int max_x, int max_y);
DECLARE_CMD_PROC(int32_t, cmd_end_record);
DECLARE_CMD_PROC(int32_t, cmd_make_axes, int dist);

// Input mode procs
void im_record_proc(void *data);
void im_make_axes_proc(void *data);
