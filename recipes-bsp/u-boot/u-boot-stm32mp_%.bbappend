FILESEXTRAPATHS_prepend := "${THISDIR}/patches:"

SRC_URI += " \
	file://stm32mp157f-ev1.patch;patch=1 \
	file://stm32mp1.patch;patch=1 \
	file://stm32mp15_trusted_defconfig.patch;patch=1 \
	file://stm32mp15-ddr3-2x4Gb-1066-binG.patch;patch=1 \
	file://cpu.patch;patch=1 \
	file://common.patch;patch=1 \
	file://lcd.patch;patch=1 \
	file://pxe_utils.patch;patch=1 \
	file://stm32mp157a-ev1-u-boot.patch;patch=1 \
	file://stm32mp157a-ed1-u-boot.patch;patch=1 \
	file://build.patch;patch=1 \
	file://dfu.patch;patch=1 \
"

PACKAGE_ARCH = "${MACHINE_ARCH}"

