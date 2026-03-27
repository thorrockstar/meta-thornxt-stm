FILESEXTRAPATHS:prepend := "${THISDIR}/patches:"

SRC_URI += " \
	file://stm32mp15_defconfig.patch;patch=1 \
	file://stm32mp15-ddr3-2x4Gb-1066-binG.patch;patch=1 \
	file://stm32mp1.patch;patch=1 \
	file://stm32mp1_h.patch;patch=1 \
	file://common.patch;patch=1 \
	file://soc.patch;patch=1 \
	file://build.patch;patch=1 \
	file://dfu.patch;patch=1 \
	file://stm32prog.patch;patch=1 \
	file://autoboot.patch;patch=1 \
	file://fastmap.patch;patch=1 \
	file://stm32mp157d-thor.patch;patch=1 \
	file://stm32mp157d-thor-u-boot.patch;patch=1 \
	file://stm32mp157a-nous.patch;patch=1 \
	file://stm32mp157a-nous-u-boot.patch;patch=1 \
	file://Makefile.patch;patch=1 \
"

PACKAGE_ARCH = "${MACHINE_ARCH}"

