/*
 * NXPS32K358 LPUART
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
 * @file nxps32k358_lpuart.c
 * @brief Implementation of the NXP S32K358 LPUART (Low Power UART).
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

/**
 * @brief Check if the LPUART can receive data.
 *
 * This function checks the status register of the LPUART to determine if
 * it can receive data.
 *
 * @param opaque Pointer to the NXPS32K358LPUartState structure.
 * @return 1 if the LPUART can receive data, 0 otherwise.
 */
static int nxps32k358_lpuart_can_receive(void *opaque) {
    NXPS32K358LPUartState *s = opaque;

    if (s->lpuart_stat & R_STAT_RDRF_MASK) {
        return 0;
    }

    return 1;
}

/**
 * @brief Update the interrupt request (IRQ) status for the NXPS32K358 LPUART.
 *
 * This function checks the status and control registers of the LPUART to
 * determine if an interrupt should be triggered. It sets or clears the IRQ
 * based on the result of this check.
 *
 * @param s Pointer to the NXPS32K358LPUartState structure containing the
 *          LPUART state.
 */
static void nxps32k358_lpuart_update_irq(NXPS32K358LPUartState *s) {
    uint32_t mask = s->lpuart_stat & s->lpuart_control;

    if (mask &
        (R_CONTROL_TIE_MASK | R_CONTROL_TCIE_MASK | R_CONTROL_RIE_MASK)) {
        qemu_set_irq(s->irq, 1);
    } else {
        qemu_set_irq(s->irq, 0);
    }
}

/**
 * @brief Handles the reception of data for the NXP S32K358 LPUART.
 *
 * @param opaque A pointer to the opaque state structure for the LPUART.
 * @param buf A pointer to the buffer containing the data to be received.
 * @param size The size of the data buffer.
 *
 * This function processes incoming data for the LPUART. If the read
 * operation is not enabled (as indicated by the R_CONTROL_RE_MASK bit
 * in the control register), the data is dropped and a debug message is
 * printed. Otherwise, the data is stored in the lpuart_data register,
 * the RDRF (Receive Data Register Full) flag is set in the status
 * register, and an interrupt is triggered if necessary. A debug message
 * is printed with the received character.
 */
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

    nxps32k358_lpuart_update_irq(s);

    DB_PRINT("Receiving: %c\n", s->lpuart_data);
}

/**
 * @brief Reset the NXP S32K358 LPUART device state.
 *
 * This function resets all the registers of the NXP S32K358 LPUART device to
 * their default values. Additionally, it updates the IRQ status of the device
 * after resetting the registers.
 *
 * @param dev Pointer to the DeviceState structure for the LPUART device.
 */
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

    nxps32k358_lpuart_update_irq(s);
}

/**
 * @brief Update the UART parameters for the NXPS32K358 LPUART.
 *
 * This function updates the UART parameters, specifically the baud rate, for
 * the NXPS32K358 LPUART. It sets the baud rate in the QEMUSerialSetParams
 * structure and then applies these parameters using the qemu_chr_fe_ioctl
 * function.
 *
 * @param s Pointer to the NXPS32K358LPUartState structure.
 */
static void nxps32k358_lpuart_update_params(NXPS32K358LPUartState *s) {
    QEMUSerialSetParams ssp;
    ssp.speed = LPUART_BAUD_RATE(s);
    DB_PRINT("Baud rate: %d\n", ssp.speed);

    qemu_chr_fe_ioctl(&s->chr, CHR_IOCTL_SERIAL_SET_PARAMS, &ssp);
}

/**
 * nxps32k358_lpuart_read - Read from the NXP S32K358 LPUART register
 * @opaque: Pointer to the LPUART state
 * @addr: Address of the register to read
 * @size: Size of the read operation
 *
 * This function reads the value from the specified LPUART register
 * based on the provided address. It handles different registers
 * such as A_VERID, A_STAT, A_GLOBAL, A_DATA, A_DATARO, A_CONTROL,
 * and A_BAUD. For the A_DATA and A_DATARO registers, it also updates
 * the status register and processes input acceptance.
 *
 * @return the value read from the specified register. If the address
 * is invalid, it logs an error and returns 0.
 */
static uint64_t nxps32k358_lpuart_read(void *opaque, hwaddr addr,
                                       unsigned int size) {
    NXPS32K358LPUartState *s = NXPS32K358_LPUART(opaque);

    DB_PRINT_READ("Read 0x%" HWADDR_PRIx "\n", addr);

    switch (addr) {
        case A_VERID:
            return s->lpuart_verid;
        case A_STAT:
            return s->lpuart_stat;
        case A_GLOBAL:
            return s->lpuart_global;
        case A_DATA:
        case A_DATARO:  // read only data, identical to A_DATA but only for
                        // reading
            DB_PRINT_READ("Value: 0x%" PRIx32 ", %c\n", s->lpuart_data,
                          (char)s->lpuart_data);
            s->lpuart_stat &= ~R_STAT_RDRF_MASK;
            qemu_chr_fe_accept_input(&s->chr);
            nxps32k358_lpuart_update_irq(s);
            return s->lpuart_data;
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

/**
 * @brief Handle writes to the NXP S32K358 LPUART registers.
 *
 * This function handles writes to various registers of the NXP S32K358 LPUART
 * device. Depending on the address, it updates the corresponding register in
 * the device state and performs necessary actions such as resetting the device,
 * writing data to the character device, updating IRQs, or updating baud rate
 * parameters.
 *
 * @param opaque Pointer to the device state.
 * @param addr Address of the register being written to.
 * @param val64 Value to write to the register.
 * @param size Size of the value being written.
 *
 * If an invalid address is provided, an error is logged.
 *
 * @note The status register is currently not completely implemented as it is
 * treated as read-only while it is not (it can be written to clear some bits).
 */
static void nxps32k358_lpuart_write(void *opaque, hwaddr addr, uint64_t val64,
                                    unsigned int size) {
    NXPS32K358LPUartState *s = NXPS32K358_LPUART(opaque);
    uint32_t value = val64;
    unsigned char ch;

    DB_PRINT("Write 0x%" PRIx32 ", 0x%" HWADDR_PRIx "\n", value, addr);

    switch (addr) {
        case A_GLOBAL:
            s->lpuart_global = value;
            if (value & R_GLOBAL_RST_MASK) {
                nxps32k358_lpuart_reset(DEVICE(s));
            }
            return;
        case READONLY A_STAT:
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
            return;
        case A_CONTROL:
            s->lpuart_control = value;
            nxps32k358_lpuart_update_irq(s);
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

/**
 * @brief Initialize the NXP S32K358 LPUART device.
 *
 * This function initializes the NXP S32K358 LPUART device by setting up
 * the necessary system bus IRQ, memory-mapped I/O region, and clock input.
 *
 * @param obj Pointer to the Object structure representing the device.
 *
 * Steps performed:
 * - Cast the generic Object pointer to NXPS32K358LPUartState structure.
 * - Initialize the system bus IRQ for the device.
 * - Initialize the memory-mapped I/O region for the device.
 * - Register the memory-mapped I/O region with the system bus.
 * - Initialize the clock input for the device.
 */
static void nxps32k358_lpuart_init(Object *obj) {
    NXPS32K358LPUartState *s = NXPS32K358_LPUART(obj);

    sysbus_init_irq(SYS_BUS_DEVICE(obj), &s->irq);

    memory_region_init_io(&s->mmio, obj, &nxps32k358_lpuart_ops, s,
                          TYPE_NXPS32K358_LPUART, 0x4000);
    sysbus_init_mmio(SYS_BUS_DEVICE(obj), &s->mmio);

    s->clk = qdev_init_clock_in(DEVICE(s), "clk", NULL, s, 0);
}

/**
 * @brief Realize the NXPS32K358 LPUART device.
 *
 * This function initializes the NXPS32K358 LPUART device. It checks if the
 * LPUART clock source is properly configured. If the clock source is not
 * set, it reports an error and returns. If the clock source is set, it
 * sets the character device handlers for the LPUART.
 *
 * @param dev The device state.
 * @param errp Pointer to an error object.
 */
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

/**
 * @brief Initialize the NXP S32K358 LPUART class
 *
 * This function sets up the NXP S32K358 LPUART device class by configuring
 * its legacy reset handler, properties, and realize function.
 *
 * @param klass The ObjectClass to initialize
 * @param data Additional data for initialization (unused)
 */
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
