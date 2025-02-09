#ifndef NXP_S32K358_EDMA_H
#define NXP_S32K358_EDMA_H

#include "hw/sysbus.h"
#include "qom/object.h"
#include "hw/registerfields.h"
#include "hw/dma/nxps32k358_tcd.h"

#define READONLY

REG32(EDMA_CSR, 0x00)
FIELD(EDMA_CSR, EDBG, 1, 1)
FIELD(EDMA_CSR, ERCA, 2, 1)
FIELD(EDMA_CSR, HALT, 5, 1)
FIELD(EDMA_CSR, ECX, 8, 1)
FIELD(EDMA_CSR, CX, 9, 1)
FIELD(EDMA_CSR, ACTIVE_ID, 24, 5)
FIELD(EDMA_CSR, ACTIVE, 31, 1)

REG32(EDMA_ES, 0x04)
FIELD(EDMA_ES, ECX, 8, 1)
FIELD(EDMA_ES, ERRCHN, 24, 5)
FIELD(EDMA_ES, VLD, 31, 1)

REG32(EDMA_INT, 0x08)
FIELD(EDMA_INT, INT, 0, 32)

REG32(EDMA_HRS, 0x0C)
FIELD(EDMA_HRS, HRS, 0, 32)

#define EDMA_CHN_GRPRI_BASE_ADDR 0x100
#define EDMA_CHN_GRPRI_NUM 32

static inline uint32_t A_EDMA_CHN_GRPRI(int n) {
    return EDMA_CHN_GRPRI_BASE_ADDR + 4 * n;
}
static inline uint32_t R_EDMA_CHN_GRPRI(int n) {
    return EDMA_CHN_GRPRI_BASE_ADDR / 4 + n;
}
static inline uint32_t R_EDMA_CHN_GRPRI_GRPRI_MASK(int n) {
    return MAKE_64BIT_MASK(0, 5);
}

#define EDMA_CSR_RESET 0x00300000
#define EDMA_ES_RESET 0x00000000
#define EDMA_INT_RESET 0x00000000
#define EDMA_HRS_RESET 0x00000000
#define EDMA_CHN_GRPRI_RESET 0x00000000

#define EDMA_CHANNELS 32

#define TYPE_NXPS32K358_EDMA "nxps32k358-edma"
OBJECT_DECLARE_SIMPLE_TYPE(NXPS32K358EDMAState, NXPS32K358_EDMA)

struct NXPS32K358EDMAState {
    SysBusDevice parent_obj;
    MemoryRegion mmio0;
    MemoryRegion mmio12;

    uint32_t edma_csr;
    uint32_t READONLY edma_es;
    uint32_t READONLY edma_int;
    uint32_t READONLY edma_hrs;
    uint32_t edma_chn_grpri[EDMA_CHANNELS];
    struct NXPS32K358EDMATCDState tcd[EDMA_CHANNELS];
};

#endif
