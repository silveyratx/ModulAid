/*
 * @brief NXP_Quick_Jack board (LPC812) board file
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

#ifndef __BOARD_H_
#define __BOARD_H_

#include "chip.h"
/* board_api.h is included at the bottom of this file after DEBUG setup */

#ifdef __cplusplus
extern "C" {
#endif



/***** Sam's Additions ******************/
#define LEDHEARTBEAT 13

#define DUALSWTCHINPUT	9
#define DUALSWTCHNC1	10
#define DUALSWTCHNC2	11
#define THIRDSENSORPIN	12
#define THERMSCL DUALSWTCHNC1
#define THERMSDA DUALSWTCHNC2

// MLX90614 constants
#define MLX90614_DEFAULT_ADDRESS_W	0xB4
#define MLX90614_DEFAULT_ADDRESS_R	0xB5
#define MLX90614_REGISTER_TA		0x06
#define MLX90614_REGISTER_TOBJ1		0x07

void SensorSwitch_ConnectToUc(void);
void SensorSwitch_ConnectToADC(void);
void IrTherm_Init(void);
int32_t IrTherm_Read(void);
uint32_t SPI_ReadCh0(void);
bool Pulse_IsNewData(void);
uint16_t Pulse_GetBPM(void);
void PulseInterrupt(void);
/*****************************************/

/** @defgroup BOARD_LPC812QUICKJACK (LPC812) board support software API functions
 * @ingroup LPCOPEN_8XX_LPC812 NXP_Quick_Jack
 * The board support software API functions provide some simple abstracted
 * functions used across multiple LPCOpen board examples. See @ref BOARD_COMMON_API
 * for the functions defined by this board support layer.<br>
 * @{
 */

/** @defgroup BOARD_LPC812QUICKJACK OPTIONS BOARD: NXP_Quick_Jack board (LPC812) board build options
 * This board has options that configure its operation at build-time.<br>
 * @{
 */

/* 1Mbps I2C bit-rate */
//#define I2C_BITRATE											(400000)
#define I2C_BITRATE											(100000)

/* For more info, read NXP's Se98 datasheet */
#define SE98_ADDR												0x38
#define SE98_ID													0x06
#define SE98_TEMP												0x05

/**
 * @brief NXP Quick-Jack Application function - NXP Quick-Jack application hardware
 */
/*!< LED numbers declaration */
#define LED1Num														0
#define LED2Num														1
#define LED3Num														2
#define LED4Num														3

//Hi-jack APP func
//JoyStick Func
#define LED1_PINNUM											8
#define LED2_PINNUM											9
#define LED3_PINNUM											12
#define LED4_PINNUM											13

#define K_Up_PINNUM											11
#define K_Down_PINNUM										17
#define K_Left_PINNUM										16
#define K_Right_PINNUM									4
#define K_Press_PINNUM									10


/*!< PIO0_14 The Pin for MCU transfer data to Mobile */
#define QUICKJACKTXPIN										IOCON_PIO14
#define QUICKJACKTXPINNUM									14
/*!< PIO0_0  The Pin for MCU receive data from Mobile */
#define QUICKJACKRXPIN										IOCON_PIO0
/*!< PIO0_6  The pin for MIC and GND comparative result */
#define MnGACMPOUTPIN											IOCON_PIO6 
#define MnGACMPOUTPINNUM									6																										
/*!< PIO0_7  The pin for Analog switch selection */
/*!< (output low 3: MIC, 4: GND; output low 3: GND, 4: MIC) */
#define MnGSWSELPIN												IOCON_PIO7	
#define MnGSWSELPINNUM										7	
/*!< PIO0_15 The pin for Analog switch enabled */
/*!< (output low enabled the SW)*/
#define MnGSWENAPIN												IOCON_PIO15 
#define MnGSWENAPINNUM										15	
/*!< PININT channel number of Manchester RX pin */
#define PININT_RXPIN_CHAN_NUM							0

/**
 * @}
 */

/* Board name */
#define BOARD_LPC812QUICKJACK

/**
 * @}
 */

#include "board_api.h"

#ifdef __cplusplus
}
#endif

#endif /* __BOARD_H_ */



