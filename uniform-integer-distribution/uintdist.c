#include "uintdist.h"

uint32_t uintdist_get_32(uint32_t min, uint32_t max, uint32_t (*gen)(void*), void* eng) {
    const uint32_t range = max - min + 1;

    uint64_t product = (uint64_t)gen(eng) * (uint64_t)range;
    uint32_t lower = (uint32_t)product;

    if (lower < range) {
        const uint32_t threshold = -range % range;
        while (lower < threshold) {
            product = (uint64_t)gen(eng) * (uint64_t)range;
            lower = (uint32_t)product;
        }
    }

    return (uint32_t)(product >> 32) + min;
}

typedef struct { uint64_t lower, upper; } uint128_t;

static uint128_t uidi_mul(uint64_t x, uint64_t y) {
    uint128_t out;
    uint32_t xl, xh, yl, yh;
    uint64_t xlyl, xhyl, xlyh, xhyh;
    uint64_t prev;

    xl = (uint32_t)x; xh = (uint32_t)(x >> 32);
    yl = (uint32_t)y; yh = (uint32_t)(y >> 32);

    xlyl = (uint64_t)xl * (uint64_t)yl;
    xhyl = (uint64_t)xh * (uint64_t)yl;
    xlyh = (uint64_t)xl * (uint64_t)yh;
    xhyh = (uint64_t)xh * (uint64_t)yh;

    out.lower = xlyl;
    out.upper = xhyh;

    prev = out.lower;
    if ((out.lower += xhyl << 32) < prev) ++out.upper;
    out.upper += xhyl >> 32;

    prev = out.lower;
    if ((out.lower += xlyh << 32) < prev) ++out.upper;
    out.upper += xlyh >> 32;

    return out;
}

uint64_t uintdist_get_64(uint64_t min, uint64_t max, uint64_t (*gen)(void*), void* eng) {
    const uint64_t range = max - min + 1;

    uint128_t product = uidi_mul(gen(eng), range);

    if (product.lower < range) {
        uint64_t threshold = -range % range;
        while (product.lower < threshold)
            product = uidi_mul(gen(eng), range);
    }

    return product.upper + min;
}