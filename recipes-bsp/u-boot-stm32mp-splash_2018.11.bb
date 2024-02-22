SUMMARY = "Universal Boot Loader Splash Screen for THOR-E boards"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COREBASE}/meta/files/common-licenses/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

SRC_URI = "${@bb.utils.contains('MACHINE_FEATURES', 'splashscreen', 'file://${UBOOT_SPLASH_LANDSCAPE_SRC} file://${UBOOT_SPLASH_PORTRAIT_SRC}', '', d)}"

S = "${WORKDIR}"

UBOOT_SPLASH_LANDSCAPE_SRC = "thor_logo_480x458.bmp"
UBOOT_SPLASH_PORTRAIT_SRC = "thor_logo_458x480.bmp"
UBOOT_SPLASH_LANDSCAPE_IMAGE ?= "splash_landscape"
UBOOT_SPLASH_PORTRAIT_IMAGE ?= "splash_portrait"

inherit deploy

do_compile[noexec] = "1"

do_install() {
    install -d ${D}/boot
    if [ -e "${S}/${UBOOT_SPLASH_LANDSCAPE_SRC}" ]; then
        install -m 644 ${S}/${UBOOT_SPLASH_LANDSCAPE_SRC} ${D}/boot/${UBOOT_SPLASH_LANDSCAPE_IMAGE}.bmp
    fi
    if [ -e "${S}/${UBOOT_SPLASH_PORTRAIT_SRC}" ]; then
        install -m 644 ${S}/${UBOOT_SPLASH_PORTRAIT_SRC} ${D}/boot/${UBOOT_SPLASH_PORTRAIT_IMAGE}.bmp
    fi
}

ALLOW_EMPTY:${PN} = "1"
FILES:${PN} = "/boot"
