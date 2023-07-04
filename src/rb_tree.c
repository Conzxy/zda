// SPDX-LICENSE-IDENTIFIER: MIT
#include <stdio.h>
#include <string.h>

#include "zda/rb_tree.h"
#include "zda/util/macro.h"
#include "zda/util/tcolor.h"
#include "zda/util/assert.h"

/* To avoid the conflict with the __name of compiler and library, use _name to define the private
 * functions */

/*********************************/
/* Color APIs                    */
/*********************************/
static zda_inline int _zda_rb_node_is_red(zda_rb_node_t *node)
{
  return node->color == ZDA_RB_COLOR_RED;
}
static zda_inline int _zda_rb_node_is_black(zda_rb_node_t *node)
{
  return node->color == ZDA_RB_COLOR_BLACK;
}

static zda_inline void _zda_rb_node_set_color_from_node(zda_rb_node_t *lhs, zda_rb_node_t *rhs)
{
  lhs->color = rhs->color;
}

/******************************/
/* Link APIs                  */
/******************************/
static zda_inline int _zda_rb_node_is_single(zda_rb_header_t *header, zda_rb_node_t *node)
{
  return zda_rb_node_is_nil(header, node->left) && zda_rb_node_is_nil(header, node->right);
}
static zda_inline int _zda_rb_node_is_header(zda_rb_header_t *header, zda_rb_node_t *node)
{
  return node == &header->node;
}

static zda_inline int _rb_node_is_lean_left(zda_rb_node_t *node)
{
  return node->parent == node->parent->parent->left;
}
static zda_inline int _rb_node_is_lean_right(zda_rb_node_t *node)
{
  return node->parent == node->parent->parent->right;
}
static zda_inline int _rb_node_is_left(zda_rb_node_t *node) { return node->parent->left == node; }
static zda_inline int _rb_node_is_right(zda_rb_node_t *node) { return node->parent->right == node; }
static zda_inline void _zda_rb_node_set_root(zda_rb_header_t *header, zda_rb_node_t *new_root)
{
  header->node.parent = new_root;
}
static zda_inline int _zda_rb_node_is_root(zda_rb_header_t *header, zda_rb_node_t *node)
{
  // assert(!_zda_rb_node_is_header(header, node));
  return node->parent == &header->node && !_zda_rb_node_is_header(header, node);
}

static zda_inline void _rb_node_right_rotate(zda_rb_header_t *header, zda_rb_node_t *node)
{
  /*
   *        B          A
   *      /  \   =>   / \
   *     A   C       SA  B
   *   /  \             /  \
   *  SA  SB           SB  C
   */
  assert(node->left);
  zda_rb_node_t *new_root = node->left;

  node->left = new_root->right;
  if (!zda_rb_node_is_nil(header, node->left)) node->left->parent = node;

  new_root->right       = node;
  zda_rb_node_t *parent = node->parent;

  if (!_zda_rb_node_is_header(header, parent)) {
    if (parent->left == node) {
      parent->left = new_root;
    } else if (parent->right == node) {
      parent->right = new_root;
    }
  } else {
    _zda_rb_node_set_root(header, new_root);
  }
  new_root->parent = parent;
  node->parent     = new_root;
}

static zda_inline void _rb_node_left_rotate(zda_rb_header_t *header, zda_rb_node_t *node)
{
  /* Similar to the right rotate. */
  assert(node->right);
  zda_rb_node_t *new_root = node->right;

  node->right = new_root->left;
  if (!zda_rb_node_is_nil(header, node->right)) node->right->parent = node;

  new_root->left        = node;
  zda_rb_node_t *parent = node->parent;
  if (!_zda_rb_node_is_header(header, parent)) {
    if (parent->left == node) {
      parent->left = new_root;
    } else if (parent->right == node) {
      parent->right = new_root;
    }
  } else {
    _zda_rb_node_set_root(header, new_root);
  }
  new_root->parent = parent;
  node->parent     = new_root;
}

static zda_inline void _zda_rb_header_set_minimum(zda_rb_header_t *header, zda_rb_node_t *node)
{
  header->node.left = node;
}

static zda_inline void _zda_rb_header_set_maximum(zda_rb_header_t *header, zda_rb_node_t *node)
{
  header->node.right = node;
}

static zda_inline int _zda_rb_header_is_minimum(zda_rb_header_t *header, zda_rb_node_t *node)
{
  return header->node.left == node;
}

static zda_inline int _zda_rb_header_is_maximum(zda_rb_header_t *header, zda_rb_node_t *node)
{
  return header->node.right == node;
}

zda_rb_node_t *zda_rb_node_get_min_entry(zda_rb_header_t *header, zda_rb_node_t *root)
{
  /* If the left subtree does exists, find the leftest node */
  while (root->left != &header->node) {
    root = root->left;
  }
  return root;
}

zda_rb_node_t *zda_rb_node_get_max_entry(zda_rb_header_t *header, zda_rb_node_t *root)
{
  /* If the right subtree does exists, find the mostest node */
  while (!zda_rb_node_is_nil(header, root->right)) {
    root = root->right;
  }
  return root;
}

zda_rb_node_t *zda_rb_node_get_successor(zda_rb_header_t *header, zda_rb_node_t *node)
{
  /* If the right subtree does exists, find the minimum of it */
  if (!zda_rb_node_is_nil(header, node->right)) {
    return zda_rb_node_get_min_entry(header, node->right);
  }

  /* Otherwise, find the successor of parents.
   * If node is the left child, the parent is the successor.
   * Suppose the parent is the left child, the grandpa > parent.
   * Suppose the parent is the right child, the grandpa < node
   *
   * If node is the right child, find bottom up, until coming the first node is the left child of
   * child.
   * Because nodes in the find path > node.
   */
  zda_rb_node_t *parent = node->parent;
  while (parent->right == node) {
    node   = parent;
    parent = node->parent;
  }

  /*
   * when parent == header and node == root,
   * then parent == root, node == header, will break the loop and
   * I expect to return header as the terminator.
   *
   * 1. If the `header->left` pointer to the maximum node,
   * ie. `header->right` pointer to the minimum node.
   *  1.1 `parent->right` may pointer to the root when root is the minimum node.
   * the parent will be the root and return it.
   * This will trigger infinite iterate of tree nodes.
   *
   *  1.2 `parent->right` may pointer to the root whien root is the maximum node.
   * ie. the tree contains the root only.
   * the parent will be the root.
   *
   * 2. If the `header->right` pointer to the maximum node,
   *  2.1 The `parent->right` don't pointer to the minimum node when the nodes count of tree > 1.
   *  2.2 `parent->right` may pointer to the maximum node when root is the only node of tree.
   * the parent will be the root.
   *
   * Becase the 2. cases need to handle is less than 1.
   * I select the 2. plan.
   */

  /* The reverse condition can meet only when node is the header */
  if (node->right == parent) {
    return node;
  }

  return parent;
}

zda_rb_node_t *zda_rb_node_get_predecessor(zda_rb_header_t *header, zda_rb_node_t *node)
{
  if (!zda_rb_node_is_nil(header, node->left)) {
    return zda_rb_node_get_min_entry(header, node->left);
  }

  zda_rb_node_t *parent = node->parent;

  /* parent must exists since the header */
  while (parent->right == node) {
    node   = parent;
    parent = node->parent;
  }
  return parent;
}

void zda_rb_tree_entry_reuse(zda_rb_header_t *header, void(entry_cb)(zda_rb_node_t *node))
{
  zda_rb_node_t *root = zda_rb_tree_get_root(header);
  while (!_zda_rb_node_is_header(header, root)) {
    if (!zda_rb_node_is_nil(header, root->left)) {
      root = root->left;
    } else if (!zda_rb_node_is_nil(header, root->right)) {
      root = root->right;
    } else {
      zda_rb_node_t *parent = root->parent;
      /* No need to check whether parent does exists*/
      if (parent->left == root) {
        _zda_rb_node_set_nil(header, parent->left);
      } else if (parent->right == root) {
        _zda_rb_node_set_nil(header, parent->right);
      }
      entry_cb(root);
      root = parent;
    }
  }
}

/************************************/
/* Insert APIs */
/************************************/

/* clang-format off */
/* We think the RB-tree as a 2-3 tree.
  * To the lean left case, then split to 5 cases:
  * | B | -- This is 2-node
  * Case 1: If the parent of new node is black 2-node, do nothing.
  * Case 2:
  * | R | -- This is red 2-node.
  * since this is lean left, and the read 2-node in 2-3 tree must be splited by a 4-node.
  * i.e. | R | B | R |
  * in this case, we can flip the color of left child of `B` node(ie. the parent of new node) to make the 
  * new node can be merged by the child to decrease the height, but this also need to flip the color
  * of the right child of `B` node otherwise the right child also will be merged to the `B` node.
  * In the end, flip the `B` node to red and this make it can be merged to parent of it.
  * In this case, the `B` node becomes the equivalent "new_node" of parent tree until rebalance.
  * In the end, the height of tree will increment.
  *
  * | R | B | --- This is a 3-node(lean left)
  * there are three insertion points.
  * The new node must be a 2-node
  * The right of the `B` node must not be red node.
  *
  * Case 3: If insert 2-node to the left of `R` node, rotation and recolor can make the height
  * rebalance.
  * Case 4: If insert 2-node to the middle between `R` and `B` node, rotation can
  * convert this to case 3.
  * Case 5: If insert 2-node to the right of `B` node, the 3-node will be a
  * 4-node and split to three 3-nodes. We can flip the colors of children and parent, thus we can
  * push the `B` node to parent, this make height reblance of the subtree.
  * But we also need to consider the parent:
  * If the parent is a 2-node, it will be a 3-node and rebalance ok.
  * Otherwise, the parent becomes a 4-node will split also and repeat the process until the root
  * becomes 4-node and split to three 2-node.
  * 
  * In fact, the case 5 is same with the case 2.
  * The cases must be handled are case 2, 3, 4.
  * 
  * case 2: this is a left single child.
  * case 3: this is a left single child.
  * case 4: this is a right single child.
  * According the relation between parent and children, we can't distinguish them.
  * when we focus on the grandpa, the latter two cases(3, 4), the `B` node doesn't has red right child,
  * but the `B` node of case 2 has a red right child.
  * Thus, the uncle color can be a different point among them.
  */
/* clang-format on */
static zda_inline void _zda_rb_tree_insert_fixup(zda_rb_header_t *header, zda_rb_node_t *new_node)
{
  assert(new_node->parent);
  /* zda_rb_node_init(header, new_node); */

  zda_rb_node_t *parent = new_node->parent;
  while (parent->color == ZDA_RB_COLOR_RED) {
    /* lean left case, i.e. the parent of new node is a left child */
    if (_rb_node_is_lean_left(new_node)) {
#define _rb_tree_insert_fixup_routine(link, rotate)                                                \
  zda_rb_node_t *grandpa = parent->parent;                                                         \
  zda_rb_node_t *uncle   = grandpa->link;                                                          \
  /* The uncle must not be NULL */                                                                 \
  if (_zda_rb_node_is_red(uncle)) {                                                                \
    /* case 2 */                                                                                   \
    assert(_zda_rb_node_is_red(parent));                                                           \
    assert(_zda_rb_node_is_black(grandpa));                                                        \
    _zda_rb_node_set_black(parent);                                                                \
    _zda_rb_node_set_black(uncle);                                                                 \
    _zda_rb_node_set_red(grandpa);                                                                 \
    new_node = grandpa;                                                                            \
    parent   = new_node->parent;                                                                   \
  } else {                                                                                         \
    /* case 4 */                                                                                   \
    if (new_node == parent->link) {                                                                \
      _rb_node_##rotate##_rotate(header, parent);                                                  \
      new_node = parent;                                                                           \
      parent   = new_node->parent;                                                                 \
      grandpa  = parent->parent;                                                                   \
    }                                                                                              \
    /* case 3 */                                                                                   \
    _rb_node_##link##_rotate(header, grandpa);                                                     \
    _zda_rb_node_set_black(parent);                                                                \
    _zda_rb_node_set_red(grandpa);                                                                 \
    assert(_zda_rb_node_is_black(new_node->parent));                                               \
  }

      _rb_tree_insert_fixup_routine(right, left)
    } else {
      /* lean right */
      _rb_tree_insert_fixup_routine(left, right)
    }
  }
  /* If the case 2 up to root, can't rebalance, will flip the children of root to black and the root
   * to red.
   * This will make the root can be merged with one child and the balance is violated again.
   * flip the root to black can avoid it. */
  if (_zda_rb_node_is_root(header, new_node)) {
    _zda_rb_node_set_black(new_node);
  }
}

void zda_rb_node_link(zda_rb_header_t *header, zda_rb_node_t *node, zda_rb_node_t *parent)
{
  if (parent->left == node) {
    if (_zda_rb_header_is_minimum(header, parent)) {
      _zda_rb_header_set_minimum(header, node);
    }
  } else if (parent->right == node) {
    if (_zda_rb_header_is_maximum(header, parent)) {
      _zda_rb_header_set_maximum(header, node);
    }
  } else {
    _zda_rb_node_set_black(node);
    header->node.left = header->node.right = node;
  }

  node->parent = parent;
}

void zda_rb_node_insert_rebalance(
    zda_rb_header_t *header,
    zda_rb_node_t   *node,
    zda_rb_node_t   *parent
)
{
  if (_zda_rb_node_is_red(parent)) {
    _zda_rb_tree_insert_fixup(header, node);
  }
}

void zda_rb_node_after_insert(zda_rb_header_t *header, zda_rb_node_t *node, zda_rb_node_t *parent)
{
  zda_rb_node_init(header, node);
  zda_rb_node_link(header, node, parent);
  zda_rb_node_insert_rebalance(header, node, parent);
}

void zda_rb_tree_find_insert_slot_eq(
    zda_rb_header_t *header,
    void const      *key,
    zda_rb_cmp_t     cmp,
    zda_rb_node_t ***p_dup,
    zda_rb_node_t  **parent
)
{
  assert(p_dup);
  zda_rb_node_t **p_slot       = zda_rb_tree_get_p_root(header);
  zda_rb_node_t  *track_parent = (*p_slot)->parent;
  for (; !zda_rb_node_is_nil(header, *p_slot);) {
    int res = cmp(*p_slot, key);
    if (res <= 0) {
      track_parent = *p_slot;
      p_slot       = &(*p_slot)->right;
    } else if (res > 0) {
      track_parent = *p_slot;
      p_slot       = &(*p_slot)->left;
    }
  }
  *p_dup  = p_slot;
  *parent = track_parent;
}

int zda_rb_tree_find_insert_slot(
    zda_rb_header_t *header,
    void const      *key,
    zda_rb_cmp_t     cmp,
    zda_rb_node_t ***p_dup,
    zda_rb_node_t  **parent
)
{
  assert(p_dup);
  zda_rb_node_t **p_slot       = zda_rb_tree_get_p_root(header);
  zda_rb_node_t  *track_parent = (*p_slot)->parent;
  for (; !zda_rb_node_is_nil(header, *p_slot);) {
    int res = cmp(*p_slot, key);
    if (res < 0) {
      track_parent = *p_slot;
      p_slot       = &(*p_slot)->right;
    } else if (res > 0) {
      track_parent = *p_slot;
      p_slot       = &(*p_slot)->left;
    } else {
      *p_dup = p_slot;
      return 0;
    }
  }
  *p_dup  = p_slot;
  *parent = track_parent;
  return 1;
}

/***************************************/
/* Search APIs */
/***************************************/
zda_rb_node_t *zda_rb_tree_search(zda_rb_header_t *header, void const *key, zda_rb_cmp_t cmp)
{
  zda_rb_node_t *p_result = &header->node;
  zda_rb_node_t *root     = zda_rb_tree_get_root(header);
  while (!zda_rb_node_is_nil(header, root)) {
    int res = cmp(root, key);
    if (res < 0) {
      root = root->right;
    } else if (res > 0) {
      root = root->left;
    } else {
      p_result = root;
      break;
    }
  }
  return p_result;
}

/*--------------------------------------------------------------------------------------*/
/* Remove aux */
/*--------------------------------------------------------------------------------------*/

zda_rb_node_t *zda_rb_tree_remove(zda_rb_header_t *header, void const *key, zda_rb_cmp_t cmp)
{
  zda_rb_node_t *old_node = zda_rb_tree_search(header, key, cmp);
  if (old_node) {
    zda_rb_tree_remove_node(header, old_node);
    return old_node;
  }
  return &header->node;
}

/* clang-format off */
/*
  * Thinking the rb-tree as a 2-3-4 tree.
  *
  * 2-node: single black node
  * 3-node: single black node with a red child, thus allow lean: lean left(child in the left) and
  * lean right.
  * 4-node: black node with two red children.
  *
  * If we remove the red node, the balance of tree isn't violated.
  * The removed black node will decrement the height of the subtree. We use a node(called `x`) to
  * replace it and need to rebalance the subtree of `x->parent`.
  *
  * (Following cases only consider the lean left case, ie. the `x->parent->left == x->parent`)
  * ------------------------------------------------------------------------------------------------
  * Case 1: sibling is black node of the lean right 3-node.
  * We can steal a node from the sibling and flip the color if the color is not expected.
  * Using right rotation can achieve it.
  *      B(?)                       D(B) -> D(?)
  *    /     \                    /            \
  *  A(B)    D(B) - E(R)   =>    B(?)-> B(B)    E(R) -> E(B)
  *         /             LR(B) /   \
  *        C(?)                A(B)  C(?)
  * First, the D should color to same color of B since we don't know the link color(red-link or
  * black link) and keep it as original.
  * * If B is black, it is a single 2-node.
  * * If B is red, it is a node of 3-node or 4-node.
  * The B node can't be merged to children, D is also like this.
  * The rebalance target is all path contains two black nodes(ie. 2 height of 2-3-4 tree)
  * 
  * C has a black node in its path(or C is the black node), and A is black also.
  * flip the B to black, the left height rebalance.
  * E has a black node in its path, so
  * flip the E to black, the right height rebalance.
  *
  * ------------------------------------------------------------------------------------------------
  * Case 2: sibling is the black node of the lean left node.
  *      B(?)                        F(B)
  *    /           \                /   \
  *   A(B)  D(R) - F(B)   =>      B(?)  G(?)
  *        /   \     \   LR(B)   /    \
  *       C(B) E(B)  G(?)       A(B)   D(R)
  *                                   /    \
  *                                 C(B)   E(B)
  * If the B is red node, rebalance is ok.
  * The reason is the D is moved to the left subtree.
  * We can transform this case to case 1 by rotation and color flip.
  * 
  *      B(?)                     B(?)                         B(?)
  *    /           \             /    \                       /    \
  *   A(B)  D(R) - F(B)   =>   A(B)   D(R)->D(B)      ==   A(B)  D(B) - F(R)
  *        /   \     \    RR(F)      /   \                         /     /   \
  *       C(B) E(B)  G(?)         C(B)   F(B)->F(R)              C(B)   E(B) G(?)
  *                                    /    \
  *                                  E(B)   G(?)
  * ------------------------------------------------------------------------------------------------
  * Case 3: sibling is the black node in 4-node.
  * This case is equivalent to lean right 3-node(ie. case 1)
  *
  * ------------------------------------------------------------------------------------------------
  * Case 4: sibling is the black 2-node.
  *      B(?)                      
  *     /     \                 
  *    A(B)    D(B)     
  *          /     \            
  *         C(B)   E(B)         
  * We can't steal the node from sibling since it is a 2-node,
  * but we can flip the sibling to red, this will make the right subtree has same height with left.
  * If `B` is a red node, this decrement the height of `B`. we can flip it to black to rebalance the `B` subtree.
  * If `B` is a black node, we can't fixup this in this case, but notice the `A` and `D` has rebalanced, we can fixup
  * the unbalanced node, `B` node, transform it to case 1/2/3/5 or case 4 until a node in the path up to root 
  * can fixup it.
  * ------------------------------------------------------------------------------------------------
  * Case 5: sibling is the red right child of 3-node
  *      B(B) - D(R)                  B(B)->B(R) - D(R) -> D(B)
  *     /       /  \           =>    /        \           \
  *    A(B)    C(B) E(B)      LR(B) A(B)      C(B)        E(B)
  * The sibiling is the red node of a 3-node.
  * We can't steal its node.
  * Using rotation to make a black sibling(case 1, 2, 3)
  */
/* clang-format on */
static zda_inline void _rb_tree_remove_fixup(
    zda_rb_header_t *header,
    zda_rb_node_t   *old_node,
    zda_rb_node_t   *parent
)
{
  while (!_zda_rb_node_is_root(header, old_node)) {
    /* Lean left case */
    if (parent->left == old_node) {
      /* lean left: right, left */
#define _rb_tree_remove_fixup_routine(link, rotate)                                                \
  zda_rb_node_t *sibling = parent->link;                                                           \
                                                                                                   \
  /* Case 5: sibling is a red node of 3-node                                                       \
     Transform to case 1/2/3/4                                                                     \
   */                                                                                              \
  if (_zda_rb_node_is_red(sibling)) {                                                              \
    _rb_node_##rotate##_rotate(header, parent);                                                    \
    _zda_rb_node_set_red(parent);                                                                  \
    _zda_rb_node_set_black(sibling);                                                               \
                                                                                                   \
    sibling = parent->link;                                                                        \
  } else {                                                                                         \
    assert(_zda_rb_node_is_black(sibling));                                                        \
    /* Case 1, 3: sibling is black node of the lean right 3-node */                                \
    if (_zda_rb_node_is_red(sibling->link)) {                                                      \
      _remove_sibling_##link##_red_handler : _rb_node_##rotate##_rotate(header, parent);           \
      _zda_rb_node_set_color_from_node(sibling, parent);                                           \
      if (_zda_rb_node_is_red(parent)) {                                                           \
        _zda_rb_node_set_black(parent);                                                            \
      }                                                                                            \
      _zda_rb_node_set_black(sibling->link);                                                       \
      /* Rebalance complete, can break */                                                          \
      break;                                                                                       \
    } else {                                                                                       \
      /* Case 2 */                                                                                 \
      if (_zda_rb_node_is_red(sibling->rotate)) {                                                  \
        /* Transform to case 1 */                                                                  \
        _rb_node_##link##_rotate(header, sibling);                                                 \
        sibling = parent->link;                                                                    \
        assert(_zda_rb_node_is_red(sibling));                                                      \
        _zda_rb_node_set_black(sibling);                                                           \
        assert(_zda_rb_node_is_black(sibling->link));                                              \
        _zda_rb_node_set_red(sibling->link);                                                       \
        goto _remove_sibling_##link##_red_handler;                                                 \
      } else {                                                                                     \
        /* Case 4: black 2-node */                                                                 \
        _zda_rb_node_set_red(sibling);                                                             \
        if (_zda_rb_node_is_red(parent)) {                                                         \
          _zda_rb_node_set_black(parent);                                                          \
          break;                                                                                   \
        }                                                                                          \
                                                                                                   \
        /* The old_node up to parent until rebalance or root */                                    \
        old_node = parent;                                                                         \
        parent   = old_node->parent;                                                               \
      }                                                                                            \
    }                                                                                              \
  }

      _rb_tree_remove_fixup_routine(right, left);
    } else {
      _rb_tree_remove_fixup_routine(left, right);
    }
  }
}

static zda_inline void _zda_rb_node_transplant(
    zda_rb_header_t *header,
    zda_rb_node_t   *old_node,
    zda_rb_node_t   *new_node
)
{
  zda_rb_node_t *old_parent = old_node->parent;

  if (_zda_rb_node_is_root(header, old_node)) {
    /* If the old_node is root and the root is the only node of tree,
     * this will set root to header.
     * This is the expected behavior. */
    _zda_rb_node_set_root(header, new_node);
  }
  if (old_parent->left == old_node) {
    old_parent->left = new_node;
  } else if (old_parent->right == old_node) {
    old_parent->right = new_node;
  }

  if (!zda_rb_node_is_nil(header, new_node)) new_node->parent = old_parent;
  if (_zda_rb_node_is_black(old_node)) _zda_rb_node_set_black(new_node);
  /* Don't relink the new_node->left/right and new_node->left/right->parent
   * If the old_node is the parent of new_node, new_node->left will be itself.
   * These links should be set properly by caller */
}

static zda_inline void _rb_tree_remove_node(zda_rb_header_t *header, zda_rb_node_t *old_node)
{
  zda_assert(old_node != ZDA_RB_HEADER_NODE, "The header node can't be removed");
  _zda_rb_color_e replace_old_color;
  /* Update the header->left and header->right only happend when
   * The removed node must don't hold two children. */
  zda_rb_node_t  *old_parent;
  if (zda_rb_node_is_nil(header, old_node->left)) {
    /* Left is nil link */
    int is_min = _zda_rb_header_is_minimum(header, old_node);
    /* Right exists */
    if (!zda_rb_node_is_nil(header, old_node->right)) {
      if (is_min) {
        _zda_rb_header_set_minimum(header, zda_rb_node_get_min_entry(header, old_node->right));
      }
    } else {
      if (is_min) {
        /* single node */
        /* old_node is the minimum node of tree, must be a left child */
        assert(old_node->parent->left == old_node);
        _zda_rb_header_set_minimum(header, old_node->parent);
        if (_zda_rb_header_is_maximum(header, old_node)) {
          assert(_zda_rb_node_is_root(header, old_node));
          _zda_rb_header_set_maximum(header, old_node->parent);
        }
      }
    }
    replace_old_color = old_node->right->color;
    old_parent        = old_node->parent;
    _zda_rb_node_transplant(header, old_node, old_node->right);
    old_node = old_node->right;
  } else if (zda_rb_node_is_nil(header, old_node->right)) {
    /* Left exists */
    assert(old_node->left);
    if (_zda_rb_header_is_maximum(header, old_node)) {
      _zda_rb_header_set_maximum(header, zda_rb_node_get_max_entry(header, old_node->left));
    }
    replace_old_color = old_node->left->color;
    old_parent        = old_node->parent;
    _zda_rb_node_transplant(header, old_node, old_node->left);
    old_node = old_node->left;
  } else {
    /* Two children case */
    zda_rb_node_t *min_node = zda_rb_node_get_min_entry(header, old_node->right);
    if (min_node->parent == old_node) {
      replace_old_color = min_node->right->color;
      old_parent        = old_node->parent;
      _zda_rb_node_transplant(header, old_node, min_node);
      min_node->left = old_node->left;
      old_node       = min_node->right;
    } else {
      zda_rb_node_t *tmp_old_node = min_node->right;
      replace_old_color           = tmp_old_node->color;
      old_parent                  = min_node->parent;
      _zda_rb_node_transplant(header, min_node, min_node->right);
      _zda_rb_node_transplant(header, old_node, min_node);
      min_node->left  = old_node->left;
      min_node->right = old_node->right;
      if (!zda_rb_node_is_nil(header, min_node->left)) min_node->left->parent = min_node;
      if (!zda_rb_node_is_nil(header, min_node->right)) min_node->right->parent = min_node;
      old_node = tmp_old_node;
    }
  }

  /* If the original color is BLACK and the color of replacer is BLACK also, need to relance.
   * 1. remove a red node from a 2-3-4 tree don't break the 3-node or 4-node
   * to decrease the height of (sub)tree.
   * 2. If the color of replacer is RED, ie. remove a black node from 3-node or 4-node,
   * we can flip the the replacer to black and rebalance complete.  */
  if (_zda_rb_node_is_black(old_node) && replace_old_color == ZDA_RB_COLOR_BLACK) {
    _rb_tree_remove_fixup(header, old_node, old_parent);
  }
}

void zda_rb_tree_remove_node(zda_rb_header_t *header, zda_rb_node_t *old_node)
{
  _rb_tree_remove_node(header, old_node);
}

/****************************************/
/* Debug APIs */
/****************************************/
size_t zda_rb_tree_get_height(zda_rb_header_t *header, zda_rb_node_t *root)
{
  size_t lh = 0;
  if (!zda_rb_node_is_nil(header, root->left)) {
    lh = zda_rb_tree_get_height(header, root->left);
  }

  size_t rh = 0;
  if (!zda_rb_node_is_nil(header, root->right)) {
    rh = zda_rb_tree_get_height(header, root->right);
  }
  return zda_max(lh, rh) + (zda_rb_node_is_nil(header, root) ? 0 : 1);
}

size_t zda_rb_tree_get_234_height(zda_rb_header_t *header, zda_rb_node_t *root)
{
  size_t lh = 0;
  if (!zda_rb_node_is_nil(header, root->left)) {
    lh = zda_rb_tree_get_234_height(header, root->left);
  }

  size_t rh = 0;
  if (!zda_rb_node_is_nil(header, root->right)) {
    rh = zda_rb_tree_get_234_height(header, root->right);
  }

  assert(lh == rh);
  return lh + (zda_rb_node_is_nil(header, root) ? 0 : (_zda_rb_node_is_black(root) ? 1 : 0));
}

int _zda_rb_tree_check_red_link(zda_rb_header_t *header, zda_rb_node_t *root)
{
  if (zda_rb_node_is_nil(header, root)) return 1;
  if (_zda_rb_node_is_red(root)) {
    if (_zda_rb_node_is_red(root->parent) || _zda_rb_node_is_red(root->left) ||
        _zda_rb_node_is_red(root->right))
    {
      return 0;
    }
  }
  return _zda_rb_tree_check_red_link(header, root->left) &&
         _zda_rb_tree_check_red_link(header, root->right);
}

int zda_rb_tree_verify_properties(zda_rb_header_t *header)
{
  zda_rb_node_t *root = zda_rb_tree_get_root(header);
  if (!_zda_rb_node_is_black(root)) {
    printf("Violate: The root must be black node");
    return 0;
  }

  size_t lbh = zda_rb_tree_get_234_height(header, root->left);
  size_t rbh = zda_rb_tree_get_234_height(header, root->right);

  if (lbh != rbh) {
    printf("Violate: The balance is broken");
    return 0;
  }

  if (!_zda_rb_tree_check_red_link(header, root)) {
    printf("Violate: There has two near red node");
    return 0;
  }
  return 1;
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

static void _zda_rb_tree_print_tree(
    zda_rb_header_t *header,
    zda_rb_node_t   *root,
    int(print_cb)(void const *),
    char const *prefix
)
{
  if (zda_rb_node_is_nil(header, root)) {
    return;
  }

  if (_zda_rb_node_is_red(root)) {
    fputs(RED, stdout);
  }
  const int entry_len = print_cb(root);
  if (_zda_rb_node_is_red(root)) {
    fputs(NONE, stdout);
  }
  fputs("\n", stdout);

  int has_left  = !zda_rb_node_is_nil(header, root->left);
  int has_right = !zda_rb_node_is_nil(header, root->right);

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
    if (has_left && !(_zda_rb_node_is_single(header, root->right))) {
      new_prefix = _make_new_prefix(prefix, entry_len, "|  ");
    } else {
      new_prefix = _make_new_prefix(prefix, entry_len, "   ");
    }
    assert(new_prefix);
    _zda_rb_tree_print_tree(header, root->right, print_cb, new_prefix);
    free(new_prefix);
  }

  if (has_left) {
    /* The prefix has printed */
    if (has_right) {
      fputs(prefix, stdout);
    }
    printf("*── ");
    char *new_prefix = _make_new_prefix(prefix, entry_len, "   ");
    _zda_rb_tree_print_tree(header, root->left, print_cb, new_prefix);
    free(new_prefix);
  }
}

void zda_rb_tree_print_tree(zda_rb_header_t *header, int(print_cb)(void const *))
{
  printf("Tree height: %zu\n", zda_rb_tree_get_height(header, zda_rb_tree_get_root(header)));
  printf("Minimum: ");
  zda_rb_node_t *min_node = zda_rb_tree_get_min_entry(header);
  if (!zda_rb_node_is_nil(header, min_node)) {
    print_cb(min_node);
  } else {
    printf("(nil)");
  }
  printf("\n");
  printf("Maximum: ");
  zda_rb_node_t *max_node = zda_rb_tree_get_max_entry(header);
  if (!zda_rb_node_is_nil(header, max_node)) {
    print_cb(max_node);
  } else {
    printf("(nil)");
  }
  printf("\n");
  _zda_rb_tree_print_tree(header, zda_rb_tree_get_root(header), print_cb, "");
}
