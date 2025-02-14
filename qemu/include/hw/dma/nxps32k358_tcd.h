#ifndef NXP_S32K358_EDMA_TCD_H
#define NXP_S32K358_EDMA_TCD_H

#include "hw/registerfields.h"

REG32(CH_CSR, 0x0)
FIELD(CH_CSR, EEI, 2, 1)
FIELD(CH_CSR, DONE, 30, 1)
FIELD(CH_CSR, ACTIVE, 31, 1)

REG32(CH_ES, 0x4)
FIELD(CH_ES, ERR, 31, 1)

REG32(CH_INT, 0x8)
FIELD(CH_INT, INT, 0, 1)

REG32(CH_SBR, 0xC)

REG32(CH_PRI, 0x10)

REG32(TCD_SADDR, 0x20)
REG16(TCD_SOFF, 0x24)

REG16(TCD_ATTR, 0x26)
FIELD(TCD_ATTR, SSIZE, 8, 3)
FIELD(TCD_ATTR, DSIZE, 0, 3)

// No support for minor loop offset enable
REG32(TCD_NBYTES_MLOFF, 0x28)
FIELD(TCD_NBYTES_MLOFF, NBYTES, 0, 30)
FIELD(TCD_NBYTES_MLOFF, DMLOE, 30, 1)
FIELD(TCD_NBYTES_MLOFF, SMLOE, 31, 1)

REG32(TCD_SLAST_SDA, 0x2C)
FIELD(TCD_SLAST_SDA, SLAST_SDA, 0, 32)

REG32(TCD_DADDR, 0x30)
REG16(TCD_DOFF, 0x34)

REG16(TCD_CITER, 0x36)
FIELD(TCD_CITER, ELINK, 15, 1)
FIELD(TCD_CITER, CITER, 0, 15)

REG32(TCD_DLAST_SGA, 0x38)
FIELD(TCD_DLAST_SGA, DLAST_SGA, 0, 32)

REG16(TCD_CSR, 0x3C)
FIELD(TCD_CSR, START, 0, 1)
FIELD(TCD_CSR, INTMAJOR, 1, 1)
FIELD(TCD_CSR, INTHALF, 2, 1)
FIELD(TCD_CSR, ESG, 4, 1)
FIELD(TCD_CSR, ESDA, 7, 1)
FIELD(TCD_CSR, MAJORELINK, 5, 1)

REG16(TCD_BITER, 0x3E)
FIELD(TCD_BITER, ELINK, 15, 1)
FIELD(TCD_BITER, BITER, 0, 15)

struct NXPS32K358EDMATCDState {
    uint32_t ch_csr;
    uint32_t ch_es;
    uint32_t ch_int;
    uint32_t ch_sbr;
    uint32_t ch_pri;
    uint32_t tcd_saddr;
    int16_t tcd_soff;
    uint16_t tcd_attr;
    uint32_t tcd_nbytes_mloff;
    // No support for minor loop offset enable
    // uint32_t tcd_nbytes_mloffno;
    uint32_t tcd_slast_sda;
    uint32_t tcd_daddr;
    uint16_t tcd_doff;
    uint16_t tcd_citer;
    // Channel linking is not supported
    // uint16_t tcd_citer_elinkyes;
    uint32_t tcd_dlast_sga;
    uint16_t tcd_csr;
    uint16_t tcd_biter;
    // uint16_t tcd_biter_elinkyes;

    qemu_irq irq;
};

#endif
