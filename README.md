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

8/ Add meta-thornxt-stm layer to bblayer configuration file.  
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

9/ Edit local.conf to specify the machine, location of source archived, package type (rpm, deb or ipk)
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

10/ Remove some unwanted recipies from the ST folders.

* meta-st-openstlinux/meta-st-openstlinux/recipes-multimedia/gstreamer
* meta-st-openstlinux/meta-st-openstlinux/recipes-webadmin
* meta-st-openstlinux/meta-st-openstlinux/recipes-samples
* meta-st-openstlinux/meta-st-openstlinux/recipes-qt

11/ Remove these lines from the meta-st-stm32mp/receipes-st/images/st-image-userfs.bb file.

    <meta-st-stm32mp/receipes-st/images/st-image-userfs.bb>

    ** Remove these lines! **

    PACKAGE_INSTALL += " \
        packagegroup-st-demo \
        "

12/ Remove some lines from the meta-st-openstlinux/conf/layer.conf file.

    <meta-st-openstlinux/conf/layer.conf>

    ** Remove these lines! **

    LAYERDEPENDS_st-openstlinux = "qt5-layer"

**IMPORTANT**

13/ Double check that in the kernel configuration **'General Setup->Timers subsystem->High Resolution Timer Support'**
has been turned **off** as well as **'General Setup->Timers subsystem->Timer tick handling'** is set to **'Periodic timer ticks'**.
This should be done by the 'defconfig' but double check before building because it is cruicial.

14/ Build Thor image
    bitbake thor-e-image

Typical bitbake output
======================
    Loading cache: 100% |###########################################################################################| Time: 0:00:00
    Loaded 3294 entries from dependency cache.
    Parsing recipes: 100% |#########################################################################################| Time: 0:00:00
    Parsing of 2162 .bb files complete (2161 cached, 1 parsed). 3283 targets, 127 skipped, 0 masked, 0 errors.
    NOTE: Resolving any missing task queue dependencies

    Build Configuration:
    BB_VERSION           = "1.46.0"
    BUILD_SYS            = "x86_64-linux"
    NATIVELSBSTRING      = "universal"
    TARGET_SYS           = "arm-poky-linux-gnueabi"
    MACHINE              = "stm32mp1-thor-e2"
    DISTRO               = "poky"
    DISTRO_VERSION       = "3.1.18"
    TUNE_FEATURES        = "arm vfp cortexa7 neon vfpv4 thumb callconvention-hard"
    TARGET_FPU           = "hard"
    meta                 
    meta-poky            
    meta-yocto-bsp       = "dunfell:d695bd0d3dc66f2111a25c6922f617be2d991071"
    meta-st-stm32mp      = "dunfell:9beb22b032d01c98aef956b43f5ed2712078a024"
    meta-st-openstlinux  = "dunfell:ed4cbda9145779f757d92d158ae612238d1c7078"
    meta-thornxt-stm     = "dunfell:xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
    meta-oe              
    meta-networking      
    meta-python          = "dunfell:52cee67833d1975a5bd52e4556c4cd312425a017"

    Initialising tasks: 100%     |#############################################################################################| Time: 0:00:01
    Sstate summary: Wanted 8 Found 0 Missed 8 Current 1949 (0% match, 99% complete)
    NOTE: Executing Tasks
    NOTE: Tasks Summary: Attempted 4931 tasks of which 4920 didn't need to be rerun and all succeeded.


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
