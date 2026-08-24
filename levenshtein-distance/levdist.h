#ifndef LEVENSHTEIN_DISTANCE_H
#define LEVENSHTEIN_DISTANCE_H

#include <stddef.h>

int levenshtein(const char* lhs_str, const char* rhs_str);

int levenshtein_extra(
    const void* lhs, size_t lhs_size,
    const void* rhs, size_t rhs_size,
    int ins_cost, int rep_cost, int del_cost,
    void* (alloc)(void* ptr, size_t size, void* ud), void* ud
);

#endif /* LEVENSHTEIN_DISTANCE_H */