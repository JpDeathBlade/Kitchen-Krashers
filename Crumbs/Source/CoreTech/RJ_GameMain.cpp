// 0===========================================================================0
// | ~ Includes ~                                                              |
// 0===========================================================================0
#include "RJ_GameMain.h"
#include "Utilities\\RJ_Timer.h"
#include "..\\RJ_Config.h"

#include "RedJaguarEngine/Renderer/RJ_Renderer.h"
#include "RedJaguarEngine/Event/EventManager.h"
#include "RedJaguarEngine/Input/CInputManager.h"
//#include "RedJaguarEngine/Input/Cursor.h"
#include "RedJaguarEngine/Scene/RJ_SceneManager.h"
#include "RedJaguarEngine/Asset/AssetManager.h"
#include "../Gameplay/Gameplay.h"
#include "RedJaguarEngine/Sound/CSoundEngine.h"
#include "RedJaguarEngine/States/RJ_IGameState.h"
#include "RedJaguarEngine/States/State_MainMenu.h"
#include "RedJaguarEngine/States/State_GameOver.h"
#include "RedJaguarEngine/States/State_Hud.h"
#include "RedJaguarEngine/States/State_CamFly.h"
#include "RedJaguarEngine/States/State_Tutorial.h"
#include "RedJaguarEngine/States/State_Config.h"
#include "RedJaguarEngine/States/State_Loading.h"
#include "RedJaguarEngine/Animations/AnimManager.h"
#include <process.h>
#include <fstream>
#include <list>
#define THREAD_END "Thread End"


//#include "Games\\MyPlayground\\States\\JS_TestState.h"

// 0===========================================================================0
// | ~ STATICS ~                                                               |
// 0===========================================================================0
RJ_GameMain RJ_GameMain::m_Instance;

// 0===========================================================================0
// | Constructor : S_GameMain(void)                                            |
// |  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  |
// | Author: Joseph Stuhr (JS)                                                 |
// | Last Edit: 04/30/10                                                       |
// |  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  |
// | Purpose: Creates an instance of S_GameMain and initalizes to default      |
// |  values.                                                                  |
// 0===========================================================================0
RJ_GameMain::RJ_GameMain(void)
{
	memset(&m_tWindowParams,0,sizeof(m_tWindowParams));
	EventManager = CEventManager::GetInstanceThreaded();
	InputManager = CInputManager::GetInstanceRender();
	SceneManager = SceneManager::getInstanceUpdate();
	AssetManager = CAssetManager::getInstanceRender();
	SoundEngine = CSoundEngine::GetInstanceRender();
	AnimManager = AnimationManager::GetInstanceRender();
	Gameplay = CGameplay::GetInstanceUpdate();
	//Curs = Cursor::GetInstanceRender();
	tSC = tStateConfig::GetInstanceRender();
	m_fFramesPerSecond = 0; m_fUPS = 0;
	m_fUpdatesPerSecond = 0; fFPS = 0; fUPS = 0;
	dLastUpdateTime = 0; dLastRenderTime = 0;
	//m_nMusic = -1;
	InitializeCriticalSection(&m_CritShutdown);
	InitializeCriticalSection(&m_CritShutdownDos);
	InitializeCriticalSection(&m_CritLoad);
	InitializeCriticalSection(&m_CritLoadDos);
	m_nGPGamesSplash = -1;
	m_nAtmoTeamSplash = -1;
	m_nWhiteFadeSplash = -1;
	m_nStudioHTSplash = -1;
	m_bLoadisDone = false;
	m_bSplashisDone = false;
	m_bEndThreadLoop = false;
	m_bThreadIsDead = false;
	m_fsplashtimer = 0;
	m_nNumStatesToPop = 0;
	m_bClearStates = false;
	m_bUpdateLoadComplete = false;
	m_bRenderLoadComplete = false;
	m_bRenderPaused = true;
}

bool RJ_GameMain::Initialize(const WindowParameters _params)
{
	m_tWindowParams = _params;

	return InternalInitialize();
}

bool RJ_GameMain::Shutdown(void)
{
	return InternalShutdown();
}

bool RJ_GameMain::Run(void)
{
	//MAIN THREAD LOOP
	// Delta time calculations.
	for(int i = 0; i < m_nNumStatesToPop; i++)
	{
		PopStateNow();
	}
	m_nNumStatesToPop = 0;
	unsigned int size = m_StatesToPush.size();
	for(unsigned int _State = 0; _State < size; _State++)
	{
		PushStateNow(m_StatesToPush[_State]);
	}
	m_StatesToPush.clear();

	if(InputManager->getKeysDown() & RJ_Config::AP_ALT && InputManager->getKeysDown() & RJ_Config::AP_CONFIRM)
	{
		RJ_RENDERER->ToggleFullScreen();
	}

	double m_dDelta = min(m_TimerRender.GetElapsedTime(),1.0f);
	m_TimerRender.Update();
	if(!Render(m_dDelta))
	{
		return false;
	}
	Sleep(0);
	return true;
}

bool RJ_GameMain::Update(double _dDelta)
{
	EventManager->TEBTtoUpdateIN();
	EventManager->ProcessEventsUpdate();
	fUPS++;
	dLastUpdateTime += m_TimerUpdate.GetElapsedTime();
	if(dLastUpdateTime > 1.0f)
	{
		m_fUpdatesPerSecond = (fUPS / float(dLastUpdateTime));
		fUPS = 0;
		dLastUpdateTime = 0;

		//DO NOT TOUCH THE INFORMATION YOU ARE ABOUT TO POINT TO AGAIN WILL CAUSE A THREAD CRASH
		volatile tUpdatesPerSecond* tUPS = new tUpdatesPerSecond(EM_UPDATESPERSECOND, m_fUpdatesPerSecond); 
		EventManager->SendEventUpdateCrossToRender(ET_UPDATESPERSECOND,(tBaseEventData*)tUPS);
		tUPS = NULL;
	}
	if(!m_bScenePause)
		SceneManager->update(_dDelta);
	//EventManager->SendEventUpdateLocal(ET_INPUT);
	//Sleep(1);
	EventManager->TEBTtoRenderCentral();
	return true;
}

bool RJ_GameMain::Render(double _dDelta)
{
	ASSETMANAGER->ResetEffects();

	EventManager->TEBTtoRenderIN();
	EventManager->ProcessEventsRender();
	fFPS++;
	dLastRenderTime += m_TimerRender.GetElapsedTime();
	if(dLastRenderTime > 1.0f)
	{
		m_fFramesPerSecond = (fFPS / float(dLastRenderTime));
		fFPS = 0;
		dLastRenderTime = 0;
	}
	InputManager->Update(_dDelta);
	//if(InputManager->getKeysPressed() & RJ_Config::AP_PAUSE)
	//	return false;

	if(!m_States.empty())
		{
			unsigned int size = m_States.size()-1;
			for(unsigned int _States = 0; _States < size; _States++)
			{
				m_States[_States]->InputUnder(_dDelta);
			}

			m_States.back()->Input(_dDelta);
		}

	SoundEngine->Update();
	if(!m_bRenderPaused)
		AssetManager->Update(_dDelta);

	if(!RJ_RENDERER->isLostDevice())
	{
		RJ_RENDERER->BeginRender();

		//RJ_RENDERER->DrawGrid();
		//RJ_RENDERER->DrawAxisLines();
		RJ_RENDERER->Render(_dDelta);
			if(!m_States.empty())
			{
				unsigned int size = m_States.size()-1;
				for(unsigned int _States = 0; _States < size; _States++)
				{
					m_States[_States]->RenderUnder(_dDelta);
				}
				m_States.back()->Render(_dDelta);
			}

			ASSETMANAGER->RenderEffects(_dDelta);

			//RJ_RENDERER->RenderText("Printing...",0,0);
	//	char szBuffer[128];
	//	sprintf_s(szBuffer, "fps %f", m_fFramesPerSecond);
	//	RJ_RENDERER->RenderText(szBuffer, 0, 210);
	//
	//	sprintf_s(szBuffer, "ups %f", m_fUPS);
	//	RJ_RENDERER->RenderText(szBuffer, 0, 230);
	////#if _DEBUG
	//	sprintf_s(szBuffer, "ViewMatX %f %f %f %f", -RJ_RENDERER->getViewMatrix()._11, -RJ_RENDERER->getViewMatrix()._21,
	//		-RJ_RENDERER->getViewMatrix()._31, -RJ_RENDERER->getViewMatrix()._41);
	//	RJ_RENDERER->RenderText(szBuffer, 600, 600);
	//
	//	sprintf_s(szBuffer, "ViewMatY %f %f %f %f", -RJ_RENDERER->getViewMatrix()._12, -RJ_RENDERER->getViewMatrix()._22,
	//		-RJ_RENDERER->getViewMatrix()._32, -RJ_RENDERER->getViewMatrix()._42);
	//	RJ_RENDERER->RenderText(szBuffer, 600, 630);
	//
	//	sprintf_s(szBuffer, "ViewMatZ %f %f %f %f", -RJ_RENDERER->getViewMatrix()._13, -RJ_RENDERER->getViewMatrix()._23,
	//		-RJ_RENDERER->getViewMatrix()._33, -RJ_RENDERER->getViewMatrix()._43);
	//	RJ_RENDERER->RenderText(szBuffer, 600, 660);
	//#endif
	//		sprintf_s(szBuffer, "Launch Power %f", RJ_RENDERER->getLaunchPower());
	//		RJ_RENDERER->RenderText(szBuffer, 0, 200);
	//
	//		sprintf_s(szBuffer, "Launch Angle %f", RJ_RENDERER->getLaunchAngle());
	//		RJ_RENDERER->RenderText(szBuffer, 0, 230);
	//
	//		sprintf_s(szBuffer, "Stone Speed %f", RJ_RENDERER->getStoneSpeed());
	//		RJ_RENDERER->RenderText(szBuffer, 0, 260);
	//#if _DEBUG
	//		sprintf_s(szBuffer, "Current Player Score %i", RJ_RENDERER->getCurrPlayerScore());
	//		RJ_RENDERER->RenderText(szBuffer, 0, 290);
	//
	//		sprintf_s(szBuffer, "Current Stone Angle %f", RJ_RENDERER->getStoneAngle());
	//		RJ_RENDERER->RenderText(szBuffer, 0, 320);
	//
	/*		sprintf_s(szBuffer, "Current Stone Forward %f %f %f", RJ_RENDERER->getStoneForward().x, RJ_RENDERER->getStoneForward().y, RJ_RENDERER->getStoneForward().z);
			RJ_RENDERER->RenderText(szBuffer, 0, 350);
	
			RJ_RENDERER->Render3DLine(RJ_RENDERER->getStoneForward(), RJ_RENDERER->getStoneForward()*2.0f, D3DXVECTOR4(0.0f, 0.0f, 0.0f, 1.0f));
			float angle = RJ_RENDERER->getLaunchAngle();
			D3DXVECTOR3 vAngle(cos(angle * D3DX_PI), 0.0f, sin(angle * D3DX_PI));
			RJ_RENDERER->Render3DLine(vAngle, vAngle*2.0f, D3DXVECTOR4(1.0f, 0.0f, 1.0f, 1.0f))*/;
	//#endif

		RJ_RENDERER->PostProcess();
		RJ_RENDERER->EndRender();
	}

	EventManager->TEBTtoUpdateCentral();
	return true;
}
unsigned int RJ_GameMain::OffThread(void* pInstance)
{
	//OFF THREAD UPDATE LOOP
	RJ_GameMain* RJGame = (RJ_GameMain*)pInstance;
	if(!RJGame->Loading(RJGame))
		return 0;
	EnterCriticalSection(&RJGame->m_CritLoad);
	RJGame->m_bUpdateLoadComplete = true;
	LeaveCriticalSection(&RJGame->m_CritLoad);
	while(true)
	{
		EnterCriticalSection(&RJGame->m_CritLoadDos);
		if(RJGame->m_bRenderLoadComplete == true)
			break;			
		LeaveCriticalSection(&RJGame->m_CritLoadDos);
		Sleep(0);
	}
	while(true)
	{
		EnterCriticalSection(&RJGame->m_CritShutdownDos);
		if(RJGame->m_bEndThreadLoop == true)
			break;
		LeaveCriticalSection(&RJGame->m_CritShutdownDos);

		double m_dDelta = min(RJGame->m_TimerUpdate.GetElapsedTime(),1.0f);
		RJGame->m_TimerUpdate.Update();
		RJGame->Update(m_dDelta);
		Sleep(0);
	}

	EnterCriticalSection(&RJGame->m_CritShutdown);
	RJGame->m_bThreadIsDead = true;
	LeaveCriticalSection(&RJGame->m_CritShutdown);

	SetEvent(THREAD_END);
	return 0;
}
bool RJ_GameMain::Loading(void* pInstance)
{
	RJ_GameMain* pGame = (RJ_GameMain*)pInstance;
	//LOAD HERE
	pGame->SceneManager->init();
	pGame->tSC->LoadStateImages();
	pGame->AnimManager->Init();
	
	ASSETMANAGER->LoadEffects();
	ASSETMANAGER->LoadAnimMesh("AnthonyBody");
	ASSETMANAGER->LoadAnimMesh("SM_Broom");
	ASSETMANAGER->LoadAnimMesh("toothPicks");
	ASSETMANAGER->LoadAnimMesh("GummyWorm");
	//GumMESS!
	ASSETMANAGER->LoadMesh("DM_GumMESS");
	//PizzaCutter
	ASSETMANAGER->LoadMesh("SM_PizzaCutter");
	//Worm Gum
	ASSETMANAGER->LoadMesh("SM_WormGum");
	//Hats
	ASSETMANAGER->LoadMesh("H_GumHat");
	ASSETMANAGER->LoadMesh("H_CherryHat");
	ASSETMANAGER->LoadMesh("H_ColdTurkeyHat");
	ASSETMANAGER->LoadMesh("H_FortuneCookie");
	ASSETMANAGER->LoadMesh("H_WhippedCreamHat");
	m_bScenePause = true;
	return true;
}
void RJ_GameMain::ChangeDisplay(unsigned int _width, unsigned int _height, bool _fullscreen)
{
	m_tWindowParams.t_uiClientHeight	= _width;
	m_tWindowParams.t_uiClientWidth		= _height;
	m_tWindowParams.t_bFullScreen		= _fullscreen;

	RJ_RENDERER->ChangeDisplay(_width, _height, _fullscreen);
}

bool RJ_GameMain::ChangeDisplay(RJ_Renderer::RenderParameters _params)
{
	m_tWindowParams.t_uiClientHeight	= _params.t_uiClientHeight;
	m_tWindowParams.t_uiClientWidth		= _params.t_uiClientWidth;
	m_tWindowParams.t_bFullScreen		= _params.t_bFullScreen;

	return RJ_RENDERER->ChangeDisplay(_params);
}

bool RJ_GameMain::ToggleFullScreen()
{
	return RJ_RENDERER->ToggleFullScreen();
}

bool RJ_GameMain::GoFullScreen()
{
	return RJ_RENDERER->GoFullScreen();
}

bool RJ_GameMain::GoWindowed()
{
	return RJ_RENDERER->GoWindowed();
}

void RJ_GameMain::ChangeState(RJ_IGameState* _state)
{
	m_bClearStates = true;
	PushState(_state);
}

void RJ_GameMain::PushState(RJ_IGameState* _state)
{
	m_StatesToPush.push_back(_state);
}

void RJ_GameMain::PopState(void)
{
	m_nNumStatesToPop++;
}

void RJ_GameMain::PushStateNow(RJ_IGameState* _state)
{
	if(_state)
	{
		if(!m_States.empty())
		{
			m_States.back()->OnExit();
		}

		m_States.push_back(_state);
		m_States.back()->OnPush();
		m_States.back()->OnEnter();
	}
}

void RJ_GameMain::PopStateNow(void)
{
	if(!m_States.empty())
	{
		m_States.back()->OnExit();
		m_States.back()->OnPop();
		m_States.pop_back();

		if(!m_States.empty())
		{
			m_States.back()->OnEnter();
		}
	}
}

void RJ_GameMain::ClearAllStates(void)
{
	m_bClearStates = false;
	unsigned int size = m_States.size();
	for(unsigned int _State = 0; _State < size; _State++)
	{
		m_States[_State]->OnExit();
		m_States[_State]->OnPop();
	}
	m_States.clear();
}

// 0===========================================================================0
// | ~ Private Functions ~                                                     |
// 0===========================================================================0

bool RJ_GameMain::InternalInitialize(void)
{
	RJ_Renderer::RenderParameters renderParams;
		renderParams.t_Window						= m_tWindowParams.t_Window;
		renderParams.t_uiClientWidth				= m_tWindowParams.t_uiClientWidth;
		renderParams.t_uiClientHeight				= m_tWindowParams.t_uiClientHeight;
		renderParams.t_uiPositionX					= m_tWindowParams.t_uiPositionX;
		renderParams.t_uiPositionY					= m_tWindowParams.t_uiPositionY;
		renderParams.t_fNearClip					= 1.0f;
		renderParams.t_fFarClip						= 1000.0f;
		renderParams.t_ucClearRed					= 0;
		renderParams.t_ucClearGreen					= 0;
		renderParams.t_ucClearBlue					= 0;
		renderParams.t_bFullScreen					= m_tWindowParams.t_bFullScreen;

	if(!RJ_RENDERER->Initialize(renderParams))
	{
		return false;
	}

	EventManager->Init();
	//LOAD SOUND ASYNC
	EventManager->RegisterListenerRender(ET_UPDATESPERSECOND,this);
	EventManager->RegisterListenerUpdate(ET_SCENEPAUSE,this);
	EventManager->RegisterListenerRender(ET_POPSTATE,this);
	EventManager->RegisterListenerRender(ET_PUSHGAMEOVER,this);
	EventManager->RegisterListenerRender(ET_ENTERLEVEL,this);
	AssetManager->Initialize();
	InputManager->Init(m_tWindowParams.t_ApplicationInstance,m_tWindowParams.t_Window,(RJ_Config::RJ_INPUT_KEYBOARD | RJ_Config::RJ_INPUT_MOUSE));
	//Curs->Init();
	m_nGPGamesSplash = AssetManager->LoadTexture("T_Splash_GPGames.dds", 0, true);
	m_nAtmoTeamSplash = AssetManager->LoadTexture("T_Splash_Atmosphere.dds", 0, true);
	m_nWhiteFadeSplash = AssetManager->LoadTexture("T_Splash_FadeToWhite.dds", 0, true);
	m_nStudioHTSplash = AssetManager->LoadTexture("StudioLogo.dds", 0, true);
	m_nLoadingScreen = AssetManager->LoadTexture("T_LoadingScreen.dds", 0, true);
	Font = new CBitmapFont("t_Crumbs_font.dds","Resources/Fonts/Crumbs_font.dat");
	m_TimerRender.Start();
	m_TimerUpdate.Start();

	//Init Loading Screen variables
	m_vBanana = D3DXVECTOR2(512.0f + (cosf(D3DX_PI * 0.5f) * 300.0f), 384.0f - (sinf(D3DX_PI * 0.5f) * 250.0f));
	m_vPotato = D3DXVECTOR2(512.0f + (cosf(D3DX_PI * 0.5f) * 300.0f), 384.0f - (sinf(D3DX_PI * 0.5f) * 250.0f));
	m_vPopsicle = D3DXVECTOR2(512.0f + (cosf(D3DX_PI * 0.5f) * 300.0f), 384.0f - (sinf(D3DX_PI * 0.5f) * 250.0f));
	m_vApple = D3DXVECTOR2(512.0f + (cosf(D3DX_PI * 0.5f) * 300.0f), 384.0f - (sinf(D3DX_PI * 0.5f) * 250.0f));
	m_vFish = D3DXVECTOR2(512.0f + (cosf(D3DX_PI * 0.5f) * 300.0f), 384.0f - (sinf(D3DX_PI * 0.5f) * 250.0f));
	m_vAnthony = D3DXVECTOR2(512.0f, 384.0f);
	uberClocked = 0.5;

	SoundEngine->Init(m_tWindowParams.t_Window);

	//BEGINS LOADING THREAD, WHICH THAN BECOMES THE UPDATE THREAD - Kurt
	m_pThreadHandle = _beginthreadex(NULL, 0, OffThread, this, 0, NULL);
	while(true)
	{
		RJ_RENDERER->BeginRender();
		m_TimerRender.Update();
		float fdt = (float)m_TimerRender.GetElapsedTime();
		InputManager->Update(fdt);
		SoundEngine->Update();
		/*if(InputManager->getKeysPressed() & RJ_Config::AP_PAUSE)
		{	
			m_bSplashisDone = true;
		}*/
		AssetManager->DrawFlatSprite(m_nWhiteFadeSplash,0,0,1.0f,1.0f,0,0,0,0,D3DCOLOR_ARGB(255,255,255,255), true);
		if (m_fsplashtimer < 8.0f)
		{
			if(m_fsplashtimer < 3.0f && m_fsplashtimer >= 1.0f)
				AssetManager->DrawFlatSprite(m_nGPGamesSplash,0,0,1.0f,0.75f,0,0,0,0,D3DCOLOR_ARGB(int(0.0f+((m_fsplashtimer-1.0f)*127.5f)),255,255,255), true);
			else if(m_fsplashtimer >= 6.0f && m_fsplashtimer < 8.0f)
				AssetManager->DrawFlatSprite(m_nGPGamesSplash,0,0,1.0f,0.75f,0,0,0,0,D3DCOLOR_ARGB(int(255.0f-((m_fsplashtimer-6.0f)*127.5f)),255,255,255), true);
			else if(m_fsplashtimer >= 3.0f && m_fsplashtimer < 6.0f)
				AssetManager->DrawFlatSprite(m_nGPGamesSplash,0,0,1.0f,0.75f,0,0,0,0,D3DCOLOR_ARGB(255,255,255,255), true);
		}
		else if (m_fsplashtimer >= 8.0f && m_fsplashtimer < 15.0f)
		{
			if(m_fsplashtimer < 10.0f && m_fsplashtimer >= 8.0f)
				AssetManager->DrawFlatSprite(m_nStudioHTSplash,0,0,1.0f,1.0f,0,0,0,0,D3DCOLOR_ARGB(int(0.0f+(m_fsplashtimer-8.0f)*127.5f),255,255,255), true);
			else if(m_fsplashtimer >= 13.0f && m_fsplashtimer < 15.0f)
				AssetManager->DrawFlatSprite(m_nStudioHTSplash,0,0,1.0f,1.0f,0,0,0,0,D3DCOLOR_ARGB(int(255.0f-(m_fsplashtimer-13.0f)*127.5f),255,255,255), true);
			else if(m_fsplashtimer >= 10.0f && m_fsplashtimer < 13.0f)
				AssetManager->DrawFlatSprite(m_nStudioHTSplash,0,0,1.0f,1.0f,0,0,0,0,D3DCOLOR_ARGB(255,255,255,255), true);
		}
		else if (m_fsplashtimer >= 15.0f && m_fsplashtimer < 21.0f)
		{
			if(m_fsplashtimer < 17.0f && m_fsplashtimer >= 15.0f)
				AssetManager->DrawFlatSprite(m_nAtmoTeamSplash,0,0,1.0f,0.75f,0,0,0,0,D3DCOLOR_ARGB(int(0.0f+(m_fsplashtimer-15.0f)*127.5f),255,255,255), true);
			else if(m_fsplashtimer >= 19.0f && m_fsplashtimer < 21.0f)
				AssetManager->DrawFlatSprite(m_nAtmoTeamSplash,0,0,1.0f,0.75f,0,0,0,0,D3DCOLOR_ARGB(int(255.0f-(m_fsplashtimer-19.0f)*127.5f),255,255,255), true);
			else if(m_fsplashtimer >= 17.0f && m_fsplashtimer < 19.0f)
				AssetManager->DrawFlatSprite(m_nAtmoTeamSplash,0,0,1.0f,0.75f,0,0,0,0,D3DCOLOR_ARGB(255,255,255,255), true);
		}
		if(fdt < 1.0f)
		{
			m_fsplashtimer += fdt;
		}
		if(m_fsplashtimer > 21.0f && m_bSplashisDone == false)
		{
			m_bSplashisDone = true;
		}
		if(m_bSplashisDone)
		{
			uberClocked += fdt;
			loadingScreen();
		}
		
		if(m_bLoadisDone && m_bSplashisDone)
		{
			AssetManager->ClearLoadTextures();
			RJ_RENDERER->EndRender();
			m_fsplashtimer = 0;
			break;
		}
		EnterCriticalSection(&m_CritLoad);
		if(m_bUpdateLoadComplete == true)
			m_bLoadisDone = true;
		LeaveCriticalSection(&m_CritLoad);
		RJ_RENDERER->EndRender();
		Sleep(0);
	}
	
	EnterCriticalSection(&m_CritLoadDos);
	m_bRenderLoadComplete = true;
	LeaveCriticalSection(&m_CritLoadDos);

	volatile tScenePause* tNP = new tScenePause(EM_SCENEPAUSE, false);
	CEventManager::GetInstanceThreaded()->SendEventRenderCrossToUpdate(ET_SCENEPAUSE,(tBaseEventData*)tNP);
	tNP = NULL;

	//Push States to begin the program!!
	//PushState(new State_HUD()); // don't need this anymore -AtG
	PushState(new State_MainMenu());
	tSC->m_bIsLoading = true;
	delete Font;
	PushState(new State_Loading());

	return true;
}

bool RJ_GameMain::InternalShutdown(void)
{
	// undo GAMMA
	RJ_RENDERER->SetGammaRamp(0.0f);

	EventManager->UnregisterListenerAllRender(this);
	//SoundEngine->UnloadSound(m_nMusic);
	EnterCriticalSection(&m_CritShutdownDos);
	m_bEndThreadLoop = true;
	LeaveCriticalSection(&m_CritShutdownDos);

	WaitForSingleObject(THREAD_END, INFINITE);

	while(true)
	{
		EnterCriticalSection(&m_CritShutdown);
		if(m_bThreadIsDead == true)
			break;
		LeaveCriticalSection(&m_CritShutdown);
		Sleep(0);
	}
	CloseHandle((HANDLE)m_pThreadHandle);

	ClearAllStates();
	m_TimerUpdate.Stop();
	m_TimerRender.Stop();
	tSC->UnloadStateImages();
	SceneManager->shutdown();
	AssetManager->Shutdown();
	SoundEngine->Shutdown();
	RJ_RENDERER->Shutdown();
	EventManager->ShutdownEventManager();
	DeleteCriticalSection(&m_CritShutdown);
	DeleteCriticalSection(&m_CritShutdownDos);
	DeleteCriticalSection(&m_CritLoad);
	DeleteCriticalSection(&m_CritLoadDos);
	return true;
}
void RJ_GameMain::HandleEvent(CEvent* pEvent)//TEMP RENDERSIDE
{
	if(pEvent->GetEventID() == ET_UPDATESPERSECOND)
	{
		tUpdatesPerSecond* tUPS;
		tUPS = (tUpdatesPerSecond*)pEvent->GetData();
		m_fUPS = tUPS->m_fUpdatesPS;
	}
	else if(pEvent->GetEventID() == ET_SCENEPAUSE)
	{
		tScenePause* tSP;
		tSP = (tScenePause*)pEvent->GetData();
		m_bScenePause = tSP->m_bIsPause;
	}
	else if(pEvent->GetEventID() == ET_PUSHGAMEOVER)
	{
		PushState(new State_GameOver());
	}
	else if(pEvent->GetEventID() == ET_POPSTATE)
	{
		PopState();
	}
	else if(pEvent->GetEventID() == ET_ENTERLEVEL)
	{
		tEnterLevel* tEL;
		tEL = (tEnterLevel*)pEvent->GetData();

		ClearAllStates();
		PushState(new State_Loading());
		PushState(new State_HUD());
		if(tEL->m_Tut)
			PushState(new State_Tutorial());
		PushState(new State_CamFly());
		PushState(new State_Loading());
	}
	
	pEvent->m_nRefs--;
}

void RJ_GameMain::loadingScreen()
{
	if(uberClocked >= 2.1f)
		m_vBanana = D3DXVECTOR2(512.0f + (cosf(D3DX_PI * ((float)uberClocked-1.6f))*300.0f), 384.0f - (sinf(D3DX_PI * ((float)uberClocked-1.6f)) * 250.0f));
	if(uberClocked >= 1.7f)
		m_vPotato = D3DXVECTOR2(512.0f + (cosf(D3DX_PI * ((float)uberClocked-1.2f))*300.0f), 384.0f - (sinf(D3DX_PI * ((float)uberClocked-1.2f)) * 250.0f));
	if(uberClocked >= 1.3f)
		m_vApple = D3DXVECTOR2(512.0f + (cosf(D3DX_PI * ((float)uberClocked-0.8f))*300.0f), 384.0f - (sinf(D3DX_PI * ((float)uberClocked-0.8f)) * 250.0f));
	if(uberClocked >= 0.9f)
		m_vPopsicle = D3DXVECTOR2(512.0f + (cosf(D3DX_PI * ((float)uberClocked-0.4f))*300.0f), 384.0f - (sinf(D3DX_PI * ((float)uberClocked-0.4f)) * 250.0f));
	m_vFish = D3DXVECTOR2(512.0f + (cosf(D3DX_PI * ((float)uberClocked))*300.0f), 384.0f - (sinf(D3DX_PI * ((float)uberClocked)) * 250.0f));


	AssetManager->DrawFlatSprite(m_nLoadingScreen,0.0f,0.0f,2.0f,1.5f, 0, 1.0f, 1.0f, 0.0f, D3DCOLOR_ARGB(255,255,255,255), true);

	//Draw Anthony
	RECT anthony;
	anthony.bottom = 330;
	anthony.top = 160;
	anthony.left = 850;
	anthony.right = 1020;
	AssetManager->DrawFlatSprite(m_nLoadingScreen,m_vAnthony.x - (anthony.right - anthony.left), 
		m_vAnthony.y - (anthony.bottom - anthony.top),1.5f,1.5f, &anthony, 1.0f, 1.0f, 0.0f, D3DCOLOR_ARGB(255,255,255,255), true);

	//Draw Banana
	RECT banana;
	banana.top = 20;
	banana.left = 540;
	banana.bottom = 120;
	banana.right = 670;
	AssetManager->DrawFlatSprite(m_nLoadingScreen,m_vBanana.x - (banana.right - banana.left), 
		m_vBanana.y - (banana.bottom - banana.top),1.5f,1.5f, &banana, 1.0f, 1.0f, 0.0f, D3DCOLOR_ARGB(255,255,255,255), true);

	//Draw Potato
	RECT potato;
	potato.top = 35;
	potato.left = 735;
	potato.bottom = 120;
	potato.right = 860;
	AssetManager->DrawFlatSprite(m_nLoadingScreen,m_vPotato.x - (potato.right - potato.left), 
		m_vPotato.y - (potato.bottom - potato.top),1.5f,1.5f, &potato, 1.0f, 1.0f, 0.0f, D3DCOLOR_ARGB(255,255,255,255), true);

	//Draw Apple
	RECT apple;
	apple.top = 200;
	apple.left = 530;
	apple.bottom = 315;
	apple.right = 630;
	AssetManager->DrawFlatSprite(m_nLoadingScreen,m_vApple.x - (apple.right - apple.left), 
		m_vApple.y - (apple.bottom - apple.top),1.5f,1.5f, &apple, 1.0f, 1.0f, 0.0f, D3DCOLOR_ARGB(255,255,255,255), true);

	//Draw Popsicle
	RECT popsicle;
	popsicle.top = 5;
	popsicle.left = 935;
	popsicle.bottom = 155;
	popsicle.right = 1010;
	AssetManager->DrawFlatSprite(m_nLoadingScreen,m_vPopsicle.x - (popsicle.right - popsicle.left), 
		m_vPopsicle.y - (popsicle.bottom - popsicle.top),1.5f,1.5f, &popsicle, 1.0f, 1.0f, 0.0f, D3DCOLOR_ARGB(255,255,255,255), true);

	//Draw Fish
	RECT fish;
	fish.top = 210;
	fish.left = 675;
	fish.bottom = 315;
	fish.right = 835;
	AssetManager->DrawFlatSprite(m_nLoadingScreen,m_vFish.x - (fish.right - fish.left), 
		m_vFish.y - (fish.bottom - fish.top),1.5f,1.5f, &fish, 1.0f, 1.0f, 0.0f, D3DCOLOR_ARGB(255,255,255,255), true);

	//static int counter = 0;
	if((int)uberClocked%3 < 1)
		Font->DrawString("LOADING!",20,700);
	else if(((int)uberClocked%3) - 1 < 1)
		Font->DrawString("LOADING!!",20,700);
	else
		Font->DrawString("LOADING!!!",20,700);
}