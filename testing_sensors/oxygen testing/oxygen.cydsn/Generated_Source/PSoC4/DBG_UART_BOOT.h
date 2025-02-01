/***************************************************************************//**
* \file DBG_UART_BOOT.h
* \version 4.0
*
* \brief
*  This file provides constants and parameter values of the bootloader
*  communication APIs for the SCB Component.
*
* Note:
*
********************************************************************************
* \copyright
* Copyright 2014-2017, Cypress Semiconductor Corporation. All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_SCB_BOOT_DBG_UART_H)
#define CY_SCB_BOOT_DBG_UART_H

#include "DBG_UART_PVT.h"

#if (DBG_UART_SCB_MODE_I2C_INC)
    #include "DBG_UART_I2C.h"
#endif /* (DBG_UART_SCB_MODE_I2C_INC) */

#if (DBG_UART_SCB_MODE_EZI2C_INC)
    #include "DBG_UART_EZI2C.h"
#endif /* (DBG_UART_SCB_MODE_EZI2C_INC) */

#if (DBG_UART_SCB_MODE_SPI_INC || DBG_UART_SCB_MODE_UART_INC)
    #include "DBG_UART_SPI_UART.h"
#endif /* (DBG_UART_SCB_MODE_SPI_INC || DBG_UART_SCB_MODE_UART_INC) */


/***************************************
*  Conditional Compilation Parameters
****************************************/

/* Bootloader communication interface enable */
#define DBG_UART_BTLDR_COMM_ENABLED ((CYDEV_BOOTLOADER_IO_COMP == CyBtldr_DBG_UART) || \
                                             (CYDEV_BOOTLOADER_IO_COMP == CyBtldr_Custom_Interface))

/* Enable I2C bootloader communication */
#if (DBG_UART_SCB_MODE_I2C_INC)
    #define DBG_UART_I2C_BTLDR_COMM_ENABLED     (DBG_UART_BTLDR_COMM_ENABLED && \
                                                            (DBG_UART_SCB_MODE_UNCONFIG_CONST_CFG || \
                                                             DBG_UART_I2C_SLAVE_CONST))
#else
     #define DBG_UART_I2C_BTLDR_COMM_ENABLED    (0u)
#endif /* (DBG_UART_SCB_MODE_I2C_INC) */

/* EZI2C does not support bootloader communication. Provide empty APIs */
#if (DBG_UART_SCB_MODE_EZI2C_INC)
    #define DBG_UART_EZI2C_BTLDR_COMM_ENABLED   (DBG_UART_BTLDR_COMM_ENABLED && \
                                                         DBG_UART_SCB_MODE_UNCONFIG_CONST_CFG)
#else
    #define DBG_UART_EZI2C_BTLDR_COMM_ENABLED   (0u)
#endif /* (DBG_UART_EZI2C_BTLDR_COMM_ENABLED) */

/* Enable SPI bootloader communication */
#if (DBG_UART_SCB_MODE_SPI_INC)
    #define DBG_UART_SPI_BTLDR_COMM_ENABLED     (DBG_UART_BTLDR_COMM_ENABLED && \
                                                            (DBG_UART_SCB_MODE_UNCONFIG_CONST_CFG || \
                                                             DBG_UART_SPI_SLAVE_CONST))
#else
        #define DBG_UART_SPI_BTLDR_COMM_ENABLED (0u)
#endif /* (DBG_UART_SPI_BTLDR_COMM_ENABLED) */

/* Enable UART bootloader communication */
#if (DBG_UART_SCB_MODE_UART_INC)
       #define DBG_UART_UART_BTLDR_COMM_ENABLED    (DBG_UART_BTLDR_COMM_ENABLED && \
                                                            (DBG_UART_SCB_MODE_UNCONFIG_CONST_CFG || \
                                                             (DBG_UART_UART_RX_DIRECTION && \
                                                              DBG_UART_UART_TX_DIRECTION)))
#else
     #define DBG_UART_UART_BTLDR_COMM_ENABLED   (0u)
#endif /* (DBG_UART_UART_BTLDR_COMM_ENABLED) */

/* Enable bootloader communication */
#define DBG_UART_BTLDR_COMM_MODE_ENABLED    (DBG_UART_I2C_BTLDR_COMM_ENABLED   || \
                                                     DBG_UART_SPI_BTLDR_COMM_ENABLED   || \
                                                     DBG_UART_EZI2C_BTLDR_COMM_ENABLED || \
                                                     DBG_UART_UART_BTLDR_COMM_ENABLED)


/***************************************
*        Function Prototypes
***************************************/

#if defined(CYDEV_BOOTLOADER_IO_COMP) && (DBG_UART_I2C_BTLDR_COMM_ENABLED)
    /* I2C Bootloader physical layer functions */
    void DBG_UART_I2CCyBtldrCommStart(void);
    void DBG_UART_I2CCyBtldrCommStop (void);
    void DBG_UART_I2CCyBtldrCommReset(void);
    cystatus DBG_UART_I2CCyBtldrCommRead       (uint8 pData[], uint16 size, uint16 * count, uint8 timeOut);
    cystatus DBG_UART_I2CCyBtldrCommWrite(const uint8 pData[], uint16 size, uint16 * count, uint8 timeOut);

    /* Map I2C specific bootloader communication APIs to SCB specific APIs */
    #if (DBG_UART_SCB_MODE_I2C_CONST_CFG)
        #define DBG_UART_CyBtldrCommStart   DBG_UART_I2CCyBtldrCommStart
        #define DBG_UART_CyBtldrCommStop    DBG_UART_I2CCyBtldrCommStop
        #define DBG_UART_CyBtldrCommReset   DBG_UART_I2CCyBtldrCommReset
        #define DBG_UART_CyBtldrCommRead    DBG_UART_I2CCyBtldrCommRead
        #define DBG_UART_CyBtldrCommWrite   DBG_UART_I2CCyBtldrCommWrite
    #endif /* (DBG_UART_SCB_MODE_I2C_CONST_CFG) */

#endif /* defined(CYDEV_BOOTLOADER_IO_COMP) && (DBG_UART_I2C_BTLDR_COMM_ENABLED) */


#if defined(CYDEV_BOOTLOADER_IO_COMP) && (DBG_UART_EZI2C_BTLDR_COMM_ENABLED)
    /* Bootloader physical layer functions */
    void DBG_UART_EzI2CCyBtldrCommStart(void);
    void DBG_UART_EzI2CCyBtldrCommStop (void);
    void DBG_UART_EzI2CCyBtldrCommReset(void);
    cystatus DBG_UART_EzI2CCyBtldrCommRead       (uint8 pData[], uint16 size, uint16 * count, uint8 timeOut);
    cystatus DBG_UART_EzI2CCyBtldrCommWrite(const uint8 pData[], uint16 size, uint16 * count, uint8 timeOut);

    /* Map EZI2C specific bootloader communication APIs to SCB specific APIs */
    #if (DBG_UART_SCB_MODE_EZI2C_CONST_CFG)
        #define DBG_UART_CyBtldrCommStart   DBG_UART_EzI2CCyBtldrCommStart
        #define DBG_UART_CyBtldrCommStop    DBG_UART_EzI2CCyBtldrCommStop
        #define DBG_UART_CyBtldrCommReset   DBG_UART_EzI2CCyBtldrCommReset
        #define DBG_UART_CyBtldrCommRead    DBG_UART_EzI2CCyBtldrCommRead
        #define DBG_UART_CyBtldrCommWrite   DBG_UART_EzI2CCyBtldrCommWrite
    #endif /* (DBG_UART_SCB_MODE_EZI2C_CONST_CFG) */

#endif /* defined(CYDEV_BOOTLOADER_IO_COMP) && (DBG_UART_EZI2C_BTLDR_COMM_ENABLED) */

#if defined(CYDEV_BOOTLOADER_IO_COMP) && (DBG_UART_SPI_BTLDR_COMM_ENABLED)
    /* SPI Bootloader physical layer functions */
    void DBG_UART_SpiCyBtldrCommStart(void);
    void DBG_UART_SpiCyBtldrCommStop (void);
    void DBG_UART_SpiCyBtldrCommReset(void);
    cystatus DBG_UART_SpiCyBtldrCommRead       (uint8 pData[], uint16 size, uint16 * count, uint8 timeOut);
    cystatus DBG_UART_SpiCyBtldrCommWrite(const uint8 pData[], uint16 size, uint16 * count, uint8 timeOut);

    /* Map SPI specific bootloader communication APIs to SCB specific APIs */
    #if (DBG_UART_SCB_MODE_SPI_CONST_CFG)
        #define DBG_UART_CyBtldrCommStart   DBG_UART_SpiCyBtldrCommStart
        #define DBG_UART_CyBtldrCommStop    DBG_UART_SpiCyBtldrCommStop
        #define DBG_UART_CyBtldrCommReset   DBG_UART_SpiCyBtldrCommReset
        #define DBG_UART_CyBtldrCommRead    DBG_UART_SpiCyBtldrCommRead
        #define DBG_UART_CyBtldrCommWrite   DBG_UART_SpiCyBtldrCommWrite
    #endif /* (DBG_UART_SCB_MODE_SPI_CONST_CFG) */

#endif /* defined(CYDEV_BOOTLOADER_IO_COMP) && (DBG_UART_SPI_BTLDR_COMM_ENABLED) */

#if defined(CYDEV_BOOTLOADER_IO_COMP) && (DBG_UART_UART_BTLDR_COMM_ENABLED)
    /* UART Bootloader physical layer functions */
    void DBG_UART_UartCyBtldrCommStart(void);
    void DBG_UART_UartCyBtldrCommStop (void);
    void DBG_UART_UartCyBtldrCommReset(void);
    cystatus DBG_UART_UartCyBtldrCommRead       (uint8 pData[], uint16 size, uint16 * count, uint8 timeOut);
    cystatus DBG_UART_UartCyBtldrCommWrite(const uint8 pData[], uint16 size, uint16 * count, uint8 timeOut);

    /* Map UART specific bootloader communication APIs to SCB specific APIs */
    #if (DBG_UART_SCB_MODE_UART_CONST_CFG)
        #define DBG_UART_CyBtldrCommStart   DBG_UART_UartCyBtldrCommStart
        #define DBG_UART_CyBtldrCommStop    DBG_UART_UartCyBtldrCommStop
        #define DBG_UART_CyBtldrCommReset   DBG_UART_UartCyBtldrCommReset
        #define DBG_UART_CyBtldrCommRead    DBG_UART_UartCyBtldrCommRead
        #define DBG_UART_CyBtldrCommWrite   DBG_UART_UartCyBtldrCommWrite
    #endif /* (DBG_UART_SCB_MODE_UART_CONST_CFG) */

#endif /* defined(CYDEV_BOOTLOADER_IO_COMP) && (DBG_UART_UART_BTLDR_COMM_ENABLED) */

/**
* \addtogroup group_bootloader
* @{
*/

#if defined(CYDEV_BOOTLOADER_IO_COMP) && (DBG_UART_BTLDR_COMM_ENABLED)
    #if (DBG_UART_SCB_MODE_UNCONFIG_CONST_CFG)
        /* Bootloader physical layer functions */
        void DBG_UART_CyBtldrCommStart(void);
        void DBG_UART_CyBtldrCommStop (void);
        void DBG_UART_CyBtldrCommReset(void);
        cystatus DBG_UART_CyBtldrCommRead       (uint8 pData[], uint16 size, uint16 * count, uint8 timeOut);
        cystatus DBG_UART_CyBtldrCommWrite(const uint8 pData[], uint16 size, uint16 * count, uint8 timeOut);
    #endif /* (DBG_UART_SCB_MODE_UNCONFIG_CONST_CFG) */

    /* Map SCB specific bootloader communication APIs to common APIs */
    #if (CYDEV_BOOTLOADER_IO_COMP == CyBtldr_DBG_UART)
        #define CyBtldrCommStart    DBG_UART_CyBtldrCommStart
        #define CyBtldrCommStop     DBG_UART_CyBtldrCommStop
        #define CyBtldrCommReset    DBG_UART_CyBtldrCommReset
        #define CyBtldrCommWrite    DBG_UART_CyBtldrCommWrite
        #define CyBtldrCommRead     DBG_UART_CyBtldrCommRead
    #endif /* (CYDEV_BOOTLOADER_IO_COMP == CyBtldr_DBG_UART) */

#endif /* defined(CYDEV_BOOTLOADER_IO_COMP) && (DBG_UART_BTLDR_COMM_ENABLED) */

/** @} group_bootloader */

/***************************************
*           API Constants
***************************************/

/* Timeout unit in milliseconds */
#define DBG_UART_WAIT_1_MS  (1u)

/* Return number of bytes to copy into bootloader buffer */
#define DBG_UART_BYTES_TO_COPY(actBufSize, bufSize) \
                            ( ((uint32)(actBufSize) < (uint32)(bufSize)) ? \
                                ((uint32) (actBufSize)) : ((uint32) (bufSize)) )

/* Size of Read/Write buffers for I2C bootloader  */
#define DBG_UART_I2C_BTLDR_SIZEOF_READ_BUFFER   (64u)
#define DBG_UART_I2C_BTLDR_SIZEOF_WRITE_BUFFER  (64u)

/* Byte to byte time interval: calculated basing on current component
* data rate configuration, can be defined in project if required.
*/
#ifndef DBG_UART_SPI_BYTE_TO_BYTE
    #define DBG_UART_SPI_BYTE_TO_BYTE   (160u)
#endif

/* Byte to byte time interval: calculated basing on current component
* baud rate configuration, can be defined in the project if required.
*/
#ifndef DBG_UART_UART_BYTE_TO_BYTE
    #define DBG_UART_UART_BYTE_TO_BYTE  (175u)
#endif /* DBG_UART_UART_BYTE_TO_BYTE */

#endif /* (CY_SCB_BOOT_DBG_UART_H) */


/* [] END OF FILE */
