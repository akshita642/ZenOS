ARCH := x86_64
SRC_DIR := src
BUILD_DIR := build
DIST_DIR := dist
ARCH_DIR := $(SRC_DIR)/arch
COMMON_ARCH_DIR := $(ARCH_DIR)/common
ASM_DIR := $(ARCH_DIR)/$(ARCH)
C_DIR := $(ARCH_DIR)/$(ARCH)
KERNEL_C_DIR := $(SRC_DIR)/kernel
ISO_DIR := targets/$(ARCH)/iso

TARGET := $(DIST_DIR)/x86_64/kernel.elf
ISO := $(DIST_DIR)/x86_64/kernel.iso

AS := nasm
CC := gcc
LD := ld
GRUB_MKRESCUE := grub-mkrescue

ASMFLAGS := -f elf64 -I $(ASM_DIR)
CFLAGS := -O3 -Iinclude -m64 -nostdlib -nostdinc -fno-builtin -fno-stack-protector -nostartfiles -nodefaultlibs -Wall -Wextra -Werror -mno-red-zone -mgeneral-regs-only
LDFLAGS := -T targets/x86_64/linker.ld -melf_x86_64

ASM_SRC := $(shell find $(ASM_DIR) $(COMMON_ARCH_DIR) -name '*.asm')
C_SRC := $(shell find $(C_DIR) $(KERNEL_C_DIR) $(COMMON_ARCH_DIR) -name '*.c')

OBJ := $(ASM_SRC:$(SRC_DIR)/%.asm=$(BUILD_DIR)/%.o) $(C_SRC:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)

$(ISO): $(TARGET) $(ISO_DIR)/boot/grub/grub.cfg
	@mkdir -p $(ISO_DIR)/boot
	cp $(TARGET) $(ISO_DIR)/boot/kernel.elf
	$(GRUB_MKRESCUE) -o $(ISO) $(ISO_DIR)

$(TARGET): $(OBJ)
	@mkdir -p $(@D)
	$(LD) $(LDFLAGS) -o $@ $^

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.asm
	@mkdir -p $(@D)
	$(AS) $(ASMFLAGS) $< -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm -rf $(BUILD_DIR) $(DIST_DIR)
	rm $(ISO_DIR)/boot/kernel.elf
