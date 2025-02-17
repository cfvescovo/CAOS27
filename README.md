# QEMU Support for NXP S32K3X8EVB-Q289

## Repo structure
In this repository, there are three main folders:
- the `qemu` folder, containing the Qemu emulation part;
- the `FreeRTOS_Demo` folder, containing a FreeRTOS Demo project created with NXP S32 Design Studio and demonstrating usage/implementation of the following:
	- FreeRTOS and its features, like task scheduling and semaphores;
	- LPUART communication, with baud rate customization;
	- DMA operation, with interrupts as showcased by the implementation of an ISR;
	- MPU support, implicitly demonstrated by defining MPU_ENABLE in the project;
- the `docs` folder, containing materials provided by NXP in its own subfolder and a presentation written using Markdown (CommonMark dialect + Marp) and exported as a PDF file. In addition, if you run Doxygen following the instructions in Part 3 of this README, documentation for the project will be created inside the `docs/qemu` folder.

## Part 1: Qemu emulation

QEMU is a powerful and open source emulator and virtualizer designed to meet a wide range of use cases and it supports many different settings and configurations. By leveraging dynamic translation, it delivers high performance and flexibility for emulating complete systems or specific userspace environments. The target board is the NXP S32K3X8EVB, that was not supported on QEMU.

### Full Machine Emulation
QEMU can emulate an entire machine in software without requiring hardware virtualization support. This enables running operating systems and applications designed for one hardware architecture (e.g., ARMv7) on a completely different architecture.
     
### Building instructions

The simple steps, on Linux, to build QEMU, are:

```shell
  mkdir build
  cd build
  ../configure --enable-debug  --target-list=arm-softmmu
  make -j N
```
Notice the configure command has two parameters set: 
- `--enable-debug` enables debug symbols useful during the debug of QEMU executable
-  `--target-list=arm-softmmu` to compile only ARM-softmmu boards (reduces compilation time)
The N parameter in make command sets the number of threads used during compilation and should be replaced according to the specs of the machine used to compile the project.
  
### Running instructions
We run QEMU when we debug our project with the parameters:
```shell
./qemu-system-arm -M nxps32k3x8evb -nographic -kernel kernel.elf -serial none -serial none -serial none -serial mon:stdio -d guest_errors
```
#### Explanation of parameters
- `-M nxps32k3x8evb` sets the board to use for the emulation. We named our board `nxps32k3x8evb`
- `-nographic` disables the graphical display output
- `-kernel kernel.elf` indicates the binary file (kernel.elf) to load as the kernel
-  `-serial none` (three times) and then `-serial mon:stdio` since there are 16 LPUART interfaces and we are using LPUART 3 (the fourth) in our DEMO project, we disable the first three and then bind the fourth to STDIO
-   `-d guest_errors` Enables debug logs for specific categories:
  1. `guest_errors`: Logs errors occurring in the emulated guest system.
  2. Optionally, during development, we also set `unimp`: it logs unimplemented functionality in the emulated machine. This is useful for identifying missing or unsupported features in the emulation as we created specific, unimplemented devices for every peripheral in the board.

## Part 2: Demo firmware

### Compiling the FreeRTOS_Demo project
To compile our `FreeRTOS_Demo` project you need the following software:
- S32 Design Studio, version 3.5
- S32K3 RTD/AUTOSAR, R21-11 version 3.0.0
- S32K3XX RTD/AUTOSAR R21-11, version 3.0.0
- S32K3XX development package, version 3.5.13
- FreeRTOS extension for S32K3, version 3.1.0

Import the project from our repo in S32 Design Studio, right click on it and build the project. Sometimes, you need to go to peripherals panel and click on `Update Code` before compiling. An ELF file will be created in the Debug_FLASH folder.

### Launching the firmware
From the build folder, run this command: `./qemu-system-arm -M nxps32k3x8evb -nographic -kernel ../../FreeRTOS_Demo/Debug_FLASH/FreeRTOS_Demo.elf -serial none -serial none -serial none -serial mon:stdio -d guest_errors`, optionally without `-d guest_errors`. You will see output from LPUART3 on your console.

## Part 3: Documentation
We used `doxygen` as our documentation tool. If you are on Debian (or its derivatives), install it with `sudo apt install doxygen`. In addition, install graphviz with `sudo apt install graphviz`.
Now you can `cd` into the `qemu` folder and run `doxygen` (without any argument). The program will create some files in the `docs/qemu` folder. If you have python3 installed, you can serve the generated HTML files using the following command: `python3 -m http.server -d ../docs/qemu/html/`. Navigate to [localhost:8000](http://localhost:8000) using your browser of choice to explore the documentation.
