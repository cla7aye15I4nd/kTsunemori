#ifndef LIFETIME_H
#define LIFETIME_H

typedef struct memory_region
{
  uint64_t addr;
  size_t len;

  struct rb_node node;
  struct rb_root root;
} memory_region_t;

typedef struct escape_entry
{
  uint64_t addr;
  struct rb_node node;
} escape_t;

void __lifetime_start(void *ptr, size_t size);
void __lifetime_escape(void *ptr, void *loc);
void __lifetime_end(void *ptr);

#endif