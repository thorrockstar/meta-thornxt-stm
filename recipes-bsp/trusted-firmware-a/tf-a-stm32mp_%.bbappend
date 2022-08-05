FILESEXTRAPATHS_prepend := "${THISDIR}/patches:"

SRC_URI += " \
	file://stm32mp157f-ev1.patch;patch=1 \
	file://stm32mp157f-ev1-fw-config.patch;patch=1 \
	file://bl2_plat_setup.patch;patch=1 \
	file://stm32mp1_clk.patch;patch=1 \
	file://stm32mp1_ddr.patch;patch=1 \
	file://stm32mp_pmic.patch;patch=1 \
"

PACKAGE_ARCH = "${MACHINE_ARCH}"

