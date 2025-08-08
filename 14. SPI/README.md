# ST7735 
## 1. Tổng quát
- 396 đường điều khiển cột (source lines)
- 162 đường điều khiển dòng (gate lines)
- => Điều này giúp điều khiển ma trận điểm ảnh trên màn hình.

Chip có thể đọc/ghi dữ liệu màn hình mà `không cần xung clock bên ngoài`, giúp tiết kiệm điện năng – rất hữu ích cho thiết bị di động, tiết kiệm pin.
Kích thước:
- Chiều ngang: 132 pixel
- Chiều dọc: 162 pixel
- **Chiều sâu: 262,144 (màu, 2^18 R-G-B 6-6-6)**

## 2. Features
### 2.1 Single chip TFT-LCD Controller/Driver with RAM
- Đây là **chip đơn** tích hợp cả:
    - Mạch **điều khiển (controller)** -> Xử lý lệnh từ vi điều khiển
    - Mạch **driver** -> Điều khiển điện áp đến các pixel LCD
    - **Bộ nhớ RAM tích hợp** dùng làm bộ đếm hiển thị (frame buffer)
### 2.2 Display Resolution 
- 132 (RGB) x 162 (GM=000)
- 128 (RGB) x 160 (GM=011)
- Tùy cấu hình GM[2:0], độ phân giải hiển thị có thể là:
    - 132 x 162 pixel (full)
    - 128 x 160 pixel (thường dùng) – để phù hợp khung hình
### 2.3 Color Mode
- Full color: 262K (RGB=666)
- 8-color: RGB=111 (Idle Mode ON) (chỉ hiển thị 8 màu cơ bản - **tiết kiệm pin**)
### 2.4 Pixel format
- 12-bit RGB: 4-4-4
- 16-bit RGB: 5-6-5
- 18-bit RGB: 6-6-6
### 2.5. Giao tiếp hỗ trợ
- Parallel 8080 (8,9,16,18 bit)
- SPI 3 wire,4 wire
### 2.6 Mạch tích hợp sẵn
- Mach tạo nguồn để chạy LCD 
- Mạch tạo điện áp VCOM
- Bộ nhớ không mất NV chứa cấu hình ban đầu
- Dao động nội để tạo xung clock cho LCD
- Bộ điều khiển thời gian (timing controller)
### 2.7 Điện áp hoạt động
| Thành phần               | Khoảng điện áp |
| ------------------------ | -------------- |
| VDDI (I/O)               | 1.65V → VDD    |
| VDD (Analog)             | 2.6V → 3.3V    |
| GVDD (nguồn nguồn LCD)   | 3.0V → 5.0V    |
| VCOMH (điện áp cao LCD)  | 2.5V → 5.0V    |
| VCOML (điện áp thấp LCD) | -2.4V → 0V     |
| VGH (Gate High)          | +10V → +15V    |
| VGL (Gate Low)           | -12.4V → -7.5V |
### 2.8 Power Level
6 level modes are defined they are in order of Maximum Power consumption to Minimum Power Consumption
1. Normal Mode On (full display), Idle Mode Off, Sleep Out.
In this mode, the display is able to show maximum 262,144 colors.
2. Partial Mode On, Idle Mode Off, Sleep Out.
In this mode part of the display is used with maximum 262,144 colors.
3. Normal Mode On (full display), Idle Mode On, Sleep Out.
In this mode, the full display area is used but with 8 colors.
4. Partial Mode On, Idle Mode On, Sleep Out.
In this mode, part of the display is used but with 8 colors.
5. Sleep In Mode
In this mode, the DC: DC converter, internal oscillator and panel driver circuit are stopped. Only the MCU interface and
memory works with VDDI power supply. Contents of the memory are safe.
6. Power Off Mode
In this mode, both VDD and VDDI are removed.

### 2.9 Reset table
### **Reset Table (Default Value, GM[2:0]=“011”, 128RGB x 160)**

| Mục (Item)                             | Sau khi Cấp Nguồn (Power On) | Sau khi Hardware Reset (H/W Reset) | Sau khi Software Reset (S/W Reset) | Mô tả / Ghi chú                                    |
|---------------------------------------|------------------------------|------------------------------------|------------------------------------|----------------------------------------------------|
| **Frame memory**                      | Dữ liệu ngẫu nhiên (Random)  | Không thay đổi (No Change)          | Không thay đổi (No Change)          | Dữ liệu trong bộ nhớ RAM hình ảnh không được xóa bởi reset |
| **Sleep In/Out Mode**                 | Sleep In                     | Sleep In                           | Sleep In                           | Màn hình ở trạng thái Sleep In (chế độ tiết kiệm pin) |
| **Display On/Off**                    | Off                          | Off                                | Off                                | Màn hình tắt (không hiển thị)                       |
| **Display mode (normal/partial)**    | Normal                       | Normal                             | Normal                             | Hiển thị chế độ bình thường, không phải partial display |
| **Display inversion On/Off**          | Off                          | Off                                | Off                                | Chế độ đảo màu tắt                                  |
| **Display Idle Mode On/Off**          | Off                          | Off                                | Off                                | Chế độ Idle tắt                                     |
| **Column: Start Address (XS)**         | 0x0000                       | 0x0000                            | 0x0000                            | Địa chỉ cột bắt đầu                                 |
| **Column: End Address (XE)**           | 0x007F (127 decimal)          | 0x007F (127 decimal)               | 0x007F (127 decimal) (MV=0) hoặc 0x009F (159 decimal) (MV=1) | Địa chỉ cột kết thúc; giá trị thay đổi nếu MV thay đổi |
| **Row: Start Address (YS)**            | 0x0000                       | 0x0000                            | 0x0000                            | Địa chỉ hàng bắt đầu                                |
| **Row: End Address (YE)**              | 0x009F (159 decimal)          | 0x009F (159 decimal)               | 0x009F (159 decimal) (MV=0) hoặc 0x007F (127 decimal) (MV=1) | Địa chỉ hàng kết thúc; giá trị thay đổi nếu MV thay đổi |
| **Gamma setting**                     | GC0                          | GC0                                | GC0                                | Gamma curve mặc định (GC0) được chọn                |
| **RGB for 4k and 65k Color Mode**    | See Section 9.17             | See Section 9.17                  | Không thay đổi                    | Tham khảo phần bảng LUT gamma và conversion màu trong datasheet |
| **Partial: Start Address (PSL)**      | 0x0000                       | 0x0000                            | 0x0000                            | Địa chỉ bắt đầu vùng partial (mặc định toàn bộ màn hình) |
| **Partial: End Address (PEL)**        | 0x009F (159 decimal)          | 0x009F (159 decimal)               | 0x009F (159 decimal)               | Địa chỉ kết thúc vùng partial                       |
| **Tearing: On/Off**                   | Off                          | Off                                | Off                                | Tearing effect tắt                                  |
| **Tearing Effect Mode**               | Mode 1 (V-Blanking only)      | Mode 1 (V-Blanking only)           | Mode 1 (V-Blanking only)           | Chế độ tearing effect mặc định                       |
| **Memory Data Access Control (MY/MX/MV/ML/RGB)** | 0/0/0/0/0                 | 0/0/0/0/0                        | Không thay đổi                    | Các bit điều khiển hướng dữ liệu hiển thị đều về giá trị mặc định |
| **Interface Pixel Color Format**      | 6 (18-Bit/Pixel)             | 6 (18-Bit/Pixel)                  | Không thay đổi                    | Định dạng màu giao diện 18-bit/pixel                |
| **Các lệnh Read Register**            | Các giá trị mặc định cho RDDPM, RDDMADCTL, RDDCOLMOD, RDDIM, RDDSM | Không thay đổi                    | Không thay đổi                    | Các lệnh đọc trạng thái chip trả về giá trị mặc định |
| **ID2, ID3**                         | Giá trị lưu trong NV Memory   | Không thay đổi                    | Không thay đổi                    | Các mã nhận dạng module/version                     |

### **Reset Table (GM[2:0]= “000”, 132RGB x 162)**

| Mục (Item)                                  | Sau khi Cấp Nguồn (Power On) | Sau khi Hardware Reset (H/W Reset) | Sau khi Software Reset (S/W Reset)            | Mô tả / Ghi chú                                                                          |
|---------------------------------------------|------------------------------|------------------------------------|-----------------------------------------------|------------------------------------------------------------------------------------------|
| **Frame memory (Bộ nhớ RAM hình ảnh)**      | Dữ liệu ngẫu nhiên (Random)  | Không thay đổi (No Change)         | Không thay đổi (No Change)                    | Dữ liệu trong bộ nhớ không bị xóa tự động khi reset, dữ liệu có thể còn cũ hoặc ngẫu nhiên. |
| **Sleep In/Out Mode**                       | Sleep In                     | Sleep In                           | Sleep In                                      | Mặc định chip ở trạng thái Sleep In để tiết kiệm năng lượng khi chưa kích hoạt hiển thị.   |
| **Display On/Off**                          | Off                          | Off                                | Off                                           | Màn hình tắt hiển thị sau khi reset.                                                     |
| **Display mode (normal/partial)**           | Normal                       | Normal                             | Normal                                        | Màn hình ở chế độ hiển thị bình thường (full display), không phải partial.                |
| **Display Inversion On/Off**                | Off                          | Off                                | Off                                           | Chế độ đảo màu (Inversion) tắt.                                                          |
| **Display Idle Mode On/Off**                | Off                          | Off                                | Off                                           | Chế độ Idle tắt, hiển thị đầy màu sắc.                                                   |
| **Column: Start Address (XS)**              | 0000h                        | 0000h                              | 0000h                                         | Địa chỉ cột bắt đầu của vùng hiển thị.                                                   |
| **Column: End Address (XE)**                | 0083h (131 decimal)          | 0083h (131 decimal)                | 0083h (131 decimal) (MV=0) hoặc 00A1h (161 decimal) (MV=1) | Địa chỉ cột kết thúc vùng hiển thị, giá trị kết thúc phụ thuộc bit MV trong lệnh MADCTL.  |
| **Row: Start Address (YS)**                 | 0000h                        | 0000h                              | 0000h                                         | Địa chỉ hàng bắt đầu của vùng hiển thị.                                                  |
| **Row: End Address (YE)**                   | 00A1h (161 decimal)          | 00A1h (161 decimal)                | 00A1h (161 decimal) (MV=0) hoặc 0083h (131 decimal) (MV=1) | Địa chỉ hàng kết thúc vùng hiển thị, giá trị kết thúc phụ thuộc bit MV.                   |
| **Gamma setting**                           | GC0                          | GC0                                | GC0                                           | Đường cong gamma mặc định (GC0).                                                         |
| **RGB for 4k and 65k Color Mode**           | Tham khảo phần 9.17          | Tham khảo phần 9.17                | Không thay đổi                                 | Bảng chuyển đổi màu và LUT (look-up table) dùng trong hiển thị.                           |
| **Partial: Start Address (PSL)**            | 0000h                        | 0000h                              | 0000h                                         | Địa chỉ bắt đầu vùng hiển thị Partial, mặc định toàn màn hình.                            |
| **Partial: End Address (PEL)**              | 00A1h (161 decimal)          | 00A1h (161 decimal)                | 00A1h (161 decimal)                            | Địa chỉ kết thúc vùng hiển thị Partial.                                                  |
| **Tearing: On/Off**                         | Off                          | Off                                | Off                                           | Tearing effect (đồng bộ quét khung hình) mặc định tắt.                                   |
| **Tearing Effect Mode**                     | Mode 1 (V-Blanking only)     | Mode 1 (V-Blanking only)           | Mode 1 (V-Blanking only)                      | Chế độ tearing effect mặc định là mode 1 (chỉ đồng bộ khoảng trắng dọc).                 |
| **Memory Data Access Control (MY/MX/MV/ML/RGB)** | 0/0/0/0/0                | 0/0/0/0/0                          | Không thay đổi                                 | Các bit điều khiển hướng quét và ánh xạ dữ liệu mặc định không đảo chiều, RGB chuẩn.       |
| **Interface Pixel Color Format**            | 6 (18-Bit/Pixel)             | 6 (18-Bit/Pixel)                   | Không thay đổi                                 | Định dạng màu giao diện: 18-bit/pixel, 262K màu.                                         |
| **Các lệnh Read Register**                  | Giá trị mặc định             | Không thay đổi                     | Không thay đổi                                 | Các lệnh đọc trạng thái trả về giá trị mặc định.                                          |
| **ID2, ID3**                                | Giá trị lưu trong NV Memory  | Không thay đổi                     | Không thay đổi                                 | Các mã nhận dạng module/version được lưu trong bộ nhớ không bay.                          |


# 3. Viết driver
## 3.1 Khởi tạo toàn cục
- `message`: bộ đệm chứa thông điệp cần hiển thị.
- `st7735Module`: con trỏ tới cấu trúc quản lý trạng thái của driver.
```c
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

char message[ST7735_MAX_BUFF]; /* Message buffer for display */
st7735_spi_module_t* st7735Module = NULL; /* ST7735 module instance */
```
## 3.2 Định nghĩa bảng tương thích và bảng ID SPI
- `st7735_driver_ids` và `st7735_spi_id`: định nghĩa các thiết bị tương thích (dùng cho Device Tree hoặc SPI subsystem Linux để tự động nhận diện driver).
```c
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
```

## 3.3 Định nghĩa các hàm thao tác file
- `st7735Open` và `st7735Release`: xử lý mở và đóng thiết bị.
- `st7735WriteOps`: nhận dữ liệu từ user space và gửi tới màn hình để hiển thị.
- `st7735ReadOps`: đọc dữ liệu (ở đây là dữ liệu message đã gửi lên màn hình) trả về user space.
```c
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
```

## 3.4 Hàm khởi tạo màn hình `st7735InitDisplay`
- Chờ màn hình cấp nguồn ổn định
```c
msleep(100); // Wait for the display to power up
```
- Hardware reset
```c
gpiod_set_value(module->resetPin, 0); // Kéo chân RESET xuống thấp
udelay(5000);                         // Đợi 5 ms
gpiod_set_value(module->resetPin, 1); // Kéo chân RESET lên cao
udelay(5000);                         // Đợi 5 ms
```
- Software reset
```c
st7735Write(module, true, 0x01); // Gửi lệnh SWRESET (0x01)
udelay(5000);                    // Đợi 5 ms để chip xử lý
```
- Sleep out
```c
st7735Write(module, true, 0x11); // Gửi lệnh Sleep Out (0x11)
udelay(5000);                    // Đợi 5 ms để chip chuẩn bị
```
- Cấu hình tốc độ khung (Frame rate control)
    - **Normal mode (full colors)**
    ```c
    st7735Write(module, true, 0xB1);  // Lệnh Frame Rate Control
    st7735Write(module, false, 0x01);
    st7735Write(module, false, 0x2C);
    st7735Write(module, false, 0x2D);
    ```
    - **Idle mode (8 colors)**
    ```c
    st7735Write(module, true, 0xB2);
    st7735Write(module, false, 0x01);
    st7735Write(module, false, 0x2C);
    st7735Write(module, false, 0x2D);
    ```
    - **Partial mode (full colors)**
    ```c
    st7735Write(module, true, 0xB3);
    st7735Write(module, false, 0x01);
    st7735Write(module, false, 0x2C);
    st7735Write(module, false, 0x2D);
    st7735Write(module, false, 0x01);
    st7735Write(module, false, 0x2C);
    st7735Write(module, false, 0x2D);
    ```

- Cấu hình Display Inversion Control
```c
st7735Write(module, true, 0xB4);  // Display Inversion Control
st7735Write(module, false, 0x07);
```
- Cấu hình Power Control (nguồn và điện áp)
```c
st7735Write(module, true, 0xC0);  // Power Control 1
st7735Write(module, false, 0xA2);
st7735Write(module, false, 0x02);
st7735Write(module, false, 0x84);

st7735Write(module, true, 0xC1);  // Power Control 2
st7735Write(module, false, 0xC5);

st7735Write(module, true, 0xC2);  // Power Control 3 (Normal mode)
st7735Write(module, false, 0x0A);
st7735Write(module, false, 0x00);

st7735Write(module, true, 0xC3);  // Power Control 4 (Idle mode)
st7735Write(module, false, 0x8A);
st7735Write(module, false, 0x2A);

st7735Write(module, true, 0xC4);  // Power Control 5 (Partial mode)
st7735Write(module, false, 0x8A);
st7735Write(module, false, 0xEE);

st7735Write(module, true, 0xC5);  // VCOM Control 1
st7735Write(module, false, 0x0E);
```
- Tắt chế độ đảo màu (Display Inversion Off)
```c
st7735Write(module, true, 0x20);  // Display Inversion Off
```
- Cấu hình Memory Data Access Control (MADCTL)
```c
st7735Write(module, true, 0x36);  // Memory Data Access Control
st7735Write(module, false, 0xA0); // Thiết lập: RGB mode + quét ngang
```
- Định dạng pixel (Interface Pixel Format)
```c
st7735Write(module, true, 0x3A);  // Interface Pixel Format
st7735Write(module, false, 0x05); // 16-bit/pixel
```
- Đặt vùng hiển thị (Column and Row Address Set)
```c
// Column address set (0 đến 160)
st7735Write(module, true, 0x2A);
st7735Write(module, false, 0x00);
st7735Write(module, false, 0x00);
st7735Write(module, false, 0x00);
st7735Write(module, false, 160);

// Row address set (0 đến 128)
st7735Write(module, true, 0x2B);
st7735Write(module, false, 0x00);
st7735Write(module, false, 0x00);
st7735Write(module, false, 0x00);
st7735Write(module, false, 128);
```
- Cấu hình Gamma Adjustment (+ polarity)
```c
st7735Write(module, true, 0xE0);
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
```
- Cấu hình Gamma Adjustment (- polarity)
```c
st7735Write(module, true, 0xE1);
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
```
- Tắt chế độ partial display nếu có (Đảm bảo hiển thị toàn màn hình)
```c
st7735Write(module, true, 0x13);
```
- Bật màn hình hiển thị (Display ON)
```c
st7735Write(module, true, 0x29);
```
- Xóa màn hình và hiển thị hình ảnh khởi tạo
```c
st7735ClearDisplay(module);                                                         
st7735DisplayImage(module, image01Tbl, ST7735_MAX_SEG, ST7735_MAX_ROW);
```



