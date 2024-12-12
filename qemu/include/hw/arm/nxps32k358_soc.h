/*
 * NXPS32K358 SoC
 *
 * Copyright (c) 2021 Alexandre Iooss <erdnaxe@crans.org>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef HW_ARM_NXPS32K358_SOC_H
#define HW_ARM_NXPS32K358_SOC_H

#include "hw/arm/armv7m.h"
#include "hw/clock.h"
#include "qom/object.h"
#include "hw/char/nxps32k358_lpuart.h"

#define TYPE_NXPS32K358_SOC "nxps32k358-soc"
OBJECT_DECLARE_SIMPLE_TYPE(NXPS32K358State, NXPS32K358_SOC)

#define CODE_FLASH_BASE_ADDRESS 0x00400000
#define CODE_FLASH_BLOCK_SIZE (2 * 1024 * 1024)
#define DATA_FLASH_BASE_ADDRESS 0x10000000
#define DATA_FLASH_SIZE (128 * 1024)
#define SRAM_BASE_ADDRESS 0x20400000
#define SRAM_BLOCK_SIZE (256 * 1024)
#define DTCM_BASE_ADDRESS 0x20000000
#define DTCM_SIZE (128 * 1024) + 1
#define ITCM_BASE_ADDRESS 0x00000000
#define ITCM_SIZE (64 * 1024)

struct NXPS32K358State {
    SysBusDevice parent_obj;

    ARMv7MState armv7m;

    MemoryRegion code_flash_0;
    MemoryRegion code_flash_1;
    MemoryRegion code_flash_2;
    MemoryRegion code_flash_3;

    MemoryRegion data_flash;

    MemoryRegion sram_0;
    MemoryRegion sram_1;
    MemoryRegion sram_2;

    MemoryRegion dtcm;
    MemoryRegion itcm;

    MemoryRegion tmp;

    NXPS32K35LPUartState lpuart[NUM_LPUARTS];

    Clock *sysclk;
    Clock *refclk;
};

typedef struct NXPS32K358State NXPS32K358State;

#endif
