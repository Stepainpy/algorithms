#include "fysample.h"

#include <string.h>

void fysample(
    void* dst, const void* src,
    size_t count_k, size_t count_n, size_t size,
    size_t (*range)(void* ud, size_t bound),
    void* range_ud
) {
    size_t i; memcpy(dst, src, size);

    for (i = 1; i < count_k; i++) {
        size_t j = range(range_ud, i);
        memmove((char*)dst + size * i, (      char*)dst + size * j, size);
        memcpy ((char*)dst + size * j, (const char*)src + size * i, size);
    }
    for (i = count_k; i < count_n; i++) {
        size_t j = range(range_ud, i);
        if (j < count_k)
            memcpy((char*)dst + size * j, (const char*)src + size * i, size);
    }
}