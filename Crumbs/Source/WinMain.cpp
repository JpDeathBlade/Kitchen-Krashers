/**
 * 0===========================================================================0
 * | WinMain.cpp                                                               |
 * |  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  |
 * |   Author        : Joseph Stuhr                                            |
 * |   E-mail        : JpDeathBlade@gmail.com                                  |
 * |  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  |
 * |   Date Created  : 09:27 pm (Eastern) on 05/21/2010                        |
 * |   Date Edited   : 02:09 pm (Eastern) on 05/22/2010                        |
 * 0=====0===============================================================0=====0
 *       |                                                               |      
 * 0=====0===============================================================0=====0
 * | Code Description:                                                         |
 * |  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  |
 * |   The main entry point of the Win32 Application.                          |
 * 0===========================================================================0
 */

// 0===========================================================================0
// | ~ Defines ~                                                               |
// 0===========================================================================0
#define WIN32_LEAN_AND_MEAN		// Removes rarely-used things from the headers
#define VC_EXTRALEAN

// 0===========================================================================0
// | ~ Includes ~                                                              |
// 0===========================================================================0
#include <windows.h>
#include <stdio.h>
#include "time.h"
#include "../resource.h" // For Icon ID

// 0===========================================================================0
// | Include VLD when we are debugging to find memory leaks.                   |
// 0===========================================================================0
#if defined(DEBUG) | defined(_DEBUG)
	#define VLD_AGGREGATE_DUPLICATES	// Disables duplicate leaks
	//#include <vld.h>					// Visual Leak Detector
	#define D3D_DEBUG_INFO				// Enable Direct3D debugging
#endif

#include "RJ_Config.h"					// Internal Config file for default variables
#include "CoreTech\\RJ_GameMain.h"			// The main games entry point

#include "CoreTech\\Utilities\\Debugger\\RJ_Debugger.h"
#include "CoreTech\\Utilities\\CodeProfiler\\RJ_CodeProfiler.h"

// 0===========================================================================0
// | ~ Prototypes ~                                                            |
// 0===========================================================================0
int MessageLoop(HWND _hWnd);
LRESULT CALLBACK MainWndProc(HWND _hWnd, UINT _unMessage, WPARAM _wParam, LPARAM _lParam);

// 0===========================================================================0
// | WinMain                                                                   |
// |  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  |
// | Author: Joseph Stuhr (JS)                                                 |
// | Last Edit: 05/21/10                                                       |
// |  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  |
// | In:  _hInstance                                                           |
// |      _hPrevInstance                                                       |
// |      _lpCmdLine                                                           |
// |      _nCmdShow                                                            |
// |                                                                           |
// | Out: _wParam                                                              |
// |  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  |
// | Purpose: Creates a Win32 Application window. Also initalizes and runs     |
// |  our game.                                                                |
// 0===========================================================================0
int APIENTRY WinMain(HINSTANCE _hInstance, HINSTANCE _hPrevInstance, LPSTR _lpCmdLine, int _nCmdShow)
{
	// seed rand()
	srand( unsigned int (time(0)) );

	// Initialize the code profiler
	//RJ_CODEPROFILER->InitCodeProfiler();

	// Testing for the debugger functions.
	//RJ_DEBUGGER->Error(RJ_Debugger::RJ_FATAL,"Testing Fatal Message");
	//RJ_DEBUGGER->Error(RJ_Debugger::RJ_ALERT,"Testing Alert Message");
	//RJ_DEBUGGER->Error(RJ_Debugger::RJ_CRIT,"Testing Critical Message");
	//RJ_DEBUGGER->Error(RJ_Debugger::RJ_ERROR,"Testing Error Message");
	//RJ_DEBUGGER->Error(RJ_Debugger::RJ_WARN,"Testing Warning Message");
	//RJ_DEBUGGER->Error(RJ_Debugger::RJ_NOTICE,"Testing Notice Message");
	//RJ_DEBUGGER->Error(RJ_Debugger::RJ_INFO,"Testing Info Message");
	//RJ_DEBUGGER->Error(RJ_Debugger::RJ_DEBUG,"Testing Debug Message");
	//RJ_DEBUGGER->Error(RJ_Debugger::RJ_NOTSET,"Testing NotSet Message");
	//RJ_DEBUGGER->Error(RJ_Debugger::RJ_UNKNOWN,"Testing Unknown Message");

	// Set the threshold for the debugger
	RJ_DEBUGGER->SetThreshold(RJ_Debugger::RJ_DEBUG);

	// Create a WindowParameters variable and fill it out with the defaults from the internal config
	RJ_GameMain::WindowParameters tWinParams;
		tWinParams.t_ApplicationInstance		= _hInstance;
		tWinParams.t_uiClientWidth				= RJ_CONFIG_CLIENT_WIDTH;
		tWinParams.t_uiClientHeight				= RJ_CONFIG_CLIENT_HEIGHT;
		tWinParams.t_uiPositionX				= RJ_CONFIG_CLIENT_XPOSITION;
		tWinParams.t_uiPositionY				= RJ_CONFIG_CLIENT_YPOSITION;
		tWinParams.t_bFullScreen				= RJ_CONFIG_CLIENT_FULLSCREEN;
		tWinParams.t_bVSync						= RJ_CONFIG_CLIENT_VSYNC;

	// Initialize the window variables
	DEVMODE	DevMode;
	if(tWinParams.t_bFullScreen)
	{
		DevMode.dmPelsHeight	= tWinParams.t_uiClientHeight;
		DevMode.dmPelsWidth		= tWinParams.t_uiClientWidth;
		DevMode.dmSize			= sizeof(DevMode);
		DevMode.dmFields		= DM_PELSWIDTH | DM_PELSHEIGHT;

		if(ChangeDisplaySettings(&DevMode, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL)
		{
			MessageBox(NULL, TEXT("WinMain: Cannot change to selected desktop resolution."),
							  NULL, MB_OK | MB_ICONSTOP);
			RJ_DEBUGGER->Error(RJ_Debugger::RJ_FATAL,"Cannot change to selected desktop resolution.");
			return -1;
		}
	}

	// The struct that holds information for the window class
		WNDCLASSEX winClass;
	// Clear out the window class
		ZeroMemory(&winClass, sizeof(winClass));

	// Fill in the struct with the needed information
		winClass.cbSize			= sizeof(winClass); 
		winClass.style			= CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
		winClass.lpfnWndProc	= (WNDPROC)MainWndProc;
		winClass.cbClsExtra		= 0;
		winClass.cbWndExtra		= 0;
		winClass.hInstance		= _hInstance;
		winClass.hIcon			= LoadIcon(_hInstance, MAKEINTRESOURCE(IDI_ICON1));
		winClass.hIconSm		= NULL;
		winClass.hCursor		= (HCURSOR)LoadCursor(NULL, IDC_ARROW);
		winClass.hbrBackground	= (HBRUSH)COLOR_WINDOW;
		winClass.lpszMenuName	= NULL;
		winClass.lpszClassName	= TEXT(RJ_CONFIG_CLIENT_CLASSNAME);

	// Register the window class
		if(!RegisterClassEx(&winClass))
		{
			MessageBox(NULL, TEXT("WinMain: RegisterClass failed."),
							  NULL, MB_OK | MB_ICONSTOP);
			RJ_DEBUGGER->Error(RJ_Debugger::RJ_FATAL,"RegisterClassEX failed.");
			return -1;
		}
		RJ_DEBUGGER->Error(RJ_Debugger::RJ_DEBUG,"Window Class Registered!");

	// Select window styles
		UINT uiStyle;
		if(tWinParams.t_bFullScreen)
		{
			uiStyle = WS_POPUP;
			tWinParams.t_uiPositionX	= 0;
			tWinParams.t_uiPositionY	= 0;				
		}
		else
		{
			uiStyle = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX;
		}

	// Create the window and use the result as the handle
		HWND hWnd = CreateWindowEx(0,
							  TEXT(RJ_CONFIG_CLIENT_CLASSNAME),		// name of the window class
							  TEXT(RJ_CONFIG_CLIENT_TITLE),			// title of the window
							  uiStyle,								// window style
		(GetSystemMetrics(SM_CXSCREEN)>>1) - (tWinParams.t_uiClientWidth>>1),				// x-position of the window
		(GetSystemMetrics(SM_CYSCREEN)>>1) - (tWinParams.t_uiClientHeight>>1),				// y-position of the window
							  tWinParams.t_uiClientWidth,			// width of the window
							  tWinParams.t_uiClientHeight,			// height of the window
							  NULL,									// we have no parent window, NULL
							  NULL,									// we aren't using menus, NULL
							  _hInstance,							// application handle
							  NULL);								// used with multiple windows, NULL

		if(!hWnd)
		{
			MessageBox(NULL, TEXT("WinMain: hWnd was not created."),
							  NULL, MB_OK | MB_ICONSTOP);
			RJ_DEBUGGER->Error(RJ_Debugger::RJ_FATAL,"hWnd was not created.");
			return -1;
		}
		tWinParams.t_Window = hWnd;
		RJ_DEBUGGER->Error(RJ_Debugger::RJ_DEBUG,"hWnd Created!");

		ShowCursor(false);

		// Display the window on the screen
		ShowWindow(hWnd, _nCmdShow);
		UpdateWindow(hWnd);

	////////////////////////////////////
	// Initialize the Game
	////////////////////////////////////
	//{
		if(RJ_MAIN_GAME->Initialize(tWinParams))
		{
			RJ_DEBUGGER->Error(RJ_Debugger::RJ_DEBUG,"GameMain Initialized!");
		}
		else
		{
			PostQuitMessage(0);
		}
	//}

		return MessageLoop(hWnd);
}

int MessageLoop(HWND _hWnd)
{
	// Enter the main loop of the program:
	//{
		// This struct holds Windows event messages
		MSG msg;
		bool bRunning = true;
		// Wait for the next message in the queue
		while(bRunning)
		{
			if(PeekMessage(&msg, 0, 0, 0, PM_REMOVE) != 0)
			{
				if(msg.message == WM_QUIT)
					bRunning = false;

				TranslateMessage(&msg);	// Translate keystroke messages into the right format
				DispatchMessage(&msg);	// Send the message to the WindowProc
			}
			if(!RJ_MAIN_GAME->Run())
			{
				PostQuitMessage(0);
			}
		}

		////////////////////////////////////
		// Shutdown the Game
		////////////////////////////////////
		//{
			RJ_MAIN_GAME->Shutdown();
			//RJ_CODEPROFILER->ShutdownCodeProfiler();
			DestroyWindow(_hWnd);
		//}
		////////////////////////////////////

		if(RJ_MAIN_GAME->getWindowParameters().t_bFullScreen)
		{
			ChangeDisplaySettings(NULL, 0);
		}
		PostQuitMessage(0);
		// Return this part of the WM_Quit message to Windows
		return msg.wParam;
	//}
}

LRESULT CALLBACK MainWndProc(HWND _hWnd, UINT _unMessage, WPARAM _wParam, LPARAM _lParam)
{
	switch(_unMessage)
	{
		// This message is read when the window is closed
			case WM_CLOSE:
			{
				// Shut down the CodeProfiler as the last thing done
				//RJ_CODEPROFILER->ShutdownCodeProfiler();
				DestroyWindow(_hWnd);
			} break;

		// Window is destroyed, it's the end of the application
			case WM_DESTROY:
			{
				PostQuitMessage(0);
			} break;

			case WM_SIZE:
			{
				//RJ_MAIN_GAME->ChangeDisplay(LOWORD(_lParam), HIWORD(_lParam), RJ_MAIN_GAME->getFullScreen());
			} break;

			case WM_KEYDOWN:
			{
				//if( _wParam == VK_ESCAPE )
				//	RJ_MAIN_GAME->GoWindowed();
				//if( _wParam == 'F' )
				//	RJ_MAIN_GAME->GoFullScreen();

				/*if( (_wParam & VK_LMENU || _wParam & VK_RMENU) && _wParam & VK_RETURN ) 
				{
					RJ_MAIN_GAME->ToggleFullScreen();
				}*/
			} break;

		// This message is read when the program is active
			case WM_ACTIVATE:
			{
				// Check to see if the window has focus
				if (LOWORD(_wParam) == WA_INACTIVE)
				{
					////////////////////////////////////
					// Pause the game
					////////////////////////////////////
					//{
						RJ_MAIN_GAME->setFocus(false);
					//}
					////////////////////////////////////
				}
				else
				{
					////////////////////////////////////
					// Unpause the game
					////////////////////////////////////
					//{
						RJ_MAIN_GAME->setFocus(true);
					//}
					////////////////////////////////////
				}
			} break;

		// Handle any messages the switch statement didn't
			default:
			{
				return DefWindowProc(_hWnd, _unMessage, _wParam, _lParam);
			} break;
	}

	return 0;
}

// 0===========================================================================0
// | main                                                                      |
// |  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  |
// | Author: Joseph Stuhr (JS)                                                 |
// | Last Edit: 05/22/10                                                       |
// |  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  |
// | In:  N/A                                                                  |
// |                                                                           |
// | Out: 0                                                                    |
// |  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  |
// | Purpose: A Win32 console to help with debugging.                          |
// 0===========================================================================0
#if defined(DEBUG) | defined(_DEBUG)
	int main(void)
	{
		WinMain(GetModuleHandle(NULL), NULL, (LPSTR)GetCommandLine(), SW_SHOWDEFAULT);

		//system("PAUSE");
		return 0;
	}
#endif