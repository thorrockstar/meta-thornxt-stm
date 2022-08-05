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
- STM32MP157 based THOR-NX-E2 lift controller boards


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

Build procedure
===============

0/ Create a directory.  
    mkdir poky  
    cd poky

1/ Clone yocto/poky git repository with the proper branch ready.  
    git clone git://git.yoctoproject.org/poky -b dunfell

2/ Clone meta-openembedded git repository with the proper branch ready.  
    git clone git://git.openembedded.org/meta-openembedded -b dunfell

3/ Clone meta-st-stm32mp layer with the proper branch ready.  
    git clone https://github.com/STMicroelectronics/meta-st-stm32mp.git -b dunfell

4/ Clone meta-st-openstlinux layer with the proper branch ready.  
    git clone https://github.com/STMicroelectronics/meta-st-openstlinux.git -b dunfell

5/ Clone meta-thornxt-stm layer with the proper branch ready.  
    git clone https://github.com/thorrockstar/meta-thornxt-stm.git -b dunfell

6/ Enter the poky directory to configure the build system and start the build process.  
   cd poky

7/ Initialize build directory and set compiler.  
    source oe-init-build-env

7/ Add meta-thore2 layer to bblayer configuration file.  
    vi conf/bblayers.conf

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

8/ Edit local.conf to specify the machine, location of source archived, package type (rpm, deb or ipk)
Pick one MACHINE name from the "Supported SoCs / MACHINE names" chapter above
and edit the "local.conf" file. Here is an example:  
    vi conf/local.conf

    [...]
    MACHINE ??= "stm32mp1-thor-e2"
    [...]
    PACKAGE_CLASSES ?= "package_rpm"
    [...]
    USER_CLASSES ?= "buildstats image-mklibs"
    [...]
    DISTRO ?= "poky"
    [...]
    ACCEPT_EULA_$MACHINE = "1"

9/ Remove some unwanted recipies from the ST folders.

* meta-st-openstlinux/meta-st-openstlinux/recipes-multimedia/gstreamer
* meta-st-openstlinux/meta-st-openstlinux/recipes-webadmin
* meta-st-openstlinux/meta-st-openstlinux/recipes-samples
* meta-st-openstlinux/meta-st-openstlinux/recipes-qt

10/ Remove these lines from the meta-st-stm32mp/receipes-st/images/st-image-userfs.bb file.

    <meta-st-stm32mp/receipes-st/images/st-image-userfs.bb>
    
    ** Remove **

    PACKAGE_INSTALL += " \
        packagegroup-st-demo \
        "

11/ Remove some lines from the meta-st-openstlinux/conf/layer.conf file.

    <meta-st-openstlinux/conf/layer.conf>

    ** Remove **

    LAYERDEPENDS_st-openstlinux = "qt5-layer"

**IMPORTANT**

12/ Double check that in the kernel configuration **'General Setup->Timers subsystem->High Resolution Timer Support'**
has been turned **off** as well as **'General Setup->Timers subsystem->Timer tick handling'** is set to **'Periodic timer ticks'**.
This should be done by the 'defconfig' but double check before building because it is cruicial.

13/ Build Thor image
    bitbake thor-e-image

Typical bitbake output
======================
Build Configuration:
BB_VERSION           = "1.46.0"
BUILD_SYS            = "x86_64-linux"
NATIVELSBSTRING      = "universal"
TARGET_SYS           = "arm-poky-linux-gnueabi"
MACHINE              = "stm32mp1-thor-e2"
DISTRO               = "poky"
DISTRO_VERSION       = "3.1.15"
TUNE_FEATURES        = "arm vfp cortexa7 neon vfpv4 thumb callconvention-hard"
TARGET_FPU           = "hard"
meta                 
meta-poky            
meta-yocto-bsp       = "dunfell:d084cd438897122d0a30c35eabc2b669b8ca3f36"
meta-st-stm32mp      = "dunfell:58f52ac42620c3735b30dc777cd115ad466a62bd"
meta-st-openstlinux  = "dunfell:5ceaaab7adbf31e0d99ae4de55b8c81b067619bf"
meta-thornxt-stm     = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
meta-oe              
meta-networking      
meta-python          = "dunfell:86b864a4d8c28185a4a464583fb86f73aa22847a"

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
git format-patch -s --subject-prefix='meta-thore2][PATCH' <origin>
