//
//  discgroove2AppController.h
//  discgroove2
//
//  Created by Alex Swan on 10/19/13.
//  Copyright __MyCompanyName__ 2013. All rights reserved.
//

#import "RootViewController.h"
#import "HelloWorldScene.h"
#import "Player.h"


@implementation RootViewController

/*
 // The designated initializer.  Override if you create the controller programmatically and want to perform customization that is not appropriate for viewDidLoad.
- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil {
    if ((self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil])) {
        // Custom initialization
    }
    return self;
}
*/

/*
// Implement loadView to create a view hierarchy programmatically, without using a nib.
- (void)loadView {
}
*/

/*
// Implement viewDidLoad to do additional setup after loading the view, typically from a nib.
- (void)viewDidLoad {
    [super viewDidLoad];
}
 
*/
// Override to allow orientations other than the default portrait orientation.
// This method is deprecated on ios6
- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation {
    return UIInterfaceOrientationIsPortrait( interfaceOrientation );
}

// For ios6, use supportedInterfaceOrientations & shouldAutorotate instead
- (NSUInteger) supportedInterfaceOrientations{
#ifdef __IPHONE_6_0
    return UIInterfaceOrientationMaskPortrait; // UIInterfaceOrientationMaskLandscape;
#endif
}

- (BOOL) shouldAutorotate {
    return YES;
}

- (void)viewWillAppear:(BOOL)animated
{
	[super viewWillAppear:animated];
	
	NSArray *devices = [PLTDevice availableDevices];
	if ([devices count]) {
		self.device = devices[0];
		self.device.connectionDelegate = self;
		[self.device openConnection];
        NSLog(@"%@", self.device);
	}
	else {
		NSLog(@"No available devices.");
	}
	
	[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(newDeviceAvailableNotification:) name:PLTDeviceNewDeviceAvailableNotification object:nil];
}

- (void)PLTDeviceDidOpenConnection:(PLTDevice *)aDevice
{
	NSLog(@"PLTDeviceDidOpenConnection: %@", aDevice);
	
	NSError *err = [self.device subscribe:self toService:PLTServiceOrientationTracking withMode:PLTSubscriptionModeOnChange minPeriod:0];
	if (err) NSLog(@"Error: %@", err);
	
	err = [self.device subscribe:self toService:PLTServiceWearingState withMode:PLTSubscriptionModeOnChange minPeriod:0];
	if (err) NSLog(@"Error: %@", err);
    
	err = [self.device subscribe:self toService:PLTServiceProximity withMode:PLTSubscriptionModeOnChange minPeriod:0];
	if (err) NSLog(@"Error: %@", err);
	
	err = [self.device subscribe:self toService:PLTServicePedometer withMode:PLTSubscriptionModeOnChange minPeriod:0];
	if (err) NSLog(@"Error: %@", err);
	
	err = [self.device subscribe:self toService:PLTServiceFreeFall withMode:PLTSubscriptionModeOnChange minPeriod:0];
	if (err) NSLog(@"Error: %@", err);
	
	// note: this doesn't work right.
	err = [self.device subscribe:self toService:PLTServiceTaps withMode:PLTSubscriptionModeOnChange minPeriod:0];
	if (err) NSLog(@"Error: %@", err);
	
	err = [self.device subscribe:self toService:PLTServiceMagnetometerCalStatus withMode:PLTSubscriptionModeOnChange minPeriod:0];
	if (err) NSLog(@"Error: %@", err);
	
	err = [self.device subscribe:self toService:PLTServiceGyroscopeCalibrationStatus withMode:PLTSubscriptionModeOnChange minPeriod:0];
	if (err) NSLog(@"Error: %@", err);
}

- (void)PLTDevice:(PLTDevice *)aDevice didUpdateInfo:(PLTInfo *)theInfo
{
	NSLog(@"PLTDevice: %@ didUpdateInfo: %@", aDevice, theInfo);
    HelloWorld *gameLayer = ( HelloWorld* ) cocos2d::CCDirector::sharedDirector()->getRunningScene()->getChildByTag(443);
    Player *player = ( Player* ) gameLayer->getChildByTag(543);
	
	if ([theInfo isKindOfClass:[PLTOrientationTrackingInfo class]]) {
		PLTEulerAngles eulerAngles = ((PLTOrientationTrackingInfo *)theInfo).eulerAngles;
//		self.headingLabel.text = [NSString stringWithFormat:@"%ldº", lroundf(eulerAngles.x)];
        player->setAngle(-2.4f * eulerAngles.x);
        player->setDiveAngle(eulerAngles.z);
//		self.pitchLabel.text = [NSString stringWithFormat:@"%ldº", lroundf(eulerAngles.y)];
//		self.rollLabel.text = [NSString stringWithFormat:@"%ldº", lroundf(eulerAngles.z)];
	}
	else if ([theInfo isKindOfClass:[PLTWearingStateInfo class]]) {
//		self.wearingStateLabel.text = (((PLTWearingStateInfo *)theInfo).isBeingWorn ? @"yes" : @"no");
	}
	else if ([theInfo isKindOfClass:[PLTProximityInfo class]]) {
		PLTProximityInfo *proximityInfo = (PLTProximityInfo *)theInfo;
//		self.mobileProximityLabel.text = NSStringFromProximity(proximityInfp.mobileProximity);
//		self.pcProximityLabel.text = NSStringFromProximity(proximityInfp.pcProximity);
	}
	else if ([theInfo isKindOfClass:[PLTPedometerInfo class]]) {
//		self.pedometerLabel.text = [NSString stringWithFormat:@"%u", ((PLTPedometerInfo *)theInfo).steps];
	}
	else if ([theInfo isKindOfClass:[PLTFreeFallInfo class]]) {
		BOOL isInFreeFall = ((PLTFreeFallInfo *)theInfo).isInFreeFall;
		if (isInFreeFall) {
//			self.freeFallLabel.text = (isInFreeFall ? @"yes" : @"no");
//			[self startFreeFallResetTimer];
		}
	}
	else if ([theInfo isKindOfClass:[PLTTapsInfo class]]) {
		PLTTapsInfo *tapsInfo = (PLTTapsInfo *)theInfo;
//		NSString *directionString = NSStringFromTapDirection(tapsInfo.direction);
//		self.tapsLabel.text = [NSString stringWithFormat:@"%u in %@", tapsInfo.taps, directionString];
        if(tapsInfo.taps > 0){
            [self.device setCalibration:nil forService:PLTServiceOrientationTracking];
        }
//		[self startTapsResetTimer];
	}
	else if ([theInfo isKindOfClass:[PLTMagnetometerCalibrationInfo class]]) {
//		self.magnetometerCalLabel.text = (((PLTMagnetometerCalibrationInfo *)theInfo).isCalibrated ? @"YES" : @"NO");
	}
	else if ([theInfo isKindOfClass:[PLTGyroscopeCalibrationInfo class]]) {
//		self.gyroscopeCalLabel.text = (((PLTGyroscopeCalibrationInfo *)theInfo).isCalibrated ? @"YES" : @"NO" );
	}
}

- (void)didReceiveMemoryWarning {
    // Releases the view if it doesn't have a superview.
    [super didReceiveMemoryWarning];
    
    // Release any cached data, images, etc that aren't in use.
}

- (void)viewDidUnload {
    [super viewDidUnload];
    // Release any retained subviews of the main view.
    // e.g. self.myOutlet = nil;
}


- (void)dealloc {
    [super dealloc];
}


@end
