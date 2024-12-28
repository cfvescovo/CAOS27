/*==================================================================================================
*   Project              : RTD AUTOSAR 4.7
*   Platform             : CORTEXM
*   Peripheral           : DMA,CACHE,TRGMUX,LCU,EMIOS,FLEXIO
*   Dependencies         : none
*
*   Autosar Version      : 4.7.0
*   Autosar Revision     : ASR_REL_4_7_REV_0000
*   Autosar Conf.Variant :
*   SW Version           : 3.0.0
*   Build Version        : S32K3_RTD_3_0_0_D2303_ASR_REL_4_7_REV_0000_20230331
*
*   Copyright 2020 - 2023 NXP Semiconductors
*   
*
*   NXP Confidential. This software is owned or controlled by NXP and may only be
*   used strictly in accordance with the applicable license terms. By expressly
*   accepting such terms or by downloading, installing, activating and/or otherwise
*   using the software, you are agreeing that you have read, and that you agree to
*   comply with and are bound by, such license terms. If you do not agree to be
*   bound by the applicable license terms, then you may not retain, install,
*   activate or otherwise use the software.
==================================================================================================*/

#ifdef __cplusplus
extern "C"
{
#endif

/*==================================================================================================
*                                        INCLUDE FILES
* 1) system and project includes
* 2) needed interfaces from external units
* 3) internal and external interfaces from this unit
==================================================================================================*/
#include "Dma_Ip_Cfg.h"

/*==================================================================================================
*                                    SOURCE FILE VERSION INFORMATION
==================================================================================================*/
#define DMA_IP_BOARD_INITPERIPHERALS_PBCFG_MODULE_ID_C                     255
#define DMA_IP_BOARD_INITPERIPHERALS_PBCFG_VENDOR_ID_C                     43
#define DMA_IP_BOARD_INITPERIPHERALS_PBCFG_AR_RELEASE_MAJOR_VERSION_C      4
#define DMA_IP_BOARD_INITPERIPHERALS_PBCFG_AR_RELEASE_MINOR_VERSION_C      7
#define DMA_IP_BOARD_INITPERIPHERALS_PBCFG_AR_RELEASE_REVISION_VERSION_C   0
#define DMA_IP_BOARD_INITPERIPHERALS_PBCFG_SW_MAJOR_VERSION_C              3
#define DMA_IP_BOARD_INITPERIPHERALS_PBCFG_SW_MINOR_VERSION_C              0
#define DMA_IP_BOARD_INITPERIPHERALS_PBCFG_SW_PATCH_VERSION_C              0

/*==================================================================================================
*                                     FILE VERSION CHECKS
==================================================================================================*/
/* Check if Dma_Ip_BOARD_InitPeripherals_PBcfg.c file and Dma_Ip_Cfg.h file are of the same vendor */
#if (DMA_IP_BOARD_INITPERIPHERALS_PBCFG_VENDOR_ID_C != DMA_IP_CFG_VENDOR_ID)
    #error "Dma_Ip_BOARD_InitPeripherals_PBcfg.c and Dma_Ip_Cfg.h have different vendor ids"
#endif

/* Check if Dma_Ip_BOARD_InitPeripherals_PBcfg.c file and Dma_Ip_Cfg.h file are of the same Autosar version */
#if ((DMA_IP_BOARD_INITPERIPHERALS_PBCFG_AR_RELEASE_MAJOR_VERSION_C != DMA_IP_CFG_AR_RELEASE_MAJOR_VERSION) || \
     (DMA_IP_BOARD_INITPERIPHERALS_PBCFG_AR_RELEASE_MINOR_VERSION_C != DMA_IP_CFG_AR_RELEASE_MINOR_VERSION) || \
     (DMA_IP_BOARD_INITPERIPHERALS_PBCFG_AR_RELEASE_REVISION_VERSION_C != DMA_IP_CFG_AR_RELEASE_REVISION_VERSION) \
    )
    #error "AutoSar Version Numbers of Dma_Ip_BOARD_InitPeripherals_PBcfg.c and Dma_Ip_Cfg.h are different"
#endif

/* Check if Dma_Ip_BOARD_InitPeripherals_PBcfg.c file and Dma_Ip_Cfg.h file are of the same Software version */
#if ((DMA_IP_BOARD_INITPERIPHERALS_PBCFG_SW_MAJOR_VERSION_C != DMA_IP_CFG_SW_MAJOR_VERSION) || \
     (DMA_IP_BOARD_INITPERIPHERALS_PBCFG_SW_MINOR_VERSION_C != DMA_IP_CFG_SW_MINOR_VERSION) || \
     (DMA_IP_BOARD_INITPERIPHERALS_PBCFG_SW_PATCH_VERSION_C != DMA_IP_CFG_SW_PATCH_VERSION) \
    )
    #error "Software Version Numbers of Dma_Ip_BOARD_InitPeripherals_PBcfg.c and Dma_Ip_Cfg.h are different"
#endif


/*==================================================================================================
*                                        LOCAL MACROS
==================================================================================================*/

/*==================================================================================================
                          LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
==================================================================================================*/

/*==================================================================================================
                                       LOCAL CONSTANTS
==================================================================================================*/

/*==================================================================================================
                                       LOCAL VARIABLES
==================================================================================================*/

/*==================================================================================================
                                       GLOBAL CONSTANTS
==================================================================================================*/

/*==================================================================================================
                                       GLOBAL VARIABLES
==================================================================================================*/

/*==================================================================================================
                                   LOCAL FUNCTION PROTOTYPES
==================================================================================================*/

/*==================================================================================================
                                       LOCAL FUNCTIONS
==================================================================================================*/

/*==================================================================================================
                                       GLOBAL FUNCTIONS
==================================================================================================*/

#define MCL_START_SEC_CONFIG_DATA_UNSPECIFIED
/* @violates @ref Mcl_Dma_h_REF_1 MISRA 2012 Required Directive 4.10, Precautions shall be taken in order to prevent the contents of a header file being included more than once. */
#include "Mcl_MemMap.h"


/* DMA Logic Instance Configuration */
const Dma_Ip_LogicInstanceConfigType LogicInstance0ConfigPB =
{
	{
		/* uint32 logicInstId; */ DMA_LOGIC_INST_0,
		/* uint8 hwVersId;     */ DMA_IP_HARDWARE_VERSION_3,
		/* uint8 hwInst;       */ DMA_IP_HW_INST_0,
	},
	/* boolean EnDebug; */                 (boolean)FALSE,
	/* boolean EnRoundRobin; */            (boolean)FALSE,
	/* boolean EnHaltAfterError; */        (boolean)FALSE,
	/* boolean EnChLinking; */             (boolean)FALSE,
	/* boolean EnGlMasterIdReplication; */ (boolean)FALSE,
};

/* DMA Logic Instance Configuration Array */
const Dma_Ip_LogicInstanceConfigType * const Dma_Ip_paxLogicInstanceConfigArrayPB[DMA_IP_NOF_CFG_LOGIC_INSTANCES] =
{
		&LogicInstance0ConfigPB,
};


#define MCL_STOP_SEC_CONFIG_DATA_UNSPECIFIED
/* @violates @ref Mcl_Dma_h_REF_1 MISRA 2012 Required Directive 4.10, Precautions shall be taken in order to prevent the contents of a header file being included more than once. */
#include "Mcl_MemMap.h"


#define MCL_START_SEC_CONFIG_DATA_UNSPECIFIED
/* @violates @ref Mcl_Dma_h_REF_1 MISRA 2012 Required Directive 4.10, Precautions shall be taken in order to prevent the contents of a header file being included more than once. */
#include "Mcl_MemMap.h"


/* DMA Logic Channel Configurations */
const Dma_Ip_GlobalConfigType LogicChannel0GlobalConfigPB =
{
	{
		/* boolean EnMasterIdReplication; */ (boolean)FALSE,
		/* boolean EnBufferedWrites; */      (boolean)FALSE,
	}, /* Control */
	{
		/* boolean EnRequest; */             (boolean)FALSE,
	}, /* Request */
	{
		/* boolean EnErrorInt; */            (boolean)TRUE,
	}, /* Interrupt */
	{
		/* uint8 SwapSize; */                (uint8)0U,
	}, /* Swap */
	{
		/* uint8 Group; */                   DMA_IP_GROUP_PRIO0,
		/* uint8 Level; */                   DMA_IP_LEVEL_PRIO0,
		/* boolean EnPreemption; */          (boolean)FALSE,
		/* boolean DisPreempt; */            (boolean)FALSE,
	}, /* Priority */
};
const Dma_Ip_TransferConfigType LogicChannel0TransferConfigPB =
{
	{
		/* uint32 ScatterGatherAddr; */    0U,
		/* uint32 DestinationStoreAddr; */ 0U,
		/* uint8 BandwidthControl; */      DMA_IP_BWC_ENGINE_NO_STALL,
		/* uint8 TransferModeControl; */   0U,
		/* boolean EnStart; */             (boolean)FALSE,
		/* boolean EnMajorInt; */          (boolean)TRUE,
		/* boolean EnHalfMajorInt; */      (boolean)FALSE,
		/* boolean DisAutoHwRequest; */    (boolean)FALSE,
		/* boolean EnEndOfPacketSignal; */ (boolean)FALSE
	}, /* Control */
	{
		/* uint32 addr; */         0U,
		/* sint32 lastAddrAdj; */  0,
		/* sint16 signedOffset; */ 2,
		/* uint8 transferSize; */  DMA_IP_TRANSFER_SIZE_2_BYTE,
		/* uint8 modulo; */        0U,
	}, /* Source */
	{
		/* uint32 addr; */         0U,
		/* sint32 lastAddrAdj; */  0,
		/* sint16 signedOffset; */ 2,
		/* uint8 transferSize; */  DMA_IP_TRANSFER_SIZE_2_BYTE,
		/* uint8 modulo; */        0U,
	}, /* Destination */
	{
		/* uint32 size; */         24U,
		/* sint32 offset; */       0,
		/* uint8 LogicLinkCh; */   0U,
		/* boolean EnLink; */      (boolean)FALSE,
		/* boolean EnSrcOffset; */ (boolean)FALSE,
		/* boolean EnDstOffset; */ (boolean)FALSE,
	}, /* MinorLoop */
	{
		/* uint16 count; */        1U,
		/* uint8 LogicLinkCh; */   0U,
		/* boolean EnLink; */      (boolean)FALSE,
	}, /* MajorLoop */
};
const Dma_Ip_GlobalConfigType LogicChannel1GlobalConfigPB =
{
	{
		/* boolean EnMasterIdReplication; */ (boolean)FALSE,
		/* boolean EnBufferedWrites; */      (boolean)FALSE,
	}, /* Control */
	{
		/* boolean EnRequest; */             (boolean)FALSE,
	}, /* Request */
	{
		/* boolean EnErrorInt; */            (boolean)TRUE,
	}, /* Interrupt */
	{
		/* uint8 SwapSize; */                (uint8)0U,
	}, /* Swap */
	{
		/* uint8 Group; */                   DMA_IP_GROUP_PRIO0,
		/* uint8 Level; */                   DMA_IP_LEVEL_PRIO0,
		/* boolean EnPreemption; */          (boolean)FALSE,
		/* boolean DisPreempt; */            (boolean)FALSE,
	}, /* Priority */
};

#define MCL_STOP_SEC_CONFIG_DATA_UNSPECIFIED
/* @violates @ref Mcl_Dma_h_REF_1 MISRA 2012 Required Directive 4.10, Precautions shall be taken in order to prevent the contents of a header file being included more than once. */
#include "Mcl_MemMap.h"

#define MCL_START_SEC_VAR_CLEARED_UNSPECIFIED_NO_CACHEABLE
/* @violates @ref Mcl_Dma_h_REF_1 MISRA 2012 Required Directive 4.10, Precautions shall be taken in order to prevent the contents of a header file being included more than once. */
#include "Mcl_MemMap.h"


__attribute__(( aligned(32) )) Dma_Ip_SwTcdRegType LogicChannel1Stcd0PB;

__attribute__(( aligned(32) )) Dma_Ip_SwTcdRegType LogicChannel1Stcd1PB;

__attribute__(( aligned(32) )) Dma_Ip_SwTcdRegType LogicChannel1Stcd2PB;

#define MCL_STOP_SEC_VAR_CLEARED_UNSPECIFIED_NO_CACHEABLE
/* @violates @ref Mcl_Dma_h_REF_1 MISRA 2012 Required Directive 4.10, Precautions shall be taken in order to prevent the contents of a header file being included more than once. */
#include "Mcl_MemMap.h"

#define MCL_START_SEC_VAR_INIT_UNSPECIFIED
/* @violates @ref Mcl_Dma_h_REF_1 MISRA 2012 Required Directive 4.10, Precautions shall be taken in order to prevent the contents of a header file being included more than once. */
#include "Mcl_MemMap.h"

extern Dma_Ip_ScatterGatherConfigType LogicChannel1SgaElem0ConfigPB;

extern Dma_Ip_ScatterGatherConfigType LogicChannel1SgaElem1ConfigPB;

extern Dma_Ip_ScatterGatherConfigType LogicChannel1SgaElem2ConfigPB;

Dma_Ip_TransferConfigType LogicChannel1TransferElem0PB =
{
	{
		/* uint32 ScatterGatherAddr; */    0U,
		/* uint32 DestinationStoreAddr; */ 0U,
		/* uint8 BandwidthControl; */      DMA_IP_BWC_ENGINE_NO_STALL,
		/* uint8 TransferModeControl; */   0U,
		/* boolean EnStart; */             (boolean)FALSE,
		/* boolean EnMajorInt; */          (boolean)FALSE,
		/* boolean EnHalfMajorInt; */      (boolean)FALSE,
		/* boolean DisAutoHwRequest; */    (boolean)FALSE,
		/* boolean EnEndOfPacketSignal; */ (boolean)FALSE,
	}, /* Control */
	{
		/* uint32 Addr; */         0U,
		/* sint32 LastAddrAdj; */  0,
		/* sint16 SignedOffset; */ 4,
		/* uint8 TransferSize; */  DMA_IP_TRANSFER_SIZE_4_BYTE,
		/* uint8 Modulo; */        0U,
	}, /* Source */
	{
		/* uint32 Addr; */         0U,
		/* sint32 LastAddrAdj; */  0,
		/* sint16 SignedOffset; */ 4,
		/* uint8 TransferSize; */  DMA_IP_TRANSFER_SIZE_4_BYTE,
		/* uint8 Modulo; */        0U,
	}, /* Destination */
	{
		/* uint32 Size; */        16U,
		/* sint32 Offset; */      0,
		/* uint8 LogicLinkCh; */  0U,
		/* boolean EnLink; */     (boolean)FALSE,
		/* boolean EnSrcOffset; */(boolean)FALSE,
		/* boolean EnDstOffset; */(boolean)FALSE,
	}, /* MinorLoop */
	{
		/* uint16 Count; */       1U,
		/* uint8 LogicLinkCh; */  0U,
		/* boolean EnLink; */     (boolean)FALSE,
	}, /* MajorLoop */
};

Dma_Ip_ScatterGatherConfigType LogicChannel1SgaElem0ConfigPB =
{
	/* Dma_Ip_TransferConfigType * TransferConfig; */         &LogicChannel1TransferElem0PB,
	/* Dma_Ip_SwTcdRegType * Stcd; */                         &LogicChannel1Stcd0PB,
	/* boolean Loaded; */                                     (boolean)FALSE,
	/* struct Dma_Ip_ScatterGatherConfigType * NextConfig; */ &LogicChannel1SgaElem1ConfigPB,
};

Dma_Ip_TransferConfigType LogicChannel1TransferElem1PB =
{
	{
		/* uint32 ScatterGatherAddr; */    0U,
		/* uint32 DestinationStoreAddr; */ 0U,
		/* uint8 BandwidthControl; */      DMA_IP_BWC_ENGINE_NO_STALL,
		/* uint8 TransferModeControl; */   0U,
		/* boolean EnStart; */             (boolean)TRUE,
		/* boolean EnMajorInt; */          (boolean)FALSE,
		/* boolean EnHalfMajorInt; */      (boolean)FALSE,
		/* boolean DisAutoHwRequest; */    (boolean)FALSE,
		/* boolean EnEndOfPacketSignal; */ (boolean)FALSE,
	}, /* Control */
	{
		/* uint32 Addr; */         0U,
		/* sint32 LastAddrAdj; */  0,
		/* sint16 SignedOffset; */ 8,
		/* uint8 TransferSize; */  DMA_IP_TRANSFER_SIZE_8_BYTE,
		/* uint8 Modulo; */        0U,
	}, /* Source */
	{
		/* uint32 Addr; */         0U,
		/* sint32 LastAddrAdj; */  0,
		/* sint16 SignedOffset; */ 8,
		/* uint8 TransferSize; */  DMA_IP_TRANSFER_SIZE_8_BYTE,
		/* uint8 Modulo; */        0U,
	}, /* Destination */
	{
		/* uint32 Size; */        32U,
		/* sint32 Offset; */      0,
		/* uint8 LogicLinkCh; */  0U,
		/* boolean EnLink; */     (boolean)FALSE,
		/* boolean EnSrcOffset; */(boolean)FALSE,
		/* boolean EnDstOffset; */(boolean)FALSE,
	}, /* MinorLoop */
	{
		/* uint16 Count; */       1U,
		/* uint8 LogicLinkCh; */  0U,
		/* boolean EnLink; */     (boolean)FALSE,
	}, /* MajorLoop */
};

Dma_Ip_ScatterGatherConfigType LogicChannel1SgaElem1ConfigPB =
{
	/* Dma_Ip_TransferConfigType * TransferConfig; */         &LogicChannel1TransferElem1PB,
	/* Dma_Ip_SwTcdRegType * Stcd; */                         &LogicChannel1Stcd1PB,
	/* boolean Loaded; */                                     (boolean)FALSE,
	/* struct Dma_Ip_ScatterGatherConfigType * NextConfig; */ &LogicChannel1SgaElem2ConfigPB,
};

Dma_Ip_TransferConfigType LogicChannel1TransferElem2PB =
{
	{
		/* uint32 ScatterGatherAddr; */    0U,
		/* uint32 DestinationStoreAddr; */ 0U,
		/* uint8 BandwidthControl; */      DMA_IP_BWC_ENGINE_NO_STALL,
		/* uint8 TransferModeControl; */   0U,
		/* boolean EnStart; */             (boolean)TRUE,
		/* boolean EnMajorInt; */          (boolean)TRUE,
		/* boolean EnHalfMajorInt; */      (boolean)FALSE,
		/* boolean DisAutoHwRequest; */    (boolean)FALSE,
		/* boolean EnEndOfPacketSignal; */ (boolean)FALSE,
	}, /* Control */
	{
		/* uint32 Addr; */         0U,
		/* sint32 LastAddrAdj; */  0,
		/* sint16 SignedOffset; */ 16,
		/* uint8 TransferSize; */  DMA_IP_TRANSFER_SIZE_16_BYTE,
		/* uint8 Modulo; */        0U,
	}, /* Source */
	{
		/* uint32 Addr; */         0U,
		/* sint32 LastAddrAdj; */  0,
		/* sint16 SignedOffset; */ 16,
		/* uint8 TransferSize; */  DMA_IP_TRANSFER_SIZE_16_BYTE,
		/* uint8 Modulo; */        0U,
	}, /* Destination */
	{
		/* uint32 Size; */        64U,
		/* sint32 Offset; */      0,
		/* uint8 LogicLinkCh; */  0U,
		/* boolean EnLink; */     (boolean)FALSE,
		/* boolean EnSrcOffset; */(boolean)FALSE,
		/* boolean EnDstOffset; */(boolean)FALSE,
	}, /* MinorLoop */
	{
		/* uint16 Count; */       1U,
		/* uint8 LogicLinkCh; */  0U,
		/* boolean EnLink; */     (boolean)FALSE,
	}, /* MajorLoop */
};

Dma_Ip_ScatterGatherConfigType LogicChannel1SgaElem2ConfigPB =
{
	/* Dma_Ip_TransferConfigType * TransferConfig; */         &LogicChannel1TransferElem2PB,
	/* Dma_Ip_SwTcdRegType * Stcd; */                         &LogicChannel1Stcd2PB,
	/* boolean Loaded; */                                     (boolean)FALSE,
	/* struct Dma_Ip_ScatterGatherConfigType * NextConfig; */ NULL_PTR,
};

Dma_Ip_ScatterGatherConfigType * LogicChannel1SgaArrayPB[DMA_LOGIC_CH_1_NOF_CFG_SGA_ELEMENTS] =
{
	&LogicChannel1SgaElem0ConfigPB,
	&LogicChannel1SgaElem1ConfigPB,
	&LogicChannel1SgaElem2ConfigPB,
};

#define MCL_STOP_SEC_VAR_INIT_UNSPECIFIED
/* @violates @ref Mcl_Dma_h_REF_1 MISRA 2012 Required Directive 4.10, Precautions shall be taken in order to prevent the contents of a header file being included more than once. */
#include "Mcl_MemMap.h"

#define MCL_START_SEC_CONFIG_DATA_UNSPECIFIED
/* @violates @ref Mcl_Dma_h_REF_1 MISRA 2012 Required Directive 4.10, Precautions shall be taken in order to prevent the contents of a header file being included more than once. */
#include "Mcl_MemMap.h"

extern void DmaCh0_Callback(void);

extern void DmaCh0_ErrorCallback(void);

const Dma_Ip_LogicChannelConfigType LogicChannel0ConfigPB = 
{
		{
				/* uint32 LogicChId; */               DMA_LOGIC_CH_0,
				/* uint8 HwVersId; */                 DMA_IP_HARDWARE_VERSION_3,
				/* uint8 HwInst; */                   DMA_IP_HW_INST_0,
				/* uint8 HwChId; */                   0U,
				/* Dma_Ip_Callback IntCallback; */    DmaCh0_Callback,
				/* Dma_Ip_Callback ErrIntCallback; */ DmaCh0_ErrorCallback,
		}, /* Dma_Ip_LogicChannelIdType */
		&LogicChannel0GlobalConfigPB,
		&LogicChannel0TransferConfigPB,
		NULL_PTR,
};

extern void DmaCh1_Callback(void);

extern void DmaCh1_ErrorCallback(void);

const Dma_Ip_LogicChannelConfigType LogicChannel1ConfigPB = 
{
		{
				/* uint32 LogicChId; */               DMA_LOGIC_CH_1,
				/* uint8 HwVersId; */                 DMA_IP_HARDWARE_VERSION_3,
				/* uint8 HwInst; */                   DMA_IP_HW_INST_0,
				/* uint8 HwChId; */                   1U,
				/* Dma_Ip_Callback IntCallback; */    DmaCh1_Callback,
				/* Dma_Ip_Callback ErrIntCallback; */ DmaCh1_ErrorCallback,
		}, /* Dma_Ip_LogicChannelIdType */
		&LogicChannel1GlobalConfigPB,
		NULL_PTR,
		(Dma_Ip_ScatterGatherConfigType**)&LogicChannel1SgaArrayPB[0U],
};

extern void DmaCh2_Callback(void);

extern void DmaCh2_ErrorCallback(void);

const Dma_Ip_LogicChannelConfigType LogicChannel2ConfigPB = 
{
		{
				/* uint32 LogicChId; */               DMA_LOGIC_CH_2,
				/* uint8 HwVersId; */                 DMA_IP_HARDWARE_VERSION_3,
				/* uint8 HwInst; */                   DMA_IP_HW_INST_0,
				/* uint8 HwChId; */                   2U,
				/* Dma_Ip_Callback IntCallback; */    DmaCh2_Callback,
				/* Dma_Ip_Callback ErrIntCallback; */ DmaCh2_ErrorCallback,
		}, /* Dma_Ip_LogicChannelIdType */
		NULL_PTR,
		NULL_PTR,
		NULL_PTR,
};

const Dma_Ip_LogicChannelConfigType * const Dma_Ip_paxLogicChannelConfigArrayPB[DMA_IP_NOF_CFG_LOGIC_CHANNELS] =
{
		&LogicChannel0ConfigPB,
		&LogicChannel1ConfigPB,
		&LogicChannel2ConfigPB,
};

#define MCL_STOP_SEC_CONFIG_DATA_UNSPECIFIED
/* @violates @ref Mcl_Dma_h_REF_1 MISRA 2012 Required Directive 4.10, Precautions shall be taken in order to prevent the contents of a header file being included more than once. */
#include "Mcl_MemMap.h"

#define MCL_START_SEC_VAR_CLEARED_UNSPECIFIED_NO_CACHEABLE
/* @violates @ref Mcl_Dma_h_REF_1 MISRA 2012 Required Directive 4.10, Precautions shall be taken in order to prevent the contents of a header file being included more than once. */
#include "Mcl_MemMap.h"

/* DMA Hardware Channel 0 */
Dma_Ip_HwChannelStateType HwChannel0StatePB;

/* DMA Hardware Channel 1 */
Dma_Ip_HwChannelStateType HwChannel1StatePB;

/* DMA Hardware Channel 2 */
Dma_Ip_HwChannelStateType HwChannel2StatePB;

#define MCL_STOP_SEC_VAR_CLEARED_UNSPECIFIED_NO_CACHEABLE
/* @violates @ref Mcl_Dma_h_REF_1 MISRA 2012 Required Directive 4.10, Precautions shall be taken in order to prevent the contents of a header file being included more than once. */
#include "Mcl_MemMap.h"

#define MCL_START_SEC_VAR_INIT_UNSPECIFIED_NO_CACHEABLE
/* @violates @ref Mcl_Dma_h_REF_1 MISRA 2012 Required Directive 4.10, Precautions shall be taken in order to prevent the contents of a header file being included more than once. */
#include "Mcl_MemMap.h"

/* DMA Hardware Channel State and DMA Logic Channel Configuration Array */
Dma_Ip_HwChannelStateType * Dma_Ip_paxHwChannelStateArrayPB[DMA_IP_NOF_CFG_LOGIC_CHANNELS] =
{
		&HwChannel0StatePB,
		&HwChannel1StatePB,
		&HwChannel2StatePB,
};

#define MCL_STOP_SEC_VAR_INIT_UNSPECIFIED_NO_CACHEABLE
/* @violates @ref Mcl_Dma_h_REF_1 MISRA 2012 Required Directive 4.10, Precautions shall be taken in order to prevent the contents of a header file being included more than once. */
#include "Mcl_MemMap.h"

#define MCL_START_SEC_CONFIG_DATA_UNSPECIFIED
/* @violates @ref Mcl_Dma_h_REF_1 MISRA 2012 Required Directive 4.10, Precautions shall be taken in order to prevent the contents of a header file being included more than once. */
#include "Mcl_MemMap.h"

/* DMA Initialization Structure */
const Dma_Ip_InitType Dma_Ip_xDmaInitPB = 
{
		/* Dma_Ip_HwChannelStateType ** HwChStateArray; */                            &Dma_Ip_paxHwChannelStateArrayPB[0U],         /* Static */
		/* const Dma_Ip_LogicChannelConfigType * const LogicChConfigArray;   */       &Dma_Ip_xLogicChannelResetConfig,           /* Static */
		/* const Dma_Ip_LogicChannelConfigType * const * LogicChConfigArray; */       &Dma_Ip_paxLogicChannelConfigArrayPB[0U],     /* Static */
		/* const Dma_Ip_LogicInstanceConfigType * const LogicInstConfigArray;   */    &Dma_Ip_xLogicInstanceResetConfig,          /* Static */
		/* const Dma_Ip_LogicInstanceConfigType * const  * LogicInstConfigArray; */   &Dma_Ip_paxLogicInstanceConfigArrayPB[0U],    /* Static */
};

#define MCL_STOP_SEC_CONFIG_DATA_UNSPECIFIED
/* @violates @ref Mcl_Dma_h_REF_1 MISRA 2012 Required Directive 4.10, Precautions shall be taken in order to prevent the contents of a header file being included more than once. */
#include "Mcl_MemMap.h"


#ifdef __cplusplus
}
#endif

/*==================================================================================================
 *                                        END OF FILE
==================================================================================================*/


