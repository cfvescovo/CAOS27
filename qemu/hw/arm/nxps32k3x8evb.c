/*
 * NXPS32K3X8EVB board emulation
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
 * @file nxps32k3x8evb.c
 * @brief Implementation of the NXPS32K3X8EVB board emulation.
 */

#include "qemu/osdep.h"
#include "qemu/units.h"
#include "qapi/error.h"
#include "hw/arm/boot.h"
#include "hw/arm/armv7m.h"
#include "qom/object.h"
#include "hw/boards.h"
#include "hw/arm/nxps32k3x8evb.h"
#include "hw/qdev-clock.h"

/**
 * @brief Initialize the NXP S32K3X8EVB board.
 *
 * This function initializes the NXP S32K3X8EVB board by performing the
 * following steps:
 * 1. Casts the generic MachineState to NXPS32K3X8EVBMachineState.
 * 2. Initializes the system clock and sets its frequency.
 * 3. Initializes the SoC (System on Chip) and connects the system clock to it.
 * 4. Loads the kernel image into the ARM CPU's flash memory.
 *
 * @param machine The generic MachineState passed by QEMU.
 */
static void NXPS32K3X8EVB_init(MachineState *machine) {
    // Cast the NXP machine from the generic machine
    NXPS32K3X8EVBMachineState *m_state = NXPS32K3X8EVB_MACHINE(machine);

    // Initialize system clock
    m_state->sysclk = clock_new(OBJECT(machine), "SYSCLK");
    clock_set_hz(m_state->sysclk, SYSCLK_FRQ);

    // Initialize the SoC
    object_initialize_child(OBJECT(machine), "s32k", &m_state->s32k,
                            TYPE_NXPS32K358_SOC);
    DeviceState *soc_state = DEVICE(&m_state->s32k);
    qdev_connect_clock_in(soc_state, "sysclk", m_state->sysclk);
    sysbus_realize(SYS_BUS_DEVICE(&m_state->s32k), &error_abort);

    // Load kernel image
    armv7m_load_kernel(ARM_CPU(first_cpu), machine->kernel_filename,
                       CODE_FLASH_BASE_ADDRESS, CODE_FLASH_BLOCK_SIZE * 4);
}

/**
 * @brief Initializes the NXPS32K3X8EVB board class.
 *
 * @param oc The ObjectClass to initialize, provided by QEMU.
 * @param data Additional data for initialization (unused).
 *
 * This function sets up the MachineClass for the NXPS32K3X8EVB-Q289 board.
 * It provides a description of the board, specifies the initialization
 * function, and defines CPU attributes such as the default CPU type and
 * the number of CPUs. Additionally, it indicates that the board does not
 * have any media drives (floppy or CD-ROM) and does not support parallel
 * threads. In our implementation we have only one core; in the real thing there
 * are 2 cores.
 */
static void NXPS32K3X8EVB_class_init(ObjectClass *oc, void *data) {
    MachineClass *mc = MACHINE_CLASS(oc);
    mc->desc = "S32K3X8EVB-Q289 board";

    static const char *const valid_cpu_types[] = {
        ARM_CPU_TYPE_NAME("cortex-m7"), NULL};

    mc->init = NXPS32K3X8EVB_init;
    mc->default_cpus = 1;
    mc->default_cpu_type = ARM_CPU_TYPE_NAME("cortex-m7");
    mc->valid_cpu_types = valid_cpu_types;
    mc->min_cpus = mc->default_cpus;
    mc->max_cpus = mc->default_cpus;
    mc->no_floppy = 1;
    mc->no_cdrom = 1;
    mc->no_parallel = 1;
}

static const TypeInfo NXPS32K3X8EVB_machine_types[] = {{
    .name = TYPE_NXPS32K3X8EVB_MACHINE,
    .parent = TYPE_MACHINE,
    .instance_size = sizeof(NXPS32K3X8EVBMachineState),
    .class_size = sizeof(NXPS32K3X8EVBMachineClass),
    .class_init = NXPS32K3X8EVB_class_init,
}};
DEFINE_TYPES(NXPS32K3X8EVB_machine_types)
