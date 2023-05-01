#include <linux/module.h>
#include <linux/slab.h>

#define MEMORY (500 * 1024 * 1024) // 500MB
#define MAXN 0x10000
#define MAXM ((MEMORY - MAXN * sizeof(struct Edge *)) / sizeof(struct Edge))

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Zheng Yu");
MODULE_DESCRIPTION("");
MODULE_VERSION("1.0");

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
  struct Edge *e = (struct Edge *)kmalloc(sizeof(struct Edge), GFP_KERNEL);
  e->node = v;
  e->next = head[u];
  head[u] = e;
}

static int __init bench_init(void)
{
  int i;

  printk(KERN_INFO "Hello Benchmark !!\n");
  head = (struct Edge **)kmalloc(sizeof(struct Edge *) * MAXN, GFP_KERNEL);

  for (i = 0; i < MAXN * 0x100; ++i)
    head[i] = (struct Edge *)NULL;
  for (i = 0; i < MAXM; ++i)
  {
    int u = randint(0, MAXN - 1);
    int v = randint(0, MAXN - 1);
    add_edge(u, v);
  }

  return 0;
}

static void __exit bench_exit(void)
{
  kfree(head);
  printk(KERN_INFO "Goodbye Benchmark!!\n");
}

module_init(bench_init);
module_exit(bench_exit);