//
//  TemperatureDAO.swift
//  ModulAid
//
//  Created by Ammar on 2/21/16.
//  Copyright © 2016 ModulAid. All rights reserved.
//

import Foundation

//an extension to add the NSString funcionality in Objective-C to Swift's String class (useful in using path methods)
public extension String {
    var NS: NSString { return (self as NSString) }
}

// Temperature Data Access Object
class TemperatureDAO : SensorDAO {
    static let sharedInstance = TemperatureDAO() //singleton instance of TemperatureDAO
    
    //temperature history objects
    let historyFilePath: String = fileInDocumentsDirectory("TemperatureData.plist") //path to file to store temperature history
    var tempHistory : NSMutableArray
    var tempDict : NSMutableDictionary //last measurement
    
    override init() {
        //get stored data history
        if let history = NSMutableArray(contentsOfFile: historyFilePath) {
            tempHistory = history
        }
        else{
            tempHistory = NSMutableArray()
        }
                
        tempDict = NSMutableDictionary()
        
        super.init();
    }
    
    //MARK: Saving data
    
    //this method updates temperature data in temperature history array
    //it ignores temperatures that are not possible for humans
    func updateTempData(temp : Temperature) {
        //45.0 is severe hyperthermia
        //35.0 is sever hypothermia
        //if(temp.getCelsuis() >= 35.0 && temp.getCelsuis() <= 45.0){
            //update temperature history
            let location = ["latitude" : Double(locator.location!.coordinate.latitude), "longitude" : Double(locator.location!.coordinate.longitude), "city" : locator.city!, "state" : locator.state!, "country" : locator.country!]
            tempDict = ["value (C)" : temp.getCelsuis(), "date" : NSDate(), "location" : location]
            tempHistory.addObject(tempDict)
            
            if(tempHistory.count > 10){
                tempHistory.removeObjectAtIndex(0) //remove oldest data point
            }
            
            //notify that last temperature measurement has been updated
            is_changed = true
        //}
    }
    
    //this method saves temperature data to plist, and firebase
    //should be called when closing a view controller
    func saveTempData() {
        //save data locally
        tempHistory.writeToFile(historyFilePath, atomically: false)
        
        //save data to database remotely
        if(is_changed){
            if let value = tempDict["value (C)"]{
                let dict : NSMutableDictionary = ["value (C)" : value, "location" : tempDict["location"]!]
                let date : NSDate = tempDict["date"] as! NSDate
                dict.setObject(date.timeIntervalSince1970, forKey: "date") //convert date to number to efficiently store in database
                userFirebaseRef.updateChildValues(["temperature" : dict])
            }
        }
    }
    

    //MARK: Sensor Communication Methods
    
    // initializeSensor
    //   should be called to initialize the headphone jack so that to start taking data from quick jack specifically for temperature sensor
    func initializeSensor(){
        communicator.sendData(0x32); //send a code through UART to let the microcontroller read temperature sensor
        communicator.resetJackBuffers();
    }
    
    func startSending(){
        communicator.sendData(0x72);
    }

    // measureTemperature
    //   used to get temperature reading
    //   returns a temperature object
    func measureTemperature() -> Temperature {
        var sensorData = communicator.receiveData()
        if(sensorData >= 0x27AD && sensorData <= 0x7FFF){
        
        var data : Double = Double(sensorData * 0.02); //get data from communicator (casted from float to double)
        data -= 273.15;                    //

     //   while(data == 0){
     //       initializeSensor();
     //       NSTimer(timeInterval: 2, target: self, selector: nil, userInfo: nil, repeats: false);
     //       data = Double(communicator.receiveData());
     //   }
        
        return Temperature(temperature: data, unit: .Celsius);
        }
        else{
            return Temperature(temperature: -273, unit: .Celsius);
        }
    }

}
