
# remove unwanted packages from image
PACKAGE_INSTALL:remove = "\
    packagegroup-st-demo \
    packagegroup-core-full-cmdline \
    packagegroup-base-bluetooth \
    packagegroup-base-usbgadget \
    packagegroup-base-wifi \
    packagegroup-base-extended \
    linux-examples-stm32mp1-userfs \
    m4projects-stm32mp1-userfs \
"

CUSTOMFILESPATH_EXTRA := "${THISDIR}/files"


