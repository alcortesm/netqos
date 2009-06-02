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

MKDIR="/bin/mkdir"
[ -f ${MKDIR} ] ; fatal "${MKDIR}: file not found"
[ -r ${MKDIR} ] ; fatal "${MKDIR}: read permission denied"
[ -x ${MKDIR} ] ; fatal "${MKDIR}: exec permission denied"

RM="/bin/rm"
[ -f ${RM} ] ; fatal "${RM}: file not found"
[ -r ${RM} ] ; fatal "${RM}: read permission denied"
[ -x ${RM} ] ; fatal "${RM}: exec permission denied"

EGREP="/bin/egrep"
[ -f ${EGREP} ] ; fatal "${EGREP}: file not found"
[ -r ${EGREP} ] ; fatal "${EGREP}: read permission denied"
[ -x ${EGREP} ] ; fatal "${EGREP}: exec permission denied"

WGET="/usr/bin/wget"
[ -f ${WGET} ] ; fatal "${WGET}: file not found"
[ -r ${WGET} ] ; fatal "${WGET}: read permission denied"
[ -x ${WGET} ] ; fatal "${WGET}: exec permission denied"

GPG="/usr/bin/gpg"
[ -f ${GPG} ] ; fatal "${GPG}: file not found"
[ -r ${GPG} ] ; fatal "${GPG}: read permission denied"
[ -x ${GPG} ] ; fatal "${GPG}: exec permission denied"

TAR="/bin/tar"
[ -f ${TAR} ] ; fatal "${TAR}: file not found"
[ -r ${TAR} ] ; fatal "${TAR}: read permission denied"
[ -x ${TAR} ] ; fatal "${TAR}: exec permission denied"









# get configuration variables: EXTDATADIR & KERNELVERSION
source ${CONF}

# sanity check on kernel version, I don't know where to download other versions
# see the SRCURL var bellow
echo ${KERNELVERSION} | ${EGREP} '^2\.6\..+$' > /dev/null ; fatal "${KERNELVERSION}: this script is only valid for 2.6 kernels"

# create dest dir for the kernel
DESTDIR=${EXTDATADIR}/linux-src
VERSIONDIR=${DESTDIR}/linux-${KERNELVERSION}
[ ! -e ${VERSIONDIR} ] || ${RM} -rf ${VERSIONDIR} ; fatal "can not remove ${VERSIONDIR} to accommodate new data"
${MKDIR} -p ${VERSIONDIR} ; fatal "can not create ${VERSIONDIR}"
[ -d ${VERSIONDIR} ] ; fatal "${VERSIONDIR}: dir not found"
[ -r ${VERSIONDIR} ] ; fatal "${VERSIONDIR}: read permission denied"
[ -w ${VERSIONDIR} ] ; fatal "${VERSIONDIR}: write permission denied"
[ -x ${VERSIONDIR} ] ; fatal "${VERSIONDIR}: exec permission denied"

# download the kernel (about 55 MB)
BASEURL=http://www.kernel.org/pub/linux/kernel/v2.6

SRCFILE=linux-${KERNELVERSION}.tar.bz2
SIGFILE=${SRCFILE}.sign

SRCURL=${BASEURL}/${SRCFILE}
SIGURL=${BASEURL}/${SIGFILE}

SRCFILE=${DESTDIR}/${SRCFILE}
SIGFILE=${DESTDIR}/${SIGFILE}

cd ${DESTDIR}
if [ -e ${SRCFILE} ]
then
    echo "$SRCFILE found, no need to download it!"
else
    ${WGET} ${SRCURL}
fi
if [ -e ${SIGFILE} ]
then
    echo "$SIGFILE found, no need to download it!"
else
    ${WGET} ${SIGURL}
fi

echo "verifying gpg signature for kernel integrity..."
${GPG} --verify ${SIGFILE} ${SRCFILE} ; fatal "kernel integrity could not be verified!"

echo "decompressing the kernel..."
${TAR} -xjf ${SRCFILE}

echo "done, your kernel tree is at ${VERSIONDIR}"

exit 0
