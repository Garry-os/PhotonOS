#!/bin/bash
set -e

SCRIPT=$(realpath "$0")
SCRIPTPATH=$(dirname "$SCRIPT")
cd "$SCRIPTPATH"

IMAGE_PATH=../build/disk.img

echo "Creating image..."
dd if=/dev/zero of=$IMAGE_PATH bs=1M count=64
mkfs.fat -F 32 $IMAGE_PATH
mmd -i $IMAGE_PATH ::/EFI
mmd -i $IMAGE_PATH ::/EFI/BOOT
mcopy -i $IMAGE_PATH ../build/osloader/BOOTX64.EFI ::/EFI/BOOT
mcopy -i $IMAGE_PATH ../build/kernel/kernel.bin ::/EFI/BOOT

