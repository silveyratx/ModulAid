//
//  Temperature.swift
//  ModulAid
//
//  Temperature objects are used to store temperature data points captured using the quick jack
//
//  Created by Ammar on 2/21/16.
//  Copyright © 2016 ModulAid. All rights reserved.
//

import Foundation

enum TempUnit {
    case Celsius
    case Fahrenheit
}

class Temperature {
    var temperature : Double
    var unit : TempUnit

    init (temperature : Double, unit : TempUnit) {
        self.temperature = temperature
        self.unit = unit
    }
    
    func getCelsuis() -> Double {
        if(unit == .Fahrenheit){
            return ((temperature - 32) * 5)/9
        }
        else{
            return temperature
        }
    }
    
    func getFahrenheit() -> Double {
        if(unit == .Celsius){
            return (temperature * 9)/5 + 32
        }
        else{
            return temperature
        }
    }    
}
