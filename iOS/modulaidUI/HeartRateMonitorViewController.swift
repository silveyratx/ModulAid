//
//  HeartRateMonitorViewController.swift
//  ModulAid
//
//  Created by Cage Johnson on 3/21/16.
//  Copyright © 2016 ModulAid. All rights reserved.
//

import Foundation
import UIKit


class HeartRateMonitorViewController : UIViewController {
    
    @IBOutlet var firstView: UIView?
    @IBOutlet var secondView: UIView?
    @IBOutlet var thirdView: UIView?
    @IBOutlet var heartRateLabel:UILabel?
    var timer: NSTimer?
    var pulseSensor = PulseDAO.sharedInstance;
    
    override func viewDidLoad() {
        firstView!.boundInsideBy(self.view, x1: 10, x2: 10, y1: 74, y2: 10)
        secondView!.boundInsideBy(self.view, x1: 10, x2: 10, y1: 74, y2: 10)
        thirdView!.boundInsideBy(self.view, x1: 10, x2: 10, y1: 74, y2: 10)
        secondView!.hidden = true
        thirdView!.hidden = true
    }
    
    override func viewDidAppear(animated: Bool) {
        pulseSensor.initializeSensor()
        timer = NSTimer(timeInterval: 1, target: self, selector: "takeRate", userInfo: nil, repeats: true)
    }
    
    override func viewWillDisappear(animated: Bool) {
        pulseSensor.savePulseData()
    }
    
    @IBAction func okayButtonPushed(sender: UIButton){
        firstView!.hidden = true
        secondView!.hidden = false
        pulseSensor.startSending()
        takeRate()
        //timer = NSTimer(timeInterval: 1, target: self, selector: "takeRate", userInfo: nil, repeats: true)
        NSRunLoop.currentRunLoop().addTimer(timer!, forMode:NSRunLoopCommonModes)
    }
    
    @IBAction func takeAnotherRateButtonPushed(sender: UIButton){
        firstView!.hidden = false
        thirdView!.hidden = true
    }
    
    func takeRate(){
        secondView!.hidden = true
        thirdView!.hidden = false
        timer!.invalidate()
        
        //NSThread.sleepForTimeInterval(1)                            //wait for the core audio buffer to fill data
        let pulse = pulseSensor.measurePulse()    //read the buffer
        if(pulse == 0.0){
            heartRateLabel!.text = "Try Again"
            return
        }
        pulseSensor.updatePulseData(pulse)
        heartRateLabel!.text = String(format: "%d bpm", Int(pulse))                 //display temperature
    }
}