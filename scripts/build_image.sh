#!/bin/bash
set -e

SCRIPT=$(realpath "$0")
SCRIPTPATH=$(dirname "$SCRIPT")
cd "$SCRIPTPATH"

TARGET_DIR=../target
MOUNT_DIR=/mnt/photonOS
IMAGE_PATH=../build/disk.img
BUILD_DIR=../build

echo "Creating image..."
dd if=/dev/zero of=$IMAGE_PATH bs=1M count=64
mkfs.fat -F 32 $IMAGE_PATH

sudo mkdir -p $MOUNT_DIR
sudo mount $IMAGE_PATH $MOUNT_DIR

echo "Copying files..."
## Copy the bootloader
sudo mkdir -p $MOUNT_DIR/EFI/BOOT
sudo cp $BUILD_DIR/osloader/BOOTX64.EFI $MOUNT_DIR/EFI/BOOT/

## Copy the kernel
sudo cp $BUILD_DIR/kernel/kernel.bin $MOUNT_DIR/EFI/BOOT/

sudo cp -r $TARGET_DIR/* $MOUNT_DIR/

echo "Cleaning up..."
sudo umount $MOUNT_DIR/

