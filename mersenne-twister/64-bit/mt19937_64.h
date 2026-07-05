#ifndef MERSENNE_TWISTER_64_BIT_H
#define MERSENNE_TWISTER_64_BIT_H

#if __STDC_VERSION__ >= 199901L
#  include <stdint.h>
#else
#  include <limits.h>
#  if ULONG_MAX > 0xFFFFFFFFul
#    define UINT64_C(lit) lit ## ul
typedef unsigned long uint64_t;
#  else
#    define UINT64_C(lit) lit ## ull
typedef unsigned long long uint64_t;
#  endif
#endif

typedef struct mt19937_64_t {
    uint64_t state[312], pos;
} mt19937_64_t;

int mt19937_64_seed(mt19937_64_t* engine, uint64_t seed);
uint64_t mt19937_64_generate(mt19937_64_t* engine);
int mt19937_64_discard(mt19937_64_t* engine, uint64_t count);

#endif /* MERSENNE_TWISTER_64_BIT_H */