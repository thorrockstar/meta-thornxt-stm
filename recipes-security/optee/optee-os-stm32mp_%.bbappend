FILESEXTRAPATHS:prepend := "${THISDIR}/patches:"

SRC_URI += " \
	file://stm32mp157a-nous.patch \
	file://stm32mp157d-thor.patch \
	file://conf.mk.patch \
	file://plat_conf.mk.patch \
"

PACKAGE_ARCH = "${MACHINE_ARCH}"

