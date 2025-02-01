/***************************************************************************//**
* \file DBG_UART_PM.c
* \version 4.0
*
* \brief
*  This file provides the source code to the Power Management support for
*  the SCB Component.
*
* Note:
*
********************************************************************************
* \copyright
* Copyright 2013-2017, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "DBG_UART.h"
#include "DBG_UART_PVT.h"

#if(DBG_UART_SCB_MODE_I2C_INC)
    #include "DBG_UART_I2C_PVT.h"
#endif /* (DBG_UART_SCB_MODE_I2C_INC) */

#if(DBG_UART_SCB_MODE_EZI2C_INC)
    #include "DBG_UART_EZI2C_PVT.h"
#endif /* (DBG_UART_SCB_MODE_EZI2C_INC) */

#if(DBG_UART_SCB_MODE_SPI_INC || DBG_UART_SCB_MODE_UART_INC)
    #include "DBG_UART_SPI_UART_PVT.h"
#endif /* (DBG_UART_SCB_MODE_SPI_INC || DBG_UART_SCB_MODE_UART_INC) */


/***************************************
*   Backup Structure declaration
***************************************/

#if(DBG_UART_SCB_MODE_UNCONFIG_CONST_CFG || \
   (DBG_UART_SCB_MODE_I2C_CONST_CFG   && (!DBG_UART_I2C_WAKE_ENABLE_CONST))   || \
   (DBG_UART_SCB_MODE_EZI2C_CONST_CFG && (!DBG_UART_EZI2C_WAKE_ENABLE_CONST)) || \
   (DBG_UART_SCB_MODE_SPI_CONST_CFG   && (!DBG_UART_SPI_WAKE_ENABLE_CONST))   || \
   (DBG_UART_SCB_MODE_UART_CONST_CFG  && (!DBG_UART_UART_WAKE_ENABLE_CONST)))

    DBG_UART_BACKUP_STRUCT DBG_UART_backup =
    {
        0u, /* enableState */
    };
#endif


/*******************************************************************************
* Function Name: DBG_UART_Sleep
****************************************************************************//**
*
*  Prepares the DBG_UART component to enter Deep Sleep.
*  The “Enable wakeup from Deep Sleep Mode” selection has an influence on this 
*  function implementation:
*  - Checked: configures the component to be wakeup source from Deep Sleep.
*  - Unchecked: stores the current component state (enabled or disabled) and 
*    disables the component. See SCB_Stop() function for details about component 
*    disabling.
*
*  Call the DBG_UART_Sleep() function before calling the 
*  CyPmSysDeepSleep() function. 
*  Refer to the PSoC Creator System Reference Guide for more information about 
*  power management functions and Low power section of this document for the 
*  selected mode.
*
*  This function should not be called before entering Sleep.
*
*******************************************************************************/
void DBG_UART_Sleep(void)
{
#if(DBG_UART_SCB_MODE_UNCONFIG_CONST_CFG)

    if(DBG_UART_SCB_WAKE_ENABLE_CHECK)
    {
        if(DBG_UART_SCB_MODE_I2C_RUNTM_CFG)
        {
            DBG_UART_I2CSaveConfig();
        }
        else if(DBG_UART_SCB_MODE_EZI2C_RUNTM_CFG)
        {
            DBG_UART_EzI2CSaveConfig();
        }
    #if(!DBG_UART_CY_SCBIP_V1)
        else if(DBG_UART_SCB_MODE_SPI_RUNTM_CFG)
        {
            DBG_UART_SpiSaveConfig();
        }
        else if(DBG_UART_SCB_MODE_UART_RUNTM_CFG)
        {
            DBG_UART_UartSaveConfig();
        }
    #endif /* (!DBG_UART_CY_SCBIP_V1) */
        else
        {
            /* Unknown mode */
        }
    }
    else
    {
        DBG_UART_backup.enableState = (uint8) DBG_UART_GET_CTRL_ENABLED;

        if(0u != DBG_UART_backup.enableState)
        {
            DBG_UART_Stop();
        }
    }

#else

    #if (DBG_UART_SCB_MODE_I2C_CONST_CFG && DBG_UART_I2C_WAKE_ENABLE_CONST)
        DBG_UART_I2CSaveConfig();

    #elif (DBG_UART_SCB_MODE_EZI2C_CONST_CFG && DBG_UART_EZI2C_WAKE_ENABLE_CONST)
        DBG_UART_EzI2CSaveConfig();

    #elif (DBG_UART_SCB_MODE_SPI_CONST_CFG && DBG_UART_SPI_WAKE_ENABLE_CONST)
        DBG_UART_SpiSaveConfig();

    #elif (DBG_UART_SCB_MODE_UART_CONST_CFG && DBG_UART_UART_WAKE_ENABLE_CONST)
        DBG_UART_UartSaveConfig();

    #else

        DBG_UART_backup.enableState = (uint8) DBG_UART_GET_CTRL_ENABLED;

        if(0u != DBG_UART_backup.enableState)
        {
            DBG_UART_Stop();
        }

    #endif /* defined (DBG_UART_SCB_MODE_I2C_CONST_CFG) && (DBG_UART_I2C_WAKE_ENABLE_CONST) */

#endif /* (DBG_UART_SCB_MODE_UNCONFIG_CONST_CFG) */
}


/*******************************************************************************
* Function Name: DBG_UART_Wakeup
****************************************************************************//**
*
*  Prepares the DBG_UART component for Active mode operation after 
*  Deep Sleep.
*  The “Enable wakeup from Deep Sleep Mode” selection has influence on this 
*  function implementation:
*  - Checked: restores the component Active mode configuration.
*  - Unchecked: enables the component if it was enabled before enter Deep Sleep.
*
*  This function should not be called after exiting Sleep.
*
*  \sideeffect
*   Calling the DBG_UART_Wakeup() function without first calling the 
*   DBG_UART_Sleep() function may produce unexpected behavior.
*
*******************************************************************************/
void DBG_UART_Wakeup(void)
{
#if(DBG_UART_SCB_MODE_UNCONFIG_CONST_CFG)

    if(DBG_UART_SCB_WAKE_ENABLE_CHECK)
    {
        if(DBG_UART_SCB_MODE_I2C_RUNTM_CFG)
        {
            DBG_UART_I2CRestoreConfig();
        }
        else if(DBG_UART_SCB_MODE_EZI2C_RUNTM_CFG)
        {
            DBG_UART_EzI2CRestoreConfig();
        }
    #if(!DBG_UART_CY_SCBIP_V1)
        else if(DBG_UART_SCB_MODE_SPI_RUNTM_CFG)
        {
            DBG_UART_SpiRestoreConfig();
        }
        else if(DBG_UART_SCB_MODE_UART_RUNTM_CFG)
        {
            DBG_UART_UartRestoreConfig();
        }
    #endif /* (!DBG_UART_CY_SCBIP_V1) */
        else
        {
            /* Unknown mode */
        }
    }
    else
    {
        if(0u != DBG_UART_backup.enableState)
        {
            DBG_UART_Enable();
        }
    }

#else

    #if (DBG_UART_SCB_MODE_I2C_CONST_CFG  && DBG_UART_I2C_WAKE_ENABLE_CONST)
        DBG_UART_I2CRestoreConfig();

    #elif (DBG_UART_SCB_MODE_EZI2C_CONST_CFG && DBG_UART_EZI2C_WAKE_ENABLE_CONST)
        DBG_UART_EzI2CRestoreConfig();

    #elif (DBG_UART_SCB_MODE_SPI_CONST_CFG && DBG_UART_SPI_WAKE_ENABLE_CONST)
        DBG_UART_SpiRestoreConfig();

    #elif (DBG_UART_SCB_MODE_UART_CONST_CFG && DBG_UART_UART_WAKE_ENABLE_CONST)
        DBG_UART_UartRestoreConfig();

    #else

        if(0u != DBG_UART_backup.enableState)
        {
            DBG_UART_Enable();
        }

    #endif /* (DBG_UART_I2C_WAKE_ENABLE_CONST) */

#endif /* (DBG_UART_SCB_MODE_UNCONFIG_CONST_CFG) */
}


/* [] END OF FILE */
