#include "State_Pause.h"
#include "State_MainMenu.h"
#include "State_Options.h"
#include "../Asset/AssetManager.h"
#include "../Input/CInputManager.h"
#include "../../../RJ_Config.h"
#include "../../RJ_GameMain.h"
#include "../Renderer/RJ_Renderer.h"
#include "State_Config.h"
#include "../Text/font_defines.h"

State_Pause::State_Pause(void) : Font("t_Crumbs_font.dds","Resources/Fonts/Crumbs_font.dat")
{
	m_nSelection = eMIN;
	tSC = NULL; 

	start = D3DXVECTOR3(255.0f, 768.0f, 0.0f);
	end = D3DXVECTOR3(255.0f, 128.0f, 0.0f);
	temp = start;

	Astart = D3DXVECTOR3(256+186, 128+89 + 768, 0.0f);
	Aend = D3DXVECTOR3(256+186, 128+89, 0.0f);
	Atemp = Astart;
	lerpdown = 0;
}

void State_Pause::OnPush(void)
{
	if(tSC == NULL)
		tSC = tStateConfig::GetInstanceRender();
}

void State_Pause::OnEnter(void)
{

}

void State_Pause::OnExit(void)
{

}
void State_Pause::SendPauseEvent(void)
{
	volatile tPause* tNP = new tPause(EM_PAUSE);
	CEventManager::GetInstanceThreaded()->SendEventRenderCrossToUpdate(ET_PAUSE,(tBaseEventData*)tNP);
	tNP = NULL;
	RJ_MAIN_GAME->setRenderPause(false);
}
void State_Pause::SendShutdownEvent(void)
{
	volatile tGamePlayShutdown* tNP = new tGamePlayShutdown(EM_GAMEPLAYSHUTDOWN);
	CEventManager::GetInstanceThreaded()->SendEventRenderCrossToUpdate(ET_GAMEPLAYSHUTDOWN,(tBaseEventData*)tNP);
	tNP = NULL;

	volatile tLevelSelect* tLS = new tLevelSelect(EM_LEVELSELECT, 0);
	CEventManager::GetInstanceThreaded()->SendEventRenderCrossToUpdate(ET_LEVELSELECT,(tBaseEventData*)tLS);
	tLS = NULL;
}
void State_Pause::SendRestartEvent(void)
{
	volatile tRestartGame* tRG = new tRestartGame(EM_RESTARTGAME);
	CEventManager::GetInstanceThreaded()->SendEventRenderCrossToUpdate(ET_RESTARTGAME,(tBaseEventData*)tRG);
	tRG = NULL;
	volatile tToggleAllBits* tAB = new tToggleAllBits(EM_TOGGLEALLBITS, true);
	CEventManager::GetInstanceThreaded()->SendEventUpdateLocal(ET_TOGGLEALLBITS,(tBaseEventData*)tAB);
	tAB = NULL;
}

bool State_Pause::Input(double _fDelta)
{
	/*if(CInputManager::GetInstanceRender()->getKeysPressed() & RJ_Config::AP_CANCEL)
	{
	PostQuitMessage(0);
	}*/
	if (lerpdown == 3)
	{
		if(CInputManager::GetInstanceRender()->getKeysPressed() & RJ_Config::AP_UP)
		{
			m_nSelection--;
			if(m_nSelection < eMIN)
				m_nSelection = eMAX;
			volatile tPlaySoundEnum* tSOUND = new tPlaySoundEnum(EM_PLAYSOUNDENUM, RJ_Config::RJ_UI_NAVIGATE,true); 
			CEventManager::GetInstanceThreaded()->SendEventRenderLocal(ET_PLAYSOUND,(tBaseEventData*)tSOUND);
			tSOUND = NULL;
		}
		else if(CInputManager::GetInstanceRender()->getKeysPressed() & RJ_Config::AP_DOWN)
		{
			m_nSelection++;
			if(m_nSelection > eMAX)
				m_nSelection = eMIN;
			volatile tPlaySoundEnum* tSOUND = new tPlaySoundEnum(EM_PLAYSOUNDENUM, RJ_Config::RJ_UI_NAVIGATE,true); 
			CEventManager::GetInstanceThreaded()->SendEventRenderLocal(ET_PLAYSOUND,(tBaseEventData*)tSOUND);
			tSOUND = NULL;
		}

		if(CInputManager::GetInstanceRender()->getKeysPressed() & RJ_Config::AP_CONFIRM)
		{
			volatile tPlaySoundEnum* tSOUND = new tPlaySoundEnum(EM_PLAYSOUNDENUM, RJ_Config::RJ_UI_SELECT,true); 
			CEventManager::GetInstanceThreaded()->SendEventRenderLocal(ET_PLAYSOUND,(tBaseEventData*)tSOUND);
			tSOUND = NULL;
			switch(m_nSelection)
			{
			case eRESUME:
				{	
					//tSC->m_nNumberofPlayers = 1;
					lerpdown = 1;
				} break;
			case eRESTART:
				{
					// restart and unpause
					SendRestartEvent();
					SendPauseEvent(); 
					RJ_MAIN_GAME->PopState();
				}
				break;
			case eOPTIONS:
				{
					//change to options menu once that is done
					RJ_MAIN_GAME->PushState(new State_Options);
				} break;
			case eMAINMENU:
				{	
					SendShutdownEvent();
					int temp = tSC->m_nNumberofPlayers * 6;
					for (int i = 0; i < temp; ++i)
					{
						RJ_RENDERER->GetOpaqueObjectList()->pop_back();
					}

					RJ_MAIN_GAME->PopState();
					//RJ_MAIN_GAME->PopState();
					RJ_MAIN_GAME->PushState(new State_MainMenu());
				} break;
			//case eEXIT:
			//	{
			//		PostQuitMessage(0);
			//	} break;
			}
		}
	}

	return true;
}

bool State_Pause::InputUnder(double _fDelta)
{
	return true;
}

bool State_Pause::Update(double _fDelta)
{
	return true;
}

bool State_Pause::UpdateUnder(double _fDelta)
{
	return true;
}

bool State_Pause::Render(double _fDelta)
{
	float lerp = 2.5f * (float)_fDelta;
	if(lerpdown == 1)
	{
		end.y = 800.0f;
		D3DXVec3Lerp(&temp, &start, &end, lerp);
		start = temp;
		CAssetManager::getInstanceRender()->DrawFlatSprite(tSC->m_nPauseBG,temp.x,temp.y,1.0f,1.0f);
		if(start.y >= 768.0f)
			lerpdown = 2;
	}
	else if(lerpdown == 2)
	{
		SendPauseEvent();
		RJ_MAIN_GAME->PopState();
	}
	else if (lerpdown == 0)
	{
		D3DXVec3Lerp(&temp, &start, &end, lerp);
		start = temp;
		CAssetManager::getInstanceRender()->DrawFlatSprite(tSC->m_nPauseBG,temp.x,temp.y,1.0f,1.0f);

		if(start.y - end.y <= 1.0f)
			lerpdown = 3;

	}
	else if(lerpdown == 3)
	{
		//Font.DrawString("Resume",300,330,1.0f,1.0f,0.0f,D3DCOLOR_ARGB(255,255,255,255));
		//Font.DrawString("Main Menu",300,360,1.0f,1.0f,0.0f,D3DCOLOR_ARGB(255,255,255,255));
		//Font.DrawString("Exit",300,390,1.0f,1.0f,0.0f,D3DCOLOR_ARGB(255,255,255,255));
		D3DXVec3Lerp(&temp, &start, &end, lerp);
		start = temp;
		CAssetManager::getInstanceRender()->DrawFlatSprite(tSC->m_nPauseBG,temp.x,temp.y,1.0f,1.0f);

		char buffer[2] = "";
		buffer[0] = (char)IMG_UP;

		switch(m_nSelection)
		{
		case eRESUME:
			{
				Font.DrawString(buffer, 256+186,128+89, 1.0f, 1.0f, D3DX_PI/2);
			} break;
		case eRESTART:
			{
				Font.DrawString(buffer, 256+178,128+168, 1.0f, 1.0f, D3DX_PI/2);
			} break;
		case eOPTIONS:
			{
				Font.DrawString(buffer, 256+187,128+243, 1.0f, 1.0f, D3DX_PI/2);
			} break;
		case eMAINMENU:
			{
				Font.DrawString(buffer, 256+164,128+318, 1.0f, 1.0f, D3DX_PI/2);
			} break;
		//case eEXIT:
		//	{
		//		Font.DrawString(buffer, 256+209,128+397, 1.0f, 1.0f, D3DX_PI/2);
		//	} break;
		}
	}

	return true;
}

bool State_Pause::RenderUnder(double _fDelta)
{
	CAssetManager::getInstanceRender()->DrawFlatSprite(tSC->m_nPauseBG,temp.x,temp.y,1.0f,1.0f);

	return true;
}