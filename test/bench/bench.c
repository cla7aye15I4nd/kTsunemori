#include <linux/module.h>
#include <linux/slab.h>

// #define MEMORY (1024 * 1024 * 1024) // 1GB
// #define MAXN 0x10000
#define MEMORY 1024
#define MAXN 10
#define MAXM ((MEMORY - MAXN * sizeof(struct Edge *)) / sizeof(struct Edge))

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Zheng Yu");
MODULE_DESCRIPTION("");
MODULE_VERSION("1.0");

void *malloc(size_t size)
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
  // struct Edge *e = (struct Edge *)kmalloc(sizeof(struct Edge), GFP_KERNEL);
  struct Edge *e = (struct Edge *)malloc(sizeof(struct Edge));
  e->node = v;
  e->next = head[u];
  head[u] = e;
}

static int __init bench_init(void)
{
  int i;

  printk(KERN_INFO "Hello Benchmark !!\n");
  // head = (struct Edge **)kmalloc(sizeof(struct Edge *) * MAXN, GFP_KERNEL);
  head = (struct Edge **)malloc(sizeof(struct Edge *) * MAXN);

  // for (i = 0; i < MAXN * 0x100; ++i)
  //   head[i] = (struct Edge *)NULL;
  for (i = 0; i < MAXM; ++i)
  {
    int u = randint(0, MAXN - 1);
    int v = randint(0, MAXN - 1);
    add_edge(u, v);
  }

  for (i = 0; i < MAXN; ++i) {
    struct Edge *e = head[i];
    while (e != NULL) {
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

void __lifetime_start(void *kptr, int size) {
  // Use Kernel RBTree to store the information
  printk(KERN_INFO "Start: %px, size: %d\n", kptr, size);
}

void __lifetime_end(void *kptr) {
  // Use Kernel RBTree to clear the information
  printk(KERN_INFO "End: %px\n", kptr);
}

void __lifetime_escape(void *memloc, void *kptr) {
  // Use Kernel RBTree to store the information
  printk(KERN_INFO "Escape: %px, memloc: %px\n", kptr, memloc);
}

module_init(bench_init);
module_exit(bench_exit);