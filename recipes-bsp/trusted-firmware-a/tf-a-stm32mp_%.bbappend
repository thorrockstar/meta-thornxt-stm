FILESEXTRAPATHS:prepend := "${THISDIR}/patches:"

SRC_URI += " \
	file://stm32mp157d-thor.dts \
	file://stm32mp157d-thor-fw-config.dts \
	file://stm32mp157a-nous.dts \
	file://stm32mp157a-nous-fw-config.dts \
	file://stm32mp_def.patch;patch=1 \
"

PACKAGE_ARCH = "${MACHINE_ARCH}"

do_configure:append() {
    cp ${WORKDIR}/stm32mp157d-thor.dts ${TMPDIR}/work-shared/stm32mp1-thor-e2/tfa-source/fdts/stm32mp157d-thor.dts
    cp ${WORKDIR}/stm32mp157d-thor-fw-config.dts ${TMPDIR}/work-shared/stm32mp1-thor-e2/tfa-source/fdts/stm32mp157d-thor-fw-config.dts
    cp ${WORKDIR}/stm32mp157a-nous.dts ${TMPDIR}/work-shared/stm32mp1-thor-e2/tfa-source/fdts/stm32mp157a-nous.dts
    cp ${WORKDIR}/stm32mp157a-nous-fw-config.dts ${TMPDIR}/work-shared/stm32mp1-thor-e2/tfa-source/fdts/stm32mp157a-nous-fw-config.dts
}

