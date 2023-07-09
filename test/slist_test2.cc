#include <zda/slist.hpp>
#include <zda/util/functor.hpp>
#include <gtest/gtest.h>

using namespace zda;

using entry_type = VEntry<int, zda_slist_node_t>;
using TestList   = zda::Slist<entry_type>;
static void prepare_entries(TestList &sl)
{
    for (int i = 0; i < 100; ++i) {
        auto entry   = (entry_type *)malloc(sizeof(entry_type));
        entry->value = i;
        sl.push_front(&entry->node);
    }
}

void iterate_list(TestList const &list)
{
    auto iter = list.begin();
    auto end  = list.end();

    for (; iter != end; ++iter) {
        printf("iter = %d\n", iter->value);
    }
}

TEST(slist_test, insert)
{
    TestList sl;
    prepare_entries(sl);

    iterate_list(sl);
}
