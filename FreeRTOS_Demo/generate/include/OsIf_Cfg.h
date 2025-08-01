/*==================================================================================================
* Project : RTD AUTOSAR 4.7
* Platform : CORTEXM
* Peripheral : S32K3XX
* Dependencies : none
*
* Autosar Version : 4.7.0
* Autosar Revision : ASR_REL_4_7_REV_0000
* Autosar Conf.Variant :
* SW Version : 3.0.0
* Build Version : S32K3_RTD_3_0_0_D2303_ASR_REL_4_7_REV_0000_20230331
*
* Copyright 2020 - 2023 NXP Semiconductors
*
* NXP Confidential. This software is owned or controlled by NXP and may only be
* used strictly in accordance with the applicable license terms. By expressly
* accepting such terms or by downloading, installing, activating and/or otherwise
* using the software, you are agreeing that you have read, and that you agree to
* comply with and are bound by, such license terms. If you do not agree to be
* bound by the applicable license terms, then you may not retain, install,
* activate or otherwise use the software.
==================================================================================================*/
/**
*   @file       OsIf_Cfg.h
*   @version 3.0.0
*
*
*   @addtogroup OSIF_DRIVER
*   @{
*/

#ifndef OSIF_CFG_H
#define OSIF_CFG_H

#ifdef __cplusplus
extern "C"{
#endif

/*==================================================================================================
                                         INCLUDE FILES
 1) system and project includes
 2) needed interfaces from external units
 3) internal and external interfaces from this unit
==================================================================================================*/
#include "OsIf_ArchCfg.h"
#include "StandardTypes.h"

#include "S32K358_SYSTICK.h"
/*==================================================================================================
*                              SOURCE FILE VERSION INFORMATION
==================================================================================================*/
#define OSIF_CFG_VENDOR_ID                    43
#define OSIF_CFG_AR_RELEASE_MAJOR_VERSION     4
#define OSIF_CFG_AR_RELEASE_MINOR_VERSION     7
#define OSIF_CFG_AR_RELEASE_REVISION_VERSION  0
#define OSIF_CFG_SW_MAJOR_VERSION             3
#define OSIF_CFG_SW_MINOR_VERSION             0
#define OSIF_CFG_SW_PATCH_VERSION             0

/*==================================================================================================
*                                     FILE VERSION CHECKS
==================================================================================================*/
/* Checks against OsIf_ArchCfg.h */
#if (OSIF_CFG_VENDOR_ID != OSIF_ARCHCFG_VENDOR_ID)
    #error "OsIf_Cfg.h and OsIf_ArchCfg.h have different vendor ids"
#endif
#if ((OSIF_CFG_AR_RELEASE_MAJOR_VERSION    != OSIF_ARCHCFG_AR_RELEASE_MAJOR_VERSION) || \
     (OSIF_CFG_AR_RELEASE_MINOR_VERSION    != OSIF_ARCHCFG_AR_RELEASE_MINOR_VERSION) || \
     (OSIF_CFG_AR_RELEASE_REVISION_VERSION != OSIF_ARCHCFG_AR_RELEASE_REVISION_VERSION))
     #error "AUTOSAR Version Numbers of OsIf_Cfg.h and OsIf_ArchCfg.h are different"
#endif
#if ((OSIF_CFG_SW_MAJOR_VERSION != OSIF_ARCHCFG_SW_MAJOR_VERSION) || \
     (OSIF_CFG_SW_MINOR_VERSION != OSIF_ARCHCFG_SW_MINOR_VERSION) || \
     (OSIF_CFG_SW_PATCH_VERSION != OSIF_ARCHCFG_SW_PATCH_VERSION))
    #error "Software Version Numbers of OsIf_Cfg.h and OsIf_ArchCfg.h are different"
#endif

/* Checks against StandardTypes.h */
#ifndef DISABLE_MCAL_INTERMODULE_ASR_CHECK
    #if ((OSIF_CFG_AR_RELEASE_MAJOR_VERSION != STD_AR_RELEASE_MAJOR_VERSION) || \
         (OSIF_CFG_AR_RELEASE_MINOR_VERSION != STD_AR_RELEASE_MINOR_VERSION))
        #error "AutoSar Version Numbers of OsIf_Cfg.h and StandardTypes.h are different"
    #endif
#endif
/*==================================================================================================
*                                            CONSTANTS
==================================================================================================*/

/*==================================================================================================
*                                      DEFINES AND MACROS
==================================================================================================*/
/* General OSIF configuration */
#define OSIF_MODULE_ID                   (255U)

#define OSIF_DRIVER_INSTANCE             (0U)

#define OSIF_ENABLE_USER_MODE_SUPPORT     (STD_OFF)

#ifndef MCAL_ENABLE_USER_MODE_SUPPORT
    #if (STD_ON == OSIF_ENABLE_USER_MODE_SUPPORT)
        #error MCAL_ENABLE_USER_MODE_SUPPORT is not enabled. For running OsIf in user mode, MCAL_ENABLE_USER_MODE_SUPPORT needs to be defined.
    #endif /* (STD_ON == OSIF_ENABLE_USER_MODE_SUPPORT */
#endif /* ifndef MCAL_ENABLE_USER_MODE_SUPPORT */

#define OSIF_ENABLE_MULTICORE_SUPPORT    (STD_OFF)

#define OSIF_MAX_COREIDX_SUPPORTED       (1U)

#define OSIF_DEV_ERROR_DETECT            (STD_ON)

#define USING_OS_BAREMETAL

#define OSIF_USE_SYSTEM_TIMER            (STD_OFF)

#define OSIF_USE_CUSTOM_TIMER            (STD_OFF)

/* Autosar OS Specific */

/* Baremetal Specific */
#if (MCAL_PLATFORM_ARM == MCAL_ARM_MARCH)
    #define OSIF_USE_SYSTICK                 (STD_ON)
#else
    #define OSIF_USE_GENERICTIMER            (STD_ON)
#endif

/*==================================================================================================
*                                             ENUMS
==================================================================================================*/

/*==================================================================================================
*                                STRUCTURES AND OTHER TYPEDEFS
==================================================================================================*/

/*==================================================================================================
*                                GLOBAL VARIABLE DECLARATIONS
==================================================================================================*/

/*==================================================================================================
                                       GLOBAL CONSTANTS
==================================================================================================*/

/*==================================================================================================
*                                    FUNCTION PROTOTYPES
==================================================================================================*/


#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* OSIF_CFG_H */
/** @} */

