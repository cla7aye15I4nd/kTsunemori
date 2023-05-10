#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/rbtree.h>
#include <linux/slab.h>

#include "lifetime.h"

static struct rb_root regions = RB_ROOT;

static void __lifetime_escape_insert(struct rb_root *root, void *loc);
static void __lifetime_escape_remove(struct rb_root *root, void *loc);
static void __lifetime_escape_rb_free(memory_region_t *n, struct rb_node *node);

static memory_region_t *__lifetime_region_lookup(void *ptr);
static void __lifetime_region_insert(void *ptr, size_t len);
static void __lifetime_region_remove(void *ptr);

void __lifetime_start(void *ptr, size_t size)
{
  DEBUG(KERN_INFO "Start: %px, len: %zu\n", ptr, size);

  if (ptr != NULL)
    __lifetime_region_insert(ptr, size);
}

void __lifetime_end(void *ptr)
{
  DEBUG(KERN_INFO "End: %px\n", ptr);

  if (IS_POISON(ptr))
  {
    ERROR(KERN_INFO "Double Free Detected\n");
    return;
  }

  if (ptr != NULL)
    __lifetime_region_remove(ptr);
}

void __lifetime_escape(void *ptr, void *loc)
{
  memory_region_t *ptr_r;
  memory_region_t *old_r;
  uint64_t old = *(uint64_t *)loc;

  DEBUG(KERN_INFO "Escape: %px, loc: %px\n", ptr, loc);

  if (IS_POISON(ptr))
  {
    ERROR(KERN_INFO "Double Free Detected\n");
    return;
  }

  ptr_r = ptr == NULL ? NULL : __lifetime_region_lookup(ptr);
  if (ptr_r != NULL && (old < ptr_r->addr || ptr_r->addr + ptr_r->len <= old))
    __lifetime_escape_insert(&ptr_r->root, loc);

  if (old != 0 && !IS_POISON(old)) {
    old_r = __lifetime_region_lookup((void *)old);
    if (old_r != NULL)
      __lifetime_escape_remove(&old_r->root, loc);
  }
}

static void __lifetime_escape_insert(struct rb_root *root, void *loc)
{
  escape_t *data;
  struct rb_node *parent = NULL;
  struct rb_node **new = &(root->rb_node);

  while (*new)
  {
    escape_t *escape = container_of(*new, escape_t, node);
    parent = *new;
    if (loc < (void *)escape->addr)
      new = &((*new)->rb_left);
    else if (loc > (void *)escape->addr)
      new = &((*new)->rb_right);
    else
      return;
  }

  data = kmalloc(sizeof(escape_t), GFP_KERNEL);
  data->addr = (uint64_t)loc;

  rb_link_node(&data->node, parent, new);
  rb_insert_color(&data->node, root);
}

static void __lifetime_escape_remove(struct rb_root *root, void *loc)
{
  struct rb_node *node = root->rb_node;

  while (node)
  {
    escape_t *escape = container_of(node, escape_t, node);
    if (loc < (void *)escape->addr)
      node = node->rb_left;
    else if (loc > (void *)escape->addr)
      node = node->rb_right;
    else
    {
      rb_erase(&escape->node, root);
      kfree(escape);
      return;
    }
  }
}

static void __lifetime_escape_rb_free(memory_region_t * n, struct rb_node *node)
{
  if (node != NULL)
  {
    escape_t *escape = container_of(node, escape_t, node);
    __lifetime_escape_rb_free(n, node->rb_left);
    __lifetime_escape_rb_free(n, node->rb_right);

    uint64_t addr = *(uint64_t *)escape->addr;
    if (n->addr <= addr && addr < n->addr + n->len)
      *(uint64_t *)escape->addr = POISON(addr);

    kfree(escape);
  }
}

static memory_region_t *__lifetime_region_lookup(void *ptr)
{
  struct rb_node *node = regions.rb_node;

  while (node)
  {
    memory_region_t *region = container_of(node, memory_region_t, node);
    if (ptr < (void *)region->addr)
      node = node->rb_left;
    else if (ptr >= (void *)(region->addr + region->len))
      node = node->rb_right;
    else
      return region;
  }

  return NULL;
}

static void __lifetime_region_insert(void *ptr, size_t len)
{
  struct rb_node *parent = NULL;
  struct rb_node **new = &(regions.rb_node);

  memory_region_t *data = kmalloc(sizeof(memory_region_t), GFP_KERNEL);

  data->addr = (uint64_t)ptr;
  data->len = len;
  data->root = RB_ROOT;

  while (*new)
  {
    memory_region_t *region = container_of(*new, memory_region_t, node);
    parent = *new;
    if (ptr < (void *)region->addr)
      new = &((*new)->rb_left);
    else if (ptr > (void *)region->addr)
      new = &((*new)->rb_right);
    else
      return;
  }

  rb_link_node(&data->node, parent, new);
  rb_insert_color(&data->node, &regions);
}

static void __lifetime_region_remove(void *ptr)
{
  memory_region_t *n = __lifetime_region_lookup(ptr);
  if (n == NULL)
    return;

  __lifetime_escape_rb_free(n, n->root.rb_node);
  rb_erase(&n->node, &regions);

  kfree(n);
}
