#ifndef MERSENNE_TWISTER_32_BIT_H
#define MERSENNE_TWISTER_32_BIT_H

#include <stddef.h>
#include <stdint.h>

typedef struct mt19937_t {
    uint32_t state[624], pos;
} mt19937_t;

int mt19937_seed(mt19937_t* engine, uint32_t seed);
uint32_t mt19937_generate(mt19937_t* engine);
int mt19937_discard(mt19937_t* engine, size_t count);

#endif /* MERSENNE_TWISTER_32_BIT_H */