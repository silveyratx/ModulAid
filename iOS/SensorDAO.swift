//
//  SensorDAO.swift
//  ModulAid
//
//  Created by Ammar on 4/18/16.
//  Copyright © 2016 ModulAid. All rights reserved.
//

import Foundation

//path finding methods

//helper function that returns path to directory to store data
func documentsDirectory() -> String {
    let documentsFolderPath = NSSearchPathForDirectoriesInDomains(NSSearchPathDirectory.DocumentDirectory, NSSearchPathDomainMask.UserDomainMask, true)[0] as String
    return documentsFolderPath
}

//function that returns path to filename in documents directory
func fileInDocumentsDirectory(filename : String) -> String {
    return documentsDirectory().NS.stringByAppendingPathComponent(filename)
}

class SensorDAO {
    //utility objects to get location and data from hardware
    var communicator : ModulAidCommunicator = ModulAidCommunicator.getInstance()
    var locator : UserLocator = UserLocator.sharedInstance

    //firebase objects
    let userInfoFilePath : String = fileInDocumentsDirectory("User.plist")
    let firebaseRef : Firebase  = Firebase(url:"https://modulaid.firebaseio.com") //root reference to our database
    let usersArrayFirebaseRef : Firebase
    var userFirebaseRef : Firebase //reference to user in our database
    var userInfo : NSMutableDictionary
    var id : String //user id
    var is_changed : Bool
    
    init(){
        //get firebase data
        if let info = NSMutableDictionary(contentsOfFile: userInfoFilePath){
            userInfo = info //information about the user stored locally
            is_changed = false
        }
        else{
            userInfo = NSMutableDictionary()
            userInfo.setObject("0", forKey: "id")
            is_changed = true
        }
        
        //get user id
        usersArrayFirebaseRef = firebaseRef.childByAppendingPath("users")
        id = userInfo.valueForKey("id") as! String
        if(id == "0"){
            //user didn't use modulaid before
            userFirebaseRef = usersArrayFirebaseRef.childByAutoId()
            
            id = userFirebaseRef.key
            
            //store the new id locally
            userInfo.setObject(id, forKey : "id")
            userInfo.writeToFile(userInfoFilePath, atomically: false)
        }
        else{
            userFirebaseRef = usersArrayFirebaseRef.childByAppendingPath(id)
        }

    }
        
}