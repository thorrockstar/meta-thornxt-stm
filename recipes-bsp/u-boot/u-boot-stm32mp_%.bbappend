FILESEXTRAPATHS:prepend := "${THISDIR}/patches:"

SRC_URI += " \
	file://Kconfig.patch;patch=1 \
	file://stm32mp15_defconfig.patch;patch=1 \
	file://stm32mp157f-ev1.patch;patch=1 \
	file://stm32mp157d-ev1.patch;patch=1 \
	file://stm32mp157a-ev1_nous_mk2.patch;patch=1 \
	file://stm32mp15-ddr3-2x4Gb-1066-binG.patch;patch=1 \
	file://stm32mp157a-ev1-u-boot.patch;patch=1 \
	file://stm32mp157a-ed1-u-boot.patch;patch=1 \
	file://stm32mp1.patch;patch=1 \
	file://stm32mp1_h.patch;patch=1 \
	file://common.patch;patch=1 \
	file://cpu.patch;patch=1 \
	file://lcd.patch;patch=1 \
	file://pxe_utils.patch;patch=1 \
	file://build.patch;patch=1 \
	file://dfu.patch;patch=1 \
	file://stm32prog.patch;patch=1 \
	file://autoboot.patch;patch=1 \
	file://fastmap.patch;patch=1 \
"

PACKAGE_ARCH = "${MACHINE_ARCH}"

