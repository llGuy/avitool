#include "ui.hpp"
#include "va_frame.hpp"
#include "va_analyser.hpp"
#include "va_commands.hpp"
#include "input_mode.hpp"

#include "utility.hpp"
#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>

namespace va {

struct ocv_video_t {
    cv::VideoCapture vc;
};

static char *line_to_free = NULL;

void allocate_video_impl(video_t *video) {
    video->impl = flmalloc<ocv_video_t>();
    new(&video->impl->vc) cv::VideoCapture();
}

static void s_update_frame_tx(
    uint32_t frame_id,
    ocv_video_t *impl,
    frame_t *current_frame,
    float fps) {
    impl->vc.set(cv::CAP_PROP_POS_FRAMES, frame_id);

    cv::Mat frame_data;
    impl->vc.read(frame_data);

    if (!frame_data.empty()) {
        float time = (float)frame_id / fps;
        current_frame->update(frame_data.data, frame_id, time);
    }

    frame_data.release();
}

int32_t load_file(const char *file, video_t *current_video) {
    static char msg[300] = {};

    cv::VideoCapture *vc = &current_video->impl->vc;

    if (current_video->impl->vc.isOpened()) {
        // Need to make sure that nothing has been opened
        vc->release();
        current_video->frame->destroy();
    }

    vc->open(file);

    if (!vc->isOpened()) {
        sprintf(msg, "Failed to load from %s\n", file);
        print_to_controller_output(msg);
        current_video->is_loaded = 0;

        return 0;
    }
    else {
        sprintf(msg, "Loaded from %s\n", file);
        print_to_controller_output(msg);

        auto res = cv::Size(
            (int)vc->get(cv::CAP_PROP_FRAME_WIDTH),
            (int)vc->get(cv::CAP_PROP_FRAME_HEIGHT));

        current_video->width = res.width;
        current_video->height = res.height;

        current_video->frame_count = vc->get(cv::CAP_PROP_FRAME_COUNT);

        current_video->fps = vc->get(cv::CAP_PROP_FPS);
        current_video->length = (float)current_video->frame_count / (float)current_video->fps;

        sprintf(
            msg,
            "Resolution: %dx%d\nFrame count: %d\nLength: %.1f\nFPS: %d\n",
            res.width,
            res.height,
            current_video->frame_count,
            current_video->length,
            current_video->fps);

        print_to_controller_output(msg);

        current_video->frame->init(res.width, res.height);
        s_update_frame_tx(0, current_video->impl, current_video->frame, current_video->fps);

        current_video->is_loaded = 1;

        return 0;
    }
}

int32_t goto_video_frame(int frame_id, video_t *current_video) {
    if (frame_id < current_video->frame_count) {
        current_video->impl->vc.set(cv::CAP_PROP_POS_FRAMES, frame_id);
        s_update_frame_tx(frame_id, current_video->impl, current_video->frame, current_video->fps);
    }

    return 0;
}

int32_t goto_video_time(int time, video_t *current_video) {
    float t_seconds = (float)time / 1000.0f;

    if (t_seconds < current_video->length) {
        float frames = t_seconds * (float)current_video->fps;
        s_update_frame_tx(frames, current_video->impl, current_video->frame, current_video->fps);
    }

    return 0;
}

int32_t begin_record(bool is_video_loaded) {
    if (is_video_loaded) {
        print_to_controller_output("Began recording...");
        push_input_mode(input_mode_t::RECORD);
    }

    return 0;
}

const char *add_record_point(
    int x, int y, int max_x, int max_y,
    axes_t *axes_info,
    video_t *video,
    record_t *record,
    const record_settings_t &settings) {
    // Move this somewhere else
    if (line_to_free) {
        free(line_to_free);
        line_to_free = NULL;
    }

    record_point_t p = compute_record_point(
        glm::vec2(x, y),
        glm::vec2(max_x, max_y),
        axes_info);

    char *info_str = record_to_string(
        video->frame->frame,
        video->frame->time,
        p,
        settings);
    
    record->points.push_back(p);

    line_to_free = info_str;

    return info_str;
}

int32_t end_record() {
    if (get_top_input_mode() == input_mode_t::RECORD) {
        print_to_controller_output("Finished recording");
        pop_input_mode();
    }

    return 0;
}

int32_t make_axes(int dist, axes_t *axes) {
    axes->flags = 0;
    axes->is_being_made = 1;
    axes->right_length = (float)dist;

    push_input_mode(input_mode_t::MAKE_AXES);

    return 0;
}

}
