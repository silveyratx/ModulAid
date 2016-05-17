//
//  UserLocator.swift
//  ModulAid
//
//  Created by Ammar on 3/2/16.
//  Copyright © 2016 ModulAid. All rights reserved.
//

import Foundation
import CoreLocation


class UserLocator : NSObject, CLLocationManagerDelegate {
    static let sharedInstance = UserLocator()
    
    var locationManager : CLLocationManager = CLLocationManager()
    var geocoder = CLGeocoder()
    var location : CLLocation? //location coordinates (altitude, longitude)
    var city : String?
    var state : String?
    var country : String?
    
    override init(){
        super.init()
        locationManager.delegate = self
        locationManager.requestWhenInUseAuthorization() //update WhenInUse (in a foreground thread)
        locationManager.desiredAccuracy = kCLLocationAccuracyBest
        locationManager.startUpdatingLocation()
    }
    
    //delegate method (called in a foreground thread) to update location of the user
    @objc func locationManager(manager: CLLocationManager, didUpdateLocations locations: [CLLocation]){
        location = locations.last
        geocoder.reverseGeocodeLocation(location!, completionHandler: { (placemarks: [CLPlacemark]?, error: NSError?) -> Void in
            if(error == nil){
                let placemark = placemarks![0]
                self.country = placemark.country
                self.state = placemark.administrativeArea
                self.city = placemark.locality
            }
        })
    }
    
}
