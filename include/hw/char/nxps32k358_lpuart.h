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

#ifndef HW_NXPS32K358_USART_H
#define HW_NXPS32K358_USART_H

#include "hw/sysbus.h"
#include "chardev/char-fe.h"
#include "qom/object.h"

#define LPUART_VERID 0x00
#define LPUART_PARAM 0x04
#define LPUART_GLOBAL 0x08
#define LPUART_PINCFG 0x0C
#define LPUART_BAUD 0x10
#define LPUART_STAT 0x14
#define LPUART_CONTROL 0x18
#define LPUART_DATA 0x1C
#define LPUART_MATCH 0x20
#define LPUART_MODIR 0x24
#define LPUART_FIFO 0x28
#define LPUART_WATER 0x2C
#define LPUART_DATARO 0x30
#define LPUART_MCR 0x40
#define LPUART_MSR 0x44
#define LPUART_REIR 0x48
#define LPUART_TEIR 0x4C
#define LPUART_HDCR 0x50
#define LPUART_TOCR 0x58
#define LPUART_TOSR 0x5C

/*
 * The following registers are array registers, so we need to define
 * the base address of each array register and the number of registers
 */
#define LPUART_TIMEOUT(n) (0x60 + 4 * (n))
#define LPUART_TCBR(n) (0x200 + 4 * (n))
#define LPUART_TDBR(n) (0x400 + 4 * (n))
#define LPUART_TIMEOUT_NUM 4
#define LPUART_TCBR_NUM 128
#define LPUART_TDBR_NUM 256

/*
The reset value for the LPUART_VERID register is 0x04040007 for
 * the first two LPUART devices, 0x04040003 otherwise */
#define LPUART_VERID_RESET(i) (i < 2 ? 0x04040007 : 0x04040003)
/* The reset value for the LPUART_PARAM register is 0x00000404 for
 * the first two LPUART devices, 0x00000202 otherwise */
#define LPUART_PARAM_RESET(i) (i < 2 ? 0x00000404 : 0x00000202)
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
#define LPUART_FIFO_RESET(i) (i < 2 ? 0x00C00033 : 0x00C00011)
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

#define LPUART_STAT_RAF (1 << 24)
#define LPUART_CONTROL_RE (1 << 18)

#define USART_SR_TXE (1 << 7)
#define USART_SR_TC (1 << 6)
#define USART_SR_RXNE (1 << 5)
#define USART_CR1_UE (1 << 13)
#define USART_CR1_TXEIE (1 << 7)
#define USART_CR1_TCEIE (1 << 6)
#define USART_CR1_RXNEIE (1 << 5)
#define USART_CR1_TE (1 << 3)
#define USART_CR1_RE (1 << 2)

#define TYPE_NXPS32K358_LPUART "nxps32k358-lpuart"
OBJECT_DECLARE_SIMPLE_TYPE(NXPS32K35LPUartState, NXPS32K358_LPUART)

struct NXPS32K35LPUartState {
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

    CharBackend chr;
    qemu_irq irq;
};
#endif
