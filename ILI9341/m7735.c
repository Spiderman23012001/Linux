#include <linux/module.h>
#include <linux/spi/spi.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/gpio/consumer.h>
#include <linux/of.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/cdev.h>
#include <linux/delay.h>

#include "st7735-driver.h"
#include "font_5_7.h"
#include "image01.h"

char message[ST7735_MAX_BUFF]; /* Message buffer for display */
st7735_spi_module_t* st7735Module = NULL; /* ST7735 module instance */

/* ST7735 of match table */
static const struct of_device_id st7735_driver_ids[] = {
    { .compatible = ST7735_DEV_COMPATIBLE, },
    { /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, st7735_driver_ids);

static const struct spi_device_id st7735_spi_id[] = {
    { ST7735_DEV_NAME, 0 },
    { }
};
MODULE_DEVICE_TABLE(spi, st7735_spi_id);

/* File operations structure */
static struct file_operations st7735_fops = {
    .owner = THIS_MODULE,
    .open = st7735Open,
    .release = st7735Release,
    .write = st7735WriteOps,
    .read = st7735ReadOps,
};

/* st7735 open */
static int st7735Open(struct inode *inode, struct file *file)
{
    pr_info("ST7735 OLED display opened\n");
    return 0;
}

/* st7735 release */
static int st7735Release(struct inode *inode, struct file *file)
{
    pr_info("ST7735 OLED display closed\n");
    return 0;
}

/* st7735 write */
static ssize_t st7735WriteOps(struct file *file, const char __user *buf, size_t len, loff_t *offset)
{
    size_t to_write;
    // int ret;

    if (!st7735Module) {
        return -ENODEV;
    }

    /* Check if the buffer length exceeds the maximum allowed */
    if (len > ST7735_MAX_BUFF) {
        return -EINVAL;
    }

    /* Copy data from user space to kernel space */
    if (copy_from_user(message, buf, len)) {
        return -EFAULT;
    }

    /* Write the message to the st7735 display */
    to_write = len; // For simplicity, we write all data received
    st7735DisplayString(st7735Module, message);
    

    pr_info("ST7735 OLED display written %zu bytes\n", to_write);

    return to_write;
}

/* st7735 read */
static ssize_t st7735ReadOps(struct file *file, char __user *buf, size_t len, loff_t *offset)
{
    size_t to_read;

    if (!st7735Module) {
        return -ENODEV;
    }

    /* Check if the buffer length exceeds the maximum allowed */
    if (len > ST7735_MAX_BUFF) {
        return -EINVAL;
    }

    /* Read the message from the st7735 display */
    to_read = len; // For simplicity, we read all data requested
    if (to_read > ST7735_MAX_BUFF) {
        to_read = ST7735_MAX_BUFF;
    }

    if (copy_to_user(buf, message, to_read)) {
        return -EFAULT;
    }

    pr_info("ST7735 OLED display read %zu bytes\n", to_read);

    return to_read;
}


/* st7735 display initialize */
static void st7735InitDisplay(st7735_spi_module_t *module)
{
    msleep(100); // Wait for the display to power up

    /* Reset the display */
    gpiod_set_value(module->resetPin, 0);
    udelay(5000);
    gpiod_set_value(module->resetPin, 1);
    udelay(5000);

    st7735Write(module, true, 0x01); //Software Reset
    udelay(5000);
    st7735Write(module, true, 0x11); //exit  Sleep
    udelay(5000);

    st7735Write(module, true, 0xB1);  //Frame Rate Control (In normal mode/ Full colors)
    st7735Write(module, false, 0x01);
    st7735Write(module, false, 0x2C);
    st7735Write(module, false, 0x2D);

    st7735Write(module, true, 0xB2);  //Frame Rate Control (In Idle mode/ 8-colors)
    st7735Write(module, false, 0x01);
    st7735Write(module, false, 0x2C);
    st7735Write(module, false, 0x2D);

    st7735Write(module, true, 0xB3);  //Frame Rate Control (In Partial mode/ full colors)
    st7735Write(module, false, 0x01);
    st7735Write(module, false, 0x2C);
    st7735Write(module, false, 0x2D);
    st7735Write(module, false, 0x01);
    st7735Write(module, false, 0x2C);
    st7735Write(module, false, 0x2D);

    st7735Write(module, true, 0xB4);  //Display Inversion Control
    st7735Write(module, false, 0x07);

    st7735Write(module, true, 0xC0);  //Power Control 1
    st7735Write(module, false, 0xA2);
    st7735Write(module, false, 0x02);
    st7735Write(module, false, 0x84);

    st7735Write(module, true, 0xC1); //Power Control 2
    st7735Write(module, false, 0xC5);

    st7735Write(module, true, 0xC2);  //Power Control 3 (in Normal mode/ Full colors)
    st7735Write(module, false, 0x0A);
    st7735Write(module, false, 0x00);

    st7735Write(module, true, 0xC3);  //Power Control 4 (in Idle mode/ 8-colors)
    st7735Write(module, false, 0x8A);
    st7735Write(module, false, 0x2A);

    st7735Write(module, true, 0xC4);  //Power Control 5 (in Partial mode/ full-colors)
    st7735Write(module, false, 0x8A);
    st7735Write(module, false, 0xEE);

    st7735Write(module, true, 0xC5);  //VCOM Control 1
    st7735Write(module, false, 0x0E);

    st7735Write(module, true, 0x20);  //Display Inversion Off

    st7735Write(module, true, 0x36);  //Memory Data Access Control           
    st7735Write(module, false, 0xA0); //RGB mode + che do man hinh ngang

    st7735Write(module, true, 0x3A); //Interface Pixel Format
    st7735Write(module, false, 0x05); //16-bit/pixel

    st7735Write(module, true, 0x2A); //Column address set
    st7735Write(module, false, 0x00);
    st7735Write(module, false, 0x00);
    st7735Write(module, false, 0x00);
    st7735Write(module, false, 160);

    st7735Write(module, true, 0x2B); //Row address set
    st7735Write(module, false, 0x00);
    st7735Write(module, false, 0x00);
    st7735Write(module, false, 0x00);
    st7735Write(module, false, 128);

    st7735Write(module, true, 0xE0); //Gamm adjustment (+ polarity)
    st7735Write(module, false, 0x02);
    st7735Write(module, false, 0x1C);
    st7735Write(module, false, 0x07);
    st7735Write(module, false, 0x12);
    st7735Write(module, false, 0x37);
    st7735Write(module, false, 0x32);
    st7735Write(module, false, 0x29);
    st7735Write(module, false, 0x2D);
    st7735Write(module, false, 0x29);
    st7735Write(module, false, 0x25);
    st7735Write(module, false, 0x2B);
    st7735Write(module, false, 0x39);
    st7735Write(module, false, 0x00);
    st7735Write(module, false, 0x01);
    st7735Write(module, false, 0x03);
    st7735Write(module, false, 0x10);

    st7735Write(module, true, 0xE1);  //Gamma adjustment(- polarity)
    st7735Write(module, false, 0x03);
    st7735Write(module, false, 0x1D);
    st7735Write(module, false, 0x07);
    st7735Write(module, false, 0x06);
    st7735Write(module, false, 0x2E);
    st7735Write(module, false, 0x2C);
    st7735Write(module, false, 0x29);
    st7735Write(module, false, 0x2D);
    st7735Write(module, false, 0x2E);
    st7735Write(module, false, 0x2E);
    st7735Write(module, false, 0x37);
    st7735Write(module, false, 0x3F);
    st7735Write(module, false, 0x00);
    st7735Write(module, false, 0x00);
    st7735Write(module, false, 0x02);
    st7735Write(module, false, 0x10);

    st7735Write(module, true, 0x13); //Partial off (Normal)

    st7735Write(module, true, 0x29); //Display on

    st7735ClearDisplay(module); // Clear the display after initialization

    st7735DisplayImage(module, image01Tbl, ST7735_MAX_SEG, ST7735_MAX_ROW); // Display the image01 on the screen

}

/* st7735 clear display */
static void st7735ClearDisplay(st7735_spi_module_t *module)
{
    int i;

    st7735SetWindow(module, 0, 0, ST7735_MAX_SEG, ST7735_MAX_ROW);
    for (i = 0; i < ST7735_MAX_ROW * ST7735_MAX_SEG; i++) {
        st7735Write(module, false, 0x00); // Clear display by writing zeros
        st7735Write(module, false, 0x00); // Clear display by writing zeros
    }
}

/* st7735 display image */
static void st7735DisplayImage(st7735_spi_module_t *module, const uint8_t *image, uint8_t width, uint8_t height)
{
    uint16_t i;

    st7735SetWindow(module, 0, 0, width, height);
    for (i = 0; i < width * height * 2; i++) {
        st7735Write(module, false, image[i]);
    }
}

/* st7735 set window */
static void st7735SetWindow(st7735_spi_module_t *module, uint8_t x, uint8_t y, uint8_t width, uint8_t height)
{
    st7735Write(module, true, 0x2A); // Column address set
    st7735Write(module, false, 0);
    st7735Write(module, false, x);
    st7735Write(module, false, 0);
    if (x + width > ST7735_MAX_SEG) {
        st7735Write(module, false, ST7735_MAX_SEG); // Set end column address to max segment
    } else {
        st7735Write(module, false, x + width); // Set end column address
    }

    st7735Write(module, true, 0x2B); // Row address set
    st7735Write(module, false, 0);
    st7735Write(module, false, y);
    st7735Write(module, false, 0);
    if (y + height > ST7735_MAX_ROW) {
        st7735Write(module, false, ST7735_MAX_ROW); // Set end row address to max row
    } else {
        st7735Write(module, false, y + height); // Set end row address
    }

    st7735Write(module, true, 0x2C); // Memory write
}

/* st7735 set cursor */
static void st7735SetCursor(st7735_spi_module_t *module, uint8_t line, uint8_t cursor)
{
    if (line > ST7735_MAX_LINE || cursor >= ST7735_MAX_SEG) {
        pr_err("DevLinux: [%s %d] Invalid line or cursor position\n", __func__, __LINE__);
        return;
    }

    module->currentLine = line;
    module->currentCursor = cursor;

    /* Set the column address */
    st7735Write(module, true, 0x21); // column start and end address command
    st7735Write(module, true, cursor); // Set start column address
    st7735Write(module, true, ST7735_MAX_SEG - 1); // Set end column address

    /* Set the page address */
    st7735Write(module, true, 0x22); // page start and end address command
    st7735Write(module, true, line); // Set start page
    st7735Write(module, true, ST7735_MAX_LINE); // Set end
}

/* st7735 display string */
static void st7735DisplayString(st7735_spi_module_t *module, const char *str)
{
    st7735ClearDisplay(module);
    st7735SetCursor(module, 0, 0);

    while (*str) {
        st7735DisplayChar(module, *str);
        str++;
    }
}

/* st7735 display character */
static void st7735DisplayChar(st7735_spi_module_t *module, uint8_t data)
{
    const uint8_t *font;
    uint8_t i;

    if ((module->currentCursor + module->fontSize >= ST7735_MAX_SEG) || (data == '\n')) {
        module->currentCursor = 0; // Reset cursor to start of line
        module->currentLine++;
        if (module->currentLine >= ST7735_MAX_LINE) {
            module->currentLine = 0; // Reset to first line
        }
        st7735SetCursor(module, module->currentLine, module->currentCursor);
    }

    if (data ^ '\n') {
        font = &fontTable5x7[data * 5]; // Get the font data for the character
        for (i = 0; i < module->fontSize; i++) {
            st7735Write(module, false, font[i]); // Write each byte of the font data
            module->currentCursor++;
        }
        st7735Write(module, false, 0);
        module->currentCursor++;
    }
}

/* ST7735 write */
static int st7735Write(st7735_spi_module_t *module, bool isCmd, uint8_t data)
{
    struct spi_transfer t;
    struct spi_message m;
    int ret;

    /* Set DC pin according to data / command mode */
    gpiod_set_value(module->dcPin, isCmd ? 0 : 1);

    /* Initialize SPI message */
    memset(&t, 0, sizeof(t));
    spi_message_init(&m);

    /* Setup transfer */
    t.tx_buf = &data;
    t.len = 1;
    t.speed_hz = module->client->max_speed_hz;
    spi_message_add_tail(&t, &m);
    
    /* Perform transfer */
    ret = spi_sync(module->client, &m);
    if (ret < 0) {
        pr_err("DevLinux: [%s %d] SPI transfer failed: %d\n", __func__, __LINE__, ret);
    }

    /* Add small delay for stability */
    udelay(10);

    return ret;
}

/* ST7735 create device file */
static int st7735CreateDeviceFile(st7735_spi_module_t *module)
{

    if (alloc_chrdev_region(&module->dev_num, 0, 1, ST7735_DEV_NAME) < 0) {
        pr_err("DevLinux: [%s %d] alloc_chrdev_region failed\n", __func__, __LINE__);
        return -1;
    }
    
    pr_info("DevLinux: [%s %d]Allocated character device region: \nMajor = %d Minor = %d\n", __func__, __LINE__,
            MAJOR(module->dev_num), MINOR(module->dev_num));

    module->class = class_create(ST7735_DEV_CLASS);
    if (IS_ERR(module->class)) {
        pr_err("DevLinux: [%s %d] class_create failed\n", __func__, __LINE__);
        goto rm_device_number;
    }

    if (device_create(module->class, NULL, module->dev_num, NULL, ST7735_DEV_NAME) == NULL) {
        pr_err("DevLinux: [%s %d] device_create failed\n", __func__, __LINE__);
        goto rm_class; 
    }

    cdev_init(&module->cdev, &st7735_fops);

    if (cdev_add(&module->cdev, module->dev_num, 1) < 0) {
        pr_err("DevLinux: [%s %d] cdev_add failed\n", __func__, __LINE__);
        goto rm_device;
    }

    module->kmalloc_ptr = kmalloc(ST7735_MAX_BUFF, GFP_KERNEL);
    
    pr_info("DevLinux: [%s %d] Character device added successfully\n", __func__, __LINE__);

    return 0;

rm_device:
    device_destroy(module->class, module->dev_num);

rm_class:
    class_destroy(module->class);

rm_device_number:
    unregister_chrdev_region(module->dev_num, 1);

    return -1;    
}

/* ST7735 probe */
static int st7735Probe(struct spi_device *client)
{
    st7735_spi_module_t *module;

    pr_info("DevLinux: [%s %d] Probing ST7735 SPI device\n", __func__, __LINE__);

    /* Allocate memory for the st7735 structure */
    module = kmalloc(sizeof(*module), GFP_KERNEL);
    if (!module) {
        pr_err("DevLinux: [%s %d] kmalloc failed\n", __func__, __LINE__);
        return -ENOMEM;
    }

    /* Get GPIO pins from device tree */
    module->resetPin = devm_gpiod_get(&client->dev, "reset", GPIOD_OUT_LOW);
    // module->resetPin = devm_gpiod_get(&client->dev, "reset", GPIOD_OUT_HIGH);
    if (IS_ERR(module->resetPin)) {
        pr_err("DevLinux: [%s %d] Failed to get reset GPIO pin\n", __func__, __LINE__);
        kfree(module);
        return PTR_ERR(module->resetPin);
    }

    if (gpiod_is_active_low(module->resetPin)) {
        pr_info("DevLinux: [%s %d] Reset pin is active low\n", __func__, __LINE__);
    } else {
        pr_info("DevLinux: [%s %d] Reset pin is active high\n", __func__, __LINE__);
    }

    module->dcPin = devm_gpiod_get(&client->dev, "dc", GPIOD_OUT_LOW);
    // module->dcPin = devm_gpiod_get(&client->dev, "dc", GPIOD_OUT_HIGH);
    if (IS_ERR(module->dcPin)) {
        pr_err("DevLinux: [%s %d] Failed to get DC GPIO pin\n", __func__, __LINE__);
        kfree(module);
        return PTR_ERR(module->dcPin);
    }

    if (gpiod_is_active_low(module->dcPin)) {
        pr_info("DevLinux: [%s %d] DC pin is active low\n", __func__, __LINE__);
    } else {
        pr_info("DevLinux: [%s %d] DC pin is active high\n", __func__, __LINE__);
    }

    // pr_info("DevLinux: [%s %d] Reset Pin: %d, DC Pin: %d\n", __func__, __LINE__, module->resetPin, module->dcPin);
    
    /* Configure SPI device */
    client->mode = SPI_MODE_0; // Set SPI mode
    client->bits_per_word = 8; // Set bits per word
    if (!client->max_speed_hz) {
        client->max_speed_hz = 2000000; // Set max speed to 2 MHz  
    }

    /* Store driver data in SPI device */
    spi_set_drvdata(client, module);

    /* Create character device st7735 */
    if (st7735CreateDeviceFile(module) < 0) {
        pr_err("DevLinux: [%s %d] st7735CreateDeviceFile failed\n", __func__, __LINE__);
        kfree(module);
        return -1;
    }

    /* Store the spi client */
    module->client = client;
    module->currentLine = 0;
    module->currentCursor = 0;
    module->fontSize = ST7735_FONT_SIZE;

    /* Initialize display */
    st7735InitDisplay(module);
    // st7735SetCursor(module, 0, 0);
    // st7735DisplayString(module, "DevLinux\nHello World!\n");

    /* Reset the display */
    // gpiod_set_value(module->resetPin, GPIOD_OUT_LOW);
    // gpiod_set_value(module->dcPin, GPIOD_OUT_LOW);
    // udelay(5000);
    // gpiod_set_value(module->resetPin, GPIOD_OUT_HIGH);
    // gpiod_set_value(module->dcPin, GPIOD_OUT_HIGH);
    // udelay(5000);

    /* Save module */
    st7735Module = module;

    pr_info("DevLinux: [%s %d] ST7735 OLED display SPI driver initialized\n", __func__, __LINE__);

    return 0;
}

/* ST7735 remove */
static void st7735Remove(struct spi_device *client)
{
    st7735_spi_module_t *module = spi_get_drvdata(client);

    pr_info("DevLinux: [%s %d]\n", __func__, __LINE__);

    if (!module) {
        pr_err("DevLinux: [%s %d] No module data found\n", __func__, __LINE__);
    }

    /* Clear display */
    st7735ClearDisplay(module);

    /* Remove character device */
    device_destroy(module->class, module->dev_num);
    class_destroy(module->class);
    cdev_del(&module->cdev);
    unregister_chrdev_region(module->dev_num, 1);

    kfree(module->kmalloc_ptr);
    kfree(module);

    pr_info("DevLinux: [%s %d] ST7735 OLED display driver removed\n", __func__, __LINE__);
}

static struct spi_driver st7735_driver = {
    .probe = st7735Probe,
    .remove = st7735Remove,
    .id_table = st7735_spi_id,
    .driver = {
        .name = ST7735_DRIVER_NAME,
        .of_match_table = st7735_driver_ids,
        .owner = THIS_MODULE,
    },
};
module_spi_driver(st7735_driver);

MODULE_LICENSE(DRIVER_LICENSE);
MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_DESCRIPTION(DRIVER_DESC);
MODULE_VERSION(DRIVER_VERSION);






