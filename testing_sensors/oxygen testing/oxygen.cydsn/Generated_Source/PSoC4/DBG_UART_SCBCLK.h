/*******************************************************************************
* File Name: DBG_UART_SCBCLK.h
* Version 2.20
*
*  Description:
*   Provides the function and constant definitions for the clock component.
*
*  Note:
*
********************************************************************************
* Copyright 2008-2012, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_CLOCK_DBG_UART_SCBCLK_H)
#define CY_CLOCK_DBG_UART_SCBCLK_H

#include <cytypes.h>
#include <cyfitter.h>


/***************************************
*        Function Prototypes
***************************************/
#if defined CYREG_PERI_DIV_CMD

void DBG_UART_SCBCLK_StartEx(uint32 alignClkDiv);
#define DBG_UART_SCBCLK_Start() \
    DBG_UART_SCBCLK_StartEx(DBG_UART_SCBCLK__PA_DIV_ID)

#else

void DBG_UART_SCBCLK_Start(void);

#endif/* CYREG_PERI_DIV_CMD */

void DBG_UART_SCBCLK_Stop(void);

void DBG_UART_SCBCLK_SetFractionalDividerRegister(uint16 clkDivider, uint8 clkFractional);

uint16 DBG_UART_SCBCLK_GetDividerRegister(void);
uint8  DBG_UART_SCBCLK_GetFractionalDividerRegister(void);

#define DBG_UART_SCBCLK_Enable()                         DBG_UART_SCBCLK_Start()
#define DBG_UART_SCBCLK_Disable()                        DBG_UART_SCBCLK_Stop()
#define DBG_UART_SCBCLK_SetDividerRegister(clkDivider, reset)  \
    DBG_UART_SCBCLK_SetFractionalDividerRegister((clkDivider), 0u)
#define DBG_UART_SCBCLK_SetDivider(clkDivider)           DBG_UART_SCBCLK_SetDividerRegister((clkDivider), 1u)
#define DBG_UART_SCBCLK_SetDividerValue(clkDivider)      DBG_UART_SCBCLK_SetDividerRegister((clkDivider) - 1u, 1u)


/***************************************
*             Registers
***************************************/
#if defined CYREG_PERI_DIV_CMD

#define DBG_UART_SCBCLK_DIV_ID     DBG_UART_SCBCLK__DIV_ID

#define DBG_UART_SCBCLK_CMD_REG    (*(reg32 *)CYREG_PERI_DIV_CMD)
#define DBG_UART_SCBCLK_CTRL_REG   (*(reg32 *)DBG_UART_SCBCLK__CTRL_REGISTER)
#define DBG_UART_SCBCLK_DIV_REG    (*(reg32 *)DBG_UART_SCBCLK__DIV_REGISTER)

#define DBG_UART_SCBCLK_CMD_DIV_SHIFT          (0u)
#define DBG_UART_SCBCLK_CMD_PA_DIV_SHIFT       (8u)
#define DBG_UART_SCBCLK_CMD_DISABLE_SHIFT      (30u)
#define DBG_UART_SCBCLK_CMD_ENABLE_SHIFT       (31u)

#define DBG_UART_SCBCLK_CMD_DISABLE_MASK       ((uint32)((uint32)1u << DBG_UART_SCBCLK_CMD_DISABLE_SHIFT))
#define DBG_UART_SCBCLK_CMD_ENABLE_MASK        ((uint32)((uint32)1u << DBG_UART_SCBCLK_CMD_ENABLE_SHIFT))

#define DBG_UART_SCBCLK_DIV_FRAC_MASK  (0x000000F8u)
#define DBG_UART_SCBCLK_DIV_FRAC_SHIFT (3u)
#define DBG_UART_SCBCLK_DIV_INT_MASK   (0xFFFFFF00u)
#define DBG_UART_SCBCLK_DIV_INT_SHIFT  (8u)

#else 

#define DBG_UART_SCBCLK_DIV_REG        (*(reg32 *)DBG_UART_SCBCLK__REGISTER)
#define DBG_UART_SCBCLK_ENABLE_REG     DBG_UART_SCBCLK_DIV_REG
#define DBG_UART_SCBCLK_DIV_FRAC_MASK  DBG_UART_SCBCLK__FRAC_MASK
#define DBG_UART_SCBCLK_DIV_FRAC_SHIFT (16u)
#define DBG_UART_SCBCLK_DIV_INT_MASK   DBG_UART_SCBCLK__DIVIDER_MASK
#define DBG_UART_SCBCLK_DIV_INT_SHIFT  (0u)

#endif/* CYREG_PERI_DIV_CMD */

#endif /* !defined(CY_CLOCK_DBG_UART_SCBCLK_H) */

/* [] END OF FILE */
