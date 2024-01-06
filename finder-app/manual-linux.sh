#!/bin/bash
# Script outline to install and build kernel.
# Author: Siddhant Jajoo.

set -e
set -u

FINDER_APP_DIR=$(realpath $(dirname $0))
OUTDIR=/var/aeld # changed from /tmp/aedl

KERNEL_REPO=git://git.kernel.org/pub/scm/linux/kernel/git/stable/linux-stable.git
KERNEL_VERSION=v5.1.10
BUSYBOX_VERSION=1_33_1

ARCH=arm64
CROSS_COMPILE=aarch64-none-linux-gnu-
makexc() {
    make ARCH=${ARCH} CROSS_COMPILE=${CROSS_COMPILE} "$@"
}

FORCE_BUILD_KERNEL=   # default: do NOT force (re-)building
FORCE_BUILD_BUSYBOX=  # default: DO force (re-)building
FORCE_MAKE_ROOTFS=1   # default: DO force (re-)making

print_content() {
    # Would prefer using the 'tree' util, but Ubuntu doesn't have in "out of the box",
    # hence using 'ls'.
    # However if you still want to use 'tree' (and have it installed!) uncomment the line below: 
    
    # tree "$@"
    
    ls -lah "$@"
} 

if [ $# -lt 1 ]
then
	echo "Using default directory ${OUTDIR} for output"
else
	OUTDIR=$1
	echo "Using passed directory ${OUTDIR} for output"
fi

mkdir -p ${OUTDIR}
cd "$OUTDIR"

# Clone Kinux kernel repo if it does not exist.
if [ ! -d "${OUTDIR}/linux-stable" ]; then
	echo "CLONING GIT LINUX STABLE VERSION ${KERNEL_VERSION} IN ${OUTDIR}"
	git clone ${KERNEL_REPO} --depth 1 --single-branch --branch ${KERNEL_VERSION}
fi

# Checkout the right branch (version)
if [ ! -z "${FORCE_BUILD_KERNEL}" ] || [ ! -e ${OUTDIR}/linux-stable/arch/${ARCH}/boot/Image ]; then
    cd linux-stable
    echo "Checking out version ${KERNEL_VERSION}"
    git checkout ${KERNEL_VERSION}

    # Building 5.1.10 kernel with recent GCC version (10 and later) fails with the following link time error:
    # (.text+0x0): multiple definition of `yylloc'; dtc-lexer.lex.o (symbol from plugin):(.text+0x0): first defined here
    # The following pathch fixes this
    # https://github.com/torvalds/linux/commit/e33a814e772cdc36436c8c188d8c42d019fda639
    ${FINDER_APP_DIR}/apply-git-patch.sh "${FINDER_APP_DIR}/scripts-dtc-Remove-redundant-YYLOC-global-declaratio.patch"

    # Source (page 15):
    # https://d3c33hcgiwev3.cloudfront.net/JNln4MtVSR-ZZ-DLVQkfYw_833bf61ded3942709a8745e579b1a0f1_Building-the-Linux-Kernel.pdf?Expires=1704412800&Signature=kViP~WLZtQm9l0Tq25DkDSiqJw8Vtpjwnu8FvadETfWEMkBfJCgshDnNV9WV8VweGsScGWutyv-jmYjgOAnWlJFRRtDWb7yvMTWnyRwC~9Dr7eEUEszU1oHedd2Zh3ijtUOyWJiisAgecT0t40yjUpZb5bOnpxmpwHFl~wjER3I_&Key-Pair-Id=APKAJLTNE6QMUY6HBC5A

    # "deep clean"
    makexc mrproper

    # Build defcongig for virt (default)
    makexc defconfig

    # Build vmlinux
    makexc -j4 all
fi

cd "$OUTDIR"
echo "Adding the Image in outdir"
cp linux-stable/arch/${ARCH}/boot/Image .
ls -lah ./Image

echo "Creating the staging directory for the root filesystem"
_ROOTFS=${OUTDIR}/rootfs
if [ ! -d "${_ROOTFS}" ]
then
    echo "Creating rootfs directory at ${_ROOTFS}"
elif [ ! -z "${FORCE_MAKE_ROOTFS}" ]
then
    echo "Deleting rootfs directory at ${_ROOTFS} and starting over"
    sudo rm -rf ${_ROOTFS}
else
    echo "Re-using existing rootfs at ${_ROOTFS}"
fi

if [ ! -d "${_ROOTFS}" ]
then
    mkdir ${_ROOTFS}
    cd ${_ROOTFS}
    mkdir \
        bin \
        dev \
        etc \
        home \
        lib \
        lib64 \
        proc \
        sbin \
        sys \
        tmp \
        usr \
        usr/bin \
        usr/lib \
        usr/sbin \
        var \
        var/log
fi

cd ${OUTDIR}
if [ ! -d "${OUTDIR}/busybox" ]
then
git clone git://busybox.net/busybox.git
    cd busybox
    git checkout ${BUSYBOX_VERSION}
else
    cd busybox
fi
# Configure (make defconfig), build and install busybox
if [ ! -z "${FORCE_BUILD_BUSYBOX}" ] || [ ! -e ./busybox ]; then
    make   distclean
    make   defconfig
    makexc
fi

_BUSYBOX_BINARY="${_ROOTFS}/bin/busybox"
if [ ! -z "${FORCE_BUILD_BUSYBOX}" ] || [ ! -e "${_BUSYBOX_BINARY}" ]; then
    makexc CONFIG_PREFIX="${_ROOTFS}" install
fi

# Add library dependencies to rootfs
echo
echo "Intalling busybox dependencies in /lib/ and /lib64/"
_GCC_SYSROOT=$(${CROSS_COMPILE}gcc -print-sysroot)

_INTERPRETER=$(${CROSS_COMPILE}readelf -a ${_BUSYBOX_BINARY} | grep "program interpreter" | sed 's|.*program interpreter: \(/.*\)].*|\1|')
# Alternatively:
#   _INTERPRETER="/lib/ld-linux-aarch64.so.1"
echo "  Interpeter:"
echo "    ${_INTERPRETER}"
cp ${_GCC_SYSROOT}/${_INTERPRETER} ${_ROOTFS}/${_INTERPRETER}

_SHARED_LIBS=$(${CROSS_COMPILE}readelf -a ${_BUSYBOX_BINARY} | grep "Shared library" | sed 's|.*Shared library: \[\(.*\)].*|\1|')
# Alternatively:
#   _SHARED_LIBS=\
#   "libm.so.6
#   libresolv.so.2
#   libc.so.6"
echo "  Shared libraries:"
while IFS= read -r lib; do
    echo "    $lib"
    cp ${_GCC_SYSROOT}/lib64/${lib} ${_ROOTFS}/lib64/
done <<< "$_SHARED_LIBS"
cd ${_ROOTFS}
print_content lib/ lib64/

# Make device nodes
# mknod <name> <type> <major> <minor>
echo
echo "Populating /dev/"
cd ${_ROOTFS}
sudo mknod -m 666 dev/null c 1 3
sudo mknod -m 600 dev/console c 5 1
print_content dev/

# Clean and build the writer utility
cd ${FINDER_APP_DIR}
make   clean
makexc all

# Copy the finder related scripts and executables to the /home directory
# on the target rootfs
echo
echo "Populating /home/"
cp -r -t "${_ROOTFS}/home/" \
    conf/ \
    autorun-qemu.sh \
    finder.sh \
    finder-test.sh \
    writer
cd ${_ROOTFS}    
print_content home/

# Chown the root directory
cd ${_ROOTFS}
sudo chown -R root:root *
echo "Copy initramfs/ into archive..."
find . | cpio -H newc -ov --owner root:root > ${OUTDIR}/initramfs.cpio

# Create initramfs.cpio.gz
echo "Compressing initramfs.cpio..."
cd ${OUTDIR}
gzip -f initramfs.cpio

echo "Done"
ls -lah ./initramfs.cpio.gz
