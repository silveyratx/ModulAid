//
//  TemperatureTakerViewController.swift
//  ModulAid
//
//  Created by Cage Johnson on 2/29/16.
//  Copyright © 2016 ModulAid. All rights reserved.
//

import Foundation
import UIKit

extension UIView{
    
    func boundInsideBy(superView: UIView, x1:Int , x2:Int, y1:Int, y2:Int){
        self.translatesAutoresizingMaskIntoConstraints = false
        superView.addConstraints(NSLayoutConstraint.constraintsWithVisualFormat("H:|-a-[subview]-b-|", options: NSLayoutFormatOptions.DirectionLeadingToTrailing, metrics:["a":NSNumber(integer: x1),"b":NSNumber(integer: x2)], views:["subview":self]))
        superView.addConstraints(NSLayoutConstraint.constraintsWithVisualFormat("V:|-c-[subview]-d-|", options: NSLayoutFormatOptions.DirectionLeadingToTrailing, metrics:["c":NSNumber(integer: y1),"d":NSNumber(integer: y2)], views:["subview":self]))
    }
}

//temperature sensor initialization (needs to be accessed by 2 view controllers)
// used to get new data from quick jack, and get stored data from history


class TemperatureTakerViewController : UIViewController {

    @IBOutlet var firstView: UIView?
    @IBOutlet var secondView: UIView?
    @IBOutlet var thirdView: UIView?
    @IBOutlet var tempLabel: UILabel?
    var timer: NSTimer?
    var tempSensor : TemperatureDAO = TemperatureDAO.sharedInstance
    
    override func viewDidLoad() {
        firstView!.boundInsideBy(self.view, x1: 10, x2: 10, y1: 74, y2: 10)
        secondView!.boundInsideBy(self.view, x1: 10, x2: 10, y1: 74, y2: 10)
        thirdView!.boundInsideBy(self.view, x1: 10, x2: 10, y1: 74, y2: 10)
        secondView!.hidden = true
        thirdView!.hidden = true
    }
    
    override func viewDidAppear(animated: Bool) {
        tempSensor.initializeSensor()
        timer = NSTimer(timeInterval: 2, target: self, selector: "takeTemp", userInfo: nil, repeats: true)
    }
    
    override func viewWillDisappear(animated: Bool) {
        tempSensor.saveTempData()
    }
    
    @IBAction func okayButtonPushed(sender: UIButton){
        firstView!.hidden = true
        secondView!.hidden = false
        tempSensor.startSending()
        takeTemp()
        //timer = NSTimer(timeInterval: 2, target: self, selector: "takeTemp", userInfo: nil, repeats: true)
        //NSRunLoop.currentRunLoop().addTimer(timer!, forMode:NSRunLoopCommonModes)
    }
    
    @IBAction func takeAnotherTempButtonPushed(sender: UIButton){
        firstView!.hidden = false
        thirdView!.hidden = true
    }
    
    func takeTemp(){
        secondView!.hidden = true
        thirdView!.hidden = false
        timer!.invalidate()
        
        let temp: Temperature = tempSensor.measureTemperature()    //read the buffer
        //NSThread.sleepForTimeInterval(1)                            //wait for the core audio buffer to fill data
        if(temp.getCelsuis() == -273){
            tempLabel!.text = "Try Again"
            return
        }
        tempSensor.updateTempData(temp)                            //update temperature data
        tempLabel!.text = String(format: "%.1f F", temp.getFahrenheit())                 //display temperature

    }
}