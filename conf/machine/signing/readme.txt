Install the Signing Tool for Production
=======================================

To install the signing tool, download STMCubeProgrammer from the ST web site.
You will need a log-in on their web page. Replace the "x.xx.x" with the actual
version number given in the file name.

    https://www.st.com/en/development-tools/stm32cubeprog.html

    unzip SetupSTM32CubeProgrammer_linux_64.zip

    chmod +x SetupSTM32CubeProgrammer-x.xx.x.linux

    ./SetupSTM32CubeProgrammer-x.xx.x.linux

Create you companies private/public keys
========================================

    STM32CubeProgrammer\bin>STM32MP_KeyGen_CLI -abs ./meta-thornxt-stm/conf/machine/signing -pwd <your password>

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
