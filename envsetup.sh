export AELD_ASSIGNMENTS_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
# Just an "alias":
export AESD_ASSIGNMENTS_DIR="${AELD_ASSIGNMENTS_DIR}"
alias croot="cd ${AELD_ASSIGNMENTS_DIR}"

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

export XC_PREFIX="aarch64-none-linux-gnu-"
export XC_ARCH="arm64"
alias xc="${XC_PREFIX}gcc"
alias m="make"
alias mxc="make ARCH=${XC_ARCH} CROSS_COMPILE=${XC_PREFIX}"

export OUT_DIR="/var/aeld" # keep in sync with OUTDIR in finder-app/manual-linux.sh
export LINUX_ROOT="$OUT_DIR/linux-stable"

alias cout="cd ${OUT_DIR}"
alias cdlr="cd ${LINUX_ROOT}"

# The following shouldn't really be here, but they are to make chaning hosts machines easier.
alias sls="ls -lahv --group-directories-first --no-group --literal --indicator-style=slash --color=auto"
alias gs="git status"
alias gl="git log --oneline -n10"
alias gsla="git log --graph --pretty=format:'%C(bold green)%h%Creset%C(bold white)%d%Creset %C(yellow)%cr%Creset %C(bold blue)%an%Creset %C(blue)<%ae>%Creset %n%C(white)%s%n%Creset'"
alias gsl="gsla -5"
alias groot="git rev-parse --show-toplevel"
