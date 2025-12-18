UEFI_FOLDER = ./UEFI/edk2-bin/x64

.PHONY: all disk bootloader uefi-files run clean always

all: disk

disk: build/disk.img
build/disk.img: bootloader uefi-files
	@ echo "Starting disk building process"
	@ chmod +x scripts/make_disk.sh
	@ ./scripts/make_disk.sh

bootloader:
	@ echo "Building bootloader"
	@ $(MAKE) -C bootloader

uefi-files:
	@ mkdir -p build/UEFI
	@ chmod +x ./UEFI/clone-uefi.sh
	@ ./UEFI/clone-uefi.sh
	@ echo "Copying UEFI files..."
	@ cp $(UEFI_FOLDER)/code.fd build/UEFI/code.fd
	@ cp $(UEFI_FOLDER)/vars.fd build/UEFI/vars.fd

always:
	@ mkdir -p build

clean:
	@ echo "Cleaning up"
	@ rm -rf build
	@ rm -rf bootloader/gnu-efi
	@ rm -rf UEFI/edk2-bin

run:
	qemu-system-x86_64 -machine q35 \
		-drive if=pflash,format=raw,readonly=on,file=build/UEFI/code.fd \
		-drive if=pflash,format=raw,file=build/UEFI/vars.fd \
		-drive format=raw,file=build/disk.img



