/***************************************************************************//**
* \file DBG_UART_SPI_UART_INT.c
* \version 4.0
*
* \brief
*  This file provides the source code to the Interrupt Service Routine for
*  the SCB Component in SPI and UART modes.
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

#include "DBG_UART_PVT.h"
#include "DBG_UART_SPI_UART_PVT.h"
#include "cyapicallbacks.h"

#if (DBG_UART_SCB_IRQ_INTERNAL)
/*******************************************************************************
* Function Name: DBG_UART_SPI_UART_ISR
****************************************************************************//**
*
*  Handles the Interrupt Service Routine for the SCB SPI or UART modes.
*
*******************************************************************************/
CY_ISR(DBG_UART_SPI_UART_ISR)
{
#if (DBG_UART_INTERNAL_RX_SW_BUFFER_CONST)
    uint32 locHead;
#endif /* (DBG_UART_INTERNAL_RX_SW_BUFFER_CONST) */

#if (DBG_UART_INTERNAL_TX_SW_BUFFER_CONST)
    uint32 locTail;
#endif /* (DBG_UART_INTERNAL_TX_SW_BUFFER_CONST) */

#ifdef DBG_UART_SPI_UART_ISR_ENTRY_CALLBACK
    DBG_UART_SPI_UART_ISR_EntryCallback();
#endif /* DBG_UART_SPI_UART_ISR_ENTRY_CALLBACK */

    if (NULL != DBG_UART_customIntrHandler)
    {
        DBG_UART_customIntrHandler();
    }

    #if(DBG_UART_CHECK_SPI_WAKE_ENABLE)
    {
        /* Clear SPI wakeup source */
        DBG_UART_ClearSpiExtClkInterruptSource(DBG_UART_INTR_SPI_EC_WAKE_UP);
    }
    #endif

    #if (DBG_UART_CHECK_RX_SW_BUFFER)
    {
        if (DBG_UART_CHECK_INTR_RX_MASKED(DBG_UART_INTR_RX_NOT_EMPTY))
        {
            do
            {
                /* Move local head index */
                locHead = (DBG_UART_rxBufferHead + 1u);

                /* Adjust local head index */
                if (DBG_UART_INTERNAL_RX_BUFFER_SIZE == locHead)
                {
                    locHead = 0u;
                }

                if (locHead == DBG_UART_rxBufferTail)
                {
                    #if (DBG_UART_CHECK_UART_RTS_CONTROL_FLOW)
                    {
                        /* There is no space in the software buffer - disable the
                        * RX Not Empty interrupt source. The data elements are
                        * still being received into the RX FIFO until the RTS signal
                        * stops the transmitter. After the data element is read from the
                        * buffer, the RX Not Empty interrupt source is enabled to
                        * move the next data element in the software buffer.
                        */
                        DBG_UART_INTR_RX_MASK_REG &= ~DBG_UART_INTR_RX_NOT_EMPTY;
                        break;
                    }
                    #else
                    {
                        /* Overflow: through away received data element */
                        (void) DBG_UART_RX_FIFO_RD_REG;
                        DBG_UART_rxBufferOverflow = (uint8) DBG_UART_INTR_RX_OVERFLOW;
                    }
                    #endif
                }
                else
                {
                    /* Store received data */
                    DBG_UART_PutWordInRxBuffer(locHead, DBG_UART_RX_FIFO_RD_REG);

                    /* Move head index */
                    DBG_UART_rxBufferHead = locHead;
                }
            }
            while(0u != DBG_UART_GET_RX_FIFO_ENTRIES);

            DBG_UART_ClearRxInterruptSource(DBG_UART_INTR_RX_NOT_EMPTY);
        }
    }
    #endif


    #if (DBG_UART_CHECK_TX_SW_BUFFER)
    {
        if (DBG_UART_CHECK_INTR_TX_MASKED(DBG_UART_INTR_TX_NOT_FULL))
        {
            do
            {
                /* Check for room in TX software buffer */
                if (DBG_UART_txBufferHead != DBG_UART_txBufferTail)
                {
                    /* Move local tail index */
                    locTail = (DBG_UART_txBufferTail + 1u);

                    /* Adjust local tail index */
                    if (DBG_UART_TX_BUFFER_SIZE == locTail)
                    {
                        locTail = 0u;
                    }

                    /* Put data into TX FIFO */
                    DBG_UART_TX_FIFO_WR_REG = DBG_UART_GetWordFromTxBuffer(locTail);

                    /* Move tail index */
                    DBG_UART_txBufferTail = locTail;
                }
                else
                {
                    /* TX software buffer is empty: complete transfer */
                    DBG_UART_DISABLE_INTR_TX(DBG_UART_INTR_TX_NOT_FULL);
                    break;
                }
            }
            while (DBG_UART_SPI_UART_FIFO_SIZE != DBG_UART_GET_TX_FIFO_ENTRIES);

            DBG_UART_ClearTxInterruptSource(DBG_UART_INTR_TX_NOT_FULL);
        }
    }
    #endif

#ifdef DBG_UART_SPI_UART_ISR_EXIT_CALLBACK
    DBG_UART_SPI_UART_ISR_ExitCallback();
#endif /* DBG_UART_SPI_UART_ISR_EXIT_CALLBACK */

}

#endif /* (DBG_UART_SCB_IRQ_INTERNAL) */


/* [] END OF FILE */
