#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/rbtree.h>
#include <linux/slab.h>

#include "lifetime.h"

static struct rb_root regions = RB_ROOT;

memory_region_t *__lifetime_lookup(void *ptr)
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

void __lifetime_insert(void *ptr, size_t len)
{
  struct rb_node *parent = NULL;
  struct rb_node **new = &(regions.rb_node);

  memory_region_t *data = kmalloc(sizeof(memory_region_t), GFP_KERNEL);

  data->addr = (uint64_t)ptr;
  data->len = len;

  while (*new)
  {
    memory_region_t *region = container_of(*new, memory_region_t, node);
    parent = *new;
    if (ptr < (void *)region->addr)
      new = &((*new)->rb_left);
    else if (ptr > (void *)region->addr)
      new = &((*new)->rb_right);
    else return;
  }

  rb_link_node(&data->node, parent, new);
  rb_insert_color(&data->node, &regions);
}

void __lifetime_remove(void *ptr)
{
  memory_region_t *n = __lifetime_lookup(ptr);
  if (n != NULL)
    rb_erase(&n->node, &regions);
  else
    printk(KERN_INFO "Double Free Detected\n");
}

void __lifetime_start(void *ptr, size_t size)
{
  printk(KERN_INFO "Start: %px, len: %zu\n", ptr, size);

  if (ptr != NULL)
    __lifetime_insert(ptr, size);
}

void __lifetime_end(void *ptr)
{
  printk(KERN_INFO "End: %px\n", ptr);

  if (ptr != NULL)
    __lifetime_remove(ptr);
}

void __lifetime_escape(void *ptr, void *loc)
{
  // printk(KERN_INFO "Escape: %px, loc: %px\n", ptr, loc);
}