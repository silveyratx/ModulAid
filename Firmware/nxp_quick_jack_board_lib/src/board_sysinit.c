/*
 * @brief NXP Quick_Jack board (LPC812) Sysinit file
 *
 * @note
 * Copyright(C) NXP Semiconductors, 2013
 * All rights reserved.
 *
 * @par
 * Software that is described herein is for illustrative purposes only
 * which provides customers with programming information regarding the
 * LPC products.  This software is supplied "AS IS" without any warranties of
 * any kind, and NXP Semiconductors and its licensor disclaim any and
 * all warranties, express or implied, including all implied warranties of
 * merchantability, fitness for a particular purpose and non-infringement of
 * intellectual property rights.  NXP Semiconductors assumes no responsibility
 * or liability for the use of the software, conveys no license or rights under any
 * patent, copyright, mask work right, or any other intellectual property rights in
 * or to any products. NXP Semiconductors reserves the right to make changes
 * in the software without notification. NXP Semiconductors also makes no
 * representation or warranty that such application will be suitable for the
 * specified use without further testing or modification.
 *
 * @par
 * Permission to use, copy, modify, and distribute this software and its
 * documentation is hereby granted, under NXP Semiconductors' and its
 * licensor's relevant copyrights in the software, without fee, provided that it
 * is used in conjunction with NXP Semiconductors microcontrollers.  This
 * copyright, permission, and disclaimer notice must appear in all copies of
 * this code.
 */

 #include "board.h"

/* The System initialization code is called prior to the application and
   initializes the board for run-time operation. Board initialization
   for the NXP Quick_Jack board board includes default pin muxing and clock
   setup configuration. */

/*****************************************************************************
 * Private types/enumerations/variables
 ****************************************************************************/

/*****************************************************************************
 * Public types/enumerations/variables
 ****************************************************************************/

/*****************************************************************************
 * Private functions
 ****************************************************************************/

/* Sets up system pin muxing */
STATIC void SystemSetupMuxing(void)
{
#if (LPC8XX_USE_XTAL_OSC == 1)
	/* Use Switch Matrix Tool swm.c file for the Pin Enable 0 variable */
	LPC_SWM->PINENABLE0 = 0xffffff82UL;	/* XTALIN + XTALOUT + CLKOUT on PIO0_1 + ACMP_I1 */

	/* Configure the pins for XTALIN/XTALOUT. */
	LPC_IOCON->PIO0[IOCON_PIO8] &= ~(3 << 3);
	LPC_IOCON->PIO0[IOCON_PIO9] &= ~(3 << 3);

#else
	/* Use Switch Matrix Tool swm.c file for the Pin Enable 0 variable */
	LPC_SWM->PINENABLE0 = 0xffffffb2UL;		/* IRC + CLKOUT on PIO0_1 + ACMP_I1 */
#endif /* (LPC8XX_USE_XTAL_OSC == 1) */
}

/*****************************************************************************
 * Public functions
 ****************************************************************************/

/* Set up and initialize hardware prior to call to main */
void Board_SystemInit(void)
{
	/* System clock to the GPIO & the SWM & the IOCON need to be enabled or
  most of the I/O related peripherals won't work. */
	Chip_Clock_EnablePeriphClock(SYSCTL_CLOCK_SWM);
	Chip_Clock_EnablePeriphClock(SYSCTL_CLOCK_GPIO);
	Chip_Clock_EnablePeriphClock(SYSCTL_CLOCK_IOCON);

	/* Setup system clocking and muxing */
	SystemSetupMuxing();
}
