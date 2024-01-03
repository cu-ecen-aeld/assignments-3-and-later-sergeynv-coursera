#!/bin/bash
# Script outline to install and build kernel.
# Author: Siddhant Jajoo.

set -e
set -u

OUTDIR=/tmp/aeld
KERNEL_REPO=git://git.kernel.org/pub/scm/linux/kernel/git/stable/linux-stable.git
KERNEL_VERSION=v5.1.10
BUSYBOX_VERSION=1_33_1
FINDER_APP_DIR=$(realpath $(dirname $0))
ARCH=arm64
CROSS_COMPILE=aarch64-none-linux-gnu-

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
if [ ! -e ${OUTDIR}/linux-stable/arch/${ARCH}/boot/Image ]; then
    cd linux-stable
    echo "Checking out version ${KERNEL_VERSION}"
    git checkout ${KERNEL_VERSION}

    # Building 5.1.10 kernel with recent GCC version (10 and later) fails with the following link time error:
    # (.text+0x0): multiple definition of `yylloc'; dtc-lexer.lex.o (symbol from plugin):(.text+0x0): first defined here
    # The following pathch fixes this
    # https://github.com/torvalds/linux/commit/e33a814e772cdc36436c8c188d8c42d019fda639
    git am ${FINDER_APP_DIR}/scripts-dtc-Remove-redundant-YYLOC-global-declaratio.patch

    # Source (page 15):
    # https://d3c33hcgiwev3.cloudfront.net/JNln4MtVSR-ZZ-DLVQkfYw_833bf61ded3942709a8745e579b1a0f1_Building-the-Linux-Kernel.pdf?Expires=1704412800&Signature=kViP~WLZtQm9l0Tq25DkDSiqJw8Vtpjwnu8FvadETfWEMkBfJCgshDnNV9WV8VweGsScGWutyv-jmYjgOAnWlJFRRtDWb7yvMTWnyRwC~9Dr7eEUEszU1oHedd2Zh3ijtUOyWJiisAgecT0t40yjUpZb5bOnpxmpwHFl~wjER3I_&Key-Pair-Id=APKAJLTNE6QMUY6HBC5A

    # "deep clean"
    make ARCH=arm64 CROSS_COMPILE=${CROSS_COMPILE} mrproper

    # Build defcongig for virt (default)
    make ARCH=arm64 CROSS_COMPILE=${CROSS_COMPILE} defconfig

    # Build vmlinux
    make -j4 ARCH=arm64 CROSS_COMPILE=${CROSS_COMPILE} all
fi

# TODO-sergeynv: (re)move.
exit 0

echo "Adding the Image in outdir"

echo "Creating the staging directory for the root filesystem"
cd "$OUTDIR"
if [ -d "${OUTDIR}/rootfs" ]
then
	echo "Deleting rootfs directory at ${OUTDIR}/rootfs and starting over"
    sudo rm  -rf ${OUTDIR}/rootfs
fi

# TODO: Create necessary base directories

cd "$OUTDIR"
if [ ! -d "${OUTDIR}/busybox" ]
then
git clone git://busybox.net/busybox.git
    cd busybox
    git checkout ${BUSYBOX_VERSION}
    # TODO:  Configure busybox
else
    cd busybox
fi

# TODO: Make and install busybox

echo "Library dependencies"
${CROSS_COMPILE}readelf -a bin/busybox | grep "program interpreter"
${CROSS_COMPILE}readelf -a bin/busybox | grep "Shared library"

# TODO: Add library dependencies to rootfs

# TODO: Make device nodes

# TODO: Clean and build the writer utility

# TODO: Copy the finder related scripts and executables to the /home directory
# on the target rootfs

# TODO: Chown the root directory

# TODO: Create initramfs.cpio.gz
