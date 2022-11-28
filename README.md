# GeneOS
GeneOS is a simple IA-32 amateur operating system. It is developed for educational purposes. 
As for now, the following features are implemented:

1. Physical memory manager (uses bitmap allocator)
2. Segmentation is not used. Only 6 segments are defined, 2 for userspace data & code, 2 for kernel space data & code, and another 2 TSS segments
3. Virtual memory manager (the kernel is in the higher half, at 3GB linear address)
4. Simple keyboard and VGA drivers
5. Simple kshell to interact with the kernel
6. kmalloc & free routines (kernel heap is implemented)
7. Kpanic (the double fault is handled by stack switching via TSS)
8. ATA PIO driver
9. FAT32 driver (for now only reading facilities are implemented)
10. DMA driver
11. PCI enumeration
12. PIC & PIT handling
13. Some simple parts of the VFS (mounted filesystems are assigned letters from A to Z much like in DOS)

The kernel is coded in C and x86 assembly (I use NASM syntax). I have decided to not write my bootloader, 
so I use GRUB to boot the kernel up. The kernel is compiled into i386 elf binary.
The following features are expected to be implemented:

1. User-mode space
2. Syscalls interface
3. Multitasking
4. ATA DMA driver
5. Full FAT32 support for MBR disks

## Obtaining the cross-compiler

To compile this code, you should obtain a GCC cross-compiler and Binutils. Set the target platform to i686-elf.
After installation, ensure your compiler's location is included in the PATH variable.
Inside the Makefile, you may find the variables which define your compiler (CC), linker (LD), 
objcopy binutil (OBJCOPY), and NASM assembler (AS) binaries. Set them as you desire if their names differ from the default ones.
If you don't understand the process of compiling your custom cross-compiler, follow this [guide](https://wiki.osdev.org/GCC_Cross-Compiler).

## Compilation process

If you wish to create an iso image, ensure you have a **genisoimage** utility in your system. Most likely 
you don't have it (especially if you are a Windows user) so use some other tool to create a bootable drive
(for example, you may install the GRUB bootloader and then configure it to load the newly created elf binary). 
Execute the following commands in your command interpreter:

```bash
make all
make os.iso
```

The target **all** generates an elf binary and places it into the build directory (which is created inside the root
directory of the repository). Additionally, it generates the kernel.sym file, which includes debugging symbols 
(this file can be used in the Bochs debugger). This target also creates the directory hierarchy to be used in the next step. 

The target **os.iso** generates an iso image that you may burn onto an optical disc. You may also use this image to 
start a virtual machine. Beware that since this is a very simple 32-bit operating system, it can run only on x86-compatible hardware in legacy BIOS mode.
The following hierarchy is created inside the build directory upon execution of the above commands:

```bash
├── iso
│   └── boot
│       ├── grub
│       │   ├── menu.lst
│       │   └── stage2_eltorito
│       └── kernel.elf
├── kernel.elf
├── kernel.sym
└── os.iso
```

**os.iso** is created only in case you execute the **os.iso** target. 
**menu.lst** is the configuration file used by GRUB to create a bootloader menu, and **stage2_eltorito** is the bootloader binary. 
These files are copied during the compilation process from the **resources** directory.  

The **clean** target deletes the build directory and all generated object files:

```bash
make clean
```

