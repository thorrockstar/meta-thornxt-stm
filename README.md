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

Build has been tested under Ubuntu 24.04 LTS. Anyway you need to install these required packages:

    sudo apt install build-essential chrpath cpio debianutils diffstat file gawk gcc git iputils-ping libacl1 liblz4-tool locales python3 python3-git python3-jinja2 python3-pexpect python3-pip python3-subunit socat texinfo unzip wget xz-utils zstd

    sudo locale-gen en_US.UTF-8

    sudo apt install git librsvg2-bin locales make python3-saneyaml python3-sphinx-rtd-theme sphinx



Build procedure
===============  

0/ Create a directory.  

    mkdir scarthgap_stm
    cd scarthgap_stm

1/ Clone yocto/poky git repository with the proper branch ready.  

    git clone https://git.yoctoproject.org/poky -b scarthgap

2/ Clone meta-openembedded git repository with the proper branch ready.  

    git clone https://git.openembedded.org/meta-openembedded -b scarthgap

3/ Clone meta-st-stm32mp layer with the proper branch ready.  

    git clone https://github.com/STMicroelectronics/meta-st-stm32mp.git -b scarthgap

4/ Clone meta-st-openstlinux layer with the proper branch ready.  

    git clone https://github.com/STMicroelectronics/meta-st-openstlinux.git -b scarthgap

5/ Clone meta-thornxt-stm layer with the proper branch ready.  

    git clone https://github.com/thorrockstar/meta-thornxt-stm.git -b scarthgap

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

9/ Edit local.conf by appending the block below at the very end of the file.

**Make sure that you have no white spaces left to "MACHINE ??=" and the other variables when editing the text block.**

    gnome-text-editor conf/local.conf

```
MACHINE = "stm32mp1-thor-e2"  

DISTRO = "thor-stm"  

ACCEPT_EULA_$MACHINE = "1"  

INIT_MANAGER = "sysvinit"  

ENABLE_BINARY_LOCALE_GENERATION = "1"  

GLIBC_SPLIT_LC_PACKAGES = "0"  

GLIBC_GENERATE_LOCALES += "en_US.UTF-8"  

IMAGE_LINGUAS += "en-us"  
```

10/ Build Thor image

    bitbake thor-e-image

Typical bitbake output
======================
    Build Configuration:
    BB_VERSION           = "2.8.1"
    BUILD_SYS            = "x86_64-linux"
    NATIVELSBSTRING      = "ubuntu-24.04"
    TARGET_SYS           = "arm-thor-linux-gnueabi"
    MACHINE              = "stm32mp1-thor-e2"
    DISTRO               = "thor-stm"
    DISTRO_VERSION       = "5.0.16"
    TUNE_FEATURES        = "arm vfp cortexa7 neon vfpv4 thumb callconvention-hard"
    TARGET_FPU           = "hard"
    meta                 
    meta-poky            
    meta-yocto-bsp       = "scarthgap:b301218f4da7793624db3545efd8bf66888b77b0"
    meta-st-stm32mp      = "scarthgap:701c0ddb5afa29842c4146773d5303a1f192ff19"
    meta-st-openstlinux  = "scarthgap:993e43adedbfe95dca8e93a64ec091a83a604633"
    meta-thornxt-stm     = "scarthgap:xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
    meta-oe              
    meta-networking      
    meta-python          = "scarthgap:4d3e2639dec542b58708244662d5ce36810fc510"


Install the Signing Tool for Production
=======================================

To install the signing tool, download STMCubeProgrammer from the ST web site.
You will need a log-in on their web page. Replace the "x.xx.x" with the actual
version number given in the file name.

    https://www.st.com/en/development-tools/stm32cubeprog.html

    unzip SetupSTM32CubeProgrammer_linux_64.zip

    chmod +x SetupSTM32CubeProgrammer-x.xx.x.linux

    ./SetupSTM32CubeProgrammer-x.xx.x.linux

Signing the Image for Production
================================

In order to sign your image, you need to:

1.) Use your companies private/public key pair and copy them into the "conf/machine/signing" folder.   
2.) Download the STMCubeProgrammer - see above.   
3.) Enable the signing procedure in the machine configuration file "conf/machine/stm32mp1-thor-e2-conf" at the very end of the file - see below.   

### Edit your machine config file

    gnome-text-editor conf/machine/stm32mp1-thor-e2-conf

##### Activate the signing of TF-A/FIP at the end of the file
    SIGN_ENABLE = "1"
    SIGN_TOOL = "absolute path to STM32CubeProgrammer/bin/STM32_SigningTool_CLI"

##### Version for STMP32MP15X
    SIGN_HEADER_VERSION_stm32mp15 = "1.0"

##### Path to the private key (PEM-Format)
    SIGN_KEY = "conf/machine/signing/privateKey.pem"

##### Path to the public key (PEM-Format)
    SIGN_KEY_PUB_stm32mp15 = "conf/machine/signing/publicKey.pem"

##### Password for the private key
    SIGN_KEY_PASS = "your password"



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
