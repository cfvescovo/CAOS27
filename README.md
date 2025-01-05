# progetto
## Part 1: Qemu emulation

QEMU is a powerful and open source emulator and virtualizer designed to meet a wide range of use cases and it supports many different settings and configurations. By leveraging dynamic translation, it delivers high performance and flexibility for emulating complete systems or specific userspace environments. The target board is the NXP S32K3X8EVB, that was not supported on QEMU.

### Full Machine Emulation
QEMU can emulate an entire machine in software without requiring hardware virtualization support. This enables running operating systems and applications designed for one hardware architecture (e.g., ARMv7) on a completely different architecture.

### Documentation
Documentation can be found hosted online at https://www.qemu.org/documentation/.
     
### Building instructions

The simple steps, on Linux, to build QEMU are:

```shell
  mkdir build
  cd build
  ../configure --enable-debug  --target-list=arm-softmmu
  make -j N
```
Notice the configure command has two parameters set: 
- `--enable-debug` enables debug symbols useful during the debug of QEMU executable
-  `--target-list=arm-softmmu` to compile only ARM-softmmu boards (reduces compilation time)
The N parameter in make command sets the number of threads used during compilation.
  
### Running instructions
We run QEMU when we debug our project with the parameters:
```shell
./qemu-system-arm -M nxps32k3x8evb -nographic -kernel kernel.elf -serial none -serial none -serial none -serial mon:stdio -d guest_errors,unimp
```
#### Explanation of parameters
- `-M nxps32k3x8evb` sets the board to use for the emulation. We named our board `nxps32k3x8evb`
- `-nographic` disables the graphical display output
- `-kernel kernel.elf` indicates the binary file (kernel.elf) to load as the kernel
-  `-serial none` (three times) and then `-serial mon:stdio` since there are 16 LPUART interfaces and we are using LPUART 3 (the fourth) in our DEMO project, we disable the first three and then bind the fourth to STDIO
-   `-d guest_errors,unimp` Enables debug logs for specific categories:
  1. `guest_errors`: Logs errors occurring in the emulated guest system.
  2. `unimp`: Logs unimplemented functionality in the emulated machine. This is useful for identifying missing or unsupported features in the emulation.

### Compiling the DEMO project
To compile our FreeRTOS DEMO project you need the following software:
- S32 Design Studio, version 3.5
- S32K3 RTD/AUTOSAR, R21-11 version 3.0.0
- S32K3XX RTD/AUTOSAR R21-11, version 3.0.0
- S32K3XX development package, version 3.5.13
- FreeRTOS extension for S32K3, version 3.1.0
Import the project from our repo in S32 Design Studio, right click on it and build the project. Sometimes, you need to go to peripherals panel and click on `Update Code` before compiling.

### Adding NXP S32K3X8EVB board

To add support for NXP S32K3X8EVB board in QEMU, you need to define a new System-on-Chip (SoC) model. This involves the following steps:

#### Create the SoC Model:

Write a new source file in the hw/arm directory (hw/arm/nxps32k358_soc.c) and the related header file in include/hw/arm.
Define the peripherals, memory map, and CPU for your SoC.

#### Define the Board:

Implement the board model in the hw/arm/ directory (hw/arm/nxps32k3x8evb.c).
Connect the SoC model to the board, specifying how peripherals are mapped and initialized.

#### Update Build System and KConfig:
Add your new files to the appropriate meson.build and KConfig files to ensure they are compiled.

(CONTROLLARE)
(ricordarsi di menzionare la periferica fittizia nel soc che restituisce 1 quando si arriva a quell'indirizzo)
