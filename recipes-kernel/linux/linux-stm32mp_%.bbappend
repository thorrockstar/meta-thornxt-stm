FILESEXTRAPATHS:prepend := "${THISDIR}/patches:"

SRC_URI += " \
	file://stm32mp157a-nous.dts \
	file://stm32mp157a-nous-scmi.dtsi \
	file://stm32mp157d-thor.dts \
	file://stm32mp157d-thor-scmi.dtsi \
	file://defconfig \
	file://stm32_sai_sub.patch;patch=1 \
	file://drm_fb_helper.patch;patch=1 \
	file://spidev.patch;patch=1 \
	file://panel-simple.patch;patch=1 \
	file://pinctrl-stm32mp157.patch;patch=1 \
	file://Kconfig.patch;patch=1 \
"

PACKAGE_ARCH = "${MACHINE_ARCH}"

KERNEL_MODULE_AUTOLOAD += "cdc-acm"

do_configure:prepend() {
    cp ${WORKDIR}/defconfig ${WORKDIR}/build/.config
}

do_configure:append() {
    cp ${WORKDIR}/stm32mp157a-nous.dts ${TMPDIR}/work-shared/stm32mp1-thor-e2/kernel-source/arch/arm/boot/dts/st/stm32mp157a-nous.dts
    cp ${WORKDIR}/stm32mp157a-nous-scmi.dtsi ${TMPDIR}/work-shared/stm32mp1-thor-e2/kernel-source/arch/arm/boot/dts/st/stm32mp157a-nous-scmi.dtsi
    cp ${WORKDIR}/stm32mp157d-thor.dts ${TMPDIR}/work-shared/stm32mp1-thor-e2/kernel-source/arch/arm/boot/dts/st/stm32mp157d-thor.dts
    cp ${WORKDIR}/stm32mp157d-thor-scmi.dtsi ${TMPDIR}/work-shared/stm32mp1-thor-e2/kernel-source/arch/arm/boot/dts/st/stm32mp157d-thor-scmi.dtsi

    cp ${WORKDIR}/defconfig ${WORKDIR}/build/.config
}

