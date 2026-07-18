/* Uniform integer distribution
 *
 * Transform generated output to close range.
 *
 * generate(engine): [0, 2^w)  ->  [min, max]
 *   where w - bit width of output (32 or 64)
 */

#ifndef UNIFORM_INTEGER_DISTRIBUTION
#define UNIFORM_INTEGER_DISTRIBUTION

#include <stdint.h>

uint32_t uintdistr_get_32(uint32_t min, uint32_t max, uint32_t (*generate)(void*), void* engine);
uint64_t uintdistr_get_64(uint64_t min, uint64_t max, uint64_t (*generate)(void*), void* engine);

#endif /* UNIFORM_INTEGER_DISTRIBUTION */