/*
 * @brief QuickJackApp.h
 *
 * @note
 *
 */


#import <Foundation/Foundation.h>
#import "AudioUnit/AudioUnit.h"
#import "aurio_helper.h"
#import "CAStreamBasicDescription.h"

@protocol QuickJackDelegate;


@interface QuickJackApp : NSObject
{
	id <QuickJackDelegate>			theDelegate;
	
	AudioUnit					rioUnit; //our remote I/O unit
	AURenderCallbackStruct		inputProc; //used for registering an input callback function with AudioUnit
	DCRejectionFilter*			dcFilter; //used to get rid of DC component in an audio signal
	CAStreamBasicDescription	thruFormat; //wrapper of AudioStreamBasicDescription class
                                            //provides audio data format specs for a stream of audio
	Float64						hwSampleRate;

	UInt8						uartByteTransmit;
    UInt8                       uartByteReceive;
    UInt8                       uartRecvFlag;
    UInt32                      uartRecvNum;
    UInt8                       uartRecvBuf[1024]; //UART Fifo, each frame sends a byte. Values are transmitted
	BOOL						mute;
	BOOL						newByte;
	UInt32						maxFPS;
    BOOL                        micFlag;
    float                       micSampleValue;
}
	
- (void) setDelegate:(id <QuickJackDelegate>) delegate;
- (id) init;


@property (nonatomic, assign)	AudioUnit				rioUnit;
@property (nonatomic, assign)	AURenderCallbackStruct	inputProc;
@property (nonatomic, assign)	int						unitIsRunning;
@property (nonatomic, assign)   UInt8					uartByteTransmit;
@property (nonatomic, assign)   UInt8                   uartByteReceive;
@property (nonatomic, assign)   UInt32					maxFPS;
@property (nonatomic, assign)	BOOL					newByte;
@property (nonatomic, assign)	BOOL					mute;
@property (nonatomic, assign)	BOOL					micFlag;
@property (nonatomic, assign)   UInt8                   uartRecvFlag;
@property (nonatomic, assign)   UInt32                  uartRecvNum;
@property (nonatomic, assign)   UInt8                   *uartRecvBuf;


@property (nonatomic, assign)   float                   micSampleValue;

@end
	
	
@protocol QuickJackDelegate <NSObject>
	
- (int) receive:(UInt8)data;
-(void) sendData:(NSNotification *)notification;

@end
