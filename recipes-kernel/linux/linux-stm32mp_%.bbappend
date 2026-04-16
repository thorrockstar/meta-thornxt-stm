FILESEXTRAPATHS:prepend := "${THISDIR}/patches:"

KERNEL_CONFIG_FRAGMENTS:append = "${WORKDIR}/fragments/${LINUX_VERSION}/fragment-custom.config"

SRC_URI:append = " file://${LINUX_VERSION}/fragment-custom.config;subdir=fragments "

SRC_URI += " \
	file://stm32mp157a-nous.dts.patch \
	file://stm32mp157a-nous-scmi.dtsi.patch \
	file://stm32mp157d-thor.dts.patch \
	file://stm32mp157d-thor-scmi.dtsi.patch \
	file://stm32_sai_sub.patch \
	file://drm_fb_helper.patch \
	file://spidev.patch \
	file://defconfig \
	file://pinctrl-stm32mp157.patch \
	file://Kconfig.patch \
	file://pinctrl-stm32.patch \
"

PACKAGE_ARCH = "${MACHINE_ARCH}"

KERNEL_MODULE_AUTOLOAD += "cdc-acm"

do_configure:prepend() {
    cp ${WORKDIR}/defconfig ${WORKDIR}/build/.config
}

do_configure:append() {
    cp ${WORKDIR}/defconfig ${WORKDIR}/build/.config
}

