/* Mersenne Twister tests
 *
 * Homepage: https://math.sci.hiroshima-u.ac.jp/m-mat/MT/emt.html
 *
 * Test vector from:
 * 32 bit: https://math.sci.hiroshima-u.ac.jp/m-mat/MT/MT2002/emt19937ar.html
 * 64 bit: https://math.sci.hiroshima-u.ac.jp/m-mat/MT/emt64.html
 */

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>

#include "32-bit/mt19937_32.h"
#include "64-bit/mt19937_64.h"

#define error_msg(strlit) do { \
    fputs(strlit"\n", stderr); \
    goto cleanup; \
} while (0)

static mt19937_32_t eng32[1];
static mt19937_64_t eng64[1];

int test_discard(void) {
    uint32_t x32 = 0, e32 = 1, seed32;
    uint64_t x64 = 0, e64 = 1, seed64;
    size_t skip32, skip64;
    FILE* tvfile32 = NULL;
    FILE* tvfile64 = NULL;

    tvfile32 = fopen("32-bit/test-discard-vector.txt", "r");
    if (!tvfile32) error_msg("Couldn't open 32 bit test discard vector");
    tvfile64 = fopen("64-bit/test-discard-vector.txt", "r");
    if (!tvfile64) error_msg("Couldn't open 64 bit test discard vector");

    if (!fscanf(tvfile32, "%"SCNu32, &seed32)) error_msg("Couldn't read 32 bit seed");
    if (!fscanf(tvfile64, "%"SCNu64, &seed64)) error_msg("Couldn't read 64 bit seed");

    if (mt19937_32_seed(eng32, seed32)) goto cleanup;
    if (mt19937_64_seed(eng64, seed64)) goto cleanup;

    if (!fscanf(tvfile32, "%"SCNu64, &skip32)) error_msg("Couldn't read 32 bit discard");
    if (!fscanf(tvfile64, "%"SCNu64, &skip64)) error_msg("Couldn't read 64 bit discard");

    if (mt19937_32_discard(eng32, skip32)) goto cleanup;
    if (mt19937_64_discard(eng64, skip64)) goto cleanup;

    if (!fscanf(tvfile32, "%"SCNu32, &e32)) error_msg("Couldn't read 32 bit expected value");
    if (!fscanf(tvfile64, "%"SCNu64, &e64)) error_msg("Couldn't read 64 bit expected value");

    x32 = mt19937_32_generate(eng32);
    x64 = mt19937_64_generate(eng64);

    if (x32 != e32) fprintf(stderr,
        "Generate %"PRIu64"th 32 bit: expect %"PRIu32", but got %"PRIu32"\n", skip32 + 1, e32, x32);
    if (x64 != e64) fprintf(stderr,
        "Generate %"PRIu64"th 64 bit: expect %"PRIu64", but got %"PRIu64"\n", skip64 + 1, e64, x64);

cleanup:
    if (tvfile32) fclose(tvfile32);
    if (tvfile64) fclose(tvfile64);
    return x32 != e32 || x64 != e64;
}

int test_seed_array(void) {
    int sdsz32, sdsz64, i;
    int n32 = 0, p32 = -1, n64 = 0, p64 = -1;
    uint32_t* seeds32 = NULL, e32;
    uint64_t* seeds64 = NULL, e64;
    FILE* tvfile32 = NULL;
    FILE* tvfile64 = NULL;

    tvfile32 = fopen("32-bit/test-seed-array-vector.txt", "r");
    if (!tvfile32) error_msg("Couldn't open 32 bit test seed array vector");
    tvfile64 = fopen("64-bit/test-seed-array-vector.txt", "r");
    if (!tvfile64) error_msg("Couldn't open 64 bit test seed array vector");

    if (!fscanf(tvfile32, "%i", &sdsz32)) error_msg("Couldn't read count of 32 bit seeds");
    if (!fscanf(tvfile64, "%i", &sdsz64)) error_msg("Couldn't read count of 64 bit seeds");

    seeds32 = malloc(sizeof *seeds32 * sdsz32);
    if (!seeds32) error_msg("Couldn't allocate memory for 32 bit seeds");
    seeds64 = malloc(sizeof *seeds64 * sdsz64);
    if (!seeds64) error_msg("Couldn't allocate memory for 64 bit seeds");

    for (i = 0; i < sdsz32; i++)
        if (!fscanf(tvfile32, "%"SCNu32, seeds32 + i)) error_msg("Couldn't read 32 bit seed");
    for (i = 0; i < sdsz64; i++)
        if (!fscanf(tvfile64, "%"SCNu64, seeds64 + i)) error_msg("Couldn't read 64 bit seed");

    if (mt19937_32_seed_array(eng32, seeds32, sdsz32)) goto cleanup;
    if (mt19937_64_seed_array(eng64, seeds64, sdsz64)) goto cleanup;

    for (i = 1, n32 = p32 = 0; fscanf(tvfile32, "%"SCNu32, &e32) == 1; i++) {
        uint32_t x32 = mt19937_32_generate(eng32);
        if (x32 != e32) fprintf(stderr,
            "Generate #%i 32 bit: expect %"PRIu32", but got %"PRIu32"\n", i, e32, x32);
        n32++; p32 += x32 == e32;
        if (n32 - p32 >= 10) { fputs("Too many errors\n", stderr); break; }
    }

    for (i = 1, n64 = p64 = 0; fscanf(tvfile64, "%"SCNu64, &e64) == 1; i++) {
        uint64_t x64 = mt19937_64_generate(eng64);
        if (x64 != e64) fprintf(stderr,
            "Generate #%i 64 bit: expect %"PRIu64", but got %"PRIu64"\n", i, e64, x64);
        n64++; p64 += x64 == e64;
        if (n64 - p64 >= 10) { fputs("Too many errors\n", stderr); break; }
    }

cleanup:
    if (tvfile32) fclose(tvfile32);
    if (tvfile64) fclose(tvfile64);
    free(seeds32);
    free(seeds64);
    return p32 != n32 || p64 != n64 || !n32 || !n64;
}

int main(void) {
    int ret = test_discard() || test_seed_array();
    if (!ret) puts("All tests passed");
    return ret;
}
