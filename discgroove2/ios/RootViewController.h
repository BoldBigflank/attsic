//
//  discgroove2AppController.h
//  discgroove2
//
//  Created by Alex Swan on 10/19/13.
//  Copyright __MyCompanyName__ 2013. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "PLTDevice.h"

@interface RootViewController : UIViewController <PLTDeviceConnectionDelegate, PLTDeviceInfoObserver> {

}

@property(nonatomic, strong)	PLTDevice				*device;


@end
