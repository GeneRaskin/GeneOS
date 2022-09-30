ENTRY(loader)

SECTIONS {
	. = 0x00100000; /* the code should be loaded at 1 MB */

	.__mbHeader ALIGN (0x1000):
	{
		*(.__mbHeader)
	}

	.__kernelStack ALIGN (0x1000):
	{
	    *(.__kernelStack)
	}

	.__interruptStack ALIGN (0x1000):
	{
	    *(.__interruptStack)
	}

	.text ALIGN (0x1000) :
	{
		*(.text)
	}

	.rodata ALIGN (0x1000) :
	{
		*(.rodata*)
	}

	.data ALIGN (0x1000) :
	{
		*(.data)
	}

	.bss ALIGN (0x1000) :
	{
		*(COMMON)
		*(.bss)
	}

}
