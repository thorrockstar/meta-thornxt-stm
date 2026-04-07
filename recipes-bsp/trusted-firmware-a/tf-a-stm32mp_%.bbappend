FILESEXTRAPATHS:prepend := "${THISDIR}/patches:"

SRC_URI += " \
	file://stm32mp157d-thor.patch;patch=1 \
	file://stm32mp157d-thor-fw-config.patch;patch=1 \
	file://stm32mp157a-nous.patch;patch=1 \
	file://stm32mp157a-nous-fw-config.patch;patch=1 \
	file://stm32mp_pmic.patch;patch=1 \
	file://bl2_plat_setup.patch;patch=1 \
	file://stm32mp_def.patch;patch=1 \
"

PACKAGE_ARCH = "${MACHINE_ARCH}"

