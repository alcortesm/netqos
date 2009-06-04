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

CAT="/bin/cat"
[ -f ${CAT} ] ; fatal "${CAT}: file not found"
[ -r ${CAT} ] ; fatal "${CAT}: read permission denied"
[ -x ${CAT} ] ; fatal "${CAT}: exec permission denied"

APTMOVE="/usr/bin/apt-move"
[ -f ${APTMOVE} ] ; fatal "${APTMOVE}: file not found"
[ -r ${APTMOVE} ] ; fatal "${APTMOVE}: read permission denied"
[ -x ${APTMOVE} ] ; fatal "${APTMOVE}: exec permission denied"

DEBOOTSTRAP="/usr/sbin/debootstrap"
[ -f ${DEBOOTSTRAP} ] ; fatal "${DEBOOTSTRAP}: file not found"
[ -r ${DEBOOTSTRAP} ] ; fatal "${DEBOOTSTRAP}: read permission denied"
[ -x ${DEBOOTSTRAP} ] ; fatal "${DEBOOTSTRAP}: exec permission denied"









# get configuration variables: EXTDATADIR & KERNELVERSION
source ${CONF}

# create tmp dir
TMPDIR=${EXTDATADIR}/tmp
[ ! -e ${TMPDIR} ] || ${RM} -rf ${TMPDIR} ; fatal "can not remove ${TMPDIR} to accommodate new data"
${MKDIR} -p ${TMPDIR} ; fatal "can not create ${TMPDIR}"
[ -d ${TMPDIR} ] ; fatal "${TMPDIR}: dir not found"
[ -r ${TMPDIR} ] ; fatal "${TMPDIR}: read permission denied"
[ -w ${TMPDIR} ] ; fatal "${TMPDIR}: write permission denied"
[ -x ${TMPDIR} ] ; fatal "${TMPDIR}: exec permission denied"

DEBSDIR=${TMPDIR}/debs
${MKDIR} -p ${DEBSDIR} ; fatal "can not create ${DEBSDIR}"
[ -d ${DEBSDIR} ] ; fatal "${DEBSDIR}: dir not found"
[ -r ${DEBSDIR} ] ; fatal "${DEBSDIR}: read permission denied"
[ -w ${DEBSDIR} ] ; fatal "${DEBSDIR}: write permission denied"
[ -x ${DEBSDIR} ] ; fatal "${DEBSDIR}: exec permission denied"

# create the mirror dir
REPODIR=${EXTDATADIR}/debian-mirror
[ ! -e ${REPODIR} ] || ${RM} -rf ${REPODIR} ; fatal "can not remove ${REPODIR} to accommodate new data"
${MKDIR} -p ${REPODIR} ; fatal "can not create ${REPODIR}"
[ -d ${REPODIR} ] ; fatal "${REPODIR}: dir not found"
[ -r ${REPODIR} ] ; fatal "${REPODIR}: read permission denied"
[ -w ${REPODIR} ] ; fatal "${REPODIR}: write permission denied"
[ -x ${REPODIR} ] ; fatal "${REPODIR}: exec permission denied"




# download all the packages required
${DEBOOTSTRAP} --download-only --include=${INCLUDE} ${DISTRO} ${DEBSDIR} http://ftp.debian.org/debian

# generate the apt-move configuration file on tmpdir
CONFFILE=${TMPDIR}/apt-move.conf
${CAT} > ${CONFFILE} << __END
#  Configuration file for the apt-move script.
# See the apt-move(8) manpage for information about these settings.
APTSITES="/all/"
LOCALDIR=${REPODIR}
DIST=${DISTRO}
PKGTYPE=binary
FILECACHE=${DEBSDIR}/var/cache/apt/archives
LISTSTATE=${DEBSDIR}/var/lib/apt/lists
DELETE=no
MAXDELETE=20
COPYONLY=yes
PKGCOMP=gzip
CONTENTS=yes
GPGKEY=
__END

# Update the local index 
${APTMOVE} -c ${CONFFILE} get

# create the local repo
${APTMOVE} -c ${CONFFILE} move

# create the local package & release files
${APTMOVE} -c ${CONFFILE} packages

${RM} -rf ${TMPDIR}

echo "done, the new local repo is at ${REPODIR}"

exit 0

