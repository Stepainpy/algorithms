#include "rbtree.h"

#include <string.h>

/* Definition of uintptr_t */

#if __STDC_VERSION__ >= 199901L
#  include <stdint.h>
#else
#  include <limits.h>
#  if ULONG_MAX > 0xFFFFFFFFul
typedef unsigned long uintptr_t;
#  else
#    if __GNUC__
#      pragma GCC diagnostic push
#      pragma GCC diagnostic ignored "-Wlong-long"
#    endif
typedef unsigned long long uintptr_t;
#    if __GNUC__
#      pragma GCC diagnostic pop
#    endif
#  endif
#endif

/* Definition of macro-constants */

#define RBT_MASK ((uintptr_t)1)

#define RBT_OKEY 0
#define RBT_FAIL 1

#define RBT_BLACK 0
#define RBT_RED   1

/* Definition of structures */

typedef struct rbti_node_t {
    /* LSB use for color, 0/1 -> black/red */
    uintptr_t prnt_clr;

    struct rbti_node_t* lt;
    struct rbti_node_t* rt;

    const void* key;
    void* data;
} rbti_node_t;

struct rbtree_t {
    rbti_node_t* root;

    rbtree_compare_t cmp;
    rbtree_delete_t  del;

    rbtree_alloc_t alloc;
    void*          udata;
};

/* Static assert for test equality sizes of scructure */

struct rbti_static_assert { int a[(
    RBTREE_NODE_SIZE == sizeof(rbti_node_t) &&
    RBTREE_NODE_SIZE == sizeof(rbtree_t)
) * 2 - 1]; };

/* Get/set of color property */

static int rbti_color(const rbti_node_t* node) {
    return node ? node->prnt_clr & RBT_MASK : RBT_BLACK;
}
static void rbti_set_color(rbti_node_t* node, int clr) {
    node->prnt_clr = (node->prnt_clr & ~RBT_MASK) | clr;
}

/* Get/set of parent property */

static rbti_node_t* rbti_parent(const rbti_node_t* node) {
    uintptr_t parent = node->prnt_clr & ~RBT_MASK;
    return (rbti_node_t*)parent;
}
static void rbti_set_parent(rbti_node_t* node, rbti_node_t* parent) {
    node->prnt_clr = (uintptr_t)parent | (node->prnt_clr & RBT_MASK);
}

/* Tree creation and destruction */

rbtree_t* rbtree_create(
    rbtree_alloc_t alloc, void* udata,
    rbtree_compare_t cmp, rbtree_delete_t del
) {
    rbtree_t* tree;

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

void rbtree_destroy(rbtree_t* tree) {
    if (tree) {
        rbti_node_t* node = tree->root;
        rbti_node_t* parent;

        while (node) {
            if (node->lt) { node = node->lt; continue; }
            if (node->rt) { node = node->rt; continue; }

            if (tree->del)
                tree->del(node->data);

            parent = rbti_parent(node);
            if (parent) {
                if (parent->lt == node) parent->lt = NULL;
                else                    parent->rt = NULL;
            }

            tree->alloc(tree->udata, node);
            node = parent;
        }

        tree->alloc(tree->udata, tree);
    }
}

/* Searching */

void* rbtree_search(rbtree_t* tree, const void* key) {
    rbti_node_t* node;

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

/* Insertation */

static void rbti_left_rotate(rbti_node_t* node) {
    rbti_node_t* parent  = rbti_parent(node);
    rbti_node_t* grandpa = rbti_parent(parent);

    parent->rt = node->lt;
    if (node->lt) rbti_set_parent(node->lt, parent);

    rbti_set_parent(node, grandpa);
    if (grandpa) {
        if (parent == grandpa->lt) grandpa->lt = node;
        else                       grandpa->rt = node;
    }

    node->lt = parent;
    rbti_set_parent(parent, node);
}

static void rbti_right_rotate(rbti_node_t* node) {
    rbti_node_t* parent  = rbti_parent(node);
    rbti_node_t* grandpa = rbti_parent(parent);

    parent->lt = node->rt;
    if (node->rt) rbti_set_parent(node->rt, parent);

    rbti_set_parent(node, grandpa);
    if (grandpa) {
        if (parent == grandpa->lt) grandpa->lt = node;
        else                       grandpa->rt = node;
    }

    node->rt = parent;
    rbti_set_parent(parent, node);
}

int rbtree_insert(rbtree_t* tree, const void* key, void* data) {
    rbti_node_t *node, *parent, *grandpa, *uncle;
    int cmp;

    if (!tree) return RBT_FAIL;

    if (!tree->root) {
        rbti_node_t* root = tree->alloc(tree->udata, NULL);
        if (!root) return RBT_FAIL;
        memset(root, 0, sizeof *root); /* automatic is black */
        root->key  = key ;
        root->data = data;
        tree->root = root;
        return RBT_OKEY;
    }

    parent = tree->root;
    while (1) {
        cmp = tree->cmp(key, parent->key);

        if (cmp == 0) { parent->data = data; return RBT_OKEY; }

        if (cmp < 0 && parent->lt) { parent = parent->lt; continue; }
        if (cmp > 0 && parent->rt) { parent = parent->rt; continue; }

        break;
    }

    node = tree->alloc(tree->udata, NULL);
    if (!node) return RBT_FAIL;
    memset(node, 0, sizeof *node);

    rbti_set_parent(node, parent);
    rbti_set_color(node, RBT_RED);

    node->key  = key ;
    node->data = data;

    if (cmp < 0) parent->lt = node;
    else         parent->rt = node;

fixup:
    parent = rbti_parent(node);

    if (!parent) { /* Insert case 1 */
        rbti_set_color(node, RBT_BLACK);
        return RBT_OKEY;
    }

    if (rbti_color(parent) == RBT_BLACK) /* Insert case 2 */
        return RBT_OKEY;

    grandpa = rbti_parent(parent);
    uncle = parent == grandpa->lt ? grandpa->rt : grandpa->lt;

    if (rbti_color(uncle) == RBT_RED) { /* Insert case 3 */
        rbti_set_color(parent , RBT_BLACK);
        rbti_set_color(grandpa, RBT_RED  );
        rbti_set_color(uncle  , RBT_BLACK);
        node = grandpa;
        goto fixup;
    } else {
        /* Insert case 4 */
        /*  */ if (parent == grandpa->lt && node == parent->rt) {
            rbti_left_rotate(node);
            node = parent;
            parent = rbti_parent(node);
            grandpa = rbti_parent(parent);
        } else if (parent == grandpa->rt && node == parent->lt) {
            rbti_right_rotate(node);
            node = parent;
            parent = rbti_parent(node);
            grandpa = rbti_parent(parent);
        }

        /* Insert case 5 */
        rbti_set_color(parent, RBT_BLACK);
        rbti_set_color(grandpa, RBT_RED);

        if (parent == grandpa->lt && node == parent->lt)
            rbti_right_rotate(parent);
        else
            rbti_left_rotate(parent);

        if (!rbti_parent(parent)) tree->root = parent;
    }

    return RBT_OKEY;
}

/* Deletation */

int rbtree_delete(rbtree_t* tree, const void* key) {
    rbti_node_t *node, *parent, *sibling; int color;

    if (!tree || !tree->root) return RBT_FAIL;

    node = tree->root;
    while (1) {
        int cmp = tree->cmp(key, node->key);

        if (cmp == 0) break;

        if (cmp < 0 && node->lt) { node = node->lt; continue; }
        if (cmp > 0 && node->rt) { node = node->rt; continue; }

        return RBT_FAIL;
    }

    /* HERE: node may 2, 1 or 0 children */

    if (node->lt && node->rt) {
        rbti_node_t* left = node->rt;
        while (left->lt) left = left->lt;
        { const void* tmp = left->key ; left->key  = node->key ; node->key  = tmp; }
        {       void* tmp = left->data; left->data = node->data; node->data = tmp; }
        node = left;
    }

    /* HERE: node may 1 or 0 children */

    if (!node->lt != !node->rt) {
        rbti_node_t* child = node->lt ? node->lt : node->rt;

        if (tree->del)
            tree->del(node->data);

        node->key  = child->key ;
        node->data = child->data;

        tree->alloc(tree->udata, child);
        node->lt = node->rt = NULL;

        return RBT_OKEY;
    }

    /* HERE: node 0 children */

    if (tree->del)
        tree->del(node->data);

    color = rbti_color(node);
    parent = rbti_parent(node);

    if (parent) {
        if (node == parent->lt)
            parent->lt = NULL;
        else
            parent->rt = NULL;
    }

    tree->alloc(tree->udata, node);

    if (color == RBT_RED) return RBT_OKEY;
    if (!parent) {
        tree->root = NULL;
        return RBT_OKEY;
    }

    node = NULL;
fixup:
    if (!parent) return RBT_OKEY; /* Delete case 1 */

    sibling = node == parent->rt ? parent->lt : parent->rt;

    if ( /* Delete case 2 */
        rbti_color(parent     ) == RBT_BLACK &&
        rbti_color(sibling    ) == RBT_BLACK &&
        rbti_color(sibling->lt) == RBT_BLACK &&
        rbti_color(sibling->rt) == RBT_BLACK
    ) {
        rbti_set_color(sibling, RBT_RED);
        node = parent;
        parent = rbti_parent(node);
        goto fixup;
    }

    if ( /* Delete case 3 */
        rbti_color(parent     ) == RBT_BLACK &&
        rbti_color(sibling    ) == RBT_RED   &&
        rbti_color(sibling->lt) == RBT_BLACK &&
        rbti_color(sibling->rt) == RBT_BLACK
    ) {
        if (sibling == parent->lt)
            rbti_right_rotate(sibling);
        else
            rbti_left_rotate(sibling);

        rbti_set_color(parent, RBT_RED);
        rbti_set_color(sibling, RBT_BLACK);

        if (!rbti_parent(sibling))
            tree->root = sibling;

        sibling = node == parent->rt ? parent->lt : parent->rt;
    }

    if ( /* Delete case 4 */
        rbti_color(parent     ) == RBT_RED   &&
        rbti_color(sibling    ) == RBT_BLACK &&
        rbti_color(sibling->lt) == RBT_BLACK &&
        rbti_color(sibling->rt) == RBT_BLACK
    ) {
        rbti_set_color(parent, RBT_BLACK);
        rbti_set_color(sibling, RBT_RED);
        return RBT_OKEY;
    }

    if ( /* Delete case 5 */
        rbti_color(sibling) == RBT_BLACK &&
        ((sibling == parent->lt && rbti_color(sibling->rt) == RBT_RED) ||
         (sibling == parent->rt && rbti_color(sibling->lt) == RBT_RED)) &&
        ((sibling == parent->lt && rbti_color(sibling->lt) == RBT_BLACK) ||
         (sibling == parent->rt && rbti_color(sibling->rt) == RBT_BLACK))
    ) {
        rbti_node_t* near = sibling == parent->lt ? sibling->rt : sibling->lt;

        if (sibling == parent->lt)
            rbti_left_rotate(near);
        else
            rbti_right_rotate(near);

        rbti_set_color(sibling, RBT_RED);
        rbti_set_color(near, RBT_BLACK);

        sibling = near;
    }

    if ( /* Delete case 6 */
        rbti_color(sibling) == RBT_BLACK &&
        ((sibling == parent->lt && rbti_color(sibling->lt) == RBT_RED) ||
         (sibling == parent->rt && rbti_color(sibling->rt) == RBT_RED))
    ) {
        rbti_node_t* away = sibling == parent->lt ? sibling->lt : sibling->rt;

        if (sibling == parent->lt)
            rbti_right_rotate(sibling);
        else
            rbti_left_rotate(sibling);

        if (!rbti_parent(sibling))
            tree->root = sibling;

        color = rbti_color(parent);
        rbti_set_color(parent, RBT_BLACK);
        rbti_set_color(sibling, color);
        rbti_set_color(away, RBT_BLACK);
    }

    return RBT_OKEY;
}

/* Output extension */

static int rbti_put_node(
    const rbtree_io_t* io, rbti_node_t* node,
    size_t level, int is_left, int use_ansi_colors
) {
    static char prefix[1024] = {0}; int clr;
    if (!node) return RBT_OKEY;
    clr = rbti_color(node);

    /* Print right branch */
    memcpy(prefix + level * 4, is_left ? "|    " : "    ", 4);
    if (rbti_put_node(io, node->rt, level + 1, 0, use_ansi_colors)) return RBT_FAIL;
    if (node->rt) {
        if (io->put_str(io->ud, prefix, 4 * level + 4)) return RBT_FAIL;
        if (io->put_str(io->ud, "|\n", 2)) return RBT_FAIL;
    }

    /* Print stalk current node */
    if (io->put_str(io->ud, prefix, 4 * level)) return RBT_FAIL;
    if (io->put_str(io->ud, is_left ? "`" : ",", 1)) return RBT_FAIL;
    if (io->put_str(io->ud, "--", 2)) return RBT_FAIL;

    /* Begin coloring */
    if (use_ansi_colors) {
        if (io->put_str(io->ud, "\x1b[", 2)) return RBT_FAIL;
        if (io->put_str(io->ud, clr == RBT_RED ? "30;41" : "37;40", 5)) return RBT_FAIL;
        if (io->put_str(io->ud, "m", 1)) return RBT_FAIL;
    }

    /* Print base of node */
    if (io->put_str(io->ud, "(", 1)) return RBT_FAIL;
    if (io->put_str(io->ud, clr == RBT_RED ? "R" : "B", 1)) return RBT_FAIL;
    if (io->put_str(io->ud, ")", 1)) return RBT_FAIL;

    /* Print key and data, if provided */
    if (io->put_key) {
        if (io->put_str(io->ud, " ", 1)) return RBT_FAIL;
        if (io->put_key(io->ud, node->key)) return RBT_FAIL;
    }
    if (io->put_dat) {
        if (io->put_str(io->ud, " -> ", 4)) return RBT_FAIL;
        if (io->put_dat(io->ud, node->data)) return RBT_FAIL;
    }

    /* End coloring */
    if (use_ansi_colors)
        if (io->put_str(io->ud, "\x1b[0m", 4)) return RBT_FAIL;

    if (io->put_str(io->ud, "\n", 1)) return RBT_FAIL;

    /* Print left branch */
    memcpy(prefix + level * 4, is_left ? "    " : "|   ", 4);
    if (node->lt) {
        if (io->put_str(io->ud, prefix, 4 * level + 4)) return RBT_FAIL;
        if (io->put_str(io->ud, "|\n", 2)) return RBT_FAIL;
    }
    if (rbti_put_node(io, node->lt, level + 1, 1, use_ansi_colors)) return RBT_FAIL;

    return RBT_OKEY;
}

int rbtree_output(rbtree_t* tree, const rbtree_io_t* io, int use_ansi_colors) {
    if (!tree || !io || !io->put_str) return RBT_FAIL;

    if (io->put_str(io->ud, "ROOT\n", 5)) return RBT_FAIL;
    if (tree->root)
        if (io->put_str(io->ud, "|\n", 2)) return RBT_FAIL;
    return rbti_put_node(io, tree->root, 0, 1, use_ansi_colors);
}