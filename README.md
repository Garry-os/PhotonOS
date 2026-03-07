# PhotonOS

## General
A small x86-64 OS (kernel), currently, it is under development. <br>
These are a few key1 features of the kernel:
- Pre-emptive multitasking kernel
- x86-64 architectures features almost fully supported (GDT, IDT, I/O, ISR).
- Memory management (Including PMM, VMM, Heap).
- PCI bus enumeration (via 32 bit CPU I/O)
- Partition-seperated, disk block devices (e.g /dev/sda, /dev/sda1).
- FAT32 and VFS driver
- PS/2 keyboard driver

## Installation
For installation guide, go to `docs/install.md`.

## Credits
Limine: a modern, advanced, portable, multiprotocol bootloader and boot manager

## License
This project is licensed under MIT license.

