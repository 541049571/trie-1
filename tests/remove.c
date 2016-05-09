/*
 * remove.c
 * Copyright (C) 2016 dershokus <lily.coder@gmail.com>
 *
 * Distributed under terms of the MIT license.
 */

#include <trie.h>
#include <assert.h>
#include <stdio.h>

struct data {
        uint8_t *key;
        size_t size;
        union {
                uint32_t value;
                void *value_;
        };
        bool enabled;
};

struct data *find_by_key(struct data *array, const char *key)
{

    const size_t key_size = strlen(key);
    for (size_t i=0; array[i].key != NULL; ++i) {
        if (strlen(key) == array[i].size &&
                memcmp(key, array[i].key, key_size) == 0)
            return &array[i];
    }

    return NULL;
}

struct data *find_by_value(struct data *array, uint32_t value)
{
    for (size_t i=0; array[i].key != NULL; ++i) {
        if (value == array[i].value)
            return &array[i];
    }

    return NULL;
}


struct data *find_by_value_(struct data *array, void *value)
{
    for (size_t i=0; array[i].key != NULL; ++i) {
        if (value == array[i].value_)
            return &array[i];
    }

    return NULL;
}

bool check(struct data *array, struct trie *trie)
{
    void *value;
    for (struct data *i = array;i->key;++i) {
        if (!i->enabled)
            continue;
        if (!trie_at(trie, i->key, i->size, &value)) {
            printf("key: %s, not found\n", (char*)i->key);
            return false;
        }
        if (i->value_ != value) {
            printf("key: %s has incorrect value: 0x%x != 0x%x", i->key, (unsigned int)i->value_, (unsigned int)value);
            return false;
        }
    }

    for (struct trie_node *i=trie_begin(trie); i; i = trie_next(i)) {
        void *data;
        const bool ret = trie_data(i, &data);
        assert(ret);
        const struct data *found = find_by_value_(array, data);
        if (!found) {
            printf("iteration not found, value: 0x%x\n", (unsigned int)data);
        }
        assert(found);
        assert(found->enabled);
    }
    return true;
}

/*
    a-b-c-d
    |   |
    f   e
    |
    g-h
    |
    i-j-m-n
    |
    k
    |
    l
*/

int main(void)
{

        struct data insertions[] = {
            /* 0. */ {(uint8_t *)"afgikl", strlen("afgikl"), {0x1}, true},
            /* 1. */ {(uint8_t *)"afgh",    strlen("afgh"),    {0x2}, true},
            /* 2. */ {(uint8_t *)"afgij",   strlen("afgij"),   {0x3}, true},
            /* 3. */ {(uint8_t *)"b",       strlen("b"),       {0x4}, true},
            /* 4. */ {(uint8_t *)"ce",      strlen("ce"),      {0x5}, true},
            /* 5. */ {(uint8_t *)"d",       strlen("d"),       {0x6}, true},
            /* 6. */ {(uint8_t *)"afgim",   strlen("afgim"),   {0x7}, true},
            /* 7. */ {(uint8_t *)"afgin",   strlen("afgin"),   {0x8}, true},
            /*end.*/ { NULL,                0,                 {0},   false},
        };

        struct trie *obj = trie_new(NULL, NULL);

        // 0. Prepare data and check it
        for (struct data *i = insertions; i->key != NULL; ++i) {
                void *old              = NULL;
                trie_insert(obj, i->key, i->size,
                            i->value_, old);
        }
        assert(check(insertions, obj));
        printf("0. [DONE] Data inserted \n");
        trie_export_dot(obj, "./trie.dot");


        struct data *delete;
        void *old;
        // 1. -------------------------
        delete = &insertions[3];
        delete->enabled = false;
        printf("delete key: %s\n", (char*)delete->key);
        assert(trie_remove(obj, delete->key, delete->size, &old));
        assert(old == delete->value_);
        bool ret = check(insertions, obj);
        assert(ret);
        printf("1. [DONE] --------------------\n");

        // 2. The midle node on the top
        delete = &insertions[4];
        delete->enabled = false;
        printf("delete key: %s\n", (char*)delete->key);
        assert(trie_remove(obj, delete->key, delete->size, &old));
        assert(old == delete->value_);
        ret = check(insertions, obj);
        assert(ret);
        printf("2. [DONE] The midle on the top\n");


        // 3. The last root's negative node
        delete = &insertions[5];
        delete->enabled = false;
        printf("delete key: %s\n", (char*)delete->key);
        assert(trie_remove(obj, delete->key, delete->size, &old));
        assert(old == delete->value_);
        ret = check(insertions, obj);
        assert(ret);
        printf("3. [DONE] The last root's negative node was deleted \n");

        // 4. The last negative node in the center
        delete = &insertions[7];
        delete->enabled = false;
        printf("delete key: %s\n", (char*)delete->key);
        assert(trie_remove(obj, delete->key, delete->size, &old));
        assert(old == delete->value_);
        ret = check(insertions, obj);
        assert(ret);
        printf("4. [DONE] The last negative node in the center\n");

        // 5.
        delete = &insertions[2];
        delete->enabled = false;
        printf("delete key: %s\n", (char*)delete->key);
        assert(trie_remove(obj, delete->key, delete->size, &old));
        assert(old == delete->value_);
        ret = check(insertions, obj);
        assert(ret);
        printf("5. [DONE] The midle node\n");

        // 6.
        delete = &insertions[0];
        delete->enabled = false;
        printf("delete key: %s\n", (char*)delete->key);
        assert(trie_remove(obj, delete->key, delete->size, &old));
        assert(old == delete->value_);
        ret = check(insertions, obj);
        assert(ret);
        printf("6. [DONE] --------------\n");

        // 7
        delete = &insertions[6];
        delete->enabled = false;
        printf("delete key: %s\n", (char*)delete->key);
        assert(trie_remove(obj, delete->key, delete->size, &old));
        assert(old == delete->value_);
        ret = check(insertions, obj);
        assert(ret);
        printf("7. [DONE] --------------\n");

        // 8
        delete = &insertions[1];
        delete->enabled = false;
        printf("delete key: %s\n", (char*)delete->key);
        assert(trie_remove(obj, delete->key, delete->size, &old));
        assert(old == delete->value_);
        ret = check(insertions, obj);
        assert(ret);
        printf("8. [DONE] --------------\n");


        return 0;
}
