#include <linux/module.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <linux/io.h>

#define DRIVER_AUTHOR "phonglt9 linuxfromscratch@gmail.com"
#define DRIVER_DESC   "Character device điều khiển GPIO26"
#define DRIVER_VERS   "1.0"

#define DEVICE_NAME   "m_cdev"
#define CLASS_NAME    "m_class"
#define NPAGES        1

// GPIO config
#define GPIO_BASE     0x3F200000
#define GPIO_SIZE     0xB4
#define GPIO_PIN      26
#define GPIO_FSEL2    0x08
#define GPIO_SET0     0x1C
#define GPIO_CLR0     0x28

static volatile uint32_t __iomem *gpio = NULL;

struct m_foo_dev {
    int32_t size;
    char *kmalloc_ptr;
    dev_t dev_num;
    struct class *m_class;
    struct cdev m_cdev;
} mdev;

/* Prototype */
static int __init chdev_init(void);
static void __exit chdev_exit(void);
static int m_open(struct inode *inode, struct file *file);
static int m_release(struct inode *inode, struct file *file);
static ssize_t m_read(struct file *filp, char __user *user_buf, size_t size, loff_t *offset);
static ssize_t m_write(struct file *filp, const char __user *user_buf, size_t size, loff_t *offset);

static struct file_operations fops = {
    .owner   = THIS_MODULE,
    .read    = m_read,
    .write   = m_write,
    .open    = m_open,
    .release = m_release,
};

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

    if (to_read == 0)
        return 0;

    if (copy_to_user(user_buf, mdev.kmalloc_ptr + *offset, to_read))
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

    // Điều khiển GPIO từ nội dung buffer
    if (strncmp(buffer, "on", 2) == 0) {
        gpio[GPIO_SET0 / 4] = (1 << GPIO_PIN);
        pr_info("LED ON\n");
    } else if (strncmp(buffer, "off", 3) == 0) {
        gpio[GPIO_CLR0 / 4] = (1 << GPIO_PIN);
        pr_info("LED OFF\n");
    }

    return to_write;
}

static int __init chdev_init(void)
{
    int ret;

    // Cấp phát device number
    if ((ret = alloc_chrdev_region(&mdev.dev_num, 0, 1, DEVICE_NAME)) < 0) {
        pr_err("Failed to allocate chrdev\n");
        return ret;
    }

    // Tạo class
    mdev.m_class = class_create(CLASS_NAME);
    if (IS_ERR(mdev.m_class)) {
        pr_err("Failed to create class\n");
        ret = PTR_ERR(mdev.m_class);
        goto err_unregister_chrdev;
    }

    // Tạo device
    if (IS_ERR(device_create(mdev.m_class, NULL, mdev.dev_num, NULL, DEVICE_NAME))) {
        pr_err("Failed to create device\n");
        ret = -ENOMEM;
        goto err_class_destroy;
    }

    // Khởi tạo và thêm cdev
    cdev_init(&mdev.m_cdev, &fops);
    if ((ret = cdev_add(&mdev.m_cdev, mdev.dev_num, 1)) < 0) {
        pr_err("Failed to add cdev\n");
        goto err_device_destroy;
    }

    // Cấp phát bộ nhớ cho buffer
    mdev.kmalloc_ptr = kmalloc(NPAGES * PAGE_SIZE, GFP_KERNEL);
    if (!mdev.kmalloc_ptr) {
        pr_err("Failed to allocate memory\n");
        ret = -ENOMEM;
        goto err_cdev_del;
    }

    // Ánh xạ GPIO
    gpio = ioremap(GPIO_BASE, GPIO_SIZE);
    if (!gpio) {
        pr_err("Failed to ioremap GPIO\n");
        ret = -ENOMEM;
        goto err_kfree;
    }

    // Cấu hình GPIO26 là output
    gpio[GPIO_FSEL2 / 4] &= ~(0x7 << 18);
    gpio[GPIO_FSEL2 / 4] |=  (0x1 << 18);

    pr_info("Module loaded. Major: %d, Minor: %d\n", MAJOR(mdev.dev_num), MINOR(mdev.dev_num));
    return 0;

err_kfree:
    kfree(mdev.kmalloc_ptr);
err_cdev_del:
    cdev_del(&mdev.m_cdev);
err_device_destroy:
    device_destroy(mdev.m_class, mdev.dev_num);
err_class_destroy:
    class_destroy(mdev.m_class);
err_unregister_chrdev:
    unregister_chrdev_region(mdev.dev_num, 1);
    return ret;
}

static void __exit chdev_exit(void)
{
    gpio[GPIO_CLR0 / 4] = (1 << GPIO_PIN);  // Tắt LED
    if (gpio)
        iounmap(gpio);

    kfree(mdev.kmalloc_ptr);
    cdev_del(&mdev.m_cdev);
    device_destroy(mdev.m_class, mdev.dev_num);
    class_destroy(mdev.m_class);
    unregister_chrdev_region(mdev.dev_num, 1);

    pr_info("Module unloaded\n");
}

module_init(chdev_init);
module_exit(chdev_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_DESCRIPTION(DRIVER_DESC);
MODULE_VERSION(DRIVER_VERS);
