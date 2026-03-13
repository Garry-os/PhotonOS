.PHONY: all disk kernel bootloader programs clean run

all: disk

disk: kernel bootloader programs
	@ chmod +x ./scripts/make_disk.sh
	@ ./scripts/make_disk.sh

kernel:
	@ $(MAKE) -C kernel

bootloader:
	@ chmod +x limine/get.sh
	@ ./limine/get.sh
programs:
	@ chmod +x programs/test/compile.sh
	@ ./programs/test/compile.sh
	@ cp ./programs/test/test target/

clean:
	rm -rf build
	rm -rf limine/limine

run:
	qemu-system-x86_64 -hda build/disk.img -debugcon stdio -machine q35
qemu-dbg:
	qemu-system-x86_64 -hda build/disk.img -debugcon stdio -machine q35 -d int

