# Installing instructions

## Dependencies
TODO.

## Building
First, you need to clone gnu-efi: `make -C bootloader gnu-efi-clone`
After that, compile the actual OS: `make`
It will automatically create a disk image at `build/disk.img`
Finally, to run using QEMU: `make run`

