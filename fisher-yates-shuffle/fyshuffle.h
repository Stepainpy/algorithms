#ifndef FISHER_YATES_SHUFFLE_H
#define FISHER_YATES_SHUFFLE_H

#include <stddef.h>

void fyshuffle(
    void* ptr, size_t count, size_t size,
    size_t (*range)(void* ud, size_t bound),
    void* range_ud /* in range [0, bound] */
);

void fyshuffle_copy(
    void* dst, const void* src,
    size_t count, size_t size,
    size_t (*range)(void* ud, size_t bound),
    void* range_ud /* in range [0, bound] */
);

#endif /* FISHER_YATES_SHUFFLE_H */