#ifndef MERSENNE_TWISTER_32_BIT_H
#define MERSENNE_TWISTER_32_BIT_H

#if __STDC_VERSION__ >= 199901L
#  include <stdint.h>
#else
#  include <limits.h>
#  if ULONG_MAX > 0xFFFFFFFFul
#    define UINT32_C(lit) lit ## u
typedef unsigned int  uint32_t;
#  else
#    define UINT32_C(lit) lit ## ul
typedef unsigned long uint32_t;
#  endif
#endif

typedef struct mt19937_t {
    uint32_t state[624], pos;
} mt19937_t;

int mt19937_seed(mt19937_t* engine, uint32_t seed);
uint32_t mt19937_generate(mt19937_t* engine);
int mt19937_discard(mt19937_t* engine, uint32_t count);

#endif /* MERSENNE_TWISTER_32_BIT_H */