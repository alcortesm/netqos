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

CHMOD="/bin/chmod"
[ -f ${CHMOD} ] ; fatal "${CHMOD}: file not found"
[ -r ${CHMOD} ] ; fatal "${CHMOD}: read permission denied"
[ -x ${CHMOD} ] ; fatal "${CHMOD}: exec permission denied"

SUDO="/usr/bin/sudo"
[ -f ${SUDO} ] ; fatal "${SUDO}: file not found"
[ -r ${SUDO} ] ; fatal "${SUDO}: read permission denied"
[ -x ${SUDO} ] ; fatal "${SUDO}: exec permission denied"

MOUNT="/bin/mount"
[ -f ${MOUNT} ] ; fatal "${MOUNT}: file not found"
[ -r ${MOUNT} ] ; fatal "${MOUNT}: read permission denied"
[ -x ${MOUNT} ] ; fatal "${MOUNT}: exec permission denied"

UMOUNT="/bin/umount"
[ -f ${UMOUNT} ] ; fatal "${UMOUNT}: file not found"
[ -r ${UMOUNT} ] ; fatal "${UMOUNT}: read permission denied"
[ -x ${UMOUNT} ] ; fatal "${UMOUNT}: exec permission denied"

DD="/bin/dd"
[ -f ${DD} ] ; fatal "${DD}: file not found"
[ -r ${DD} ] ; fatal "${DD}: read permission denied"
[ -x ${DD} ] ; fatal "${DD}: exec permission denied"

MKFSEXT3="/sbin/mkfs.ext3"
[ -f ${MKFSEXT3} ] ; fatal "${MKFSEXT3}: file not found"
[ -r ${MKFSEXT3} ] ; fatal "${MKFSEXT3}: read permission denied"
[ -x ${MKFSEXT3} ] ; fatal "${MKFSEXT3}: exec permission denied"

DEBOOTSTRAP="/usr/sbin/debootstrap"
[ -f ${DEBOOTSTRAP} ] ; fatal "${DEBOOTSTRAP}: file not found"
[ -r ${DEBOOTSTRAP} ] ; fatal "${DEBOOTSTRAP}: read permission denied"
[ -x ${DEBOOTSTRAP} ] ; fatal "${DEBOOTSTRAP}: exec permission denied"

BASH="/bin/bash"
[ -f ${BASH} ] ; fatal "${BASH}: file not found"
[ -r ${BASH} ] ; fatal "${BASH}: read permission denied"
[ -x ${BASH} ] ; fatal "${BASH}: exec permission denied"

LN="/bin/ln"
[ -f ${LN} ] ; fatal "${LN}: file not found"
[ -r ${LN} ] ; fatal "${LN}: read permission denied"
[ -x ${LN} ] ; fatal "${LN}: exec permission denied"

EGREP="/bin/egrep"
[ -f ${EGREP} ] ; fatal "${EGREP}: file not found"
[ -r ${EGREP} ] ; fatal "${EGREP}: read permission denied"
[ -x ${EGREP} ] ; fatal "${EGREP}: exec permission denied"

CP="/bin/cp"
[ -f ${CP} ] ; fatal "${CP}: file not found"
[ -r ${CP} ] ; fatal "${CP}: read permission denied"
[ -x ${CP} ] ; fatal "${CP}: exec permission denied"

RMDIR="/bin/rmdir"
[ -f ${RMDIR} ] ; fatal "${RMDIR}: file not found"
[ -r ${RMDIR} ] ; fatal "${RMDIR}: read permission denied"
[ -x ${RMDIR} ] ; fatal "${RMDIR}: exec permission denied"






# get configuration variables: EXTDATADIR & KERNELVERSION
source ${CONF}

# create dest dir for the rootfs
DESTDIR=${EXTDATADIR}/rootfs
[ ! -e ${DESTDIR} ] || ${RM} -rf ${DESTDIR} ; fatal "can not remove ${DESTDIR} to accommodate new data"
${MKDIR} -p ${DESTDIR} ; fatal "can not create ${DESTDIR}"
[ -d ${DESTDIR} ] ; fatal "${DESTDIR}: dir not found"
[ -r ${DESTDIR} ] ; fatal "${DESTDIR}: read permission denied"
[ -w ${DESTDIR} ] ; fatal "${DESTDIR}: write permission denied"
[ -x ${DESTDIR} ] ; fatal "${DESTDIR}: exec permission denied"

debianVersion=$DISTRO
rootfsSize=450 # in MB
timezone=Europe/Madrid
umlPath=${EXTDATADIR}/linux-src/linux-${KERNELVERSION}/linux
umlModulesPath=${EXTDATADIR}/linux-src/linux-${KERNELVERSION}/modules/lib/modules
launchScriptFile=${DESTDIR}/run-uml.sh
localRepo=${EXTDATADIR}/debian-mirror

[ -f ${umlPath} ] ; fatal "${umlPath}: file not found"
[ -r ${umlPath} ] ; fatal "${umlPath}: read permission denied"
[ -x ${umlPath} ] ; fatal "${umlPath}: exec permission denied"

[ -d ${umlModulesPath} ] ; fatal "${umlModulesPath}: dir not found"
[ -r ${umlModulesPath} ] ; fatal "${umlModulesPath}: read permission denied"
[ -x ${umlModulesPath} ] ; fatal "${umlModulesPath}: exec permission denied"

[ ! -e ${launchScriptFile} ] || ${RM} -rf ${launchScriptFile} ; fatal "can not remove ${launchScriptFile} to accommodate new data"

if [ ! -d $localRepo ]
then
    debianFTP=http://ftp.debian.org/debian
else
    debianFTP=file://$localRepo
fi
    


# DON'T EDIT BELLOW THIS LINE IF YOU DON'T
# KNOW WHAT YOU ARE DOING

# First, generate de run script
cat > $launchScriptFile << __END
#!/bin/sh

BASE=${DESTDIR}
TMP=\${BASE}/tmp

CLIENT=\${BASE}/client.rootfs
HUB1=\${TMP}/hub1.ctl
CCOW=\${TMP}/client.cow

TERM=/usr/bin/rxvt
UMLTERM=/usr/bin/xterm
UML=${umlPath}
SWITCH=/usr/bin/uml_switch


# DON'T EDIT BELLOW THIS LINE IF YOU DON'T
# KNOW WHAT YOU ARE DOING

function fatal {
    if [ ! \$? -eq 0 ]
    then
        echo \$1 > /dev/stderr
        exit 1
    fi
}

# check useful commands
MKDIR="/bin/mkdir"
[ -f \${MKDIR} ] ; fatal "\${MKDIR}: file not found"
[ -r \${MKDIR} ] ; fatal "\${MKDIR}: read permission denied"
[ -x \${MKDIR} ] ; fatal "\${MKDIR}: exec permission denied"

RM="/bin/rm"
[ -f \${RM} ] ; fatal "\${RM}: file not found"
[ -r \${RM} ] ; fatal "\${RM}: read permission denied"
[ -x \${RM} ] ; fatal "\${RM}: exec permission denied"


# check base & tmp dir
[ -d \${BASE} ] ; fatal "\${BASE}: dir not found"
[ -r \${BASE} ] ; fatal "\${BASE}: read permission denied"
[ -w \${BASE} ] ; fatal "\${BASE}: write permission denied"
[ -x \${BASE} ] ; fatal "\${BASE}: exec permission denied"
\${MKDIR} -p \${TMP}
[ -d \${TMP} ] ; fatal "\${TMP}: dir not found"
[ -r \${TMP} ] ; fatal "\${TMP}: read permission denied"
[ -w \${TMP} ] ; fatal "\${TMP}: write permission denied"
[ -x \${TMP} ] ; fatal "\${TMP}: exec permission denied"

# remove leftover from previous incantations
[ ! -e \${CCOW} ] || \${RM} -f \${CCOW} ; fatal "can not remove \${CCOW} to accommodate new data"
[ ! -e \${HUB1} ] || \${RM} -f \${HUB1} ; fatal "can not remove \${HUB1} to accommodate new data"

# check root file systems
[ -f \${CLIENT} ] ; fatal "\${CLIENT}: file not found"
[ -r \${CLIENT} ] ; fatal "\${CLIENT}: read permission denied"

# check commands
[ -f \${UMLTERM} ] ; fatal "\${UMLTERM}: command not found"
[ -x \${UMLTERM} ] ; fatal "\${UMLTERM}: exec permission denied"
[ -f \${TERM} ] ; fatal "\${TERM}: command not found"
[ -x \${TERM} ] ; fatal "\${TERM}: exec permission denied"
[ -f \${UML} ] ; fatal "\${UML}: command not found"
[ -x \${UML} ] ; fatal "\${UML}: exec permission denied"
[ -f \${SWITCH} ] ; fatal "\${SWITCH}: command not found"
[ -x \${SWITCH} ] ; fatal "\${SWITCH}: exec permission denied"

#build the launch command
command1="\${TERM} -bg black -fg white -e \${SWITCH} -hub -unix \${HUB1} &"
command2="\${TERM} -bg black -fg white -e \${UML} mem=64M ubd0=\${CCOW},\${CLIENT} eth0=daemon,fe:fd:00:00:01:00,unix,\${HUB1} eth1=daemon,fe:fd:00:00:01:01,unix,\${HUB1} eth2=daemon,fe:fd:00:00:01:02,unix,\${HUB1} eth3=daemon,fe:fd:00:00:01:03,unix,\${HUB1} eth4=daemon,fe:fd:00:00:01:04,unix,\${HUB1} con=pty con0=fd:0,fd:1 con1=xterm con2=xterm xterm=\${UMLTERM},-T,-e umid=client &"

# echo the commands we are going to run
echo "\$command1"
echo "\$command2"

# run the command
eval \$command1
eval \$command2
__END
${CHMOD} u+x ${launchScriptFile}

echo "########## ${launchScriptFile} script succesfully generated"



# Generate the client image (client.rootfs)

name=client
packages=$INCLUDE
image=${DESTDIR}/$name.rootfs

echo "########## Refreshing sudo passwd"
$SUDO $SUDO > /dev/null

# umount the image from previous invocations
${MOUNT} 2>/dev/null | ${EGREP} "^$image .*$" >/dev/null
if [ $? -eq 0 ]
then
    ${SUDO} ${UMOUNT} $image
fi

# remove the image from previous invocations
[ ! -e ${image} ] || ${RM} -f ${image} ; fatal "can not remove ${image} to accommodate new data"

echo "########## Creating the $name image"
${DD} if=/dev/zero of=$image bs=1M count=$rootfsSize
${MKFSEXT3} -F $image

# create tmp dir
TMPDIR=${EXTDATADIR}/tmp
[ ! -e ${TMPDIR} ] || ${RM} -rf ${TMPDIR} ; fatal "can not remove ${TMPDIR} to accommodate new data"
${MKDIR} -p ${TMPDIR} ; fatal "can not create ${TMPDIR}"
[ -d ${TMPDIR} ] ; fatal "${TMPDIR}: dir not found"
[ -r ${TMPDIR} ] ; fatal "${TMPDIR}: read permission denied"
[ -w ${TMPDIR} ] ; fatal "${TMPDIR}: write permission denied"
[ -x ${TMPDIR} ] ; fatal "${TMPDIR}: exec permission denied"

mountPoint=${TMPDIR}/mnt-$name
if [ ! -e $mountPoint ]
then
    echo "########## Making mount point $mountPoint"
    ${MKDIR} -p $mountPoint
fi
echo "########## Mounting $image"
${SUDO} ${MOUNT} -o loop $image $mountPoint

echo "########## Installing base system"
${SUDO} ${DEBOOTSTRAP} --include=$packages $debianVersion $mountPoint $debianFTP

echo "########## Configuring base system"

echo "########## Configuring hostname"
string="$name"
${SUDO} ${BASH} -c "echo \"$string\" > $mountPoint/etc/hostname"

echo "########## Configuring timezone"
string="$timezone"
${SUDO} ${BASH} -c "echo \"$string\" > $mountPoint/etc/timezone"
${SUDO} ${RM} $mountPoint/etc/localtime
${SUDO} ${LN} -sf /usr/share/zoneinfo/$timezone $mountPoint/etc/localtime

echo "########## Configuring udev iface rules"
tmpfile=${TMPDIR}/z25_persistent-net.rules
${CAT} > $tmpfile << __END
# This file was automatically generated by the /lib/udev/write_net_rules
# program, probably run by the persistent-net-generator.rules rules file.
#
# You can modify it, as long as you keep each rule on a single line.
# MAC addresses must be written in lowercase.

SUBSYSTEM=="net", DRIVERS=="?*", ATTRS{address}=="fe:fd:00:00:00:00", NAME="eth0"

SUBSYSTEM=="net", DRIVERS=="?*", ATTRS{address}=="fe:fd:00:00:00:01", NAME="eth1"

SUBSYSTEM=="net", DRIVERS=="?*", ATTRS{address}=="fe:fd:00:00:00:02", NAME="eth2"

SUBSYSTEM=="net", DRIVERS=="?*", ATTRS{address}=="fe:fd:00:00:00:03", NAME="eth3"

SUBSYSTEM=="net", DRIVERS=="?*", ATTRS{address}=="fe:fd:00:00:00:04", NAME="eth4"

__END
${SUDO} ${CP} $tmpfile $mountPoint/etc/udev/rules.d/z25_persistent-net.rules

echo "########## Configuring /etc/network/interfaces"
tmpfile=${TMPDIR}/net-int
${CAT} > $tmpfile << __END
# Used by ifup(8) and ifdown(8). See the interfaces(5) manpage or
# /usr/share/doc/ifupdown/examples for more information.

auto lo
iface lo inet loopback

auto eth0
iface eth0 inet static
        address 192.168.254.1
        netmask 255.255.255.0
        broadcast 192.168.254.255

auto eth1
iface eth1 inet static
        address 192.168.1.11
        netmask 255.255.255.0
        broadcast 192.168.1.255

auto eth2
iface eth2 inet static
        address 192.168.1.12
        netmask 255.255.255.0
        broadcast 192.168.1.255

auto eth3
iface eth3 inet static
        address 192.168.1.13
        netmask 255.255.255.0
        broadcast 192.168.1.255

auto eth4
iface eth4 inet static
        address 192.168.1.14
        netmask 255.255.255.0
        broadcast 192.168.1.255
__END
${SUDO} ${CP} $tmpfile $mountPoint/etc/network/interfaces

echo "########## Configuring /etc/fstab"
${SUDO} ${MKDIR} $mountPoint/mnt/host-tmp

tmpfile=${TMPDIR}/fstab
${CAT} > $tmpfile << __END
# PRECONFIGURED FSTAB

/dev/ubda   /               ext3    defaults        0       1
none        /proc           proc    defaults        0       0
none        /dev/shm        tmpfs   defaults        0       0
none        /mnt/host-tmp   hostfs  defaults,/tmp   0       0
__END
${SUDO} ${CP} $tmpfile $mountPoint/etc/fstab


echo "########## Configuring /etc/hosts"
tmpfile=${TMPDIR}/hosts
${CAT} > $tmpfile << __END
127.0.0.1    localhost

192.168.254.1    client
192.168.1.11    client1
192.168.1.12    client2
192.168.1.13    client3
192.168.1.14    client4
__END
${SUDO} ${CP} $tmpfile $mountPoint/etc/hosts

echo "########## Configuring /root/.bashrc"
sudo bash -c "echo PATH=\\\$PATH:/mnt/host-tmp >> $mountPoint/root/.bashrc"

# Root password
#sudo chroot $mountPoint passwd

echo "########## Copying kernel modules"
${SUDO} ${CP} -Rp $umlModulesPath/* $mountPoint/lib/modules

# Done with the base system, unmounting
${SUDO} ${UMOUNT} $image
${SUDO} ${RMDIR} $mountPoint

echo "########## $name image succesfully generated"
echo "You can find your image an the launch script at"
echo "    $DESTDIR"
exit 0
