#include <gtest/gtest.h>

#include <zda/delist.hpp>

using namespace zda;

struct int_entry_t {
    int key;
    ZDA_DELIST_HOOK;
};

void prepare_entries(Delist<int_entry_t> &dl, int n)
{
    for (int i = 0; i < n; ++i) {
        auto entry = (int_entry_t *)malloc(sizeof(int_entry_t));
        entry->key = i;
        dl.push_back(&entry->node);
    }
}

TEST(delist_test, insert)
{
    Delist<int_entry_t> dl;
    prepare_entries(dl, 100);

    int i = 0;
    for (auto const &entry : dl) {
        EXPECT_EQ(entry.key, i);
        ++i;
    }

    Delist<int_entry_t> const *p_dl = &dl;

    auto iter = p_dl->begin();
    EXPECT_EQ(i, 100);
}

TEST(delist_test, move)
{
    Delist<int_entry_t> dl;
    prepare_entries(dl, 100);

    auto dl2 = std::move(dl);

    int i = 0;
    for (auto const &entry : dl2) {
        EXPECT_EQ(entry.key, i);
        ++i;
    }
    EXPECT_EQ(i, 100);
}
