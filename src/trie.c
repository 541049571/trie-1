/*
 * trie.c
 * Copyright (C) 2016 DerShokus <lily.coder@gmail.com>
 *
 * Distributed under terms of the MIT license.
 */

#include "trie.h"

#include <assert.h>
#include <stdlib.h>
#include <strings.h>
#include <stdio.h>

struct trie_node {
        uint8_t symbol;

        struct trie_node *negative; // negative or parent node
        bool parent;

        bool data_flag;
        union {
                struct trie_node *positive;
                void *data;
        };
};

struct trie {
        struct trie_node *root;
        trie_allocator_t allocator;
        trie_deallocator_t deallocator;
};

static inline struct trie_node *trie_node_new(const struct trie *obj,
                                              uint8_t symbol)
{
        assert(obj != NULL);
        assert(obj->allocator != NULL);

        struct trie_node *node = obj->allocator(sizeof(struct trie_node));
        memset(node, 0, sizeof(*node));
        if (node) {
                node->symbol = symbol;
        }
        return node;
}

static inline void trie_node_set_parent(struct trie_node *node,
                                        struct trie_node *parent)
{
        assert(node != NULL);

        node->parent   = true;
        node->negative = parent;
}

static inline struct trie_node *trie_node_get_parent(struct trie_node *node)
{
        assert(node != NULL);
        return node->parent ? node->negative : NULL;
}

static inline void trie_node_set_negative(struct trie_node *node,
                                          struct trie_node *negative)
{
        assert(node != NULL);

        node->parent   = false;
        node->negative = negative;
}

static inline struct trie_node *trie_node_get_negative(struct trie_node *node)
{
        assert(node != NULL);

        return node->parent ? NULL : node->negative;
}

static inline struct trie_node *trie_node_get_positive(struct trie_node *node)
{
        assert(node != NULL);

        return node->data_flag ? NULL : node->positive;
}

static inline void trie_node_set_positive(struct trie_node *node,
                                          struct trie_node *positive)
{
        assert(node != NULL);

        node->data_flag = false;
        node->positive  = positive;
}

static inline bool trie_node_attach(struct trie_node *node,
                                    struct trie_node *attached,
                                    bool is_positive)
{
        assert(node != NULL);
        assert(attached != NULL);
        assert(!attached->parent && "the node already attached");
        assert(attached->negative == NULL && "can't attach a subtree");

        if (is_positive) {
                // is the node full?
                if (node->data_flag || node->positive)
                        return false;
                node->positive = attached;
                trie_node_set_parent(attached, node);
        } else {
                if (node->parent)
                        trie_node_set_parent(attached, node->negative);
                else
                        trie_node_set_negative(attached, node->negative);
                trie_node_set_negative(node, attached);
        }

        return true;
}

static inline void trie_node_set_chain_parent(struct trie_node *node,
                                              struct trie_node *parent)
{
        assert(node != NULL);

        // go to the end of a negative chain
        while (trie_node_get_negative(node)) {
                node = trie_node_get_negative(node);
        }
        trie_node_set_parent(node, parent);
}

static inline struct trie_node *
trie_node_get_chain_parent(struct trie_node *node)
{
        assert(node != NULL);

        while (trie_node_get_negative(node)) {
                node = trie_node_get_negative(node);
        }
        return trie_node_get_parent(node);
}

struct find_res {
        const size_t sz;
        struct trie_node *last;
        struct trie_node *prev;
};

static inline struct find_res
trie_find(struct trie_node *root, const uint8_t *key, const size_t key_size)
{
        if (root == NULL || key == NULL || key_size == 0) {
                struct find_res res = {.sz = 0, .last = NULL, .prev = NULL};
                return res;
        }

        size_t i               = 0;
        struct trie_node *node = root, *prev;

        for (i = 0; i <= (key_size - 1) && node;) {
                prev = node;
                if (node->symbol == key[i]) {
                        node = node->positive;
                        ++i;
                } else {
                        if (node->parent)
                                break;
                        node = trie_node_get_negative(node);
                }
        }

        struct find_res res = {.sz = i, .last = node, .prev = prev};
        return res;
}

static inline struct trie_node *trie_new_chain(const struct trie *obj,
                                               const uint8_t *str,
                                               const size_t size,
                                               struct trie_node **last)
{
        assert(obj != NULL);

        if (str == NULL || size == 0)
                return NULL;

        struct trie_node *node = trie_node_new(obj, str[0]), *res = node;
        if (node == NULL)
                return NULL;

        bool need_free = false;
        for (size_t i = 1; i < size; ++i) {
                node->positive = trie_node_new(obj, str[i]);
                trie_node_set_parent(node->positive, node);
                if (!node->positive) {
                        need_free = true;
                        break;
                }
                node = node->positive;
        }

        if (need_free) {
                while (res) {
                        struct trie_node *item = res;
                        res                    = res->positive;
                        obj->deallocator(item);
                }
        }

        if (last) {
                *last = node;
        }

        return res;
}

static inline struct trie_node *begin(struct trie_node *node)
{
        assert(node != NULL);

        for (struct trie_node *positive = trie_node_get_positive(node);
             positive; positive         = trie_node_get_positive(positive))
                node = positive;
        return node;
}

static inline struct trie_node *trie_node_delete_up(struct trie *obj,
                                          struct trie_node *node)
{
        assert(obj != NULL);
        assert(node != NULL);

        struct trie_node *delete = NULL;
        do {
                delete = node;
                node   = trie_node_get_parent(node);
                obj->deallocator(delete);
        } while (node && trie_node_get_negative(node) == NULL);
        return node;
}

static inline struct trie_node *trie_node_delete_right(struct trie *obj,
                                             struct trie_node *node)
{
        assert(obj != NULL);

        if (node == NULL)
                return node;
        struct trie_node *delete = trie_node_get_negative(node);
        if (delete == NULL)
                return node;
        memcpy(node, delete, sizeof(*node));
        obj->deallocator(delete);
        delete = trie_node_get_positive(node);
        if (delete)
                trie_node_set_chain_parent(delete, node);
        return node;
}

static inline struct trie_node *trie_node_delete_end(struct trie *obj,
                                           struct trie_node *node)
{
        assert(obj != NULL);
        assert(node != NULL);
        assert(trie_node_get_negative(node) == NULL);

        struct trie_node *prev = trie_node_get_parent(node);
        if (!prev) {
                // only one value in the trie?
                if (node == obj->root) {
                        obj->deallocator(obj->root);
                        obj->root = NULL;
                        return NULL;
                }
        } else {
                // The node has a parent and hasn't negative nodes.
                // o
                // |
                // x <- this node whill be deleted
                if (trie_node_get_positive(prev) == node) {
                        obj->deallocator(node);
                        trie_node_set_positive(prev, NULL);
                        return prev;
                }
        }

        // o
        // |
        // o-o-...-o-x - the last node is node to delete
        // |       ^- we find the previous node.
        // ...
        //
        // or
        //
        // o-o-...-o-x
        // |       ^
        // ...
        prev = prev ? trie_node_get_positive(prev) : obj->root;
        while (trie_node_get_negative(prev) != node) {
                prev = trie_node_get_negative(prev);
        }
        trie_node_set_parent(prev, trie_node_get_parent(node));
        obj->deallocator(node);
        return prev;
}

// +--------------------------------------------------------------------------+
// | Public functions                                                         |
// +--------------------------------------------------------------------------+

struct trie *trie_new(trie_allocator_t allocator,
                      trie_deallocator_t deallocator)
{
        struct trie *trie;
        // create a structure
        if (allocator) {
                trie = allocator(sizeof(struct trie));
        } else {
                trie = calloc(1, sizeof(struct trie));
        }
        if (trie) {
                trie->allocator   = allocator ? allocator : malloc;
                trie->deallocator = deallocator ? deallocator : free;
        }

        return trie;
}

void trie_delete(struct trie **trie)
{
        if (!trie || !(*trie))
                return;
        for (struct trie_node *node = trie_begin(*trie); node;
             node                   = trie_next_delete(*trie, node)) {
        }
        // Seppuku!
        (*trie)->deallocator(*trie);
        *trie = NULL;
}

bool trie_insert(struct trie *root, const uint8_t *key, const size_t key_size,
                 void *data, void **old)
{
        if (old != NULL)
                *old = NULL;

        struct find_res found  = trie_find(root->root, key, key_size);
        struct trie_node *last = NULL;
        if (found.sz == 0) {
                if (found.prev == NULL)
                        root->root = trie_new_chain(root, key, key_size, &last);
                else {
                        struct trie_node *chain =
                            trie_new_chain(root, key, key_size, &last);
                        if (!trie_node_attach(found.prev, chain, false)) {
                                trie_node_delete_up(root, chain);
                                return false;
                        }
                }

        } else if (found.sz == key_size) {
                last = found.prev;
        } else {
                struct trie_node *tail = trie_new_chain(
                    root, &key[found.sz], key_size - found.sz, &last);
                trie_node_attach(found.prev, tail,
                                 key[found.sz] == found.prev->symbol);
        }

        memcpy(&old, &last->data, sizeof(last->data));
        memcpy(&last->data, &data, sizeof(last->data));
        last->data_flag = true;

        return true;
}

bool trie_at(struct trie *root, const uint8_t *key, const size_t key_size,
             void **data)
{
        struct find_res found = trie_find(root->root, key, key_size);
        if (found.sz == key_size && found.prev) {
                memcpy(data, &found.prev->data, sizeof(*data));
                return true;
        }

        return false;
}

bool trie_remove(struct trie *obj, const uint8_t *key, const size_t key_size,
                 void **data)
{
        struct find_res found = trie_find(obj->root, key, key_size);
        if (found.sz == key_size && found.prev) {
                if (trie_data(found.prev, data)) {
                        trie_next_delete(obj, found.prev);
                        return true;
                }
        }
        return false;
}

struct trie_node *trie_begin(struct trie *trie)
{
        if (trie == NULL || trie->root == NULL)
                return NULL;
        return begin(trie->root);
}

struct trie_node *trie_next(struct trie_node *node)
{
        while (node) {
                struct trie_node *negative = trie_node_get_negative(node);
                while (negative) {
                        struct trie_node *res = begin(negative);
                        if (res)
                                return res;
                        negative = trie_node_get_negative(negative);
                }
                node = trie_node_get_parent(node);
        }
        return NULL;
}

bool trie_data(struct trie_node *node, void **data)
{
        if (node == NULL || data == NULL || !node->data_flag)
                return false;
        memcpy(data, &node->data, sizeof(node->data));
        return true;
}

struct trie_node *trie_next_delete(struct trie *obj, struct trie_node *node)
{
        if (obj == NULL || node == NULL)
                return NULL;

        if (trie_node_get_negative(node)) {
                node = trie_node_delete_right(obj, node);
        } else {
                struct trie_node *parent = trie_node_get_parent(node);
                if (parent && trie_node_get_positive(parent) == node) {
                        node = trie_node_delete_up(obj, node);
                        if (node) {
                                node = trie_node_delete_right(obj, node);
                        }
                } else {
                        node = trie_node_delete_end(obj, node);
                }
        }
        if (node) {
                node = begin(node);
                assert(node->data_flag);
        } else {
                obj->root = NULL;
        }
        return node;
}

bool trie_export_dot(struct trie *obj, const char *file_name)
{
        FILE *file                = fopen(file_name, "w");
        const static char *header = "digraph name {\n";
        fwrite(header, sizeof(header[0]), strlen(header), file);
        struct trie_node *node = obj->root;
        while (node) {
                fprintf(file, "{ rank = same; N%zu", (size_t)node);
                for (struct trie_node *i = trie_node_get_negative(node); i;
                     i                   = trie_node_get_negative(i)) {
                        fprintf(file, ", N%zu", (size_t)i);
                }
                fprintf(file, " }\n");

                char symbol[4] = {'\0'};
                symbol[0]      = (char)node->symbol;
                symbol[1]      = '\0';
                if (symbol[0] == '\0') {
                        symbol[0] = '\\';
                        symbol[1] = '\\';
                        symbol[2] = '0';
                        symbol[3] = '\0';
                }
                fprintf(file, "\tN%zu [label=\"%s\"];\n", (size_t)node, symbol);
                struct trie_node *p = node->positive, *n = node->negative;
                if (p)
                        fprintf(file, "\tN%zu -> %s%zu [%s];\n", (size_t)node,
                                node->data_flag ? "D" : "N", (size_t)p,
                                node->data_flag ? "color=\"steelblue\""
                                                : "color=\"chartreuse\"");
                if (n)
                        fprintf(file, "\tN%zu -> N%zu %s;\n", (size_t)node,
                                (size_t)n,
                                node->parent ? "[style=\"dotted\"]"
                                             : "[color=\"indianred\"]");

                if (trie_node_get_positive(node) == NULL) {
                        while (node && trie_node_get_negative(node) == NULL) {
                                node = trie_node_get_parent(node);
                        }
                        if (node)
                                node = trie_node_get_negative(node);
                } else {
                        node = trie_node_get_positive(node);
                }
        }

        fprintf(file, "}\n");

        return true;
}
