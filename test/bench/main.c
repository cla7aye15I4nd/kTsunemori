#include <linux/module.h>
#include <linux/slab.h>

#include "lifetime.h"

#define MEMORY (1024 * 1024 * 1024) // 1GB
#define MAXN 4
#define MAXM 4

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Zheng Yu");
MODULE_DESCRIPTION("");
MODULE_VERSION("1.0");

void noinline *kmalloc_wrapper(size_t size)
{
  void *ptr = kmalloc(size, GFP_KERNEL);
  return ptr;
}

static uint32_t randint(int min, int max)
{
  static uint32_t a = 0xdeadbeef, b = 0xcafebabe, x = 0x12345678;
  return (x = a * x + b) % (max - min + 1) + min;
}

struct Edge
{
  int node;
  struct Edge *next;
};

struct Edge **head;

static void add_edge(int u, int v)
{
  struct Edge *e = (struct Edge *)kmalloc_wrapper(sizeof(struct Edge));
  e->node = v;
  e->next = head[u];
  head[u] = e;
}

static int __init bench_init(void)
{
  int i;

  printk(KERN_INFO "Hello Benchmark !!\n");

  head = (struct Edge **)kmalloc_wrapper(sizeof(struct Edge *) * MAXN);
  memset(head, 0, sizeof(struct Edge *) * MAXN);

  for (i = 0; i < MAXM; ++i)
  {
    int u = randint(0, MAXN - 1);
    int v = randint(0, MAXN - 1);
    add_edge(u, v);
  }

  for (i = 0; i < MAXN; ++i)
  {
    struct Edge *e = head[i];
    while (e != NULL)
    {
      struct Edge *tmp = e;
      e = e->next;
      kfree(tmp);
    }
  }

  kfree(head);

  printk(KERN_INFO "Finish generating random graph\n");

  return 0;
}

static void __exit bench_exit(void)
{
  printk(KERN_INFO "Goodbye Benchmark!!\n");
}

module_init(bench_init);
module_exit(bench_exit);