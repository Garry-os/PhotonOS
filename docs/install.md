# Installation

## Building environment
Building on a Unix-like system like Linux is recommended. <br>
Although on MacOS, it isn't supported since loopback device isn't avaliable, I recommended using a virtual machine or even docker.
On Windows, it is obviously not Unix-like. I recommended using WSL2 (Windows subsystem for Linux) or even a virtual machine.

## Dependencies
- GNU make
- nasm
- parted
- git
- mkfs (dosfstools)
- A cross compiler (x86-64)

## Compile
To compile, simply run: `make`
## Run
To run using QEMU: `make run`

## Real hardware
For booting on real hardware, you can copy the disk image to a flash drive or something by: `dd if=build/disk.img of=/dev/sdX`
**IMPORTANT: ** Replace /dev/sdX with your actual drive. <br>
If you accidentally did it wrong, the data from that disk will all be erased! <br>

Although, booting on real hardware isn't guarenteed to work.

