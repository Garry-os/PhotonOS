#!/bin/bash
set -e

SCRIPT=$(realpath "$0")
SCRIPTPATH=$(dirname "$SCRIPT")
cd "${SCRIPTPATH}"

fetch() {
	rm -rf gnu-efi
	git clone https://github.com/ncroxon/gnu-efi.git
	cd gnu-efi/
	make
	cd ../
}

if [[ ! -d "gnu-efi" ]]; then
	fetch
else
	echo "Fetch: GNU EFI is already cloned"
fi


