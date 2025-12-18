#!/bin/bash
set -e

SCRIPT=$(realpath "$0")
SCRIPTPATH=$(dirname "$SCRIPT")
cd "${SCRIPTPATH}"

clone() {
	mkdir -p ../UEFI
	wget -O ../UEFI/edk2-bin.tar.xz https://github.com/rust-osdev/ovmf-prebuilt/releases/download/edk2-stable202508-r1/edk2-stable202508-r1-bin.tar.xz
	tar xvf ../UEFI/edk2-bin.tar.xz
	mv edk2-stable202508-r1-bin edk2-bin
	rm -rf ../UEFI/edk2-bin.tar.xz
}

if [[ ! -d "edk2-bin" ]]; then
	clone
else
	echo "Fetch: edk2-bin already cloned"
fi

