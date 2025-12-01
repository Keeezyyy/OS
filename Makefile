# ===========================
#  Compiler / Tools
# ===========================
CC        = i686-elf-gcc
LD        = i686-elf-ld
AS        = nasm
OBJCOPY   = i686-elf-objcopy

CFLAGS    = -ffreestanding -fno-pie -fno-stack-protector -m32 -O2 -Wall -Wextra
ASFLAGS32 = -f elf32
NASMFLAGS = -f bin

# 64-bit Compiler für Kernel
CC64      = x86_64-elf-gcc
LD64      = x86_64-elf-ld
OBJCOPY64 = x86_64-elf-objcopy

CFLAGS64  = -ffreestanding -fno-pie -fno-stack-protector -m64 -O2 -Wall -Wextra
ASFLAGS64 = -f elf64

BPB_SIZE  = 62

# ===========================
#  Verzeichnisse
# ===========================
SRC_STAGE1 = src/boot/stage_1
SRC_STAGE2 = src/boot/stage_2
SRC_KERNEL = src/kernel

BUILD      = build
IMG_DIR    = img

STAGE1_DIR = $(BUILD)/boot/stage_1
STAGE2_DIR = $(BUILD)/boot/stage_2
KERNEL_DIR = $(BUILD)/kernel

# ===========================
#  Stage-2 Quellen
# ===========================
STAGE2_C_SRC   = $(shell find $(SRC_STAGE2) -type f -name "*.c")
STAGE2_ASM_SRC = $(shell find $(SRC_STAGE2) -type f -name "*.asm")

STAGE2_C_OBJ   = $(STAGE2_C_SRC:$(SRC_STAGE2)/%.c=$(STAGE2_DIR)/%.o)
STAGE2_ASM_OBJ = $(STAGE2_ASM_SRC:$(SRC_STAGE2)/%.asm=$(STAGE2_DIR)/%.o)

STAGE2_OBJ = $(STAGE2_C_OBJ) $(STAGE2_ASM_OBJ)
STAGE2_ELF = $(STAGE2_DIR)/stage2.elf
STAGE2_BIN = $(STAGE2_DIR)/stage2.bin

# ===========================
#  Kernel-Quellen
# ===========================
KERNEL_C_SRC   = $(shell find $(SRC_KERNEL) -type f -name "*.c")
KERNEL_ASM_SRC = $(shell find $(SRC_KERNEL) -type f -name "*.asm")

KERNEL_C_OBJ   = $(KERNEL_C_SRC:$(SRC_KERNEL)/%.c=$(KERNEL_DIR)/%.o)
KERNEL_ASM_OBJ = $(KERNEL_ASM_SRC:$(SRC_KERNEL)/%.asm=$(KERNEL_DIR)/%.o)

KERNEL_OBJ = $(KERNEL_C_OBJ) $(KERNEL_ASM_OBJ)
KERNEL_ELF = $(KERNEL_DIR)/kernel.elf
KERNEL_BIN = $(KERNEL_DIR)/kernel.bin

# ===========================
#  Images
# ===========================
FLOPPY_IMG = $(IMG_DIR)/floppy.img
HDD_IMG    = $(IMG_DIR)/harddisk.img

# ===========================
#  Phony Targets
# ===========================
.PHONY: all clean stage1 stage2 kernel image harddisk run

all: image harddisk

# ===========================
#  Stage-2 C kompilieren
# ===========================
$(STAGE2_DIR)/%.o: $(SRC_STAGE2)/%.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# ===========================
#  Stage-2 ASM kompilieren
# ===========================
$(STAGE2_DIR)/%.o: $(SRC_STAGE2)/%.asm
	mkdir -p $(dir $@)
	$(AS) $(ASFLAGS32) $< -o $@

# ===========================
#  Stage-2 Linken (ELF → BIN)
# ===========================
stage2: $(STAGE2_BIN)

$(STAGE2_BIN): $(STAGE2_OBJ) src/boot/linker.ld
	mkdir -p $(STAGE2_DIR)
	$(LD) -T src/boot/linker.ld -o $(STAGE2_ELF) $(STAGE2_OBJ)
	$(OBJCOPY) -O binary $(STAGE2_ELF) $(STAGE2_BIN)

# ===========================
#  Stage-1 Bootloader bauen
# ===========================
stage1: $(STAGE1_DIR)/boot.bin

$(STAGE1_DIR)/boot.bin: $(SRC_STAGE1)/boot.asm
	mkdir -p $(STAGE1_DIR)
	$(AS) $(NASMFLAGS) -o $@ $<

# ===========================
#  Kernel C kompilieren (64-bit)
# ===========================
$(KERNEL_DIR)/%.o: $(SRC_KERNEL)/%.c
	mkdir -p $(dir $@)
	$(CC64) $(CFLAGS64) -c $< -o $@

# ===========================
#  Kernel ASM kompilieren (64-bit)
# ===========================
$(KERNEL_DIR)/%.o: $(SRC_KERNEL)/%.asm
	mkdir -p $(dir $@)
	$(AS) $(ASFLAGS64) $< -o $@

# ===========================
#  Kernel Link (ELF → BIN)
#  — FIXED: jetzt mit LD64
# ===========================
kernel: $(KERNEL_BIN)

$(KERNEL_BIN): $(KERNEL_OBJ) src/kernel/linker.ld
	mkdir -p $(KERNEL_DIR)
	$(LD64) -T src/kernel/linker.ld -o $(KERNEL_ELF) $(KERNEL_OBJ)
	$(OBJCOPY64) -O binary $(KERNEL_ELF) $(KERNEL_BIN)

# ===========================
#  FAT12 Floppy-Image erzeugen
# ===========================
image: $(FLOPPY_IMG)

$(FLOPPY_IMG): $(STAGE1_DIR)/boot.bin $(STAGE2_BIN)
	mkdir -p $(IMG_DIR) $(BUILD)/boot

	dd if=/dev/zero of=$(FLOPPY_IMG) bs=512 count=2880
	mkfs.fat -F 12 $(FLOPPY_IMG)

	dd if=$(FLOPPY_IMG) of=$(BUILD)/boot/bpb.bin bs=1 count=$(BPB_SIZE)

	cat $(BUILD)/boot/bpb.bin $(STAGE1_DIR)/boot.bin > $(BUILD)/boot/boot_sector.tmp
	truncate -s 510 $(BUILD)/boot/boot_sector.tmp
	printf "\x55\xAA" >> $(BUILD)/boot/boot_sector.tmp

	dd if=$(BUILD)/boot/boot_sector.tmp of=$(FLOPPY_IMG) conv=notrunc bs=512 count=1

	mcopy -i $(FLOPPY_IMG) $(STAGE2_BIN) ::entry.bin

# ===========================
#  Harddisk-Image mit Kernel
# ===========================
harddisk: $(HDD_IMG)

$(HDD_IMG): $(KERNEL_BIN)
	mkdir -p $(IMG_DIR)

	dd if=/dev/zero of=$(HDD_IMG) bs=1M count=128
	mkfs.fat -F 16 $(HDD_IMG)

	mmd -i $(HDD_IMG) ::/boot
	mmd -i $(HDD_IMG) ::/boot/bin
	mmd -i $(HDD_IMG) ::/boot/bin/kernel
	mmd -i $(HDD_IMG) ::/boot/bin/kernel/core

	mcopy -i $(HDD_IMG) $(KERNEL_DIR)/kernel.elf ::/boot/bin/kernel/core/kernel.o
	mcopy -i $(HDD_IMG) $(KERNEL_DIR)/kernel.elf ::/kernel.o

# ===========================
#  QEMU Start
# ===========================
run: all
	qemu-system-x86_64 \
	    -monitor stdio \
	    -fda $(FLOPPY_IMG) \
	    -drive file=$(HDD_IMG),format=raw,if=ide \
	    -boot a

# ===========================
#  Reinigung
# ===========================
clean:
	rm -rf $(BUILD) $(IMG_DIR)
