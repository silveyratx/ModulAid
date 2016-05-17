//
//  ModulAidCommunicator.mm
//  ModulAid
//
//  Created by Cage Johnson on 2/17/16.
//  Copyright © 2016 ModulAid. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "../Lib/QuickJackApp.h"
#import "ModulAidCommunicator.h"
#import "ModulAid-swift.h"

//a singleton object to communicate with the headphone jack

@implementation ModulAidCommunicator : NSObject

QuickJackApp *QuickJack;

- (id)init {
    QuickJack = [[QuickJackApp alloc] init];
    return self;
}

+ (ModulAidCommunicator*) getInstance {
    static ModulAidCommunicator *sharedCommunicator = nil;
    @synchronized(self) {
        if(sharedCommunicator == nil){
            sharedCommunicator = [[ModulAidCommunicator alloc] init];
        }
    }
    return sharedCommunicator;
}


//IMPORTANT: how the LPC800 Firmware sends sensor data:
//  it sends a data header 0xFFFF first,
//  and sends the sensor data in the next 2 bytes (data is 16-bit) with the Most significant byte first
- (float) receiveData {
    UInt16 SensorValue = 0;
    int counter = 0;
    while(QuickJack.uartRecvNum < 4 && counter < 10000){
        counter++;
    }
    int i;
    if(QuickJack.uartRecvNum >= 4) {
        for(i=0; i<QuickJack.uartRecvNum; i++) {
            if((QuickJack.uartRecvBuf[i] == 0xFF) && (QuickJack.uartRecvBuf[i+1]==0xFF))
            {
                //get data
                UInt16 data1 = (QuickJack.uartRecvBuf[i+2] * 256);
                SensorValue = (data1 + QuickJack.uartRecvBuf[i+3]); //casting a int16_t value to float, wtf??
                
                //reset UART buffer
                memset(QuickJack.uartRecvBuf, 0x00, QuickJack.uartRecvNum);
                QuickJack.uartRecvNum = 0;
            }
        }
    }
    
    return SensorValue;

}

// resets the receive buffer of quick jack
- (void) resetJackBuffers {
    memset(QuickJack.uartRecvBuf, 0x00, 256);
    QuickJack.uartRecvNum = 0;

}

- (int) sendData:(UInt8)data{
    uint32_t i;
    while(QuickJack.newByte == TRUE) {
        if( i<10 ) { //quick jack is busy, wait
            i++;
            [NSThread sleepForTimeInterval:1];
        }
        else {
            return 1; //failed to send data
        }
        
    };
    if (QuickJack.newByte == FALSE) {
        // transmitter ready
        QuickJack.uartByteTransmit = data;
        QuickJack.newByte = TRUE;
        return 0;
    }
    return 1;
}

@end
