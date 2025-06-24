#include <linux/module.h>
#include <linux/init.h>
#include <linux/gpio.h>    // Legacy GPIO (integer-based)

#define GPIO_NUMBER_26  26
#define LOW             0
#define HIGH            1

static int __init mgpio_driver_init(void)
{
    int ret;

    pr_info("DevLinux: Initializing GPIO driver\n");

    // Kiểm tra GPIO có hợp lệ không
    if (!gpio_is_valid(GPIO_NUMBER_26)) {
        pr_err("DevLinux: GPIO %d is not valid\n", GPIO_NUMBER_26);
        return -ENODEV;
    }

    // Yêu cầu kernel cấp quyền sử dụng GPIO
    ret = gpio_request(GPIO_NUMBER_26, "gpio_26");
    if (ret) {
        pr_err("DevLinux: Failed to request GPIO %d\n", GPIO_NUMBER_26);
        return ret;
    }

    // Cấu hình GPIO26 là output và đặt giá trị LOW
    gpio_direction_output(GPIO_NUMBER_26, LOW);

    // Bật GPIO26 (HIGH)
    gpio_set_value(GPIO_NUMBER_26, HIGH);
    pr_info("DevLinux: GPIO %d set to HIGH, current state: %d\n",
            GPIO_NUMBER_26, gpio_get_value(GPIO_NUMBER_26));

    return 0;
}

static void __exit mgpio_driver_exit(void)
{
    // Tắt GPIO trước khi thoát
    gpio_set_value(GPIO_NUMBER_26, LOW);
    pr_info("DevLinux: GPIO %d set to LOW, current state: %d\n",
            GPIO_NUMBER_26, gpio_get_value(GPIO_NUMBER_26));

    // Trả lại GPIO cho hệ thống
    gpio_free(GPIO_NUMBER_26);

    pr_info("DevLinux: GPIO driver exited\n");
}

module_init(mgpio_driver_init);
module_exit(mgpio_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("DevLinux");
MODULE_DESCRIPTION("Integer GPIO Driver for Raspberry Pi GPIO26 (BCM2835)");
MODULE_VERSION("1.0");
