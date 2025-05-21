#include <linux/module.h>
#include <linux/init.h>  // For __init and __exit macros

#define DRIVER_AUTHOR   "trantin156697@gmail.com"
#define DRIVER_DESC     "A simple Hello World Linux Kernel Module"
#define DRIVER_VERSION  "1.0"
#define DRIVER_LICENSE  "GPL"

/**
 * hello_init - Module initialization function
 *
 * Returns 0 on success.
 */
static int __init hello_init(void)
{
    printk(KERN_INFO "Hello, world! Module loaded.\n");
    return 0;
}

/**
 * hello_exit - Module cleanup function
 */
static void __exit hello_exit(void)
{
    printk(KERN_INFO "Goodbye, world! Module unloaded.\n");
}

module_init(hello_init);
module_exit(hello_exit);

MODULE_LICENSE(DRIVER_LICENSE);
MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_DESCRIPTION(DRIVER_DESC);
MODULE_VERSION(DRIVER_VERSION);
