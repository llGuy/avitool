#pragma once

#include <stdlib.h>
#include <stdint.h>

template <typename T>
T *flmalloc(uint32_t count = 1) {
    return (T *)malloc(sizeof(T) * count);
}

template <typename T>
void flfree(T *p) {
    free(p);
}
