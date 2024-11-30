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
#include "hw/qdev-properties.h"
#include "hw/qdev-properties-system.h"
#include "qemu/log.h"
#include "qemu/module.h"

#ifndef STM_USART_ERR_DEBUG
#define STM_USART_ERR_DEBUG 0
#endif

#define DB_PRINT_L(lvl, fmt, args...)               \
    do {                                            \
        if (STM_USART_ERR_DEBUG >= lvl) {           \
            qemu_log("%s: " fmt, __func__, ##args); \
        }                                           \
    } while (0)

#define DB_PRINT(fmt, args...) DB_PRINT_L(1, fmt, ##args)

static int nxps32k358_lpuart_can_receive(void *opaque) {
    NXPS32K35LPUartState *s = opaque;

    if (s->lpuart_stat & LPUART_STAT_RAF) {
        return 1;
    }

    return 0;
}

static void nxps32k358_update_irq(STM32F2XXUsartState *s) {
    uint32_t mask = s->usart_sr & s->usart_cr1;

    if (mask & (USART_SR_TXE | USART_SR_TC | USART_SR_RXNE)) {
        qemu_set_irq(s->irq, 1);
    } else {
        qemu_set_irq(s->irq, 0);
    }
}

static void nxps32k358_lpuart_receive(void *opaque, const uint8_t *buf,
                                      int size) {
    NXPS32K35LPUartState *s = opaque;

    if (!(s->lpuart_control & LPUART_CONTROL_RE)) {
        /* Read not enabled - drop the chars */
        DB_PRINT("Dropping the chars\n");
        return;
    }

    s->lpuart_stat |= LPUART_STAT_RAF;

    s->lpuart_data = *buf;

    nxps32k358_update_irq(s);

    DB_PRINT("Receiving: %c\n", s->lpuart_data);
}

static void nxps32k358_lpuart_reset(DeviceState *dev) {
    NXPS32K35LPUartState *s = NXPS32K358_LPUART(dev);

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

static uint64_t nxps32k358_lpuart_read(void *opaque, hwaddr addr,
                                       unsigned int size) {
    NXPS32K35LPUartState *s = opaque;
    uint64_t retvalue;

    DB_PRINT("Read 0x%" HWADDR_PRIx "\n", addr);

    switch (addr) {
        case LPUART_STAT:
            retvalue = s->lpuart_stat;
            qemu_chr_fe_accept_input(&s->chr);
            return retvalue;
        case LPUART_DATA:
            DB_PRINT("Value: 0x%" PRIx32 ", %c\n", s->lpuart_data,
                     (char)s->lpuart_data);
            retvalue = s->lpuart_data & 0x3FF;
            s->usart_sr &= ~USART_SR_RXNE;
            qemu_chr_fe_accept_input(&s->chr);
            stm32f2xx_update_irq(s);
            return retvalue;
        case USART_BRR:
            return s->usart_brr;
        case USART_CR1:
            return s->usart_cr1;
        case USART_CR2:
            return s->usart_cr2;
        case USART_CR3:
            return s->usart_cr3;
        case USART_GTPR:
            return s->usart_gtpr;
        default:
            qemu_log_mask(LOG_GUEST_ERROR,
                          "%s: Bad offset 0x%" HWADDR_PRIx "\n", __func__,
                          addr);
            return 0;
    }

    return 0;
}

static void stm32f2xx_usart_write(void *opaque, hwaddr addr, uint64_t val64,
                                  unsigned int size) {
    STM32F2XXUsartState *s = opaque;
    uint32_t value = val64;
    unsigned char ch;

    DB_PRINT("Write 0x%" PRIx32 ", 0x%" HWADDR_PRIx "\n", value, addr);

    switch (addr) {
        case USART_SR:
            if (value <= 0x3FF) {
                /* I/O being synchronous, TXE is always set. In addition, it may
                   only be set by hardware, so keep it set here. */
                s->usart_sr = value | USART_SR_TXE;
            } else {
                s->usart_sr &= value;
            }
            stm32f2xx_update_irq(s);
            return;
        case USART_DR:
            if (value < 0xF000) {
                ch = value;
                /* XXX this blocks entire thread. Rewrite to use
                 * qemu_chr_fe_write and background I/O callbacks */
                qemu_chr_fe_write_all(&s->chr, &ch, 1);
                /* XXX I/O are currently synchronous, making it impossible for
                   software to observe transient states where TXE or TC aren't
                   set. Unlike TXE however, which is read-only, software may
                   clear TC by writing 0 to the SR register, so set it again
                   on each write. */
                s->usart_sr |= USART_SR_TC;
                stm32f2xx_update_irq(s);
            }
            return;
        case USART_BRR:
            s->usart_brr = value;
            return;
        case USART_CR1:
            s->usart_cr1 = value;
            stm32f2xx_update_irq(s);
            return;
        case USART_CR2:
            s->usart_cr2 = value;
            return;
        case USART_CR3:
            s->usart_cr3 = value;
            return;
        case USART_GTPR:
            s->usart_gtpr = value;
            return;
        default:
            qemu_log_mask(LOG_GUEST_ERROR,
                          "%s: Bad offset 0x%" HWADDR_PRIx "\n", __func__,
                          addr);
    }
}

static const MemoryRegionOps stm32f2xx_usart_ops = {
    .read = stm32f2xx_usart_read,
    .write = stm32f2xx_usart_write,
    .endianness = DEVICE_NATIVE_ENDIAN,
};

static Property stm32f2xx_usart_properties[] = {
    DEFINE_PROP_CHR("chardev", STM32F2XXUsartState, chr),
    DEFINE_PROP_END_OF_LIST(),
};

static void stm32f2xx_usart_init(Object *obj) {
    STM32F2XXUsartState *s = STM32F2XX_USART(obj);

    sysbus_init_irq(SYS_BUS_DEVICE(obj), &s->irq);

    memory_region_init_io(&s->mmio, obj, &stm32f2xx_usart_ops, s,
                          TYPE_STM32F2XX_USART, 0x400);
    sysbus_init_mmio(SYS_BUS_DEVICE(obj), &s->mmio);
}

static void stm32f2xx_usart_realize(DeviceState *dev, Error **errp) {
    STM32F2XXUsartState *s = STM32F2XX_USART(dev);

    qemu_chr_fe_set_handlers(&s->chr, stm32f2xx_usart_can_receive,
                             stm32f2xx_usart_receive, NULL, NULL, s, NULL,
                             true);
}

static void stm32f2xx_usart_class_init(ObjectClass *klass, void *data) {
    DeviceClass *dc = DEVICE_CLASS(klass);

    device_class_set_legacy_reset(dc, stm32f2xx_usart_reset);
    device_class_set_props(dc, stm32f2xx_usart_properties);
    dc->realize = stm32f2xx_usart_realize;
}

static const TypeInfo stm32f2xx_usart_info = {
    .name = TYPE_STM32F2XX_USART,
    .parent = TYPE_SYS_BUS_DEVICE,
    .instance_size = sizeof(STM32F2XXUsartState),
    .instance_init = stm32f2xx_usart_init,
    .class_init = stm32f2xx_usart_class_init,
};

static void stm32f2xx_usart_register_types(void) {
    type_register_static(&stm32f2xx_usart_info);
}

type_init(stm32f2xx_usart_register_types)
