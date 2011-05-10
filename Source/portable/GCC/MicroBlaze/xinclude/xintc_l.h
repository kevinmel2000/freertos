/* $Id: xintc_l.h,v 1.3 2003/12/23 23:55:21 moleres Exp $ */
/******************************************************************************
*
*       XILINX IS PROVIDING THIS DESIGN, CODE, OR INFORMATION "AS IS"
*       AS A COURTESY TO YOU, SOLELY FOR USE IN DEVELOPING PROGRAMS AND
*       SOLUTIONS FOR XILINX DEVICES.  BY PROVIDING THIS DESIGN, CODE,
*       OR INFORMATION AS ONE POSSIBLE IMPLEMENTATION OF THIS FEATURE,
*       APPLICATION OR STANDARD, XILINX IS MAKING NO REPRESENTATION
*       THAT THIS IMPLEMENTATION IS FREE FROM ANY CLAIMS OF INFRINGEMENT,
*       AND YOU ARE RESPONSIBLE FOR OBTAINING ANY RIGHTS YOU MAY REQUIRE
*       FOR YOUR IMPLEMENTATION.  XILINX EXPRESSLY DISCLAIMS ANY
*       WARRANTY WHATSOEVER WITH RESPECT TO THE ADEQUACY OF THE
*       IMPLEMENTATION, INCLUDING BUT NOT LIMITED TO ANY WARRANTIES OR
*       REPRESENTATIONS THAT THIS IMPLEMENTATION IS FREE FROM CLAIMS OF
*       INFRINGEMENT, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
*       FOR A PARTICULAR PURPOSE.
*
*       (c) Copyright 2002-2003 Xilinx Inc.
*       All rights reserved.
*
******************************************************************************/
/*****************************************************************************/
/**
*
* @file xintc_l.h
*
* This header file contains identifiers and low-level driver functions (or
* macros) that can be used to access the device.  The user should refer to the
* hardware device specification for more details of the device operation.
* High-level driver functions are defined in xintc.h.
*
* Note that users of this Level 0 driver interface can register an interrupt
* handler dynamically (at run-time) using the XIntc_RegisterHandler() function.
* User of the Level 1 xintc driver interface should still use XIntc_Connect(),
* as always.  Also see the discussion of the interrupt vector tables in
* xintc.h.
*
* There are currently two interrupt handlers specified in this interface.
*
* - XIntc_LowLevelInterruptHandler() is a handler without any arguments that
*   is used in cases where there is a single interrupt controller device in
*   the system and the handler cannot be passed an argument. This function is
*   provided mostly for backward compatibility.
*
* - XIntc_DeviceInterruptHandler() is a handler that takes a device ID as an
*   argument, indicating which interrupt controller device in the system is
*   causing the interrupt - thereby supporting multiple interrupt controllers.
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who  Date     Changes
* ----- ---- -------- -----------------------------------------------------
* 1.00b jhl  04/24/02 First release
* 1.00c rpm  10/17/03 New release. Support the static vector table created
*                     in the xintc_g.c configuration table.
* </pre>
*
******************************************************************************/

#ifndef XINTC_L_H /* prevent circular inclusions */
#define XINTC_L_H /* by using protection macros */

/***************************** Include Files *********************************/

#include "xbasic_types.h"
#include "xparameters.h"
#include "xio.h"

#if (XPAR_XINTC_USE_DCR != 0)
#include "xio_dcr.h"
#endif

/************************** Constant Definitions *****************************/

/* define the offsets from the base address for all the registers of the
 * interrupt controller, some registers may be optional in the hardware device
 */
#if (XPAR_XINTC_USE_DCR != 0)

#define XIN_ISR_OFFSET      0       /* Interrupt Status Register */
#define XIN_IPR_OFFSET      1       /* Interrupt Pending Register */
#define XIN_IER_OFFSET      2       /* Interrupt Enable Register */
#define XIN_IAR_OFFSET      3       /* Interrupt Acknowledge Register */
#define XIN_SIE_OFFSET      4       /* Set Interrupt Enable Register */
#define XIN_CIE_OFFSET      5       /* Clear Interrupt Enable Register */
#define XIN_IVR_OFFSET      6       /* Interrupt Vector Register */
#define XIN_MER_OFFSET      7       /* Master Enable Register */

#else /*(XPAR_XINTC_USE_DCR != 0)*/

#define XIN_ISR_OFFSET      0       /* Interrupt Status Register */
#define XIN_IPR_OFFSET      4       /* Interrupt Pending Register */
#define XIN_IER_OFFSET      8       /* Interrupt Enable Register */
#define XIN_IAR_OFFSET      12      /* Interrupt Acknowledge Register */
#define XIN_SIE_OFFSET      16      /* Set Interrupt Enable Register */
#define XIN_CIE_OFFSET      20      /* Clear Interrupt Enable Register */
#define XIN_IVR_OFFSET      24      /* Interrupt Vector Register */
#define XIN_MER_OFFSET      28      /* Master Enable Register */

#endif /*(XPAR_XINTC_USE_DCR != 0)*/

/* Bit definitions for the bits of the MER register */

#define XIN_INT_MASTER_ENABLE_MASK      0x1UL
#define XIN_INT_HARDWARE_ENABLE_MASK    0x2UL /* once set cannot be cleared */

/**************************** Type Definitions *******************************/

/* The following data type defines each entry in an interrupt vector table.
 * The callback reference is the base address of the interrupting device
 * for the low level driver and an instance pointer for the high level driver.
 */
typedef struct
{
    XInterruptHandler Handler;
    void *CallBackRef;
} XIntc_VectorTableEntry;


/***************** Macros (Inline Functions) Definitions *********************/

/*
 * Define the appropriate I/O access method to memory mapped I/O or DCR.
 */
#if (XPAR_XINTC_USE_DCR != 0)

#define XIntc_In32  XIo_DcrIn
#define XIntc_Out32 XIo_DcrOut

#else

#define XIntc_In32  XIo_In32
#define XIntc_Out32 XIo_Out32

#endif

/****************************************************************************/
/**
*
* Enable all interrupts in the Master Enable register of the interrupt
* controller.  The interrupt controller defaults to all interrupts disabled
* from reset such that this macro must be used to enable interrupts.
*
* @param    BaseAddress is the base address of the device.
*
* @return   None.
*
* @note
*
* Signature: void XIntc_mMasterEnable(Xuint32 BaseAddress)
*
*****************************************************************************/
#define XIntc_mMasterEnable(BaseAddress) \
    XIntc_Out32((BaseAddress) + XIN_MER_OFFSET, \
              XIN_INT_MASTER_ENABLE_MASK | XIN_INT_HARDWARE_ENABLE_MASK)

/****************************************************************************/
/**
*
* Disable all interrupts in the Master Enable register of the interrupt
* controller.
*
* @param    BaseAddress is the base address of the device.
*
* @return   None.
*
* @note
*
* Signature: void XIntc_mMasterDisable(Xuint32 BaseAddress)
*
*****************************************************************************/
#define XIntc_mMasterDisable(BaseAddress) \
    XIntc_Out32((BaseAddress) + XIN_MER_OFFSET, 0)

/****************************************************************************/
/**
*
* Enable specific interrupt(s) in the interrupt controller.
*
* @param    BaseAddress is the base address of the device
* @param    EnableMask is the 32-bit value to write to the enable register.
*           Each bit of the mask corresponds to an interrupt input signal that
*           is connected to the interrupt controller (INT0 = LSB). Only the
*           bits which are set in the mask will enable interrupts.
*
* @return   None.
*
* @note
*
* Signature: void XIntc_mEnableIntr(Xuint32 BaseAddress, Xuint32 EnableMask)
*
*****************************************************************************/
#define XIntc_mEnableIntr(BaseAddress, EnableMask) \
    XIntc_Out32((BaseAddress) + XIN_IER_OFFSET, (EnableMask))

/****************************************************************************/
/**
*
* Disable specific interrupt(s) in the interrupt controller.
*
* @param    BaseAddress is the base address of the device
* @param    DisableMask is the 32-bit value to write to the enable register.
*           Each bit of the mask corresponds to an interrupt input signal that
*           is connected to the interrupt controller (INT0 = LSB).  Only the
*           bits which are set in the mask will disable interrupts.
*
* @return   None.
*
* @note
*
* Signature: void XIntc_mDisableIntr(Xuint32 BaseAddress, Xuint32 DisableMask)
*
*****************************************************************************/
#define XIntc_mDisableIntr(BaseAddress, DisableMask) \
    XIntc_Out32((BaseAddress) + XIN_IER_OFFSET, ~(DisableMask))

/****************************************************************************/
/**
*
* Acknowledge specific interrupt(s) in the interrupt controller.
*
* @param    BaseAddress is the base address of the device
* @param    AckMask is the 32-bit value to write to the acknowledge register.
*           Each bit of the mask corresponds to an interrupt input signal that
*           is connected to the interrupt controller (INT0 = LSB).  Only the
*           bits which are set in the mask will acknowledge interrupts.
*
* @return   None.
*
* @note
*
* Signature: void XIntc_mAckIntr(Xuint32 BaseAddress, Xuint32 AckMask)
*
*****************************************************************************/
#define XIntc_mAckIntr(BaseAddress, AckMask) \
    XIntc_Out32((BaseAddress) + XIN_IAR_OFFSET, (AckMask))

/****************************************************************************/
/**
*
* Get the interrupt status from the interrupt controller which indicates
* which interrupts are active and enabled.
*
* @param    BaseAddress is the base address of the device
*
* @return   The 32-bit contents of the interrupt status register. Each bit
*           corresponds to an interrupt input signal that is connected to the
*           interrupt controller (INT0 = LSB). Bits which are set indicate an
*           active interrupt which is also enabled.
*
* @note
*
* Signature: Xuint32 XIntc_mGetIntrStatus(Xuint32 BaseAddress)
*
*****************************************************************************/
#define XIntc_mGetIntrStatus(BaseAddress) \
    (XIntc_In32((BaseAddress) + XIN_ISR_OFFSET) & \
     XIntc_In32((BaseAddress) + XIN_IER_OFFSET))

/************************** Function Prototypes ******************************/

/*
 * Interrupt controller handlers, to be connected to processor exception
 * handling code.
 */
void XIntc_LowLevelInterruptHandler(void);
void XIntc_DeviceInterruptHandler(void *DeviceId);

/* Various configuration functions */
void XIntc_SetIntrSvcOption(Xuint32 BaseAddress, int Option);

void XIntc_RegisterHandler(Xuint32 BaseAddress, int InterruptId,
                           XInterruptHandler Handler, void *CallBackRef);

/************************** Variable Definitions *****************************/

#endif            /* end of protection macro */


