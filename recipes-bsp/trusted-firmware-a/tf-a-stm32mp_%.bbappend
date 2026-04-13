FILESEXTRAPATHS:prepend := "${THISDIR}/patches:"

SRC_URI += " \
	file://stm32mp1_thor_ddr_1x4Gb.dtsi.patch \
	file://stm32mp157d-thor.dts.patch \
	file://stm32mp157d-thor-fw-config.dts.patch \
	file://stm32mp157a-nous.dts.patch \
	file://stm32mp157a-nous-fw-config.dts.patch \
	file://bl2_plat_setup.patch \
	file://stm32mp_def.patch \
	file://stm32mp_pmic.patch \
"

