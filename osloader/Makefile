CC = x86_64-w64-mingw32-gcc
CFLAGS = -ffreestanding -Wall -Wextra -mno-red-zone -I src -I src/inc

LD = x86_64-w64-mingw32-gcc
LDFLAGS = -nostdlib -Wl,-dll -shared -Wl,--subsystem,10 -e efi_main

BUILD_DIR=../build

C_SOURCES := $(shell find src -name "*.c")
C_OBJECTS := $(patsubst src/%.c, $(BUILD_DIR)/osloader/%.o, $(C_SOURCES))

.PHONY: all osloader clean

all: osloader

osloader: $(BUILD_DIR)/osloader/BOOTX64.EFI
$(BUILD_DIR)/osloader/BOOTX64.EFI: $(C_OBJECTS)
	$(LD) $(LDFLAGS) -o $@ $^

$(BUILD_DIR)/osloader/%.o: src/%.c
	@ mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c -o $@ $<


