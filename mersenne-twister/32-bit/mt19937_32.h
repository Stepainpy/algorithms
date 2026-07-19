#ifndef MERSENNE_TWISTER_32_BIT_H
#define MERSENNE_TWISTER_32_BIT_H

#include <stddef.h>
#include <stdint.h>

typedef struct mt19937_32_t {
    uint32_t state[624], pos;
} mt19937_32_t;

int mt19937_32_seed(mt19937_32_t* engine, uint32_t seed);
int mt19937_32_seed_array(mt19937_32_t* engine, const uint32_t seeds[], size_t count);
uint32_t mt19937_32_generate(mt19937_32_t* engine);
int mt19937_32_discard(mt19937_32_t* engine, size_t count);

#endif /* MERSENNE_TWISTER_32_BIT_H */