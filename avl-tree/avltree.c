#include "avltree.h"

#include <stdint.h>
#include <string.h>
#include <setjmp.h>

/* Definition of macro-constants */

#define AVLT_OKEY 0
#define AVLT_FAIL 1

/* Definition of structures */

typedef struct avlti_node_t {
    struct avlti_node_t* lt;
    struct avlti_node_t* rt;
    int height;

    const void* key;
    void* data;
} avlti_node_t;

struct avltree_t {
    avlti_node_t* root;

    avltree_compare_t cmp;
    avltree_delete_t  del;

    avltree_alloc_t alloc;
    void*           udata;
};

/* Static assert for test equality sizes of scructure */

struct avlti_static_assert { int a[(
    AVLTREE_NODE_SIZE == sizeof(avlti_node_t) &&
    AVLTREE_NODE_SIZE == sizeof(avltree_t)
) * 2 - 1]; };

/* Get of height */

static int avlti_height(avlti_node_t* node) {
    return node ? node->height : 0;
}

/* Tree creation and destruction */

avltree_t* avltree_create(
    avltree_compare_t cmp, avltree_delete_t del,
    avltree_alloc_t alloc, void* udata
) {
    avltree_t* tree;

    if (!alloc || !cmp) return NULL;
    tree = alloc(udata, NULL);
    if (!tree) return NULL;

    tree->root  = NULL;
    tree->cmp   = cmp;
    tree->del   = del;
    tree->alloc = alloc;
    tree->udata = udata;

    return tree;
}

static void avlti_destroy_node(avltree_t* tree, avlti_node_t* node) {
    if (!node) return;

    avlti_destroy_node(tree, node->lt);
    avlti_destroy_node(tree, node->rt);

    if (tree->del) tree->del(node->key, node->data);

    tree->alloc(tree->udata, node);
}

void avltree_destroy(avltree_t* tree) {
    if (!tree) return;
    avlti_destroy_node(tree, tree->root);
    tree->alloc(tree->udata, tree);
}

/* Searching */

void* avltree_search(const avltree_t* tree, const void* key) {
    avlti_node_t* node;

    if (!tree || !tree->root) return NULL;

    node = tree->root;
    while (1) {
        int cmp = tree->cmp(key, node->key);

        if (cmp == 0) return node->data;

        if (cmp < 0 && node->lt) { node = node->lt; continue; }
        if (cmp > 0 && node->rt) { node = node->rt; continue; }

        break;
    }

    return NULL;
}

/* Node manipulations */

static int avlti_max(int a, int b) { return a > b ? a : b; }

static void avlti_fixup_height(avlti_node_t* node) {
    node->height = avlti_max(
        avlti_height(node->lt),
        avlti_height(node->rt)
    ) + 1;
}

static avlti_node_t* avlti_left_rotate(avlti_node_t* node) {
    avlti_node_t* child = node->rt;
    node->rt = child->lt; child->lt = node;
    avlti_fixup_height(node);
    avlti_fixup_height(child);
    return child;
}

static avlti_node_t* avlti_right_rotate(avlti_node_t* node) {
    avlti_node_t* child = node->lt;
    node->lt = child->rt; child->rt = node;
    avlti_fixup_height(node);
    avlti_fixup_height(child);
    return child;
}

static avlti_node_t* avlti_balance(avlti_node_t* node) {
    int bf; avlti_fixup_height(node);

    bf = avlti_height(node->lt) - avlti_height(node->rt);

    /*  */ if (bf > +1) {
        if (avlti_height(node->lt->rt) > avlti_height(node->lt->lt))
            node->lt = avlti_left_rotate(node->lt);
        node = avlti_right_rotate(node);
    } else if (bf < -1) {
        if (avlti_height(node->rt->lt) > avlti_height(node->rt->rt))
            node->rt = avlti_right_rotate(node->rt);
        node = avlti_left_rotate(node);
    }

    return node;
}

/* Insertation */

static jmp_buf avlti_insert_buf;

static avlti_node_t* avlti_insert_node(
    avltree_t* tree, avlti_node_t* node,
    const void* key, void* data
) {
    int cmp;

    if (!node) {
        avlti_node_t* leaf = tree->alloc(tree->udata, NULL);
        if (!leaf) longjmp(avlti_insert_buf, AVLT_FAIL + 1); /* throw exception */
        memset(leaf, 0, sizeof *leaf);
        leaf->height = 1;
        leaf->key  = key ;
        leaf->data = data;
        return leaf;
    }

    cmp = tree->cmp(key, node->key);

    /**/ if (cmp < 0) node->lt = avlti_insert_node(tree, node->lt, key, data);
    else if (cmp > 0) node->rt = avlti_insert_node(tree, node->rt, key, data);
    else { node->data = data; longjmp(avlti_insert_buf, AVLT_OKEY + 1); } /* fast return */

    return avlti_balance(node);
}

int avltree_insert(avltree_t* tree, const void* key, void* data) {
    if (!tree) return AVLT_FAIL;
    switch (setjmp(avlti_insert_buf) - 1) {
        case AVLT_OKEY: return AVLT_OKEY;
        case AVLT_FAIL: return AVLT_FAIL;
    }

    tree->root = avlti_insert_node(tree, tree->root, key, data);

    return AVLT_OKEY;
}

/* Deletation */

static avlti_node_t* avlti_delete_node_near(avlti_node_t* node) {
    if (!node->lt) return node->rt;
    node->lt = avlti_delete_node_near(node->lt);
    return avlti_balance(node);
}

static avlti_node_t* avlti_delete_node(
    avltree_t* tree, avlti_node_t* node,
    const void* key
) {
    int cmp;

    if (!node) return NULL;

    cmp = tree->cmp(key, node->key);

    /**/ if (cmp < 0) node->lt = avlti_delete_node(tree, node->lt, key);
    else if (cmp > 0) node->rt = avlti_delete_node(tree, node->rt, key);
    else {
        avlti_node_t* lt = node->lt;
        avlti_node_t* rt = node->rt;
        avlti_node_t* near;

        if (tree->del) tree->del(node->key, node->data);
        tree->alloc(tree->udata, node);

        /* l-r or l-* or *-r or *-* */
        if (!rt) return lt;
        /* l-r or *-r */
        if (!lt) return rt;
        /* l-r */

        for (near = rt; near->lt; near = near->lt) {}

        near->rt = avlti_delete_node_near(rt);
        near->lt = lt;

        return avlti_balance(near);
    }

    return avlti_balance(node);
}

int avltree_delete(avltree_t* tree, const void* key) {
    if (!tree) return AVLT_FAIL;

    tree->root = avlti_delete_node(tree, tree->root, key);

    return AVLT_OKEY;
}

/* Output extension */

static int avlti_put_node(
    const avltree_io_t* io, const avlti_node_t* node,
    size_t level, int is_left, int use_ansi_colors
) {
    static char prefix[1024] = {0}; char bf_str;
    if (!node) return AVLT_OKEY;

    /* Print right branch */
    memcpy(prefix + level * 4, is_left ? "|    " : "    ", 4);
    if (avlti_put_node(io, node->rt, level + 1, 0, use_ansi_colors)) return AVLT_FAIL;
    if (node->rt) {
        if (io->put_str(io->ud, prefix, 4 * level + 4)) return AVLT_FAIL;
        if (io->put_str(io->ud, "|\n", 2)) return AVLT_FAIL;
    }

    /* Print stalk current node */
    if (io->put_str(io->ud, prefix, 4 * level)) return AVLT_FAIL;
    if (io->put_str(io->ud, is_left ? "`" : ",", 1)) return AVLT_FAIL;
    if (io->put_str(io->ud, "--", 2)) return AVLT_FAIL;

    /* Begin coloring */
    if (use_ansi_colors) {
        const char* clr;
        int bf = avlti_height(node->lt) - avlti_height(node->rt);

        /**/ if (bf == 0) { clr = "43;30"; bf_str =  '|'; }
        else if (bf <  0) { clr = "41;30"; bf_str =  '/'; }
        else if (bf >  0) { clr = "42;30"; bf_str = '\\'; }

        if (io->put_str(io->ud, "\x1b[", 2)) return AVLT_FAIL;
        if (io->put_str(io->ud, clr, 5)) return AVLT_FAIL;
        if (io->put_str(io->ud, "m", 1)) return AVLT_FAIL;
    }

    /* Print base of node */
    if (io->put_str(io->ud, "(", 1)) return AVLT_FAIL;
    if (io->put_str(io->ud, &bf_str, 1)) return AVLT_FAIL;
    if (io->put_str(io->ud, ")", 1)) return AVLT_FAIL;

    /* Print key and data, if provided */
    if (io->put_key) {
        if (io->put_str(io->ud, " ", 1)) return AVLT_FAIL;
        if (io->put_key(io->ud, node->key)) return AVLT_FAIL;
    }
    if (io->put_dat) {
        if (io->put_str(io->ud, " -> ", 4)) return AVLT_FAIL;
        if (io->put_dat(io->ud, node->data)) return AVLT_FAIL;
    }

    /* End coloring */
    if (use_ansi_colors)
        if (io->put_str(io->ud, "\x1b[0m", 4)) return AVLT_FAIL;

    if (io->put_str(io->ud, "\n", 1)) return AVLT_FAIL;

    /* Print left branch */
    memcpy(prefix + level * 4, is_left ? "    " : "|   ", 4);
    if (node->lt) {
        if (io->put_str(io->ud, prefix, 4 * level + 4)) return AVLT_FAIL;
        if (io->put_str(io->ud, "|\n", 2)) return AVLT_FAIL;
    }
    if (avlti_put_node(io, node->lt, level + 1, 1, use_ansi_colors)) return AVLT_FAIL;

    return AVLT_OKEY;
}

int avltree_output(const avltree_t* tree, const avltree_io_t* io, int use_ansi_colors) {
    if (!tree || !io || !io->put_str) return AVLT_FAIL;
    return avlti_put_node(io, tree->root, 0, 1, use_ansi_colors);
}