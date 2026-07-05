#include "mt19937.h"

#define CONCAT_(x, y) x ## y
#define CONCAT(x, y) CONCAT_(x, y)

#define MT_W 32
#define MT_N 624
#define MT_M 397
#define MT_R 31

#define MT_U 11
#define MT_S  7
#define MT_T 15
#define MT_L 18

#define MTW_C CONCAT(CONCAT(UINT, MT_W), _C)
typedef CONCAT(CONCAT(uint, MT_W), _t) mtw_t;

#define MT_UPPER (~MTW_C(0) << MT_R)
#define MT_LOWER (~MT_UPPER)

#define MT_A MTW_C(0x9908b0df)
#define MT_F MTW_C(0x6c078965)

#define MT_D MTW_C(0xffffffff)
#define MT_B MTW_C(0x9d2c5680)
#define MT_C MTW_C(0xefc60000)

static void mt19937i_step(mt19937_t* e) {
    mtw_t i;

    for (i = 0; i < MT_N - MT_M; i++) {
        mtw_t x = (e->state[i] & MT_UPPER) | (e->state[i + 1] & MT_LOWER);
        e->state[i] = e->state[i + MT_M] ^ (x >> 1) ^ (x & 1 ? MT_A : 0);
    }

    for (i = MT_N - MT_M; i < MT_N - 1; i++) {
        mtw_t x = (e->state[i] & MT_UPPER) | (e->state[i + 1] & MT_LOWER);
        e->state[i] = e->state[i + (MT_M - MT_N)] ^ (x >> 1) ^ (x & 1 ? MT_A : 0);
    }

    {
        mtw_t x = (e->state[MT_N - 1] & MT_UPPER) | (e->state[0] & MT_LOWER);
        e->state[MT_N - 1] = e->state[MT_M - 1] ^ (x >> 1) ^ (x & 1 ? MT_A : 0);
    }

    e->pos = 0;
}

int mt19937_seed(mt19937_t* e, mtw_t s) {
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

mtw_t mt19937_generate(mt19937_t* e) {
    mtw_t x; if (!e) return 1;
    if (e->pos >= MT_N) mt19937i_step(e);

    x = e->state[e->pos++];
    x ^= (x >> MT_U) & MT_D;
    x ^= (x << MT_S) & MT_B;
    x ^= (x << MT_T) & MT_C;
    x ^= (x >> MT_L);

    return x;
}

int mt19937_discard(mt19937_t* e, mtw_t count) {
    if (!e) return 1;
    while (count > MT_N - e->pos) {
        count -= MT_N - e->pos;
        mt19937i_step(e);
    }
    e->pos += count;
    return 0;
}