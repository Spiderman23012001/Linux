# Recipe
## 1. Introduction
- Example Recipes
    - Create `meta-custom/recipes-devtools/helloworldc_1.0.0.bb`:
    ```bash
    SUMMARY = "Simple Hello World app"
    LICENSE = "CLOSED"

    SRC_URI = "file://helloyp.c"
    S = "${WORKDIR}"

    do_configure[noexec] = "1"  

    do_compile() {
        ${CC} ${CFLAGS} ${LDFLAGS} ${S}/helloyp.c -o helloyp
    }

    do_install() {
        install -d ${D}${bindir}
        install -m 0755 ${B}/helloyp ${D}${bindir}
    }

    ```
    - Create `meta-custom/recipes-devtools/helloworldc/files/helloyp.c`:
    ```c
    #include <stdio.h>

    int main()
    {
        printf("Hello World! \n");
        return 0;
    }
    ```
    - Run `bitbake helloworldc`
    - Add 
    ```bash
    IMAGE_INSTALL:append = " \
        nano \
        helloworldc \
    "
    ```
## 2. Creating a Recipe for Software Package that uses GNU Make
- Check [Makefile](https://github.com/mozcelikors/makefile-example/blob/master/Makefile)
- Create `meta-custom/recipes-devtools/helloworldmake/helloworldmake_1.0.0.bb`:
    ```bash
    SUMMARY = "Recipe for software package that uses GNU Make"
    LICENSE = "CLOSED"
    SRC_URI = "git://github.com/mozcelikors/makefile-example.git;protocol=https"
    SRCREV = "5fc593af10244751039f2a1163134b6b60a7df37"
    SRCREV = ${AUTOREV}
    RDEPENDS:${PN}:append = " bash "
    DEPENDS:append = " "

    S = "${WORKDIR}/git"

    do_compile(){
        export OECORE_TARGET_SYSROOT=$(STAGING_DIR_TARGET)
        oe_make -C .
    }

    do_install(){
        install -d ${D}${bindir}
        install -m 0755 ${S}/app ${D}/${bindir}/app
        install -m 0755 ${S}/app ${D}/${bindir}/app2
    }

    PACKAGES = "${PN} \
                ${PN}-app \
                ${PN}-app2 \
                ${PN}-dbg \
    "

    FILE:${PN}:append = " ${bindir}/app "
    FILE:${PN}-app:append = " ${bindir}/app "
    FILE:${PN}-app2:append = " ${bindir}/app2 "
    FILE:${PN}-dbg:append = " ${bindir}/.debug ${bindir}/.debug/app ${bindir}/.debug/app2 "
    ```
## 3. Creating a recipe for software package that uses CMake
- Create `meta-custom/recipes-devtools/raspberrypi-utils/raspberrypi-utils_1.0.0.bb`:
    ```bash
    FILESEXTRAPATHS_prepend := "${THISDIR}/file2:"


    SUMMARY = "Recipe file for few raspberry pi tools"
    LICENSE = "CLOSED"

    RDEPENDS:${PN}:append = " bash "
    SRC_URI = "git://github.com/raspberrypi/utils.git;protocol=https;branch=master"
    SRC_URI += "file://git/CMakeLists.txt "
    SRCREV = "b7651d86d71a172b2208c67b2e360cbcb4f9d98f"

    S = "${WORKDIR}/git"

    inherit cmake pkgconfig

    do_install:append(){
        install -d ${D}${bindir}
        install -m 0755 ${B}/pinctrl/pinctrl ${D}${bindir}
        install -m 0755 ${S}/raspinfo/raspinfo ${D}${bindir}
    }

    FILES:${PN} += " /usr/share/bash-completion/completions/pinctrl"
    FILES:${PN} += " ${bindir}/pinctrl"
    FILES:${PN} += " ${bindir}/raspinfo"
    ```

- Create `meta-custom/recipes-devtools/raspberrypi-utils/file2/git/CMakeLists.txt`:
    ```bash
    cmake_minimum_required(VERSION 3.1...3.27)

    project(utils)

    # List of subsidiary CMakeLists
    #add_subdirectory(dtmerge)
    #add_subdirectory(eeptools)
    #add_subdirectory(kdtc)
    #add_subdirectory(otpset)
    #add_subdirectory(overlaycheck)
    #add_subdirectory(ovmerge)
    add_subdirectory(pinctrl)
    #add_subdirectory(piolib)
    add_subdirectory(raspinfo)
    #add_subdirectory(vcgencmd)
    #add_subdirectory(vclog)
    #add_subdirectory(vcmailbox)
    ```
- Run: `runqemu qemux86-64 nographic` -> `raspinfo`
- Tool: [hehe](https://yoctoproject.net/)

