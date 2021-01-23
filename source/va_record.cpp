#include <stdio.h>
#include "va_record.hpp"

namespace va {

static glm::vec2 s_start_from_0(const glm::vec2 &v) {
    return glm::vec2(v.x, 1.0f - v.y);
}

record_point_t compute_record_point(
    const glm::vec2 &ps_coord,
    const glm::vec2 &ps_rect,
    const axes_t *axes) {
    record_point_t p = {};

    p.pos = p.axis_space_pos = ps_coord / ps_rect;
    p.axis_space_pos = s_start_from_0(p.pos);

    // Calculate the coordinates of the point int he space we defined
    if (axes->is_origin_set && axes->is_right_set) {
        float aspect = float(ps_rect.x) / float(ps_rect.y);

        // We first create a vector space with the pixel distance of max_x
        // Corresponding to 1 in all directions.
        glm::vec2 pos = p.axis_space_pos;
        pos.y /= aspect;

        glm::vec2 axis_origin = s_start_from_0(axes->origin);
        glm::vec2 axis_x = glm::vec2(axes->right.x, axes->right.y * -1.0f);
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

    return p;
}

char *record_to_string(
    uint32_t frame_id,
    float time,
    const record_point_t &point,
    const record_settings_t &settings) {
    char *info_str = (char *)malloc(sizeof(char) * 100);
    memset(info_str, 0, sizeof(char) * 100);

    if (settings.frame_id) {
        sprintf(info_str, "%d,", frame_id);
    }

    if (settings.btime) {
        uint32_t len = strlen(info_str);
        sprintf(info_str + len, "%f,", time);
    }

    if (settings.xcoord) {
        uint32_t len = strlen(info_str);
        sprintf(info_str + len, "%f,", point.axis_space_pos.x);
    }

    if (settings.ycoord) {
        uint32_t len = strlen(info_str);
        sprintf(info_str + len, "%f,", point.axis_space_pos.y);
    }

    uint32_t len = strlen(info_str);

    if (len > 0)
        info_str[len - 1] = 0;

    return info_str;
}

}
