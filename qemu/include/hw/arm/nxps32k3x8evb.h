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
 * @file nxps32k3x8evb.h
 * @brief Definition of the NXPS32K3X8EVB board emulation.
 */

#ifndef HW_ARM_NXPS32K3X8EVB_H
#define HW_ARM_NXPS32K3X8EVB_H

#include "qom/object.h"
#include "hw/boards.h"
#include "hw/arm/nxps32k358_soc.h"
#include "hw/qdev-clock.h"

#define SYSCLK_FRQ 160000000ULL

/**
 * @struct NXPS32K3X8EVBMachineState
 * @brief Represents the state of the NXPS32K3X8EVB machine.
 *
 * @var NXPS32K3X8EVBMachineState::parent_obj
 * The parent MachineState object.
 *
 * @var NXPS32K3X8EVBMachineState::s32k
 * The state specific to the NXPS32K358.
 *
 * @var NXPS32K3X8EVBMachineState::sysclk
 * Pointer to the system clock.
 */
struct NXPS32K3X8EVBMachineState {
    MachineState parent_obj;
    NXPS32K358State s32k;

    Clock *sysclk;
};
typedef struct NXPS32K3X8EVBMachineState NXPS32K3X8EVBMachineState;

/**
 * @struct NXPS32K3X8EVBMachineClass
 * @brief Represents the machine class for the NXPS32K3X8EVB board.
 *
 * @var NXPS32K3X8EVBMachineClass::parent_class
 * The parent machine class which this structure extends.
 */
struct NXPS32K3X8EVBMachineClass {
    MachineClass parent_class;
};
typedef struct NXPS32K3X8EVBMachineClass NXPS32K3X8EVBMachineClass;

#define TYPE_NXPS32K3X8EVB_MACHINE MACHINE_TYPE_NAME("nxps32k3x8evb")

OBJECT_DECLARE_TYPE(NXPS32K3X8EVBMachineState, NXPS32K3X8EVBMachineClass,
                    NXPS32K3X8EVB_MACHINE)

#endif