#ifndef LIFETIME_H
#define LIFETIME_H

#ifndef DEBUG
#define DEBUG(...)
#else
#define DEBUG(...) printk(KERN_INFO __VA_ARGS__)
#endif

#define ERROR(...) printk(KERN_ERR __VA_ARGS__)
#define MASK 0xfffffff000000000
#define POISON(x) ((uint64_t)(x) | MASK)
#define IS_POISON(x) (((uint64_t)(x)&MASK) == MASK)

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