## 1. Adding **Graphical Desktop Environment** to Yocto Project Images
- Add to `local.conf`
    ```bash
    IMAGE_INSTALL:append = " \
        packagegroup-xfce-base \
        packagegroup-core-x11 \
    "

    DISTRO_FEATURES:append = " x11"
    ```
- Create `/meta-custom/conf/xorg-vc4-config/files/99-vc4.conf`
    ```bash
    Section "OutputClass"
    Identifier "vc4"
    MatchDriver "vc4"
    Driver "modesetting"
    Option "PrimaryGPU" "true"
    EndSection
    ```

- Create `meta-custom/conf/xorg-vc4-config/xorg-vc4-config_1.0.0.bb`
    ```bash
    SUMMARY = "Deploys X11 configuration"
    LICENSE = "CLOSED"
    SRC_URI:append = " file://99-vc4.conf "

    S = "${WORKDIR}"

    do_install(){
        install -d ${D}/etc/X11/xorg.conf.d
        cp -r ${S}/99-vc4.conf ${D}/etc/X11/xorg.conf.d
    }

    FILES:${PN}:append = " /etc/X11/xorg.conf.d/99-vc4.conf "
    ```
- Add to `local.conf`
    ```bash
    IMAGE_INSTALL:append = " \
        packagegroup-xfce-base \
        packagegroup-core-x11 \
        xorg-vc4-config \
    "
    ```
- Add `VC4DTBO = "vc4-fkms-v3d"`(check in meta-raspberrypi/raspberrypi3.conf) to `local.conf`
- Run: `bitbake core-image-minimal` 

## 2. Adding Qt6 Support and Creating Recipe for Qt6 Application
