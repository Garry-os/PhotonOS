BUILD_DIR = build

.PHONY: all osloader kernel clean run

all: disk

disk: osloader kernel
	@ chmod +x ./scripts/build_image.sh
	@ ./scripts/build_image.sh

ovmf-prebuilts:
	@ chmod +x ./scripts/get_ovmf.sh
	@ ./scripts/get_ovmf.sh
	@ echo "Copying files"
	@ mkdir -p build/OVMFbin
	@ cp OVMFbin/edk2-stable202511-r1-bin/x64/* build/OVMFbin/

osloader: 
	@ $(MAKE) -C osloader
kernel:
	@ $(MAKE) -C kernel

clean:
	@ echo "Cleaning up..."
	@ rm -rf build
	@ rm -rf OVMFbin

run:
	qemu-system-x86_64 -drive if=pflash,format=raw,unit=0,file=build/OVMFbin/code.fd,readonly=on \
		-drive if=pflash,format=raw,unit=1,file=build/OVMFbin/vars.fd \
		-drive format=raw,file=build/disk.img \
		-net none -machine q35

