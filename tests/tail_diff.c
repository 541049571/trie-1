/*
 * RootDiff.c
 * Copyright (C) 2016 DerShokus <lily.coder@gmail.com>
 *
 * Distributed under terms of the MIT license.
 */
#include <trie.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

struct word {
        uint8_t *data;
        size_t size;
        void *value;
};

void dealloc(void *ptr)
{
        printf("delete: 0x%p\n", ptr);
        free(ptr);
}

int main(void)
{
        static struct word word1 = {
            .data = (uint8_t *)"aaa", .size = 4, .value = (void *)0xA};
        static struct word word2 = {
            .data = (uint8_t *)"aba", .size = 4, .value = (void *)0xB};

        struct trie *obj = trie_new(NULL, dealloc);
        assert(obj);

        void *data;

        bool ret;

        ret = trie_insert(obj, word1.data, word1.size, word1.value, &data);
        assert(ret);
        assert(data == NULL);

        // check the first insertion
        ret = trie_at(obj, word1.data, word1.size, &data);
        assert(ret);
        assert(data == word1.value);

        ret = trie_insert(obj, word2.data, word2.size, word2.value, &data);
        assert(ret);
        assert(data == NULL);

        // check all after all insertions
        ret = trie_at(obj, word1.data, word1.size, &data);
        assert(ret);
        assert(data == word1.value);

        ret = trie_at(obj, word2.data, word2.size, &data);
        assert(ret);
        assert(data == word2.value);

        for (struct trie_node *i = trie_begin(obj); i;
             i                   = trie_next_delete(obj, i)) {
                char *str = NULL;
                if (trie_data(i, (void *)&str))
                        printf("trie next: 0x%x\n", (unsigned int)str);
                else
                        printf("trie next failed!\n");
        }

        trie_delete(&obj);
        assert(obj == NULL);

        return 0;
}
