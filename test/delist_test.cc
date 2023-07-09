#include "zda/delist.h"

#include <gtest/gtest.h>

typedef struct int_entry {
    int               key;
    zda_delist_node_t node;
} int_entry_t;

TEST(delist_test, push_back)
{
    zda_delist_t dl;
    zda_delist_init(&dl);
    for (int i = 0; i < 10; ++i) {
        auto entry = (int_entry_t *)malloc(sizeof(int_entry_t));
        entry->key = i;

        zda_delist_push_back(&dl, &entry->node);
    }

    auto               node  = zda_delist_front_const(&dl);
    int_entry_t const *entry = zda_delist_entry(node, int_entry_t const);

    int i = 0;
    zda_delist_iterate(&dl)
    {
        auto entry = zda_delist_entry(pos, int_entry_t);

        EXPECT_EQ(i, entry->key);
        i++;
    }

    zda_delist_destroy_inplace(&dl, int_entry_t, free);
}

TEST(delist_test, pop_front)
{
    zda_delist_t dl;
    zda_delist_init(&dl);
    for (int i = 0; i < 10; ++i) {
        auto entry = (int_entry_t *)malloc(sizeof(int_entry_t));
        entry->key = i;

        zda_delist_push_back(&dl, &entry->node);
    }

    for (int i = 0; i < 10; ++i) {
        auto node  = zda_delist_pop_front(&dl);
        auto entry = zda_delist_entry(node, int_entry_t);
        free(entry);
    }

    EXPECT_TRUE(zda_delist_is_empty(&dl));

    zda_delist_destroy_inplace(&dl, int_entry_t, free);
}

