FILESEXTRAPATHS:prepend := "${THISDIR}/patches:"

SRC_URI += " \
	file://defconfig \
	file://spidev.patch;patch=1 \
	file://panel-simple.patch;patch=1 \
	file://Kconfig.patch;patch=1 \
	file://stm32mp157f-ev1.patch;patch=1 \
	file://stm32mp157d-ev1.patch;patch=1 \
	file://stm32mp157a-ev1_nous_mk2.patch;patch=1 \
	file://drm_fb_helper.patch;patch=1 \
	file://pinctrl-stm32.patch;patch=1 \
	file://pinctrl-stm32mp157.patch;patch=1 \
	file://stm32_sai_sub.patch;patch=1 \
	file://stm32mp157f-ev1-m4-examples.patch;patch=1 \
"

PACKAGE_ARCH = "${MACHINE_ARCH}"

KERNEL_MODULE_AUTOLOAD += "cdc-acm"

do_configure:prepend() {
    cp ${WORKDIR}/defconfig ${WORKDIR}/build/.config
}

do_configure:append() {
    cp ${WORKDIR}/defconfig ${WORKDIR}/build/.config
}

