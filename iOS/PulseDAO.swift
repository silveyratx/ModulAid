//
//  PulseDAO.swift
//  ModulAid
//
//  Created by Ammar on 4/18/16.
//  Copyright © 2016 ModulAid. All rights reserved.
//

import Foundation

class PulseDAO : SensorDAO {
    static let sharedInstance = PulseDAO()
    
    //pulse history objects
    let pulseFilePath: String = fileInDocumentsDirectory("PulseData.plist") //path to file to store temperature history
    var pulseHistory : NSMutableArray
    var pulseDict : NSMutableDictionary //last measurement
    
    override init() {
        //get stored data history
        if let history = NSMutableArray(contentsOfFile: pulseFilePath) {
            pulseHistory = history
        }
        else{
            pulseHistory = NSMutableArray()
        }
        
        pulseDict = NSMutableDictionary()
        
        super.init();
    }
    
    //MARK: Saving data
    
    //this method updates pulse data in pulse history array
    func updatePulseData(value : Double) {
        //update pulse history
        let location = ["latitude" : Double(locator.location!.coordinate.latitude), "longitude" : Double(locator.location!.coordinate.longitude), "city" : locator.city!, "state" : locator.state!, "country" : locator.country!]
        pulseDict = ["value (bpm)" : value, "date" : NSDate(), "location" : location]
        pulseHistory.addObject(pulseDict)
        
        if(pulseHistory.count > 10){
            pulseHistory.removeObjectAtIndex(0) //remove oldest data point
        }
        
        //notify that last pulse measurement has been updated
        is_changed = true
    }
    
    //this method saves pulse data to plist, and firebase
    //should be called when closing a view controller
    func savePulseData() {
        //save data locally
        pulseHistory.writeToFile(pulseFilePath, atomically: false)
        
        //save data to database remotely
        if(is_changed){
            if let value = pulseDict["value (bpm)"]{
                let dict : NSMutableDictionary = ["value (bpm)" : value, "location" : pulseDict["location"]!]
                let date : NSDate = pulseDict["date"] as! NSDate
                dict.setObject(date.timeIntervalSince1970, forKey: "date") //convert date to number to efficiently store in database
                userFirebaseRef.updateChildValues(["pulse" : dict])
            }
        }
    }
    
    
    //MARK: Sensor Communication Methods
    
    // initializeSensor
    //   should be called to initialize the headphone jack so that to start taking data from quick jack specifically for pulse sensor
    func initializeSensor(){
        communicator.sendData(0x31); //send a code through UART to let the microcontroller read temperature sensor
        communicator.resetJackBuffers();
    }
    
    func startSending(){
        communicator.sendData(0x47);
    }
    
    // measurePulse
    //   used to get pulse reading
    func measurePulse() -> Double {
        var data : Double = Double(communicator.receiveData()); //get data from communicator (casted from float to double)
        //   while(data == 0){
        //       initializeSensor();
        //       NSTimer(timeInterval: 2, target: self, selector: nil, userInfo: nil, repeats: false);
        //       data = Double(communicator.receiveData());
        //   }

        return data;
    }
    
}