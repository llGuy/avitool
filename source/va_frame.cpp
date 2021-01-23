#include <GL/glew.h>
#include "va_frame.hpp"

namespace va {

void frame_t::init(uint32_t w, uint32_t h) {
    frame = 0;
    time = 0.0f;
    width = w;
    height = h;

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

void frame_t::update(void *pixels, uint32_t frame_id, float new_time) {
    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGBA,
        width,
        height,
        0,
        GL_BGR,
        GL_UNSIGNED_BYTE,
        pixels);

    frame = frame_id;
    time = new_time;
}

void frame_t::destroy() {
    glDeleteTextures(1, &texture);
}

}
