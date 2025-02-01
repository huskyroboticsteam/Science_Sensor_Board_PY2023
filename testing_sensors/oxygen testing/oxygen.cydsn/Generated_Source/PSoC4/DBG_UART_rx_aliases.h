/*******************************************************************************
* File Name: DBG_UART_rx.h  
* Version 2.20
*
* Description:
*  This file contains the Alias definitions for Per-Pin APIs in cypins.h. 
*  Information on using these APIs can be found in the System Reference Guide.
*
* Note:
*
********************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_PINS_DBG_UART_rx_ALIASES_H) /* Pins DBG_UART_rx_ALIASES_H */
#define CY_PINS_DBG_UART_rx_ALIASES_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"


/***************************************
*              Constants        
***************************************/
#define DBG_UART_rx_0			(DBG_UART_rx__0__PC)
#define DBG_UART_rx_0_PS		(DBG_UART_rx__0__PS)
#define DBG_UART_rx_0_PC		(DBG_UART_rx__0__PC)
#define DBG_UART_rx_0_DR		(DBG_UART_rx__0__DR)
#define DBG_UART_rx_0_SHIFT	(DBG_UART_rx__0__SHIFT)
#define DBG_UART_rx_0_INTR	((uint16)((uint16)0x0003u << (DBG_UART_rx__0__SHIFT*2u)))

#define DBG_UART_rx_INTR_ALL	 ((uint16)(DBG_UART_rx_0_INTR))


#endif /* End Pins DBG_UART_rx_ALIASES_H */


/* [] END OF FILE */
