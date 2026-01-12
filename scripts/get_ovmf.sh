#!/bin/bash
set -e

SCRIPT=$(realpath "$0")
SCRIPTPATH=$(dirname "$SCRIPT")
cd "$SCRIPTPATH"

fetch() {
	wget https://github.com/rust-osdev/ovmf-prebuilt/releases/download/edk2-stable202511-r1/edk2-stable202511-r1-bin.tar.xz
	tar xvf edk2-stable202511-r1-bin.tar.xz
	mkdir -p ../OVMFbin
	mv edk2-stable202511-r1-bin ../OVMFbin
	rm -rf edk2*
}

if [[ ! -d "../OVMFbin" ]]; then
	echo "Downloading EDK2 OVMF prebuilts"
	fetch
fi

