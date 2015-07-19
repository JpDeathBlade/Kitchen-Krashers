/**
 * 0===========================================================================0
 * | RJ_GameMain.h                                                             |
 * |  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  |
 * |   Author        : Joseph Stuhr                                            |
 * |   E-mail        : JpDeathBlade@gmail.com                                  |
 * |  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  |
 * |   Date Created  : 11:22 pm (Eastern) on 05/21/2010                        |
 * |   Date Edited   : 00:00 pm (Eastern) on 00/00/2010                        |
 * 0=====0===============================================================0=====0
 *       |                                                               |      
 * 0=====0===============================================================0=====0
 * | Code Description:                                                         |
 * |  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  |
 * |   A wrapper class between the main game code and WinMain.                 |
 * 0===========================================================================0
 */

#ifndef  REDJAGUAR_JS_GAMEMAIN_H_	// Check to see if this .h is defined
#define  REDJAGUAR_JS_GAMEMAIN_H_	//  if not, define it

// 0===========================================================================0
// | ~ Includes ~                                                              |
// 0===========================================================================0
//#define _WIN32_WINNT 0x0502
#include <windows.h>
#include <vector>
#include <deque>
#include "Utilities\\RJ_Timer.h"
#include "RedJaguarEngine/Renderer/RJ_Renderer.h"
#include "RedJaguarEngine/Event/IListener.h"
using std::deque;
#include "RedJaguarEngine\\Text\\CBitmapFont.h"

//class CBitmapFont;

// 0===========================================================================0
// | ~ Defines ~                                                               |
// 0===========================================================================0
#define RJ_MAIN_GAME RJ_GameMain::GetInstance()	// Allows for easy access to 
												//  this classes Instance
class RJ_IGameState;
class RJ_Timer;
class CEventManager;
class CInputManager;
class SceneManager;
class CAssetManager;
class CSoundEngine;
class CGameplay;
class Cursor;
class AnimationManager;
struct tStateConfig;

// 0===========================================================================0
// | class S_GameMain                                                          |
// |  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  |
// |   A wrapper class between the main game code and WinMain.                 |
// 0===========================================================================0
class RJ_GameMain : public IListener
{
	public:
		// 0===================================================================0
		// | struct WindowParameters - sizeof(32)                              |
		// |  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  |
		// |   Holds information about the current Win32 Application.          |
		// 0===================================================================0
		//__declspec(align(4))		// Allow the struct to be 4 byte aligned
			struct WindowParameters
			{
				HINSTANCE			t_ApplicationInstance;
				HWND				t_Window;
				unsigned int		t_uiClientWidth;
				unsigned int		t_uiClientHeight;
				unsigned int		t_uiPositionX;
				unsigned int		t_uiPositionY;
				bool				t_bFullScreen;
				bool				t_bVSync;
				bool				t_bInFocus;
				char				t_cFiller[2];	//Forces the struct size
			};	// end of struct WindowParameters

	private:
		// 0===================================================================0
		// | ~ VARIABLES ~                                                     |
		// 0=====0=======================================================0=====0
		//       |                                                       |      
		// 0=====0=======================================================0=====0
		// | S_GameMain m_Instance                                             |
		// |  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  |
		// |   Singleton: Holds the instance to this class.                    |
		// 0===================================================================0
		static RJ_GameMain				m_Instance;
		RJ_Timer						m_TimerUpdate;
		RJ_Timer						m_TimerRender;

		// 0===================================================================0
		// | WindowParameters  m_WindowParams                                  |
		// |  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  |
		// |   Holds the current window paramaters.                            |
		// 0===================================================================0
		WindowParameters				m_tWindowParams;

		// 0===================================================================0
		// | WindowParameters  m_WindowParams                                  |
		// |  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  |
		// |   Holds the current window paramaters.                            |
		// 0===================================================================0
		std::vector<RJ_IGameState*>		m_States;
		std::vector<RJ_IGameState*>		m_StatesToPush;
		int								m_nNumStatesToPop;
		bool							m_bClearStates;


		bool							m_bScenePause;

		// 0===================================================================0
		// | ~ PRIVATE FUNCTIONS ~                                             |
		// 0=====0=======================================================0=====0
		//       |                                                       |      
		// 0=====0=======================================================0=====0
		// | Constructor : S_GameMain(void)                                    |
		// |  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  |
		// |   Creates an instance of S_GameMain.                              |
		// 0===================================================================0
		RJ_GameMain(void);
		RJ_GameMain(RJ_GameMain& _ref);

		// 0===================================================================0
		// | Operator: =                                                       |
		// |  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  |
		// |   Assignment operator.                                            |
		// 0===================================================================0
		RJ_GameMain& operator= (RJ_GameMain& _ref);

		// 0===================================================================0
		// | bool Update(float)                                                |
		// |  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  |
		// |   Updates all the objects in the game.                            |
		// 0===================================================================0
		bool Update(double _dDelta);

		// 0===================================================================0
		// | bool Render(float)                                                |
		// |  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  |
		// |   Draws every object to the screen.                               |
		// 0===================================================================0
		bool Render(double _dDelta);
		bool m_bRenderPaused;

		bool InternalInitialize(void);
		bool InternalShutdown(void);

		// 0===================================================================0
		// | ~ THREADING VARIABLES ~                                             |
		// 0=====0=======================================================0=====0
		volatile bool m_bRenderLoadComplete;
		volatile bool m_bUpdateLoadComplete;
		volatile bool m_bEndThreadLoop;
		volatile bool m_bThreadIsDead;
		uintptr_t m_pThreadHandle;
		CRITICAL_SECTION m_CritShutdown;
		CRITICAL_SECTION m_CritShutdownDos;
		CRITICAL_SECTION m_CritLoad;
		CRITICAL_SECTION m_CritLoadDos;
		// 0===================================================================0
		// | ~ LOADING VARIABLES ~                                             |
		// 0=====0=======================================================0=====0
		bool m_bLoadisDone;
		bool m_bSplashisDone;
		// 0===================================================================0
		// | ~ THREADING FUNCTIONS ~                                             |
		// 0=====0=======================================================0=====0
		unsigned int static __stdcall OffThread(void* pInstance);
		bool Loading(void* pInstance);
		// 0===================================================================0
		// | ~ SINGLETON POINTERS ~                                             |
		// 0=====0=======================================================0=====0
		CEventManager* EventManager;
		CInputManager* InputManager;
		SceneManager* SceneManager;
		CAssetManager* AssetManager;
		CSoundEngine* SoundEngine;
		CGameplay*		Gameplay;
		Cursor*			Curs;
		tStateConfig*	tSC;
		AnimationManager* AnimManager;
		// 0===================================================================0
		// | ~ FRAME COUNTERS ~                                             |
		// 0=====0=======================================================0=====0
		float m_fFramesPerSecond; //RENDERSIDE
		float m_fUpdatesPerSecond; //UPDATESIDE
		float m_fUPS; //TEMP RENDERSIDE FOR DISPLAYING UPS
		int fFPS;
		int fUPS;
		double dLastUpdateTime;
		double dLastRenderTime;
		// 0===================================================================0
		// | ~ SOUND IDS ~                                             |
		// 0=====0=======================================================0=====0
		//int m_nMusic;
		// 0===================================================================0
		// | ~ SPLASH TEXTURE IDS ~                                             |
		// 0=====0=======================================================0=====0
		int m_nGPGamesSplash;
		int m_nAtmoTeamSplash;
		int m_nWhiteFadeSplash;
		int m_nStudioHTSplash;
		float m_fsplashtimer;

		// 0===================================================================0
		// | ~ Loading Data ~                                                  |
		// 0=====0=======================================================0=====0
		CBitmapFont *Font;
		int m_nLoadingScreen;
		D3DXVECTOR2 m_vBanana;
		D3DXVECTOR2 m_vPotato;
		D3DXVECTOR2 m_vPopsicle;
		D3DXVECTOR2 m_vApple;
		D3DXVECTOR2 m_vFish;
		D3DXVECTOR2 m_vAnthony;
		double uberClocked;
	public:
		// 0===================================================================0
		// | ~ PUBLIC FUNCTIONS ~                                              |
		// 0=====0=======================================================0=====0
		//       |                                                       |      
		// 0=====0=======================================================0=====0
		// | S_GameMain* GetInstance(void)                                     |
		// |  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  |
		// |   Singleton: Returns the instance to this class.                  |
		// 0===================================================================0
		static RJ_GameMain* GetInstance(void)	{ return &m_Instance; }
		virtual ~RJ_GameMain(void) {};

		bool Initialize(const WindowParameters _params);
		bool Run(void);
		bool Shutdown(void);	
		void ChangeDisplay(const unsigned int _width, const unsigned int _height, bool _fullscreen);
		bool ChangeDisplay(RJ_Renderer::RenderParameters _params);
		bool ToggleFullScreen();
		bool GoFullScreen();
		bool GoWindowed();
		void HandleEvent(CEvent* pEvent);

		void ChangeState(RJ_IGameState* _state);
		void PushState(RJ_IGameState* _state);
		void PopState(void);
		void PushStateNow(RJ_IGameState* _state);
		void PopStateNow(void);
		void ClearAllStates(void);

		// Renders loading screen
		void loadingScreen();

		// 0===================================================================0
		// | ~ ACCESSORS ~                                                     |
		// 0===================================================================0
		const WindowParameters getWindowParameters(void)				{ return m_tWindowParams; }
			const HINSTANCE getApplicationInstance(void)				{ return m_tWindowParams.t_ApplicationInstance; }
			const HWND getWindowHandle(void)							{ return m_tWindowParams.t_Window; }
			const unsigned int getClientWidth(void)						{ return m_tWindowParams.t_uiClientWidth; }
			const unsigned int getClientHeight(void)					{ return m_tWindowParams.t_uiClientHeight; }
			const bool getFullScreen(void)								{ return m_tWindowParams.t_bFullScreen; }
			const bool getVSync(void)									{ return m_tWindowParams.t_bVSync;	}
			const bool getFocus(void)									{ return m_tWindowParams.t_bInFocus; }
			const bool getLoadingDone() {return m_bLoadisDone;}
			const bool getSplashDone() {return m_bSplashisDone;}
			bool getRenderPause(){return m_bRenderPaused;}

			RJ_Timer* GetRenderTime()	
			{
				return &m_TimerRender;
			}
			RJ_Timer* GetUpdateTime()	
			{
				return &m_TimerUpdate;
			}
		// 0===================================================================0
		// | ~ MUTATORS ~                                                      |
		// 0===================================================================0
		void setWindowParameters(const WindowParameters _params)		{ m_tWindowParams = _params; }
			void setApplicationInstance(const HINSTANCE _hInstance)		{ m_tWindowParams.t_ApplicationInstance = _hInstance; }
			void setWindowHandle(const HWND _hWnd)						{ m_tWindowParams.t_Window = _hWnd; }
			void setClientWidth(const unsigned int _width)				{ m_tWindowParams.t_uiClientWidth = _width; }
			void setClientHeight(const unsigned int _height)			{ m_tWindowParams.t_uiClientHeight = _height; }
			void setFullScreen(const bool _fullscreen)					{ m_tWindowParams.t_bFullScreen = _fullscreen; }
			void setVSync(const bool _vsync)							{ m_tWindowParams.t_bVSync = _vsync;	}
			void setFocus(const bool _focus)							{ m_tWindowParams.t_bInFocus = _focus; }
			void setRenderPause(bool bPaused)							{ m_bRenderPaused = bPaused; }
};

#endif	// end of REDJAGUAR_JS_GAMEMAIN_H_