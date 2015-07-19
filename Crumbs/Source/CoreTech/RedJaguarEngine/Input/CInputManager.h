#ifndef CINPUTMANAGER_H_ 
#define	CINPUTMANAGER_H_
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include "../../../RJ_Config.h"
#pragma comment(lib, "dinput8.lib")
#define KEYDOWN(name, key) (name[key] & 0x80  ? true : false)
#define BUFFEREDKEYSIZE 10
class CEventManager;
class Cursor;

class CInputManager
{
	INT64 m_n64KeysDown;
	INT64 m_n64KeysPressed;
	//DirectInput Objects:
	IDirectInput8*			m_pDI;
	IDirectInputDevice8*	m_pKeyboard;
	IDirectInputDevice8*	m_pMouse;
	IDirectInputDevice8*	m_pController;
	unsigned int m_uiPlayerCount;
	//XINPUT_STATE	m_XboxControllerState;
	//XINPUT_STATE	m_PreviousXboxControllerState;
	DIJOYSTATE m_ControllerState;
	DIJOYSTATE m_PreviousControllerState;
	//DIMOUSESTATE2  m_MouseState;
	//DIMOUSESTATE2  m_PreviousMouseState;
	char m_KeyboardState[256];
	char m_PreviousKeyboardState[256];
	unsigned char		m_ucAsciiVals[256];
	HKL					m_keyLayout;
	DIDEVICEOBJECTDATA		m_didod[BUFFEREDKEYSIZE];
	DWORD					m_dwBufferElements;
	// Mouse Position 
	/*float m_fCurrMousePosX;
	float m_fCurrMousePosY;
	float m_fPrevMousePosX;
	float m_fPrevMousePosY;
	*/
	Cursor* cur;
	CInputManager();
	~CInputManager();	
public:
	static CInputManager* GetInstanceRender();
	bool Init(HINSTANCE ApplicationInstance, HWND WindowInstance, unsigned int nInputDevice);
	bool Shutdown();
	void Update(double _fdelta);
	void PollKeyboard();
	void PollGamePad();
	bool ReadBufferedDevice(void);
	void ClearBufferedData(void);
	//void PollMouse();
	// 0===================================================================0
	// | ~ ACCESSORS ~                                                     |
	// 0===================================================================0
	//float getMousePosX(){return m_fCurrMousePosX;}
	//float getMousePosY(){return m_fCurrMousePosY;}
	INT64 getKeysDown(){return m_n64KeysDown;}
	INT64 getKeysPressed(){return m_n64KeysPressed;}
	char  CheckBufferedKeysEx(void);
	// 0===================================================================0
	// | ~ MUTATORS ~                                                     |
	// 0===================================================================0
	//void setMousePosX(float fposX){m_fCurrMousePosX = fposX;}
	//void setMousePosY(float fposY){m_fCurrMousePosY = fposY;}
};

#endif