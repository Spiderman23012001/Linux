# LED BLINK KERNEL MODULE (m_cdev)
## 1. Introduction
- Header files that define the interface between the Linux kernel and user-space programs or kernel modules. They include definitions for system calls, data structures, and constants needed to compile programs or drivers that interact with the kernel.
- `uname -r`: Displays the kernel version
- `sudo apt install -y linux-headers-` `uname -r`
- Linux kernel module include: 
    - **init (insmod)**
    - **exit (rmmod)** 
    ```c
    module_init(chdev_init);
    module_exit(chdev_exit);
    ```

## 2. Character device file
### 2.1 Device number 
- To identify device file
- Include: **major number** and **minor number**
- 2 ways to generate device number:
    - static
    ```c
    dev_t dev = MKDEV(173, 0);
    register_chrdev_region(&mdev.dev_num, 1, "m-cdev")
    ```
    - dynamic (recommend)
    ```c
    if ((ret = alloc_chrdev_region(&mdev.dev_num, 0, 1, DEVICE_NAME)) < 0) {
        pr_err("Failed to allocate chrdev\n");
    return ret;
    }
    ```
### 2.1 Device file
- Device file use to communicate with device in /dev
- Create device (device file):
    ```c
    ```
- **Operation:** open, release, read, write
    - 
## 3. GPIO
### 3.1 Legacy
    - Peripherals (at physical address 0x3F000000 on) are mapped into the kernel virtual address
    space starting at address 0xF2000000. Thus a peripheral advertised here at bus address
    0x7Ennnnnn is available in the ARM kenel at virtual address 0xF2nnnnnn.
    - **Base address gpio physical at 0x3F200000 = 0x7E200000 at virtual address**
    - Select 
    | GPIO\_PIN | GPFSELx | Offset bit |
    | --------- | ------- | ---------- |
    | 0–9       | GPFSEL0 | 0–27       |
    | 10–19     | GPFSEL1 | 0–27       |
    | 20–29     | GPFSEL2 | 0–27       |
    | 30–39     | GPFSEL3 | 0–27       |
    | 40–49     | GPFSEL4 | 0–27       |
    | 50–53     | GPFSEL5 | 0–11       |
    - Set : Logic 1
    - Clear : Logic 0

### 3.2 Integer-base

### 3.3 Device tree

## 4. Build 
- Step 1: Terminal: `devtool modify virtual/kernel`
- Step 2: Create:
    ```c
    workspace/sources/linux-raspberrypi/drivers/mgpio/
    ├── Kconfig
    ├── Makefile
    └── mgpio.c
    ```
- Step 3: Add module to kernel 
    - **Makefile**
    ```Makefile
    EXTRA_CFLAGS = -Wall
    obj-$(CONFIG_MGPIO) = mgpio.o
    ```
    - **Kconfig** (include 1 line below endmenu)
    ```Makefile
    menu "mgpio device driver"

    config MGPIO
        bool "mgpio device driver"
        depends on ARM
        default y if ARM
        help
            Select this option to run mgpio module.
    endmenu

    ```
- Step 4: Update Makefile and Kconfig
    - Add to linux-raspberrypi/drivers/Makefile:
    ```Makefile
    obj-$(CONFIG_MGPIO) += mgpio/
    ```
    - Add to linux-raspberrypi/drivers/Kconfig:
    ```Makefile
    source "drivers/mgpio/Kconfig"
    ```
- Step 5: Configure kernel to active module
    - Add `linux-raspberrypi/arch/arm/configs/bcmrpi_defconfig`
    ```Makefile
    CONFIG_MGPIO=m
    ```
- Step 6: Create patch
    - cd to `linux-raspberrypi:` and run cmd below:
    ```bash
    git add drivers/mgpio/
    git add arch/arm/configs/bcmrpi_defconfig
    git add drivers/Kconfig
    git add drivers/Makefile
    git commit -m "Add mgpio module for controlling LED on GPIO27"
    git format-patch -1
    ```
    - Copy `.patch` file to
    ```bash
    cp workspace/sources/linux-raspberrypi/0001-Add-mgpio-module-for-controlling-LED-on-GPIO27.patch ../meta-custom/recipes-kernel/linux/linux-raspberrypi/
    ```
- Step 6: Create bbappend file for kernel
    - **Method 1**
        - `cd /meta-custom/recipes-kernel/linux/linux-raspberrypi_%.bbappend`
        - 
        ```c
        FILESEXTRAPATHS_prepend := "${THISDIR}/${PN}:"

        SRC_URI += "file://0001-Add-mgpio-module-for-controlling-LED-on-GPIO27.patch \
                file://mgpio.cfg \
        "

        KERNEL_MODULE_AUTOLOAD += " mgpio"
        ```
        - mgpio.cfg
        ```bash
        CONFIG_MGPIO=m
        ```
    - **Method 2**
        - `cd /build/workspace/sources/linux-raspberrypi/arch/arm/configs`
        - Select bcm2709_defconfig for raspberry pi3B+
        - Add `CONFIG_MGPIO=m` (build module) or `CONFIG_MGPIO=y` (build-in)
- Step 7:
    - run `bitbake virtual/kernel`
## 4.1 Build module 
- Intergrate module (.ko) to image
    - ../meta-custom/recipes-core/images/core-image-minimal.bbappend
    ```bash
    IMAGE_INSTALL:append = " kernel-module-mgpio"
    ```



## 4.2 Build-in
