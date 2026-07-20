#include "mt19937_64.h"

#define MT_W 64
#define MT_N 312
#define MT_M 156
#define MT_R 31

#define MT_U 29
#define MT_S 17
#define MT_T 37
#define MT_L 43

#define MT_A MTW_C(0xb5026f5aa96619e9)
#define MT_F MTW_C(0x5851f42d4c957f2d)

#define MT_P MTW_C(0x369dea0f31a53f85)
#define MT_Q MTW_C(0x27bb2ee687b0b0fd)

#define MT_D MTW_C(0x5555555555555555)
#define MT_B MTW_C(0x71d67fffeda60000)
#define MT_C MTW_C(0xfff7eee000000000)

#define MT_UPPER (~MTW_C(0) << MT_R)
#define MT_LOWER (~MT_UPPER)

#define _CONCAT(x, y)         x##y
#define  CONCAT(x, y) _CONCAT(x, y)
#define MTW_C CONCAT(CONCAT(UINT, MT_W), _C)
typedef       CONCAT(CONCAT(uint, MT_W), _t) mtw_t;

static void mt19937_64i_step(mt19937_64_t* e) {
    static const mtw_t chA[2] = {0, MT_A};
    mtw_t i;

    for (i = 0; i < MT_N - MT_M; i++) {
        mtw_t x = (e->state[i] & MT_UPPER) | (e->state[i + 1] & MT_LOWER);
        e->state[i] = e->state[i + MT_M] ^ (x >> 1) ^ chA[x & 1];
    }

    for (i = MT_N - MT_M; i < MT_N - 1; i++) {
        mtw_t x = (e->state[i] & MT_UPPER) | (e->state[i + 1] & MT_LOWER);
        e->state[i] = e->state[i + MT_M - MT_N] ^ (x >> 1) ^ chA[x & 1];
    }

    {
        mtw_t x = (e->state[MT_N - 1] & MT_UPPER) | (e->state[0] & MT_LOWER);
        e->state[MT_N - 1] = e->state[MT_M - 1] ^ (x >> 1) ^ chA[x & 1];
    }

    e->pos = 0;
}

int mt19937_64_seed(mt19937_64_t* e, mtw_t s) {
    mtw_t i; if (!e) return 1;

    e->state[0] = s;
    for (i = 1; i < MT_N; i++) {
        mtw_t x = e->state[i - 1];
        x = x ^ (x >> (MT_W - 2));
        x = x * MT_F + i;
        e->state[i] = x;
    }
    e->pos = MT_N;

    return 0;
}

int mt19937_64_seed_array(mt19937_64_t* e, const mtw_t ss[], size_t c) {
    mtw_t i, j; size_t k;
    if (!e || !ss || !c) return 1;
    mt19937_64_seed(e, MTW_C(19650218));

    i = 1; j = 0;
    k = MT_N > c ? MT_N : c;

    while (k --> 0) {
        mtw_t x = e->state[i - 1];
        x = (x ^ (x >> (MT_W - 2))) * MT_P;
        e->state[i] = (e->state[i] ^ x) + ss[j] + j;

        if (++i >= MT_N) { e->state[0] = e->state[MT_N - 1]; i = 1; }
        if (++j >= c) j = 0;
    }

    for (k = MT_N - 1; k --> 0;) {
        mtw_t x = e->state[i - 1];
        x = (x ^ (x >> (MT_W - 2))) * MT_Q;
        e->state[i] = (e->state[i] ^ x) - i;

        if (++i >= MT_N) { e->state[0] = e->state[MT_N - 1]; i = 1; }
    }

    e->state[0] = MTW_C(1) << (MT_W - 1);

    return 0;
}

mtw_t mt19937_64_generate(mt19937_64_t* e) {
    mtw_t x; if (!e) return 1;
    if (e->pos >= MT_N) mt19937_64i_step(e);

    x = e->state[e->pos++];
    x ^= (x >> MT_U) & MT_D;
    x ^= (x << MT_S) & MT_B;
    x ^= (x << MT_T) & MT_C;
    x ^= (x >> MT_L);

    return x;
}

int mt19937_64_discard(mt19937_64_t* e, size_t count) {
    if (!e) return 1;
    while (count > MT_N - e->pos) {
        count -= MT_N - e->pos;
        mt19937_64i_step(e);
    }
    e->pos += count;
    return 0;
}