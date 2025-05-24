## Hello kernel modules
- `make clean`
- `make all`
- `insmod hello.ko`
- `rmmod hello`
- `dmesg | tail`

## BSP = U-Boot + Kernel + Device Tree + RootFS + Firmware (nếu cần) + Toolchain + Scripts
- U-Boot, SPL (Secondary Program Loader): Bootloader
- Linux Kernel: Source kernel , driver (GPIO, UART, SPI, I2C, Ethernet, v.v.), Device Tree (DTB)   
- Device Tree (DT)
- Root Filesystem (RootFS): Yocto/Buildroot build as BusyBox, Debian/Ubuntu-based
- Kernel Modules
- Firmware & Binary Blobs (Option): firmware for camera, wifi, GPU.
- Toolchain: GCC, GDB, QEMU, v.v.
- Scripts & Build System: Makefile, Yocto recipes, Buildroot configs, Flashing tool/scripts.