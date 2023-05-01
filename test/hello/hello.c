#include <linux/module.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Zheng Yu");
MODULE_DESCRIPTION("");
MODULE_VERSION("1.0");

static int __init hello_init(void)
{
	printk(KERN_INFO "Hello World !!");
	return 0;
}

static void __exit hello_exit(void)
{
	printk(KERN_INFO "Goodbye !!");
}

module_init(hello_init);
module_exit(hello_exit);