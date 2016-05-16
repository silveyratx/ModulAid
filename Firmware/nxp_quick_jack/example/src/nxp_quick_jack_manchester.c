/*
 * @brief Manchester code/encode function for NXP Quick-Jack solution
 *
 * @par
 * Copyright (c) 2010 The Regents of the University of Michigan, NXP Semiconductors 2014
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * - Redistributions of source code must retain the above copyright
 *  notice, this list of conditions and the following disclaimer.
 * - Redistributions in binary form must reproduce the above copyright
 *  notice, this list of conditions and the following disclaimer in the
 *  documentation and/or other materials provided with the
 *  distribution.
 * - Neither the name of the copyright holder nor the names of
 *  its contributors may be used to endorse or promote products derived
 *  from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL
 * THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Original author: Thomas Schmid.
 * Modified by NXP Semiconductors (2014).
 */

#include "board.h"
#include "nxp_quick_jack_manchester.h" 

/*****************************************************************************
 * Private types/enumerations/variables
 ****************************************************************************/
static volatile QUICKJACK_STATE_T	TxState			= QUICKJACK_IDLE;								// transmit status
static volatile QUICKJACK_STATE_T	LastState		= QUICKJACK_IDLE;								// last transmit status

static volatile uint8_t				TxBit;													// transfer bits
static volatile uint8_t				SendByteTx;												// the data tranfer to Mobile
static volatile uint8_t				IdleBit;													// how many times of transfer idle status

/*****************************************************************************
 * Public types/enumerations/variables
 ****************************************************************************/
volatile uint8_t			PhoneData;
volatile uint8_t			DataRecvFlag;
volatile uint32_t			QuickJackRxCounter;

/*****************************************************************************
 * Private functions
 ****************************************************************************/
static uint8_t Data_ValidCheck(uint8_t Data)
{
	uint8_t i, ret;
	ret = 0;
	for(i=0; i<8; i++) {
		ret = ret +((Data >> i)&0x01);
	}
	ret = ret&0x01;
	return ret;
}

/*****************************************************************************
 * Public functions
 ****************************************************************************/
/* Transmit a byte over the NXP Quick_Jack interface. */
uint8_t QuickJack_Data_Tx(uint8_t Data)
{
	uint8_t i;

	while(IdleBit<3) {							/* waiting the transfer in IDLE mode */
		for(i=0; i<10; i++);
	}
	
	while(TxState	!= LastState) {
          i = i;
	}
	
	/* Interrupt disable */
	__disable_irq();
	
	SendByteTx	= Data;
	TxState			= QUICKJACK_STARTBIT;
	IdleBit			= 0;
	TxBit				= 0;
	
	/* Interrupt enable */
	__enable_irq();
	return 1;
}

/* Encode the data and send the data to Mobile . */
void QuickJack_Manchester_Encode(void)
{
	static uint8_t CurrentPin	= 1;													// recode transfer pin status
	static uint8_t TxParity		= 0;													// transfer data parity bit count
	uint8_t TempByte, tmp;

	switch(TxState) {
		case QUICKJACK_STARTBIT:																		/* Start Bit process */
			CurrentPin		= 0;
			TxBit					= 0;
			TxParity	    = 0;
			IdleBit				= 0;
			TxState		    = QUICKJACK_SENDBIT;
			LastState			= QUICKJACK_BYTE;

			Chip_GPIO_SetValue(LPC_GPIO_PORT, 0, (1 << QUICKJACKTXPINNUM));																			/* Pull-up tx pin */
		break;
		
		case QUICKJACK_BYTE:																				/* Transfer data process */
			if(TxBit < 8) {
				TempByte    = SendByteTx;
				CurrentPin  = (TempByte >> TxBit) & 0x01;
				TxBit++;
				tmp         = TxParity;
				TxParity    = tmp + CurrentPin;
				LastState   = QUICKJACK_BYTE;
			}
			else if(TxBit == 8) {
				CurrentPin  = TxParity & 0x01;
				TxBit++;
				LastState   = QUICKJACK_BYTE;
			}
			else if(TxBit > 8) {
				CurrentPin  = 1;
				LastState   = QUICKJACK_IDLE;
			}
			TxState			= QUICKJACK_SENDBIT;												
			
			if(CurrentPin == 1) Chip_GPIO_ClearValue(LPC_GPIO_PORT, 0, (1 << QUICKJACKTXPINNUM));
			else								Chip_GPIO_SetValue(LPC_GPIO_PORT, 0, (1 << QUICKJACKTXPINNUM));
		break;
			
		case QUICKJACK_IDLE:																					/* Transfer in IDLE status */
			CurrentPin = 1;
			if(CurrentPin == 1) Chip_GPIO_ClearValue(LPC_GPIO_PORT, 0, (1 << QUICKJACKTXPINNUM));
			else								Chip_GPIO_SetValue(LPC_GPIO_PORT, 0, (1 << QUICKJACKTXPINNUM));
			IdleBit++;
			if(IdleBit >= 3) {
				IdleBit = 3;
			}
			TxState			= QUICKJACK_SENDBIT;
			LastState		= QUICKJACK_IDLE;
		break;
		case QUICKJACK_SENDBIT:																			/* Transfer a bit process */
			if(CurrentPin == 1) Chip_GPIO_SetValue(LPC_GPIO_PORT, 0, (1 << QUICKJACKTXPINNUM));
			else								Chip_GPIO_ClearValue(LPC_GPIO_PORT, 0, (1 << QUICKJACKTXPINNUM));

			TxState		= LastState;
		break;
		
		default:
			
		break;
	}
}

/* Received the data from Mobile and decode it. */
void QuickJack_Manchester_Decode(uint32_t DataTime)
{
	static QUICKJACK_STATE_T DecodeState = QUICKJACK_STARTBIT_FALL;						// decode status
	static uint32_t BitCounter	= 0;													// receive data bit count from Mobile
	static uint32_t ones				= 0;														
	static uint32_t LastTime		= 0;													// capture timing data
	static uint8_t  ByteRx		= 0;													// the data received from Mobile
	
	uint32_t diff;
	uint8_t  flag;

	/* Read Rx pin status */
	flag = Chip_ACMP_GetCompStatus(LPC_CMP) ? 1 : 0;
	
	/* Caculate the captured timing */
	if(DataTime > LastTime)
		diff = DataTime - LastTime;
	else
		diff = 0xFFFFFFFF - LastTime + DataTime;
	
	switch(DecodeState) {
	case QUICKJACK_STARTBIT:
		Chip_PININT_EnableIntLow(LPC_PININT, (1<<PININT_RXPIN_CHAN_NUM));	
		Chip_PININT_DisableIntHigh(LPC_PININT, (1<<PININT_RXPIN_CHAN_NUM));
		Chip_ACMP_SetEdgeSelection(LPC_CMP, ACMP_EDGESEL_FALLING);
		DecodeState = QUICKJACK_STARTBIT_FALL;
		break;
	case QUICKJACK_STARTBIT_FALL:
		if ((QUICKJACKRXCOUNTERMIN < diff) && (diff < QUICKJACKRXCOUNTERMAX))	{
			if(ones < 2) {
				/* we didn't have enough ones */
				ones = 0;
				/* Cofig Capture Rising Edge Mode */
				Chip_PININT_EnableIntHigh(LPC_PININT, (1<<PININT_RXPIN_CHAN_NUM));	
				Chip_PININT_DisableIntLow(LPC_PININT, (1<<PININT_RXPIN_CHAN_NUM));
				Chip_ACMP_SetEdgeSelection(LPC_CMP, ACMP_EDGESEL_RISING);
				DecodeState = QUICKJACK_STARTBIT;
			}
			else {
				/* Config Capture as both Edge Mode */
				Chip_PININT_EnableIntLow(LPC_PININT, (1<<PININT_RXPIN_CHAN_NUM));	
				Chip_PININT_EnableIntHigh(LPC_PININT, (1<<PININT_RXPIN_CHAN_NUM));
				Chip_ACMP_SetEdgeSelection(LPC_CMP, ACMP_EDGESEL_BOTH);
        BitCounter = 0;
        ByteRx = 0;
				DecodeState = QUICKJACK_DECODE;
			}
		}
		else {
			/* no, we have to search again */
			/* Config Capture as Rising Edge Mode */
			Chip_PININT_EnableIntHigh(LPC_PININT, (1<<PININT_RXPIN_CHAN_NUM));	
			Chip_PININT_DisableIntLow(LPC_PININT, (1<<PININT_RXPIN_CHAN_NUM));
			Chip_ACMP_SetEdgeSelection(LPC_CMP, ACMP_EDGESEL_RISING);
			DecodeState = QUICKJACK_STARTBIT;
			if(diff < QUICKJACKRXCOUNTERMIN) {
        /* Count the number of shorts for robustness. */
        ones++;
      }
		}
		break;
	case QUICKJACK_DECODE:
		if ((QUICKJACKRXCOUNTERMIN < diff) && (diff < QUICKJACKRXCOUNTERMAX))	{
			if (BitCounter >= 8) {
				/* We got the whole byte, output stop bit and search for startbit. */
				Chip_PININT_EnableIntHigh(LPC_PININT, (1<<PININT_RXPIN_CHAN_NUM));	
				Chip_PININT_DisableIntLow(LPC_PININT, (1<<PININT_RXPIN_CHAN_NUM));
				Chip_ACMP_SetEdgeSelection(LPC_CMP, ACMP_EDGESEL_RISING);
				DecodeState = QUICKJACK_STARTBIT;
				
				if(Data_ValidCheck(ByteRx) == flag) {
					PhoneData = ByteRx;
					DataRecvFlag = 1;
				}
				if(PhoneData == 0) {
					ones = 3;
				}
				ones = 0;
				return;
			}
			/* Check what transition it was. */
      if (flag) {
        /* We read a 1. */
        ByteRx = (ByteRx >> 1) + (1 << 7);
      }
      else {
        /* We got a 0. */
        ByteRx = (ByteRx >> 1);
      }

      BitCounter++;
    }
    else if (diff >= QUICKJACKRXCOUNTERMAX) {
      /* Something is wrong, start search again. */
			Chip_PININT_EnableIntHigh(LPC_PININT, (1<<PININT_RXPIN_CHAN_NUM));	
			Chip_PININT_DisableIntLow(LPC_PININT, (1<<PININT_RXPIN_CHAN_NUM));
			Chip_ACMP_SetEdgeSelection(LPC_CMP, ACMP_EDGESEL_RISING);
      DecodeState = QUICKJACK_STARTBIT;
      ones = 0;
    }
    else {
      /* Return here and don't update the time! */
      return;
    }
		break;
	default:
		break;
	}
	LastTime = DataTime;
}

/**
 * @}
 */
