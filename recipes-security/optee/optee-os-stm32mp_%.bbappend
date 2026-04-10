FILESEXTRAPATHS:prepend := "${THISDIR}/patches:"

SRC_URI += " \
	file://stm32mp157a-nous.patch;patch=1 \
	file://stm32mp157d-thor.patch;patch=1 \
	file://conf.mk.patch;patch=1 \
	file://plat_conf.mk.patch;patch=1 \
"

PACKAGE_ARCH = "${MACHINE_ARCH}"

