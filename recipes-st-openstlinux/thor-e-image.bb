SUMMARY = "OpenSTLinux core image for the THOR-E2 boards, based on the STM32MP157 MPU."
LICENSE = "Proprietary"

include recipes-st-openstlinux/thor-e.inc

inherit core-image

IMAGE_LINGUAS = "en-us"

#
# INSTALL addons
#
CORE_IMAGE_EXTRA_INSTALL += " \
    resize-helper \
    \
    packagegroup-framework-core-base    \
    packagegroup-framework-tools-base   \
    \
    ${@bb.utils.contains('COMBINED_FEATURES', 'optee', 'packagegroup-optee-core', '', d)}   \
    ${@bb.utils.contains('COMBINED_FEATURES', 'optee', 'packagegroup-optee-test', '', d)}   \
    "

# remove unwanted components from image
IMAGE_INSTALL_remove = "\
    mpg123 \
    bash \
    bash-completion \
    nodejs \
    coreutils \
    lmbench \
    hcitool \
    dtc \
    dtc-misc \
    python \
    python2 \
    python3 \
    python-core \
    python2-core \
    python3-core \
    python-pyserial \
    python-smbus \
    python-ctypes \
    python-pip \
    python-distribute \
    python-pycurl \	
    python-native \	
    linux-firmware \
    usbutils \
    evtest \
    libdrm-tests \
    9bit \
    rng-tools \
    bluez4 \
    bluez5 \
    wget \
    phytool \
    dhcp-server \
    dhcp-server-config \
    gdb \
    btmon \
    tcpdump \
    cracklib \
    sqlite3 \
    nfc \
    zeroconf \
    "

# add important components to image
IMAGE_INSTALL_append = "\
    glibc \
    glib-2.0 \
    gnutls \
    zeromq \
    dbus \
    gdbserver \
    busybox \
    ca-certificates \
    openssh \
    openssh-sshd \
    "

CUSTOMFILESPATH_EXTRA := "${THISDIR}/images/files"

ROOTFS_POSTPROCESS_COMMAND += " fix_udev_files ; fix_usr_files ; fix_firmware_files ; "

fix_udev_files () {
    rm -f $D${sysconfdir}/udev/hwdb.bin
    rm -fr $D${sysconfdir}/udev/hwdb.d
    rm -fr $D${sysconfdir}/rc3.d/S03rng-tools
    rm -fr $D${sysconfdir}/rc3.d/S21avahi-daemon
    rm -fr $D${sysconfdir}/rc3.d/S22ofono
    rm -fr $D${sysconfdir}/rc3.d/S39alsa-state
    rm -fr $D${sysconfdir}/rc3.d/S64neard
    rm -fr $D${sysconfdir}/rc5.d/S03rng-tools
    rm -fr $D${sysconfdir}/rc5.d/S21avahi-daemon
    rm -fr $D${sysconfdir}/rc5.d/S22ofono
    rm -fr $D${sysconfdir}/rc5.d/S39alsa-state
    rm -fr $D${sysconfdir}/rc5.d/S64neard
    rm -fr $D${sysconfdir}/fw_env.config.mmc
    rm -fr $D${sysconfdir}/fw_env.config.nor
}

fix_usr_files () {
    rm -fr $D/usr/games
    rm -fr $D/usr/bin/kermit
    rm -fr $D/usr/bin/strace
    rm -fr $D/usr/bin/btmon
    rm -fr $D/usr/bin/bluetoothctl
    rm -fr $D/usr/bin/bccmd
    rm -fr $D/usr/bin/hcidump
    rm -fr $D/usr/bin/v4l2-ctl
    rm -fr $D/usr/bin/v4l2-compliance
    rm -fr $D/usr/sbin/ofonod
    rm -fr $D/usr/sbin/wpa_supplicant
    rm -fr $D/usr/share/mobile-broadband-provider-info
    rm -fr $D/usr/share/mime
    rm -fr $D/usr/share/alsa/ucm2
    rm -fr $D/usr/share/pci.ids.gz
    rm -fr $D/usr/libexec/bluetooth
    rm -fr $D/usr/libexec/nfc
}

fix_firmware_files () {
    rm -fr $D/usr/lib/firmware/*
    rm -fr $D/usr/share/sounds/alsa/*.wav
}


