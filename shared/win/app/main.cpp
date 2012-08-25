//THIS FILE IS SHARED BETWEEN ALL GAMES (during Windows and WebOS builds) !  I don't always put it in the "shared" folder in the msvc project tree because
//I want quick access to it.

#include "PlatformPrecomp.h"
#include "main.h"
#include "WebOS/SDLMain.h"
#include "BaseApp.h"

#ifdef RT_FLASH_TEST
	#include "flash/app/cpp/GLFlashAdaptor.h"
#endif

#ifndef RT_WEBOS
	#include <direct.h>
#endif

//uncomment below and so you can use alt print-screen to take screenshots easier (no border)
//#define C_BORDERLESS_WINDOW_MODE_FOR_SCREENSHOT_EASE 

//My system, or the PVR GLES emulator or something often has issues with WM_CHAR missing messages.  So I work around it with this:
#define C_DONT_USE_WM_CHAR

//If this is uncommented, the app won't suspend/resume when losing focus in windows, but always runs.
//(You should probably add it up as a preprocessor compiler define if you need it, instead of uncommenting it here)
//#define RT_RUNS_IN_BACKGROUND

bool g_winAllowFullscreenToggle = true;
bool g_winAllowWindowResize = true;

vector<VideoModeEntry> g_videoModes;
void AddVideoMode(string name, int x, int y, ePlatformID platformID, eOrientationMode forceOrientation = ORIENTATION_DONT_CARE);
void SetVideoModeByName(string name);

void InitVideoSize()
{
#ifdef RT_WEBOS_ARM
	return;
#endif

	AddVideoMode("Windows", 1024, 768, PLATFORM_ID_WINDOWS);
	AddVideoMode("Windows Wide", 1280, 800, PLATFORM_ID_WINDOWS);
	
#ifndef RT_WEBOS
	// get native window size
	HWND        hDesktopWnd = GetDesktopWindow();
	HDC         hDesktopDC = GetDC(hDesktopWnd);
	int nScreenX = GetDeviceCaps(hDesktopDC, HORZRES);
	int nScreenY = GetDeviceCaps(hDesktopDC, VERTRES);
	ReleaseDC(hDesktopWnd, hDesktopDC);
	AddVideoMode("Windows Native", nScreenX, nScreenY, PLATFORM_ID_WINDOWS);
#endif

	//OSX
	AddVideoMode("OSX", 1024,768, PLATFORM_ID_OSX); 
	AddVideoMode("OSX Wide", 1280,800, PLATFORM_ID_OSX); 

	//iOS - for testing on Windows, you should probably use the "Landscape" versions unless you want to hurt your
	//neck.

	AddVideoMode("iPhone", 320, 480, PLATFORM_ID_IOS);
	AddVideoMode("iPhone Landscape", 480, 320, PLATFORM_ID_IOS, ORIENTATION_PORTRAIT); //force orientation for emulation so it's not sideways
	AddVideoMode("iPad", 768, 1024, PLATFORM_ID_IOS);
	AddVideoMode("iPad Landscape", 1024, 768, PLATFORM_ID_IOS, ORIENTATION_PORTRAIT); //force orientation for emulation so it's not sideways);
	AddVideoMode("iPhone4", 640, 960, PLATFORM_ID_IOS);
	AddVideoMode("iPhone4 Landscape", 960,640, PLATFORM_ID_IOS, ORIENTATION_PORTRAIT); //force orientation for emulation so it's not sideways););
	AddVideoMode("iPad HD", 768*2, 1024*2, PLATFORM_ID_IOS);
	
	//Palm er, I mean HP. These should use the Debug WebOS build config in MSVC for the best results, it will
	//use their funky SDL version
	AddVideoMode("Pre", 320, 480, PLATFORM_ID_WEBOS);
	AddVideoMode("Pre Landscape", 480, 320, PLATFORM_ID_WEBOS);
	AddVideoMode("Pixi", 320, 400, PLATFORM_ID_WEBOS);
	AddVideoMode("Pre 3", 480, 800, PLATFORM_ID_WEBOS);
	AddVideoMode("Pre 3 Landscape", 800,480, PLATFORM_ID_WEBOS);
	AddVideoMode("Touchpad", 768, 1024, PLATFORM_ID_WEBOS);
	AddVideoMode("Touchpad Landscape", 1024, 768, PLATFORM_ID_WEBOS);

	//Android
	AddVideoMode("G1", 320, 480, PLATFORM_ID_ANDROID);
	AddVideoMode("G1 Landscape", 480, 320, PLATFORM_ID_ANDROID);
	AddVideoMode("Nexus One", 480, 800, PLATFORM_ID_ANDROID);
	AddVideoMode("Nexus One Landscape", 800, 480, PLATFORM_ID_ANDROID); 
	AddVideoMode("Droid Landscape", 854, 480, PLATFORM_ID_ANDROID); 
	AddVideoMode("Xoom Landscape", 1280,800, PLATFORM_ID_ANDROID);
	AddVideoMode("Xoom", 800,1280, PLATFORM_ID_ANDROID);
	AddVideoMode("Galaxy Tab 7.7 Landscape", 1024,600, PLATFORM_ID_ANDROID);
	AddVideoMode("Galaxy Tab 10.1 Landscape", 1280,800, PLATFORM_ID_ANDROID);
	AddVideoMode("Xperia Play Landscape", 854, 480, PLATFORM_ID_ANDROID);

	//RIM Playbook OS/BBX/BB10/Whatever they name it to next week
	AddVideoMode("Playbook", 600,1024, PLATFORM_ID_BBX);
	AddVideoMode("Playbook Landscape", 1024,600, PLATFORM_ID_BBX);

	AddVideoMode("Flash", 480, 320, PLATFORM_ID_FLASH);

	//WORK: Change device emulation here
	string desiredVideoMode = "Windows"; //name needs to match one of the ones defined above
 	SetVideoModeByName(desiredVideoMode);
	GetBaseApp()->OnPreInitVideo(); //gives the app level code a chance to override any of these parms if it wants to
}

//***************************************************************************

int g_winVideoScreenX = 0;
int g_winVideoScreenY = 0;
bool g_bIsFullScreen = false;
int g_fpsLimit = 0; //0 for no fps limit (default)  Use MESSAGE_SET_FPS_LIMIT to set
bool g_bIsMinimized = false;


void AddVideoMode(string name, int x, int y, ePlatformID platformID, eOrientationMode forceOrientation)
{
	g_videoModes.push_back(VideoModeEntry(name, x, y, platformID, forceOrientation));
}

void SetVideoModeByName(string name)
{
	VideoModeEntry *v = NULL;

	for (unsigned int i=0; i < g_videoModes.size(); i++)
	{
		v = &g_videoModes[i];
		if (v->name == name)
		{
			g_winVideoScreenX = v->x;
			g_winVideoScreenY = v->y;
			SetEmulatedPlatformID(v->platformID);
			SetForcedOrientation(v->forceOrientation);
			return;
		}
	}

	LogError("Don't have %s registered as a video mode.", name.c_str());
	assert(!"huh?");
}

#ifdef _IRR_STATIC_LIB_
#include "Irrlicht/IrrlichtManager.h"
using namespace irr;
#endif

#ifdef C_GL_MODE

#define eglGetError glGetError
#define EGLint GLint
#define EGL_SUCCESS GL_NO_ERROR
#else

#ifndef RT_WEBOS
EGLDisplay			g_eglDisplay	= 0;
EGLSurface			g_eglSurface	= 0;
#endif
#endif

int GetPrimaryGLX() 
{
	return g_winVideoScreenX;
}

int GetPrimaryGLY() 
{
	return g_winVideoScreenY;
}	

bool g_bHasFocus = true;
int mousePosX = 0;
int mousePosY = 0;
bool g_bAppFinished = false;
bool g_escapeMessageSent = false; //work around for problems on my dev machine with escape not being sent on keydown sometimes, fixed by reboot (!?)

#ifndef RT_WEBOS

#define	WINDOW_CLASS _T("AppClass")

#include <windows.h>

#ifndef GET_X_LPARAM
#define GET_X_LPARAM(lp)                        ((int)(short)LOWORD(lp))
#endif
#ifndef GET_Y_LPARAM
#define GET_Y_LPARAM(lp)                        ((int)(short)HIWORD(lp))
#endif
bool g_leftMouseButtonDown = false; //to help emulate how an iphone works
bool g_rightMouseButtonDown = false; //to help emulate how an iphone works

// Windows variables
HWND				g_hWnd	= 0;
HDC					g_hDC		= 0;
HINSTANCE g_hInstance = 0;

#define KEY_DOWN  0x8000

uint32 GetWinkeyModifiers()
{

	uint32 modifierKeys = 0;

	if (GetKeyState(VK_CONTROL) & KEY_DOWN)
	{
		modifierKeys = modifierKeys | VIRTUAL_KEY_MODIFIER_CONTROL;
	}

	if (GetKeyState(VK_SHIFT)& KEY_DOWN)
	{
		modifierKeys = modifierKeys | VIRTUAL_KEY_MODIFIER_SHIFT;
	}

	if (GetKeyState(VK_MENU)& KEY_DOWN)
	{
		modifierKeys = modifierKeys | VIRTUAL_KEY_MODIFIER_ALT;
	}
	return modifierKeys;
}

int ConvertWindowsKeycodeToProtonVirtualKey(int keycode)
{
	switch (keycode)
	{
	case 37: keycode = VIRTUAL_KEY_DIR_LEFT; break;
	case 39: keycode = VIRTUAL_KEY_DIR_RIGHT; break;
	case 38: keycode = VIRTUAL_KEY_DIR_UP; break;
	case 40: keycode = VIRTUAL_KEY_DIR_DOWN; break;
	case VK_SHIFT: keycode = VIRTUAL_KEY_SHIFT; break;
	case VK_CONTROL: keycode = VIRTUAL_KEY_CONTROL; break;
	case VK_ESCAPE:  keycode = VIRTUAL_KEY_BACK; break;

	default:
		if (keycode >= VK_F1 && keycode <= VK_F12)
		{
				keycode = VIRTUAL_KEY_F1+(keycode-VK_F1);
		}

	}

	return keycode;
}


void ChangeEmulationOrientationIfPossible(int desiredX, int desiredY, eOrientationMode desiredOrienation)
{
#ifdef _DEBUG
	if (GetKeyState(VK_CONTROL)& 0xfe)
	{
		if (GetForcedOrientation() != ORIENTATION_DONT_CARE)
		{
			LogMsg("Can't change orientation because SetForcedOrientation() is set.  Change to emulation of 'iPhone' instead of 'iPhone Landscape' for this to work.");
			return;
		}
	
		SetupScreenInfo(desiredX, desiredY, desiredOrienation);
	}
#endif
}


//0 signals bad key
int VKeyToWMCharKey(int vKey)
{
	if (vKey > 0 && vKey < 255)
	{
		static unsigned char  keystate[256];
		static HKL _gkey_layout = GetKeyboardLayout(0);

		int      result;
		unsigned short    val = 0;

		if (GetKeyboardState(keystate) == FALSE) return 0;
		result = ToAsciiEx(vKey,vKey,keystate,&val,0,_gkey_layout);

		
		if (result == 0)
		{
			val = vKey; //VK_1 etc don't get handled by the above thing.. or need to be
		}
#ifdef _DEBUG
		//LogMsg("Changing %d (%c) to %d (%c)", vKey, (char)vKey, val, (char)val);
#endif
		vKey = val;
	} else 
	{
		//out of range, ignore
		vKey = 0;
	}

	return vKey;
}

HGLRC		g_hRC=NULL;		// Permanent Rendering Context

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		// Handles the close message when a user clicks the quit icon of the window
	case WM_CLOSE:
		g_bAppFinished = true;
		
		//PostQuitMessage(0);
		return 1;


	case WM_PAINT:
		{

		//just tell windows we painted so it will shutup
			RECT rect;
			if (GetUpdateRect(g_hWnd, &rect, FALSE))
			{
				PAINTSTRUCT paint;
				BeginPaint(g_hWnd, &paint);
				EndPaint(g_hWnd, &paint);
			}
			//LogMsg("PAINT!");
  		   return true;
		}
	case WM_ACTIVATE:
		{
			return true;
		}

	case WM_KILLFOCUS:
#ifndef RT_RUNS_IN_BACKGROUND
		if (g_bHasFocus && IsBaseAppInitted() && g_hWnd)
		{
			GetBaseApp()->OnEnterBackground();
		}

		g_bHasFocus = false;
#endif
		break;

	case WM_SETFOCUS:
#ifndef RT_RUNS_IN_BACKGROUND
		if (!g_bHasFocus && IsBaseAppInitted() && g_hWnd)
		{
			GetBaseApp()->OnEnterForeground();
		}
		g_bHasFocus = true;
#endif
		break;

	case WM_SIZING:
		{
		
		if ( 
			(GetForceAspectRatioWhenResizing() && !(GetKeyState(VK_SHIFT)& 0xfe))
			||
			(!GetForceAspectRatioWhenResizing() && (GetKeyState(VK_SHIFT)& 0xfe) )
			)
		{

			float aspect_r=(float)GetPrimaryGLX()/(float)GetPrimaryGLY(); // aspect ratio

			if (GetFakePrimaryScreenSizeX() != 0)
			{
				//more reliable way to get the aspect ratio
				aspect_r=(float)GetFakePrimaryScreenSizeX()/(float)GetFakePrimaryScreenSizeY(); // aspect ratio
			}
			
			// difference between window and client size
			RECT rect, clientRect;

			GetWindowRect(g_hWnd, &rect);
			GetClientRect(g_hWnd, &clientRect);
			
			int add_x=(rect.right-rect.left)-clientRect.right; // bits we add like borders, title bar
			int add_y=(rect.bottom-rect.top)-clientRect.bottom;

			//LogMsg("Aspect ratio is %.2f. Border x:%d y:%d", aspect_r, add_x, add_y);

			LPRECT r=LPRECT(lParam);
				switch(wParam)
				{
				case WMSZ_LEFT:
				case WMSZ_BOTTOMLEFT:
				case WMSZ_BOTTOMRIGHT:
				case WMSZ_RIGHT:
					r->bottom=r->top+(LONG)((float)(r->right-r->left-add_x)/aspect_r)+add_y;
					break;
				case WMSZ_TOPRIGHT:
				case WMSZ_TOP:
				case WMSZ_BOTTOM:
					r->right=r->left+(LONG)((float)(r->bottom-r->top-add_y)*aspect_r)+add_x;
					break;
				case WMSZ_TOPLEFT:
					r->left=r->right-(LONG)((float)(r->bottom-r->top)*aspect_r)+add_x;
					break;
				}
			
			return TRUE;
			}
		}
		break;
	
	case WM_SIZE:
		{
	
			// Respond to the message:				
			int Width = LOWORD( lParam );
			int Height = HIWORD( lParam ); 
			
		
			if (Width != GetPrimaryGLX() || Height != GetPrimaryGLY())
			{
				
				//LogMsg("Got new size: %d, %d", Width, Height);
				GetBaseApp()->KillOSMessagesByType(OSMessage::MESSAGE_SET_VIDEO_MODE);
				GetBaseApp()->SetVideoMode(Width, Height, false, 0);
			
				if (GetFakePrimaryScreenSizeX() != 0)
				{
					//we're stretching the screen to fit, so make this look a little better
					//by drawing what we have during the drag operation.  Why does it only
					//draw when dragging bigger, not smaller?  GL surface thing?  Hrm.

					RECT rect;
					if (GetUpdateRect(g_hWnd, &rect, FALSE))
					{
						PAINTSTRUCT paint;
						BeginPaint(g_hWnd, &paint);
	#ifdef C_GL_MODE
						SwapBuffers(g_hDC);
	#endif
						EndPaint(g_hWnd, &paint);
					}
				}
			}
		}
		
		break;

	case WM_COMMAND:
		{
			if (LOWORD(wParam)==IDCANCEL)
			{
				LogMsg("WM command escape");
			}
		}
		break;

	case WM_CHAR:
		{

			if (!g_bHasFocus) break;

			const bool isBitSet = (lParam & (1 << 30)) != 0;

			//only register the first press of the escape key

			if (wParam == VK_ESCAPE) 
			{
				if (isBitSet) break;
				wParam = VIRTUAL_KEY_BACK;
			}

			#ifdef C_DONT_USE_WM_CHAR
				break;
			#endif

			//int vKey = ConvertWindowsKeycodeToProtonVirtualKey(wParam); 

			GetMessageManager()->SendGUI(MESSAGE_TYPE_GUI_CHAR, (float)wParam, (float)lParam);  //lParam holds a lot of random data about the press, look it up if
			//you actually want to access it
		}

		break;

	case WM_KEYDOWN:
		
		if (!g_bHasFocus) break;

	
		switch (wParam)
		{
	
		//case VK_ESCAPE:

			//g_escapeMessageSent = true;
		//	break;

		case VK_RETURN:
			{
				if (GetKeyState(VK_MENU)& 0xfe)
				{
					LogMsg("Toggle fullscreen from WM_KEYDOWN?, this should never happen");
					assert(0);
					return true;
				}
			}
		
			break;

		case 'L': //left landscape mode
			ChangeEmulationOrientationIfPossible(GetPrimaryGLY(), GetPrimaryGLX(), ORIENTATION_LANDSCAPE_LEFT);
			break;

		case 'R': //right landscape mode
			ChangeEmulationOrientationIfPossible(GetPrimaryGLY(), GetPrimaryGLX(), ORIENTATION_LANDSCAPE_RIGHT);
		
			break;

		case 'P': //portrait mode
			ChangeEmulationOrientationIfPossible(GetPrimaryGLX(), GetPrimaryGLY(), ORIENTATION_PORTRAIT);
			
			break;
		case 'U': //Upside down portrait mode
			ChangeEmulationOrientationIfPossible(GetPrimaryGLX(), GetPrimaryGLY(), ORIENTATION_PORTRAIT_UPSIDE_DOWN);
			break;

		case 'C':

			if (GetKeyState(VK_CONTROL)& 0xfe)
			{
				//LogMsg("Copy");
				GetMessageManager()->SendGUI(MESSAGE_TYPE_GUI_COPY, 0, 0);  //lParam holds a lot of random data about the press, look it up if
			}
			break;
		
		case 'V':

			if (GetKeyState(VK_CONTROL)& 0xfe)
			{
				//LogMsg("Paste");
				string text = GetClipboardText();

				if (!text.empty())
				{
					GetMessageManager()->SendGUI(MESSAGE_TYPE_GUI_PASTE, Variant(text), 0);  //lParam holds a lot of random data about the press, look it up if
				}
			}
		break;
		}

		{//this is to get around issue with the goto skipping the var init

		//send the raw key data as well
		VariantList v;
		const bool isBitSet = (lParam & (1 << 30)) != 0;

			if (!isBitSet)
			{
				int vKey = ConvertWindowsKeycodeToProtonVirtualKey(wParam); 
				GetMessageManager()->SendGUI(MESSAGE_TYPE_GUI_CHAR_RAW, (float)vKey, 1.0f);  

			#ifdef C_DONT_USE_WM_CHAR

			//also send as a normal key press.  Yes this convoluted.. it's done this way so Fkeys also go out as WM proton virtual keys to help with
		    //hotkeys and such.   -Seth

				int wmCharKey = VKeyToWMCharKey(wParam);
				if (wmCharKey != 0)
				{
					if (wmCharKey == 27)
					{
						wmCharKey = VIRTUAL_KEY_BACK; //we use this instead of escape for consistency across platforms
					}
					if (wmCharKey == wParam && wParam != 13 && wParam != 8)
					{
						//no conversion was done, it may need additional vkey processing
						if (wmCharKey >= VK_F1 &&wmCharKey <= VK_F24)
						{
							GetMessageManager()->SendGUIEx2(MESSAGE_TYPE_GUI_CHAR, (float)ConvertWindowsKeycodeToProtonVirtualKey(wmCharKey), 1.0f, 0, GetWinkeyModifiers());  
						} else
						{
							if (wmCharKey <= 90)
							{
								GetMessageManager()->SendGUIEx2(MESSAGE_TYPE_GUI_CHAR, (float)wmCharKey, 1.0f, 0, GetWinkeyModifiers());  
							}
						}

					} else
					{
						//normal
						GetMessageManager()->SendGUIEx2(MESSAGE_TYPE_GUI_CHAR, (float)wmCharKey, 1.0f, 0, GetWinkeyModifiers());  
					}
				}

			#endif
			}else
			{
				//repeat key
				#ifdef C_DONT_USE_WM_CHAR
				int vKey = ConvertWindowsKeycodeToProtonVirtualKey(wParam); 
				int wmCharKey = VKeyToWMCharKey(wParam);
				if (wmCharKey != 0)
				{
					//LogMsg("Sending repeat key..");
					GetMessageManager()->SendGUIEx2(MESSAGE_TYPE_GUI_CHAR, (float)wmCharKey, 1.0f, 0, GetWinkeyModifiers());  
				}

				#endif
			}

		} 
		break;
	

	case WM_KEYUP:
		{
#ifdef _DEBUG
		//	LogMsg("Got key up %d (%c)", (int)wParam, (char)wParam);
#endif
			uint32 key = ConvertWindowsKeycodeToProtonVirtualKey(wParam);

			/*
			if (key == VIRTUAL_KEY_BACK)
			{
				if (!g_escapeMessageSent)
				{
					//work around for not registering escape messages on my dev machine sometimes
					GetMessageManager()->SendGUI(MESSAGE_TYPE_GUI_CHAR, float(key) , float(1));  
					GetMessageManager()->SendGUI(MESSAGE_TYPE_GUI_CHAR_RAW, float(key) , float(1));  
				}
				//g_escapeMessageSent

				g_escapeMessageSent = false;
			}
			*/

			GetMessageManager()->SendGUIEx2(MESSAGE_TYPE_GUI_CHAR_RAW, float(key) , float(0), 0, GetWinkeyModifiers());  
		}
	break;
	
	case WM_SETCURSOR:
		{
			break;
		}
	case WM_SYSCOMMAND:
		// Do not allow screensaver to start. - wait, why would I want to disable the screensaver?!
		//if (wParam == SC_SCREENSAVE) return true;
		if ((wParam & 0xFFF0) == SC_MINIMIZE)
		{
			// shrink the application to the notification area
			// ...
			LogMsg("App minimized.");
			g_bIsMinimized = true;
			
		}

		if ((wParam & 0xFFF0) == SC_RESTORE)
		{
			// shrink the application to the notification area
			// ...
			LogMsg("App maximized");
			g_bIsMinimized = false;
		
		}
		break;

	case WM_LBUTTONDOWN:
	case WM_LBUTTONDBLCLK:
		{
			if (!g_bHasFocus) break;

			g_leftMouseButtonDown = true;
			int xPos = GET_X_LPARAM(lParam);
			int yPos = GET_Y_LPARAM(lParam);
			ConvertCoordinatesIfRequired(xPos, yPos);
			GetMessageManager()->SendGUIEx2(MESSAGE_TYPE_GUI_CLICK_START, (float)xPos, (float)yPos, 0, GetWinkeyModifiers());
			break;
		}
		break;

	case WM_LBUTTONUP:
		{
			if (!g_bHasFocus) break;
			int xPos = GET_X_LPARAM(lParam);
			int yPos = GET_Y_LPARAM(lParam);
			ConvertCoordinatesIfRequired(xPos, yPos);
			GetMessageManager()->SendGUIEx2(MESSAGE_TYPE_GUI_CLICK_END, (float)xPos, (float)yPos, 0, GetWinkeyModifiers());
			g_leftMouseButtonDown = false;
		}
		return true;
		break;


	case WM_RBUTTONDOWN:
	case WM_RBUTTONDBLCLK:
		{
			if (!g_bHasFocus) break;
			g_rightMouseButtonDown = true;
			int xPos = GET_X_LPARAM(lParam);
			int yPos = GET_Y_LPARAM(lParam);
			ConvertCoordinatesIfRequired(xPos, yPos);
			GetMessageManager()->SendGUIEx2(MESSAGE_TYPE_GUI_CLICK_START, (float)xPos, (float)yPos, 1, GetWinkeyModifiers());
			break;
		}
		break;

	case WM_RBUTTONUP:
		{
			if (!g_bHasFocus) break;
			int xPos = GET_X_LPARAM(lParam);
			int yPos = GET_Y_LPARAM(lParam);
			ConvertCoordinatesIfRequired(xPos, yPos);
			GetMessageManager()->SendGUIEx2(MESSAGE_TYPE_GUI_CLICK_END, (float)xPos, (float)yPos, 1, GetWinkeyModifiers());
			g_rightMouseButtonDown = false;
		}
		return true;
		break;
	
	case WM_MOUSEMOVE:
		{
			if (!g_bHasFocus) break;
		
			float xPos = (float)GET_X_LPARAM(lParam);
			float yPos = (float)GET_Y_LPARAM(lParam);
			ConvertCoordinatesIfRequired(xPos, yPos);
	
			if (g_leftMouseButtonDown)
			{
				GetMessageManager()->SendGUIEx2(MESSAGE_TYPE_GUI_CLICK_MOVE, xPos, yPos, 0, GetWinkeyModifiers());
			} 
		
			if (g_rightMouseButtonDown)
			{
				GetMessageManager()->SendGUIEx2(MESSAGE_TYPE_GUI_CLICK_MOVE, xPos, yPos, 1, GetWinkeyModifiers());
			} 

			GetMessageManager()->SendGUIEx2(MESSAGE_TYPE_GUI_CLICK_MOVE_RAW, xPos, yPos, 0, GetWinkeyModifiers());
		}
		//sreturn true;

		break;

	case WM_CANCELMODE:

		LogMsg("Got WM cancel mode");
		break;
	
	default:
		break;
	}

	// Calls the default window procedure for messages we did not handle
	return DefWindowProc(hWnd, message, wParam, lParam);
}

bool TestEGLError(HWND hWnd, char* pszLocation)
{

	EGLint iErr = eglGetError();
	if (iErr != EGL_SUCCESS)
	{
		TCHAR pszStr[256];
		_stprintf_s(pszStr, _T("%s failed (%d).\n"), pszLocation, iErr);
		MessageBox(hWnd, pszStr, _T("Error"), MB_OK|MB_ICONEXCLAMATION);
		return false;
	}

	return true;
}

void CenterWindow(HWND hWnd)
{
	RECT r, desk;
	GetWindowRect(hWnd, &r);
	GetWindowRect(GetDesktopWindow(), &desk);

	int wa,ha,wb,hb;

	wa = (r.right - r.left) / 2;
	ha = (r.bottom - r.top) / 2;

	wb = (desk.right - desk.left) / 2;
	hb = (desk.bottom - desk.top) / 2;

	SetWindowPos(hWnd, NULL, wb - wa, hb - ha, r.right - r.left, r.bottom - r.top, 0); 

}

bool InitVideo(int width, int height, bool bFullscreen, float aspectRatio)
{
	LogMsg("Setting native video mode to %d, %d - Fullscreen: %d  Aspect Ratio: %.2f", width, height, int(bFullscreen), aspectRatio);
	g_winVideoScreenY = height;
	g_winVideoScreenX = width;

	// EGL variables
#ifndef C_GL_MODE
	EGLConfig			eglConfig	= 0;
	EGLContext			eglContext	= 0;
	NativeWindowType	eglWindow	= 0;
	EGLint				pi32ConfigAttribs[128];
	int				i;
#else
	int bits = 16;
	GLuint		PixelFormat;			// Holds The Results After Searching For A Match

	static	PIXELFORMATDESCRIPTOR pfd=				// pfd Tells Windows How We Want Things To Be
	{
		sizeof(PIXELFORMATDESCRIPTOR),				// Size Of This Pixel Format Descriptor
		1,											// Version Number
		PFD_DRAW_TO_WINDOW |						// Format Must Support Window
		PFD_SUPPORT_OPENGL |						// Format Must Support OpenGL
		PFD_DOUBLEBUFFER,							// Must Support Double Buffering
		PFD_TYPE_RGBA,								// Request An RGBA Format
		bits,										// Select Our Color Depth
		0, 0, 0, 0, 0, 0,							// Color Bits Ignored
		0,											// No Alpha Buffer
		0,											// Shift Bit Ignored
		0,											// No Accumulation Buffer
		0, 0, 0, 0,									// Accumulation Bits Ignored
		16,											// 16Bit Z-Buffer (Depth Buffer)  
		0,											// No Stencil Buffer
		0,											// No Auxiliary Buffer
		PFD_MAIN_PLANE,								// Main Drawing Layer
		0,											// Reserved
		0, 0, 0										// Layer Masks Ignored
	};

#endif

	RECT	sRect;
	SetRect(&sRect, 0, 0, width, height);
	//for taking screenshots with no borders with Alt-Print screen, try this:
	
	DWORD style = WS_POPUP | WS_SYSMENU | WS_CAPTION | CS_DBLCLKS;
	
#ifdef C_BORDERLESS_WINDOW_MODE_FOR_SCREENSHOT_EASE
	style = WS_POPUP | CS_DBLCLKS;
#endif
	
#ifndef C_BORDERLESS_WINDOW_MODE_FOR_SCREENSHOT_EASE
	if (g_winAllowWindowResize)
	{
		style = style |WS_SIZEBOX | WS_MAXIMIZEBOX | WS_MINIMIZEBOX ;
	}
#endif
	if (bFullscreen)
	{
		//actually, do it this way:
		style = WS_POPUP;
	}

	g_bIsFullScreen = bFullscreen;
	DWORD ex_style = 0;

	AdjustWindowRectEx(&sRect, style, false, ex_style);
	
	g_bHasFocus = true;

	bool bCenterWindow = false;

	if (g_hWnd)
	{
		RECT clientRect;
		GetClientRect(g_hWnd, &clientRect);
	
		if (clientRect.right != width || clientRect.bottom != height)
		{
			//we'll need to actually recreate this
			DestroyWindow(g_hWnd);
			g_hWnd = NULL;
		}
	}

	if (!g_hWnd)
	{
		bCenterWindow = true;

		g_hWnd = CreateWindowEx(
		ex_style,
		WINDOW_CLASS,
		GetAppName(),
		style,
		0,
		0, 
		sRect.right-sRect.left,
		sRect.bottom-sRect.top,
		NULL,
		NULL,
		g_hInstance,
		NULL);
	} else
	{
		SetWindowLong(g_hWnd, GWL_STYLE, style);
	}
	
assert(!g_hDC);

#ifndef C_GL_MODE
	eglWindow = g_hWnd;
#endif

	// Get the associated device context
	g_hDC = GetDC(g_hWnd);
	if (!g_hDC)
	{
		MessageBox(0, _T("Failed to create the device context"), _T("Error"), MB_OK|MB_ICONEXCLAMATION);
		return false;
	}

#ifndef C_GL_MODE
	g_eglDisplay = eglGetDisplay((NativeDisplayType) g_hDC);

	if(g_eglDisplay == EGL_NO_DISPLAY)
		g_eglDisplay = eglGetDisplay((NativeDisplayType) EGL_DEFAULT_DISPLAY);

	EGLint iMajorVersion, iMinorVersion;
	if (!eglInitialize(g_eglDisplay, &iMajorVersion, &iMinorVersion))
	{

		MessageBox(0, _T("eglInitialize() failed."), _T("Error"), MB_OK|MB_ICONEXCLAMATION);

		return false;
	}

	i = 0;
	pi32ConfigAttribs[i++] = EGL_RED_SIZE;
	pi32ConfigAttribs[i++] = 5;
	pi32ConfigAttribs[i++] = EGL_GREEN_SIZE;
	pi32ConfigAttribs[i++] = 6;
	pi32ConfigAttribs[i++] = EGL_BLUE_SIZE;
	pi32ConfigAttribs[i++] = 5;
	pi32ConfigAttribs[i++] = EGL_ALPHA_SIZE;
	pi32ConfigAttribs[i++] = 0;
	pi32ConfigAttribs[i++] = EGL_SURFACE_TYPE;
	pi32ConfigAttribs[i++] = EGL_WINDOW_BIT;
	
	//Hmm, seems to ignore this.  Too bad, I'd like to test with smaller depth buffers sometimes.
	/*
	pi32ConfigAttribs[i++] = EGL_DEPTH_SIZE;
	pi32ConfigAttribs[i++] = 16;
	*/
	
	pi32ConfigAttribs[i++] = EGL_NONE;

	int iConfigs;
	if (!eglChooseConfig(g_eglDisplay, pi32ConfigAttribs, &eglConfig, 1, &iConfigs) || (iConfigs != 1))
	{
		MessageBox(0, _T("eglChooseConfig() failed."), _T("Error"), MB_OK|MB_ICONEXCLAMATION);
		return false;
	}

	g_eglSurface = eglCreateWindowSurface(g_eglDisplay, eglConfig, eglWindow, NULL);

	if(g_eglSurface == EGL_NO_SURFACE)
	{
		eglGetError(); // Clear error
		g_eglSurface = eglCreateWindowSurface(g_eglDisplay, eglConfig, NULL, NULL);
	}

	if (!TestEGLError(g_hWnd, "eglCreateWindowSurface"))
	{
		return false;
	}

	eglContext = eglCreateContext(g_eglDisplay, eglConfig, NULL, NULL);
	if (!TestEGLError(g_hWnd, "eglCreateContext"))
	{
		return false;
	}

	eglMakeCurrent(g_eglDisplay, g_eglSurface, g_eglSurface, eglContext);
	if (!TestEGLError(g_hWnd, "eglMakeCurrent"))
	{
		return false;
	}
		/*
	
	GLuint viewFramebuffer;
	GLuint textureFrameBuffer;

	glGenFramebuffersOES(1, &textureFrameBuffer);
*/
#else

	//NORMAL GL INIT

	
	if (!(PixelFormat=ChoosePixelFormat(g_hDC,&pfd)))	// Did Windows Find A Matching Pixel Format?
	{
		MessageBox(NULL,"Can't Find A Suitable PixelFormat.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	if(!SetPixelFormat(g_hDC,PixelFormat,&pfd))		// Are We Able To Set The Pixel Format?
	{
		MessageBox(NULL,"Can't Set The PixelFormat.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	if (!(g_hRC=wglCreateContext(g_hDC)))				// Are We Able To Get A Rendering Context?
	{
		MessageBox(NULL,"Can't Create A GL Rendering Context.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	if(!wglMakeCurrent(g_hDC,g_hRC))					// Try To Activate The Rendering Context
	{
		MessageBox(NULL,"Can't Activate The GL Rendering Context.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}
#endif
	GetBaseApp()->InitializeGLDefaults();

	if (!bFullscreen && bCenterWindow)
	{
		CenterWindow(g_hWnd);
	}
	ShowWindow(g_hWnd, SW_SHOW);
	
	SetupScreenInfo(GetPrimaryGLX(), GetPrimaryGLY(), GetOrientation());

	//UpdateWindow(g_hWnd);
	//RedrawWindow(0, 0, 0, RDW_ALLCHILDREN|RDW_INVALIDATE|RDW_UPDATENOW);
	return true;
}

void DestroyVideo(bool bDestroyHWNDAlso)
{

#ifdef C_GL_MODE

	if (g_hRC)											// Do We Have A Rendering Context?
	{
		if (!wglMakeCurrent(NULL,NULL))					// Are We Able To Release The DC And RC Contexts?
		{
			MessageBox(NULL,"Release Of DC And RC Failed.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		}

		if (!wglDeleteContext(g_hRC))						// Are We Able To Delete The RC?
		{
			MessageBox(NULL,"Release Rendering Context Failed.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		}
		g_hRC=NULL;										// Set RC To NULL
	}

#else

	eglMakeCurrent(g_eglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
	eglTerminate(g_eglDisplay);
	
#endif

	if (g_hDC && !ReleaseDC(g_hWnd,g_hDC))					// Are We Able To Release The DC
	{
		MessageBox(NULL,"Release Device Context Failed.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		g_hDC=NULL;										// Set DC To NULL
	}
	g_hDC = NULL;

	if (bDestroyHWNDAlso)
	{

		if (g_hWnd && !DestroyWindow(g_hWnd))					// Are We Able To Destroy The Window?
		{
			MessageBox(NULL,"Could Not Release hWnd.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
			g_hWnd=NULL;										// Set hWnd To NULL
		}
		g_hWnd = NULL;
	}
}


string GetExePath()
{
	// Get path to executable:
	TCHAR szDllName[_MAX_PATH];
	TCHAR szDrive[_MAX_DRIVE];
	TCHAR szDir[_MAX_DIR];
	TCHAR szFilename[256];
	TCHAR szExt[256];
	GetModuleFileName(0, szDllName, _MAX_PATH);
	_splitpath(szDllName, szDrive, szDir, szFilename, szExt);

	return string(szDrive) + string(szDir); 
}

void ForceVideoUpdate()
{
	g_globalBatcher.Flush();

#ifdef C_GL_MODE
	SwapBuffers(g_hDC);
#else
	eglSwapBuffers(g_eglDisplay, g_eglSurface);
#endif
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, TCHAR *lpCmdLine, int nCmdShow)
{
	
#ifdef WIN32
	
	//I don't *think* we need this...
	::SetProcessAffinityMask( ::GetCurrentProcess(), 1 );
	SetDoubleClickTime(0);
#endif

	//first make sure our working directory is the .exe dir
	 _chdir(GetExePath().c_str());
	
	g_hInstance = hInstance;
	RemoveFile("log.txt", false);

	if (lpCmdLine[0])
	{
		vector<string> parms = StringTokenize(lpCmdLine, " ");
	
		for (unsigned int i=0; i < parms.size(); i++)
		{
			GetBaseApp()->AddCommandLineParm(parms[i]);
		}
	}
	
	InitVideoSize();

	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	srand( (unsigned)GetTickCount() );

	// Register the windows class
	WNDCLASS sWC;
	sWC.style = CS_DBLCLKS;
	sWC.lpfnWndProc = WndProc;
	sWC.cbClsExtra = 0;
	sWC.cbWndExtra = 0;
	sWC.hInstance = hInstance;
	sWC.hIcon = 0;
	sWC.hCursor = LoadCursor (NULL,IDC_ARROW);;
	sWC.lpszMenuName = 0;
	sWC.hbrBackground = (HBRUSH) GetStockObject(BLACK_BRUSH);
	sWC.lpszClassName = WINDOW_CLASS;
	RegisterClass(&sWC);
	
	/*
	if (!registerClass)
	{
		MessageBox(0, _T("Failed to register the window class"), _T("Error"), MB_OK | MB_ICONEXCLAMATION);
	}
	*/

#ifdef RT_FLASH_TEST
	GLFlashAdaptor_Initialize();
#endif

	if (!InitVideo(GetPrimaryGLX(), GetPrimaryGLY(), false, 0))
	{
		goto cleanup;
	}



	if (!GetBaseApp()->Init())
	{
		assert(!"Unable to init - did you run media/update_media.bat to build the resources?");
		MessageBox(NULL, "Error initializing the game.  Did you unzip everything right?", "Unable to load stuff", NULL);
		goto cleanup;
	}



#ifdef C_GL_MODE
	if (!g_glesExt.InitExtensions())
	{
		MessageBox(NULL, "Error initializing GL extensions. Update your GL drivers!", "Missing GL Extensions", NULL);
		goto cleanup;
	}
#endif

	//our main loop
	static float fpsTimer=0;

	while(1)
	{
		
		if (GetAsyncKeyState('Q') && GetAsyncKeyState(VK_MENU))
		{
			SendMessage(g_hWnd, WM_CLOSE, 0, 0);
		}

		if (g_winAllowFullscreenToggle)
		{
			if (GetAsyncKeyState(VK_RETURN) && GetAsyncKeyState(VK_MENU))
			{
				LogMsg("Toggle fullscreen");
				GetMessageManager()->SendGUI(MESSAGE_TYPE_GUI_TOGGLE_FULLSCREEN, 0, 0);  //lParam holds a lot of random data about the press, look it up if
				//return true;
			}

		}

		if (g_bAppFinished) break;

		if (g_bHasFocus)
		{
			GetBaseApp()->Update();
			if (!g_bIsMinimized)
				GetBaseApp()->Draw();
		} else
		{
			//LogMsg("Sleeping");
			Sleep(50);
		}

		if (g_fpsLimit != 0)
		{
			while (fpsTimer > GetSystemTimeAccurate())
			{
				Sleep(0);
			}
			fpsTimer = float(GetSystemTimeAccurate())+(1000.0f/ (float(g_fpsLimit)));
		}

		while (!GetBaseApp()->GetOSMessages()->empty())
		{
			OSMessage m = GetBaseApp()->GetOSMessages()->front();
			GetBaseApp()->GetOSMessages()->pop_front();
			//LogMsg("Got OS message %d, %s", m.m_type, m.m_string.c_str());

			switch (m.m_type)
			{
			case OSMessage::MESSAGE_CHECK_CONNECTION:
				//pretend we did it
				GetMessageManager()->SendGUI(MESSAGE_TYPE_OS_CONNECTION_CHECKED, RT_kCFStreamEventOpenCompleted, 0);	
				break;
			case OSMessage::MESSAGE_OPEN_TEXT_BOX:
				break;
			case OSMessage::MESSAGE_CLOSE_TEXT_BOX:
				SetIsUsingNativeUI(false);
				break;
			
			case OSMessage::MESSAGE_FINISH_APP:
			case OSMessage::MESSAGE_SUSPEND_TO_HOME_SCREEN:

				PostMessage(g_hWnd, WM_CLOSE, 0, 0);
				break;
			
			case OSMessage::MESSAGE_SET_FPS_LIMIT:
				g_fpsLimit = int(m.m_x);
				
				break;
			case OSMessage::MESSAGE_SET_VIDEO_MODE:
			
				SwapBuffers(g_hDC);
				GetBaseApp()->Draw();

				g_bHasFocus = true;
				
				if (g_bIsMinimized) goto skipRender; //we don't need to re-init anything, we're just minimized

				GetBaseApp()->OnEnterBackground();
				GetBaseApp()->m_sig_unloadSurfaces();
#ifdef C_GL_MODE
				DestroyVideo(false);
			
				g_bHasFocus = false;
				if (!InitVideo(int(m.m_x), int(m.m_y), m.m_fullscreen, m.m_fontSize))
				{
					MessageBox(NULL, "Error changing video mode", "Error", NULL);
					goto cleanup;
				}
#else
				LogMsg("Ignoring SET_VIDEO_MODE, only setup to work with normal GL, not GLES");
#endif
				SetupOrtho();
				GetBaseApp()->OnEnterForeground();
				GetBaseApp()->m_sig_loadSurfaces();
				
				goto skipRender;
				//continue;
			}
		}
	
		if (g_bHasFocus && !g_bIsMinimized)
		{
	#ifdef C_GL_MODE
			SwapBuffers(g_hDC);
	#else
			eglSwapBuffers(g_eglDisplay, g_eglSurface);
			if (!TestEGLError(g_hWnd, "eglSwapBuffers"))
			{
				goto cleanup;
			}
	#endif
		}

skipRender:
		// Managing the window messages
		MSG msg;
		while (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE) == TRUE)
		{
			int ret = GetMessage(&msg, NULL, 0, 0);
			if (ret > 0)
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			else
			{
				LogMsg("Error?");
			}
		}
		Sleep(0);
	}

cleanup:

	if (IsBaseAppInitted())
	{
		GetBaseApp()->OnEnterBackground();
		GetBaseApp()->Kill();
	}

	DestroyVideo(true);

	WSACleanup(); 
	return 0;
}


void AddText(const char *tex ,char *filename)
{
	FILE *          fp;
	if (strlen(tex) < 1) return;

	if (FileExists(filename) == false)
	{

		fp = fopen( (GetBaseAppPath()+filename).c_str(), "wb");
		if (!fp) return;
		fwrite( tex, strlen(tex), 1, fp);      
		fclose(fp);
		return;
	} else
	{
		fp = fopen( (GetBaseAppPath()+filename).c_str(), "ab");
		if (!fp) return;
		fwrite( tex, strlen(tex), 1, fp);      
		fclose(fp);
	}
}

void LogMsg ( const char* traceStr, ... )
{
	va_list argsVA;
	const int logSize = 1024*10;
	char buffer[logSize];
	memset ( (void*)buffer, 0, logSize );

	va_start ( argsVA, traceStr );
	vsnprintf_s( buffer, logSize, logSize, traceStr, argsVA );
	va_end( argsVA );
	

	OutputDebugString(buffer);
	OutputDebugString("\n");

	if (IsBaseAppInitted())
	{
		GetBaseApp()->GetConsole()->AddLine(buffer);
		strcat(buffer, "\r\n");
		AddText(buffer, "log.txt");
	}

}

#endif