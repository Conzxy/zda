// SPDX-LICENSE-IDENTIFIER: MIT
#include "zda/avl_tree.h"
#include "zda/util/assert.h"
#include "zda/util/macro.h"
#include <stdio.h>
#include <string.h>

static zda_inline zda_bool _zda_avl_tree_is_root(zda_avl_tree_t *tree, zda_avl_node_t *node)
    zda_noexcept
{
  return node == tree->node;
}

static zda_inline void _zda_avl_tree_set_root(zda_avl_tree_t *tree, zda_avl_node_t *node)
    zda_noexcept
{
  tree->node = node;
}

static zda_inline void _zda_avl_tree_right_rotate(zda_avl_tree_t *tree, zda_avl_node_t *node)
    zda_noexcept
{
  /*
   *        B          A
   *      /  \   =>   / \
   *     A   C       SA  B
   *   /  \             /  \
   *  SA  SB           SB  C
   */
  assert(node->left);
  assert(tree->node);

  zda_avl_node_t *new_node = node->left;
  node->left               = new_node->right;
  if (node->left) node->left->parent = node;

  zda_avl_node_t *parent = node->parent;
  new_node->right        = node;
  node->parent           = new_node;
  if (_zda_avl_tree_is_root(tree, node)) {
    _zda_avl_tree_set_root(tree, new_node);
  } else {
    if (parent->left == node) {
      parent->left = new_node;
    } else {
      assert(parent->right == node);
      parent->right = new_node;
    }
  }
  new_node->parent = parent;
}

static zda_inline void _zda_avl_tree_left_rotate(zda_avl_tree_t *tree, zda_avl_node_t *node)
    zda_noexcept
{
  assert(node->right);
  assert(tree->node);

  zda_avl_node_t *new_node = node->right;
  node->right              = new_node->left;
  if (node->right) node->right->parent = node;

  zda_avl_node_t *parent = node->parent;
  new_node->left         = node;
  node->parent           = new_node;
  if (_zda_avl_tree_is_root(tree, node)) {
    _zda_avl_tree_set_root(tree, new_node);
  } else {
    if (parent->left == node) {
      parent->left = new_node;
    } else {
      assert(parent->right == node);
      parent->right = new_node;
    }
  }
  new_node->parent = parent;
}

static zda_inline size_t _zda_avl_node_compute_height(zda_avl_node_t *node) zda_noexcept
{
  return node ? node->height : 0;
}

static zda_inline void _zda_avl_node_update_height(zda_avl_node_t *node) zda_noexcept
{
  assert(node);
  const size_t lh = _zda_avl_node_compute_height(node->left);
  const size_t rh = _zda_avl_node_compute_height(node->right);

  node->height = zda_max(lh, rh) + 1;
}

/*
 *         node                 B
 *       /     \             /    \
 *      B(h+2) D(h)  ==>  A(h+1) node
 *    /      \             /     /    \
 *   A(h+1)   C(h)              C     D
 * /  \         \
 * h  h
 *
 *  The height of A or C must be (h+1) otherwise the height of B can't be (h+2).
 *  If H(A) == h + 1, Right-Rotation(node) can resolve it.
 *  If H(C) == h + 1 and H(A) == h, the right subtree will be unrebalanced state.
 *  We can use Left-Rotation(node->left) to conver it to former case.
 */
static zda_inline void _zda_avl_tree_fix_left_heavy(zda_avl_tree_t *tree, zda_avl_node_t *node)
    zda_noexcept
{
  assert(node->left);
  zda_avl_node_t *left = node->left;

  size_t lh = _zda_avl_node_compute_height(left->left);
  size_t rh = _zda_avl_node_compute_height(left->right);

  if (rh > lh) {
    _zda_avl_tree_left_rotate(tree, left);
    _zda_avl_node_update_height(left);
    _zda_avl_node_update_height(left->parent);
  }

  _zda_avl_tree_right_rotate(tree, node);
  _zda_avl_node_update_height(node);
  _zda_avl_node_update_height(node->parent);
}

/* symmetric operation of left fixup */
static zda_inline void _zda_avl_tree_fix_right_heavy(zda_avl_tree_t *tree, zda_avl_node_t *node)
    zda_noexcept
{
  assert(node->right);
  zda_avl_node_t *right = node->right;

  size_t lh = _zda_avl_node_compute_height(right->left);
  size_t rh = _zda_avl_node_compute_height(right->right);

  if (lh > rh) {
    _zda_avl_tree_right_rotate(tree, right);
    _zda_avl_node_update_height(right);
    _zda_avl_node_update_height(right->parent);
  }

  _zda_avl_tree_left_rotate(tree, node);
  _zda_avl_node_update_height(node);
  _zda_avl_node_update_height(node->parent);
}

static zda_inline void zda_avl_tree_after_insert(
    zda_avl_tree_t *tree,
    zda_avl_node_t *node,
    zda_avl_node_t *parent
) zda_noexcept
{
  size_t lh, rh, h;
  int    diff;

  node->parent = parent;
  for (; node; node = node->parent) {
    lh = _zda_avl_node_compute_height(node->left);
    rh = _zda_avl_node_compute_height(node->right);
    h  = zda_max(lh, rh) + 1;
    if (node->height == h) {
      break;
    }
    node->height = h;

    diff = lh - rh;
    if (diff >= 2) {
      _zda_avl_tree_fix_left_heavy(tree, node);
      /* Update currernt node to the new node that replace it */
      node = node->parent;
    } else if (diff <= -2) {
      _zda_avl_tree_fix_right_heavy(tree, node);
      node = node->parent;
    }
  }
}

void zda_avl_tree_insert_commit(
    zda_avl_tree_t       *tree,
    zda_avl_commit_ctx_t *p_ctx,
    zda_avl_node_t       *new_node
) zda_noexcept
{
  *p_ctx->pp_slot = new_node;
  if (!p_ctx->p_parent) {
    new_node->height = 1;
    return;
  }

  zda_avl_tree_after_insert(tree, new_node, p_ctx->p_parent);
}

/******************************/
/* Remove APIs */
/******************************/
static zda_inline void _zda_avl_tree_remove_fix(zda_avl_tree_t *tree, zda_avl_node_t *node)
    zda_noexcept
{
  size_t lh, rh, h;
  int    diff;

  for (; node; node = node->parent) {
    lh = _zda_avl_node_compute_height(node->left);
    rh = _zda_avl_node_compute_height(node->right);
    h  = zda_max(lh, rh) + 1;

    diff = lh - rh;
    if (node->height != h) {
      node->height = h;
    } else if (diff >= -1 && diff <= 1)
      break;

    if (diff >= 2) {
      _zda_avl_tree_fix_left_heavy(tree, node);
      node = node->parent;
    } else if (diff <= -2) {
      _zda_avl_tree_fix_right_heavy(tree, node);
      node = node->parent;
    }
  }
}

static zda_inline void _zda_avl_node_transplant(
    zda_avl_tree_t *tree,
    zda_avl_node_t *victim,
    zda_avl_node_t *node
) zda_noexcept
{
  zda_avl_node_t *parent = victim->parent;
  if (parent) {
    if (parent->left == victim) {
      parent->left = node;
    } else {
      assert(parent->right);
      parent->right = node;
    }
  }

  if (node) node->parent = parent;
  if (_zda_avl_tree_is_root(tree, victim)) {
    _zda_avl_tree_set_root(tree, node);
  }
}

void zda_avl_tree_remove_node(zda_avl_tree_t *tree, zda_avl_node_t *old_node) zda_noexcept
{
  assert(old_node);

  zda_avl_node_t *parent = old_node->parent;
  zda_avl_node_t *replace_node;
  if (!old_node->left) {
    /* Single node */
    if (!old_node->right) {
      _zda_avl_node_transplant(tree, old_node, NULL);
    } else {
      /* Right only */
      replace_node = old_node->right;
      _zda_avl_node_transplant(tree, old_node, replace_node);
      replace_node->left = old_node->left;
    }
  } else if (!old_node->right) {
    /* Left only*/
    replace_node = old_node->left;
    _zda_avl_node_transplant(tree, old_node, replace_node);
    replace_node->right = old_node->right;
  } else {
    /* Two children */
    replace_node = zda_avl_node_get_min(old_node->right);

    if (replace_node->parent == old_node) {
      assert(old_node->right == replace_node);
      _zda_avl_node_transplant(tree, old_node, replace_node);
      replace_node->left         = old_node->left;
      replace_node->left->parent = replace_node;
    } else {
      _zda_avl_node_transplant(tree, replace_node, replace_node->right);
      parent       = replace_node->parent;
      parent->left = replace_node->right;

      _zda_avl_node_transplant(tree, old_node, replace_node);
      replace_node->left = old_node->left;
      if (replace_node->left) replace_node->left->parent = replace_node;
      replace_node->right = old_node->right;
      if (replace_node->right) replace_node->right->parent = replace_node;
      _zda_avl_node_update_height(replace_node);
      _zda_avl_node_update_height(parent);
    }
  }

  _zda_avl_tree_remove_fix(tree, parent);
}

/**************************/
/* Debug APIs */
/**************************/
static zda_bool zda_avl_node_verify_properties(zda_avl_node_t *node) zda_noexcept
{
  size_t lh, rh;
  if (!node) return zda_true;
  lh = _zda_avl_node_compute_height(node->left);
  rh = _zda_avl_node_compute_height(node->right);

  int diff = lh - rh;
  if (diff >= 2 || diff <= -2) {
    return zda_false;
  }
  return zda_avl_node_verify_properties(node->left) && zda_avl_node_verify_properties(node->right);
}

zda_bool zda_avl_tree_verify_properties(zda_avl_tree_t *tree)
{
  return zda_avl_node_verify_properties(tree->node);
}

static zda_inline char *_make_new_prefix(char const *prefix, int entry_len, char const *new_tail)
{
  const int new_prefix_len = strlen(prefix) + entry_len + strlen(new_tail);
  char     *new_prefix     = (char *)malloc(new_prefix_len + 1);
  char     *ret            = new_prefix;
  new_prefix               = stpcpy(new_prefix, prefix);
  new_prefix               = stpcpy(new_prefix, new_tail);
  for (int i = 0; i < entry_len; ++i) {
    new_prefix[i] = ' ';
  }
  new_prefix  += entry_len;
  *new_prefix = 0;
  return ret;
}

static void _zda_avl_tree_print_tree(
    zda_avl_tree_t *tree,
    zda_avl_node_t *root,
    int(print_cb)(zda_avl_node_t const *),
    char const *prefix
)
{
  if (!root) {
    return;
  }

  int entry_len = print_cb(root);
  entry_len     += ((printf("(%zu)\n", root->height)) - 1 - 1);

  int has_left  = root->left != NULL;
  int has_right = root->right != NULL;

  if (!has_left && !has_right) {
    return;
  }

  fputs(prefix, stdout);
  if (has_right) {
    if (has_left) {
      printf("├── ");
    } else {
      printf("└── ");
    }
  }

  if (has_right) {
    char *new_prefix = ZDA_NULL;
    if (has_left && !(zda_avl_node_is_single(root->right))) {
      new_prefix = _make_new_prefix(prefix, entry_len, "|  ");
    } else {
      new_prefix = _make_new_prefix(prefix, entry_len, "   ");
    }
    assert(new_prefix);
    assert(root->right->parent == root);
    _zda_avl_tree_print_tree(tree, root->right, print_cb, new_prefix);
    free(new_prefix);
  }

  if (has_left) {
    /* The prefix has printed */
    if (has_right) {
      fputs(prefix, stdout);
    }
    printf("*── ");
    char *new_prefix = _make_new_prefix(prefix, entry_len, "   ");
    assert(root->left->parent == root);
    _zda_avl_tree_print_tree(tree, root->left, print_cb, new_prefix);
    free(new_prefix);
  }
}

void zda_avl_tree_print_tree(zda_avl_tree_t *tree, int(print_cb)(zda_avl_node_t const *))
{
  printf("Tree height: %zu\n", zda_avl_tree_get_height(tree));
  printf("Minimum: ");
  zda_avl_node_t *min_node = zda_avl_tree_get_first(tree);
  if (min_node) {
    print_cb(min_node);
  } else {
    printf("(nil)");
  }
  printf("\n");
  printf("Maximum: ");
  zda_avl_node_t *max_node = zda_avl_tree_get_last(tree);
  if (max_node) {
    print_cb(max_node);
  } else {
    printf("(nil)");
  }
  printf("\n");
  _zda_avl_tree_print_tree(tree, zda_avl_tree_get_root(tree), print_cb, "");
}