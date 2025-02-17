/*
 * NXPS32K358 EDMA
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
 * @file nxps32k358_edma.h
 * @brief Definition of the NXPS32K358 eDMA module.
 */

#ifndef NXP_S32K358_EDMA_H
#define NXP_S32K358_EDMA_H

#include "hw/sysbus.h"
#include "qom/object.h"
#include "hw/registerfields.h"
#include "hw/dma/nxps32k358_tcd.h"

#define READONLY

REG32(EDMA_CSR, 0x00)
FIELD(EDMA_CSR, EDBG, 1, 1)
FIELD(EDMA_CSR, ERCA, 2, 1)
FIELD(EDMA_CSR, HALT, 5, 1)
FIELD(EDMA_CSR, ECX, 8, 1)
FIELD(EDMA_CSR, CX, 9, 1)
FIELD(EDMA_CSR, ACTIVE_ID, 24, 5)
FIELD(EDMA_CSR, ACTIVE, 31, 1)

REG32(EDMA_ES, 0x04)
FIELD(EDMA_ES, ECX, 8, 1)
FIELD(EDMA_ES, ERRCHN, 24, 5)
FIELD(EDMA_ES, VLD, 31, 1)

REG32(EDMA_INT, 0x08)
FIELD(EDMA_INT, INT, 0, 32)

REG32(EDMA_HRS, 0x0C)
FIELD(EDMA_HRS, HRS, 0, 32)

#define EDMA_CHN_GRPRI_BASE_ADDR 0x100
#define EDMA_CHN_GRPRI_NUM 32

static inline uint32_t A_EDMA_CHN_GRPRI(int n) {
    return EDMA_CHN_GRPRI_BASE_ADDR + 4 * n;
}
static inline uint32_t R_EDMA_CHN_GRPRI(int n) {
    return EDMA_CHN_GRPRI_BASE_ADDR / 4 + n;
}
static inline uint32_t R_EDMA_CHN_GRPRI_GRPRI_MASK(int n) {
    return MAKE_64BIT_MASK(0, 5);
}

#define EDMA_CSR_RESET 0x00300000
#define EDMA_ES_RESET 0x00000000
#define EDMA_INT_RESET 0x00000000
#define EDMA_HRS_RESET 0x00000000
#define EDMA_CHN_GRPRI_RESET 0x00000000

#define EDMA_CHANNELS 32

#define TYPE_NXPS32K358_EDMA "nxps32k358-edma"
OBJECT_DECLARE_SIMPLE_TYPE(NXPS32K358EDMAState, NXPS32K358_EDMA)

/**
 * @struct NXPS32K358EDMAState
 * @brief Represents the state of the NXP S32K358 eDMA controller.
 *
 * This structure holds the state information for the NXP S32K358 eDMA
 * controller, including memory-mapped I/O regions, control and status
 * registers, error status, interrupt status, hardware request status, channel
 * group priority, and transfer control descriptors.
 *
 * @var NXPS32K358EDMAState::parent_obj
 * The parent system bus device.
 *
 * @var NXPS32K358EDMAState::mmio0
 * Memory-mapped I/O region 0.
 *
 * @var NXPS32K358EDMAState::mmio12
 * Memory-mapped I/O region 12.
 *
 * @var NXPS32K358EDMAState::edma_csr
 * Control and status register for the eDMA.
 *
 * @var NXPS32K358EDMAState::edma_es
 * Error status register (read-only).
 *
 * @var NXPS32K358EDMAState::edma_int
 * Interrupt status register (read-only).
 *
 * @var NXPS32K358EDMAState::edma_hrs
 * Hardware request status register (read-only).
 *
 * @var NXPS32K358EDMAState::edma_chn_grpri
 * Channel group priority registers.
 *
 * @var NXPS32K358EDMAState::tcd
 * Transfer control descriptors for each eDMA channel.
 */
struct NXPS32K358EDMAState {
    SysBusDevice parent_obj;
    MemoryRegion mmio0;
    MemoryRegion mmio12;

    uint32_t edma_csr;
    uint32_t READONLY edma_es;
    uint32_t READONLY edma_int;
    uint32_t READONLY edma_hrs;
    uint32_t edma_chn_grpri[EDMA_CHANNELS];
    struct NXPS32K358EDMATCDState tcd[EDMA_CHANNELS];
};

#endif
