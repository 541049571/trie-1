/*
 * trie.h
 * Copyright (C) 2016 DerShokus <lily.coder@gmail.com>
 *
 * Distributed under terms of the MIT license.
 */

#ifndef TRIE_H
#define TRIE_H

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

/*
 * Trie object. All fields are hidden
 */
struct trie;

/*
 * Trie node.
 */
struct trie_node;

/*
 * Allocator for trie object and all trie's nodes.
 */
typedef void *(*trie_allocator_t)(size_t);

/*
 * Delete memory which allocated by trie_allocator_t)
 */
typedef void (*trie_deallocator_t)(void *);

/*
 * Create a new trie object.
 * Returns a trie object or null if the operation failed.
 * If an allocator is NULL - trie uses malloc.
 * If a deallocator is NULL - trie uses free.
 */
struct trie *trie_new(trie_allocator_t allocator,
                      trie_deallocator_t deallocator);
/*
 * Delete an object and all data which contained there.
 * Pointer to an object sets to NULL.
 */
void trie_delete(struct trie **trie);

/*
 * Insert new data into the trie.
 * Previous data (associated with the key) will be returned by old parameter and
 * will be replaced by data parameter.
 *
 * Returns true if the operation completed successfully.
 */
bool trie_insert(struct trie *root, const uint8_t *key, const size_t key_size,
                 void *data, void **old);

/*
 * Get a value associated with the key.
 * A value returns by data parameter.
 *
 * Returns true if a trie contains the key and the operation completed
 * successfully.
 */
bool trie_at(struct trie *root, const uint8_t *key, const size_t key_size,
             void **data);

/*
 * Remove subtree for the key.
 * The old value returns by data parameter.
 *
 * Returns true if the operation completed successfully.
 */
bool trie_remove(struct trie *obj, const uint8_t *key, const size_t key_size,
                 void **data);

/*
 * Returns the first node with data from a trie.
 * If a trie is empty - returns NULL.
 */
struct trie_node *trie_begin(struct trie *trie);

/*
 * Returns the next node with data or (if the end was riched) NULL.
 */
struct trie_node *trie_next(struct trie_node *node);

/*
 * Delete a node and returns the next one.
 * If the end of a trie was riched - returns NULL.
 */
struct trie_node *trie_next_delete(struct trie *obj, struct trie_node *node);

/*
 * Get data of a node.
 *
 * Returns true if the operation completed successfully.
 */
bool trie_data(struct trie_node *node, void **data);

#endif /* !TRIE_H */
