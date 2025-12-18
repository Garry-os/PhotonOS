#!/bin/bash
set -e

echo "Creating disk image (100MB)"
dd if=/dev/zero of=build/disk.img bs=1M count=100

echo "Formatting disk image"
mformat -i build/disk.img ::

echo "Copying files to disk image"
mmd -i build/disk.img ::/EFI
mmd -i build/disk.img ::/EFI/BOOT
mcopy -i build/disk.img build/boot/BOOTX64.EFI ::/EFI/BOOT

echo "Done."


