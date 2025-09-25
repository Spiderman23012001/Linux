// ili9341_fb.c
// Minimal framebuffer driver for ILI9341 over SPI (RGB565).
// SPDX-License-Identifier: GPL-2.0
#include <linux/module.h>
#include <linux/spi/spi.h>
#include <linux/gpio/consumer.h>
#include <linux/delay.h>
#include <linux/fb.h>
#include <linux/slab.h>
#include <linux/of.h>
#include <linux/err.h>
#include <linux/fb_deferred_io.h>
#include <linux/mutex.h>

#define DRIVER_NAME "ili9341_fb"
#define ILI9341_WIDTH  240
#define ILI9341_HEIGHT 320
#define BPP 16
#define PSEUDO_PALETTE_LEN 16

struct ili_fb {
    struct spi_device *spi;
    struct fb_info *info;
    struct gpio_desc *dc;
    struct gpio_desc *reset;
    struct mutex lock; /* protect SPI */
    struct fb_deferred_io defio;
};

static inline int ili_spi_write_cmd(struct ili_fb *fbi, const u8 *cmd, size_t len)
{
    int ret;
    /* Set DC = 0 for command */
    if (fbi->dc)
        gpiod_set_value_cansleep(fbi->dc, 0);
    mutex_lock(&fbi->lock);
    ret = spi_write(fbi->spi, cmd, len);
    mutex_unlock(&fbi->lock);
    return ret;
}

static inline int ili_spi_write_data(struct ili_fb *fbi, const u8 *data, size_t len)
{
    int ret;
    /* Set DC = 1 for data */
    if (fbi->dc)
        gpiod_set_value_cansleep(fbi->dc, 1);
    mutex_lock(&fbi->lock);
    ret = spi_write(fbi->spi, data, len);
    mutex_unlock(&fbi->lock);
    return ret;
}

/* Send command followed optionally by data */
static int ili_cmd_data(struct ili_fb *fbi, u8 cmd, const u8 *data, size_t dlen)
{
    int ret;
    ret = ili_spi_write_cmd(fbi, &cmd, 1);
    if (ret) return ret;
    if (dlen)
        ret = ili_spi_write_data(fbi, data, dlen);
    return ret;
}

/* Set column (X) and page (Y) address windows */
static int ili_set_window(struct ili_fb *fbi,
                          u16 x0, u16 y0, u16 x1, u16 y1)
{
    u8 buf[4];
    int ret;

    /* Column address set (0x2A) */
    buf[0] = (x0 >> 8) & 0xFF;
    buf[1] = x0 & 0xFF;
    buf[2] = (x1 >> 8) & 0xFF;
    buf[3] = x1 & 0xFF;
    ret = ili_cmd_data(fbi, 0x2A, buf, 4);
    if (ret) return ret;

    /* Page address set (0x2B) */
    buf[0] = (y0 >> 8) & 0xFF;
    buf[1] = y0 & 0xFF;
    buf[2] = (y1 >> 8) & 0xFF;
    buf[3] = y1 & 0xFF;
    ret = ili_cmd_data(fbi, 0x2B, buf, 4);
    if (ret) return ret;

    /* Memory write (0x2C) follows in caller */
    return 0;
}

/* Flush framebuffer memory to display (sends entire buffer). */
static void ili_flush_fb(struct fb_info *info, struct list_head *pages)
{
    struct ili_fb *fbi = info->par;
    u8 cmd = 0x2C; /* memory write */
    size_t total_bytes = info->fix.smem_len;
    u8 *buf = info->screen_base;
    size_t chunk = 4096; /* chunk size for spi_write */
    size_t offset = 0;
    int ret;

    /* Set full window */
    ret = ili_set_window(fbi, 0, 0, ILI9341_WIDTH - 1, ILI9341_HEIGHT - 1);
    if (ret) {
        dev_err(&fbi->spi->dev, "set_window failed %d\n", ret);
        return;
    }

    /* send memory write command */
    ret = ili_spi_write_cmd(fbi, &cmd, 1);
    if (ret) {
        dev_err(&fbi->spi->dev, "memwrite cmd failed\n");
        return;
    }

    /* send pixel data in chunks */
    while (offset < total_bytes) {
        size_t to_send = min(chunk, total_bytes - offset);
        ret = ili_spi_write_data(fbi, buf + offset, to_send);
        if (ret) {
            dev_err(&fbi->spi->dev, "spi data write failed: %d\n", ret);
            return;
        }
        offset += to_send;
    }
}

/* framebuffer ops (minimal) */
static struct fb_ops ili_fbops = {
    .owner = THIS_MODULE,
    /* We use deferred_io to flush; minimal ops here: */
    .fb_check_var = fb_check_var,
    .fb_set_par = NULL,
    .fb_fillrect = sys_fillrect,   /* use generic helpers operating on memory */
    .fb_copyarea = sys_copyarea,
    .fb_imageblit = sys_imageblit,
};

static void ili_hw_reset(struct ili_fb *fbi)
{
    if (!fbi->reset)
        return;
    /* active low reset */
    gpiod_set_value_cansleep(fbi->reset, 0);
    msleep(20);
    gpiod_set_value_cansleep(fbi->reset, 1);
    msleep(120);
}

/* Basic init sequence for ILI9341 (common subset). Adjust as needed. */
static int ili_init_sequence(struct ili_fb *fbi)
{
    int ret;
    /* Hardware reset */
    ili_hw_reset(fbi);

    /* Software initialization commands (subset) */
    /* Power control A */
    {
        u8 data[] = {0x39, 0x2C, 0x00, 0x34, 0x02};
        ret = ili_cmd_data(fbi, 0xCB, data, sizeof(data));
        if (ret) return ret;
    }
    /* Power control B */
    {
        u8 data[] = {0x00, 0xC1, 0x30};
        ret = ili_cmd_data(fbi, 0xCF, data, sizeof(data));
        if (ret) return ret;
    }
    /* Driver timing control A */
    {
        u8 data[] = {0x85, 0x00, 0x78};
        ret = ili_cmd_data(fbi, 0xE8, data, sizeof(data));
        if (ret) return ret;
    }
    /* Driver timing control B */
    {
        u8 data[] = {0x00, 0x00};
        ret = ili_cmd_data(fbi, 0xEA, data, sizeof(data));
        if (ret) return ret;
    }
    /* Power on sequence control */
    {
        u8 data[] = {0x64, 0x03, 0x12, 0x81};
        ret = ili_cmd_data(fbi, 0xED, data, sizeof(data));
        if (ret) return ret;
    }
    /* Pump ratio control */
    {
        u8 data[] = {0x20};
        ret = ili_cmd_data(fbi, 0xF7, data, sizeof(data));
        if (ret) return ret;
    }
    /* Power control, VRH[5:0] */
    {
        u8 data[] = {0x23};
        ret = ili_cmd_data(fbi, 0xC0, data, sizeof(data));
        if (ret) return ret;
    }
    /* Power control, SAP[2:0];BT[3:0] */
    {
        u8 data[] = {0x10};
        ret = ili_cmd_data(fbi, 0xC1, data, sizeof(data));
        if (ret) return ret;
    }
    /* VCM control */
    {
        u8 data[] = {0x3e, 0x28};
        ret = ili_cmd_data(fbi, 0xC5, data, sizeof(data));
        if (ret) return ret;
    }
    /* VCM control 2 */
    {
        u8 data[] = {0x86};
        ret = ili_cmd_data(fbi, 0xC7, data, sizeof(data));
        if (ret) return ret;
    }
    /* Memory access control (orientation) */
    {
        u8 data[] = {0x48}; /* MX, BGR order; change if needed */
        ret = ili_cmd_data(fbi, 0x36, data, sizeof(data));
        if (ret) return ret;
    }
    /* Pixel format set (16 bits/pixel) */
    {
        u8 data[] = {0x55}; /* 0x55 = 16-bit/pixel */
        ret = ili_cmd_data(fbi, 0x3A, data, sizeof(data));
        if (ret) return ret;
    }
    /* Frame rate control */
    {
        u8 data[] = {0x00, 0x18};
        ret = ili_cmd_data(fbi, 0xB1, data, sizeof(data));
        if (ret) return ret;
    }
    /* Display function control */
    {
        u8 data[] = {0x08, 0x82, 0x27};
        ret = ili_cmd_data(fbi, 0xB6, data, sizeof(data));
        if (ret) return ret;
    }
    /* Exit Sleep */
    ret = ili_cmd_data(fbi, 0x11, NULL, 0);
    if (ret) return ret;
    msleep(120);
    /* Display ON */
    ret = ili_cmd_data(fbi, 0x29, NULL, 0);
    if (ret) return ret;

    return 0;
}

/* probe / remove */
static int ili_probe(struct spi_device *spi)
{
    struct ili_fb *fbi;
    struct fb_info *info;
    int ret;

    fbi = devm_kzalloc(&spi->dev, sizeof(*fbi), GFP_KERNEL);
    if (!fbi)
        return -ENOMEM;

    fbi->spi = spi;
    mutex_init(&fbi->lock);

    /* get optional gpios from DT: "dc" and "reset" */
    fbi->dc = devm_gpiod_get_optional(&spi->dev, "dc", GPIOD_OUT_LOW);
    if (IS_ERR(fbi->dc)) {
        dev_warn(&spi->dev, "no dc gpio found or invalid\n");
        fbi->dc = NULL;
    }
    fbi->reset = devm_gpiod_get_optional(&spi->dev, "reset", GPIOD_OUT_HIGH);
    if (IS_ERR(fbi->reset)) {
        dev_warn(&spi->dev, "no reset gpio found or invalid\n");
        fbi->reset = NULL;
    }

    spi_set_drvdata(spi, fbi);

    /* allocate framebuffer info */
    info = framebuffer_alloc(0, &spi->dev);
    if (!info)
        return -ENOMEM;

    fbi->info = info;
    info->par = fbi;
    info->fbops = &ili_fbops;
    info->fix.smem_len = ILI9341_WIDTH * ILI9341_HEIGHT * (BPP/8);
    info->fix.type = FB_TYPE_PACKED_PIXELS;
    info->fix.visual = FB_VISUAL_TRUECOLOR;
    info->fix.line_length = ILI9341_WIDTH * (BPP/8);

    strncpy(info->fix.id, "ili9341fb", sizeof(info->fix.id));

    info->var.xres = ILI9341_WIDTH;
    info->var.yres = ILI9341_HEIGHT;
    info->var.xres_virtual = ILI9341_WIDTH;
    info->var.yres_virtual = ILI9341_HEIGHT;
    info->var.bits_per_pixel = BPP;

    /* RGB565 layout */
    info->var.red.offset = 11;   info->var.red.length = 5;
    info->var.green.offset = 5;  info->var.green.length = 6;
    info->var.blue.offset = 0;   info->var.blue.length = 5;

    /* allocate actual screen memory */
    info->screen_base = vzalloc(info->fix.smem_len);
    if (!info->screen_base) {
        framebuffer_release(info);
        return -ENOMEM;
    }
    info->fix.smem_start = (unsigned long)info->screen_base;

    /* deferred io setup: flush entire buffer on changes */
    fbi->defio.delay = 100; /* ms */
    fbi->defio.deferred_io = ili_flush_fb;
    fb_deferred_io_init(info, &fbi->defio);

    /* init hardware */
    /* make sure spi is configured: mode 0, 8bits */
    spi->mode = SPI_MODE_0;
    spi->bits_per_word = 8;
    spi->max_speed_hz = 16000000; /* 16 MHz typical; adjust as your panel allows */
    ret = spi_setup(spi);
    if (ret) {
        dev_err(&spi->dev, "spi_setup failed: %d\n", ret);
        goto err_free;
    }

    ret = ili_init_sequence(fbi);
    if (ret) {
        dev_err(&spi->dev, "ili_init_sequence failed: %d\n", ret);
        goto err_free;
    }

    ret = register_framebuffer(info);
    if (ret < 0) {
        dev_err(&spi->dev, "register_framebuffer failed: %d\n", ret);
        goto err_free;
    }

    dev_info(&spi->dev, "ili9341 framebuffer probed: /dev/fb%d\n", info->node);
    return 0;

err_free:
    if (fbi->info) {
        fb_deferred_io_cleanup(info);
        vfree(info->screen_base);
        framebuffer_release(info);
    }
    return ret;
}

static int ili_remove(struct spi_device *spi)
{
    struct ili_fb *fbi = spi_get_drvdata(spi);
    struct fb_info *info = fbi->info;

    unregister_framebuffer(info);
    fb_deferred_io_cleanup(info);
    vfree(info->screen_base);
    framebuffer_release(info);

    dev_info(&spi->dev, "ili9341 removed\n");
    return 0;
}

static const struct of_device_id ili_of_match[] = {
    { .compatible = "ilitek,ili9341", },
    { /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, ili_of_match);

static struct spi_driver ili_driver = {
    .driver = {
        .name = DRIVER_NAME,
        .of_match_table = ili_of_match,
    },
    .probe = ili_probe,
    .remove = ili_remove,
};

module_spi_driver(ili_driver);

MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("Minimal ILI9341 framebuffer driver over SPI (RGB565)");
MODULE_LICENSE("GPL");
