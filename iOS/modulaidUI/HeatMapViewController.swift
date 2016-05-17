//
//  HeatMapViewController.swift
//  ModulAid
//
//  Created by Cage Johnson on 3/23/16.
//  Copyright © 2016 ModulAid. All rights reserved.
//

import Foundation
import UIKit
import MapKit


class HeatMapViewController: UIViewController {
    
    @IBOutlet var mapView: MKMapView?
    @IBOutlet var heatMapImage: UIImageView?
    var locations: NSMutableArray?
    var weights: NSMutableArray?
    var tempSensor = TemperatureDAO.sharedInstance
    let firebaseTempData : NSMutableArray = NSMutableArray(); //array to store data from firebase
    
    //this method returns an array of temperature measurements from firebase database
    func getUsersData(){
        tempSensor.usersArrayFirebaseRef.observeSingleEventOfType(.Value, withBlock: { [unowned self] snapshot in
            print(snapshot.childrenCount) // I got the expected number of items
            for rest in snapshot.children.allObjects as! [FDataSnapshot] {
                if let temp = rest.value["temperature"] as? NSMutableDictionary{
                    self.firebaseTempData.addObject(temp);
                }
            }
        });
    }
    
    
    func redrawMapOverlay(){
        
        for object in firebaseTempData  {
            if object is NSDictionary {
                
                let temperature = object.valueForKey("value (C)")?.doubleValue
                let location = object.valueForKey("location")
                if location is NSDictionary {
                    
                    var latitude = CLLocationDegrees((location!.objectForKey("latitude")?.doubleValue)!)
                    var longitude = CLLocationDegrees((location!.objectForKey("longitude")?.doubleValue)!)
                    
                    locations!.addObject(CLLocation(latitude: latitude, longitude: longitude))
                }
                
                weights!.addObject(NSNumber(double: 30))
                
            }
            
        }

         heatMapImage!.image = LFHeatMap.heatMapForMapView(mapView, boost: 0.5, locations: locations! as [AnyObject], weights: weights! as [AnyObject])
        
        
    }
    
    
    override func viewDidLoad() {
        
        mapView?.boundInsideBy(self.view, x1: 5, x2: 5, y1: 5, y2: 5)
      //  let dataFile: String? = NSBundle.mainBundle().pathForResource("quake", ofType: "plist")
        
        //var tempData = TemperatureDAO.sharedInstance.tempHistory
        
        locations = NSMutableArray()
        weights = NSMutableArray()
        
        for object in firebaseTempData  {
            if object is NSDictionary {
                
                let temperature = object.valueForKey("value (C)")?.doubleValue
                let location = object.valueForKey("location")
                if location is NSDictionary {
                    
                    var latitude = CLLocationDegrees((location!.objectForKey("latitude")?.doubleValue)!)
                    var longitude = CLLocationDegrees((location!.objectForKey("longitude")?.doubleValue)!)
                    
                    locations!.addObject(CLLocation(latitude: latitude, longitude: longitude))
                }
                
                weights!.addObject(NSNumber(double: 30))

            }
            
        }
        
        
        
        /*
        if firstObject is NSDictionary {
            var lat: Double = (firstObject?.objectForKey("latitude")?.doubleValue)!
            var long: Double = (firstObject?.objectForKey("longitude")?.doubleValue)!
           
            var i = 100
            while(i>0){
                
                lat = lat + 1
                long = long + 1
                var dict = NSMutableDictionary()
                dict.setObject(NSNumber(double:lat), forKey: "latitude")
                dict.setObject(NSNumber(double:long), forKey: "longitude")
                dict.setObject(NSNumber(double:3.4), forKey: "magnitude")
                tempData.addObject(dict)
                i = i - 1
            }
        }
        */
        
  //    var worked = tempData.writeToFile(dataFile!, atomically: false)
    /*
        let quakeData: NSArray? = NSArray(contentsOfFile: dataFile!)
        locations = NSMutableArray(capacity: (quakeData?.count)!)
        weights = NSMutableArray(capacity: (quakeData?.count)!)
        
 */
        
        /*
        for reading in quakeData! {
            if let read = reading as? NSDictionary {
                var latitude: CLLocationDegrees = (read.objectForKey("latitude")?.doubleValue)!
                var longitude: CLLocationDegrees = (read.objectForKey("longitude")?.doubleValue)!
                var magnitude: Double = (read.objectForKey("magnitude")?.doubleValue)!
                
                locations!.addObject(CLLocation(latitude: latitude, longitude: longitude))
                weights!.addObject(NSNumber(double: magnitude * 10))
            }
        }
        
        */
        
        var span =  MKCoordinateSpan(latitudeDelta: 17.0, longitudeDelta: 21.5)
        var center = CLLocationCoordinate2D(latitude: 39.0, longitude: -77.0)
        self.mapView?.region = MKCoordinateRegionMake(center, span)
        
    
        
        heatMapImage!.image = LFHeatMap.heatMapForMapView(mapView, boost: 0.5, locations: locations! as [AnyObject], weights: weights! as [AnyObject])
        
    }
    
    //get updated data every time we open the view controller
    override func viewDidAppear(animated: Bool) {
        getUsersData();
    }
    
    override func touchesEnded(touches: Set<UITouch>, withEvent event: UIEvent?) {
        redrawMapOverlay()  
      //  heatMapImage!.image = LFHeatMap.heatMapForMapView(mapView, boost: 0.5, locations: locations! as [AnyObject], weights: weights! as [AnyObject])
    }
  
    
    
}