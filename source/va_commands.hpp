#pragma once

#include <stdint.h>

namespace va {

struct video_t;
struct axes_t;
struct record_t;
struct record_settings_t;

void allocate_video_impl(video_t *video);

/*
  Contains implementations of the commands (which get invoked in va_analyser.cpp)
 */

int32_t load_file(const char *file, video_t *current_video);
int32_t goto_video_frame(int frame_id, video_t *current_video);
int32_t goto_video_time(int time, video_t *current_video);
int32_t begin_record(bool is_video_loaded);
const char *add_record_point(
    int x, int y, int max_x, int max_y,
    axes_t *axes_info,
    video_t *video,
    record_t *record,
    const record_settings_t &settings);
int32_t end_record();
int32_t make_axes(int dist, axes_t *axes);

}
