#include "CInputManager.h"
#include "../Event/EventManager.h"
//#include "Cursor.h"
#include "../../RJ_GameMain.h"

CInputManager* CInputManager::GetInstanceRender(void)
{
	static CInputManager instance;
	return &instance;
}
CInputManager::CInputManager()
{
	//cur = Cursor::GetInstanceRender();
	m_pDI = NULL;
	ClearBufferedData();
}
CInputManager::~CInputManager()
{

}
bool CInputManager::Init(HINSTANCE ApplicationInstance, HWND WindowInstance, unsigned int nInputDevice)
{
	HRESULT InitResult = DirectInput8Create(ApplicationInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_pDI, 0);
	if(InitResult == DI_OK)
	{
		if (nInputDevice & RJ_Config::RJ_INPUT_KEYBOARD)
		{
			m_pDI->CreateDevice(GUID_SysKeyboard, &m_pKeyboard, NULL);
			m_pKeyboard->SetDataFormat(&c_dfDIKeyboard);
			m_pKeyboard->SetCooperativeLevel(WindowInstance, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
			m_pKeyboard->Acquire(); 
		}
		/*if (nInputDevice & RJ_Config::RJ_INPUT_MOUSE)
		{
			m_pDI->CreateDevice(GUID_SysMouse, &m_pMouse, NULL);
			m_pMouse->SetDataFormat(&c_dfDIMouse2);
			m_pMouse->SetCooperativeLevel(WindowInstance, DISCL_FOREGROUND | DISCL_EXCLUSIVE);
			m_pMouse->Acquire();
		}*/
	}
	else
		return false;
	return true;
}
bool CInputManager::Shutdown()
{
	return true;
}
void CInputManager::Update(double _fdelta)
{
	PollKeyboard();
	//PollMouse();
	//cur->Render();
	ReadBufferedDevice();
	if(RJ_GameMain::GetInstance()->getLoadingDone() && RJ_GameMain::GetInstance()->getSplashDone())
	{
		//DO NOT TOUCH THE INFORMATION YOU ARE ABOUT TO POINT TO AGAIN WILL CAUSE A THREAD CRASH
		volatile tInput* tINPUT = new tInput(EM_INPUT, m_n64KeysDown,m_n64KeysPressed,0.0f,0.0f); 
		CEventManager::GetInstanceThreaded()->SendEventRenderCrossToUpdate(ET_INPUT,(tBaseEventData*)tINPUT);
		tINPUT = NULL;
	}
}
void CInputManager::PollKeyboard()
{
	if (m_pKeyboard)
	{
		m_n64KeysDown = 0;
		m_n64KeysPressed = 0;
		// Store the previous keyboard state:
		memcpy(m_PreviousKeyboardState, m_KeyboardState, sizeof(m_PreviousKeyboardState));
		// Get the current keyboard state:
		m_pKeyboard->Acquire();
		m_pKeyboard->GetDeviceState(sizeof(m_KeyboardState), (void*)m_KeyboardState);
		// Handle Button Presses:
		if (KEYDOWN(m_KeyboardState, DIK_P))
		{
			m_n64KeysDown |= RJ_Config::AP_PAUSE;
			if (!KEYDOWN(m_PreviousKeyboardState, DIK_P))
				m_n64KeysPressed |= RJ_Config::AP_PAUSE;
		}
		if(RJ_GameMain::GetInstance()->getLoadingDone() && RJ_GameMain::GetInstance()->getSplashDone())
		{
			if (KEYDOWN(m_KeyboardState, DIK_RETURN))
			{
				m_n64KeysDown |= RJ_Config::AP_CONFIRM;
				if (!KEYDOWN(m_PreviousKeyboardState, DIK_RETURN))
					m_n64KeysPressed |= RJ_Config::AP_CONFIRM;
			}
			if (KEYDOWN(m_KeyboardState, DIK_W))
			{
				m_n64KeysDown |= RJ_Config::AP_UP;
				if (!KEYDOWN(m_PreviousKeyboardState, DIK_W))
					m_n64KeysPressed |= RJ_Config::AP_UP;
			}
			if (KEYDOWN(m_KeyboardState, DIK_A))
			{
				m_n64KeysDown |= RJ_Config::AP_LEFT;
				if (!KEYDOWN(m_PreviousKeyboardState, DIK_A))
					m_n64KeysPressed |= RJ_Config::AP_LEFT;
			}
			if (KEYDOWN(m_KeyboardState, DIK_D))
			{
				m_n64KeysDown |= RJ_Config::AP_RIGHT;
				if (!KEYDOWN(m_PreviousKeyboardState, DIK_D))
					m_n64KeysPressed |= RJ_Config::AP_RIGHT;
			}
			if (KEYDOWN(m_KeyboardState, DIK_S))
			{
				m_n64KeysDown |= RJ_Config::AP_DOWN;
				if (!KEYDOWN(m_PreviousKeyboardState, DIK_S))
					m_n64KeysPressed |= RJ_Config::AP_DOWN;
			}
			if (KEYDOWN(m_KeyboardState, DIK_UP))
			{
				m_n64KeysDown |= RJ_Config::AP_UP;
				if (!KEYDOWN(m_PreviousKeyboardState, DIK_UP))
					m_n64KeysPressed |= RJ_Config::AP_UP;
			}
			if (KEYDOWN(m_KeyboardState, DIK_LEFT))
			{
				m_n64KeysDown |= RJ_Config::AP_LEFT;
				if (!KEYDOWN(m_PreviousKeyboardState, DIK_LEFT))
					m_n64KeysPressed |= RJ_Config::AP_LEFT;
			}
			if (KEYDOWN(m_KeyboardState, DIK_RIGHT))
			{
				m_n64KeysDown |= RJ_Config::AP_RIGHT;
				if (!KEYDOWN(m_PreviousKeyboardState, DIK_RIGHT))
					m_n64KeysPressed |= RJ_Config::AP_RIGHT;
			}
			if (KEYDOWN(m_KeyboardState, DIK_DOWN))
			{
				m_n64KeysDown |= RJ_Config::AP_DOWN;
				if (!KEYDOWN(m_PreviousKeyboardState, DIK_DOWN))
					m_n64KeysPressed |= RJ_Config::AP_DOWN;
			}
			if (KEYDOWN(m_KeyboardState, DIK_NUMPAD8))
			{
				m_n64KeysDown |= RJ_Config::CAM_FORWARD;
				if (!KEYDOWN(m_PreviousKeyboardState, DIK_W))
					m_n64KeysPressed |= RJ_Config::CAM_FORWARD;
			}
			if (KEYDOWN(m_KeyboardState, DIK_NUMPAD5))
			{
				m_n64KeysDown |= RJ_Config::CAM_BACKWARD;
				if (!KEYDOWN(m_PreviousKeyboardState, DIK_A))
					m_n64KeysPressed |= RJ_Config::CAM_BACKWARD;
			}
			if (KEYDOWN(m_KeyboardState, DIK_NUMPAD4))
			{
				m_n64KeysDown |= RJ_Config::CAM_LEFT;
				if (!KEYDOWN(m_PreviousKeyboardState, DIK_D))
					m_n64KeysPressed |= RJ_Config::CAM_LEFT;
			}
			if (KEYDOWN(m_KeyboardState, DIK_NUMPAD6))
			{
				m_n64KeysDown |= RJ_Config::CAM_RIGHT;
				if (!KEYDOWN(m_PreviousKeyboardState, DIK_S))
					m_n64KeysPressed |= RJ_Config::CAM_RIGHT;
			}
			if (KEYDOWN(m_KeyboardState, DIK_NUMPAD7))
			{
				m_n64KeysDown |= RJ_Config::CAM_ROTLEFT;
				if (!KEYDOWN(m_PreviousKeyboardState, DIK_W))
					m_n64KeysPressed |= RJ_Config::CAM_ROTLEFT;
			}
			if (KEYDOWN(m_KeyboardState, DIK_NUMPAD9))
			{
				m_n64KeysDown |= RJ_Config::CAM_ROTRIGHT;
				if (!KEYDOWN(m_PreviousKeyboardState, DIK_A))
					m_n64KeysPressed |= RJ_Config::CAM_ROTRIGHT;
			}
			if (KEYDOWN(m_KeyboardState, DIK_NUMPAD1))
			{
				m_n64KeysDown |= RJ_Config::CAM_DOWN;
				if (!KEYDOWN(m_PreviousKeyboardState, DIK_D))
					m_n64KeysPressed |= RJ_Config::CAM_DOWN;
			}
			if (KEYDOWN(m_KeyboardState, DIK_NUMPAD3))
			{
				m_n64KeysDown |= RJ_Config::CAM_UP;
				if (!KEYDOWN(m_PreviousKeyboardState, DIK_S))
					m_n64KeysPressed |= RJ_Config::CAM_UP;
			}
			if (KEYDOWN(m_KeyboardState, DIK_SUBTRACT))
			{
				m_n64KeysDown |= RJ_Config::CAM_PITCHUP;
				if (!KEYDOWN(m_PreviousKeyboardState, DIK_SUBTRACT))
					m_n64KeysPressed |= RJ_Config::CAM_PITCHUP;
			}
			if (KEYDOWN(m_KeyboardState, DIK_ADD))
			{
				m_n64KeysDown |= RJ_Config::CAM_PITCHDOWN;
				if (!KEYDOWN(m_PreviousKeyboardState, DIK_ADD))
					m_n64KeysPressed |= RJ_Config::CAM_PITCHDOWN;
			}
			if (KEYDOWN(m_KeyboardState, DIK_SPACE))
			{
				m_n64KeysDown |= RJ_Config::AP_SPACE;
				if (!KEYDOWN(m_PreviousKeyboardState, DIK_SPACE))
					m_n64KeysPressed |= RJ_Config::AP_SPACE;
			}
			if (KEYDOWN(m_KeyboardState, DIK_F2))
			{
				m_n64KeysDown |= RJ_Config::AP_CAMLOCK;
				if (!KEYDOWN(m_PreviousKeyboardState, DIK_F2))
					m_n64KeysPressed |= RJ_Config::AP_CAMLOCK;
			}
			if (KEYDOWN(m_KeyboardState, DIK_ESCAPE))
			{
				m_n64KeysDown |= RJ_Config::AP_CANCEL;
				if (!KEYDOWN(m_PreviousKeyboardState, DIK_ESCAPE))
					m_n64KeysPressed |= RJ_Config::AP_CANCEL;
			}
			if (KEYDOWN(m_KeyboardState, DIK_F1))
			{
				m_n64KeysDown |= RJ_Config::AP_F1;
				if (!KEYDOWN(m_PreviousKeyboardState, DIK_F1))
					m_n64KeysPressed |= RJ_Config::AP_F1;
			}
			if (KEYDOWN(m_KeyboardState, DIK_M))
			{
				m_n64KeysDown |= RJ_Config::AP_OTHER2;
				if (!KEYDOWN(m_PreviousKeyboardState, DIK_M))
					m_n64KeysPressed |= RJ_Config::AP_OTHER2;
			}
			if (KEYDOWN(m_KeyboardState, DIK_N))
			{
				m_n64KeysDown |= RJ_Config::AP_OTHER3;
				if (!KEYDOWN(m_PreviousKeyboardState, DIK_N))
					m_n64KeysPressed |= RJ_Config::AP_OTHER3;
			}
			if (KEYDOWN(m_KeyboardState, DIK_LALT))
			{
				m_n64KeysDown |= RJ_Config::AP_ALT;
				if (!KEYDOWN(m_PreviousKeyboardState, DIK_LALT))
					m_n64KeysPressed |= RJ_Config::AP_ALT;
			}
			if (KEYDOWN(m_KeyboardState, DIK_TAB))
			{
				m_n64KeysDown |= RJ_Config::AP_TAB;
				if (!KEYDOWN(m_PreviousKeyboardState, DIK_TAB))
					m_n64KeysPressed |= RJ_Config::AP_TAB;
			}
		}
	}
}
void CInputManager::PollGamePad()
{

}
//void CInputManager::PollMouse()
//{
//	if (m_pMouse)
//	{
//		// Store the previous mouse state:
//		m_PreviousMouseState = m_MouseState;
//		m_pMouse->Acquire();
//		m_pMouse->GetDeviceState(sizeof(m_MouseState), (LPVOID)&m_MouseState);
//		m_fPrevMousePosX = m_fCurrMousePosX;
//		m_fPrevMousePosY = m_fCurrMousePosY;
//		m_fCurrMousePosX += (m_MouseState.lX+m_MouseState.lX);
//		m_fCurrMousePosY += (m_MouseState.lY+m_MouseState.lY);
//		
//		// left mouse button
//		if (m_MouseState.rgbButtons[0] & 0x80)
//		{
//			m_n64KeysDown |= RJ_Config::AP_CONFIRM;
//			if (!(m_PreviousMouseState.rgbButtons[0] & 0x80))
//				m_n64KeysPressed |= RJ_Config::AP_CONFIRM;
//		}
//		// right mouse button
//		if (m_MouseState.rgbButtons[1] & 0x80)
//		{
//			m_n64KeysDown |= RJ_Config::AP_CANCEL;
//			if (!(m_PreviousMouseState.rgbButtons[1] & 0x80))
//				m_n64KeysPressed |= RJ_Config::AP_CANCEL;
//		}
//	}
//}
void CInputManager::ClearBufferedData(void)
{
	memset(&m_didod, 0, sizeof(m_didod));	//	clear out device object data.
	m_dwBufferElements = BUFFEREDKEYSIZE;	//	set buffer elements to the size of the array.
}
bool CInputManager::ReadBufferedDevice(void)
{
	//	Make sure the device was made.
	if (!m_pKeyboard)
		return false;

	ClearBufferedData();

	//	Attempt to read the device...
	if (FAILED( m_pKeyboard->GetDeviceData(sizeof(DIDEVICEOBJECTDATA), m_didod, &m_dwBufferElements, 0) ))
	{
		//	If we couldn't, try to re-acquire the device.
		if (FAILED( m_pKeyboard->Acquire() ))
			return false; //	Could not re-acquire the Device.

		ClearBufferedData();

		//	Now try reading it
		if (FAILED( m_pKeyboard->GetDeviceData(sizeof(DIDEVICEOBJECTDATA), m_didod, &m_dwBufferElements, 0) ))
			return false;
	}

	//	Success.
	return true;
}
char CInputManager::CheckBufferedKeysEx(void)
{
	//	Get the Current Keyboard State.
	//	Get the Current Keyboard State.
	if ( !GetKeyboardState(m_ucAsciiVals) )
		return 0; // Could not GetKeyboardState.

	unsigned short usNum	= 0;
	unsigned int uiVKCode	= 0;

	//	Go through each keyboard key
	for (int i = 0; i < 256; i++)
	{
		// if the high bit is on, it is pressed
		if (KEYDOWN(m_KeyboardState, (unsigned char)i ))
		{
			m_n64KeysDown |= RJ_Config::AP_TYPE;
			if (!KEYDOWN(m_PreviousKeyboardState, (unsigned char)i))
			{
				m_n64KeysPressed |= RJ_Config::AP_TYPE;
			
				//	Map the Scan Code from DirectInput to a Virtual Key value...
				uiVKCode = MapVirtualKeyEx(i, 1, m_keyLayout);
				//	...and translate that Virtual Key into an Ascii Value.
				ToAsciiEx(uiVKCode, i, m_ucAsciiVals, &usNum, 0, m_keyLayout);
			}
		}
	}

	//	Return the Ascii Value.
	return (char)usNum;
}