OUTPUT_FORMAT("elf64-x86-64", "elf64-x86-64", "elf64-x86-64")
OUTPUT_ARCH(i386:x86-64)

SECTIONS
{
	. = 0xffff800000100000;
	.text :
	{
		*(.text.header)
		*(.text)
	}

	.rodata :
	{
		*(.rodata)
	}

	.data :
	{
		*(.data)
	}

	.bss   :
	{
		*(.bss)
	}
	
	img_end = .;
}
