#include "fyshuffle.h"

#include <string.h>

static void memswp(void* lhs, void* rhs, size_t size) {
    unsigned char *l = lhs, *r = rhs; size_t i;
    for (i = 0; i < size; i++, l++, r++) {
        unsigned char t = *l; *l = *r; *r = t;
    }
}

void fyshuffle(
    void* ptr, size_t count, size_t size,
    size_t (*range)(void* ud, size_t bound),
    void* range_ud
) {
    size_t i; for (i = count; i --> 1;) {
        size_t j = range(range_ud, i);
        if (i != j)
            memswp((char*)ptr + size * i, (char*)ptr + size * j, size);
    }
}

void fyshuffle_copy(
    void* dst, const void* src,
    size_t count, size_t size,
    size_t (*range)(void* ud, size_t bound),
    void* range_ud
) {
    size_t i; for (i = 0; i < count; i++) {
        size_t j = range(range_ud, i);
        memmove((char*)dst + size * i, (      char*)dst + size * j, size);
        memcpy ((char*)dst + size * j, (const char*)src + size * i, size);
    }
}