FILESEXTRAPATHS:prepend := "${THISDIR}/patches:"

SRC_URI += " \
	file://stm32mp157a-nous-optee.dts \
	file://stm32mp157d-thor-optee.dts \
	file://defconfig \
	file://conf.mk.patch;patch=1 \
"

PACKAGE_ARCH = "${MACHINE_ARCH}"

do_configure:prepend() {
    cp ${WORKDIR}/defconfig ${WORKDIR}/build/.config
}

do_configure:append() {
    cp ${WORKDIR}/stm32mp157a-nous-optee.dts ${TMPDIR}/work-shared/stm32mp1-thor-e2/external-dt/stm32mp1/optee/stm32mp157a-nous.dts
    cp ${WORKDIR}/stm32mp157d-thor-optee.dts ${TMPDIR}/work-shared/stm32mp1-thor-e2/external-dt/stm32mp1/optee/stm32mp157d-thor.dts

    cp ${WORKDIR}/defconfig ${WORKDIR}/build/.config
}

