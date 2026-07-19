#ifndef MERSENNE_TWISTER_64_BIT_H
#define MERSENNE_TWISTER_64_BIT_H

#include <stddef.h>
#include <stdint.h>

typedef struct mt19937_64_t {
    uint64_t state[312], pos;
} mt19937_64_t;

int mt19937_64_seed(mt19937_64_t* engine, uint64_t seed);
int mt19937_64_seed_array(mt19937_64_t* engine, const uint64_t seeds[], size_t count);
uint64_t mt19937_64_generate(mt19937_64_t* engine);
int mt19937_64_discard(mt19937_64_t* engine, size_t count);

#endif /* MERSENNE_TWISTER_64_BIT_H */