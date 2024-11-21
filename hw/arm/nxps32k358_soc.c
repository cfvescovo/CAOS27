/*
 * NXPS32K358 SoC
 *
 * Copyright (c) 2021 Alexandre Iooss <erdnaxe@crans.org>
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
#include "qapi/error.h"
#include "qemu/module.h"
#include "hw/arm/boot.h"
#include "exec/address-spaces.h"
#include "hw/arm/nxps32k358_soc.h"
#include "hw/qdev-properties.h"
#include "hw/qdev-clock.h"
#include "hw/misc/unimp.h"
#include "sysemu/sysemu.h"

static void nxps32k358_soc_initfn(Object *obj) {
    NXPS32K358State *s = NXPS32K358_SOC(obj);
    // int i;

    object_initialize_child(obj, "armv7m", &s->armv7m, TYPE_ARMV7M);

    s->sysclk = qdev_init_clock_in(DEVICE(s), "sysclk", NULL, NULL, 0);
    s->refclk = qdev_init_clock_in(DEVICE(s), "refclk", NULL, NULL, 0);
}

static void nxps32k358_soc_realize(DeviceState *dev_soc, Error **errp) {
    NXPS32K358State *s = NXPS32K358_SOC(dev_soc);
    DeviceState *armv7m;
    // DeviceState *dev;
    // SysBusDevice *busdev;
    // int i;

    MemoryRegion *system_memory = get_system_memory();

    /*
     * We use s->refclk internally and only define it with qdev_init_clock_in()
     * so it is correctly parented and not leaked on an init/deinit; it is not
     * intended as an externally exposed clock.
     */
    if (clock_has_source(s->refclk)) {
        error_setg(errp, "refclk clock must not be wired up by the board code");
        return;
    }

    if (!clock_has_source(s->sysclk)) {
        error_setg(errp, "sysclk clock must be wired up by the board code");
        return;
    }

    /*
     * TODO: ideally we should model the SoC RCC and its ability to
     * change the sysclk frequency and define different sysclk sources.
     */

    /* The refclk always runs at frequency HCLK / 8 */
    clock_set_mul_div(s->refclk, 8, 1);
    clock_set_source(s->refclk, s->sysclk);

    /*
     * Init code flash region
     */
    memory_region_init_rom(&s->code_flash_0, OBJECT(dev_soc),
                           "NXPS32K358.code_flash_0", CODE_FLASH_BLOCK_SIZE,
                           &error_fatal);
    memory_region_add_subregion(system_memory, CODE_FLASH_BASE_ADDRESS,
                                &s->code_flash_0);

    memory_region_init_rom(&s->code_flash_1, OBJECT(dev_soc),
                           "NXPS32K358.code_flash_1", CODE_FLASH_BLOCK_SIZE,
                           &error_fatal);
    memory_region_add_subregion(system_memory,
                                CODE_FLASH_BASE_ADDRESS + CODE_FLASH_BLOCK_SIZE,
                                &s->code_flash_1);

    memory_region_init_rom(&s->code_flash_2, OBJECT(dev_soc),
                           "NXPS32K358.code_flash_2", CODE_FLASH_BLOCK_SIZE,
                           &error_fatal);
    memory_region_add_subregion(
        system_memory, CODE_FLASH_BASE_ADDRESS + 2 * CODE_FLASH_BLOCK_SIZE,
        &s->code_flash_2);

    memory_region_init_rom(&s->code_flash_3, OBJECT(dev_soc),
                           "NXPS32K358.code_flash_3", CODE_FLASH_BLOCK_SIZE,
                           &error_fatal);
    memory_region_add_subregion(
        system_memory, CODE_FLASH_BASE_ADDRESS + 3 * CODE_FLASH_BLOCK_SIZE,
        &s->code_flash_3);

    /* Init data flash region */
    memory_region_init_rom(&s->data_flash, OBJECT(dev_soc),
                           "NXPS32K358.data_flash", DATA_FLASH_SIZE,
                           &error_fatal);
    memory_region_add_subregion(system_memory, DATA_FLASH_BASE_ADDRESS,
                                &s->data_flash);

    /* Init SRAM region */
    memory_region_init_ram(&s->sram_0, NULL, "NXPS32K358.sram_0",
                           SRAM_BLOCK_SIZE, &error_fatal);
    memory_region_add_subregion(system_memory, SRAM_BASE_ADDRESS, &s->sram_0);

    memory_region_init_ram(&s->sram_1, NULL, "NXPS32K358.sram_1",
                           SRAM_BLOCK_SIZE, &error_fatal);
    memory_region_add_subregion(
        system_memory, SRAM_BASE_ADDRESS + SRAM_BLOCK_SIZE, &s->sram_1);

    memory_region_init_ram(&s->sram_2, NULL, "NXPS32K358.sram_2",
                           SRAM_BLOCK_SIZE, &error_fatal);
    memory_region_add_subregion(
        system_memory, SRAM_BASE_ADDRESS + 2 * SRAM_BLOCK_SIZE, &s->sram_2);

    /* Init DTCM */
    memory_region_init_ram(&s->dtcm, NULL, "NXPS32K358.dtcm", DTCM_SIZE,
                           &error_fatal);
    memory_region_add_subregion(system_memory, DTCM_BASE_ADDRESS, &s->dtcm);

    /* Init ITCM */
    memory_region_init_ram(&s->itcm, NULL, "NXPS32K358.itcm", ITCM_SIZE,
                           &error_fatal);
    memory_region_add_subregion(system_memory, ITCM_BASE_ADDRESS, &s->itcm);

    /* Init ARMv7m */
    armv7m = DEVICE(&s->armv7m);
    qdev_prop_set_uint32(armv7m, "num-irq",
                         240);  // TODO: Check if it should be 241
    qdev_prop_set_uint8(armv7m, "num-prio-bits",
                        4);  // 16 priority levels = 4 bits
    qdev_prop_set_string(armv7m, "cpu-type", ARM_CPU_TYPE_NAME("cortex-m7"));
    qdev_prop_set_bit(armv7m, "enable-bitband", true);
    qdev_prop_set_uint32(armv7m, "init-svtor", CODE_FLASH_BASE_ADDRESS);
    qdev_prop_set_uint32(armv7m, "init-nsvtor", CODE_FLASH_BASE_ADDRESS);
    qdev_connect_clock_in(armv7m, "cpuclk", s->sysclk);
    qdev_connect_clock_in(armv7m, "refclk", s->refclk);
    object_property_set_link(OBJECT(&s->armv7m), "memory",
                             OBJECT(get_system_memory()), &error_abort);
    if (!sysbus_realize(SYS_BUS_DEVICE(&s->armv7m), errp)) {
        return;
    }

    // TODO: create unimplemented devices to map to memory regions for FreeRTOS
}

static void nxps32k358_soc_class_init(ObjectClass *klass, void *data) {
    DeviceClass *dc = DEVICE_CLASS(klass);

    dc->realize = nxps32k358_soc_realize;
    /* No vmstate or reset required: device has no internal state */
}

static const TypeInfo nxps32k358_soc_info = {
    .name = TYPE_NXPS32K358_SOC,
    .parent = TYPE_SYS_BUS_DEVICE,
    .instance_size = sizeof(NXPS32K358State),
    .instance_init = nxps32k358_soc_initfn,
    .class_init = nxps32k358_soc_class_init,
};

static void nxps32k358_soc_types(void) {
    type_register_static(&nxps32k358_soc_info);
}

type_init(nxps32k358_soc_types)
