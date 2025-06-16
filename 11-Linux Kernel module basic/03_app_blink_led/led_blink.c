#include <linux/io.h>
#include <linux/module.h>   /* Define module_init(), module_exit() */
#include <linux/fs.h>       /* Define alloc_chrdev_region(), register_chrdev_region() */
#include <linux/device.h>   /* Define device_create(), class_create() */
#include <linux/cdev.h>     /* Define cdev_init(), cdev_add() */
#include <linux/uaccess.h>  /* Define copy_to_user(), copy_from_user() */
#include <linux/slab.h>     /* Define kfree() */
#define DRIVER_AUTHOR   "tranductin trantin156697@gmail.com"
#define DRIVER_DESC     "Character device control GPIO26 (LED)"
#define DRIVER_VERS     "1.0"

#define DEVICE_NAME     "m_device"
#define CLASS_NAME      "m_class"
#define NPAGES          1

// GPIO config 
#define GPIO_BASE   