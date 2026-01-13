# Installing instructions

## Dependencies
- MINGW toolchain
- GNU make
- QEMU (For emulator)
- wget

## Compile & run
To compile, run: `make`
The output will be in the `build/disk.img`

Before running using QEMU, you should get the OVMFbin prebuilts by:
`make ovmf-prebuilts`
After than, you can run: `make run`

