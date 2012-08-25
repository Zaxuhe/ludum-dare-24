//
//  MyAppDelegate.m
//  Created by Seth Robinson on 3/6/09.
//  For license info, check the license.txt file that should have come with this.
//


#import "MyAppDelegate.h"
#import "EAGLView.h"
#import "InAppPurchaseManager.h"

#ifdef RT_TAPJOY_ENABLED
#import "TapjoyManager.h"
#endif

@implementation MyAppDelegate

@synthesize window;
@synthesize viewController;
@synthesize glView;

- (void) KillNetInit
{
	//LogMsg("Killing net init");
	 if (m_ReadRef) 
	 {
        /* CFReadStreamClose terminates the stream. */
        CFReadStreamClose(m_ReadRef);
        CFRelease(m_ReadRef);
        m_ReadRef = NULL;
    }

    if (m_WriteRef) 
	{
        /* CFWriteStreamClose terminates the stream. */
        
        CFWriteStreamClose(m_WriteRef);
        CFRelease(m_WriteRef);
		m_WriteRef = NULL;
    }
	
	m_HostRef = NULL;
	
}

- (void)applicationDidFinishLaunching:(UIApplication *)application
{
    
    CGRect bounds = [[UIScreen mainScreen] bounds];
    CGPoint center = CGPointMake(bounds.size.width/2, bounds.size.height/2);

    //without doing this, an iPhone-only app won't display on an iPad correctly, starting around iOS 4 or 5?
    [viewController.view setFrame:bounds];
    [viewController.view setCenter:center];
    
	[window addSubview:viewController.view];
	[window makeKeyAndVisible];

    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(keyboardWillShow:) name:UIKeyboardWillShowNotification object:nil];
	[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(keyboardWillHide:) name:UIKeyboardWillHideNotification object:nil];

	//we don't want to waste cycles doing the accelerometer so we half disable it
	[[UIAccelerometer sharedAccelerometer] setDelegate:nil];
	[[UIAccelerometer sharedAccelerometer] setUpdateInterval:1.0f];

	_textField = NULL;
	m_inputboxMaxLen = 10;
	m_requestedKeyboardType = UIKeyboardTypeASCIICapable; //default
	m_HostRef = NULL;
	m_ReadRef = NULL;
	m_WriteRef = NULL;
    
#ifdef RT_IAP_SUPPORT
    m_IOSIAPManager = [[InAppPurchaseManager alloc] init];
    [m_IOSIAPManager InitIAP];
#endif

#ifdef RT_TAPJOY_ENABLED
	m_tapjoyManager = [[TapjoyManager alloc] init];
	[m_tapjoyManager InitTapjoy:application viewController:viewController]; //viewController.view
#endif
	
}

- (void)accelerometer:(UIAccelerometer*)accelerometer didAccelerate:(UIAcceleration*)acceleration
{
	//LogMsg("%.2f, %.2f, %.2f", acceleration.x, acceleration.y, acceleration.z);
	GetMessageManager()->SendGUI(MESSAGE_TYPE_GUI_ACCELEROMETER,  Variant(acceleration.x, acceleration.y, acceleration.z));
}

- (void)OnOpenTextBox: (OSMessage *)pMsg
{
	
	[self KillTextBox];
	LogMsg("OnOpenTextBox: Setting up textbox");
	m_inputboxMaxLen =  pMsg->m_parm1;
	
	//LogMsg("Text is %s, len is %d.  x is %.2f, y is %.2f", pMsg->m_string.c_str(), pMsg->m_parm1, xPos, yPos);
		
	//Create and editable text field. This is used only when the user successfully lands the rocket.
	_textField = [[UITextField alloc] initWithFrame:CGRectMake( pMsg->m_x,  pMsg->m_y, pMsg->m_sizeX, pMsg->m_sizeY)];
	[_textField setDelegate:self];
	[_textField setBackgroundColor:[UIColor colorWithWhite:0.0 alpha:0.5]];
	[_textField setTextColor:[UIColor whiteColor]];
	[_textField setFont:[UIFont fontWithName:@"Arial" size:pMsg->m_fontSize]];
	[_textField setPlaceholder:@""];	
    //we send in the "0" as default text, because if we send blank, we won't get backspace messages.
	[_textField setText:[NSString stringWithCString: "00" encoding: [NSString defaultCStringEncoding]]];
	[_textField setAutocorrectionType: UITextAutocorrectionTypeNo];
	[_textField setClearsOnBeginEditing: NO];
	
    m_keyboardType = pMsg->m_parm2;
    
	switch (m_keyboardType)
	{
		case OSMessage::PARM_KEYBOARD_TYPE_NUMBERS:
		m_requestedKeyboardType = UIKeyboardTypeNumbersAndPunctuation;
		break;
		
		case OSMessage::PARM_KEYBOARD_TYPE_URL:
		m_requestedKeyboardType = UIKeyboardTypeURL;
		break;
		
		case OSMessage::PARM_KEYBOARD_TYPE_EMAIL:
		m_requestedKeyboardType = UIKeyboardTypeEmailAddress;
		break;
	
	default:
		m_requestedKeyboardType = UIKeyboardTypeASCIICapable;
	}
	
	[_textField setKeyboardType: m_requestedKeyboardType]; 
	
	[_textField setTextAlignment: UITextAlignmentLeft];
	[_textField setBorderStyle: UITextBorderStyleBezel];
	[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(OnTextChangedNotification:) name:@"UITextFieldTextDidChangeNotification" object:_textField];
	[viewController.view addSubview:_textField];
	[_textField becomeFirstResponder];  //make the keyboard pop up now
	SetIsUsingNativeUI(true);
	
}
- (void) keyboardWillHide:(NSNotification *) notification
{
   	GetMessageManager()->SendGUI(MESSAGE_TYPE_HW_TOUCH_KEYBOARD_WILL_HIDE, 0.0f);		
}

- (void) keyboardWillShow:(NSNotification *) notification
{
   
   LogMsg("Keyboarding showing");
  
    
    /*
     //What is all this?  Can't remember why I need it..
     
    
    UIView *keyboardView = nil;
 
    NSArray *windows = [[UIApplication sharedApplication] windows];
 
    for (int i = 0; i < [windows count]; ++i) 
	{
         UIWindow *myWindow =  [windows objectAtIndex:i];
 
        for (int j = 0; j < [myWindow.subviews count]; ++j) 
		{
            keyboardView = [myWindow.subviews objectAtIndex:j];
 
            if ([[keyboardView description] hasPrefix:@"<UIKeyboard"] == YES)
			{
                if (m_inputboxMaxLen == 0)
				{
                LogMsg("Making keyboard hidden");
                    keyboardView.hidden = YES;
                    keyboardView.userInteractionEnabled = NO;
                } else
				{
                    keyboardView.hidden = NO;
                    keyboardView.userInteractionEnabled = YES;
                }
                return;
            }
        }
    }
     */
    
    	GetMessageManager()->SendGUI(MESSAGE_TYPE_HW_TOUCH_KEYBOARD_WILL_SHOW, 0.0f);		

    
}
- (void)OnTextChangedNotification:(NSNotification *)note 
{
}

- (void) KillTextBox
{
	if (_textField != NULL)
	{
		//LogMsg("killing text box");
	
		[[NSNotificationCenter defaultCenter] removeObserver:self name:@"UITextFieldTextDidChangeNotification" object:_textField];
		[_textField endEditing:YES];
		[_textField removeFromSuperview];
		_textField = NULL;
		SetIsUsingNativeUI(false);
	}	
}

- (void)onOSMessage:(OSMessage *)pMsg
{
  
	switch (pMsg->m_type)
	{
	
		case OSMessage::MESSAGE_OPEN_TEXT_BOX:
			[self OnOpenTextBox:pMsg];
			break;
		
		case OSMessage::MESSAGE_CHECK_CONNECTION:
			[self ActivateNetworkConnection];
			break;
		
		case OSMessage::MESSAGE_CLOSE_TEXT_BOX:
			[self KillTextBox];
			break;
		case OSMessage::MESSAGE_SET_FPS_LIMIT:
			glView.animationIntervalSave = 1.0/pMsg->m_x;
			break;
		case OSMessage::MESSAGE_SET_ACCELEROMETER_UPDATE_HZ:
			if (pMsg->m_x == 0)
			{
				//disable it
				[[UIAccelerometer sharedAccelerometer] setDelegate:nil];
				[[UIAccelerometer sharedAccelerometer] setUpdateInterval:(1.0)];
			} else
			{
				LogMsg("Enabling acceler with %.2f", pMsg->m_x);
				[[UIAccelerometer sharedAccelerometer] setUpdateInterval:(1.0 / pMsg->m_x)];
				[[UIAccelerometer sharedAccelerometer] setDelegate:self];
				
			}
			break;
			
		case OSMessage::MESSAGE_ALLOW_SCREEN_DIMMING:
		{
			//I read setting it to NO, then YES fixes a bug on iOS 3.0, so thats
			//why I'm doing it that way. -Seth

			UIApplication *myApp = [UIApplication sharedApplication];
			myApp.idleTimerDisabled = NO;
			
			if (pMsg->m_x == 0)
			{
				//disable it
				myApp.idleTimerDisabled = YES;
			}
		}
		break;

		case OSMessage::MESSAGE_IAP_GET_PURCHASED_LIST:
			[m_IOSIAPManager GetPurchasedList];
		break;
			
		case OSMessage::MESSAGE_IAP_PURCHASE:
			LogMsg("iOS> BUYING %s", pMsg->m_string.c_str());
#ifdef RT_IAP_SUPPORT
			
#ifdef _DEBUG
			[m_IOSIAPManager GetProductData:pMsg->m_string]; //prints debug into to the xcode log, only for debugging
#endif
			[m_IOSIAPManager BuyItemByID:pMsg->m_string];
#else
			LogMsg("ERROR: RT_IAP_SUPPORT must be defined in xcode settings, and InAppPurchaseManager.mm added to the project if it's not!");
			assert(!"ERROR: RT_IAP_SUPPORT must be defined in xcode settings, and InAppPurchaseManager.mm added to the project if it's not!");
#endif
		break;
		
		
		//***************** TAPJOY STUFF
        #ifndef RT_TAPJOY_ENABLED
			
		case OSMessage::MESSAGE_TAPJOY_INIT_MAIN:
				LogMsg("ERROR: RT_TAPJOY_ENABLED must be defined in xcode settings to use Tapjoy! (check the Proton wiki for more info)");
				assert(!"ERROR: RT_TAPJOY_ENABLED must be defined in xcode settings to use Tapjoy! (check the Proton wiki for more info)");
        break;
		#endif
		
		default:
		
			Boolean bHandled = false;
			
			#ifdef RT_TAPJOY_ENABLED
				//give our tapjoy manager a chance to handle it
				if (!bHandled) bHandled = [m_tapjoyManager onOSMessage:pMsg];
			#endif

			if (!bHandled)
			{
				LogMsg("iOS Target Warning: unhandled OSMessage type: %d", pMsg->m_type);
			}
	}
}

- (BOOL)textField:(UITextField *)textField shouldChangeCharactersInRange:(NSRange)range replacementString:(NSString *)string
{
	NSLog(string);
	
    if ([string length] == 0)
    {
        //hack for backspace
        GetMessageManager()->SendGUI(MESSAGE_TYPE_GUI_CHAR, 8.0f, 1.0f);
        
        //signal keyboard down
        GetMessageManager()->SendGUI(MESSAGE_TYPE_GUI_CHAR_RAW, 8.0f, 1.0f);
        
        //we don't really have a keyboard up, so we'll fake it now
        GetMessageManager()->SendGUI(MESSAGE_TYPE_GUI_CHAR_RAW, 8.0f, 0.0f);
        return false;
    }
    
    for (int i=0; i < [string length]; i++)
    {
        float letter = (float)[string characterAtIndex:i];
        float upCasedLetter = (float)[ [string uppercaseString] characterAtIndex:i];
        
        GetMessageManager()->SendGUI(MESSAGE_TYPE_GUI_CHAR, letter, 1.0f);
        
        //signal keyboard down
        GetMessageManager()->SendGUI(MESSAGE_TYPE_GUI_CHAR_RAW, upCasedLetter, 1.0f);

        //we don't really have a keyboard up, so we'll fake it now
        GetMessageManager()->SendGUI(MESSAGE_TYPE_GUI_CHAR_RAW, upCasedLetter, 0.0f);
    }
    
    //ignore all changes so our text field doesn't fill up, but the truth is we already sent the characters to Proton.  This
    //method doesn't work for kanji/unicode stuff...
    return false;
}

// Saves the user name and score after the user enters it in the provided text field. 
- (void)textFieldDidEndEditing:(UITextField*)textField 
{
	//Save name
	//NSLog([textField text]);
	//SetLastStringInput([[textField text] UTF8String]);
	
	if (_textField != NULL)
	{
		[_textField endEditing:YES];
		[self KillTextBox];
	}

}

- (void)textFieldDidBeginEditing:(UITextField *)textField
{
	//[textField setPlaceholder:@""];	
}

// Terminates the editing session
- (BOOL)textFieldShouldReturn:(UITextField*)textField
{
	//Send a fake ENTER keypress
    GetMessageManager()->SendGUI(MESSAGE_TYPE_GUI_CHAR, (float)13, 0.0f);
    
    //Terminate editing
	[textField resignFirstResponder];
	
	return YES;
}

- (void)applicationWillResignActive:(UIApplication *)application 
{
	[glView stopAnimation];
	//	glView.animationInterval = 1.0 / 5.0;
	
}

- (void)applicationWillTerminate:(UIApplication *)application
{
	[glView onKill];
}

- (void)applicationDidBecomeActive:(UIApplication *)application 
{
	glView.animationInterval = glView.animationIntervalSave;
	[glView startAnimation];
}

- (void)applicationDidEnterBackground:(UIApplication *)application
{
	GetBaseApp()->OnEnterBackground();
}

- (void)applicationWillEnterForeground:(UIApplication *)application
{
	GetBaseApp()->OnEnterForeground();
}

- (void)ActivateNetworkConnection
{
	
	[self KillNetInit];
	
	m_HostRef = CFHostCreateWithName(nil, (CFStringRef)@"rtsoft.com");
	CFStreamCreatePairWithSocketToCFHost(nil, m_HostRef, 80, &m_ReadRef, &m_WriteRef);
	memset(&context, 0, sizeof(context));
	context.version = 0;
	context.info = self;
	CFWriteStreamSetClient(m_WriteRef,
						   kCFStreamEventOpenCompleted | kCFStreamEventErrorOccurred,
						   MyWriteStreamCallback,
						   &context);
	
	// put the write ref on the runloop
	CFWriteStreamScheduleWithRunLoop(m_WriteRef,
									 [[NSRunLoop currentRunLoop] getCFRunLoop],
									 (CFStringRef)NSDefaultRunLoopMode);
	
	CFWriteStreamOpen(m_WriteRef);
}


- (void)dealloc 
{
	[window release];
	[super dealloc];
}

@end

static void MyWriteStreamCallback(CFWriteStreamRef streamRef,
								  CFStreamEventType eventType,
								  void *clientCallBackInfo)

{
	
    MyAppDelegate *v = (MyAppDelegate *)clientCallBackInfo;
	
    if (eventType == kCFStreamEventOpenCompleted)
    {
		// at this point, the connection is up.  It's possible this is wifi, or EDGE/3G
        // and BSD socket functions will work.
    }
    //LogMsg("Connection is ready: %d", eventType);
    [v KillNetInit];
	GetMessageManager()->SendGUI(MESSAGE_TYPE_OS_CONNECTION_CHECKED, (float)eventType, 0.0f);		
    CFRunLoopStop(CFRunLoopGetCurrent());
}

