#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/io.h>

#define GPIO_BASE       0x3F200000
#define GPIO_SIZE       0xB4

#define GPIO_PIN        26
#define GPIO_FSEL2      0x08
#define GPIO_SET0       0x1C
#define GPIO_CLR0       0x28

static volatile uint32_t __iomem *gpio;

static int __init led_init(void)
{
    gpio = ioremap(GPIO_BASE, GPIO_SIZE);
    if (!gpio) {
        pr_err("Failed to ioremap GPIO\n");
        return -ENOMEM;
    }

    // GPFSEL2: set GPIO26 as output (bits 18–20)
    printk(KERN_INFO "gpio: %u\n", (uint32_t)gpio);
    
    gpio[GPIO_FSEL2 / 4] &= ~(0x7 << 18);  // clear bits 
    gpio[GPIO_FSEL2 / 4] |=  (0x1 << 18);  // set output (001)

    /*
    
    *(gpio + GPIO_FSEL2 / 4) &= ~(0x7 << 18);
    *(gpio + GPIO_FSEL2 / 4) |=  (0x1 << 18);
    *(gpio + GPIO_SET0 / 4) = 1 << GPIO_PIN;

    */

    // GPSET0: set GPIO26 high (turn LED on)
    gpio[GPIO_SET0 / 4] = 1 << GPIO_PIN;

    pr_info("GPIO26 set to HIGH (LED ON)\n");
    return 0;
}

static void __exit led_exit(void)
{
    // GPCLR0: clear GPIO26 (turn LED off)
    gpio[GPIO_CLR0 / 4] = 1 << GPIO_PIN;

    if (gpio)
        iounmap(gpio);

    pr_info("GPIO26 set to LOW (LED OFF)\n");
}

module_init(led_init);
module_exit(led_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Bạn");
MODULE_DESCRIPTION("Điều khiển LED GPIO26 không dùng iowrite32");
MODULE_VERSION("1.0");
