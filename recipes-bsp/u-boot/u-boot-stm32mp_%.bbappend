FILESEXTRAPATHS:prepend := "${THISDIR}/patches:"

SRC_URI += " \
	file://stm32mp157d-thor.dts.patch \
	file://stm32mp157d-thor-u-boot.dtsi.patch \
	file://stm32mp157d-thor-scmi.dtsi.patch \
	file://stm32mp157a-nous.dts.patch \
	file://stm32mp157a-nous-u-boot.dtsi.patch \
	file://stm32mp157a-nous-scmi.dtsi.patch \
	file://stm32mp15_defconfig.patch \
	file://stm32mp1.patch \
	file://stm32mp1_h.patch \
	file://common.patch \
	file://soc.patch \
	file://build.patch \
	file://dfu.patch \
	file://stm32prog.patch \
	file://autoboot.patch \
	file://fastmap.patch \
	file://Makefile.patch \
"
# file://pxe_utils.patch

