all: blue-kernel vmlinuz

blue-kernel:
	make -C ../kernel
	
vmlinuz:
	objdump -D ../kernel/kernel.elf > ../kernel/kernel.dump
	cat grub_header ../kernel/kernel.bin > vmlinuz-0-blue

clean:
	make -C ../kernel clean
	rm -f vmlinuz-0-blue *.tmp *.dump
