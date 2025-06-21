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


struct m_foo_dev {
    int32_t size;
    char *kmalloc_ptr;
    dev_t dev_num;
    struct class *m_class;
    struct cdev m_dev;
} mdev;

/* Prototype */
static int __init chdev_init(void);
static void __exit chdev_exit(void);
static int m_open(struct inode *inode, struct file *file);
static int m_release(struct inode *inode, struct file *file);
static ssize_t m_read(struct file *filp, char __user *user_buf, size_t size, loff_t *offset);
static ssize_t m_write(struct file *filp, const char __user *user_buf, size_t size, loff_t *offset);

static int m_open(struct inode *inode, struct file *file)
{
    pr_info("Device opened\n");
    return 0;
}

static int m_release(struct inode *inode, struct file *file)
{
    pr_info("Device closed\n");
    return 0;
}

static ssize_t m_read(struct file *filp, char __user *user_buf, size_t size, loff_t *offset)
{
    size_t to_read = min((size_t)(mdev.size - *offset), size);

    if(to_read == 0)
        return 0

    if(copy_to_user(user_buf, mdev.kmalloc_ptr + *offset, to_read))
        return -EFAULT;

    *offset += to_read;
    return to_read;
}

static ssize_t m_write(struct file *filp, const char __user *user_buf, size_t size, loff_t *offset)
{
        size_t to_write = min(size, (size_t)(NPAGES * PAGE_SIZE - *offset));
    char buffer[16] = {0};

    if (copy_from_user(mdev.kmalloc_ptr + *offset, user_buf, to_write))
        return -EFAULT;

    mdev.kmalloc_ptr[to_write] = '\0';
    strncpy(buffer, mdev.kmalloc_ptr + *offset, sizeof(buffer) - 1);
    *offset += to_write;
    mdev.size = *offset;

    return to_write;
}