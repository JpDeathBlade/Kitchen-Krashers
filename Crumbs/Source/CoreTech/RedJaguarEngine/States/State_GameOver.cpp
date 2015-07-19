#include "State_GameOver.h"
#include "..\\Input\\CInputManager.h"
#include "..\\..\\..\\RJ_Config.h"
#include "../Asset/AssetManager.h"
#include "../Event/EventManager.h"
#include "..\\..\\RJ_GameMain.h"
#include "../../../Gameplay/Objects/CPlayer.h"
#include "../../../Gameplay/Gameplay.h"
#include "State_MainMenu.h"


State_GameOver::State_GameOver(void) : Font("t_Crumbs_font.dds","Resources/Fonts/Crumbs_font.dat"), DakotaFont("t_Dakota_font.dds","Resources/Fonts/dakota_font.dat")
{
	CEventManager::GetInstanceThreaded()->RegisterListenerRender(ET_PLAYERDATA,this);

	m_nSelection = eMIN;
	Gameplay = CGameplay::GetInstanceUpdate();
	Config = tStateConfig::GetInstanceRender();

	m_vColors[0][0] = 153; m_vColors[0][1] = 13; m_vColors[0][2] = 0;
	m_vColors[1][0] = 72;  m_vColors[1][1] = 27; m_vColors[1][2] = 108;
	m_vColors[2][0] = 216; m_vColors[2][1] = 118; m_vColors[2][2] = 6;
	m_vColors[3][0] = 12;  m_vColors[3][1] = 204; m_vColors[3][2] = 87;

	Standing[0] = 0;
	Standing[1] = 0;
	Standing[2] = 0;
	Standing[3] = 0;
}
void State_GameOver::OnPush(void){}
void State_GameOver::OnEnter(void){}
void State_GameOver::OnPop()
{
	CEventManager::GetInstanceThreaded()->UnregisterListenerRender(ET_PLAYERDATA,this);

	delete this;
}

void State_GameOver::OnExit(void)
{

}
void State_GameOver::HandleEvent(CEvent* pEvent)
{
	if(pEvent->GetEventID() == ET_PLAYERDATA)
	{
		if(pEvent->GetData() != NULL)
		{
			if(pEvent->GetData()->GetMsgID() == EM_PLAYERDATA)
			{
				tPlayerData* tPD;
				tPD = (tPlayerData*)pEvent->GetData();
				playDat = *tPD;
				par = playDat.m_nPar;
				for(int _player = 0; _player < Config->m_nNumberofPlayers; ++_player)
				{
					Score[_player] = playDat.m_nScores[_player];
					Standing[_player] = playDat.m_nStanding[_player];
				}

			}
		}
	}

	pEvent->m_nRefs--;
}
void State_GameOver::SendShutdownEvent(void)
{
	volatile tGamePlayShutdown* tNP = new tGamePlayShutdown(EM_GAMEPLAYSHUTDOWN);
	CEventManager::GetInstanceThreaded()->SendEventRenderCrossToUpdate(ET_GAMEPLAYSHUTDOWN,(tBaseEventData*)tNP);
	tNP = NULL;

	volatile tLevelSelect* tLS = new tLevelSelect(EM_LEVELSELECT, 0);
	CEventManager::GetInstanceThreaded()->SendEventRenderCrossToUpdate(ET_LEVELSELECT,(tBaseEventData*)tLS);
	tLS = NULL;
}
bool State_GameOver::Input(double _fDelta)
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
			case ePLAYAGAIN:
			{	
				volatile tRestartGame* tRestart = new tRestartGame(EM_RESTARTGAME); 
				CEventManager::GetInstanceThreaded()->SendEventRenderCrossToUpdate(ET_RESTARTGAME,(tBaseEventData*)tRestart);
				tRestart = NULL;
				RJ_MAIN_GAME->PopState();
			} break;
			case eMENU:
				{
					SendShutdownEvent();
					int temp = Config->m_nNumberofPlayers * 6;
					for (int i = 0; i < temp; ++i)
					{
						RJ_RENDERER->GetOpaqueObjectList()->pop_back();
					}

					RJ_MAIN_GAME->PopState();
					//RJ_MAIN_GAME->PopState();
					RJ_MAIN_GAME->PushState(new State_MainMenu());
				} break;

			//case eEXIT:
			//{
			//	PostQuitMessage(0);
			//} break;
		}
	}

	return true;
}

bool State_GameOver::InputUnder(double _fDelta)	{ return true; }
bool State_GameOver::Update(double _fDelta)		{ return true; }
bool State_GameOver::UpdateUnder(double _fDelta){ return true; }

bool State_GameOver::Render(double _fDelta)
{
	CAssetManager::getInstanceRender()->DrawFlatSprite(Config->m_nGameOverBuildSetID,0.0f,0.0f,1.0f,0.75f);

	//Font.DrawString("Game Over!",300,100);

	DakotaFont.DrawString("play again", 86, 565, 1.0f, 1.0f, 0.0f, D3DCOLOR_XRGB(0,0,0));
	DakotaFont.DrawString("main menu", 86, 618, 1.0f, 1.0f, 0.0f, D3DCOLOR_XRGB(0,0,0));
	//DakotaFont.DrawString("exit game", 86, 660, 1.0f, 1.0f, 0.0f, D3DCOLOR_XRGB(0,0,0));

	switch(m_nSelection)
	{
		case ePLAYAGAIN:
			{
				DakotaFont.DrawString("play again",86,565,1.0f, 1.0f, 0.0f, D3DCOLOR_XRGB(0,0,203));
			} break;
		case eMENU:
			{
				DakotaFont.DrawString("main menu",86,618,1.0f, 1.0f, 0.0f, D3DCOLOR_XRGB(0,0,203));
			} break;

		//case eEXIT:
		//	{
		//		DakotaFont.DrawString("exit game",86,660,1.0f, 1.0f, 0.0f, D3DCOLOR_XRGB(0,0,203));
		//	} break;
	}
	
	char szBuffer[128];

	/// <Player 1>
	sprintf_s(szBuffer, "1) Player %i", Standing[0] + 1);
	Font.DrawString(szBuffer, 414, 334, 1.0f, 1.0f, 0.0f,
		D3DCOLOR_ARGB(245, m_vColors[Standing[0]][0], m_vColors[Standing[0]][1], m_vColors[Standing[0]][2]));
	sprintf_s(szBuffer, "%i", Score[0]);
	Font.DrawString(szBuffer, 574, 334, 1.0f, 1.0f, 0.0f,
		D3DCOLOR_ARGB(245, m_vColors[Standing[0]][0], m_vColors[Standing[0]][1], m_vColors[Standing[0]][2]));

	if(Config->m_nNumberofPlayers >= 2)
	{
		sprintf_s(szBuffer, "2) Player %i", Standing[1] + 1);
		Font.DrawString(szBuffer, 414, 404, 1.0f, 1.0f, 0.0f,
			D3DCOLOR_ARGB(245, m_vColors[Standing[1]][0], m_vColors[Standing[1]][1], m_vColors[Standing[1]][2]));
		sprintf_s(szBuffer, "%i", Score[1]);
		Font.DrawString(szBuffer, 574, 404, 1.0f, 1.0f, 0.0f,
			D3DCOLOR_ARGB(245, m_vColors[Standing[1]][0], m_vColors[Standing[1]][1], m_vColors[Standing[1]][2]));

		if(Config->m_nNumberofPlayers >= 3)
		{
			sprintf_s(szBuffer, "3) Player %i", Standing[2] + 1);
			Font.DrawString(szBuffer, 414, 478, 1.0f, 1.0f, 0.0f,
				D3DCOLOR_ARGB(245, m_vColors[Standing[2]][0], m_vColors[Standing[2]][1], m_vColors[Standing[2]][2]));
			sprintf_s(szBuffer, "%i", Score[2]);
			Font.DrawString(szBuffer, 574, 478, 1.0f, 1.0f, 0.0f,
				D3DCOLOR_ARGB(245, m_vColors[Standing[2]][0], m_vColors[Standing[2]][1], m_vColors[Standing[2]][2]));
		
			if(Config->m_nNumberofPlayers >= 4)
			{
				sprintf_s(szBuffer, "4) Player %i", Standing[3] + 1);
				Font.DrawString(szBuffer, 414, 550, 1.0f, 1.0f, 0.0f,
					D3DCOLOR_ARGB(245, m_vColors[Standing[3]][0], m_vColors[Standing[3]][1], m_vColors[Standing[3]][2]));
				sprintf_s(szBuffer, "%i", Score[3]);
				Font.DrawString(szBuffer, 574, 550, 1.0f, 1.0f, 0.0f,
					D3DCOLOR_ARGB(245, m_vColors[Standing[3]][0], m_vColors[Standing[3]][1], m_vColors[Standing[3]][2]));
			}
		}
	}

	else if(Config->m_nNumberofPlayers == 1)
	{
		if (Score[0] > par)
		{
			sprintf_s(szBuffer, "Par");
			Font.DrawString(szBuffer, 414, 404, 1.0f, 1.0f, 0.0f, D3DCOLOR_ARGB(245, 0, 0, 0));
			sprintf_s(szBuffer, "%i", par);
			Font.DrawString(szBuffer, 574, 404, 1.0f, 1.0f, 0.0f, D3DCOLOR_ARGB(245, 0, 0, 0));
			sprintf_s(szBuffer, "You Win!");
			Font.DrawString(szBuffer, 414, 478, 1.0f, 1.0f, 0.0f, D3DCOLOR_ARGB(245, 0, 0, 0));
		}
		else if(Score[0] < par)
		{
			sprintf_s(szBuffer, "Par");
			Font.DrawString(szBuffer, 414, 404, 1.0f, 1.0f, 0.0f, D3DCOLOR_ARGB(245, 0, 0, 0));
			sprintf_s(szBuffer, "%i", par);
			Font.DrawString(szBuffer, 574, 404, 1.0f, 1.0f, 0.0f, D3DCOLOR_ARGB(245, 0, 0, 0));
			sprintf_s(szBuffer, "You Lose! Good day, Sir!");
			Font.DrawString(szBuffer, 414, 478, 1.0f, 1.0f, 0.0f, D3DCOLOR_ARGB(245, 0, 0, 0));
		}
	}

	return true;
}

bool State_GameOver::RenderUnder(double _fDelta){ return true; }