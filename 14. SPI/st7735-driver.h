#ifndef ST7735_DRIVER_H
#define ST7735_DRIVER_H

#include <linux/spi/spi.h>
#include <linux/cdev.h>

/* ST7735 driver information */
#define DRIVER_AUTHOR "Trantin"
#define DRIVER_DESC "ST7735 Driver Kernel Module DevLinux"
#define DRIVER_VERSION "1.0"
#define DRIVER_LICENSE "GPL"

/* ST7735 device name */
#define ST7735_DEV_COMPATIBLE "devlinux,st7735spi"
#define ST7735_DEV_NAME "st7735spi"
#define ST7735_DEV_CLASS "st7735-class"
#define ST7735_DRIVER_NAME "st7735_spi_driver"

/* ST7735 Display Specs */
#define ST7735_MAX_SEG     160
#define ST7735_MAX_ROW     128
#define ST7735_FONT_HEIGHT 8
#define ST7735_FONT_SIZE   5
#define ST7735_MAX_LINE    (ST7735_MAX_ROW / ST7735_FONT_HEIGHT)
#define ST7735_MAX_BUFF    (ST7735_MAX_SEG * ST7735_MAX_ROW * ST7735_FONT_SIZE / 8)

/* GPIO Logic Level */
#define LOW  0
#define HIGH 1

/* ST7735 module structure */
typedef struct st7735_spi_module {
    struct spi_device *client;      /* SPI client */
    struct gpio_desc *resetPin;     /* Reset pin */
    struct gpio_desc *dcPin;        /* DC pin */
    struct cdev cdev;               /* Character device structure */
    dev_t dev_num;                  /* Device number */
    struct class *class;            /* Device class */
    struct device *device;          /* Device */
    uint8_t currentLine;            /* Current line for display */
    uint8_t currentCursor;          /* Current cursor for display */
    uint8_t fontSize;               /* Font size for display */
    char *kmalloc_ptr;
} st7735_spi_module_t;

extern char message[ST7735_MAX_BUFF]; /* Message buffer for display */
extern st7735_spi_module_t* st7735Module; /* ST7735 module instance */

/* ST7735 open */
static int st7735Open(struct inode *inode, struct file *file);

/* ST7735 release */
static int st7735Release(struct inode *inode, struct file *file);

/* ST7735 write */
static ssize_t st7735WriteOps(struct file *file, const char __user *buf, size_t len, loff_t *offset);

/* ST7735 read */
static ssize_t st7735ReadOps(struct file *file, char __user *buf, size_t len, loff_t *offset);


/* ST7735 display initialize */
static void st7735InitDisplay(st7735_spi_module_t *module);

/* st7735 clear display */
static void st7735ClearDisplay(st7735_spi_module_t *module);

/* st7735 display image */
static void st7735DisplayImage(st7735_spi_module_t *module, const uint8_t *image, uint8_t width, uint8_t height);

/* st7735 set window */
static void st7735SetWindow(st7735_spi_module_t *module, uint8_t x, uint8_t y, uint8_t width, uint8_t height);

/* ST7735 set cursor */
static void st7735SetCursor(st7735_spi_module_t *module, uint8_t line, uint8_t cursor);

/* ST7735 display string */
static void st7735DisplayString(st7735_spi_module_t *module, const char *str);

/* ST7735 display character */
static void st7735DisplayChar(st7735_spi_module_t *module, uint8_t data);

/* ST7735 write */
static int st7735Write(st7735_spi_module_t *module, bool isCmd, uint8_t data);


#endif // ST7735_DRIVER_H