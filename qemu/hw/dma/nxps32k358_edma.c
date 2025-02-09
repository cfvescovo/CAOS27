/*
 * NXP S32K358 eDMA controller - Copyright (c) 2025 CFV & Giovanni
 *
 * This work is licensed under the terms of the GNU GPL, version 2 or later.
 * See the COPYING file in the top-level directory.
 */

#include "qemu/osdep.h"
#include "qapi/error.h"
#include "hw/dma/nxps32k358_edma.h"
#include "hw/dma/nxps32k358_tcd.h"
#include "hw/irq.h"
#include "migration/vmstate.h"
#include "qemu/log.h"
#include "qemu/module.h"

#define READONLY
#define NO_SUPPORT

#define MAX_SIZE 64

static void nxps32k358_edma_transmit(NXPS32K358EDMAState *s, int tcd_no) {
    uint8_t buf[MAX_SIZE];
    struct NXPS32K358EDMATCDState *ch = &s->tcd[tcd_no];

    uint32_t saddr = 0;
    uint32_t daddr = 0;

    uint32_t ssize = FIELD_EX32(ch->tcd_attr, TCD_ATTR, SSIZE);
    uint32_t dsize = FIELD_EX32(ch->tcd_attr, TCD_ATTR, DSIZE);

    // These values are reserved
    assert(ssize != 0b111);
    assert(dsize != 0b111);

    ssize = 1 << ssize;
    dsize = 1 << dsize;

    uint32_t nbytes =
        FIELD_EX32(ch->tcd_nbytes_mloff, TCD_NBYTES_MLOFF, NBYTES);

    uint32_t max_size = ssize > dsize ? ssize : dsize;

    // Major Loop
    if (FIELD_EX32(ch->tcd_citer, TCD_CITER, CITER) > 0) {
        saddr = ch->tcd_saddr;
        daddr = ch->tcd_daddr;

        // Minor Loop
        for (int i = 0; i < nbytes / max_size; i++) {
            // Read from source
            for (int j = 0; j < max_size / ssize; j++) {
                cpu_physical_memory_read(saddr, buf, ssize);
                saddr += ch->tcd_soff;
            }

            // Write to destination
            for (int j = 0; j < max_size / dsize; j++) {
                cpu_physical_memory_write(daddr, buf, dsize);
                daddr += ch->tcd_doff;
            }
        }

        // In theory it should be incremented by mloff if smloe/dmloe
        // are active
        ch->tcd_saddr = saddr;
        ch->tcd_daddr = daddr;

        // Decrement current iteration counter
        uint32_t next_citer = FIELD_EX32(ch->tcd_citer, TCD_CITER, CITER) - 1;
        ch->tcd_citer = FIELD_DP32(ch->tcd_citer, TCD_CITER, CITER, next_citer);

        // Disable ACTIVE after first minor loop is completed
        ch->ch_csr &= ~R_CH_CSR_ACTIVE_MASK;
    }

    if (FIELD_EX32(ch->tcd_citer, TCD_CITER, CITER) == 0) {
        uint8_t esda = FIELD_EX32(ch->tcd_csr, TCD_CSR, ESDA);
        if (esda) {
            uint32_t slast_sda =
                FIELD_EX32(ch->tcd_slast_sda, TCD_SLAST_SDA, SLAST_SDA);
            cpu_physical_memory_write(slast_sda, (void *)ch->tcd_daddr,
                                      sizeof(slast_sda));
        } else {
            ch->tcd_saddr =
                ch->tcd_saddr +
                FIELD_EX32(ch->tcd_slast_sda, TCD_SLAST_SDA, SLAST_SDA);
        }

        uint8_t esg = FIELD_EX32(ch->tcd_csr, TCD_CSR, ESG);
        if (esg) {
            uint32_t dlast_sga =
                FIELD_EX32(ch->tcd_dlast_sga, TCD_DLAST_SGA, DLAST_SGA);
            uint8_t next_tcd_data[32];
            cpu_physical_memory_read(dlast_sga, next_tcd_data, 32);

            assert(((uint32_t)&ch->tcd_biter + sizeof(ch->tcd_biter) -
                    (uint32_t)&ch->tcd_saddr) == 32);
            memcpy(&ch->tcd_saddr, next_tcd_data, 32);
        } else {
            ch->tcd_daddr =
                ch->tcd_daddr +
                FIELD_EX32(ch->tcd_dlast_sga, TCD_DLAST_SGA, DLAST_SGA);
        }

        FIELD_DP32(ch->tcd_citer, TCD_CITER, CITER,
                   FIELD_EX32(ch->tcd_biter, TCD_BITER, BITER));

        ch->ch_csr |= R_CH_CSR_DONE_MASK;
        ch->ch_int |= R_CH_INT_INT_MASK;
    }
}

static void nxps32k358_edma_arbitrate(NXPS32K358EDMAState *s) {
    static int offset = 0;

    // This is a round-robin
    for (int i = 0; i < EDMA_CHANNELS; i++) {
        int j = (i + offset) % EDMA_CHANNELS;

        if (s->tcd[j].tcd_csr & R_TCD_CSR_START_MASK) {
            s->tcd[j].ch_csr &= ~R_CH_CSR_DONE_MASK;
            s->tcd[j].tcd_csr &= ~R_TCD_CSR_START_MASK;
            s->tcd[j].ch_csr |= R_CH_CSR_ACTIVE_MASK;
            nxps32k358_edma_transmit(s, j);
            offset = (j + 1) % EDMA_CHANNELS;
            return;
        }
    }
}

static uint64_t nxps32k358_edma_tcd_read(NXPS32K358EDMAState *s, hwaddr offset,
                                         unsigned size, unsigned c) {
    uint32_t res = 0;

    struct NXPS32K358EDMATCDState *ch = &s->tcd[c];

    switch (offset) {
        case A_CH_CSR:
            res = ch->ch_csr;
            break;
        case A_CH_ES:
            res = ch->ch_es;
            break;
        case A_CH_INT:
            res = ch->ch_int;
            break;
        case A_CH_SBR:
            res = ch->ch_sbr;
            break;
        case A_CH_PRI:
            res = ch->ch_pri;
            break;
        case A_TCD_SADDR:
            res = ch->tcd_saddr;
            break;
        case A_TCD_SOFF:
            res = ch->tcd_soff;
            break;
        case A_TCD_ATTR:
            res = ch->tcd_attr;
            break;
        case A_TCD_NBYTES_MLOFF:
            res = ch->tcd_nbytes_mloff;
            break;
        case A_TCD_SLAST_SDA:
            res = ch->tcd_slast_sda;
            break;
        case A_TCD_DADDR:
            res = ch->tcd_daddr;
            break;
        case A_TCD_DOFF:
            res = ch->tcd_doff;
            break;
        case A_TCD_CITER:
            res = ch->tcd_citer;
            break;
        case A_TCD_DLAST_SGA:
            res = ch->tcd_dlast_sga;
            break;
        case A_TCD_CSR:
            res = ch->tcd_csr;
            break;
        case A_TCD_BITER:
            res = ch->tcd_biter;
            break;
        default:
            qemu_log_mask(LOG_GUEST_ERROR,
                          "%s: Bad offset 0x%" HWADDR_PRIx "\n", __func__,
                          offset);
            break;
    }

    return res;
}

static void nxps32k358_edma_tcd_write(NXPS32K358EDMAState *s, hwaddr offset,
                                      uint64_t value, unsigned size,
                                      unsigned c) {
    struct NXPS32K358EDMATCDState *ch = &s->tcd[c];

    switch (offset) {
        case A_CH_CSR:
            ch->ch_csr &= ~R_CH_CSR_DONE_MASK;
            ch->ch_csr |= value & R_CH_CSR_DONE_MASK;
            // TODO: last 4 bits are writable too
            break;
        case A_CH_ES:
            // SW should be able to clear the error status (first bit)
            ch->ch_es &= ~R_CH_ES_ERR_MASK;
            ch->ch_es |= value & R_CH_ES_ERR_MASK;
            break;
        case A_CH_INT:
            ch->ch_int &= ~R_CH_INT_INT_MASK;
            ch->ch_int |= value & R_CH_INT_INT_MASK;
            break;
        case NO_SUPPORT A_CH_SBR:
        case NO_SUPPORT A_CH_PRI:
            break;
        case A_TCD_SADDR:
            ch->tcd_saddr = value;
            break;
        case A_TCD_SOFF:
            ch->tcd_soff = value;
            break;
        case A_TCD_ATTR:
            ch->tcd_attr = value;
            break;
        case A_TCD_NBYTES_MLOFF:
            // no support for SMLOE and DMLOE
            assert((value & R_TCD_NBYTES_MLOFF_SMLOE_MASK) == 0);
            assert((value & R_TCD_NBYTES_MLOFF_DMLOE_MASK) == 0);
            ch->tcd_nbytes_mloff = value;
            break;
        case A_TCD_SLAST_SDA:
            ch->tcd_slast_sda = value;
            break;
        case A_TCD_DADDR:
            ch->tcd_daddr = value;
            break;
        case A_TCD_DOFF:
            ch->tcd_doff = value;
            break;
        case A_TCD_CITER:
            // no support for channel linking
            assert((value & R_TCD_CITER_ELINK_MASK) == 0);
            assert(FIELD_EX32(ch->tcd_citer, TCD_CITER, CITER) ==
                   FIELD_EX32(ch->tcd_biter, TCD_BITER, BITER));
            ch->tcd_citer = value;
            break;
        case A_TCD_DLAST_SGA:
            ch->tcd_dlast_sga = value;
            break;
        case A_TCD_CSR:
            // no support for bwc, don't care
            assert((value & R_TCD_CSR_MAJORELINK_MASK) == 0);
            if (value & R_TCD_CSR_START_MASK) {
                nxps32k358_edma_arbitrate(s);
            }
            break;
        case A_TCD_BITER:
            assert((value & R_TCD_BITER_ELINK_MASK) == 0);
            // no support for beginning iteration count greater than 1
            assert((value & R_TCD_BITER_BITER_MASK) <= 1);
            ch->tcd_biter = value;
            break;
        default:
            qemu_log_mask(LOG_GUEST_ERROR,
                          "%s: Bad offset 0x%" HWADDR_PRIx "\n", __func__,
                          offset);
            break;
    }
}

static void nxps32k358_edma_tcd_reset(struct NXPS32K358EDMATCDState *s) {
    s->ch_csr = 0;
    s->ch_es = 0;
    s->ch_int = 0;
    s->ch_sbr = 0x00008002;
    s->ch_pri = 0;
    s->tcd_saddr = 0;
    s->tcd_soff = 0;
    s->tcd_attr = 0;
    s->tcd_nbytes_mloff = 0;
    s->tcd_slast_sda = 0;
    s->tcd_daddr = 0;
    s->tcd_doff = 0;
    s->tcd_citer = 0;
    s->tcd_dlast_sga = 0;
    s->tcd_csr = 0;
    s->tcd_biter = 0;
}

static uint64_t nxps32k358_edma_global_read(NXPS32K358EDMAState *s,
                                            hwaddr offset, unsigned size) {
    uint32_t res = 0;

    switch (offset) {
        case A_EDMA_CSR:
            res = s->edma_csr;
            break;
        case A_EDMA_ES:
            res = s->edma_es;
            break;
        case A_EDMA_INT:
            res = s->edma_int;
            break;
        case A_EDMA_HRS:
            res = s->edma_hrs;
            break;
        default:
            if (offset >= A_EDMA_CHN_GRPRI(0) &&
                offset < A_EDMA_CHN_GRPRI(EDMA_CHANNELS)) {
                int n = (offset - A_EDMA_CHN_GRPRI(0)) / 4;
                res = s->edma_chn_grpri[n];
                break;
            }
            qemu_log_mask(LOG_GUEST_ERROR,
                          "%s: Bad offset 0x%" HWADDR_PRIx "\n", __func__,
                          offset);
            break;
    }

    return res;
}

static void nxps32k358_edma_global_write(NXPS32K358EDMAState *s, hwaddr offset,
                                         uint64_t value, unsigned size) {
    const uint32_t CSR_WR_MASK = 0x000003f6;
    const uint32_t GRPRI_WR_MASK = 0x0000001f;
    switch (offset) {
        case A_EDMA_CSR:
            s->edma_csr &= ~CSR_WR_MASK;
            s->edma_csr |= value & CSR_WR_MASK;
            break;
        case A_EDMA_ES:
        case A_EDMA_INT:
        case A_EDMA_HRS:
            READONLY break;
        default:
            if (offset >= A_EDMA_CHN_GRPRI(0) &&
                offset < A_EDMA_CHN_GRPRI(EDMA_CHANNELS)) {
                int n = (offset - A_EDMA_CHN_GRPRI(0)) / 4;
                s->edma_chn_grpri[n] &= ~GRPRI_WR_MASK;
                s->edma_chn_grpri[n] |= value & GRPRI_WR_MASK;
                break;
            }
            qemu_log_mask(LOG_GUEST_ERROR,
                          "%s: Bad offset 0x%" HWADDR_PRIx "\n", __func__,
                          offset);
            break;
    }
}

static uint64_t nxps32k358_edma0_read(void *obj, hwaddr offset, unsigned size) {
    NXPS32K358EDMAState *s = obj;
    if (offset >= 0x4000) {
        hwaddr tcd_offset = offset % 0x4000;
        unsigned tcd_num = (offset - 0x4000) / 0x4000;
        return nxps32k358_edma_tcd_read(s, tcd_offset, size, tcd_num);
    } else {
        return nxps32k358_edma_global_read(s, offset, size);
    }
}

static void nxps32k358_edma0_write(void *obj, hwaddr offset, uint64_t value,
                                   unsigned size) {
    NXPS32K358EDMAState *s = obj;
    if (offset >= 0x4000) {
        hwaddr tcd_offset = offset % 0x4000;
        unsigned tcd_num = (offset - 0x4000) / 0x4000;
        return nxps32k358_edma_tcd_write(s, tcd_offset, value, size, tcd_num);
    } else {
        return nxps32k358_edma_global_write(s, offset, value, size);
    }
}

static uint64_t nxps32k358_edma12_read(void *obj, hwaddr offset,
                                       unsigned size) {
    NXPS32K358EDMAState *s = obj;
    hwaddr tcd_offset = offset % 0x4000;
    unsigned tcd_num = offset / 0x4000 + 12;
    return nxps32k358_edma_tcd_read(s, tcd_offset, size, tcd_num);
}

static void nxps32k358_edma12_write(void *obj, hwaddr offset, uint64_t value,
                                    unsigned size) {
    NXPS32K358EDMAState *s = obj;
    hwaddr tcd_offset = offset % 0x4000;
    unsigned tcd_num = offset / 0x4000 + 12;
    return nxps32k358_edma_tcd_write(s, tcd_offset, value, size, tcd_num);
}

static const MemoryRegionOps nxps32k358_edma0_ops = {
    .read = nxps32k358_edma0_read,
    .write = nxps32k358_edma0_write,
    .endianness = DEVICE_NATIVE_ENDIAN,
};

static const MemoryRegionOps nxps32k358_edma12_ops = {
    .read = nxps32k358_edma12_read,
    .write = nxps32k358_edma12_write,
    .endianness = DEVICE_NATIVE_ENDIAN,
};

static void nxps32k358_edma_init(Object *obj) {
    NXPS32K358EDMAState *s = NXPS32K358_EDMA(obj);
    int n;

    memory_region_init_io(&s->mmio0, OBJECT(s), &nxps32k358_edma0_ops, s,
                          TYPE_NXPS32K358_EDMA, 0x4000 * 13);
    sysbus_init_mmio(SYS_BUS_DEVICE(s), &s->mmio0);

    memory_region_init_io(&s->mmio12, OBJECT(s), &nxps32k358_edma12_ops, s,
                          TYPE_NXPS32K358_EDMA "-tcd12", 0x4000 * 20);
    sysbus_init_mmio(SYS_BUS_DEVICE(s), &s->mmio12);

    for (int i = 0; i < EDMA_CHANNELS; i++) {
        sysbus_init_irq(SYS_BUS_DEVICE(s), &s->tcd[n].irq);
    }
}

static void nxps32k358_edma_reset(DeviceState *dev) {
    NXPS32K358EDMAState *s = NXPS32K358_EDMA(dev);

    s->edma_csr = EDMA_CSR_RESET;
    s->edma_es = EDMA_ES_RESET;
    s->edma_int = EDMA_INT_RESET;
    s->edma_hrs = EDMA_HRS_RESET;
    for (int i = 0; i < EDMA_CHANNELS; i++) {
        s->edma_chn_grpri[i] = EDMA_CHN_GRPRI_RESET;
        nxps32k358_edma_tcd_reset(&s->tcd[i]);
    }
}

static void nxps32k358_edma_realize(DeviceState *dev, Error **errp) {
    nxps32k358_edma_reset(dev);
}

static void nxps32k358_edma_class_init(ObjectClass *klass, void *data) {
    DeviceClass *dc = DEVICE_CLASS(klass);

    dc->realize = nxps32k358_edma_realize;
    device_class_set_legacy_reset(dc, nxps32k358_edma_reset);
}

static const TypeInfo nxps32k358_edma_info = {
    .name = TYPE_NXPS32K358_EDMA,
    .parent = TYPE_SYS_BUS_DEVICE,
    .instance_size = sizeof(NXPS32K358EDMAState),
    .class_init = nxps32k358_edma_class_init,
    .instance_init = nxps32k358_edma_init,
};

static void nxps32k358_edma_register_types(void) {
    type_register_static(&nxps32k358_edma_info);
}

type_init(nxps32k358_edma_register_types)
