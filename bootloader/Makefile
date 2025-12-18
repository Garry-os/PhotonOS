CC = gcc # Maybe change this to a cross compiler later
CFLAGS = -Wall -Wextra -fpic -ffreestanding -fno-stack-protector -fno-stack-check -fshort-wchar \
		  -mno-red-zone -maccumulate-outgoing-args -Ignu-efi/inc -Ignu-efi/inc/x86_64
LD = ld
LDFLAGS = -shared -Bsymbolic -Lgnu-efi/x86_64/lib -Lgnu-efi/x86_64/gnuefi \
		  -Tgnu-efi/gnuefi/elf_x86_64_efi.lds gnu-efi/x86_64/gnuefi/crt0-efi-x86_64.o \
		  -lgnuefi -lefi

C_SOURCES := $(shell find src -name "*.c")
C_OBJECTS := $(patsubst src/%.c, $(BUILD_DIR)/boot/%.o, $(C_SOURCES))

BUILD_DIR = ../build
SRC_DIR = src

.PHONY: all bootloader clean always gnu-efi-clone always

all: bootloader

bootloader: $(BUILD_DIR)/boot/BOOTX64.EFI
$(BUILD_DIR)/boot/BOOTX64.EFI: $(BUILD_DIR)/boot/boot.so
	objcopy -j .text -j .sdata -j .data -j .rodata -j .dynamic -j .dynsym  -j .rel -j .rela -j .rel.* -j .rela.* -j .reloc \
		--target efi-app-x86_64 --subsystem=10 $^ $@

$(BUILD_DIR)/boot/boot.so: $(BUILD_DIR)/boot/boot.o
	$(LD) $(LDFLAGS) -o $@ $^

$(BUILD_DIR)/boot/%.o: src/%.c
	@ mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c -o $@ $<

gnu-efi-clone:
	@ echo "Cloning GNU-EFI"
	@ chmod +x fetch-gnu-efi.sh
	@ ./fetch-gnu-efi.sh

clean:
	rm -rf $(BUILD_DIR)

