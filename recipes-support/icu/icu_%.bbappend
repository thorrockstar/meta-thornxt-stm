FILESEXTRAPATHS:prepend := "${THISDIR}/files:"

# Add the filter file to the sources
SRC_URI += "file://filter.json"

# Enable the task that regenerates ICU data from source
PACKAGECONFIG:append = " make-icudata"

# Set the environment variable for the build tool
# Use UNPACKDIR for newer Yocto versions, or WORKDIR for older ones
export ICU_DATA_FILTER_FILE = "${WORKDIR}/filter.json"
