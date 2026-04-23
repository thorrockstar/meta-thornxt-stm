FILESEXTRAPATHS:prepend := "${THISDIR}/patches:"

SRC_URI += " \
	file://stm32mp157x-nous.patch \
	file://stm32mp157x-thor.patch \
	file://conf.mk.patch \
	file://plat_conf.mk.patch \
	file://boot.c.patch \
"

