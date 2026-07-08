#include "urealdist.h"

float urealdist_get_32(float min, float max, uint32_t (*gen)(void*), void* eng) {
    union { float f; uint32_t i; } u;
    u.i = UINT32_C(0x7F) << 23 | gen(eng) >> 9;
    return (u.f - 1.f) * (max - min) + min;
}

double urealdist_get_64(double min, double max, uint64_t (*gen)(void*), void* eng) {
    union { double d; uint64_t i; } u;
    u.i = UINT64_C(0x3FF) << 52 | gen(eng) >> 12;
    return (u.d - 1.0) * (max - min) + min;
}