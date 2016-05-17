//
//  HistoryViewController.swift
//  ModulAid
//
//  Created by Cage Johnson on 3/21/16.
//  Copyright © 2016 ModulAid. All rights reserved.
//

import Foundation
import UIKit

class HistoryViewController: UIViewController, UITableViewDataSource, UITableViewDelegate {
    
    @IBOutlet var tableView: UITableView?
    var tempData = TemperatureDAO.sharedInstance.tempHistory
    var pulseData = PulseDAO.sharedInstance.pulseHistory
    
    var numOfTempsInTable: Int = 0
    var numOfPulseInTable: Int = 0
    
    
    override func viewDidLoad() {
        tableView?.boundInsideBy(self.view, x1: 20, x2: 20, y1: 100, y2: 20)
        
        tableView?.dataSource = self
        tableView?.delegate = self
    
    }
    
    override func viewDidAppear(animated: Bool) {
        tableView?.reloadData()
    }
  
    // UITableViewDataSource Functions
    
    func tableView(tableView: UITableView, numberOfRowsInSection section: Int) -> Int {
        numOfTempsInTable = 0
        numOfPulseInTable = 0
        return tempData.count + pulseData.count
    }
    
    
    
    
    func tableView(tableView: UITableView, cellForRowAtIndexPath indexPath: NSIndexPath) ->   UITableViewCell {
        
        var tData: NSDictionary?
        var pData: NSDictionary?
        
        let row = indexPath.row
        let tCell = UITableViewCell()
        var tDate: NSDate = NSDate()
        var pDate: NSDate = NSDate()
        
        if (row < tempData.count) {
            tData = tempData.objectAtIndex(row) as! NSDictionary
            if tData! is NSDictionary {
                let temp = tData!.valueForKey("value (C)")
                let date: NSDate = tData!.valueForKey("date") as! NSDate
                tDate = date
                
                var tempAndUnit = String()
                tempAndUnit = String(Int(temp! as! NSNumber))
                tempAndUnit += " C"
                let label = UILabel(frame: CGRect(x:60, y:0, width:80, height:50))
                label.text = tempAndUnit  //this line unwraps an optional, and converts it to a string
                tCell.addSubview(label)
                
                
                let dateFormatter = NSDateFormatter()
                dateFormatter.dateStyle = .ShortStyle
                dateFormatter.timeStyle = .ShortStyle
                let label2 = UILabel(frame: CGRect(x: 145, y: 0, width: 200, height: 50))
                label2.text = dateFormatter.stringFromDate(date)
                tCell.addSubview(label2)
                
                
                let label3 = UILabel(frame: CGRect(x: 5, y: 0, width: 100, height: 50))
                label3.text = String("temp")
                tCell.addSubview(label3)
                
        
            }
        }
        else if (row < tempData.count + pulseData.count) {
            pData = pulseData.objectAtIndex(row - tempData.count) as! NSDictionary
            if pData! is NSDictionary {
                let temp = pData!.valueForKey("value (bpm)")
                let date: NSDate = pData!.valueForKey("date") as! NSDate
                pDate = date
                
                var tempAndUnit = String()
                tempAndUnit = String(temp!)
                tempAndUnit += " BPM"
                let label = UILabel(frame: CGRect(x:60, y:0, width:80, height:50))
                label.text = tempAndUnit  //this line unwraps an optional, and converts it to a string
                tCell.addSubview(label)
                
                
                let dateFormatter = NSDateFormatter()
                dateFormatter.dateStyle = .ShortStyle
                dateFormatter.timeStyle = .ShortStyle
                let label2 = UILabel(frame: CGRect(x: 145, y: 0, width: 200, height: 50))
                label2.text = dateFormatter.stringFromDate(date)
                tCell.addSubview(label2)
                
                
                let label3 = UILabel(frame: CGRect(x: 5, y: 0, width: 100, height: 50))
                label3.text = String("pulse")
                tCell.addSubview(label3)
                
            }
        }
        return tCell
//        
//        if(numOfTempsInTable < tempData.count){
//        tData = tempData.objectAtIndex(numOfTempsInTable) as! NSDictionary
//        }
//        let tCell = UITableViewCell()
//        var tDate: NSDate = NSDate()
//        
//        if(numOfPulseInTable < pulseData.count){
//        pData = pulseData.objectAtIndex(numOfPulseInTable) as! NSDictionary
//        }
//        let pCell = UITableViewCell()
//        var pDate: NSDate = NSDate()
//        
//        if(numOfPulseInTable < pulseData.count){
//            if pData! is NSDictionary {
//                let temp = pData!.valueForKey("value (bpm)")
//                let date: NSDate = pData!.valueForKey("date") as! NSDate
//                pDate = date
//                
//                var tempAndUnit = String()
//                tempAndUnit = String(temp!)
//                tempAndUnit += " BPM"
//                let label = UILabel(frame: CGRect(x:60, y:0, width:80, height:50))
//                label.text = tempAndUnit  //this line unwraps an optional, and converts it to a string
//                pCell.addSubview(label)
//                
//                
//                let dateFormatter = NSDateFormatter()
//                dateFormatter.dateStyle = .ShortStyle
//                dateFormatter.timeStyle = .ShortStyle
//                let label2 = UILabel(frame: CGRect(x: 145, y: 0, width: 200, height: 50))
//                label2.text = dateFormatter.stringFromDate(date)
//                pCell.addSubview(label2)
//                
//                
//                let label3 = UILabel(frame: CGRect(x: 5, y: 0, width: 100, height: 50))
//                label3.text = String("pulse")
//                pCell.addSubview(label3)
//                
//            }
//        }
//        
//        if(numOfTempsInTable < tempData.count){
//            if tData! is NSDictionary {
//                let temp = tData!.valueForKey("value (C)")
//                let date: NSDate = tData!.valueForKey("date") as! NSDate
//                tDate = date
//                
//                var tempAndUnit = String()
//                tempAndUnit = String(Int(temp! as! NSNumber))
//                tempAndUnit += " C"
//                let label = UILabel(frame: CGRect(x:60, y:0, width:80, height:50))
//                label.text = tempAndUnit  //this line unwraps an optional, and converts it to a string
//                tCell.addSubview(label)
//                
//        
//                let dateFormatter = NSDateFormatter()
//                dateFormatter.dateStyle = .ShortStyle
//                dateFormatter.timeStyle = .ShortStyle
//                let label2 = UILabel(frame: CGRect(x: 145, y: 0, width: 200, height: 50))
//                label2.text = dateFormatter.stringFromDate(date)
//                tCell.addSubview(label2)
//
//                
//                let label3 = UILabel(frame: CGRect(x: 5, y: 0, width: 100, height: 50))
//                label3.text = String("temp")
//                tCell.addSubview(label3)
//                
//               
//            }
//        }
//        
//        if (tDate.compare(pDate) == .OrderedAscending){
//            numOfTempsInTable += 1
//            return tCell
//        } else {
//            
//        numOfPulseInTable += 1
//        return pCell
//            
//        }
//        
//        
//        return tCell
        }
    
    // UITableViewDelegate Functions
    func tableView(tableView: UITableView, heightForRowAtIndexPath indexPath: NSIndexPath) -> CGFloat {
        return 50
    }
    
}
