/* Red-black tree
 *
 * Allocator restrictions:
 *   If passed NULL, returns pointer to memory with size at least RBTREE_NODE_SIZE
 *   If passed not NULL, free memory by passed pointer
 *   Memory has alignment for pointer (minimal 2)
 *
 * Equivalent in std lib:
 *   ptr = alloc(ud, NULL)  <=>  ptr = malloc(RBTREE_NODE_SIZE)
 *   alloc(ud, ptr)         <=>  free(ptr)
 *
 * Alignment:
 *   (uintptr_t)nptr % 2             == 0  must always be true
 *   (uintptr_t)nptr % sizeof(void*) == 0  preferably it should be true
 */

#ifndef RED_BLACK_TREE_H
#define RED_BLACK_TREE_H

#include <stddef.h>

#define RBTREE_NODE_SIZE (5 * sizeof(void*))

typedef struct rbtree_t rbtree_t;

typedef void* (*rbtree_alloc_t)(void* ud, void* ptr);
typedef int (*rbtree_compare_t)(const void* lhs, const void* rhs);
typedef void (*rbtree_delete_t)(const void* key, void* data);

rbtree_t* rbtree_create(
    rbtree_compare_t cmp_func,
    rbtree_delete_t  del_func, /* optional */
    rbtree_alloc_t alloc_func,
    void* alloc_userdata
);

int   rbtree_insert(      rbtree_t* tree, const void* key, void* data);
void* rbtree_search(const rbtree_t* tree, const void* key            );
int   rbtree_delete(      rbtree_t* tree, const void* key            );

void rbtree_destroy(rbtree_t* tree);

/* Output extension */

typedef struct {
    int (*put_str)(void* ud, const char* str, size_t size);
    int (*put_key)(void* ud, const void* key); /* optional */
    int (*put_dat)(void* ud, const void* dat); /* optional */
    void* ud;
} rbtree_io_t;

int rbtree_output(
    const rbtree_t* tree,
    const rbtree_io_t* io,
    int use_ansi_colors
);

#endif /* RED_BLACK_TREE_H */