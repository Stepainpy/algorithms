/* Uniform real distribution
 *
 * Transform generated output to half-close range.
 *
 * generate(engine): [0, 2^w)  ->  [min, max)
 *   where w - bit width of output (32 or 64)
 */

#ifndef UNIFORM_REAL_DISTRIBUTION
#define UNIFORM_REAL_DISTRIBUTION

#include <stdint.h>

float  urealdistr_get_32(float  min, float  max, uint32_t (*generate)(void*), void* engine);
double urealdistr_get_64(double min, double max, uint64_t (*generate)(void*), void* engine);

#endif /* UNIFORM_REAL_DISTRIBUTION */