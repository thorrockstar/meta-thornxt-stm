This layer provides support for the Thor-NX-E2 platform reference boards
========================================================================

For more information about the Thor-NX-E2 product line see:
https://www.thor.engineering

NeXt-Group - Open projects for lifts
https://www.next-group.org

STM32 MPU OpenSTLinux Distribution:
https://www.st.com/en/embedded-software/stm32-mpu-openstlinux-distribution.html


Supported SoCs / MACHINE names
==============================
- STM32MP157 based THOR-E2 and Nous-X lift controller boards


Sources
=======
- meta-thornxt-stm
URI: https://github.com/thorrockstar/meta-thornxt-stm.git


Dependencies
============
This Layer depends on:

- meta-openembedded
URI: git://git.openembedded.org/meta-openembedded
URI: http://cgit.openembedded.org/meta-openembedded/

- meta-st-stm32mp layer
URI: https://github.com/STMicroelectronics/meta-st-stm32mp.git

- meta-st-openstlinux
URI: https://github.com/STMicroelectronics/meta-st-openstlinux.git


Requisites
==========

Build has been tested under Ubuntu 22.04 LTS. Anyway you need to install these required packages:

    $ sudo apt install gawk wget git diffstat unzip texinfo gcc build-essential chrpath socat cpio python3 python3-pip python3-pexpect xz-utils debianutils iputils-ping python3-git python3-jinja2 libegl1-mesa libsdl1.2-dev python3-subunit mesa-common-dev zstd liblz4-tool file locales libacl1
    $ sudo locale-gen en_US.UTF-8

    $ sudo apt install make python3-pip inkscape texlive-latex-extra
    $ sudo pip3 install sphinx sphinx_rtd_theme pyyaml


Build procedure
===============

0/ Create a directory.  

    mkdir kirkstone_stm
    cd kirkstone_stm

1/ Clone yocto/poky git repository with the proper branch ready.  

    git clone git://git.yoctoproject.org/poky -b kirkstone

2/ Clone meta-openembedded git repository with the proper branch ready.  

    git clone git://git.openembedded.org/meta-openembedded -b kirkstone

3/ Clone meta-st-stm32mp layer with the proper branch ready.  

    git clone https://github.com/STMicroelectronics/meta-st-stm32mp.git -b kirkstone

4/ Clone meta-st-openstlinux layer with the proper branch ready.  

    git clone https://github.com/STMicroelectronics/meta-st-openstlinux.git -b kirkstone

5/ Clone meta-thornxt-stm layer with the proper branch ready.  

    git clone https://github.com/thorrockstar/meta-thornxt-stm.git -b kirkstone

6/ Enter the poky directory to configure the build system and start the build process.  

    cd poky

7/ Initialize build directory and set compiler.  

    source oe-init-build-env

8/ Add meta-thornxt-stm layer to bblayer configuration file.

    gnome-text-editor conf/bblayers.conf

```
# POKY_BBLAYERS_CONF_VERSION is increased each time build/conf/bblayers.conf
# changes incompatibly
POKY_BBLAYERS_CONF_VERSION = "2"

BBPATH = "${TOPDIR}"
BBFILES ?= ""

BSPDIR := "${@os.path.abspath(os.path.dirname(d.getVar('FILE', True)) + '/../../..')}"

BBLAYERS ?= " \
  ${BSPDIR}/poky/meta \
  ${BSPDIR}/poky/meta-poky \
  ${BSPDIR}/poky/meta-yocto-bsp \
  ${BSPDIR}/meta-st-stm32mp \
  ${BSPDIR}/meta-st-openstlinux \
  ${BSPDIR}/meta-thornxt-stm \
  ${BSPDIR}/meta-openembedded/meta-oe \
  ${BSPDIR}/meta-openembedded/meta-networking \
  ${BSPDIR}/meta-openembedded/meta-python \
"

BLAYERS_NON_REMOVABLE ?= " \
  ${BSPDIR}/poky/meta \
  ${BSPDIR}/poky/meta-poky \
"
```

9/ Edit local.conf to specify the machine, location of source archived, package type (rpm, deb or ipk)
Pick one MACHINE name from the "Supported SoCs / MACHINE names" chapter above
and edit the "local.conf" file. Here is an example:  

**Make sure that you have no white spaces left to "MACHINE ??=" and the other variables when editing the text block.**

    gnome-text-editor conf/local.conf

```
MACHINE ??= "stm32mp1-thor-e2"  

PACKAGE_CLASSES ?= "package_rpm"  

USER_CLASSES ?= "buildstats"  

DISTRO ?= "thor-stm"  

ACCEPT_EULA_$MACHINE = "1"  

INIT_MANAGER = "sysvinit"  

ENABLE_BINARY_LOCALE_GENERATION = "1"  

GLIBC_SPLIT_LC_PACKAGES = "0"  

GLIBC_GENERATE_LOCALES += "en_US.UTF-8"  

IMAGE_LINGUAS += "en-us"  
```

10/ Remove some unwanted recipies from the ST folders.

**meta-st-openstlinux**

* meta-st-openstlinux/recipes-multimedia/gstreamer
* meta-st-openstlinux/recipes-webadmin
* meta-st-openstlinux/recipes-samples
* meta-st-openstlinux/recipes-qt
* meta-st-openstlinux/oe-core/recipes-gnome
* meta-st-openstlinux/oe-core/recipes-core/systemd

11/ Remove these lines from the meta-st-stm32mp/receipes-st/images/st-image-userfs.bb file.

    <meta-st-stm32mp/recipes-st/images/st-image-userfs.bb>

    ** Remove these lines! **

    PACKAGE_INSTALL += " \
        packagegroup-st-demo \
        "

12/ Remove some lines from the meta-st-openstlinux/conf/layer.conf file.

    <meta-st-openstlinux/conf/layer.conf>

    ** Remove this line! **

    LAYERDEPENDS_st-openstlinux = "qt5-layer"


**VERY IMPORTANT**

13/ There is an issue with patching the file "st-machine-extlinux-config-stm32mp.inc" inside the ST-Layer at "meta-st-stm32mp/conf/machine/include".

    So please copy the file "st-machine-extlinux-config-stm32mp.inc"
    from the THOR-layer at "meta-thornxt-stm/conf/machine/include"
    to the STM-layer at "meta-st-stm32mp/conf/machine/include"
    and overwrite the original file.

**IMPORTANT**

14/ Double check that in the kernel configuration **'General Setup->Timers subsystem->Timer tick handling'** is set to **'Periodic timer ticks'**. This should be done by the 'defconfig' but double check before building because it is cruicial.

15/ Build Thor image

    bitbake thor-e-image

Typical bitbake output
======================
    Build Configuration:
    BB_VERSION           = "2.0.0"
    BUILD_SYS            = "x86_64-linux"
    NATIVELSBSTRING      = "universal"
    TARGET_SYS           = "arm-thor-linux-gnueabi"
    MACHINE              = "stm32mp1-thor-e2"
    DISTRO               = "thor-stm"
    DISTRO_VERSION       = "4.0.13"
    TUNE_FEATURES        = "arm vfp cortexa7 neon vfpv4 thumb callconvention-hard"
    TARGET_FPU           = "hard"
    meta                 
    meta-poky            
    meta-yocto-bsp       = "kirkstone:02c15addea7c3303552df5994ce0616b2059388c"
    meta-st-stm32mp      = "kirkstone:fcdd9c0100ccb1317bf4c6904a1c90692891fa6d"
    meta-st-openstlinux  = "kirkstone:7f5d2a17b52f968da5dd73f627f1634223bd427d"
    meta-thornxt-stm     = "kirkstone:xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
    meta-oe              
    meta-networking      
    meta-python          = "kirkstone:8a598a2bc9199a4fbb9008a32ab143fb509a0933"

Contributing
============
To contribute to this layer you should submit the patches for review to:
the github pull-request facility directly. Anyway, don't forget to
Cc the maintainers.

STM32MPU Wiki & Forum:
https://wiki.st.com/stm32mpu/wiki/OpenSTLinux_distribution

for some useful guidelines to be followed when submitting patches:
http://www.openembedded.org/wiki/How_to_submit_a_patch_to_OpenEmbedded

Maintainers:
Roy Schneider <roy@thor.engineering>

When creating patches insert the [meta-thornxt-stm] tag in the subject, for example
use something like:
git format-patch -s --subject-prefix='meta-thornxt-stm][PATCH' <origin>
