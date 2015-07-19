#include "State_Tutorial.h"
#include "State_MainMenu.h"
#include "State_Options.h"
#include "../Asset/AssetManager.h"
#include "../Input/CInputManager.h"
#include "../../../RJ_Config.h"
#include "../../RJ_GameMain.h"
#include "../Renderer/RJ_Renderer.h"
#include "State_Config.h"
#include "../Text/font_defines.h"

State_Tutorial::State_Tutorial() : Font("t_Crumbs_font.dds","Resources/Fonts/Crumbs_font.dat")
{
	m_nSelection = eMIN;
	CurrScreen = etMIN;
	PrevScreen = -1;
	tSC = NULL;

	start = D3DXVECTOR3(1024.0f, 0.0f, 0.0f);
	end = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	gone = D3DXVECTOR3(-1024.0f, 0.0f, 0.0f);

	Pstart = D3DXVECTOR3(1024.0f, 0.0f, 0.0f);
	Pend = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	Pgone = D3DXVECTOR3(-1024.0f, 0.0f, 0.0f);

	temp = start;

	lerpdown = 3;
	Plerpdown = 3;

	//Screen 0
	//Do you want to see
	//Screen 1
	//Controls?
	//Screen 2
	//Scoring?
	//Screen 3
	//Power Ups?
	//Screen 4
	//Hazards?
	//Screen 5
	//HUD?
	//Screen 6
	//Turn off next time
}

void State_Tutorial::OnPush(void)
{
	if(tSC == NULL)
		tSC = tStateConfig::GetInstanceRender();
}

void State_Tutorial::OnEnter(void)
{
	volatile tPause* tNP = new tPause(EM_PAUSE, true);
	CEventManager::GetInstanceThreaded()->SendEventRenderCrossToUpdate(ET_PAUSE,(tBaseEventData*)tNP);
	tNP = NULL;
}

void State_Tutorial::OnExit(void)
{
}
void State_Tutorial::SendPauseEvent(void)
{
	volatile tScenePause* tSP = new tScenePause(EM_SCENEPAUSE);
	CEventManager::GetInstanceThreaded()->SendEventRenderCrossToUpdate(ET_SCENEPAUSE,(tBaseEventData*)tSP);
	tSP = NULL;
}
void State_Tutorial::SendShutdownEvent(void)
{
	volatile tGamePlayShutdown* tNP = new tGamePlayShutdown(EM_GAMEPLAYSHUTDOWN);
	CEventManager::GetInstanceThreaded()->SendEventRenderCrossToUpdate(ET_GAMEPLAYSHUTDOWN,(tBaseEventData*)tNP);
	tNP = NULL;

	volatile tLevelSelect* tLS = new tLevelSelect(EM_LEVELSELECT, 0);
	CEventManager::GetInstanceThreaded()->SendEventRenderCrossToUpdate(ET_LEVELSELECT,(tBaseEventData*)tLS);
	tLS = NULL;
}
void State_Tutorial::SendRestartEvent(void)
{
	volatile tRestartGame* tRG = new tRestartGame(EM_RESTARTGAME);
	CEventManager::GetInstanceThreaded()->SendEventRenderCrossToUpdate(ET_RESTARTGAME,(tBaseEventData*)tRG);
	tRG = NULL;
	volatile tToggleAllBits* tAB = new tToggleAllBits(EM_TOGGLEALLBITS, true);
	CEventManager::GetInstanceThreaded()->SendEventRenderCrossToUpdate(ET_TOGGLEALLBITS,(tBaseEventData*)tAB);
	tAB = NULL;
}

bool State_Tutorial::Input(double _fDelta)
{
	if(CInputManager::GetInstanceRender()->getKeysPressed() & RJ_Config::AP_CANCEL)
	{
		PrevScreen = CurrScreen;
		CurrScreen = etMAX;
		m_nSelection = eYES;

		lerpdown = 1;

		start.x = 1024.0f;
		end.x = 0.0f;
		gone.x = -1024.0f;
		Pstart.x = 1024.0f;
		Pgone.x = -1024.0f;
		Pend.x = 0.0f;
	}
	//////////////////////////////////////////////////////////////////////////
	//Screen 0 Input
	if(CurrScreen == etVIEW)
	{
		if(CInputManager::GetInstanceRender()->getKeysPressed() & RJ_Config::AP_UP)
		{
			if(m_nSelection == eMAX)
				m_nSelection = eMIN;
			else if(m_nSelection == eMIN)
				m_nSelection = eMAX;
			volatile tPlaySoundEnum* tSOUND = new tPlaySoundEnum(EM_PLAYSOUNDENUM, RJ_Config::RJ_UI_NAVIGATE,true); 
			CEventManager::GetInstanceThreaded()->SendEventRenderLocal(ET_PLAYSOUND,(tBaseEventData*)tSOUND);
			tSOUND = NULL;
		}
		else if(CInputManager::GetInstanceRender()->getKeysPressed() & RJ_Config::AP_DOWN)
		{
			if(m_nSelection == eMAX)
				m_nSelection = eMIN;
			else if(m_nSelection == eMIN)
				m_nSelection = eMAX;
			volatile tPlaySoundEnum* tSOUND = new tPlaySoundEnum(EM_PLAYSOUNDENUM, RJ_Config::RJ_UI_NAVIGATE,true); 
			CEventManager::GetInstanceThreaded()->SendEventRenderLocal(ET_PLAYSOUND,(tBaseEventData*)tSOUND);
			tSOUND = NULL;
		}

		if(CInputManager::GetInstanceRender()->getKeysPressed() & RJ_Config::AP_CONFIRM)
		{
			volatile tPlaySoundEnum* tSOUND = new tPlaySoundEnum(EM_PLAYSOUNDENUM, RJ_Config::RJ_UI_SELECT,true); 
			CEventManager::GetInstanceThreaded()->SendEventRenderLocal(ET_PLAYSOUND,(tBaseEventData*)tSOUND);
			tSOUND = NULL;

			switch (m_nSelection)
			{
			case eYES:
				{
					PrevScreen = CurrScreen;
					CurrScreen = etPRECONTROLS;
					m_nSelection = eNONE;
				}
				break;
			case eNO:
				{
					volatile tToggleTut* tNP = new tToggleTut(EM_TOGGLETUT, false);
					CEventManager::GetInstanceThreaded()->SendEventRenderCrossToUpdate(ET_TOGGLETUT,(tBaseEventData*)tNP);
					tNP = NULL;

					volatile tTutOver* tTO = new tTutOver(EM_TUTOVER);
					CEventManager::GetInstanceThreaded()->SendEventRenderCrossToUpdate(ET_TUTOVER,(tBaseEventData*)tTO);
					tTO = NULL;

					RJ_MAIN_GAME->PopState();
				}
				break;
			}
		}
	}
	//End Screen 0 Input
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	//Screen 1-7 Input
	if(CurrScreen > etMIN && CurrScreen < etMAX)
	{
		if(CInputManager::GetInstanceRender()->getKeysPressed() & RJ_Config::AP_LEFT)
		{
			PrevScreen = CurrScreen;
			CurrScreen--;
			if(CurrScreen == etMIN)
				CurrScreen = etMAX - 1;
			volatile tPlaySoundEnum* tSOUND = new tPlaySoundEnum(EM_PLAYSOUNDENUM, RJ_Config::RJ_UI_NAVIGATE,true); 
			CEventManager::GetInstanceThreaded()->SendEventRenderLocal(ET_PLAYSOUND,(tBaseEventData*)tSOUND);
			tSOUND = NULL;
			
			lerpdown = 0;
			
			start.x = 1024.0f;
			end.x = 0.0f;
			gone.x = -1024.0f;
			Pstart.x = 1024.0f;
			Pgone.x = -1024.0f;
			Pend.x = 0.0f;

		}
		else if(CInputManager::GetInstanceRender()->getKeysPressed() & RJ_Config::AP_RIGHT)
		{
			PrevScreen = CurrScreen;
			CurrScreen++;

			if (CurrScreen == etMAX)
			{
				m_nSelection = eYES;
			}

			volatile tPlaySoundEnum* tSOUND = new tPlaySoundEnum(EM_PLAYSOUNDENUM, RJ_Config::RJ_UI_NAVIGATE,true); 
			CEventManager::GetInstanceThreaded()->SendEventRenderLocal(ET_PLAYSOUND,(tBaseEventData*)tSOUND);
			tSOUND = NULL;
			
			lerpdown = 1;
			
			start.x = 1024.0f;
			end.x = 0.0f;
			gone.x = -1024.0f;
			Pstart.x = 1024.0f;
			Pgone.x = -1024.0f;
			Pend.x = 0.0f;
		}
	}
	//End Screen 1-7 Input
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	//Screen 8 Input
	if(CurrScreen == etMAX)
	{
		if(CInputManager::GetInstanceRender()->getKeysPressed() & RJ_Config::AP_LEFT)
		{
			PrevScreen = CurrScreen;
			CurrScreen = etMAX - 1;
			m_nSelection = eNONE;
			
			lerpdown = 0;
			
			start.x = 1024.0f;
			end.x = 0.0f;
			gone.x = -1024.0f;
			Pstart.x = 1024.0f;
			Pgone.x = -1024.0f;
			Pend.x = 0.0f;
		}
		if(CInputManager::GetInstanceRender()->getKeysPressed() & RJ_Config::AP_UP)
		{
			if(m_nSelection == eMAX)
				m_nSelection = eMIN;
			else if(m_nSelection == eMIN)
				m_nSelection = eMAX;
			volatile tPlaySoundEnum* tSOUND = new tPlaySoundEnum(EM_PLAYSOUNDENUM, RJ_Config::RJ_UI_NAVIGATE,true); 
			CEventManager::GetInstanceThreaded()->SendEventRenderLocal(ET_PLAYSOUND,(tBaseEventData*)tSOUND);
			tSOUND = NULL;
		}
		else if(CInputManager::GetInstanceRender()->getKeysPressed() & RJ_Config::AP_DOWN)
		{
			if(m_nSelection == eMAX)
				m_nSelection = eMIN;
			else if(m_nSelection == eMIN)
				m_nSelection = eMAX;
			volatile tPlaySoundEnum* tSOUND = new tPlaySoundEnum(EM_PLAYSOUNDENUM, RJ_Config::RJ_UI_NAVIGATE,true); 
			CEventManager::GetInstanceThreaded()->SendEventRenderLocal(ET_PLAYSOUND,(tBaseEventData*)tSOUND);
			tSOUND = NULL;
		}

		if(CInputManager::GetInstanceRender()->getKeysPressed() & RJ_Config::AP_CONFIRM)
		{
			volatile tPlaySoundEnum* tSOUND = new tPlaySoundEnum(EM_PLAYSOUNDENUM, RJ_Config::RJ_UI_SELECT,true); 
			CEventManager::GetInstanceThreaded()->SendEventRenderLocal(ET_PLAYSOUND,(tBaseEventData*)tSOUND);
			tSOUND = NULL;

			switch (m_nSelection)
			{
			case eYES:
				{
					volatile tTutOver* tNP = new tTutOver(EM_TUTOVER);
					CEventManager::GetInstanceThreaded()->SendEventRenderCrossToUpdate(ET_TUTOVER,(tBaseEventData*)tNP);
					tNP = NULL;

					RJ_MAIN_GAME->PopState();
				}
				break;
			case eNO:
				{
					volatile tToggleTut* tNP = new tToggleTut(EM_TOGGLETUT, false);
					CEventManager::GetInstanceThreaded()->SendEventRenderCrossToUpdate(ET_TOGGLETUT,(tBaseEventData*)tNP);
					tNP = NULL;

					volatile tTutOver* tTO = new tTutOver(EM_TUTOVER);
					CEventManager::GetInstanceThreaded()->SendEventRenderCrossToUpdate(ET_TUTOVER,(tBaseEventData*)tTO);
					tTO = NULL;

					RJ_MAIN_GAME->PopState();
				}
				break;
			}
		}
	}
	//End Screen 8 Input
	//////////////////////////////////////////////////////////////////////////

	return true;
}

bool State_Tutorial::InputUnder(double _fDelta)
{
	return true;
}

bool State_Tutorial::Update(double _fDelta)
{
	return true;
}

bool State_Tutorial::UpdateUnder(double _fDelta)
{
	return true;
}

bool State_Tutorial::Render(double _fDelta)
{
	float lerp = 2.5f * (float)_fDelta;
	if(lerpdown == 1)
	{
		D3DXVec3Lerp(&temp, &start, &end, lerp);
		start = temp;
		CAssetManager::getInstanceRender()->DrawFlatSprite(tSC->m_nTutorialIDs[CurrScreen],temp.x,temp.y,1.0f,1.0f);
		if(PrevScreen != -1)
		{
			D3DXVec3Lerp(&Ptemp, &Pend, &Pgone, lerp);
			Pend = Ptemp;
			CAssetManager::getInstanceRender()->DrawFlatSprite(tSC->m_nTutorialIDs[PrevScreen],Ptemp.x,Ptemp.y,1.0f,1.0f);
		}
		if(start.x - end.x <= 1.0f)
			lerpdown = 2;
		Plerpdown = 1;
	}
	else if (lerpdown == 0)
	{
		D3DXVec3Lerp(&temp, &gone, &end, lerp);
		gone = temp;
		CAssetManager::getInstanceRender()->DrawFlatSprite(tSC->m_nTutorialIDs[CurrScreen],temp.x,temp.y,1.0f,1.0f);
		if(PrevScreen != -1)
		{
			D3DXVec3Lerp(&Ptemp, &Pend, &Pstart, lerp);
			Pend = Ptemp;
			CAssetManager::getInstanceRender()->DrawFlatSprite(tSC->m_nTutorialIDs[PrevScreen],Ptemp.x,Ptemp.y,1.0f,1.0f);
		}
		if(gone.x - end.x >= -1.0f)
			lerpdown = 2;
		Plerpdown = 0;

	}
	else if(lerpdown == 2)
	{
		if(Plerpdown == 1)
		{
			D3DXVec3Lerp(&temp, &start, &end, lerp);
			start = temp;
		}
		else if(Plerpdown == 0)
		{
			D3DXVec3Lerp(&temp, &gone, &end, lerp);
			gone = temp;
		}
		
		CAssetManager::getInstanceRender()->DrawFlatSprite(tSC->m_nTutorialIDs[CurrScreen],temp.x,temp.y,1.0f,1.0f);
		if(PrevScreen != -1)
		{
			if(Plerpdown == 1)
			{
				D3DXVec3Lerp(&Ptemp, &Pend, &Pgone, lerp);
				Pstart = Ptemp;
			}
			else if(Plerpdown == 0)
			{
				D3DXVec3Lerp(&Ptemp, &Pend, &Pstart, lerp);
				Pgone = Ptemp;
			}
			CAssetManager::getInstanceRender()->DrawFlatSprite(tSC->m_nTutorialIDs[PrevScreen],Ptemp.x,Ptemp.y,1.0f,1.0f);
		}
		char buffer[2] = "";
		buffer[0] = (char)IMG_UP;

		switch(m_nSelection)
		{
		case eYES:
			{
				Font.DrawString(buffer, 480,365, 1.0f, 1.0f, D3DX_PI/2);
			} break;
		case eNONE:
			{
				//No Arrow
			} break;
		case eNO:
			{
				Font.DrawString(buffer, 480,415, 1.0f, 1.0f, D3DX_PI/2);
			} break;
		}
	}
	else if(lerpdown == 3)
	{
		CAssetManager::getInstanceRender()->DrawFlatSprite(tSC->m_nTutorialIDs[CurrScreen],0.0f,0.0f,1.0f,1.0f);
		char buffer[2] = "";
		buffer[0] = (char)IMG_UP;

		switch(m_nSelection)
		{
		case eYES:
			{
				Font.DrawString(buffer, 480,365, 1.0f, 1.0f, D3DX_PI/2);
			} break;
		case eNONE:
			{
				//No Arrow
			} break;
		case eNO:
			{
				Font.DrawString(buffer, 480,415, 1.0f, 1.0f, D3DX_PI/2);
			} break;
		}
	}

	return true;
}

bool State_Tutorial::RenderUnder(double _fDelta)
{
	//CAssetManager::getInstanceRender()->DrawFlatSprite(tSC->m_nPauseBG,temp.x,temp.y,1.0f,1.0f);

	return true;
}