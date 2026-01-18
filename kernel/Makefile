CC = x86_64-elf-gcc
CFLAGS = -ffreestanding -mno-red-zone -Wall -Wextra -fno-stack-protector \
		 -fno-stack-check -fno-lto -fno-PIC -mno-mmx -mno-sse -mno-sse2 -mno-80387

LD = x86_64-elf-gcc
LDFLAGS = -nostdlib -T linker.ld

CFLAGS += -I src -I src/arch -I ../common -I src/graphical

BUILD_DIR = ../build

C_SOURCES := $(shell find src -name "*.c")
C_OBJECTS := $(patsubst src/%.c, $(BUILD_DIR)/kernel/%.o, $(C_SOURCES))

.PHONY: all kernel

all: kernel

kernel: $(BUILD_DIR)/kernel/kernel.bin
$(BUILD_DIR)/kernel/kernel.bin: $(C_OBJECTS)
	$(LD) $(LDFLAGS) -o $@ $^

$(BUILD_DIR)/kernel/%.o: src/%.c
	@ mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c -o $@ $<

