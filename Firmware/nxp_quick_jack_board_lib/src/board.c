/*
 * @brief NXP Quick-Jack board (LPC812) board file
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
#include "retarget.h"


/***** Sam's Additions ******************/

void SensorSwitch_Init(void){
	Chip_SWM_DisableFixedPin(SWM_FIXED_XTALOUT);
	Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT, 0, DUALSWTCHINPUT);
}

void SensorSwitch_ConnectToUc(void){
	Chip_GPIO_SetPinState(LPC_GPIO_PORT, 0, DUALSWTCHINPUT, false);
}

void SensorSwitch_ConnectToADC(void){
	Chip_GPIO_SetPinState(LPC_GPIO_PORT, 0, DUALSWTCHINPUT, true);
}

/* I2C master handle and memory for ROM API */
static I2C_HANDLE_T *i2cHandleMaster;

/* Use a buffer size larger than the expected return value of
   i2c_get_mem_size() for the static I2C handle type */
static uint32_t i2cMasterHandleMEM[0x20];

int32_t IrTherm_Read(void){
	uint16_t rawTemp;
	float fTemp;
	uint8_t recvData[4], sendData[2];
	I2C_PARAM_T param;
	I2C_RESULT_T result;
	ErrorCode_t error;

	/* Enable clocks to SWM and IOCON to save power */
	//Chip_Clock_EnablePeriphClock(SYSCTL_CLOCK_SWM);
	//Chip_Clock_EnablePeriphClock(SYSCTL_CLOCK_IOCON);

	sendData[0] = MLX90614_DEFAULT_ADDRESS_W;
	sendData[1] = MLX90614_REGISTER_TOBJ1;
	recvData[0] = MLX90614_DEFAULT_ADDRESS_R;

	/* Setup parameters for transfer */
	param.num_bytes_send    = 2;
	param.num_bytes_rec     = 4;
	param.buffer_ptr_send   = &sendData[0];
	param.buffer_ptr_rec    = &recvData[0];
	param.stop_flag         = 1;
	error = LPC_I2CD_API->i2c_master_tx_rx_poll(i2cHandleMaster, &param, &result);
	rawTemp = ((recvData[2] << 8) & 0xff00) | ((recvData[1] & 0xff));

	fTemp = (float)rawTemp * 0.02;
	fTemp -= 273.15;
	fTemp = fTemp * 9.0 / 5.0 + 32;


	/* Disable clocks to SWM and IOCON to save power */
	//Chip_Clock_DisablePeriphClock(SYSCTL_CLOCK_SWM);
	//Chip_Clock_DisablePeriphClock(SYSCTL_CLOCK_IOCON);

	return rawTemp;
}

void IrTherm_Init(void){

	/* Enable clocks to SWM and IOCON to save power */
	Chip_Clock_EnablePeriphClock(SYSCTL_CLOCK_SWM);
	Chip_Clock_EnablePeriphClock(SYSCTL_CLOCK_IOCON);

	Chip_SWM_MovablePinAssign(SWM_I2C_SDA_IO, 11);
	Chip_SWM_MovablePinAssign(SWM_I2C_SCL_IO, 10);

#if (I2C_BITRATE > 400000)
	/* Enable Fast Mode Plus for I2C pins */
	Chip_IOCON_PinSetI2CMode(LPC_IOCON, IOCON_PIO10, PIN_I2CMODE_FASTPLUS);
	Chip_IOCON_PinSetI2CMode(LPC_IOCON, IOCON_PIO11, PIN_I2CMODE_FASTPLUS);
#else
	Chip_IOCON_PinSetI2CMode(LPC_IOCON, IOCON_PIO10, PIN_I2CMODE_STDFAST);
	Chip_IOCON_PinSetI2CMode(LPC_IOCON, IOCON_PIO11, PIN_I2CMODE_STDFAST);
#endif

	/* Enable I2C clock and reset I2C peripheral - the boot ROM does not do this */
	Chip_I2C_Init();

	/* Setup the I2C handle */
	i2cHandleMaster = LPC_I2CD_API->i2c_setup(LPC_I2C_BASE, i2cMasterHandleMEM);

	/* Set I2C bitrate */
	LPC_I2CD_API->i2c_set_bitrate(i2cHandleMaster, Chip_Clock_GetSystemClockRate(), I2C_BITRATE);

	/* Disable the interrupt for the I2C */
	NVIC_DisableIRQ(I2C_IRQn);

	/* Disable clocks to SWM and IOCON to save power */
	Chip_Clock_DisablePeriphClock(SYSCTL_CLOCK_SWM);
	Chip_Clock_DisablePeriphClock(SYSCTL_CLOCK_IOCON);
}

#define BUFFER_SIZE         2
/* Tx buffer */
static uint16_t TxBuf[BUFFER_SIZE];

/* Rx buffer */
static uint16_t RxBuf[BUFFER_SIZE];

static SPI_CONFIG_T ConfigStruct;
static SPI_DELAY_CONFIG_T DelayConfigStruct;
static SPI_DATA_SETUP_T XfSetup;
static volatile uint8_t  isXferCompleted = 0;

uint32_t SPI_ReadCh0(void){

	/* Disable clocks to SWM and IOCON to save power */
	Chip_Clock_EnablePeriphClock(SYSCTL_CLOCK_SWM);
	Chip_Clock_EnablePeriphClock(SYSCTL_CLOCK_IOCON);

	// initialize pins
	Chip_SWM_DisableFixedPin(SWM_FIXED_XTALIN);
	Chip_SWM_MovablePinAssign(SWM_SPI1_SCK_IO, 8);
	Chip_SWM_MovablePinAssign(SWM_SPI1_MOSI_IO, 17);
	Chip_SWM_MovablePinAssign(SWM_SPI1_MISO_IO, 16);
	//TODO: do we need to set slave select if it's tied to ground?
	Chip_SWM_MovablePinAssign(SWM_SPI1_SSEL_IO, 4);


	ConfigStruct.Mode = SPI_MODE_MASTER;
	ConfigStruct.ClkDiv = Chip_SPI_CalClkRateDivider(LPC_SPI1, 100000);
	ConfigStruct.ClockMode = SPI_CLOCK_CPHA0_CPOL0;
	ConfigStruct.DataOrder = SPI_DATA_MSB_FIRST;		//enable MSB first option
	ConfigStruct.SSELPol = SPI_CFG_SPOL_LO;
	Chip_SPI_Init(LPC_SPI1, &ConfigStruct);


	DelayConfigStruct.FrameDelay = 0;
	DelayConfigStruct.PostDelay = 0;
	DelayConfigStruct.PreDelay = 0;
	DelayConfigStruct.TransferDelay = 0;
	Chip_SPI_DelayConfig(LPC_SPI1, &DelayConfigStruct);

	Chip_SPI_Enable(LPC_SPI1);


	//Chip_SPI_EnableLoopBack(LPC_SPI1);

	// init transmit buffer
	TxBuf[0] = 0x68;
	TxBuf[1] = 0x00;

	XfSetup.Length = BUFFER_SIZE;
	XfSetup.pTx = TxBuf;
	XfSetup.RxCnt = XfSetup.TxCnt = 0;
	XfSetup.DataSize = 8;
	XfSetup.pRx = RxBuf;


	Chip_SPI_RWFrames_Blocking(LPC_SPI1, &XfSetup);

	uint16_t value = ((RxBuf[0] & 0x03) << 8) + (RxBuf[1] & 0xFF);


	//Chip_SPI_DisableLoopBack(LPC_SPI1);

	/* DeInitialize SPI peripheral */
	Chip_SPI_DeInit(LPC_SPI1);

	/* Disable clocks to SWM and IOCON to save power */
	Chip_Clock_DisablePeriphClock(SYSCTL_CLOCK_SWM);
	Chip_Clock_DisablePeriphClock(SYSCTL_CLOCK_IOCON);

	return value;
}


volatile uint16_t BPM;                   // int that holds raw Analog in 0. updated every 2mS
volatile int Signal;                // holds the incoming raw data
volatile int IBI = 600;             // int that holds the time interval between beats! Must be seeded!
volatile bool Pulse = false;     // "True" when User's live heartbeat is detected. "False" when not a "live beat".
volatile bool QS = false;        // becomes true when Arduoino finds a beat.

volatile int rate[10];                    // array to hold last ten IBI values
volatile unsigned long sampleCounter = 0;          // used to determine pulse timing
volatile unsigned long lastBeatTime = 0;           // used to find IBI
volatile int P =512;                      // used to find peak in pulse wave, seeded
volatile int T = 512;                     // used to find trough in pulse wave, seeded
volatile int thresh = 525;                // used to find instant moment of heart beat, seeded
volatile int amp = 100;                   // used to hold amplitude of pulse waveform, seeded
volatile bool firstBeat = true;        // used to seed rate array so we startup with reasonable BPM
volatile bool secondBeat = false;      // used to seed rate array so we startup with reasonable BPM

void PulseInterrupt(void){
	int i;
	  //cli();                                      // disable interrupts while we do this
	  Signal = SPI_ReadCh0();              // read the Pulse Sensor
	  sampleCounter += 2;                         // keep track of the time in mS with this variable
	  int N = sampleCounter - lastBeatTime;       // monitor the time since the last beat to avoid noise

	    //  find the peak and trough of the pulse wave
	  if(Signal < thresh && N > (IBI/5)*3){       // avoid dichrotic noise by waiting 3/5 of last IBI
	    if (Signal < T){                        // T is the trough
	      T = Signal;                         // keep track of lowest point in pulse wave
	    }
	  }

	  if(Signal > thresh && Signal > P){          // thresh condition helps avoid noise
	    P = Signal;                             // P is the peak
	  }                                        // keep track of highest point in pulse wave

	  //  NOW IT'S TIME TO LOOK FOR THE HEART BEAT
	  // signal surges up in value every time there is a pulse
	  if (N > 250){                                   // avoid high frequency noise
	    if ( (Signal > thresh) && (Pulse == false) && (N > (IBI/5)*3) ){
	      Pulse = true;                               // set the Pulse flag when we think there is a pulse
	      Board_LED_Set(LEDHEARTBEAT, true);                // turn on pin 13 LED
	      IBI = sampleCounter - lastBeatTime;         // measure time between beats in mS
	      lastBeatTime = sampleCounter;               // keep track of time for next pulse

	      if(secondBeat){                        // if this is the second beat, if secondBeat == TRUE
	        secondBeat = false;                  // clear secondBeat flag
	        for(i=0; i<=9; i++){             // seed the running total to get a realisitic BPM at startup
	          rate[i] = IBI;
	        }
	      }

	      if(firstBeat){                         // if it's the first time we found a beat, if firstBeat == TRUE
	        firstBeat = false;                   // clear firstBeat flag
	        secondBeat = true;                   // set the second beat flag
	        //sei();                               // enable interrupts again
	        return;                              // IBI value is unreliable so discard it
	      }


	      // keep a running total of the last 10 IBI values
	      long runningTotal = 0;                  // clear the runningTotal variable

	      for(i=0; i<=8; i++){                // shift data in the rate array
	        rate[i] = rate[i+1];                  // and drop the oldest IBI value
	        runningTotal += rate[i];              // add up the 9 oldest IBI values
	      }

	      rate[9] = IBI;                          // add the latest IBI to the rate array
	      runningTotal += rate[9];                // add the latest IBI to runningTotal
	      runningTotal /= 10;                     // average the last 10 IBI values
	      BPM = 60000/runningTotal;               // how many beats can fit into a minute? that's BPM!
	      QS = true;                              // set Quantified Self flag
	      // QS FLAG IS NOT CLEARED INSIDE THIS ISR
	    }
	  }

	  if (Signal < thresh && Pulse == true){   // when the values are going down, the beat is over
		  Board_LED_Set(LEDHEARTBEAT, false);            // turn off pin 13 LED
	    Pulse = false;                         // reset the Pulse flag so we can do it again
	    amp = P - T;                           // get amplitude of the pulse wave
	    thresh = amp/2 + T;                    // set thresh at 50% of the amplitude
	    P = thresh;                            // reset these for next time
	    T = thresh;
	  }

	  if (N > 2500){                           // if 2.5 seconds go by without a beat
	    thresh = 512;                          // set thresh default
	    P = 512;                               // set P default
	    T = 512;                               // set T default
	    lastBeatTime = sampleCounter;          // bring the lastBeatTime up to date
	    firstBeat = true;                      // set these to avoid noise
	    secondBeat = false;                    // when we get the heartbeat back
	  }

	  //sei();   				// enable interrupts when youre done!
}

bool Pulse_IsNewData(void){
	return QS;
}

uint16_t Pulse_GetBPM(void){
	QS = false;
	return BPM;
}

/****************************************/

/* 
	J9A		FUNC2		FUNC1		PIN		I/O		MCUFUNC		MCUFUNC		I/O		PIN		FUNC1		FUNC2		J9B
	 A2 	S_RST	K_Up		  1			P17		----			----			P14		20		MIC			----		B2			HjTx
	 A3 	S_SCLK	K_Down	2			P13		----			ACMP0			P0		19		IN1+		L_IN		B3			HjRx
	 A4 	S_MOSI	K_Press	3			P12		ISP				VDDCMP		P6		18		P06			----		B4			MgAcmpOut
	 A5 	----		RESET		4			P5		RESET			----			P7		17		P07			----		B5			MgSwSel
	 A6 	S_MISO	K_Left	5			P4		WAKEUP		GND				VSS		16		GND			----		B0A0
CN1/4		----		SWCLK		6			P3		SWCLK			VCC				VDD		15		VCC			----		B1A1
CN1/2		----		SWDIO		7			P2		SWDIO			XTALIN		P8		14		XTALIN	LD3			B6
	 A7 	S_DOUT	LD1/SDA	8			P11		SDA				XTALOUT		P9		13		XTALOUT	LD4			B7
	 A8 	S_CSN		LD2/SCL	9			P10		SCL				ACMP1			P1		12		P01			----		B8			BatDet
	 A9 	S_SINT	K_Right	10		P16		----			----			P15		11		P15			----		B9			MgSwEna
*/

/*****************************************************************************
 * Private types/enumerations/variables
 ****************************************************************************/


/*****************************************************************************
 * Public types/enumerations/variables
 ****************************************************************************/
/* System oscillator rate and clock rate on the CLKIN pin */
const uint32_t OscRateIn = 0;
const uint32_t ExtRateIn = 0;

/*****************************************************************************
 * Private functions
 ****************************************************************************/

/* Initialize the LEDs on the NXP_Quick_Jack board */
void Board_LED_Init(void)
{
	uint32_t i;
	
	Chip_SWM_DisableFixedPin(SWM_FIXED_XTALIN);			//pin 8
	Chip_SWM_DisableFixedPin(SWM_FIXED_XTALOUT);		//pin 9

	//heart beat on pin 13
	Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT, 0, LEDHEARTBEAT);
	Chip_GPIO_SetPinState(LPC_GPIO_PORT, 0, LEDHEARTBEAT, true);

}

/*****************************************************************************
 * Public functions
 ****************************************************************************/

/* Set the LED to the state of "On" */
void Board_LED_Set(uint8_t pinNum, bool On)
{
	Chip_GPIO_SetPinState(LPC_GPIO_PORT, 0, pinNum, (bool) !On);
}

/* Toggles the current state of a board LED */
void Board_LED_Toggle(uint8_t pinNum)
{
	Chip_GPIO_SetPinToggle(LPC_GPIO_PORT, 0, pinNum);
}

/*****************************************************************************
 * Public functions
 ****************************************************************************/

/* NXP_Quick_Jack io initilize detect the mic&gnd */
void QUICKJACK_IO_Init(void)
{
	uint32_t MgAcmpOutVal;
	uint32_t i = 0;

	Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT, 0, MnGSWENAPINNUM);
	Chip_GPIO_SetPinState(LPC_GPIO_PORT, 0, MnGSWENAPINNUM, 1);

	Chip_IOCON_PinSetMode(LPC_IOCON, IOCON_PIO14, PIN_MODE_INACTIVE);
	Chip_GPIO_SetPinDIRInput(LPC_GPIO_PORT, 0, 14);
	
	Chip_IOCON_PinSetMode(LPC_IOCON, MnGACMPOUTPIN, PIN_MODE_INACTIVE);
	Chip_GPIO_SetPinDIRInput(LPC_GPIO_PORT, 0, MnGACMPOUTPINNUM);
	
	MgAcmpOutVal = Chip_GPIO_ReadPortBit(LPC_GPIO_PORT, 0, 6);
	
	for(i=0; i<5; i++) {																									/* waiting a while */
		__NOP();
	}
	
	while(MgAcmpOutVal!= Chip_GPIO_ReadPortBit(LPC_GPIO_PORT, 0, 6)) {
		for(i=0; i<5; i++) {																									/* waiting a while */
			__NOP();
		}
		MgAcmpOutVal = Chip_GPIO_ReadPortBit(LPC_GPIO_PORT, 0, 6);
		for(i=0; i<5; i++) {																									/* waiting a while */
			__NOP();
		}
	}
	
	/* configured analog switch selectable pin */
	Chip_IOCON_PinSetMode(LPC_IOCON, MnGSWSELPIN, PIN_MODE_INACTIVE);
	Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT, 0, MnGSWSELPINNUM);

	if(MgAcmpOutVal==0) {
		Chip_GPIO_SetPinState(LPC_GPIO_PORT, 0, MnGSWSELPINNUM, 1);
	}
	else {
		Chip_GPIO_SetPinState(LPC_GPIO_PORT, 0, MnGSWSELPINNUM, 0);
	}

	/* enabled analog switch enabled pin */
	Chip_IOCON_PinSetMode(LPC_IOCON, MnGSWENAPIN, PIN_MODE_INACTIVE);
	Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT, 0, MnGSWENAPINNUM);
	Chip_GPIO_SetPinState(LPC_GPIO_PORT, 0, MnGSWENAPINNUM, 0);
}

/* Set up and initialize all required blocks and functions related to the
   board hardware */
void Board_Init(void)
{
	QUICKJACK_IO_Init();
	
	/* Initialize GPIO */
	Chip_GPIO_Init(LPC_GPIO_PORT);

	/* Initialize the LEDs */
	Board_LED_Init();

	// init sensor switch
	SensorSwitch_Init();

	/* Configure GPIO pin as input pin */
	Chip_GPIO_SetPinDIRInput(LPC_GPIO_PORT, 0, 0);

	/* MCU Tx Pin initialize */
	Chip_IOCON_PinSetMode(LPC_IOCON, QUICKJACKTXPIN, PIN_MODE_INACTIVE);
	Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT, 0, QUICKJACKTXPINNUM);
	Chip_GPIO_SetPinState(LPC_GPIO_PORT, 0, QUICKJACKTXPINNUM, 1);	
	
	/* MCU Rx Pin initialize */
	Chip_IOCON_PinSetMode(LPC_IOCON, QUICKJACKRXPIN, PIN_MODE_INACTIVE);
	/* Configure analog comparator for Manchester Rx */
	Chip_SWM_FixedPinEnable(SWM_FIXED_ACMP_I1, 1);
	Chip_ACMP_Init(LPC_CMP);
	Chip_ACMP_SetPosVoltRef(LPC_CMP, ACMP_POSIN_ACMP_I1);
	Chip_ACMP_SetNegVoltRef(LPC_CMP, ACMP_NEGIN_VLO);
	Chip_ACMP_SetupVoltLadder(LPC_CMP, (15<<1), false);
	Chip_ACMP_EnableVoltLadder(LPC_CMP);
	Chip_ACMP_SetHysteresis(LPC_CMP, ACMP_HYS_10MV);
	Chip_ACMP_SetEdgeSelection(LPC_CMP, ACMP_EDGESEL_RISING);
	/* Uncomment below 2 lines to connect analog comparator output to P1_0 (not needed for normal operation) */
//	Chip_SWM_FixedPinEnable(SWM_FIXED_ACMP_I2, 0);
//	Chip_SWM_MovablePinAssign(SWM_ACMP_O_O, 1);
	
	Chip_ACMP_EdgeClear(LPC_CMP);
	NVIC_EnableIRQ(CMP_IRQn);
	
	/* Disable clocks to SWM and IOCON to save power */
	Chip_Clock_DisablePeriphClock(SYSCTL_CLOCK_SWM);
	Chip_Clock_DisablePeriphClock(SYSCTL_CLOCK_IOCON);
}
