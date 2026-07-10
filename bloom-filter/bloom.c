#include "bloom.h"

#include <limits.h>
#include <stdlib.h>
#include <string.h>

typedef unsigned long ulong;

#define ULONG_BITS (sizeof(ulong) * CHAR_BIT)
#define bits2ulong(bits) (((bits) + ULONG_BITS - 1) / ULONG_BITS)

struct bloom_filter_t {
    blfltr_hash_t* hashes;
    size_t hashes_count;

    ulong* bits;
    size_t bits_count;
};

bloom_filter_t* blfltr_create(size_t bits, const blfltr_hash_t hashes[], size_t count) {
    bloom_filter_t* filter; size_t bytes, i;

    if (!bits || !hashes || !count) return NULL;

    bytes = sizeof *filter
        + bits2ulong(bits) * sizeof(ulong)
        + count * sizeof *hashes;
    filter = malloc(bytes);
    if (!filter) return NULL;
    memset(filter, 0, bytes);

    filter->hashes = (void*)(filter + 1);
    filter->hashes_count = count;
    for (i = 0; i < count; i++)
        filter->hashes[i] = hashes[i];

    filter->bits = (void*)(filter->hashes + count);
    filter->bits_count = bits;

    return filter;
}

void blfltr_destroy(bloom_filter_t* filter) { free(filter); }

void blfltr_add(bloom_filter_t* filter, const void* data, size_t size) {
    size_t i; if (!filter) return;
    for (i = 0; i < filter->hashes_count; i++) {
        ulong index = filter->hashes[i](data, size) % filter->bits_count;
        filter->bits[index / ULONG_BITS] |= 1ul << (index % ULONG_BITS);
    }
}

int blfltr_test(const bloom_filter_t* filter, const void* data, size_t size) {
    size_t i; int out = 1;
    if (!filter) return 0;
    for (i = 0; i < filter->hashes_count; i++) {
        ulong index = filter->hashes[i](data, size) % filter->bits_count;
        out = out && ((filter->bits[index / ULONG_BITS] >> (index % ULONG_BITS)) & 1);
    }
    return out;
}