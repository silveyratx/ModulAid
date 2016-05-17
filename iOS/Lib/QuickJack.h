//
//  QuickJack.h
//  ModulAid
//
//  Created by Ammar on 2/15/16.
//  Copyright © 2016 ModulAid. All rights reserved.
//

#ifndef QuickJack_h
#define QuickJack_h

#import <Foundation/Foundation.h>

@protocol QuickJackDelegate <NSObject>

- (int) receive:(UInt8)data;
-(void) sendData:(NSNotification *)notification;

@end

#endif /* QuickJack_h */
