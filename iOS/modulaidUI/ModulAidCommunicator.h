//
//  ModulAidCommunicator.h
//  ModulAid
//
//  Created by Cage Johnson on 2/17/16.
//  Copyright © 2016 ModulAid. All rights reserved.
//

#ifndef ModulAidCommunicator_h
#define ModulAidCommunicator_h

#import <UIKit/UIKit.h> 

@interface ModulAidCommunicator : NSObject

+ (ModulAidCommunicator*) getInstance;

- (void) resetJackBuffers;
- (float) receiveData;
- (int) sendData:(UInt8)data; //return 0 if data was transmitted, 1 if data was not transmitted

@end

#endif /* ModulAidCommunicator_h */
