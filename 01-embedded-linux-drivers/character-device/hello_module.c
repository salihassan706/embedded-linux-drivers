#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/version.h> // Pulls in version macros
#include <generated/utsrelease.h>
static int __init hello_start(void) {
    
    printk(KERN_INFO "Hello: Module loaded! Kernel Version: %s\n", UTS_RELEASE);
    return 0;
}

static void __exit hello_end(void) {
    printk(KERN_INFO "Hello: Module removed. Goodbye!\n");
}

module_init(hello_start);
module_exit(hello_end);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Sohail");
MODULE_DESCRIPTION("My first kernel module");


