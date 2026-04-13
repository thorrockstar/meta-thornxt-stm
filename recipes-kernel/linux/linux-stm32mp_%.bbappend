FILESEXTRAPATHS:prepend := "${THISDIR}/patches:"

SRC_URI += " \
	file://stm32mp157a-nous.dts.patch \
	file://stm32mp157a-nous-scmi.dtsi.patch \
	file://stm32mp157d-thor.dts.patch \
	file://stm32mp157d-thor-scmi.dtsi.patch \
	file://defconfig \
	file://stm32_sai_sub.patch \
	file://drm_fb_helper.patch \
	file://spidev.patch \
	file://panel-simple.patch \
	file://pinctrl-stm32mp157.patch \
	file://Kconfig.patch \
"

PACKAGE_ARCH = "${MACHINE_ARCH}"

KERNEL_MODULE_AUTOLOAD += "cdc-acm"

do_configure:prepend() {
    cp ${WORKDIR}/defconfig ${WORKDIR}/build/.config
}

do_configure:append() {
    cp ${WORKDIR}/defconfig ${WORKDIR}/build/.config
}

