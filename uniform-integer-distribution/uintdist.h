/* Uniform integer distribution
 *
 * Transform generated output to close range.
 *
 * generate(engine): [0, 2^w)  ->  [min, max]
 *   where w - bit width of output (32 or 64)
 */

#ifndef UNIFORM_INTEGER_DISTRIBUTION
#define UNIFORM_INTEGER_DISTRIBUTION

#if __STDC_VERSION__ >= 199901L
#  include <stdint.h>
#else
#  include <limits.h>
#  if ULONG_MAX > 0xFFFFFFFFul
typedef unsigned int  uint32_t;
typedef unsigned long uint64_t;
#  else
typedef unsigned long uint32_t;
#    if __GNUC__
#      pragma GCC diagnostic push
#      pragma GCC diagnostic ignored "-Wlong-long"
#    endif
typedef unsigned long long uint64_t;
#    if __GNUC__
#      pragma GCC diagnostic pop
#    endif
#  endif
#endif

uint32_t uintdist_get_32(uint32_t min, uint32_t max, uint32_t (*generate)(void*), void* engine);
uint64_t uintdist_get_64(uint64_t min, uint64_t max, uint64_t (*generate)(void*), void* engine);

#endif /* UNIFORM_INTEGER_DISTRIBUTION */