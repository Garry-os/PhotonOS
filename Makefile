.PHONY: all disk kernel bootloader clean run

all: disk

disk: kernel bootloader
	@ chmod +x ./scripts/make_disk.sh
	@ ./scripts/make_disk.sh

kernel:
	@ $(MAKE) -C kernel

bootloader:
	@ chmod +x limine/get.sh
	@ ./limine/get.sh

clean:
	rm -rf build
	rm -rf limine/limine

run:
	qemu-system-x86_64 -hda ./build/disk.img -debugcon stdio -machine q35

