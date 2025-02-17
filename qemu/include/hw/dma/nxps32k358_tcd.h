/*
 * NXPS32K358 EDMA TCD
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
 * @file nxps32k358_tcd.h
 * @brief Definition of the NXP S32K358 eDMA TCD (Transfer Control Descriptor).
 */

#ifndef NXP_S32K358_EDMA_TCD_H
#define NXP_S32K358_EDMA_TCD_H

#include "hw/registerfields.h"

REG32(CH_CSR, 0x0)
FIELD(CH_CSR, EEI, 2, 1)
FIELD(CH_CSR, DONE, 30, 1)
FIELD(CH_CSR, ACTIVE, 31, 1)

REG32(CH_ES, 0x4)
FIELD(CH_ES, ERR, 31, 1)

REG32(CH_INT, 0x8)
FIELD(CH_INT, INT, 0, 1)

REG32(CH_SBR, 0xC)

REG32(CH_PRI, 0x10)

REG32(TCD_SADDR, 0x20)
REG16(TCD_SOFF, 0x24)

REG16(TCD_ATTR, 0x26)
FIELD(TCD_ATTR, SSIZE, 8, 3)
FIELD(TCD_ATTR, DSIZE, 0, 3)

// No support for minor loop offset enable
REG32(TCD_NBYTES_MLOFF, 0x28)
FIELD(TCD_NBYTES_MLOFF, NBYTES, 0, 30)
FIELD(TCD_NBYTES_MLOFF, DMLOE, 30, 1)
FIELD(TCD_NBYTES_MLOFF, SMLOE, 31, 1)

REG32(TCD_SLAST_SDA, 0x2C)
FIELD(TCD_SLAST_SDA, SLAST_SDA, 0, 32)

REG32(TCD_DADDR, 0x30)
REG16(TCD_DOFF, 0x34)

REG16(TCD_CITER, 0x36)
FIELD(TCD_CITER, ELINK, 15, 1)
FIELD(TCD_CITER, CITER, 0, 15)

REG32(TCD_DLAST_SGA, 0x38)
FIELD(TCD_DLAST_SGA, DLAST_SGA, 0, 32)

REG16(TCD_CSR, 0x3C)
FIELD(TCD_CSR, START, 0, 1)
FIELD(TCD_CSR, INTMAJOR, 1, 1)
FIELD(TCD_CSR, INTHALF, 2, 1)
FIELD(TCD_CSR, ESG, 4, 1)
FIELD(TCD_CSR, ESDA, 7, 1)
FIELD(TCD_CSR, MAJORELINK, 5, 1)

REG16(TCD_BITER, 0x3E)
FIELD(TCD_BITER, ELINK, 15, 1)
FIELD(TCD_BITER, BITER, 0, 15)

/**
 * @struct NXPS32K358EDMATCDState
 * @brief Represents the state of an NXP S32K358 eDMA TCD (Transfer Control
 * Descriptor).
 *
 * @note This implementation does not support minor loop offset enable and
 * channel linking.
 *
 * @var NXPS32K358EDMATCDState::ch_csr
 * Channel Control and Status register.
 *
 * @var NXPS32K358EDMATCDState::ch_es
 * Channel Error Status register.
 *
 * @var NXPS32K358EDMATCDState::ch_int
 * Channel Interrupt Status register.
 *
 * @var NXPS32K358EDMATCDState::ch_sbr
 * Channel System Bus register.
 *
 * @var NXPS32K358EDMATCDState::ch_pri
 * Channel Priority register.
 *
 * @var NXPS32K358EDMATCDState::tcd_saddr
 * TCD Source Address.
 *
 * @var NXPS32K358EDMATCDState::tcd_soff
 * TCD Signed Source Address Offset.
 *
 * @var NXPS32K358EDMATCDState::tcd_attr
 * TCD Transfer Attributes.
 *
 * @var NXPS32K358EDMATCDState::tcd_nbytes_mloff
 * TCD Minor Byte Count with Minor Loop Offset.
 *
 * @var NXPS32K358EDMATCDState::tcd_slast_sda
 * TCD Last Source Address Adjustment or Scatter Gather Address.
 *
 * @var NXPS32K358EDMATCDState::tcd_daddr
 * TCD Destination Address.
 *
 * @var NXPS32K358EDMATCDState::tcd_doff
 * TCD Signed Destination Address Offset.
 *
 * @var NXPS32K358EDMATCDState::tcd_citer
 * TCD Current Minor Loop Link, Major Loop Count (Channel Linking Disabled).
 *
 * @var NXPS32K358EDMATCDState::tcd_dlast_sga
 * TCD Last Destination Address Adjustment or Scatter Gather Address.
 *
 * @var NXPS32K358EDMATCDState::tcd_csr
 * TCD Control and Status.
 *
 * @var NXPS32K358EDMATCDState::tcd_biter
 * TCD Beginning Minor Loop Link, Major Loop Count (Channel Linking Disabled).
 *
 * @var NXPS32K358EDMATCDState::irq
 * QEMU IRQ object for handling interrupts.
 *
 */
struct NXPS32K358EDMATCDState {
    uint32_t ch_csr;
    uint32_t ch_es;
    uint32_t ch_int;
    uint32_t ch_sbr;
    uint32_t ch_pri;
    uint32_t tcd_saddr;
    int16_t tcd_soff;
    uint16_t tcd_attr;
    uint32_t tcd_nbytes_mloff;
    uint32_t tcd_slast_sda;
    uint32_t tcd_daddr;
    uint16_t tcd_doff;
    uint16_t tcd_citer;
    uint32_t tcd_dlast_sga;
    uint16_t tcd_csr;
    uint16_t tcd_biter;

    qemu_irq irq;
};

#endif
