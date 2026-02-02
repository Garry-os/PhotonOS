CC = x86_64-elf-gcc
CFLAGS = -Wall -Wextra -ffreestanding -fno-stack-protector -fno-stack-check \
		 -fno-lto -fno-PIC -mabi=sysv -mno-80387 -mno-mmx -mno-sse \
		 -mno-sse2 -mno-red-zone -mcmodel=large
ASM = nasm
ASMFLAGS = -felf64

## Include paths
CFLAGS += -I src/ -I src/arch -I src/graphics -I src/memory -I src/drivers -I src/multitasking

LD = x86_64-elf-gcc
LDFLAGS = -nostdlib -z max-page-size=0x1000 -T linker.ld
LDLIBS = -lgcc

BUILD_DIR = ../build

C_SOURCES := $(shell find src -name "*.c")
C_OBJECTS := $(patsubst src/%.c, $(BUILD_DIR)/kernel/%.o, $(C_SOURCES))

C_HEADERS := $(shell find src -name "*.h")

ASM_SOURCES := $(shell find src -name "*.asm")
ASM_OBJECTS := $(patsubst src/%.asm, $(BUILD_DIR)/kernel/%.asm_o, $(ASM_SOURCES))

.PHONY: all kernel

all: kernel

kernel: $(BUILD_DIR)/kernel/kernel.bin
$(BUILD_DIR)/kernel/kernel.bin: $(C_OBJECTS) $(ASM_OBJECTS)
	$(LD) $(LDFLAGS) -o $@ $^ $(LDLIBS)

$(BUILD_DIR)/kernel/%.asm_o: src/%.asm
	$(ASM) $(ASMFLAGS) -o $@ $<

$(BUILD_DIR)/kernel/%.o: src/%.c $(C_HEADERS)
	@ mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c -o $@ $<

