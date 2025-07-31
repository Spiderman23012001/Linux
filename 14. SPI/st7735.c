#include <linux/module.h>
#include <linux/spi/spi.h>
#include <linux/gpio/consumer.h>
#include <linux/delay.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/slab.h>

#define DEVICE_NAME "st7735"
#define SCREEN_WIDTH 80
#define SCREEN_HEIGHT 160
#define SCREEN_BUF_SIZE (SCREEN_WIDTH * SCREEN_HEIGHT * 2) // RGB565 (2 bytes/pixel)

struct st7735_data {
    struct spi_device *spi;
    struct gpio_desc *dc_gpio;
    struct gpio_desc *reset_gpio;

    dev_t dev_num;
    struct class *class;
    struct device *device;
    struct cdev cdev;

    uint8_t *buffer;
    size_t buffer_size;
};

// Ghi 1 byte command
static int st7735_write_cmd(struct st7735_data *lcd, uint8_t cmd)
{
    int ret;
    gpiod_set_value(lcd->dc_gpio, 0); // DC = 0 cho lệnh
    ret = spi_write(lcd->spi, &cmd, 1);
    if (ret < 0)
        dev_err(&lcd->spi->dev, "Write command failed: %d\n", ret);
    return ret;
}

// Ghi data (có thể nhiều byte)
static int st7735_write_data(struct st7735_data *lcd, const uint8_t *data, size_t len)
{
    int ret;
    gpiod_set_value(lcd->dc_gpio, 1); // DC = 1 cho data
    ret = spi_write(lcd->spi, data, len);
    if (ret < 0)
        dev_err(&lcd->spi->dev, "Write data failed: %d\n", ret);
    return ret;
}

// Reset màn hình
static void st7735_reset(struct st7735_data *lcd)
{
    if (lcd->reset_gpio) {
        gpiod_set_value(lcd->reset_gpio, 0);
        msleep(20);
        gpiod_set_value(lcd->reset_gpio, 1);
        msleep(120);
    }
}

// Gửi lệnh thiết lập con trỏ vùng vẽ (column + page)
static int st7735_set_address_window(struct st7735_data *lcd, uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
    uint8_t buf[4];

    // Column address set
    st7735_write_cmd(lcd, 0x2A);
    buf[0] = (x0 >> 8) & 0xFF;
    buf[1] = x0 & 0xFF;
    buf[2] = (x1 >> 8) & 0xFF;
    buf[3] = x1 & 0xFF;
    st7735_write_data(lcd, buf, 4);

    // Page address set
    st7735_write_cmd(lcd, 0x2B);
    buf[0] = (y0 >> 8) & 0xFF;
    buf[1] = y0 & 0xFF;
    buf[2] = (y1 >> 8) & 0xFF;
    buf[3] = y1 & 0xFF;
    st7735_write_data(lcd, buf, 4);

    // Memory write
    st7735_write_cmd(lcd, 0x2C);

    return 0;
}

// Chuỗi lệnh khởi tạo màn hình ST7735 80x160 (basic)
static int st7735_init_sequence(struct st7735_data *lcd)
{
    st7735_reset(lcd);

    msleep(120);

    st7735_write_cmd(lcd, 0x01); // Software reset
    msleep(150);

    st7735_write_cmd(lcd, 0x11); // Sleep out
    msleep(150);

    // Frame rate control
    st7735_write_cmd(lcd, 0xB1);
    uint8_t fr_buf1[] = {0x01, 0x2C, 0x2D};
    st7735_write_data(lcd, fr_buf1, sizeof(fr_buf1));

    st7735_write_cmd(lcd, 0xB2);
    uint8_t fr_buf2[] = {0x01, 0x2C, 0x2D};
    st7735_write_data(lcd, fr_buf2, sizeof(fr_buf2));

    st7735_write_cmd(lcd, 0xB3);
    uint8_t fr_buf3[] = {0x01, 0x2C, 0x2D, 0x01, 0x2C, 0x2D};
    st7735_write_data(lcd, fr_buf3, sizeof(fr_buf3));

    // Power sequence
    st7735_write_cmd(lcd, 0xB4);
    st7735_write_data(lcd, (uint8_t[]){0x07}, 1);

    // VCOM control
    st7735_write_cmd(lcd, 0xC0);
    st7735_write_data(lcd, (uint8_t[]){0xA2, 0x02, 0x84}, 3);

    st7735_write_cmd(lcd, 0xC1);
    st7735_write_data(lcd, (uint8_t[]){0xC5}, 1);

    st7735_write_cmd(lcd, 0xC2);
    st7735_write_data(lcd, (uint8_t[]){0x0A, 0x00}, 2);

    st7735_write_cmd(lcd, 0xC3);
    st7735_write_data(lcd, (uint8_t[]){0x8A, 0x2A}, 2);

    st7735_write_cmd(lcd, 0xC4);
    st7735_write_data(lcd, (uint8_t[]){0x8A, 0xEE}, 2);

    st7735_write_cmd(lcd, 0xC5);
    st7735_write_data(lcd, (uint8_t[]){0x0E}, 1);

    // Memory access control (rotation, RGB order)
    st7735_write_cmd(lcd, 0x36);
    st7735_write_data(lcd, (uint8_t[]){0xC8}, 1);

    // Pixel format (16-bit)
    st7735_write_cmd(lcd, 0x3A);
    st7735_write_data(lcd, (uint8_t[]){0x05}, 1); // RGB565

    // Display on
    st7735_write_cmd(lcd, 0x29);
    msleep(100);

    return 0;
}

// Viết buffer pixel RGB565 lên màn hình
static int st7735_update_display(struct st7735_data *lcd)
{
    st7735_set_address_window(lcd, 0, 0, SCREEN_WIDTH - 1, SCREEN_HEIGHT - 1);

    return st7735_write_data(lcd, lcd->buffer, lcd->buffer_size);
}

/* --- Các hàm device file operations --- */

static int st7735_open(struct inode *inode, struct file *file)
{
    struct st7735_data *lcd = container_of(inode->i_cdev, struct st7735_data, cdev);
    file->private_data = lcd;
    dev_info(&lcd->spi->dev, "Device opened\n");
    return 0;
}

static int st7735_release(struct inode *inode, struct file *file)
{
    dev_info(NULL, "Device closed\n");
    return 0;
}

// User-space ghi buffer dữ liệu RGB565 (raw) vào driver để hiển thị
static ssize_t st7735_write(struct file *file, const char __user *buf, size_t count, loff_t *ppos)
{
    struct st7735_data *lcd = file->private_data;
    ssize_t ret;

    if (count > lcd->buffer_size) {
        dev_err(&lcd->spi->dev, "Write size too large: %zu\n", count);
        return -EINVAL;
    }

    // Copy dữ liệu từ user-space vào buffer kernel
    if (copy_from_user(lcd->buffer, buf, count))
        return -EFAULT;

    // Cập nhật buffer size đúng bằng count (nếu ngắn hơn kích thước full)
    lcd->buffer_size = count;

    // Gửi dữ liệu lên màn hình
    ret = st7735_update_display(lcd);
    if (ret < 0) {
        dev_err(&lcd->spi->dev, "Update display failed: %zd\n", ret);
        return ret;
    }

    return count;
}

static const struct file_operations st7735_fops = {
    .owner = THIS_MODULE,
    .open = st7735_open,
    .release = st7735_release,
    .write = st7735_write,
};

static int st7735_probe(struct spi_device *spi)
{
    struct st7735_data *lcd;
    int ret;

    lcd = devm_kzalloc(&spi->dev, sizeof(*lcd), GFP_KERNEL);
    if (!lcd)
        return -ENOMEM;

    lcd->spi = spi;

    lcd->dc_gpio = devm_gpiod_get(&spi->dev, "dc", GPIOD_OUT_LOW);
    if (IS_ERR(lcd->dc_gpio)) {
        dev_err(&spi->dev, "Cannot get dc GPIO\n");
        return PTR_ERR(lcd->dc_gpio);
    }

    lcd->reset_gpio = devm_gpiod_get(&spi->dev, "reset", GPIOD_OUT_HIGH);
    if (IS_ERR(lcd->reset_gpio)) {
        dev_warn(&spi->dev, "No reset GPIO configured\n");
        lcd->reset_gpio = NULL;
    }

    // Cấp phát buffer pixel lớn
    lcd->buffer_size = SCREEN_BUF_SIZE;
    lcd->buffer = devm_kmalloc(&spi->dev, lcd->buffer_size, GFP_KERNEL);
    if (!lcd->buffer)
        return -ENOMEM;
    memset(lcd->buffer, 0x00, lcd->buffer_size);

    spi_set_drvdata(spi, lcd);

    ret = st7735_init_sequence(lcd);
    if (ret) {
        dev_err(&spi->dev, "Failed to init screen\n");
        return ret;
    }

    // Đăng ký device char
    ret = alloc_chrdev_region(&lcd->dev_num, 0, 1, DEVICE_NAME);
    if (ret) {
        dev_err(&spi->dev, "Failed to alloc chrdev region\n");
        return ret;
    }

    cdev_init(&lcd->cdev, &st7735_fops);
    lcd->cdev.owner = THIS_MODULE;
    ret = cdev_add(&lcd->cdev, lcd->dev_num, 1);
    if (ret) {
        unregister_chrdev_region(lcd->dev_num, 1);
        dev_err(&spi->dev, "Failed to add cdev\n");
        return ret;
    }

    // Tạo class và device file /dev/st7735
    lcd->class = class_create(THIS_MODULE, DEVICE_NAME);
    if (IS_ERR(lcd->class)) {
        cdev_del(&lcd->cdev);
        unregister_chrdev_region(lcd->dev_num, 1);
        return PTR_ERR(lcd->class);
    }

    lcd->device = device_create(lcd->class, NULL, lcd->dev_num, NULL, DEVICE_NAME);
    if (IS_ERR(lcd->device)) {
        class_destroy(lcd->class);
        cdev_del(&lcd->cdev);
        unregister_chrdev_region(lcd->dev_num, 1);
        return PTR_ERR(lcd->device);
    }

    dev_info(&spi->dev, "ST7735 LCD driver probed\n");

    return 0;
}

static int st7735_remove(struct spi_device *spi)
{
    struct st7735_data *lcd = spi_get_drvdata(spi);

    device_destroy(lcd->class, lcd->dev_num);
    class_destroy(lcd->class);
    cdev_del(&lcd->cdev);
    unregister_chrdev_region(lcd->dev_num, 1);

    dev_info(&spi->dev, "ST7735 LCD driver removed\n");

    return 0;
}

static const struct of_device_id st7735_of_match[] = {
    { .compatible = "sitronix,st7735", },
    { /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, st7735_of_match);

static struct spi_driver st7735_driver = {
    .driver = {
        .name = DEVICE_NAME,
        .of_match_table = st7735_of_match,
    },
    .probe = st7735_probe,
    .remove = st7735_remove,
};

module_spi_driver(st7735_driver);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Tran Tin");
MODULE_DESCRIPTION("SPI driver for ST7735 80x160 RGB IPS LCD");
