/* Memory pool
 *
 * Allocator of objects with fixed size.
 * Responsibility about releasing of buffer on programist
 */

#ifndef MEMORY_POOL_H
#define MEMORY_POOL_H

#include <stddef.h>

typedef struct mempool_t mempool_t;

mempool_t* mp_create(
    void* buffer, size_t length,
    size_t size, size_t align
);

void* mp_alloc(mempool_t* pool           );
void  mp_free (mempool_t* pool, void* ptr);

#endif /* MEMORY_POOL_H */