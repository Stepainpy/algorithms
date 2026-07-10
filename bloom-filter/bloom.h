#ifndef BLOOM_FILTER_H
#define BLOOM_FILTER_H

#include <stddef.h>

typedef struct bloom_filter_t bloom_filter_t;

typedef unsigned long (*blfltr_hash_t)(const void* data, size_t size);

bloom_filter_t* blfltr_create(size_t bits, const blfltr_hash_t hashes[], size_t count);
void blfltr_destroy(bloom_filter_t* filter);

void blfltr_add (      bloom_filter_t* filter, const void* data, size_t size);
int  blfltr_test(const bloom_filter_t* filter, const void* data, size_t size);

#endif /* BLOOM_FILTER_H */