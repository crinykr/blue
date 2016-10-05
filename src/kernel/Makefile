# Set build utils
AS = nasm -f elf64
CC = gcc -w -m64 -ffreestanding -mcmodel=large -std=c99 -Icpu -Idevice -Ids -Ifs -Igen -Imem -Imisc -Isys -Iinit
LD = ld -melf_x86_64 -T blue.ld -nostdlib
OBJCOPY = objcopy -j .text -j .data -j .rodata -j .bss --set-section-flags .bss=alloc,load,contents -S -O binary

# All of the core parts of the kernel are built directly.
KERNEL_OBJS  = $(patsubst %.c,%.o,$(wildcard ./*/*.c))
KERNEL_OBJS += $(patsubst %.s,%.o,$(wildcard ./*/*.s))

all: generated kernel

generated:
	nasm init/jumper.asm -o init/jumper.bin
	xxd -i init/jumper.bin > gen/jumper.h

kernel: $(KERNEL_OBJS)
	$(LD) -o kernel.elf $^
	$(OBJCOPY) kernel.elf kernel.bin

%.o: %.c
	$(CC) -c -o $@ $<

%.o: %.s
	$(AS) -o $@ $<
	
clean:
	rm -f */*.o */*.bin
	rm -f *.bin *.elf *.dump
