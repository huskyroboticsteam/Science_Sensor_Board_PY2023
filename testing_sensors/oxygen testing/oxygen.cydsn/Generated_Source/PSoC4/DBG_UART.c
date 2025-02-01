/***************************************************************************//**
* \file DBG_UART.c
* \version 4.0
*
* \brief
*  This file provides the source code to the API for the SCB Component.
*
* Note:
*
*******************************************************************************
* \copyright
* Copyright 2013-2017, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "DBG_UART_PVT.h"

#if (DBG_UART_SCB_MODE_I2C_INC)
    #include "DBG_UART_I2C_PVT.h"
#endif /* (DBG_UART_SCB_MODE_I2C_INC) */

#if (DBG_UART_SCB_MODE_EZI2C_INC)
    #include "DBG_UART_EZI2C_PVT.h"
#endif /* (DBG_UART_SCB_MODE_EZI2C_INC) */

#if (DBG_UART_SCB_MODE_SPI_INC || DBG_UART_SCB_MODE_UART_INC)
    #include "DBG_UART_SPI_UART_PVT.h"
#endif /* (DBG_UART_SCB_MODE_SPI_INC || DBG_UART_SCB_MODE_UART_INC) */


/***************************************
*    Run Time Configuration Vars
***************************************/

/* Stores internal component configuration for Unconfigured mode */
#if (DBG_UART_SCB_MODE_UNCONFIG_CONST_CFG)
    /* Common configuration variables */
    uint8 DBG_UART_scbMode = DBG_UART_SCB_MODE_UNCONFIG;
    uint8 DBG_UART_scbEnableWake;
    uint8 DBG_UART_scbEnableIntr;

    /* I2C configuration variables */
    uint8 DBG_UART_mode;
    uint8 DBG_UART_acceptAddr;

    /* SPI/UART configuration variables */
    volatile uint8 * DBG_UART_rxBuffer;
    uint8  DBG_UART_rxDataBits;
    uint32 DBG_UART_rxBufferSize;

    volatile uint8 * DBG_UART_txBuffer;
    uint8  DBG_UART_txDataBits;
    uint32 DBG_UART_txBufferSize;

    /* EZI2C configuration variables */
    uint8 DBG_UART_numberOfAddr;
    uint8 DBG_UART_subAddrSize;
#endif /* (DBG_UART_SCB_MODE_UNCONFIG_CONST_CFG) */


/***************************************
*     Common SCB Vars
***************************************/
/**
* \addtogroup group_general
* \{
*/

/** DBG_UART_initVar indicates whether the DBG_UART 
*  component has been initialized. The variable is initialized to 0 
*  and set to 1 the first time SCB_Start() is called. This allows 
*  the component to restart without reinitialization after the first 
*  call to the DBG_UART_Start() routine.
*
*  If re-initialization of the component is required, then the 
*  DBG_UART_Init() function can be called before the 
*  DBG_UART_Start() or DBG_UART_Enable() function.
*/
uint8 DBG_UART_initVar = 0u;


#if (! (DBG_UART_SCB_MODE_I2C_CONST_CFG || \
        DBG_UART_SCB_MODE_EZI2C_CONST_CFG))
    /** This global variable stores TX interrupt sources after 
    * DBG_UART_Stop() is called. Only these TX interrupt sources 
    * will be restored on a subsequent DBG_UART_Enable() call.
    */
    uint16 DBG_UART_IntrTxMask = 0u;
#endif /* (! (DBG_UART_SCB_MODE_I2C_CONST_CFG || \
              DBG_UART_SCB_MODE_EZI2C_CONST_CFG)) */
/** \} globals */

#if (DBG_UART_SCB_IRQ_INTERNAL)
#if !defined (CY_REMOVE_DBG_UART_CUSTOM_INTR_HANDLER)
    void (*DBG_UART_customIntrHandler)(void) = NULL;
#endif /* !defined (CY_REMOVE_DBG_UART_CUSTOM_INTR_HANDLER) */
#endif /* (DBG_UART_SCB_IRQ_INTERNAL) */


/***************************************
*    Private Function Prototypes
***************************************/

static void DBG_UART_ScbEnableIntr(void);
static void DBG_UART_ScbModeStop(void);
static void DBG_UART_ScbModePostEnable(void);


/*******************************************************************************
* Function Name: DBG_UART_Init
****************************************************************************//**
*
*  Initializes the DBG_UART component to operate in one of the selected
*  configurations: I2C, SPI, UART or EZI2C.
*  When the configuration is set to "Unconfigured SCB", this function does
*  not do any initialization. Use mode-specific initialization APIs instead:
*  DBG_UART_I2CInit, DBG_UART_SpiInit, 
*  DBG_UART_UartInit or DBG_UART_EzI2CInit.
*
*******************************************************************************/
void DBG_UART_Init(void)
{
#if (DBG_UART_SCB_MODE_UNCONFIG_CONST_CFG)
    if (DBG_UART_SCB_MODE_UNCONFIG_RUNTM_CFG)
    {
        DBG_UART_initVar = 0u;
    }
    else
    {
        /* Initialization was done before this function call */
    }

#elif (DBG_UART_SCB_MODE_I2C_CONST_CFG)
    DBG_UART_I2CInit();

#elif (DBG_UART_SCB_MODE_SPI_CONST_CFG)
    DBG_UART_SpiInit();

#elif (DBG_UART_SCB_MODE_UART_CONST_CFG)
    DBG_UART_UartInit();

#elif (DBG_UART_SCB_MODE_EZI2C_CONST_CFG)
    DBG_UART_EzI2CInit();

#endif /* (DBG_UART_SCB_MODE_UNCONFIG_CONST_CFG) */
}


/*******************************************************************************
* Function Name: DBG_UART_Enable
****************************************************************************//**
*
*  Enables DBG_UART component operation: activates the hardware and 
*  internal interrupt. It also restores TX interrupt sources disabled after the 
*  DBG_UART_Stop() function was called (note that level-triggered TX 
*  interrupt sources remain disabled to not cause code lock-up).
*  For I2C and EZI2C modes the interrupt is internal and mandatory for 
*  operation. For SPI and UART modes the interrupt can be configured as none, 
*  internal or external.
*  The DBG_UART configuration should be not changed when the component
*  is enabled. Any configuration changes should be made after disabling the 
*  component.
*  When configuration is set to “Unconfigured DBG_UART”, the component 
*  must first be initialized to operate in one of the following configurations: 
*  I2C, SPI, UART or EZ I2C, using the mode-specific initialization API. 
*  Otherwise this function does not enable the component.
*
*******************************************************************************/
void DBG_UART_Enable(void)
{
#if (DBG_UART_SCB_MODE_UNCONFIG_CONST_CFG)
    /* Enable SCB block, only if it is already configured */
    if (!DBG_UART_SCB_MODE_UNCONFIG_RUNTM_CFG)
    {
        DBG_UART_CTRL_REG |= DBG_UART_CTRL_ENABLED;

        DBG_UART_ScbEnableIntr();

        /* Call PostEnable function specific to current operation mode */
        DBG_UART_ScbModePostEnable();
    }
#else
    DBG_UART_CTRL_REG |= DBG_UART_CTRL_ENABLED;

    DBG_UART_ScbEnableIntr();

    /* Call PostEnable function specific to current operation mode */
    DBG_UART_ScbModePostEnable();
#endif /* (DBG_UART_SCB_MODE_UNCONFIG_CONST_CFG) */
}


/*******************************************************************************
* Function Name: DBG_UART_Start
****************************************************************************//**
*
*  Invokes DBG_UART_Init() and DBG_UART_Enable().
*  After this function call, the component is enabled and ready for operation.
*  When configuration is set to "Unconfigured SCB", the component must first be
*  initialized to operate in one of the following configurations: I2C, SPI, UART
*  or EZI2C. Otherwise this function does not enable the component.
*
* \globalvars
*  DBG_UART_initVar - used to check initial configuration, modified
*  on first function call.
*
*******************************************************************************/
void DBG_UART_Start(void)
{
    if (0u == DBG_UART_initVar)
    {
        DBG_UART_Init();
        DBG_UART_initVar = 1u; /* Component was initialized */
    }

    DBG_UART_Enable();
}


/*******************************************************************************
* Function Name: DBG_UART_Stop
****************************************************************************//**
*
*  Disables the DBG_UART component: disable the hardware and internal 
*  interrupt. It also disables all TX interrupt sources so as not to cause an 
*  unexpected interrupt trigger because after the component is enabled, the 
*  TX FIFO is empty.
*  Refer to the function DBG_UART_Enable() for the interrupt 
*  configuration details.
*  This function disables the SCB component without checking to see if 
*  communication is in progress. Before calling this function it may be 
*  necessary to check the status of communication to make sure communication 
*  is complete. If this is not done then communication could be stopped mid 
*  byte and corrupted data could result.
*
*******************************************************************************/
void DBG_UART_Stop(void)
{
#if (DBG_UART_SCB_IRQ_INTERNAL)
    DBG_UART_DisableInt();
#endif /* (DBG_UART_SCB_IRQ_INTERNAL) */

    /* Call Stop function specific to current operation mode */
    DBG_UART_ScbModeStop();

    /* Disable SCB IP */
    DBG_UART_CTRL_REG &= (uint32) ~DBG_UART_CTRL_ENABLED;

    /* Disable all TX interrupt sources so as not to cause an unexpected
    * interrupt trigger after the component will be enabled because the 
    * TX FIFO is empty.
    * For SCB IP v0, it is critical as it does not mask-out interrupt
    * sources when it is disabled. This can cause a code lock-up in the
    * interrupt handler because TX FIFO cannot be loaded after the block
    * is disabled.
    */
    DBG_UART_SetTxInterruptMode(DBG_UART_NO_INTR_SOURCES);

#if (DBG_UART_SCB_IRQ_INTERNAL)
    DBG_UART_ClearPendingInt();
#endif /* (DBG_UART_SCB_IRQ_INTERNAL) */
}


/*******************************************************************************
* Function Name: DBG_UART_SetRxFifoLevel
****************************************************************************//**
*
*  Sets level in the RX FIFO to generate a RX level interrupt.
*  When the RX FIFO has more entries than the RX FIFO level an RX level
*  interrupt request is generated.
*
*  \param level: Level in the RX FIFO to generate RX level interrupt.
*   The range of valid level values is between 0 and RX FIFO depth - 1.
*
*******************************************************************************/
void DBG_UART_SetRxFifoLevel(uint32 level)
{
    uint32 rxFifoCtrl;

    rxFifoCtrl = DBG_UART_RX_FIFO_CTRL_REG;

    rxFifoCtrl &= ((uint32) ~DBG_UART_RX_FIFO_CTRL_TRIGGER_LEVEL_MASK); /* Clear level mask bits */
    rxFifoCtrl |= ((uint32) (DBG_UART_RX_FIFO_CTRL_TRIGGER_LEVEL_MASK & level));

    DBG_UART_RX_FIFO_CTRL_REG = rxFifoCtrl;
}


/*******************************************************************************
* Function Name: DBG_UART_SetTxFifoLevel
****************************************************************************//**
*
*  Sets level in the TX FIFO to generate a TX level interrupt.
*  When the TX FIFO has less entries than the TX FIFO level an TX level
*  interrupt request is generated.
*
*  \param level: Level in the TX FIFO to generate TX level interrupt.
*   The range of valid level values is between 0 and TX FIFO depth - 1.
*
*******************************************************************************/
void DBG_UART_SetTxFifoLevel(uint32 level)
{
    uint32 txFifoCtrl;

    txFifoCtrl = DBG_UART_TX_FIFO_CTRL_REG;

    txFifoCtrl &= ((uint32) ~DBG_UART_TX_FIFO_CTRL_TRIGGER_LEVEL_MASK); /* Clear level mask bits */
    txFifoCtrl |= ((uint32) (DBG_UART_TX_FIFO_CTRL_TRIGGER_LEVEL_MASK & level));

    DBG_UART_TX_FIFO_CTRL_REG = txFifoCtrl;
}


#if (DBG_UART_SCB_IRQ_INTERNAL)
    /*******************************************************************************
    * Function Name: DBG_UART_SetCustomInterruptHandler
    ****************************************************************************//**
    *
    *  Registers a function to be called by the internal interrupt handler.
    *  First the function that is registered is called, then the internal interrupt
    *  handler performs any operation such as software buffer management functions
    *  before the interrupt returns.  It is the user's responsibility not to break
    *  the software buffer operations. Only one custom handler is supported, which
    *  is the function provided by the most recent call.
    *  At the initialization time no custom handler is registered.
    *
    *  \param func: Pointer to the function to register.
    *        The value NULL indicates to remove the current custom interrupt
    *        handler.
    *
    *******************************************************************************/
    void DBG_UART_SetCustomInterruptHandler(void (*func)(void))
    {
    #if !defined (CY_REMOVE_DBG_UART_CUSTOM_INTR_HANDLER)
        DBG_UART_customIntrHandler = func; /* Register interrupt handler */
    #else
        if (NULL != func)
        {
            /* Suppress compiler warning */
        }
    #endif /* !defined (CY_REMOVE_DBG_UART_CUSTOM_INTR_HANDLER) */
    }
#endif /* (DBG_UART_SCB_IRQ_INTERNAL) */


/*******************************************************************************
* Function Name: DBG_UART_ScbModeEnableIntr
****************************************************************************//**
*
*  Enables an interrupt for a specific mode.
*
*******************************************************************************/
static void DBG_UART_ScbEnableIntr(void)
{
#if (DBG_UART_SCB_IRQ_INTERNAL)
    /* Enable interrupt in NVIC */
    #if (DBG_UART_SCB_MODE_UNCONFIG_CONST_CFG)
        if (0u != DBG_UART_scbEnableIntr)
        {
            DBG_UART_EnableInt();
        }

    #else
        DBG_UART_EnableInt();

    #endif /* (DBG_UART_SCB_MODE_UNCONFIG_CONST_CFG) */
#endif /* (DBG_UART_SCB_IRQ_INTERNAL) */
}


/*******************************************************************************
* Function Name: DBG_UART_ScbModePostEnable
****************************************************************************//**
*
*  Calls the PostEnable function for a specific operation mode.
*
*******************************************************************************/
static void DBG_UART_ScbModePostEnable(void)
{
#if (DBG_UART_SCB_MODE_UNCONFIG_CONST_CFG)
#if (!DBG_UART_CY_SCBIP_V1)
    if (DBG_UART_SCB_MODE_SPI_RUNTM_CFG)
    {
        DBG_UART_SpiPostEnable();
    }
    else if (DBG_UART_SCB_MODE_UART_RUNTM_CFG)
    {
        DBG_UART_UartPostEnable();
    }
    else
    {
        /* Unknown mode: do nothing */
    }
#endif /* (!DBG_UART_CY_SCBIP_V1) */

#elif (DBG_UART_SCB_MODE_SPI_CONST_CFG)
    DBG_UART_SpiPostEnable();

#elif (DBG_UART_SCB_MODE_UART_CONST_CFG)
    DBG_UART_UartPostEnable();

#else
    /* Unknown mode: do nothing */
#endif /* (DBG_UART_SCB_MODE_UNCONFIG_CONST_CFG) */
}


/*******************************************************************************
* Function Name: DBG_UART_ScbModeStop
****************************************************************************//**
*
*  Calls the Stop function for a specific operation mode.
*
*******************************************************************************/
static void DBG_UART_ScbModeStop(void)
{
#if (DBG_UART_SCB_MODE_UNCONFIG_CONST_CFG)
    if (DBG_UART_SCB_MODE_I2C_RUNTM_CFG)
    {
        DBG_UART_I2CStop();
    }
    else if (DBG_UART_SCB_MODE_EZI2C_RUNTM_CFG)
    {
        DBG_UART_EzI2CStop();
    }
#if (!DBG_UART_CY_SCBIP_V1)
    else if (DBG_UART_SCB_MODE_SPI_RUNTM_CFG)
    {
        DBG_UART_SpiStop();
    }
    else if (DBG_UART_SCB_MODE_UART_RUNTM_CFG)
    {
        DBG_UART_UartStop();
    }
#endif /* (!DBG_UART_CY_SCBIP_V1) */
    else
    {
        /* Unknown mode: do nothing */
    }
#elif (DBG_UART_SCB_MODE_I2C_CONST_CFG)
    DBG_UART_I2CStop();

#elif (DBG_UART_SCB_MODE_EZI2C_CONST_CFG)
    DBG_UART_EzI2CStop();

#elif (DBG_UART_SCB_MODE_SPI_CONST_CFG)
    DBG_UART_SpiStop();

#elif (DBG_UART_SCB_MODE_UART_CONST_CFG)
    DBG_UART_UartStop();

#else
    /* Unknown mode: do nothing */
#endif /* (DBG_UART_SCB_MODE_UNCONFIG_CONST_CFG) */
}


#if (DBG_UART_SCB_MODE_UNCONFIG_CONST_CFG)
    /*******************************************************************************
    * Function Name: DBG_UART_SetPins
    ****************************************************************************//**
    *
    *  Sets the pins settings accordingly to the selected operation mode.
    *  Only available in the Unconfigured operation mode. The mode specific
    *  initialization function calls it.
    *  Pins configuration is set by PSoC Creator when a specific mode of operation
    *  is selected in design time.
    *
    *  \param mode:      Mode of SCB operation.
    *  \param subMode:   Sub-mode of SCB operation. It is only required for SPI and UART
    *             modes.
    *  \param uartEnableMask: enables TX or RX direction and RTS and CTS signals.
    *
    *******************************************************************************/
    void DBG_UART_SetPins(uint32 mode, uint32 subMode, uint32 uartEnableMask)
    {
        uint32 pinsDm[DBG_UART_SCB_PINS_NUMBER];
        uint32 i;
        
    #if (!DBG_UART_CY_SCBIP_V1)
        uint32 pinsInBuf = 0u;
    #endif /* (!DBG_UART_CY_SCBIP_V1) */
        
        uint32 hsiomSel[DBG_UART_SCB_PINS_NUMBER] = 
        {
            DBG_UART_RX_SCL_MOSI_HSIOM_SEL_GPIO,
            DBG_UART_TX_SDA_MISO_HSIOM_SEL_GPIO,
            0u,
            0u,
            0u,
            0u,
            0u,
        };

    #if (DBG_UART_CY_SCBIP_V1)
        /* Supress compiler warning. */
        if ((0u == subMode) || (0u == uartEnableMask))
        {
        }
    #endif /* (DBG_UART_CY_SCBIP_V1) */

        /* Set default HSIOM to GPIO and Drive Mode to Analog Hi-Z */
        for (i = 0u; i < DBG_UART_SCB_PINS_NUMBER; i++)
        {
            pinsDm[i] = DBG_UART_PIN_DM_ALG_HIZ;
        }

        if ((DBG_UART_SCB_MODE_I2C   == mode) ||
            (DBG_UART_SCB_MODE_EZI2C == mode))
        {
        #if (DBG_UART_RX_SCL_MOSI_PIN)
            hsiomSel[DBG_UART_RX_SCL_MOSI_PIN_INDEX] = DBG_UART_RX_SCL_MOSI_HSIOM_SEL_I2C;
            pinsDm  [DBG_UART_RX_SCL_MOSI_PIN_INDEX] = DBG_UART_PIN_DM_OD_LO;
        #elif (DBG_UART_RX_WAKE_SCL_MOSI_PIN)
            hsiomSel[DBG_UART_RX_WAKE_SCL_MOSI_PIN_INDEX] = DBG_UART_RX_WAKE_SCL_MOSI_HSIOM_SEL_I2C;
            pinsDm  [DBG_UART_RX_WAKE_SCL_MOSI_PIN_INDEX] = DBG_UART_PIN_DM_OD_LO;
        #else
        #endif /* (DBG_UART_RX_SCL_MOSI_PIN) */
        
        #if (DBG_UART_TX_SDA_MISO_PIN)
            hsiomSel[DBG_UART_TX_SDA_MISO_PIN_INDEX] = DBG_UART_TX_SDA_MISO_HSIOM_SEL_I2C;
            pinsDm  [DBG_UART_TX_SDA_MISO_PIN_INDEX] = DBG_UART_PIN_DM_OD_LO;
        #endif /* (DBG_UART_TX_SDA_MISO_PIN) */
        }
    #if (!DBG_UART_CY_SCBIP_V1)
        else if (DBG_UART_SCB_MODE_SPI == mode)
        {
        #if (DBG_UART_RX_SCL_MOSI_PIN)
            hsiomSel[DBG_UART_RX_SCL_MOSI_PIN_INDEX] = DBG_UART_RX_SCL_MOSI_HSIOM_SEL_SPI;
        #elif (DBG_UART_RX_WAKE_SCL_MOSI_PIN)
            hsiomSel[DBG_UART_RX_WAKE_SCL_MOSI_PIN_INDEX] = DBG_UART_RX_WAKE_SCL_MOSI_HSIOM_SEL_SPI;
        #else
        #endif /* (DBG_UART_RX_SCL_MOSI_PIN) */
        
        #if (DBG_UART_TX_SDA_MISO_PIN)
            hsiomSel[DBG_UART_TX_SDA_MISO_PIN_INDEX] = DBG_UART_TX_SDA_MISO_HSIOM_SEL_SPI;
        #endif /* (DBG_UART_TX_SDA_MISO_PIN) */
        
        #if (DBG_UART_CTS_SCLK_PIN)
            hsiomSel[DBG_UART_CTS_SCLK_PIN_INDEX] = DBG_UART_CTS_SCLK_HSIOM_SEL_SPI;
        #endif /* (DBG_UART_CTS_SCLK_PIN) */

            if (DBG_UART_SPI_SLAVE == subMode)
            {
                /* Slave */
                pinsDm[DBG_UART_RX_SCL_MOSI_PIN_INDEX] = DBG_UART_PIN_DM_DIG_HIZ;
                pinsDm[DBG_UART_TX_SDA_MISO_PIN_INDEX] = DBG_UART_PIN_DM_STRONG;
                pinsDm[DBG_UART_CTS_SCLK_PIN_INDEX] = DBG_UART_PIN_DM_DIG_HIZ;

            #if (DBG_UART_RTS_SS0_PIN)
                /* Only SS0 is valid choice for Slave */
                hsiomSel[DBG_UART_RTS_SS0_PIN_INDEX] = DBG_UART_RTS_SS0_HSIOM_SEL_SPI;
                pinsDm  [DBG_UART_RTS_SS0_PIN_INDEX] = DBG_UART_PIN_DM_DIG_HIZ;
            #endif /* (DBG_UART_RTS_SS0_PIN) */

            #if (DBG_UART_TX_SDA_MISO_PIN)
                /* Disable input buffer */
                 pinsInBuf |= DBG_UART_TX_SDA_MISO_PIN_MASK;
            #endif /* (DBG_UART_TX_SDA_MISO_PIN) */
            }
            else 
            {
                /* (Master) */
                pinsDm[DBG_UART_RX_SCL_MOSI_PIN_INDEX] = DBG_UART_PIN_DM_STRONG;
                pinsDm[DBG_UART_TX_SDA_MISO_PIN_INDEX] = DBG_UART_PIN_DM_DIG_HIZ;
                pinsDm[DBG_UART_CTS_SCLK_PIN_INDEX] = DBG_UART_PIN_DM_STRONG;

            #if (DBG_UART_RTS_SS0_PIN)
                hsiomSel [DBG_UART_RTS_SS0_PIN_INDEX] = DBG_UART_RTS_SS0_HSIOM_SEL_SPI;
                pinsDm   [DBG_UART_RTS_SS0_PIN_INDEX] = DBG_UART_PIN_DM_STRONG;
                pinsInBuf |= DBG_UART_RTS_SS0_PIN_MASK;
            #endif /* (DBG_UART_RTS_SS0_PIN) */

            #if (DBG_UART_SS1_PIN)
                hsiomSel [DBG_UART_SS1_PIN_INDEX] = DBG_UART_SS1_HSIOM_SEL_SPI;
                pinsDm   [DBG_UART_SS1_PIN_INDEX] = DBG_UART_PIN_DM_STRONG;
                pinsInBuf |= DBG_UART_SS1_PIN_MASK;
            #endif /* (DBG_UART_SS1_PIN) */

            #if (DBG_UART_SS2_PIN)
                hsiomSel [DBG_UART_SS2_PIN_INDEX] = DBG_UART_SS2_HSIOM_SEL_SPI;
                pinsDm   [DBG_UART_SS2_PIN_INDEX] = DBG_UART_PIN_DM_STRONG;
                pinsInBuf |= DBG_UART_SS2_PIN_MASK;
            #endif /* (DBG_UART_SS2_PIN) */

            #if (DBG_UART_SS3_PIN)
                hsiomSel [DBG_UART_SS3_PIN_INDEX] = DBG_UART_SS3_HSIOM_SEL_SPI;
                pinsDm   [DBG_UART_SS3_PIN_INDEX] = DBG_UART_PIN_DM_STRONG;
                pinsInBuf |= DBG_UART_SS3_PIN_MASK;
            #endif /* (DBG_UART_SS3_PIN) */

                /* Disable input buffers */
            #if (DBG_UART_RX_SCL_MOSI_PIN)
                pinsInBuf |= DBG_UART_RX_SCL_MOSI_PIN_MASK;
            #elif (DBG_UART_RX_WAKE_SCL_MOSI_PIN)
                pinsInBuf |= DBG_UART_RX_WAKE_SCL_MOSI_PIN_MASK;
            #else
            #endif /* (DBG_UART_RX_SCL_MOSI_PIN) */

            #if (DBG_UART_CTS_SCLK_PIN)
                pinsInBuf |= DBG_UART_CTS_SCLK_PIN_MASK;
            #endif /* (DBG_UART_CTS_SCLK_PIN) */
            }
        }
        else /* UART */
        {
            if (DBG_UART_UART_MODE_SMARTCARD == subMode)
            {
                /* SmartCard */
            #if (DBG_UART_TX_SDA_MISO_PIN)
                hsiomSel[DBG_UART_TX_SDA_MISO_PIN_INDEX] = DBG_UART_TX_SDA_MISO_HSIOM_SEL_UART;
                pinsDm  [DBG_UART_TX_SDA_MISO_PIN_INDEX] = DBG_UART_PIN_DM_OD_LO;
            #endif /* (DBG_UART_TX_SDA_MISO_PIN) */
            }
            else /* Standard or IrDA */
            {
                if (0u != (DBG_UART_UART_RX_PIN_ENABLE & uartEnableMask))
                {
                #if (DBG_UART_RX_SCL_MOSI_PIN)
                    hsiomSel[DBG_UART_RX_SCL_MOSI_PIN_INDEX] = DBG_UART_RX_SCL_MOSI_HSIOM_SEL_UART;
                    pinsDm  [DBG_UART_RX_SCL_MOSI_PIN_INDEX] = DBG_UART_PIN_DM_DIG_HIZ;
                #elif (DBG_UART_RX_WAKE_SCL_MOSI_PIN)
                    hsiomSel[DBG_UART_RX_WAKE_SCL_MOSI_PIN_INDEX] = DBG_UART_RX_WAKE_SCL_MOSI_HSIOM_SEL_UART;
                    pinsDm  [DBG_UART_RX_WAKE_SCL_MOSI_PIN_INDEX] = DBG_UART_PIN_DM_DIG_HIZ;
                #else
                #endif /* (DBG_UART_RX_SCL_MOSI_PIN) */
                }

                if (0u != (DBG_UART_UART_TX_PIN_ENABLE & uartEnableMask))
                {
                #if (DBG_UART_TX_SDA_MISO_PIN)
                    hsiomSel[DBG_UART_TX_SDA_MISO_PIN_INDEX] = DBG_UART_TX_SDA_MISO_HSIOM_SEL_UART;
                    pinsDm  [DBG_UART_TX_SDA_MISO_PIN_INDEX] = DBG_UART_PIN_DM_STRONG;
                    
                    /* Disable input buffer */
                    pinsInBuf |= DBG_UART_TX_SDA_MISO_PIN_MASK;
                #endif /* (DBG_UART_TX_SDA_MISO_PIN) */
                }

            #if !(DBG_UART_CY_SCBIP_V0 || DBG_UART_CY_SCBIP_V1)
                if (DBG_UART_UART_MODE_STD == subMode)
                {
                    if (0u != (DBG_UART_UART_CTS_PIN_ENABLE & uartEnableMask))
                    {
                        /* CTS input is multiplexed with SCLK */
                    #if (DBG_UART_CTS_SCLK_PIN)
                        hsiomSel[DBG_UART_CTS_SCLK_PIN_INDEX] = DBG_UART_CTS_SCLK_HSIOM_SEL_UART;
                        pinsDm  [DBG_UART_CTS_SCLK_PIN_INDEX] = DBG_UART_PIN_DM_DIG_HIZ;
                    #endif /* (DBG_UART_CTS_SCLK_PIN) */
                    }

                    if (0u != (DBG_UART_UART_RTS_PIN_ENABLE & uartEnableMask))
                    {
                        /* RTS output is multiplexed with SS0 */
                    #if (DBG_UART_RTS_SS0_PIN)
                        hsiomSel[DBG_UART_RTS_SS0_PIN_INDEX] = DBG_UART_RTS_SS0_HSIOM_SEL_UART;
                        pinsDm  [DBG_UART_RTS_SS0_PIN_INDEX] = DBG_UART_PIN_DM_STRONG;
                        
                        /* Disable input buffer */
                        pinsInBuf |= DBG_UART_RTS_SS0_PIN_MASK;
                    #endif /* (DBG_UART_RTS_SS0_PIN) */
                    }
                }
            #endif /* !(DBG_UART_CY_SCBIP_V0 || DBG_UART_CY_SCBIP_V1) */
            }
        }
    #endif /* (!DBG_UART_CY_SCBIP_V1) */

    /* Configure pins: set HSIOM, DM and InputBufEnable */
    /* Note: the DR register settings do not effect the pin output if HSIOM is other than GPIO */

    #if (DBG_UART_RX_SCL_MOSI_PIN)
        DBG_UART_SET_HSIOM_SEL(DBG_UART_RX_SCL_MOSI_HSIOM_REG,
                                       DBG_UART_RX_SCL_MOSI_HSIOM_MASK,
                                       DBG_UART_RX_SCL_MOSI_HSIOM_POS,
                                        hsiomSel[DBG_UART_RX_SCL_MOSI_PIN_INDEX]);

        DBG_UART_uart_rx_i2c_scl_spi_mosi_SetDriveMode((uint8) pinsDm[DBG_UART_RX_SCL_MOSI_PIN_INDEX]);

        #if (!DBG_UART_CY_SCBIP_V1)
            DBG_UART_SET_INP_DIS(DBG_UART_uart_rx_i2c_scl_spi_mosi_INP_DIS,
                                         DBG_UART_uart_rx_i2c_scl_spi_mosi_MASK,
                                         (0u != (pinsInBuf & DBG_UART_RX_SCL_MOSI_PIN_MASK)));
        #endif /* (!DBG_UART_CY_SCBIP_V1) */
    
    #elif (DBG_UART_RX_WAKE_SCL_MOSI_PIN)
        DBG_UART_SET_HSIOM_SEL(DBG_UART_RX_WAKE_SCL_MOSI_HSIOM_REG,
                                       DBG_UART_RX_WAKE_SCL_MOSI_HSIOM_MASK,
                                       DBG_UART_RX_WAKE_SCL_MOSI_HSIOM_POS,
                                       hsiomSel[DBG_UART_RX_WAKE_SCL_MOSI_PIN_INDEX]);

        DBG_UART_uart_rx_wake_i2c_scl_spi_mosi_SetDriveMode((uint8)
                                                               pinsDm[DBG_UART_RX_WAKE_SCL_MOSI_PIN_INDEX]);

        DBG_UART_SET_INP_DIS(DBG_UART_uart_rx_wake_i2c_scl_spi_mosi_INP_DIS,
                                     DBG_UART_uart_rx_wake_i2c_scl_spi_mosi_MASK,
                                     (0u != (pinsInBuf & DBG_UART_RX_WAKE_SCL_MOSI_PIN_MASK)));

         /* Set interrupt on falling edge */
        DBG_UART_SET_INCFG_TYPE(DBG_UART_RX_WAKE_SCL_MOSI_INTCFG_REG,
                                        DBG_UART_RX_WAKE_SCL_MOSI_INTCFG_TYPE_MASK,
                                        DBG_UART_RX_WAKE_SCL_MOSI_INTCFG_TYPE_POS,
                                        DBG_UART_INTCFG_TYPE_FALLING_EDGE);
    #else
    #endif /* (DBG_UART_RX_WAKE_SCL_MOSI_PIN) */

    #if (DBG_UART_TX_SDA_MISO_PIN)
        DBG_UART_SET_HSIOM_SEL(DBG_UART_TX_SDA_MISO_HSIOM_REG,
                                       DBG_UART_TX_SDA_MISO_HSIOM_MASK,
                                       DBG_UART_TX_SDA_MISO_HSIOM_POS,
                                        hsiomSel[DBG_UART_TX_SDA_MISO_PIN_INDEX]);

        DBG_UART_uart_tx_i2c_sda_spi_miso_SetDriveMode((uint8) pinsDm[DBG_UART_TX_SDA_MISO_PIN_INDEX]);

    #if (!DBG_UART_CY_SCBIP_V1)
        DBG_UART_SET_INP_DIS(DBG_UART_uart_tx_i2c_sda_spi_miso_INP_DIS,
                                     DBG_UART_uart_tx_i2c_sda_spi_miso_MASK,
                                    (0u != (pinsInBuf & DBG_UART_TX_SDA_MISO_PIN_MASK)));
    #endif /* (!DBG_UART_CY_SCBIP_V1) */
    #endif /* (DBG_UART_RX_SCL_MOSI_PIN) */

    #if (DBG_UART_CTS_SCLK_PIN)
        DBG_UART_SET_HSIOM_SEL(DBG_UART_CTS_SCLK_HSIOM_REG,
                                       DBG_UART_CTS_SCLK_HSIOM_MASK,
                                       DBG_UART_CTS_SCLK_HSIOM_POS,
                                       hsiomSel[DBG_UART_CTS_SCLK_PIN_INDEX]);

        DBG_UART_uart_cts_spi_sclk_SetDriveMode((uint8) pinsDm[DBG_UART_CTS_SCLK_PIN_INDEX]);

        DBG_UART_SET_INP_DIS(DBG_UART_uart_cts_spi_sclk_INP_DIS,
                                     DBG_UART_uart_cts_spi_sclk_MASK,
                                     (0u != (pinsInBuf & DBG_UART_CTS_SCLK_PIN_MASK)));
    #endif /* (DBG_UART_CTS_SCLK_PIN) */

    #if (DBG_UART_RTS_SS0_PIN)
        DBG_UART_SET_HSIOM_SEL(DBG_UART_RTS_SS0_HSIOM_REG,
                                       DBG_UART_RTS_SS0_HSIOM_MASK,
                                       DBG_UART_RTS_SS0_HSIOM_POS,
                                       hsiomSel[DBG_UART_RTS_SS0_PIN_INDEX]);

        DBG_UART_uart_rts_spi_ss0_SetDriveMode((uint8) pinsDm[DBG_UART_RTS_SS0_PIN_INDEX]);

        DBG_UART_SET_INP_DIS(DBG_UART_uart_rts_spi_ss0_INP_DIS,
                                     DBG_UART_uart_rts_spi_ss0_MASK,
                                     (0u != (pinsInBuf & DBG_UART_RTS_SS0_PIN_MASK)));
    #endif /* (DBG_UART_RTS_SS0_PIN) */

    #if (DBG_UART_SS1_PIN)
        DBG_UART_SET_HSIOM_SEL(DBG_UART_SS1_HSIOM_REG,
                                       DBG_UART_SS1_HSIOM_MASK,
                                       DBG_UART_SS1_HSIOM_POS,
                                       hsiomSel[DBG_UART_SS1_PIN_INDEX]);

        DBG_UART_spi_ss1_SetDriveMode((uint8) pinsDm[DBG_UART_SS1_PIN_INDEX]);

        DBG_UART_SET_INP_DIS(DBG_UART_spi_ss1_INP_DIS,
                                     DBG_UART_spi_ss1_MASK,
                                     (0u != (pinsInBuf & DBG_UART_SS1_PIN_MASK)));
    #endif /* (DBG_UART_SS1_PIN) */

    #if (DBG_UART_SS2_PIN)
        DBG_UART_SET_HSIOM_SEL(DBG_UART_SS2_HSIOM_REG,
                                       DBG_UART_SS2_HSIOM_MASK,
                                       DBG_UART_SS2_HSIOM_POS,
                                       hsiomSel[DBG_UART_SS2_PIN_INDEX]);

        DBG_UART_spi_ss2_SetDriveMode((uint8) pinsDm[DBG_UART_SS2_PIN_INDEX]);

        DBG_UART_SET_INP_DIS(DBG_UART_spi_ss2_INP_DIS,
                                     DBG_UART_spi_ss2_MASK,
                                     (0u != (pinsInBuf & DBG_UART_SS2_PIN_MASK)));
    #endif /* (DBG_UART_SS2_PIN) */

    #if (DBG_UART_SS3_PIN)
        DBG_UART_SET_HSIOM_SEL(DBG_UART_SS3_HSIOM_REG,
                                       DBG_UART_SS3_HSIOM_MASK,
                                       DBG_UART_SS3_HSIOM_POS,
                                       hsiomSel[DBG_UART_SS3_PIN_INDEX]);

        DBG_UART_spi_ss3_SetDriveMode((uint8) pinsDm[DBG_UART_SS3_PIN_INDEX]);

        DBG_UART_SET_INP_DIS(DBG_UART_spi_ss3_INP_DIS,
                                     DBG_UART_spi_ss3_MASK,
                                     (0u != (pinsInBuf & DBG_UART_SS3_PIN_MASK)));
    #endif /* (DBG_UART_SS3_PIN) */
    }

#endif /* (DBG_UART_SCB_MODE_UNCONFIG_CONST_CFG) */


#if (DBG_UART_CY_SCBIP_V0 || DBG_UART_CY_SCBIP_V1)
    /*******************************************************************************
    * Function Name: DBG_UART_I2CSlaveNackGeneration
    ****************************************************************************//**
    *
    *  Sets command to generate NACK to the address or data.
    *
    *******************************************************************************/
    void DBG_UART_I2CSlaveNackGeneration(void)
    {
        /* Check for EC_AM toggle condition: EC_AM and clock stretching for address are enabled */
        if ((0u != (DBG_UART_CTRL_REG & DBG_UART_CTRL_EC_AM_MODE)) &&
            (0u == (DBG_UART_I2C_CTRL_REG & DBG_UART_I2C_CTRL_S_NOT_READY_ADDR_NACK)))
        {
            /* Toggle EC_AM before NACK generation */
            DBG_UART_CTRL_REG &= ~DBG_UART_CTRL_EC_AM_MODE;
            DBG_UART_CTRL_REG |=  DBG_UART_CTRL_EC_AM_MODE;
        }

        DBG_UART_I2C_SLAVE_CMD_REG = DBG_UART_I2C_SLAVE_CMD_S_NACK;
    }
#endif /* (DBG_UART_CY_SCBIP_V0 || DBG_UART_CY_SCBIP_V1) */


/* [] END OF FILE */
