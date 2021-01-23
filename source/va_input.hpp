#pragma once

#include "va_render.hpp"

namespace va {

struct video_t;

void handle_record_input(video_t *video, video_panel_render_info_t *data);
void handle_make_axes_input(axes_t *axes, video_panel_render_info_t *data);

}
