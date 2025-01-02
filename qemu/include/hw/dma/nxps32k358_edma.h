#ifndef NXP_S32K358_EDMA_H
#define NXP_S32K358_EDMA_H

#include "hw/sysbus.h"
#include "qom/object.h"
#include "hw/registerfields.h"

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

REG32()

#define EDMA_CHANNELS 32

struct NXPS32K358eDMAChannel {
    uint32_t ch_csr;
    uint32_t ch_es;
    uint32_t ch_int;
    uint32_t ch_sbr;
    uint32_t ch_pri;
    uint32_t tcd_saddr;
    uint16_t tcd_soff;
    uint16_t tcd_attr;
    uint32_t tcd_nbytes_mloffno;
    uint32_t tcd_nbytes_mloffyes;
    uint32_t tcd_slast_sda;
    uint32_t tcd_daddr;
    uint16_t tcd_doff;
    uint16_t tcd_citer_elinkno;
    uint16_t tcd_citer_elinkyes;
    uint32_t tcd_dlast_sga;
    uint16_t tcd_csr;
    uint16_t tcd_biter_elinkno;
    uint16_t tcd_biter_elinkyes;
};

#define TYPE_NXPS32K358_EDMA "nxps32k358-edma"
OBJECT_DECLARE_SIMPLE_TYPE(NXPS32K358eDMAState, NXPS32K358_EDMA)

struct NXPS32K358eDMAState {
    SysBusDevice parent_obj;
    MemoryRegion mmio;

    uint32_t edma_csr;
    uint32_t edma_es;   // READ ONLY
    uint32_t edma_int;  // READ ONLY
    uint32_t edma_hrs;  // READ ONLY
    uint32_t edma_chn_grpri[EDMA_CHANNELS];
    NXPS32K358eDMAChannel tcd[EDMA_CHANNELS];
    qemu_irq irq;
};

#endif
