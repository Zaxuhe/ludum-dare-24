//
//  MyViewController.m
//  Created by Seth Robinson on 3/6/09.
//  For license info, check the license.txt file that should have come with this.
//


#import "MyViewController.h"
#import "EAGLView.h"

@implementation MyViewController

@synthesize glView;
@synthesize window;

// Implement viewDidLoad to do additional setup after loading the view.
- (void)viewDidLoad 
{
	m_bIgnoredFirst4 = false;
	[glView startAnimation];
	
	[[NSNotificationCenter defaultCenter] addObserver:self
											 selector:@selector(didRotate:)
												 name:UIDeviceOrientationDidChangeNotification
											   object:nil];
	
	[super viewDidLoad];
	
}

- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)myinterfaceOrientation
{
    // Return YES for supported orientations
	//No matter what, we tell it to do portrait mode
	/*
	if (!m_bIgnoredFirst4 && myinterfaceOrientation == 4)
	{
		m_bIgnoredFirst4 = true;
		return false; //ignore this, it sends it on startup incorrectly?
	}
	if (!CanRotateTo(myinterfaceOrientation))
	{
			LogMsg("Refusing orientation %d", myinterfaceOrientation);
			
			return false;
	}
	//LogMsg("Setting orientation to interfaceOrientation %d", myinterfaceOrientation);
	[[UIApplication sharedApplication] setStatusBarOrientation: myinterfaceOrientation animated:NO];
	SetupScreenInfoIPhone(myinterfaceOrientation); 
	*/
	return false;
}

-(void)didRotate:(NSNotification *)theNotification 
{
	UIDeviceOrientation interfaceOrientation = [[UIDevice currentDevice] orientation];
	//NSLog(@"Got didRotate: %d", interfaceOrientation);
	if (!UIDeviceOrientationIsValidInterfaceOrientation(interfaceOrientation))
	{
		//it's probably like a face up/down message.  Ignore it
		return;
	}

	// custom rotation code based on interfaceOrientation here...

	if (!CanRotateTo(interfaceOrientation))
	{
		LogMsg("Refusing orientation %d",interfaceOrientation);
		
		return ;
	}
	//LogMsg("Setting orientation to interfaceOrientation %d", interfaceOrientation);
	[[UIApplication sharedApplication] setStatusBarOrientation: (UIInterfaceOrientation) interfaceOrientation animated:NO];
	SetupScreenInfoIPhone(interfaceOrientation); 
	
}

 
- (void)didRotateFromInterfaceOrientation:(UIInterfaceOrientation)fromInterfaceOrientation
{
	//this should never actually be called.  Why the hell are we even using a ViewController?  Do we really need to?
	LogMsg("DidRotateFromInterface: Got %d, App orientation is %d", fromInterfaceOrientation, GetOrientation());
	return;
}

- (void)didReceiveMemoryWarning 
{
    [super didReceiveMemoryWarning]; // Releases the view if it doesn't have a superview
    GetBaseApp()->OnMemoryWarning();
    
    // Release anything that's not essential, such as cached data
}

- (void)dealloc {
  [super dealloc];
}


@end
