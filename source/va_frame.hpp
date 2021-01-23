#pragma once

#include <stdint.h>

namespace va {

struct frame_t {
    uint32_t frame;
    float time;
    uint32_t texture;
    uint32_t width, height;

    void init(uint32_t width, uint32_t height);
    void update(void *pixels, uint32_t frame_id, float time);
    void destroy();
};

}
