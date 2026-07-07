#include "mempool.h"

#include <stdint.h>

typedef struct mpi_node_t {
    struct mpi_node_t* next;
} mpi_node_t;

struct mempool_t {
    mpi_node_t* head;
    mpi_node_t* end;
    size_t obj_size;
};

/* Memory layout
 *
 * |<-  mp hdr  ->||<--  ...nodes...  -->
 * |----|----|----||----|----|----|----||----|
 * ^              ^
 * |              +- alignment of object
 * +- alignment of pointers
 *
 */

static int mpi_is_pow2(size_t n) { return n > 0 && (n & (n - 1)) == 0; }
static size_t mpi_max(size_t a, size_t b) { return a > b ? a : b; }

mempool_t* mp_create(
    void* buffer, size_t length,
    size_t size, size_t align
) {
    mpi_node_t *begin, *prev;
    void* origin = buffer;
    mempool_t* pool;
    size_t count, i;

    if (!buffer || !length) return NULL;
    if (!mpi_is_pow2(align)) return NULL;

    align = mpi_max(align, sizeof(void*     ));
    size  = mpi_max(size , sizeof(mpi_node_t));
    if (size % align > 0)
        size += align - size % align;

    if ((uintptr_t)buffer % align > 0) {
        size_t padding = align - (uintptr_t)buffer % align;
        if (length < padding) return NULL;
        buffer = (char*)buffer + padding;
        length =        length - padding;
    }

    while ((char*)buffer - (char*)origin < (ptrdiff_t)sizeof(mempool_t) && length >= align) {
        buffer = (char*)buffer + align;
        length =        length - align;
    }
    if ((char*)buffer - (char*)origin < (ptrdiff_t)sizeof(mempool_t) || length < size) return NULL;

    pool = (mempool_t*)buffer - 1;
    count = length / size;

    begin = (mpi_node_t*)buffer;
    pool->obj_size = size;
    pool->end = begin + count;

    prev = NULL;
    for (i = count; i --> 0;) {
        mpi_node_t* current = (void*)((char*)begin + size * i);
        current->next = prev;
        prev = current;
    }
    pool->head = prev;

    return pool;
}

void* mp_alloc(mempool_t* pool) {
    void* ptr;

    if (!pool || !pool->head) return NULL;

    ptr = pool->head;
    pool->head = pool->head->next;
    return ptr;
}

void mp_free(mempool_t* pool, void* ptr) {
    mpi_node_t *begin, *current;

    if (!pool || !ptr) return;
    begin = (mpi_node_t*)(pool + 1);
    if ((mpi_node_t*)ptr < begin || pool->end <= (mpi_node_t*)ptr) return;
    if (((char*)ptr - (char*)begin) % pool->obj_size > 0) return;

    current = ptr;
    current->next = pool->head;
    pool->head = current;
}