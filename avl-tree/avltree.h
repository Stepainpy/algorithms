/* AVL tree
 *
 * Return value: 0 upon success, nonzero otherwise
 *
 * Allocator restrictions:
 *   If passed NULL, returns pointer to memory with size at least AVLTREE_NODE_SIZE
 *   If passed not NULL, free memory by passed pointer
 *
 * Equivalent in std lib:
 *   ptr = alloc(ud, NULL)  <=>  ptr = malloc(AVLTREE_NODE_SIZE)
 *   alloc(ud, ptr)         <=>  free(ptr)
 *
 * Alignment:
 *   (uintptr_t)nptr % sizeof(void*) == 0  preferably it should be true
 */

#ifndef AVL_TREE_H
#define AVL_TREE_H

#include <stddef.h>

#define AVLTREE_NODE_SIZE (5 * sizeof(void*))

typedef struct avltree_t avltree_t;

typedef void* (*avltree_alloc_t)(void* ud, void* ptr);
typedef int (*avltree_compare_t)(const void* lhs, const void* rhs);
typedef void (*avltree_delete_t)(const void* key, void* data);

avltree_t* avltree_create(
    avltree_compare_t cmp_func,
    avltree_delete_t  del_func, /* optional */
    avltree_alloc_t alloc_func,
    void* alloc_userdata
);

int   avltree_insert(      avltree_t* tree, const void* key, void* data);
void* avltree_search(const avltree_t* tree, const void* key            );
int   avltree_delete(      avltree_t* tree, const void* key            );

void avltree_destroy(avltree_t* tree);

/* Output extension */

typedef struct {
    int (*put_str)(void* ud, const char* str, size_t size);
    int (*put_key)(void* ud, const void* key); /* optional */
    int (*put_dat)(void* ud, const void* dat); /* optional */
    void* ud;
} avltree_io_t;

int avltree_output(
    const avltree_t* tree,
    const avltree_io_t* io,
    int use_ansi_colors
);

#endif /* AVL_TREE_H */