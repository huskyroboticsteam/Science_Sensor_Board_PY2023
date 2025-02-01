/***************************************************************************//**
* \file .h
* \version 4.0
*
* \brief
*  This private file provides constants and parameter values for the
*  SCB Component.
*  Please do not use this file or its content in your project.
*
* Note:
*
********************************************************************************
* \copyright
* Copyright 2013-2017, Cypress Semiconductor Corporation. All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_SCB_PVT_DBG_UART_H)
#define CY_SCB_PVT_DBG_UART_H

#include "DBG_UART.h"


/***************************************
*     Private Function Prototypes
***************************************/

/* APIs to service INTR_I2C_EC register */
#define DBG_UART_SetI2CExtClkInterruptMode(interruptMask) DBG_UART_WRITE_INTR_I2C_EC_MASK(interruptMask)
#define DBG_UART_ClearI2CExtClkInterruptSource(interruptMask) DBG_UART_CLEAR_INTR_I2C_EC(interruptMask)
#define DBG_UART_GetI2CExtClkInterruptSource()                (DBG_UART_INTR_I2C_EC_REG)
#define DBG_UART_GetI2CExtClkInterruptMode()                  (DBG_UART_INTR_I2C_EC_MASK_REG)
#define DBG_UART_GetI2CExtClkInterruptSourceMasked()          (DBG_UART_INTR_I2C_EC_MASKED_REG)

#if (!DBG_UART_CY_SCBIP_V1)
    /* APIs to service INTR_SPI_EC register */
    #define DBG_UART_SetSpiExtClkInterruptMode(interruptMask) \
                                                                DBG_UART_WRITE_INTR_SPI_EC_MASK(interruptMask)
    #define DBG_UART_ClearSpiExtClkInterruptSource(interruptMask) \
                                                                DBG_UART_CLEAR_INTR_SPI_EC(interruptMask)
    #define DBG_UART_GetExtSpiClkInterruptSource()                 (DBG_UART_INTR_SPI_EC_REG)
    #define DBG_UART_GetExtSpiClkInterruptMode()                   (DBG_UART_INTR_SPI_EC_MASK_REG)
    #define DBG_UART_GetExtSpiClkInterruptSourceMasked()           (DBG_UART_INTR_SPI_EC_MASKED_REG)
#endif /* (!DBG_UART_CY_SCBIP_V1) */

#if(DBG_UART_SCB_MODE_UNCONFIG_CONST_CFG)
    extern void DBG_UART_SetPins(uint32 mode, uint32 subMode, uint32 uartEnableMask);
#endif /* (DBG_UART_SCB_MODE_UNCONFIG_CONST_CFG) */


/***************************************
*     Vars with External Linkage
***************************************/

#if (DBG_UART_SCB_IRQ_INTERNAL)
#if !defined (CY_REMOVE_DBG_UART_CUSTOM_INTR_HANDLER)
    extern cyisraddress DBG_UART_customIntrHandler;
#endif /* !defined (CY_REMOVE_DBG_UART_CUSTOM_INTR_HANDLER) */
#endif /* (DBG_UART_SCB_IRQ_INTERNAL) */

extern DBG_UART_BACKUP_STRUCT DBG_UART_backup;

#if(DBG_UART_SCB_MODE_UNCONFIG_CONST_CFG)
    /* Common configuration variables */
    extern uint8 DBG_UART_scbMode;
    extern uint8 DBG_UART_scbEnableWake;
    extern uint8 DBG_UART_scbEnableIntr;

    /* I2C configuration variables */
    extern uint8 DBG_UART_mode;
    extern uint8 DBG_UART_acceptAddr;

    /* SPI/UART configuration variables */
    extern volatile uint8 * DBG_UART_rxBuffer;
    extern uint8   DBG_UART_rxDataBits;
    extern uint32  DBG_UART_rxBufferSize;

    extern volatile uint8 * DBG_UART_txBuffer;
    extern uint8   DBG_UART_txDataBits;
    extern uint32  DBG_UART_txBufferSize;

    /* EZI2C configuration variables */
    extern uint8 DBG_UART_numberOfAddr;
    extern uint8 DBG_UART_subAddrSize;
#endif /* (DBG_UART_SCB_MODE_UNCONFIG_CONST_CFG) */

#if (! (DBG_UART_SCB_MODE_I2C_CONST_CFG || \
        DBG_UART_SCB_MODE_EZI2C_CONST_CFG))
    extern uint16 DBG_UART_IntrTxMask;
#endif /* (! (DBG_UART_SCB_MODE_I2C_CONST_CFG || \
              DBG_UART_SCB_MODE_EZI2C_CONST_CFG)) */


/***************************************
*        Conditional Macro
****************************************/

#if(DBG_UART_SCB_MODE_UNCONFIG_CONST_CFG)
    /* Defines run time operation mode */
    #define DBG_UART_SCB_MODE_I2C_RUNTM_CFG     (DBG_UART_SCB_MODE_I2C      == DBG_UART_scbMode)
    #define DBG_UART_SCB_MODE_SPI_RUNTM_CFG     (DBG_UART_SCB_MODE_SPI      == DBG_UART_scbMode)
    #define DBG_UART_SCB_MODE_UART_RUNTM_CFG    (DBG_UART_SCB_MODE_UART     == DBG_UART_scbMode)
    #define DBG_UART_SCB_MODE_EZI2C_RUNTM_CFG   (DBG_UART_SCB_MODE_EZI2C    == DBG_UART_scbMode)
    #define DBG_UART_SCB_MODE_UNCONFIG_RUNTM_CFG \
                                                        (DBG_UART_SCB_MODE_UNCONFIG == DBG_UART_scbMode)

    /* Defines wakeup enable */
    #define DBG_UART_SCB_WAKE_ENABLE_CHECK       (0u != DBG_UART_scbEnableWake)
#endif /* (DBG_UART_SCB_MODE_UNCONFIG_CONST_CFG) */

/* Defines maximum number of SCB pins */
#if (!DBG_UART_CY_SCBIP_V1)
    #define DBG_UART_SCB_PINS_NUMBER    (7u)
#else
    #define DBG_UART_SCB_PINS_NUMBER    (2u)
#endif /* (!DBG_UART_CY_SCBIP_V1) */

#endif /* (CY_SCB_PVT_DBG_UART_H) */


/* [] END OF FILE */
