#!/bin/bash

function fatal {
    if [ ! $? -eq 0 ]
    then
        echo $1 > /dev/stderr
        exit 1
    fi
}

CONF="CONFIGUREME"
[ -f ${CONF} ] ; fatal "${CONF}: file not found"
[ -r ${CONF} ] ; fatal "${CONF}: read permission denied"

MAKE="/usr/bin/make"
[ -f ${MAKE} ] ; fatal "${MAKE}: file not found"
[ -r ${MAKE} ] ; fatal "${MAKE}: read permission denied"
[ -x ${MAKE} ] ; fatal "${MAKE}: exec permission denied"









# get configuration variables: EXTDATADIR & KERNELVERSION
source ${CONF}

# sanity checks on kernel dir
KERNELDIR=${EXTDATADIR}/linux-src/linux-${KERNELVERSION}
[ -d ${KERNELDIR} ] ; fatal "${KERNELDIR}: dir not found"
[ -r ${KERNELDIR} ] ; fatal "${KERNELDIR}: read permission denied"
[ -w ${KERNELDIR} ] ; fatal "${KERNELDIR}: write permission denied"
[ -x ${KERNELDIR} ] ; fatal "${KERNELDIR}: exec permission denied"

cd ${KERNELDIR}
${MAKE} mrproper

# generate cscope database, useful to browse the sources
echo "generating cscope database"
${MAKE} cscope ; fatal "could not build the cscope database"

# generate default configuration for UML
${MAKE} defconfig ARCH=um ; fatal "could not build the default config file"
echo "default configuration stored at ${KERNELDIR}/.config"

# change default configuration
${MAKE} menuconfig ARCH=um ; fatal "could not generate your ad-hoc config file"

${MAKE} ARCH=um ; fatal "kernel compilation failed!"
${MAKE} modules_install INSTALL_MOD_PATH=modules ARCH=um
fatal "modules install failed! "

echo "your uml binary is at ${KERNELDIR}/linux"
echo "your uml modules are at ${KERNELDIR}/modules/lib/modules"

exit 0
