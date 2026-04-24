FILESEXTRAPATHS:prepend := "${THISDIR}/patches:"

SRC_URI += " \
	file://stm32mp157x-thor.dts.patch \
	file://stm32mp157x-thor-u-boot.dtsi.patch \
	file://stm32mp157x-thor-scmi.dtsi.patch \
	file://stm32mp157x-nous.dts.patch \
	file://stm32mp157x-nous-u-boot.dtsi.patch \
	file://stm32mp157x-nous-scmi.dtsi.patch \
	file://stm32mp15_defconfig.patch \
	file://stm32mp1.patch \
	file://stm32mp1_h.patch \
	file://stm32mp15_st_common.h.patch \
	file://common.patch \
	file://soc.patch \
	file://build.patch \
	file://dfu.patch \
	file://stm32prog.patch \
	file://autoboot.patch \
	file://pxe_utils.patch \
	file://fastmap.patch \
	file://Makefile.patch \
"

