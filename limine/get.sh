#!/bin/bash
set -e

SCRIPT=$(realpath "$0")
SCRIPTPATH=$(dirname "$SCRIPT")
cd "${SCRIPTPATH}"

get() {
	git clone https://codeberg.org/Limine/Limine.git limine --branch=v10.x-binary --depth=1

	make -C limine
}

if [[ ! -f "limine/limine" ]]; then
	get
fi

