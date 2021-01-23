#pragma once

#include <vector>
#include <glm/glm.hpp>

namespace va {

/*
  Defines the coordinate space in which all points will be defined
 */
struct axes_t {
    glm::vec2 origin;
    glm::vec2 right;
    float right_length; // This is a parameter that is required to start recording

    union {
        struct {
            uint8_t is_origin_set: 1;
            uint8_t is_right_set: 1;
            uint8_t is_being_made: 1;
        };

        uint8_t flags;
    };
};

struct record_point_t {
    // Goes from 0->1 in X and Y
    glm::vec2 pos;

    // Corresponds to the position in the space that was defined by user
    glm::vec2 axis_space_pos;
};

record_point_t compute_record_point(
    const glm::vec2 &ps_coord,
    const glm::vec2 &ps_rect,
    const axes_t *axes);

struct record_settings_t {
    bool frame_id;
    bool btime;
    bool xcoord;
    bool ycoord;
};

char *record_to_string(
    uint32_t frame_id,
    float time,
    const record_point_t &point,
    const record_settings_t &settings);

struct record_t {
    std::vector<record_point_t> points;
    bool is_recording = 0;
};

}
