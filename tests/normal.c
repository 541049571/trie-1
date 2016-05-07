/*
 * normal.c
 * Copyright (C) 2016 DerShokus <lily.coder@gmail.com>
 *
 * Distributed under terms of the MIT license.
 */

#include <trie.h>
#include <stdio.h>

int main(void)
{
        static char word_1[] = "aaa";
        static char word_2[] = "bba";

        struct trie *obj = trie_new(NULL, NULL);

        void *old, *data;
        data = (void *)0xA;
        bool ret =
            trie_insert(obj, (uint8_t *)word_1, strlen(word_1) + 1, data, &old);
        data = (void *)0xB;
        ret =
            trie_insert(obj, (uint8_t *)word_2, strlen(word_2) + 1, data, &old);

        ret = trie_at(obj, (uint8_t *)word_1, strlen(word_1) + 1, &old);
        ret = trie_at(obj, (uint8_t *)word_2, strlen(word_2) + 1, &old);
        return 0;
}
