//
//  ModulAidViewController.swift
//  ModulAid
//
//  Created by Ammar on 2/17/16.
//  Copyright © 2016 ModulAid. All rights reserved.
//

import Foundation
import UIKit

class ModulAidViewController: UIViewController {
    
    var currentViewController: UIViewController?
    var firstChildTabVC : UIViewController?
    var secondChildTabVC : UIViewController?
    var thirdChildTabVC : UIViewController?
    var locator : UserLocator = UserLocator.sharedInstance
    var communicator = ModulAidCommunicator.getInstance()
    
    override func viewDidLoad(){
        super.viewDidLoad()

        displayCurrentTab(1)
        
        //communicator.sendData(0x99)
        //communicator.resetJackBuffers()

        NSThread.sleepForTimeInterval(1)

       // NSThread.sleepForTimeInterval(1)

        NSThread.sleepForTimeInterval(1) //sleep for 1 second (wait till audio jack is initialized)
        //let temp = sensor.measureTemperature()
        //print(temp)
        
    }
    
    override func viewDidAppear(animated: Bool) {
        communicator.sendData(0x99)
    }

    
    @IBAction func segmentedControlPresses(sender:UISegmentedControl){
        self.currentViewController!.view.removeFromSuperview()
        self.currentViewController!.removeFromParentViewController()
        displayCurrentTab(sender.selectedSegmentIndex)
    }
    
    func displayCurrentTab(tabIndex: Int){
        
        if let vc = viewControllerForSelectedSegmentIndex(tabIndex) {
            self.addChildViewController(vc)
            vc.didMoveToParentViewController(self)
            vc.view.frame = self.view.bounds
            self.view.addSubview(vc.view)
            self.currentViewController = vc
        }
    }

    func viewControllerForSelectedSegmentIndex(index: Int) -> UIViewController?{
        var vc: UIViewController?
        switch index {
        case 0 :
            if firstChildTabVC == nil {
                firstChildTabVC = self.storyboard!.instantiateViewControllerWithIdentifier("HeatMapViewController") as? UIViewController
            }
            vc = firstChildTabVC
        case 1 :
            if secondChildTabVC == nil {
                secondChildTabVC = self.storyboard!.instantiateViewControllerWithIdentifier("HomeViewController") as? UIViewController
            }
            vc = secondChildTabVC
        case 2 :
            if thirdChildTabVC == nil {
                thirdChildTabVC = self.storyboard!.instantiateViewControllerWithIdentifier("HistoryViewController") as? UIViewController
            }
            vc = thirdChildTabVC
        default:
            return nil
        }
        return vc
        
        }
    
    
    
    
    
    
    
    
    /*
    required init?(coder aDecoder: NSCoder) {
        super.init(nibName: "ModulAidViewController", bundle: NSBundle.mainBundle())
    }
    */
    
}
