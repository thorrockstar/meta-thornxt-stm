FILESEXTRAPATHS_prepend := "${THISDIR}/patches:"

SRC_URI += " \
	file://defconfig \
	file://spidev.patch;patch=1 \
	file://panel-simple.patch;patch=1 \
	file://Kconfig.patch;patch=1 \
	file://stm32mp157f-ev1.patch;patch=1 \
	file://drm_fb_helper.patch;patch=1 \
	file://pinctrl-stm32.patch;patch=1 \
	file://stmmac_platform.patch;patch=1 \
	file://stm32_sai_sub.patch;patch=1 \
"

PACKAGE_ARCH = "${MACHINE_ARCH}"

KERNEL_MODULE_AUTOLOAD += "g_serial cdc-acm"

do_configure_prepend() {
    cp ${WORKDIR}/defconfig ${WORKDIR}/build/.config
}

do_configure_append() {
    cp ${WORKDIR}/defconfig ${WORKDIR}/build/.config
}

