/*
 * @brief Common types used in NXP Quick_Jack Manchester functions
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

#ifndef __NXP_QUICK_JACK_MANCHESTER_H_
#define __NXP_QUICK_JACK_MANCHESTER_H_

/** @defgroup QUICKJACK MANCHESTER FUNCTION: NXP Quick_Jack manchester initialize, decode & encode function
 * @ingroup QUICKJACK
 * @{
 */
 
/**
 * @brief NXP Quick_Jack manchester function - NXP Quick_Jack manchester hardware function
 */

/*!< NXP Quick_Jack audio clock definition */
#define QUICKJACKAUDIOCLK										(44100)
/*!< NXP Quick_Jack communication clock definition = HjAudioClk/32 = 1378.125Hz = 1378Hz */
#define QUICKJACKCOMMUNICATIONCLK			  		(QUICKJACKAUDIOCLK >> 0x5)
/*!< NXP Quick_Jack receive clock sample definition */
#define QUICKJACKRXCLKSAMPLEBITS			  			(32)
/*!< NXP Quick_Jack receive clcok timing min value */
#define QUICKJACKRXCOUNTERMIN			  				(QUICKJACKRXCLKSAMPLEBITS/2 + QUICKJACKRXCLKSAMPLEBITS/4)
/*!< NXP Quick_Jack receive clcok timing max value */
#define QUICKJACKRXCOUNTERMAX			  				(QUICKJACKRXCLKSAMPLEBITS + QUICKJACKRXCLKSAMPLEBITS/2)

/* NXP Quick_Jack decode & encode states. */
typedef enum
{
	/*!< NXP Quick_Jack status */
  QUICKJACK_STARTBIT	= 0U,			/*!< NXP Quick_Jack start bit status */
  QUICKJACK_STARTBIT_FALL,			/*!< NXP Quick_Jack detect falling edge status */
  QUICKJACK_DECODE,							/*!< NXP Quick_Jack decode status */
  QUICKJACK_STOPBIT,						/*!< NXP Quick_Jack stop bit status */
  QUICKJACK_BYTE,								/*!< NXP Quick_Jack send byte status */
  QUICKJACK_IDLE,								/*!< NXP Quick_Jack idle status */
	QUICKJACK_STARTBIT0,					/*!< NXP Quick_Jack start bit 0 status */
	QUICKJACK_STARTBIT1,					/*!< NXP Quick_Jack start bit 1 status */
	QUICKJACK_STARTBIT2,					/*!< NXP Quick_Jack start bit 2 status */
	QUICKJACK_SENDBIT,						/*!< NXP Quick_Jack send bit status */
} QUICKJACK_STATE_T;			

/**
 * @brief NXP Quick_Jack public parameter
 */
/*!< NXP Quick_Jack PhoneData: data from Mobile Phone to MCU */
extern volatile uint8_t	PhoneData;	

/*!< NXP Quick_Jack PhoneData: data from Mobile Phone to MCU */
extern volatile uint8_t	DataRecvFlag;			// 1 -- new data arrived
/*!< NXP Quick_Jack Receive timeing counter */
extern volatile uint32_t QuickJackRxCounter;

/**
 * @brief NXP Quick_Jack Manchester initialize function
 */
void QuickJack_Manchester_Init(void);

/**
 * @brief NXP Quick_Jack Manchester decode function
 */
void QuickJack_Manchester_Decode(uint32_t DataTime);

/**
 * @brief NXP Quick_Jack Manchester encode function
 */
void QuickJack_Manchester_Encode(void);

/**
 * @brief MCU send data to Mobile phone function
 */
uint8_t	QuickJack_Data_Tx(uint8_t Data);

/**
 * @}
 */

#endif /* __NXP_QUICK_JACK_MANCHESTER_H_ */
