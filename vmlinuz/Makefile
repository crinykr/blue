PFX = /Users/criny/Desktop/os_dev_tools/root/bin/x86_64-pc-linux-

all: blue-kernel vmlinuz

blue-kernel:
	make -C ../kernel
	
vmlinuz:
	$(PFX)objdump -D ../kernel/kernel.elf > ../kernel/kernel.dump
	cat grub_header ../kernel/kernel.bin > vmlinuz-0-blue

clean:
	make -C ../kernel clean
	rm -f vmlinuz-0-blue *.tmp *.dump
