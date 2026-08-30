#ifndef FISHER_YATES_SAMPLING_H
#define FISHER_YATES_SAMPLING_H

#include <stddef.h>

void fysample(
    void* dst, const void* src,
    size_t count_k, size_t count_n, size_t size,
    size_t (*range)(void* ud, size_t bound),
    void* range_ud /* in range [0, bound] */
);

#endif /* FISHER_YATES_SAMPLING_H */