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

BPB_SIZE  = 62

# ===========================
#  Verzeichnisse
# ===========================
SRC_STAGE2 = src/boot/stage_2
SRC_STAGE1 = src/boot/stage_1
BUILD      = build
IMG_DIR    = img

STAGE1_DIR = $(BUILD)/boot/stage_1
STAGE2_DIR = $(BUILD)/boot/stage_2

# ===========================
#  Stage-2 Quellen
#  (alle .c und .asm rekursiv)
# ===========================
C_SRC   = $(shell find $(SRC_STAGE2) -type f -name "*.c")
ASM_SRC = $(shell find $(SRC_STAGE2) -type f \( -name "*.asm" -o -name "*.ASM" -o -name "*.s" -o -name "*.S" \))


# Automatisch zugehörige Objektdateien erzeugen
C_OBJ   = $(C_SRC:$(SRC_STAGE2)/%.c=$(STAGE2_DIR)/%.o)
ASM_OBJ = $(ASM_SRC:$(SRC_STAGE2)/%.asm=$(STAGE2_DIR)/%.o)

STAGE2_OBJ = $(C_OBJ) $(ASM_OBJ)
STAGE2_ELF = $(STAGE2_DIR)/stage2.elf
STAGE2_BIN = $(STAGE2_DIR)/stage2.bin

# ===========================
#  Ziel
# ===========================
IMG = $(IMG_DIR)/floppy.img

.PHONY: all clean stage1 stage2 image

all: $(IMG)

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

$(STAGE2_BIN): $(STAGE2_OBJ) linker.ld
	mkdir -p $(STAGE2_DIR)
	$(LD) -T linker.ld -o $(STAGE2_ELF) $(STAGE2_OBJ)
	$(OBJCOPY) -O binary $(STAGE2_ELF) $(STAGE2_BIN)

# ===========================
#  Stage-1 Bootloader bauen
# ===========================
stage1: $(STAGE1_DIR)/boot.bin

$(STAGE1_DIR)/boot.bin: $(SRC_STAGE1)/boot.asm
	mkdir -p $(STAGE1_DIR)
	$(AS) $(NASMFLAGS) -o $@ $<

# ===========================
#  FAT12 Image erzeugen
# ===========================
image: $(IMG)

$(IMG): $(STAGE1_DIR)/boot.bin $(STAGE2_BIN)
	mkdir -p $(IMG_DIR) $(BUILD)/boot

	dd if=/dev/zero of=$(IMG) bs=512 count=2880
	mkfs.fat -F 12 $(IMG)

	# BPB kopieren, Bootloader einbetten
	dd if=$(IMG) of=$(BUILD)/boot/bpb.bin bs=1 count=$(BPB_SIZE)
	cat $(BUILD)/boot/bpb.bin $(STAGE1_DIR)/boot.bin > $(BUILD)/boot/boot_sector.tmp
	truncate -s 510 $(BUILD)/boot/boot_sector.tmp
	printf "\x55\xAA" >> $(BUILD)/boot/boot_sector.tmp
	dd if=$(BUILD)/boot/boot_sector.tmp of=$(IMG) conv=notrunc bs=512 count=1

	# Stage-2 als Datei "entry.bin" auf Diskette
	mcopy -i $(IMG) $(STAGE2_BIN) ::entry.bin


HDD_IMG = $(IMG_DIR)/harddisk.img
EMPTY_KERNEL = $(BUILD)/kernel.o

.PHONY: harddisk

# Leere kernel.o erzeugen
$(EMPTY_KERNEL):
	mkdir -p $(BUILD)
	touch $(EMPTY_KERNEL)

# Harddisk-Image erzeugen
harddisk: $(EMPTY_KERNEL)
	mkdir -p $(IMG_DIR)

	# 10 MB leeres HDD-Image
	dd if=/dev/zero of=$(HDD_IMG) bs=1M count=128

	# FAT16 oder FAT32 möglich – hier FAT16
	mkfs.fat -F 16 $(HDD_IMG)

	# kernel.o in Root Directory der Festplatte ablegen
	mcopy -i $(HDD_IMG) $(EMPTY_KERNEL) ::kernel.o

# ===========================
#  Reinigung
# ===========================
clean:
	rm -rf $(BUILD) $(IMG)
# ===========================
#  Compiler / Tools
# ===========================
