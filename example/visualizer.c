/*
 * visualizer.c
 * Copyright (C) 2016 dershokus <lily.coder@gmail.com>
 *
 * Distributed under terms of the MIT license.
 */

#include <trie.h>

int main(int argc, char *argv[])
{
    struct trie *trie = trie_new(NULL, NULL);
    void *value, *old;
    uint8_t *key;
    for (int i=1; i<argc; ++i)
    {
        key = (uint8_t*)argv[i];
        value = NULL;
        memcpy(&value, &i, sizeof(i));
        trie_insert(trie,key, strlen(argv[i]) + 1, value, &old);
    }

    trie_export_dot(trie, "./trie.dot");

    return 0;
}
