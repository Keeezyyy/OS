# qemu-system-i386 -fda img/floppy.img

#qemu-system-x86_64 \
#    -monitor stdio \
#    -fda img/floppy.img


qemu-system-x86_64 \
    -monitor stdio \
    -fda img/floppy.img \
    -drive file=img/harddisk.img,format=raw,if=ide \
    -boot a

rm -rf ram.bin
#dump-guest-memory ram.bin