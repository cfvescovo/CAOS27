/*
 * NXPS32K358 SoC
 *
 * Copyright (c) 2024-2025 CAOS group 27: C. F. Vescovo, C. Sanna, F. Stella
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

/**
 * @file nxps32k358_soc.h
 * @brief Definition of the NXP S32K358 SoC.
 */

#ifndef HW_ARM_NXPS32K358_SOC_H
#define HW_ARM_NXPS32K358_SOC_H

#include "hw/arm/armv7m.h"
#include "hw/clock.h"
#include "qom/object.h"
#include "hw/char/nxps32k358_lpuart.h"
#include "hw/dma/nxps32k358_edma.h"

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
#define MC_ME_BASE_ADDRESS 0x402DC000
#define MC_ME_SIZE 1340

static inline uint32_t LPUART_ADDR(int n) { return 0x40328000 + 0x4000 * n; }
static inline uint32_t LPUART_IRQ(int n) { return 141 + n; }
#define NUM_LPUARTS 16

#define EDMA_BASE_ADDRESS 0x4020C000
static inline uint32_t EDMA_IRQ(int n) { return 4 + n; }
#define NUM_EDMA_CHANNELS 32

/**
 * @struct NXPS32K358State
 * @brief Represents the state of the NXP S32K358 SoC.
 *
 * @var NXPS32K358State::parent_obj
 * The parent system bus device.
 *
 * @var NXPS32K358State::armv7m
 * The ARMv7-M CPU state.
 *
 * @var NXPS32K358State::code_flash_0
 * Memory region for the first code flash.
 *
 * @var NXPS32K358State::code_flash_1
 * Memory region for the second code flash.
 *
 * @var NXPS32K358State::code_flash_2
 * Memory region for the third code flash.
 *
 * @var NXPS32K358State::code_flash_3
 * Memory region for the fourth code flash.
 *
 * @var NXPS32K358State::data_flash
 * Memory region for the data flash.
 *
 * @var NXPS32K358State::sram_0
 * Memory region for the first SRAM.
 *
 * @var NXPS32K358State::sram_1
 * Memory region for the second SRAM.
 *
 * @var NXPS32K358State::sram_2
 * Memory region for the third SRAM.
 *
 * @var NXPS32K358State::dtcm
 * Memory region for the Data Tightly Coupled Memory.
 *
 * @var NXPS32K358State::itcm
 * Memory region for the Instruction Tightly Coupled Memory.
 *
 * @var NXPS32K358State::mc_me
 * Memory region for the Mode Entry module.
 * @note This module is not really implemented, it just returns a magic value
 * when reading at offset 0x310.
 *
 * @var NXPS32K358State::lpuart
 * Array of LPUART states.
 *
 * @var NXPS32K358State::edma
 * The eDMA state.
 *
 * @var NXPS32K358State::sysclk
 * System clock.
 *
 * @var NXPS32K358State::refclk
 * Reference clock.
 *
 * @var NXPS32K358State::aips_plat_clk
 * Clock used by LPUART channels 0, 1, and 8 (80MHz).
 *
 * @var NXPS32K358State::aips_slow_clk
 * Clock used by other LPUART channels (40MHz).
 */
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

    MemoryRegion mc_me;

    NXPS32K358LPUartState lpuart[NUM_LPUARTS];
    NXPS32K358EDMAState edma;

    Clock *sysclk;
    Clock *refclk;

    Clock *aips_plat_clk;
    Clock *aips_slow_clk;
};

typedef struct NXPS32K358State NXPS32K358State;

#endif
