/*
 * NXPS32K358 eDMA
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
 * @file nxps32k358_edma.c
 * @brief Implementation of the NXPS32K358 eDMA module.
 */

#include "qemu/osdep.h"
#include "qapi/error.h"
#include "hw/dma/nxps32k358_edma.h"
#include "hw/dma/nxps32k358_tcd.h"
#include "hw/irq.h"
#include "migration/vmstate.h"
#include "qemu/log.h"
#include "qemu/module.h"

#define READONLY
#define NO_SUPPORT

#define MAX_SIZE 64

// If NXP_EDMA_DEBUG is 0, no debug messages will be printed
// If it is 1, only write logs will be printed
// If it is 2, read and write logs will be printed
#ifndef NXP_EDMA_DEBUG
#define NXP_EDMA_DEBUG 0
#endif

#define DB_PRINT_L(lvl, fmt, args...)               \
    do {                                            \
        if (NXP_EDMA_DEBUG >= lvl) {                \
            qemu_log("%s: " fmt, __func__, ##args); \
        }                                           \
    } while (0)

#define DB_PRINT(fmt, args...) DB_PRINT_L(1, fmt, ##args)
#define DB_PRINT_READ(fmt, args...) DB_PRINT_L(2, fmt, ##args)

/**
 * @brief Update the interrupt request (IRQ) status for a specific Transfer
 * Control Descriptor (TCD).
 *
 * This function checks the status of the TCD and updates the interrupt request
 * status based on the current state of the TCD's control and iteration
 * registers.
 *
 * @param s Pointer to the NXPS32K358EDMAState structure.
 * @param tcd_no The index of the TCD to update.
 *
 * The function performs the following checks:
 * 1. If the INTHALF bit in the TCD's control and status register (tcd_csr) is
 * set and the current iteration count (tcd_citer) is greater than or equal to
 * half of the beginning iteration count (tcd_biter), the interrupt request is
 * set.
 * 2. If the INTMAJOR bit in the TCD's control and status register (tcd_csr) is
 * set and the current iteration count (tcd_citer) is zero, the interrupt
 * request is set.
 *
 * If any of the above conditions are met, the interrupt request for the TCD is
 * set and the corresponding bit in the edma_int register is updated. The IRQ is
 * then triggered using qemu_set_irq. If none of the conditions are met, the
 * interrupt request is cleared and the IRQ is deasserted.
 *
 * @note Error interrupt (EEI) is not supported.
 */
static void nxps32k358_edma_tcd_update_irq(NXPS32K358EDMAState *s, int tcd_no) {
    struct NXPS32K358EDMATCDState *ch = &s->tcd[tcd_no];

    if (FIELD_EX32(ch->tcd_csr, TCD_CSR, INTHALF) &&
        FIELD_EX16(ch->tcd_citer, TCD_CITER, CITER) >=
            FIELD_EX16(ch->tcd_biter, TCD_BITER, BITER) / 2) {
        ch->ch_int |= R_CH_INT_INT_MASK;
    }

    if (FIELD_EX32(ch->tcd_csr, TCD_CSR, INTMAJOR) &&
        FIELD_EX16(ch->tcd_citer, TCD_CITER, CITER) == 0) {
        ch->ch_int |= R_CH_INT_INT_MASK;
    }

    if (ch->ch_int & R_CH_INT_INT_MASK) {
        s->edma_int |= 1 << tcd_no;
        qemu_set_irq(ch->irq, 1);
    } else {
        s->edma_int &= ~(1 << tcd_no);
        qemu_set_irq(ch->irq, 0);
    }
}

/**
 * @brief Transmits data using the eDMA controller.
 *
 * This function handles the transmission of data using the eDMA controller
 * for the specified Transfer Control Descriptor (TCD). It performs the
 * following steps:
 * - Reads the source and destination addresses and sizes.
 * - Asserts that the source and destination sizes are valid.
 * - Calculates the number of bytes to transfer.
 * - Performs the major loop, which includes:
 *   - Reading data from the source address.
 *   - Writing data to the destination address.
 *   - Updating the source and destination addresses.
 *   - Decrementing the current iteration counter.
 *   - Disabling the ACTIVE flag after the first minor loop is completed.
 *   - Updating the interrupt request.
 * - Handles the completion of the major loop, which includes:
 *   - Writing the source last address adjustment if ESDA is set.
 *   - Writing the destination last address adjustment if ESG is set.
 *   - Resetting the current iteration counter to the beginning iteration count.
 *   - Setting the DONE flag and the interrupt flag.
 *
 * @param s Pointer to the eDMA state structure.
 * @param tcd_no The TCD number to be processed.
 */
static void nxps32k358_edma_transmit(NXPS32K358EDMAState *s, int tcd_no) {
    uint8_t buf[MAX_SIZE];
    struct NXPS32K358EDMATCDState *ch = &s->tcd[tcd_no];

    uint32_t saddr = 0;
    uint32_t daddr = 0;

    uint32_t ssize = FIELD_EX16(ch->tcd_attr, TCD_ATTR, SSIZE);
    uint32_t dsize = FIELD_EX16(ch->tcd_attr, TCD_ATTR, DSIZE);

    // These values are reserved
    assert(ssize != 0b111);
    assert(dsize != 0b111);

    ssize = 1 << ssize;
    dsize = 1 << dsize;

    uint32_t nbytes =
        FIELD_EX32(ch->tcd_nbytes_mloff, TCD_NBYTES_MLOFF, NBYTES);

    uint32_t max_size = ssize > dsize ? ssize : dsize;

    // Major Loop, a new request is needed every time we want to advance
    if (FIELD_EX16(ch->tcd_citer, TCD_CITER, CITER) > 0) {
        saddr = ch->tcd_saddr;
        daddr = ch->tcd_daddr;

        // Minor Loop
        for (int i = 0; i < nbytes / max_size; i++) {
            // Read from source
            for (int j = 0; j < max_size / ssize; j++) {
                cpu_physical_memory_read(saddr, buf, ssize);
                saddr += ch->tcd_soff;
            }

            // Write to destination
            for (int j = 0; j < max_size / dsize; j++) {
                cpu_physical_memory_write(daddr, buf, dsize);
                daddr += ch->tcd_doff;
            }
        }

        // In theory it should be incremented by mloff if smloe/dmloe
        // are active. We write in the registers at the end of the minor loop as
        // stated by the documentation
        ch->tcd_saddr = saddr;
        ch->tcd_daddr = daddr;

        // Decrement current iteration counter
        uint32_t next_citer = FIELD_EX16(ch->tcd_citer, TCD_CITER, CITER) - 1;
        ch->tcd_citer = FIELD_DP16(ch->tcd_citer, TCD_CITER, CITER, next_citer);

        // Disable ACTIVE after first minor loop is completed
        ch->ch_csr &= ~R_CH_CSR_ACTIVE_MASK;

        nxps32k358_edma_tcd_update_irq(s, tcd_no);
    }

    // Major loop completed
    if (FIELD_EX16(ch->tcd_citer, TCD_CITER, CITER) == 0) {
        uint8_t esda = FIELD_EX32(ch->tcd_csr, TCD_CSR, ESDA);
        if (esda) {
            uint32_t slast_sda =
                FIELD_EX32(ch->tcd_slast_sda, TCD_SLAST_SDA, SLAST_SDA);
            cpu_physical_memory_write(
                slast_sda, (void *)(uintptr_t)ch->tcd_daddr, sizeof(slast_sda));
        } else {
            ch->tcd_saddr =
                ch->tcd_saddr +
                FIELD_EX32(ch->tcd_slast_sda, TCD_SLAST_SDA, SLAST_SDA);
        }

        uint8_t esg = FIELD_EX32(ch->tcd_csr, TCD_CSR, ESG);
        if (esg) {
            uint32_t dlast_sga =
                FIELD_EX32(ch->tcd_dlast_sga, TCD_DLAST_SGA, DLAST_SGA);
            uint8_t next_tcd_data[32];
            cpu_physical_memory_read(dlast_sga, next_tcd_data, 32);

            memcpy(&ch->tcd_saddr, next_tcd_data, 32);
        } else {
            ch->tcd_daddr =
                ch->tcd_daddr +
                FIELD_EX32(ch->tcd_dlast_sga, TCD_DLAST_SGA, DLAST_SGA);
        }

        ch->tcd_citer = FIELD_DP16(ch->tcd_citer, TCD_CITER, CITER,
                                   FIELD_EX16(ch->tcd_biter, TCD_BITER, BITER));

        ch->ch_csr |= R_CH_CSR_DONE_MASK;
        ch->ch_int |= R_CH_INT_INT_MASK;
    }
}

/**
 * @brief Perform round-robin arbitration for the eDMA channels.
 *
 * This function implements a basic round-robin arbitration mechanism for the
 * eDMA channels. It iterates through the channels starting from the last
 * offset and checks if the channel's TCD (Transfer Control Descriptor) has
 * the START bit set in its CSR (Control and Status Register). If a channel
 * is found with the START bit set, it clears the DONE bit, clears the START
 * bit, sets the ACTIVE bit, and initiates the transmission for that channel.
 * The offset is then updated to the next channel.
 *
 * @param s Pointer to the NXPS32K358EDMAState structure.
 *
 * @note There is no support for priorities in this implementation.
 */
static void nxps32k358_edma_arbitrate(NXPS32K358EDMAState *s) {
    static int offset = 0;

    // Since there is no support for priorities, we implement a basic
    // round-robin arbitration
    for (int i = 0; i < EDMA_CHANNELS; i++) {
        int j = (i + offset) % EDMA_CHANNELS;
        if (s->tcd[j].tcd_csr & R_TCD_CSR_START_MASK) {
            s->tcd[j].ch_csr &= ~R_CH_CSR_DONE_MASK;
            s->tcd[j].tcd_csr &= ~R_TCD_CSR_START_MASK;
            s->tcd[j].ch_csr |= R_CH_CSR_ACTIVE_MASK;
            nxps32k358_edma_transmit(s, j);
            offset = (j + 1) % EDMA_CHANNELS;
            return;
        }
    }
}

/**
 * @brief Reads a Transfer Control Descriptor (TCD) register value for a given
 * channel.
 *
 * This function reads the value of a specified TCD register for a given channel
 * in the NXPS32K358 EDMA state. The register to be read is determined by the
 * offset parameter.
 *
 * @param s Pointer to the NXPS32K358 EDMA state.
 * @param offset Offset of the TCD register to be read.
 * @param size Size of the read operation (unused in this function).
 * @param c Channel number from which to read the TCD register.
 * @return The value of the specified TCD register.
 */
static uint64_t nxps32k358_edma_tcd_read(NXPS32K358EDMAState *s, hwaddr offset,
                                         unsigned size, unsigned c) {
    uint32_t res = 0;

    struct NXPS32K358EDMATCDState *ch = &s->tcd[c];

    switch (offset) {
        case A_CH_CSR:
            res = ch->ch_csr;
            break;
        case A_CH_ES:
            res = ch->ch_es;
            break;
        case A_CH_INT:
            res = ch->ch_int;
            break;
        case A_CH_SBR:
            res = ch->ch_sbr;
            break;
        case A_CH_PRI:
            res = ch->ch_pri;
            break;
        case A_TCD_SADDR:
            res = ch->tcd_saddr;
            break;
        case A_TCD_SOFF:
            res = ch->tcd_soff;
            break;
        case A_TCD_ATTR:
            res = ch->tcd_attr;
            break;
        case A_TCD_NBYTES_MLOFF:
            res = ch->tcd_nbytes_mloff;
            break;
        case A_TCD_SLAST_SDA:
            res = ch->tcd_slast_sda;
            break;
        case A_TCD_DADDR:
            res = ch->tcd_daddr;
            break;
        case A_TCD_DOFF:
            res = ch->tcd_doff;
            break;
        case A_TCD_CITER:
            res = ch->tcd_citer;
            break;
        case A_TCD_DLAST_SGA:
            res = ch->tcd_dlast_sga;
            break;
        case A_TCD_CSR:
            res = ch->tcd_csr;
            break;
        case A_TCD_BITER:
            res = ch->tcd_biter;
            break;
        default:
            qemu_log_mask(LOG_GUEST_ERROR,
                          "%s: Bad offset 0x%" HWADDR_PRIx "\n", __func__,
                          offset);
            break;
    }

    DB_PRINT_READ("Read 0x%" PRIx32 ", 0x%" HWADDR_PRIx "\n", res, offset);

    return res;
}
/**
 * @brief Write to the Transfer Control Descriptor (TCD) of the eDMA module.
 *
 * This function handles writing to various fields of the TCD for a specific
 * channel in the eDMA module. It updates the appropriate fields based on the
 * provided offset and value.
 *
 * @param s Pointer to the NXPS32K358EDMAState structure.
 * @param offset Offset within the TCD to write to.
 * @param value Value to write to the specified offset.
 * @param size Size of the value being written.
 * @param c Channel number to which the TCD belongs.
 *
 * The function supports writing to all the fields of the TCD, excluding:
 * - A_CH_SBR
 * - A_CH_PRI
 *
 * The function also performs various checks and assertions to ensure the
 * correctness of the written values, such as ensuring that certain bits are
 * not set and that specific fields match expected values, as stated in the
 * documentation.
 *
 * If an unsupported offset is provided, an error message is logged.
 *
 * @note The function does not support channel linking, SMLOE, DMLOE, BWC, the
 * last 4 bits of CH_CSR and probably other features.
 */
static void nxps32k358_edma_tcd_write(NXPS32K358EDMAState *s, hwaddr offset,
                                      uint64_t value, unsigned size,
                                      unsigned c) {
    struct NXPS32K358EDMATCDState *ch = &s->tcd[c];

    DB_PRINT("Write 0x%" PRIx64 ", 0x%" HWADDR_PRIx "\n", value, offset);

    switch (offset) {
        case A_CH_CSR:
            ch->ch_csr &= ~R_CH_CSR_DONE_MASK;
            ch->ch_csr |= value & R_CH_CSR_DONE_MASK;
            // The last 4 bits should be writable too but there is no support
            break;
        case A_CH_ES:
            // SW must be able to clear the error status (first bit)
            ch->ch_es &= ~R_CH_ES_ERR_MASK;
            ch->ch_es |= value & R_CH_ES_ERR_MASK;
            break;
        case A_CH_INT:
            // Write 1 to clear (i.e. disable the interrupt request)
            if (value & R_CH_INT_INT_MASK) ch->ch_int &= ~R_CH_INT_INT_MASK;
            nxps32k358_edma_tcd_update_irq(s, c);
            break;
        case NO_SUPPORT A_CH_SBR:
        case NO_SUPPORT A_CH_PRI:
            break;
        case A_TCD_SADDR:
            ch->tcd_saddr = value;
            break;
        case A_TCD_SOFF:
            ch->tcd_soff = value;
            break;
        case A_TCD_ATTR:
            ch->tcd_attr = value;
            break;
        case A_TCD_NBYTES_MLOFF:
            // No support for SMLOE and DMLOE, we implement MLOFF only
            assert((value & R_TCD_NBYTES_MLOFF_SMLOE_MASK) == 0);
            assert((value & R_TCD_NBYTES_MLOFF_DMLOE_MASK) == 0);
            ch->tcd_nbytes_mloff = value;
            break;
        case A_TCD_SLAST_SDA:
            ch->tcd_slast_sda = value;
            break;
        case A_TCD_DADDR:
            ch->tcd_daddr = value;
            break;
        case A_TCD_DOFF:
            ch->tcd_doff = value;
            break;
        case A_TCD_CITER:
            // No support for channel linking
            assert((value & R_TCD_CITER_ELINK_MASK) == 0);
            // The documentation states that when CITER is written, its value
            // must be equal to BITER
            assert(value == FIELD_EX32(ch->tcd_biter, TCD_BITER, BITER));
            ch->tcd_citer = value;
            break;
        case A_TCD_DLAST_SGA:
            ch->tcd_dlast_sga = value;
            break;
        case A_TCD_CSR:
            // No support for bwc (it would make little sense in an emulated
            // context), don't care. Also, no support for channel linking
            assert((value & R_TCD_CSR_MAJORELINK_MASK) == 0);
            ch->tcd_csr = value;
            // Start request
            if (value & R_TCD_CSR_START_MASK) {
                nxps32k358_edma_arbitrate(s);
            }
            break;
        case A_TCD_BITER:
            // No support for channel linking
            assert((value & R_TCD_BITER_ELINK_MASK) == 0);
            ch->tcd_biter = value;
            break;
        default:
            qemu_log_mask(LOG_GUEST_ERROR,
                          "%s: Bad offset 0x%" HWADDR_PRIx "\n", __func__,
                          offset);
            break;
    }
}

/**
 * @brief Resets the Transfer Control Descriptor (TCD) state of the NXPS32K358
 * eDMA module.
 *
 * This function initializes all the fields of the NXPS32K358EDMATCDState
 * structure to their default values.
 *
 * @param s Pointer to the NXPS32K358EDMATCDState structure to be reset.
 *
 */
static void nxps32k358_edma_tcd_reset(struct NXPS32K358EDMATCDState *s) {
    s->ch_csr = 0;
    s->ch_es = 0;
    s->ch_int = 0;
    s->ch_sbr = 0x00008002;
    s->ch_pri = 0;
    s->tcd_saddr = 0;
    s->tcd_soff = 0;
    s->tcd_attr = 0;
    s->tcd_nbytes_mloff = 0;
    s->tcd_slast_sda = 0;
    s->tcd_daddr = 0;
    s->tcd_doff = 0;
    s->tcd_citer = 0;
    s->tcd_dlast_sga = 0;
    s->tcd_csr = 0;
    s->tcd_biter = 0;
}

/**
 * nxps32k358_edma_global_read - Read from the global EDMA registers
 * @param s: Pointer to the NXPS32K358EDMAState structure
 * @param offset: Offset of the register to read
 * @param size: Size of the read operation (unused)
 *
 * This function reads the value from the specified global EDMA register
 * based on the provided offset. For offsets within the range of
 * A_EDMA_CHN_GRPRI, it calculates the appropriate channel group priority
 * register to read from. If an invalid offset is provided, it logs an error
 * message.
 *
 * @return The value read from the specified register.
 *
 * @note There is no real support for group priorities, we just return the value
 * saved in the field.
 */
static uint64_t nxps32k358_edma_global_read(NXPS32K358EDMAState *s,
                                            hwaddr offset, unsigned size) {
    uint32_t res = 0;

    switch (offset) {
        case A_EDMA_CSR:
            res = s->edma_csr;
            break;
        case A_EDMA_ES:
            res = s->edma_es;
            break;
        case A_EDMA_INT:
            res = s->edma_int;
            break;
        case A_EDMA_HRS:
            res = s->edma_hrs;
            break;
        default:
            if (offset >= A_EDMA_CHN_GRPRI(0) &&
                offset < A_EDMA_CHN_GRPRI(EDMA_CHANNELS)) {
                int n = (offset - A_EDMA_CHN_GRPRI(0)) / 4;
                res = s->edma_chn_grpri[n];
                break;
            }
            qemu_log_mask(LOG_GUEST_ERROR,
                          "%s: Bad offset 0x%" HWADDR_PRIx "\n", __func__,
                          offset);
            break;
    }

    return res;
}

/**
 * nxps32k358_edma_global_write - Write to the global registers of the eDMA
 * controller
 * @param s: Pointer to the NXPS32K358EDMAState structure
 * @param offset: Offset of the register to write to
 * @param value: Value to write to the register
 * @param size: Size of the value to write
 *
 * This function handles writing to the global registers of the eDMA controller.
 * It supports writing to the CSR and GRPRI registers. For the CSR register,
 * only the bits specified by CSR_WR_MASK are writable. For the GRPRI registers,
 * only the bits specified by GRPRI_WR_MASK are writable. Writes to the ES, INT,
 * and HRS registers are not allowed and are treated as read-only.
 *
 * If an invalid offset is provided, an error message is logged.
 *
 * @note There is no real support for group priorities, we just write the value
 * to the field.
 */
static void nxps32k358_edma_global_write(NXPS32K358EDMAState *s, hwaddr offset,
                                         uint64_t value, unsigned size) {
    const uint32_t CSR_WR_MASK = 0x000003f6;
    const uint32_t GRPRI_WR_MASK = 0x0000001f;
    switch (offset) {
        case A_EDMA_CSR:
            s->edma_csr &= ~CSR_WR_MASK;
            s->edma_csr |= value & CSR_WR_MASK;
            break;
        case A_EDMA_ES:
        case A_EDMA_INT:
        case A_EDMA_HRS:
            READONLY break;
        default:
            if (offset >= A_EDMA_CHN_GRPRI(0) &&
                offset < A_EDMA_CHN_GRPRI(EDMA_CHANNELS)) {
                int n = (offset - A_EDMA_CHN_GRPRI(0)) / 4;
                s->edma_chn_grpri[n] &= ~GRPRI_WR_MASK;
                s->edma_chn_grpri[n] |= value & GRPRI_WR_MASK;
                break;
            }
            qemu_log_mask(LOG_GUEST_ERROR,
                          "%s: Bad offset 0x%" HWADDR_PRIx "\n", __func__,
                          offset);
            break;
    }
}

/**
 * nxps32k358_edma0_read - Read from the NXPS32K358 EDMA0 module.
 * @param obj: Pointer to the NXPS32K358EDMAState object.
 * @param offset: The offset within the EDMA0 module to read from.
 * @param size: The size of the read operation.
 *
 * This function reads data from the NXPS32K358 EDMA0 module. If the offset
 * is greater than or equal to 0x4000, it reads from the Transfer Control
 * Descriptor (TCD) area. The TCD offset is calculated as the offset modulo
 * 0x4000, and the TCD number is calculated as the integer division of the
 * offset minus 0x4000 by 0x4000. The function then calls
 * nxps32k358_edma_tcd_read() to perform the read operation from the TCD area.
 * If the offset is less than 0x4000, it reads from the global area of the
 * EDMA0 module by calling nxps32k358_edma_global_read().
 *
 * @return The data read from the specified offset and size.
 */
static uint64_t nxps32k358_edma0_read(void *obj, hwaddr offset, unsigned size) {
    NXPS32K358EDMAState *s = obj;
    if (offset >= 0x4000) {
        hwaddr tcd_offset = offset % 0x4000;
        unsigned tcd_num = (offset - 0x4000) / 0x4000;
        return nxps32k358_edma_tcd_read(s, tcd_offset, size, tcd_num);
    } else {
        return nxps32k358_edma_global_read(s, offset, size);
    }
}

/**
 * nxps32k358_edma0_write - Handles write operations to the eDMA controller.
 * @param obj: Pointer to the eDMA state object.
 * @param offset: The offset within the eDMA memory space being written to.
 * @param value: The value to write.
 * @param size: The size of the value being written.
 *
 * This function determines whether the write operation is targeting the global
 * eDMA registers or a specific Transfer Control Descriptor (TCD) based on the
 * provided offset. If the offset is greater than or equal to 0x4000, the write
 * is directed to a TCD; otherwise, it is directed to the global eDMA registers.
 */
static void nxps32k358_edma0_write(void *obj, hwaddr offset, uint64_t value,
                                   unsigned size) {
    NXPS32K358EDMAState *s = obj;
    if (offset >= 0x4000) {
        hwaddr tcd_offset = offset % 0x4000;
        unsigned tcd_num = (offset - 0x4000) / 0x4000;
        return nxps32k358_edma_tcd_write(s, tcd_offset, value, size, tcd_num);
    } else {
        return nxps32k358_edma_global_write(s, offset, value, size);
    }
}

/**
 * @brief Reads data from the NXPS32K358 EDMA12 module.
 *
 * This function reads data from the specified offset within the NXPS32K358
 * EDMA12 module. It calculates the TCD (Transfer Control Descriptor) offset and
 * number based on the given offset and size, and then calls the
 * `nxps32k358_edma_tcd_read` function to perform the read operation.
 *
 * @param obj Pointer to the NXPS32K358EDMAState object.
 * @param offset The offset within the EDMA12 module to read from.
 * @param size The size of the data to read.
 * @return The data read from the specified offset and size.
 */
static uint64_t nxps32k358_edma12_read(void *obj, hwaddr offset,
                                       unsigned size) {
    NXPS32K358EDMAState *s = obj;
    hwaddr tcd_offset = offset % 0x4000;
    unsigned tcd_num = offset / 0x4000 + 12;
    return nxps32k358_edma_tcd_read(s, tcd_offset, size, tcd_num);
}

/**
 * nxps32k358_edma12_write - Write to the eDMA12 module of the NXPS32K358
 * @param obj: Pointer to the eDMA state object
 * @param offset: Offset within the eDMA module
 * @param value: Value to be written
 * @param size: Size of the value to be written
 *
 * This function writes a value to a specific offset within the eDMA12 module
 * of the NXPS32K358. The offset is adjusted to target a specific Transfer
 * Control Descriptor (TCD) within the eDMA module. The TCD number is calculated
 * based on the offset and a base index of 12.
 */
static void nxps32k358_edma12_write(void *obj, hwaddr offset, uint64_t value,
                                    unsigned size) {
    NXPS32K358EDMAState *s = obj;
    hwaddr tcd_offset = offset % 0x4000;
    unsigned tcd_num = offset / 0x4000 + 12;
    return nxps32k358_edma_tcd_write(s, tcd_offset, value, size, tcd_num);
}

static const MemoryRegionOps nxps32k358_edma0_ops = {
    .read = nxps32k358_edma0_read,
    .write = nxps32k358_edma0_write,
    .endianness = DEVICE_NATIVE_ENDIAN,
};

static const MemoryRegionOps nxps32k358_edma12_ops = {
    .read = nxps32k358_edma12_read,
    .write = nxps32k358_edma12_write,
    .endianness = DEVICE_NATIVE_ENDIAN,
};

/**
 * nxps32k358_edma_init - Initialize the NXP S32K358 eDMA controller
 * @param obj: Pointer to the Object structure
 *
 * This function initializes the NXP S32K358 eDMA controller by setting up
 * multiple memory regions and initializing the system bus for memory-mapped
 * I/O (MMIO). The TCDs (Transfer Control Descriptors) are interleaved with
 * the global registers, so multiple memory regions are created to handle
 * this interleaving. Additionally, it initializes the IRQs for each eDMA
 * channel.
 *
 * Memory regions initialized:
 * - mmio0: Base memory region for the eDMA controller and for the first 12 TCDs
 * (TCD0-TCD11)
 * - mmio12: Memory region for the remaining TCDs (TCD12-TCD31)
 *
 * IRQs initialized for each eDMA channel.
 */
static void nxps32k358_edma_init(Object *obj) {
    NXPS32K358EDMAState *s = NXPS32K358_EDMA(obj);

    memory_region_init_io(&s->mmio0, OBJECT(s), &nxps32k358_edma0_ops, s,
                          TYPE_NXPS32K358_EDMA, 0x4000 * 13);
    sysbus_init_mmio(SYS_BUS_DEVICE(s), &s->mmio0);

    memory_region_init_io(&s->mmio12, OBJECT(s), &nxps32k358_edma12_ops, s,
                          TYPE_NXPS32K358_EDMA "-tcd12", 0x4000 * 20);
    sysbus_init_mmio(SYS_BUS_DEVICE(s), &s->mmio12);

    for (int i = 0; i < EDMA_CHANNELS; i++) {
        sysbus_init_irq(SYS_BUS_DEVICE(s), &s->tcd[i].irq);
    }
}

/**
 * nxps32k358_edma_reset - Resets the state of the NXPS32K358 eDMA controller.
 * @param dev: Pointer to the DeviceState structure representing the device.
 *
 * This function resets the eDMA controller by initializing its control and
 * status registers to their default reset values. It also resets each channel's
 * priority group and TCD (Transfer Control Descriptor) and updates the IRQ
 * status for each channel.
 */
static void nxps32k358_edma_reset(DeviceState *dev) {
    NXPS32K358EDMAState *s = NXPS32K358_EDMA(dev);

    s->edma_csr = EDMA_CSR_RESET;
    s->edma_es = EDMA_ES_RESET;
    s->edma_int = EDMA_INT_RESET;
    s->edma_hrs = EDMA_HRS_RESET;
    for (int i = 0; i < EDMA_CHANNELS; i++) {
        s->edma_chn_grpri[i] = EDMA_CHN_GRPRI_RESET;
        nxps32k358_edma_tcd_reset(&s->tcd[i]);
        nxps32k358_edma_tcd_update_irq(s, i);
    }
}

static void nxps32k358_edma_realize(DeviceState *dev, Error **errp) {
    nxps32k358_edma_reset(dev);
}

static void nxps32k358_edma_class_init(ObjectClass *klass, void *data) {
    DeviceClass *dc = DEVICE_CLASS(klass);

    dc->realize = nxps32k358_edma_realize;
    device_class_set_legacy_reset(dc, nxps32k358_edma_reset);
}

static const TypeInfo nxps32k358_edma_info = {
    .name = TYPE_NXPS32K358_EDMA,
    .parent = TYPE_SYS_BUS_DEVICE,
    .instance_size = sizeof(NXPS32K358EDMAState),
    .class_init = nxps32k358_edma_class_init,
    .instance_init = nxps32k358_edma_init,
};

static void nxps32k358_edma_register_types(void) {
    type_register_static(&nxps32k358_edma_info);
}

type_init(nxps32k358_edma_register_types)
