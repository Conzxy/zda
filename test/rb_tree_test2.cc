#include "zda/rb_tree.h"
#include "zda/util/map_functor.hpp"
#include <zda/rb_tree.hpp>

#include <gtest/gtest.h>

using TestRbTree = zda::RbTree<int, zda::KEntry<int, zda_rb_node_t>>;