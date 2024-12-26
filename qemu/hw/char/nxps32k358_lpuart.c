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

#include "qemu/osdep.h"
#include "hw/char/nxps32k358_lpuart.h"
#include "hw/irq.h"
#include "chardev/char-serial.h"
#include "qapi/error.h"
#include "hw/qdev-clock.h"
#include "hw/qdev-properties.h"
#include "hw/qdev-properties-system.h"
#include "qemu/log.h"
#include "qemu/module.h"

// If NXP_LPUART_DEBUG is 0, no debug messages will be printed
// If it is 1, only write logs will be printed
// If it is 2, read and write logs will be printed
#ifndef NXP_LPUART_DEBUG
#define NXP_LPUART_DEBUG 0
#endif

#define DB_PRINT_L(lvl, fmt, args...)               \
    do {                                            \
        if (NXP_LPUART_DEBUG >= lvl) {              \
            qemu_log("%s: " fmt, __func__, ##args); \
        }                                           \
    } while (0)

#define DB_PRINT(fmt, args...) DB_PRINT_L(1, fmt, ##args)
#define DB_PRINT_READ(fmt, args...) DB_PRINT_L(2, fmt, ##args)

static int nxps32k358_lpuart_can_receive(void *opaque) {
    NXPS32K358LPUartState *s = opaque;

    if (s->lpuart_stat & R_STAT_RDRF_MASK) {
        return 0;
    }

    return 1;
}

static void nxps32k358_update_irq(NXPS32K358LPUartState *s) {
    uint32_t mask = s->lpuart_stat & s->lpuart_control;

    if (mask &
        (R_CONTROL_TIE_MASK | R_CONTROL_TCIE_MASK | R_CONTROL_RIE_MASK)) {
        qemu_set_irq(s->irq, 1);
    } else {
        qemu_set_irq(s->irq, 0);
    }
}

static void nxps32k358_lpuart_receive(void *opaque, const uint8_t *buf,
                                      int size) {
    NXPS32K358LPUartState *s = opaque;

    if (!(s->lpuart_control & R_CONTROL_RE_MASK)) {
        /* Read not enabled - drop the chars */
        DB_PRINT("Dropping the chars, read is disabled\n");
        return;
    }

    s->lpuart_data = *buf;

    s->lpuart_stat |= R_STAT_RDRF_MASK;

    nxps32k358_update_irq(s);

    DB_PRINT("Receiving: %c\n", s->lpuart_data);
}

static void nxps32k358_lpuart_reset(DeviceState *dev) {
    NXPS32K358LPUartState *s = NXPS32K358_LPUART(dev);

    s->lpuart_verid = LPUART_VERID_RESET(s->lpuart_port);
    s->lpuart_param = LPUART_PARAM_RESET(s->lpuart_port);
    s->lpuart_global = LPUART_GLOBAL_RESET;
    s->lpuart_pincfg = LPUART_PINCFG_RESET;
    s->lpuart_baud = LPUART_BAUD_RESET;
    s->lpuart_stat = LPUART_STAT_RESET;
    s->lpuart_control = LPUART_CONTROL_RESET;
    s->lpuart_data = LPUART_DATA_RESET;
    s->lpuart_match = LPUART_MATCH_RESET;
    s->lpuart_modir = LPUART_MODIR_RESET;
    s->lpuart_fifo = LPUART_FIFO_RESET(s->lpuart_port);
    s->lpuart_water = LPUART_WATER_RESET;
    s->lpuart_dataro = LPUART_DATARO_RESET;
    s->lpuart_mcr = LPUART_MCR_RESET;
    s->lpuart_msr = LPUART_MSR_RESET;
    s->lpuart_reir = LPUART_REIR_RESET;
    s->lpuart_teir = LPUART_TEIR_RESET;
    s->lpuart_hdcr = LPUART_HDCR_RESET;
    s->lpuart_tocr = LPUART_TOCR_RESET;
    s->lpuart_tosr = LPUART_TOSR_RESET;
    for (int i = 0; i < LPUART_TIMEOUT_NUM; i++) {
        s->lpuart_timeout[i] = LPUART_TIMEOUT_RESET;
    }
    for (int i = 0; i < LPUART_TCBR_NUM; i++) {
        s->lpuart_tcb[i] = LPUART_TCB_RESET;
    }
    for (int i = 0; i < LPUART_TDBR_NUM; i++) {
        s->lpuart_tdb[i] = LPUART_TDB_RESET;
    }

    nxps32k358_update_irq(s);
}

static void nxps32k358_lpuart_update_params(NXPS32K358LPUartState *s) {
    QEMUSerialSetParams ssp;
    uint32_t sbr = s->lpuart_baud & 0x1FFF;
    uint32_t osr = (s->lpuart_baud >> 24) & 0x1F;
    unsigned int baud_base = clock_get_hz(s->clk);
    ssp.speed = baud_base / (sbr * (osr + 1));
    DB_PRINT("Baud rate: %d\n", ssp.speed);

    qemu_chr_fe_ioctl(&s->chr, CHR_IOCTL_SERIAL_SET_PARAMS, &ssp);
}

static uint64_t nxps32k358_lpuart_read(void *opaque, hwaddr addr,
                                       unsigned int size) {
    NXPS32K358LPUartState *s = opaque;
    uint64_t retvalue;

    DB_PRINT_READ("Read 0x%" HWADDR_PRIx "\n", addr);

    switch (addr) {
        case A_VERID:
            return s->lpuart_verid;
        case A_STAT:
            retvalue = s->lpuart_stat;
            return retvalue;
        case A_DATA:
            DB_PRINT_READ("Value: 0x%" PRIx32 ", %c\n", s->lpuart_data,
                          (char)s->lpuart_data);
            retvalue = s->lpuart_data;
            s->lpuart_stat &= ~R_STAT_RDRF_MASK;
            qemu_chr_fe_accept_input(&s->chr);
            nxps32k358_update_irq(s);
            return retvalue;
        case A_CONTROL:
            return s->lpuart_control;
        case A_BAUD:
            return s->lpuart_baud;
        default:
            qemu_log_mask(LOG_GUEST_ERROR,
                          "%s: Bad offset 0x%" HWADDR_PRIx "\n", __func__,
                          addr);
            return 0;
    }

    return 0;
}

static void nxps32k358_lpuart_write(void *opaque, hwaddr addr, uint64_t val64,
                                    unsigned int size) {
    NXPS32K358LPUartState *s = opaque;
    uint32_t value = val64;
    unsigned char ch;

    DB_PRINT("Write 0x%" PRIx32 ", 0x%" HWADDR_PRIx "\n", value, addr);

    switch (addr) {
        case A_STAT:
            nxps32k358_update_irq(s);
            return;
        case A_DATA:
            bool is_7bit = s->lpuart_control & R_CONTROL_M7_MASK;
            bool is_9bit = s->lpuart_control & R_CONTROL_M_MASK;
            if (is_9bit) {
                qemu_log_mask(LOG_GUEST_ERROR,
                              "%s: 9-bit data format not supported\n",
                              __func__);
                return;
            }
            if (is_7bit) {
                value &= 0x7F;
            }
            ch = value;
            qemu_chr_fe_write_all(&s->chr, &ch, 1);
            nxps32k358_update_irq(s);
            return;
        case A_CONTROL:
            s->lpuart_control = value;
            nxps32k358_update_irq(s);
            return;
        case A_BAUD:
            s->lpuart_baud = value;
            nxps32k358_lpuart_update_params(s);
            return;
        default:
            qemu_log_mask(LOG_GUEST_ERROR,
                          "%s: Bad offset 0x%" HWADDR_PRIx "\n", __func__,
                          addr);
    }
}

static const MemoryRegionOps nxps32k358_lpuart_ops = {
    .read = nxps32k358_lpuart_read,
    .write = nxps32k358_lpuart_write,
    .endianness = DEVICE_NATIVE_ENDIAN,
};

static Property nxps32k358_lpuart_properties[] = {
    DEFINE_PROP_CHR("chardev", NXPS32K358LPUartState, chr),
    DEFINE_PROP_END_OF_LIST(),
};

static void nxps32k358_lpuart_init(Object *obj) {
    NXPS32K358LPUartState *s = NXPS32K358_LPUART(obj);

    sysbus_init_irq(SYS_BUS_DEVICE(obj), &s->irq);

    memory_region_init_io(&s->mmio, obj, &nxps32k358_lpuart_ops, s,
                          TYPE_NXPS32K358_LPUART, 0x4000);
    sysbus_init_mmio(SYS_BUS_DEVICE(obj), &s->mmio);

    s->clk = qdev_init_clock_in(DEVICE(s), "clk", NULL, s, 0);
}

static void nxps32k358_lpuart_realize(DeviceState *dev, Error **errp) {
    NXPS32K358LPUartState *s = NXPS32K358_LPUART(dev);
    if (!clock_has_source(s->clk)) {
        error_setg(errp, "LPUART clock must be wired up by SoC code");
        return;
    }

    qemu_chr_fe_set_handlers(&s->chr, nxps32k358_lpuart_can_receive,
                             nxps32k358_lpuart_receive, NULL, NULL, s, NULL,
                             true);
}

static void nxps32k358_lpuart_class_init(ObjectClass *klass, void *data) {
    DeviceClass *dc = DEVICE_CLASS(klass);

    device_class_set_legacy_reset(dc, nxps32k358_lpuart_reset);
    device_class_set_props(dc, nxps32k358_lpuart_properties);
    dc->realize = nxps32k358_lpuart_realize;
}

static const TypeInfo nxps32k358_lpuart_info = {
    .name = TYPE_NXPS32K358_LPUART,
    .parent = TYPE_SYS_BUS_DEVICE,
    .instance_size = sizeof(NXPS32K358LPUartState),
    .instance_init = nxps32k358_lpuart_init,
    .class_init = nxps32k358_lpuart_class_init,
};

static void nxps32k358_lpuart_register_types(void) {
    type_register_static(&nxps32k358_lpuart_info);
}

type_init(nxps32k358_lpuart_register_types)
