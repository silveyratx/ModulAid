/*
 * @brief Main function for NXP Quick_Jack Solution
 *
 * @note
 * Copyright(C) NXP Semiconductors, 2014
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
 #include "nxp_quick_jack_manchester.h"

/*****************************************************************************
 * Private types/enumerations/variables
 ****************************************************************************/

/*****************************************************************************
 * Public types/enumerations/variables
 ****************************************************************************/

/*****************************************************************************
 * Private functions
 ****************************************************************************/

/**
 * @brief	Initializes Quick Jack board for command received from phone
 * @return	Updated *data and *proc parameters
 */
static void ProcessPhoneData(uint8_t *data)
{
	uint16_t temp;
	uint16_t bpm;
	uint8_t command;

	if (*data == 0x31 || *data == 0x32 || *data == 0x47 || *data == 0x72){
		command = *data;
	}

	switch(command){
		case 0x99:
			*data = 0;	
			NVIC_SystemReset();
			break;
		case 0x31:
			//QuickJack_Data_Tx(0xAA);								/* HandShake Protocol AA 55 AA 55 */
			//QuickJack_Data_Tx(0x55);
			//QuickJack_Data_Tx(0xAA);
			//QuickJack_Data_Tx(0x55);

			//Board_LED_Set(LEDHEARTBEAT, false);

			SensorSwitch_ConnectToADC();
			/* start systick for measuring pulse */
			SysTick_Config(24000);

			command = 0x00;
			*data = 0x00;
			break;
		case 0x32:
			//QuickJack_Data_Tx(0xAA);								/* HandShake Protocal AA 55 AA 55*/
			//QuickJack_Data_Tx(0x55);
			//QuickJack_Data_Tx(0xAA);
			//QuickJack_Data_Tx(0x55);

			//Board_LED_Set(LEDHEARTBEAT, true);

			/* disable systick */
			SysTick->CTRL  = 0;

			SensorSwitch_ConnectToUc();
			IrTherm_Init();

			command = 0;
			*data = 0x00;

			break;
		case 0x47:
			//if (Pulse_IsNewData()){
				bpm = Pulse_GetBPM();
				QuickJack_Data_Tx(0xff);							/* Send data header 0xFF 0xFF */
				QuickJack_Data_Tx(0xff);
				QuickJack_Data_Tx((bpm>>8)&0xff);					/* Send pulese value to Mobile phone */
				QuickJack_Data_Tx(bpm&0xff);

			//}
			break;
		case 0x72:
			temp = IrTherm_Read();
			if (temp > 0x7fff || temp < 0x27ad){
				temp = 0xffff;
			}
			//temp = 15000;
			QuickJack_Data_Tx(0xff);								/* Send data header 0xFF 0xFF */
			QuickJack_Data_Tx(0xff);
			QuickJack_Data_Tx((temp>>8)&0xff);						/* Send temperature value to Mobile phone */
			QuickJack_Data_Tx(temp&0xff);

			break;
		default:
			break;
	}
}

/*****************************************************************************
 * Public functions
 ****************************************************************************/

/* when activated measures pulse */
void SysTick_Handler(void){
	PulseInterrupt();
}

/**
 * @brief	MRT IRQ handler
 * @return	None
 */
void MRT_IRQHandler(void)
{  

	uint32_t int_pend;
	/* Get interrupt pending status for all timers */
	int_pend = Chip_MRT_GetIntPending();
	/* Channel 0 is periodic, toggle on either interrupt */
	if (int_pend & MRT0_INTFLAG) {
		QuickJackRxCounter++;
	}	
	/* Channel 1 is periodic, toggle on either interrupt */
	if (int_pend & MRT1_INTFLAG) {
		QuickJack_Manchester_Encode();
	}	
	/* Channel 2 is periodic, toggle on either interrupt */
	if (int_pend & MRT2_INTFLAG) {
		Board_LED_Toggle(LEDHEARTBEAT);
	}
	/* clear interrupt flag */
	Chip_MRT_ClearIntPending(int_pend);
}

/**
 * @brief	Analog comparator IRQ handler
 * @return	Function should not exit
 */
void CMP_IRQHandler(void)
{
	Chip_ACMP_EdgeClear(LPC_CMP);
	QuickJack_Manchester_Decode(QuickJackRxCounter);
}

/**
 * @brief	Main routine for Quick Jack
 * @return	Function should not exit
 */
int main(void)
{
	
	SystemCoreClockUpdate();
	Board_Init();
	
	/* Init timers */
	Chip_MRT_Init();
	
	/* Set MRT timing parameter */
	Chip_MRT_SetInterval(LPC_MRT_CH0, ((SystemCoreClock / QUICKJACKCOMMUNICATIONCLK) / QUICKJACKRXCLKSAMPLEBITS));
	Chip_MRT_SetInterval(LPC_MRT_CH1, ((SystemCoreClock / QUICKJACKCOMMUNICATIONCLK) >> 1));
	//Chip_MRT_SetInterval(LPC_MRT_CH2, ((SystemCoreClock >> 9)* 109));
	//Chip_MRT_SetInterval(LPC_MRT_CH2, 48000);
	
	/* Enable MRT Channel 0, 1 and 2 */
	Chip_MRT_SetEnabled(LPC_MRT_CH0);
	Chip_MRT_SetEnabled(LPC_MRT_CH1);
	//Chip_MRT_SetEnabled(LPC_MRT_CH2);
	NVIC_EnableIRQ(MRT_IRQn);

	while(1) {
		ProcessPhoneData((uint8_t *)&PhoneData);
		//__WFI();
	}
}
