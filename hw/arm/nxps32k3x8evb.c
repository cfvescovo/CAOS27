#include "qemu/osdep.h"
#include "qemu/units.h"
#include "qapi/error.h"
#include "hw/arm/boot.h"
#include "hw/arm/armv7m.h"
#include "qom/object.h"
#include "hw/boards.h"
#include "hw/arm/nxps32k358_soc.h"
#include "hw/qdev-clock.h"

#define SYSCLK_FRQ 240000000ULL

struct NXPS32K3X8EVBMachineState {
    MachineState parent_obj;
    NXPS32K358State s32k;

    Clock *sysclk;
};
typedef struct NXPS32K3X8EVBMachineState NXPS32K3X8EVBMachineState;

struct NXPS32K3X8EVBMachineClass {
    MachineClass parent_class;
};
typedef struct NXPS32K3X8EVBMachineClass NXPS32K3X8EVBMachineClass;

#define TYPE_NXPS32K3X8EVB_MACHINE MACHINE_TYPE_NAME("nxps32k3x8evb")

DECLARE_OBJ_CHECKERS(NXPS32K3X8EVBMachineState, NXPS32K3X8EVBMachineClass,
                     NXPS32K3X8EVB_MACHINE, TYPE_NXPS32K3X8EVB_MACHINE)

// Here we initialize the board
// The generic MachineState is passed by QEMU
static void NXPS32K3X8EVB_init(MachineState *machine) {
    // Cast the NXP machine from the generic machine
    NXPS32K3X8EVBMachineState *m_state = NXPS32K3X8EVB_MACHINE(machine);

    // Initialize system clock
    m_state->sysclk = clock_new(OBJECT(machine), "SYSCLK");

    // Initialize the SoC
    object_initialize_child(OBJECT(machine), "s32k", &m_state->s32k,
                            TYPE_NXPS32K358_SOC);

    DeviceState *soc_state = DEVICE(&m_state->s32k);
    // We link the machine sysclk to SoC sysclk
    clock_set_hz(m_state->sysclk, SYSCLK_FRQ);
    qdev_connect_clock_in(soc_state, "sysclk", m_state->sysclk);

    // And we connect it via QEMU's SYSBUS.
    sysbus_realize(SYS_BUS_DEVICE(&m_state->s32k), &error_abort);

    // Finally we load the kernel image at address 0x400000 (beginning of flash
    // ram)
    armv7m_load_kernel(ARM_CPU(first_cpu), machine->kernel_filename,
                       CODE_FLASH_BASE_ADDRESS, CODE_FLASH_BLOCK_SIZE * 4);
}

// Generic Objectc is passed by QEMU
static void NXPS32K3X8EVB_class_init(ObjectClass *oc, void *data) {
    // The generic machine class from object
    MachineClass *mc = MACHINE_CLASS(oc);
    mc->desc = "S32K3X8EVB-Q289 board";

    static const char *const valid_cpu_types[] = {
        ARM_CPU_TYPE_NAME("cortex-m7"), NULL};

    // Notice that we tell QEMU what function is used to initialize our board
    // here.
    mc->init = NXPS32K3X8EVB_init;
    // Define CPU attributes
    mc->default_cpus = 1;
    mc->default_cpu_type = ARM_CPU_TYPE_NAME("cortex-m7");
    mc->valid_cpu_types = valid_cpu_types;
    mc->min_cpus = mc->default_cpus;
    mc->max_cpus = mc->default_cpus;
    // Our board does not have any media drive
    mc->no_floppy = 1;
    mc->no_cdrom = 1;
    // We also will not have threads
    mc->no_parallel = 1;
}

static const TypeInfo NXPS32K3X8EVB_machine_types[] = {{
    // Notice that this is the TYPE that we defined above.
    .name = TYPE_NXPS32K3X8EVB_MACHINE,
    // Our machine is a direct child of QEMU generic machine
    .parent = TYPE_MACHINE,
    .instance_size = sizeof(NXPS32K3X8EVBMachineState),
    .class_size = sizeof(NXPS32K3X8EVBMachineClass),
    // We need to registers the class inti function
    .class_init = NXPS32K3X8EVB_class_init,
}};
DEFINE_TYPES(NXPS32K3X8EVB_machine_types)
