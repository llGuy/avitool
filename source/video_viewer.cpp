#include <SDL.h>
#include "ui.hpp"
#include <imgui.h>
#include <glm/glm.hpp>
#include "video_viewer.hpp"
#include "input_mode.hpp"
#include "controller.hpp"

#include <opencv2/core.hpp>     // Basic OpenCV structures (cv::Mat, Scalar)
#include <opencv2/videoio.hpp>

#include <GL/glew.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <vector>

record_settings_t g_record_settings;
static record_t record;

static struct {
    cv::VideoCapture current_capture;
    cv::Size resolution;
    uint32_t frame_count;
    uint32_t fps;
    float length;

    frame_t current_frame;

    bool is_loaded;
    bool is_playing;

    axes_t axes;

    im_video_panel_info_t *panel_info;
    im_axes_info_t *axes_info;
} video;

void init_video_viewer() {
    video.is_loaded = 0;
    record.points.reserve(100);
    video.panel_info = new im_video_panel_info_t;
    video.axes_info = new im_axes_info_t;
    video.axes_info->axes = &video.axes;
    video.axes_info->panel = video.panel_info;

    bind_proc_to_input_mode(input_mode_t::RECORD, im_record_proc, video.panel_info);
    bind_proc_to_input_mode(input_mode_t::MAKE_AXES, im_make_axes_proc, video.axes_info);

    g_record_settings.xcoord = 1;
    g_record_settings.ycoord = 1;
    g_record_settings.btime = 1;
    g_record_settings.frame_id = 0;
}

void destroy_texture() {
    glDeleteTextures(1, &video.current_frame.texture);
}

static void s_fill_texture(uint32_t frame_id) {
    video.current_capture.set(cv::CAP_PROP_POS_FRAMES, frame_id);

    cv::Mat frame;
    video.current_capture.read(frame);

    if (!frame.empty()) {
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RGBA,
            video.resolution.width,
            video.resolution.height,
            0,
            GL_BGR,
            GL_UNSIGNED_BYTE,
            frame.data);
    }
}

static void s_make_texture_out_of_video(uint32_t width, uint32_t height) {
    glGenTextures(1, &video.current_frame.texture);
    glBindTexture(GL_TEXTURE_2D, video.current_frame.texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    s_fill_texture(0);
}

int32_t cmd_load_file(const char *file) {
    char msg[100] = {};

    if (video.current_capture.isOpened()) {
        // Need to make sure that nothing has been opened
        video.current_capture.release();
        destroy_texture();
    }

    video.current_capture.open(file);

    if (!video.current_capture.isOpened()) {
        sprintf(msg, "Failed to load from %s\n", file);
        print_to_controller_output(msg);
        video.is_loaded = 0;

        return 0;
    }
    else {
        sprintf(msg, "Loaded from %s\n", file);
        print_to_controller_output(msg);

        video.resolution = cv::Size(
            (int)video.current_capture.get(cv::CAP_PROP_FRAME_WIDTH),
            (int)video.current_capture.get(cv::CAP_PROP_FRAME_HEIGHT));

        video.current_frame.width = video.resolution.width;
        video.current_frame.height = video.resolution.height;
        video.current_frame.time = 0.0f;
        video.current_frame.frame = 0;

        video.frame_count = video.current_capture.get(cv::CAP_PROP_FRAME_COUNT);

        video.fps = video.current_capture.get(cv::CAP_PROP_FPS);
        video.length = (float)video.frame_count / (float)video.fps;

        sprintf(
            msg,
            "Resolution: %dx%d\nFrame count: %d\nLength: %.1f\nFPS: %d\n",
            video.resolution.width,
            video.resolution.height,
            video.frame_count,
            video.length,
            video.fps);

        print_to_controller_output(msg);

        s_make_texture_out_of_video(video.resolution.width, video.resolution.height);

        video.is_loaded = 1;

        video.current_frame.video_length = video.length;

        return 0;
    }
}

int32_t cmd_goto_video_frame(int frame_id) {
    if (frame_id < video.frame_count) {
        video.current_capture.set(cv::CAP_PROP_POS_FRAMES, frame_id);
        s_fill_texture((uint32_t)frame_id);

        video.current_frame.frame = frame_id;
        video.current_frame.time = (float)frame_id / (float)video.fps;
    }

    return 0;
}

int32_t cmd_goto_video_time(int time) {
    float t_seconds = (float)time / 1000.0f;

    if (t_seconds < video.length) {
        float frames = t_seconds * (float)video.fps;
        s_fill_texture((uint32_t)frames);

        video.current_frame.frame = frames;
        video.current_frame.time = (float)frames / (float)video.fps;
    }

    return 0;
}

int32_t cmd_begin_record() {
    if (video.is_loaded) {
        print_to_controller_output("Began recording...");

        push_input_mode(input_mode_t::RECORD);
    }

    return 0;
}

static glm::vec2 s_start_from_0(const glm::vec2 &v) {
    return glm::vec2(v.x, 1.0f - v.y);
}

const char *cmd_add_record_point(int x, int y, int max_x, int max_y) {
    record_point_t p = {};
    p.pos = p.axis_space_pos = glm::vec2(x, y) / glm::vec2(max_x, max_y);
    p.axis_space_pos = s_start_from_0(p.pos);
    // Calculate the coordinates of the point int he space we defined
    if (video.axes.x2_is_set && video.axes.x1_is_set) {
        float aspect = float(max_x) / float(max_y);

        // We first create a vector space with the pixel distance of max_x
        // Corresponding to 1 in all directions.
        glm::vec2 pos = p.axis_space_pos;
        pos.y /= aspect;

        glm::vec2 axis_origin = s_start_from_0(video.axes.x1);
        glm::vec2 axis_x = (s_start_from_0(video.axes.x2) - axis_origin) / float(video.axes.dist);
        glm::vec2 axis_y = glm::vec2(-axis_x.y, axis_x.x);

        axis_origin.y /= aspect;
        axis_x.y /= aspect;
        axis_y.y /= aspect;

        glm::mat3 t_inv = glm::mat3(1.0f);
        t_inv[2] = glm::vec3(-axis_origin.x, -axis_origin.y, 1.0f);

        glm::mat3 r_inv = glm::mat3(1.0f);
        glm::vec2 norm_x = glm::normalize(axis_x);
        glm::vec2 norm_y = glm::vec2(-norm_x.y, norm_x.x);

        r_inv[0] = glm::vec3(norm_x.x, norm_y.x, 0.0f);
        r_inv[1] = glm::vec3(norm_x.y, norm_y.y, 0.0f);
        r_inv[2] = glm::vec3(0.0f, 0.0f, 1.0f);

        glm::mat3 s_inv = glm::mat3(1.0f);
        float len = glm::length(axis_x);
        s_inv[0][0] = 1.0f / len;
        s_inv[1][1] = 1.0f / len;

        glm::vec2 translated = t_inv * glm::vec3(pos, 1.0f);
        glm::vec2 rotated = r_inv * glm::vec3(translated, 1.0f);
        glm::vec2 scaled = s_inv * glm::vec3(rotated, 1.0f);

        p.axis_space_pos = scaled;
    }

    char *info_str = new char[30];
    memset(info_str, 0, sizeof(char) * 30);

    if (g_record_settings.frame_id) {
        sprintf(info_str, "%d,", video.current_frame.frame);
    }
    if (g_record_settings.btime) {
        uint32_t len = strlen(info_str);
        sprintf(info_str + len, "%f,", video.current_frame.time);
    }
    if (g_record_settings.xcoord) {
        uint32_t len = strlen(info_str);
        sprintf(info_str + len, "%f,", p.axis_space_pos.x);
    }
    if (g_record_settings.ycoord) {
        uint32_t len = strlen(info_str);
        sprintf(info_str + len, "%f,", p.axis_space_pos.y);
    }

    uint32_t len = strlen(info_str);
    if (len > 0) {
        info_str[len - 1] = 0;
    }

    record.points.push_back(p);

    return info_str;
}

int32_t cmd_end_record() {
    if (get_top_input_mode() == input_mode_t::RECORD) {
        print_to_controller_output("Finished recording");
        pop_input_mode();
    }

    return 0;
}

int32_t cmd_make_axes(int dist) {
    video.axes.flags = 0;
    video.axes.is_being_made = 1;
    video.axes.dist = dist;

    push_input_mode(input_mode_t::MAKE_AXES);

    return 0;
}

static glm::vec2 s_render_frame(
    float window_aspect,
    float img_aspect,
    const glm::vec2 &window_size,
    frame_t *frame) {
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

    ImGui::Image((void *)frame->texture, ImVec2(img_res.x, img_res.y));

    return img_res;
}

static void s_render_recorded_points(const ImVec2 &min, const ImVec2 &size) {
    auto *dl = ImGui::GetWindowDrawList();

    // Render recorded points
    for (auto p : record.points) {
        ImVec2 relative_pos = size;
        relative_pos.x *= p.pos.x;
        relative_pos.y *= p.pos.y;

        ImVec2 position = min;
        position.x += relative_pos.x;
        position.y += relative_pos.y;

        dl->AddCircleFilled(position, 4.0f, ImGui::ColorConvertFloat4ToU32(ImVec4(1.0f, 1.0f, 1.0f, 1.0f)));
    }
}

static void s_video_slider() {
    frame_t *frame = &video.current_frame;

    static float progress = frame->time;
    if (ImGui::SliderFloat("Time", &progress, 0.0f, frame->video_length, "%.2fs")) {
        int time_milli = (int)(progress * 1000.0f);

        static char cmdbuf[80] = {};
        sprintf(cmdbuf, "goto_video_time(%d)", time_milli);

        begin_controller_cmd(cmdbuf, 0);
        finish_controller_cmd(0);
    }
}

static void s_render_axes(const ImVec2 &min, const ImVec2 &max, const ImVec2 &size) {
    auto *dl = ImGui::GetWindowDrawList();

    axes_t *axes = &video.axes;
    glm::vec2 size_glm = glm::vec2(size.x, size.y);

    glm::vec2 x1_pixel = axes->x1 * size_glm;
    glm::vec2 x2_pixel = (axes->x2 * size_glm - x1_pixel) / (float)axes->dist + x1_pixel;

    ImVec2 d = ImVec2();
    dl->AddLine(
        ImVec2(x1_pixel.x + min.x, x1_pixel.y + min.y),
        ImVec2(x2_pixel.x + min.x, x2_pixel.y + min.y),
        ImGui::ColorConvertFloat4ToU32(ImVec4(1.0f, 0.0f, 0.0f, 1.0f)), 3.0f);

    glm::vec2 perp_vec = axes->x2 * size_glm - x1_pixel;
    perp_vec = glm::vec2(perp_vec.y, -perp_vec.x);
    x2_pixel = perp_vec / (float)axes->dist + x1_pixel;

    d = ImVec2();
    dl->AddLine(
        ImVec2(x1_pixel.x + min.x, x1_pixel.y + min.y),
        ImVec2(x2_pixel.x + min.x, x2_pixel.y + min.y),
        ImGui::ColorConvertFloat4ToU32(ImVec4(0.0f, 1.0f, 0.0f, 1.0f)), 3.0f);
}

static void s_update_axes(const ImVec2 &size) {
    if (ImGui::IsKeyDown(SDL_SCANCODE_LCTRL) && ImGui::IsKeyDown(SDL_SCANCODE_LSHIFT)) {
        auto &axes = video.axes;
        if (ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {
            // Start moving the axes
            auto mouse_pos = ImGui::GetMousePos();
            axes.current_mouse_pos = glm::vec2(mouse_pos.x, mouse_pos.y);
        }
        else if (ImGui::IsMouseDown(ImGuiMouseButton_Right)) {
            auto mouse_pos = ImGui::GetMousePos();
            glm::vec2 size_glm = glm::vec2(size.x, size.y);
            glm::vec2 current_pos = glm::vec2(mouse_pos.x, mouse_pos.y);
            glm::vec2 diff = current_pos - axes.current_mouse_pos;

            glm::vec2 x2_x1_diff = axes.x2 - axes.x1;

            glm::vec2 new_x1 = axes.x1 * size_glm + diff;
            axes.x1 = new_x1 / size_glm;
            axes.x2 = axes.x1 + x2_x1_diff;

            axes.current_mouse_pos = current_pos;
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

            glm::vec2 x2_x1_diff = (axes.x2 - axes.x1) * size_glm;
            x2_x1_diff = rot * x2_x1_diff;

            axes.x2 = (axes.x1 * size_glm + x2_x1_diff) / size_glm;
        }
    }
}

void render_panel_video_viewer(ImGuiID master) {
    ImGui::SetNextWindowDockID(master, ImGuiCond_FirstUseEver);
    ImGui::Begin("Video Viewer");

    glm::vec2 window_pos, window_size;
    {
        auto wpos = ImGui::GetWindowPos(); window_pos.x = wpos.x, window_pos.y = wpos.y;
        auto wsize = ImGui::GetWindowSize(); window_size.x = wsize.x, window_size.y = wsize.y;
    }

    if (video.is_loaded) {
        video.axes_info->dl = ImGui::GetWindowDrawList();

        frame_t *frame = &video.current_frame;
        float img_aspect = (float)frame->width / (float)frame->height;
        float window_aspect = window_size.x / window_size.y;

        glm::vec2 img_res = s_render_frame(window_aspect, img_aspect, window_size, frame);

        ImVec2 min = ImGui::GetItemRectMin(), max = ImGui::GetItemRectMax(), size = ImGui::GetItemRectSize();
        video.panel_info->min = min;
        video.panel_info->max = max;
        video.panel_info->size = size;

        s_render_recorded_points(min, size);
        s_video_slider();

        if (video.axes.x1_is_set && video.axes.x2_is_set) {
            s_render_axes(min, max, size);
        }

        // These need to be able to happen no matter what the input mode is
        if (ImGui::IsKeyDown(SDL_SCANCODE_LCTRL) && ImGui::IsKeyReleased(SDL_SCANCODE_W)) {
            cmd_goto_video_frame(frame->frame + 1);
        }

        s_update_axes(size);

        ImGui::Text("Frame: %d\n", video.current_frame.frame);
    }

    video.panel_info->is_focused = ImGui::IsWindowFocused();

    ImGui::End();
}

void im_record_proc(void *p) {
    auto *data = (im_video_panel_info_t *)p;

    if (ImGui::IsMouseReleased(ImGuiMouseButton_Left) && video.panel_info->is_focused) {
        ImVec2 min = data->min, max = data->max, size = data->size;

        ImVec2 mouse_pos = ImGui::GetMousePos();
        if (mouse_pos.x > min.x && mouse_pos.x < max.x &&
            mouse_pos.y > min.y && mouse_pos.y < max.y) {
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

void im_make_axes_proc(void *p) {
    auto *info = (im_axes_info_t *)p;
    auto *panel = info->panel;
    auto *axes = info->axes;

    ImVec2 min = panel->min, max = panel->max, size = panel->size;
    auto *dl = info->dl;

    if (!axes->x1_is_set) {
        if (ImGui::IsMouseReleased(ImGuiMouseButton_Right)) {
            ImVec2 mouse_pos = ImGui::GetMousePos();
            if (mouse_pos.x > min.x && mouse_pos.x < max.x &&
                mouse_pos.y > min.y && mouse_pos.y < max.y) {
                int x = mouse_pos.x - min.x;
                int y = mouse_pos.y - min.y;

                axes->x1 = glm::vec2(x / size.x, y / size.y);
                axes->x1_is_set = 1;
            }
        }
    }
    else if (!axes->x2_is_set) {
        ImVec2 mouse_pos = ImGui::GetMousePos();
        ImVec2 clamped = mouse_pos;
        clamped.x = glm::clamp(mouse_pos.x, min.x, max.x);
        clamped.y = glm::clamp(mouse_pos.y, min.y, max.y);

        ImVec2 d = ImVec2();
        dl->AddLine(ImVec2(axes->x1.x * size.x + min.x, axes->x1.y * size.y + min.y), clamped, ImGui::ColorConvertFloat4ToU32(ImVec4(1.0f, 1.0f, 1.0f, 1.0f)));

        if (ImGui::IsMouseReleased(ImGuiMouseButton_Right)) {
            if (mouse_pos.x > min.x && mouse_pos.x < max.x &&
                mouse_pos.y > min.y && mouse_pos.y < max.y) {
                int x = mouse_pos.x - min.x;
                int y = mouse_pos.y - min.y;

                axes->x2 = glm::vec2(x / size.x, y / size.y);
                axes->x2_is_set = 1;
            }
        }
    }
    else {
        ImVec2 d = ImVec2();
        dl->AddLine(
            ImVec2(axes->x1.x * size.x + min.x, axes->x1.y * size.y + min.y),
            ImVec2(axes->x2.x * size.x + min.x, axes->x2.y * size.y + min.y),
            ImGui::ColorConvertFloat4ToU32(ImVec4(1.0f, 1.0f, 1.0f, 1.0f)));

        pop_input_mode();
    }
}

bool should_block_output_io() {
    return record.is_recording;
}
