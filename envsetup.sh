# Look for the aarch64-none-linux-gnu ARM GNU Toolchain in the parent dir,
# and, if found, add it to PATH.
toolchain_options=$(ls -d1 ../arm-gnu-toolchain-*-aarch64-none-linux-gnu/ | wc -l)
if [ $toolchain_options -lt 1 ]; then
    echo "Did not find ARM GNU Toolchain"
elif [ $toolchain_options -gt 1 ]; then
    echo "Found more than 1 ($toolchain_options) ARM GNU Toolchains: "
    ls -d1 ../arm-gnu-toolchain-*-aarch64-none-linux-gnu/
elif [ $toolchain_options -eq 1 ]; then
    toolchain=../arm-gnu-toolchain-*-aarch64-none-linux-gnu
    echo "Found ARM GNU Toolchains:"
    echo $toolchain
    echo
    toolchain=$(realpath $toolchain)
    if [ -d $toolchain/bin ]; then
        export PATH=$PATH:$toolchain/bin
        echo "Added $toolchain/bin to PATH"
        echo
        aarch64-none-linux-gnu-gcc --version
        # aarch64-none-linux-gnu-gcc -print-sysroot
    else
        echo "$toolchain is missing bin/"
    fi
fi

export CC_PREFIX="aarch64-none-linux-gnu-"
alias armgcc="${CC_PREFIX}gcc"

alias m="make"
alias mcc="make CROSS_COMPILE=${CC_PREFIX}"
alias ml="make ARCH=arm64 CROSS_COMPILE=${CC_PREFIX}"
alias mlj="make -j4 ARCH=arm64 CROSS_COMPILE=${CC_PREFIX}"

alias croot="cd $(pwd)"
alias cdlr="cd /tmp/aeld/linux-stable"
