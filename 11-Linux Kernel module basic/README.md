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

###  
