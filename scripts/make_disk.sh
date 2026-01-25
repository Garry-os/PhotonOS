#!/bin/bash
set -e

SCRIPT=$(realpath "$0")
SCRIPTPATH=$(dirname "$SCRIPT")
cd "$SCRIPTPATH"

IMAGE_PATH=../build/disk.img
MOUNT_DIR=/mnt/PhotonOS
TARGET_DIR=../target

echo "Creating disk image"
dd if=/dev/zero of=$IMAGE_PATH bs=1M count=64 ## 64 MB image


## Partitioning (MBR)
parted $IMAGE_PATH mklabel msdos
parted $IMAGE_PATH mkpart primary fat32 2048s 100%
parted $IMAGE_PATH set 1 boot on

echo "Installing limine bootloader"
../limine/limine/limine bios-install $IMAGE_PATH

## Make loopback device
sudo losetup -P /dev/loop101 $IMAGE_PATH

echo "Formatting disk image"
sudo mkfs.fat -F 32 /dev/loop101p1
sudo fatlabel /dev/loop101p1 OS

echo "Copying files to disk image"
## Mount the image
sudo mkdir -p $MOUNT_DIR
sudo mount /dev/loop101p1 $MOUNT_DIR

sudo mkdir -p $MOUNT_DIR/boot/limine $MOUNT_DIR/EFI/BOOT
sudo cp ../limine/limine/limine-bios.sys $MOUNT_DIR/boot/limine
sudo cp ../limine/limine/BOOTX64.EFI $MOUNT_DIR/EFI/BOOT
sudo cp ../build/kernel/kernel.bin $MOUNT_DIR/boot

sudo cp -r $TARGET_DIR/* $MOUNT_DIR

echo "Cleaning up"
sudo umount -l $MOUNT_DIR
sudo losetup -d /dev/loop101p1

