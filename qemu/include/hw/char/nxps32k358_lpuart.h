/*
 * NXPS32K358 LPUART
 *
 * Copyright (c) 2014 Alistair Francis <alistair@alistair23.me>
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

#ifndef HW_NXPS32K358_LPUART_H
#define HW_NXPS32K358_LPUART_H

#include "hw/sysbus.h"
#include "chardev/char-fe.h"
#include "qom/object.h"
#include "hw/registerfields.h"

REG32(VERID, 0x00)
REG32(PARAM, 0x04)
REG32(GLOBAL, 0x08)
REG32(PINCFG, 0x0C)
REG32(BAUD, 0x10)

REG32(STAT, 0x14)
// Receiver Data Register Full Flag
FIELD(STAT, RDRF, 21, 1)

REG32(CONTROL, 0x18)
// M = 0 for 8-bit data format, M = 1 for 9-bit data format
FIELD(CONTROL, M, 4, 1)
// M7 = 1 for 7-bit data format, M7 = 0 for other data formats
FIELD(CONTROL, M7, 11, 1)
// RE = 1 to enable the receiver
FIELD(CONTROL, RE, 18, 1)
// RIE = 1 to generate an IRQ if STAT[RDRF] is 1
FIELD(CONTROL, RIE, 21, 1)
// TCIE = 1 to generate an IRQ if STAT[TC] is 1
FIELD(CONTROL, TCIE, 22, 1)
// TIE = 1 to generate an IRQ if STAT[TDRE] is 1
FIELD(CONTROL, TIE, 23, 1)

REG32(DATA, 0x1C)
REG32(MATCH, 0x20)
REG32(MODIR, 0x24)
REG32(FIFO, 0x28)
REG32(WATER, 0x2C)
REG32(DATARO, 0x30)
REG32(MCR, 0x40)
REG32(MSR, 0x44)
REG32(REIR, 0x48)
REG32(TEIR, 0x4C)
REG32(HDCR, 0x50)
REG32(TOCR, 0x58)
REG32(TOSR, 0x5C)

#define LPUART_TIMEOUT_BASE_ADDR 0x60
#define LPUART_TIMEOUT_NUM 4
static inline uint32_t LPUART_TIMEOUT(int n) {
    return LPUART_TIMEOUT_BASE_ADDR + 4 * n;
}

#define LPUART_TCBR_BASE_ADDR 0x200
#define LPUART_TCBR_NUM 128
static inline uint32_t LPUART_TCBR(int n) {
    return LPUART_TCBR_BASE_ADDR + 4 * n;
}

#define LPUART_TDBR_BASE_ADDR 0x400
#define LPUART_TDBR_NUM 256
static inline uint32_t LPUART_TDBR(int n) {
    return LPUART_TDBR_BASE_ADDR + 4 * n;
}

/*
The reset value for the LPUART_VERID register is 0x04040007 for
 * the first two LPUART devices, 0x04040003 otherwise */
static inline uint32_t LPUART_VERID_RESET(int n) {
    return n < 2 ? 0x04040007 : 0x04040003;
}
/* The reset value for the LPUART_PARAM register is 0x00000404 for
 * the first two LPUART devices, 0x00000202 otherwise */
static inline uint32_t LPUART_PARAM_RESET(int n) {
    return n < 2 ? 0x00000404 : 0x00000202;
}
#define LPUART_GLOBAL_RESET 0x00000002
#define LPUART_PINCFG_RESET 0x00000000
#define LPUART_BAUD_RESET 0x0F000004
#define LPUART_STAT_RESET 0x00C00000
#define LPUART_CONTROL_RESET 0x0000000
#define LPUART_DATA_RESET 0X00001000
#define LPUART_MATCH_RESET 0x00000000
#define LPUART_MODIR_RESET 0x00000000
/*The reset value for the LPUART_FIFO register is 0x00C00033 if we consider
 * the first two LPUART devices, 0x00C00011 otherwise */
static inline uint32_t LPUART_FIFO_RESET(int n) {
    return n < 2 ? 0x00C00033 : 0x00C00011;
}
#define LPUART_WATER_RESET 0x00000000
#define LPUART_DATARO_RESET 0x00001000
#define LPUART_MCR_RESET 0x00000000
#define LPUART_MSR_RESET 0x00000000
#define LPUART_REIR_RESET 0x00000000
#define LPUART_TEIR_RESET 0x00000000
#define LPUART_HDCR_RESET 0x00000000
#define LPUART_TOCR_RESET 0x00000000
#define LPUART_TOSR_RESET 0x0000000F
#define LPUART_TIMEOUT_RESET 0x00000000
#define LPUART_TCB_RESET 0x00000000
#define LPUART_TDB_RESET 0x00000000

#define TYPE_NXPS32K358_LPUART "nxps32k358-lpuart"
OBJECT_DECLARE_SIMPLE_TYPE(NXPS32K358LPUartState, NXPS32K358_LPUART)

struct NXPS32K358LPUartState {
    /* <private> */
    SysBusDevice parent_obj;

    /* <public> */
    MemoryRegion mmio;

    // Will be an integer from 0 to 15
    uint32_t lpuart_port;

    uint32_t lpuart_verid;
    uint32_t lpuart_param;
    uint32_t lpuart_global;
    uint32_t lpuart_pincfg;
    uint32_t lpuart_baud;
    uint32_t lpuart_stat;
    uint32_t lpuart_control;
    uint32_t lpuart_data;
    uint32_t lpuart_match;
    uint32_t lpuart_modir;
    uint32_t lpuart_fifo;
    uint32_t lpuart_water;
    uint32_t lpuart_dataro;
    uint32_t lpuart_mcr;
    uint32_t lpuart_msr;
    uint32_t lpuart_reir;
    uint32_t lpuart_teir;
    uint32_t lpuart_hdcr;
    uint32_t lpuart_tocr;
    uint32_t lpuart_tosr;
    uint32_t lpuart_timeout[LPUART_TIMEOUT_NUM];
    uint32_t lpuart_tcb[LPUART_TCBR_NUM];
    uint32_t lpuart_tdb[LPUART_TDBR_NUM];

    Clock *clk;
    CharBackend chr;
    qemu_irq irq;
};

#endif
