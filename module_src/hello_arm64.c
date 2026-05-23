#include <linux/module.h>
#include <linux/kernel.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Sohail Ali Hassan Abbasi");
MODULE_DESCRIPTION("ARM64 QEMU Test Module");

static int __init hello_init(void)
{
    printk(KERN_INFO "ARM64_TEST: Module loaded in QEMU\n");
    printk(KERN_INFO "ARM64_TEST: Sohail's embedded Linux framework running\n");
    return 0;
}

static void __exit hello_exit(void)
{
    printk(KERN_INFO "ARM64_TEST: Module unloaded\n");
}

module_init(hello_init);
module_exit(hello_exit);
