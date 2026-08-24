#include "levdist.h"

#include <stdlib.h>
#include <string.h>

static void* ldi_alloc(void* p, size_t s, void* u) { return realloc(p, s); (void)u; }

int levenshtein(const char* s1, const char* s2) {
    if (!s1 || !s2) return -1;
    return levenshtein_extra(
        s1, strlen(s1), s2, strlen(s2),
        1, 1, 1, ldi_alloc, NULL);
}

static int ldi_min2(int a, int b) { return a < b ? a : b; }
static int ldi_min3(int a, int b, int c) { return ldi_min2(a, ldi_min2(b, c)); }

int levenshtein_extra(
    const void* lhs, size_t lsz,
    const void* rhs, size_t rsz,
    int ic, int rc, int dc,
    void* (alloc)(void*, size_t, void*), void* ud
) {
    const unsigned char *lptr, *rptr;
    int* coef, out; size_t i, j;

    if (ic < 0 || rc < 0 || dc < 0 ||
        !lhs || !rhs || !alloc) return -1;

    if (lsz == 0) return rsz;
    if (rsz == 0) return lsz;

    if (lsz > rsz) {
        const void* tp; size_t ts;
        tp = lhs; lhs = rhs; rhs = tp;
        ts = lsz; lsz = rsz; rsz = ts;
    }

    lptr = lhs; rptr = rhs;
    coef = alloc(NULL, (lsz + 1) * sizeof *coef, ud);
    if (!coef) return -1;

    coef[0] = 0;
    for (j = 1; j <= lsz; j++)
        coef[j] = coef[j - 1] + ic;

    for (i = 1; i <= rsz; i++) {
        int next_left = coef[0] + dc, new_value;
        for (j = 1; j <= lsz; j++) {
            new_value = ldi_min3(
                coef[j]     + dc,
                next_left   + ic,
                coef[j - 1] + rc
                    * (lptr[j - 1] != rptr[i - 1])
            );
            coef[j - 1] = next_left;
            next_left = new_value;
        }
        coef[j - 1] = new_value;
    }

    out = coef[lsz];
    alloc(coef, 0, ud);
    return out;
}