#include "State_LevelMenu.h"
#include "State_PlayerMenu.h"
#include "State_Loading.h"
#include "State_CamFly.h"
#include "State_Tutorial.h"
#include "State_HUD.h"
#include "../Asset/AssetManager.h"
#include "..\\Input\\CInputManager.h"
//#include "..\\Input\\Cursor.h"
#include "..\\..\\..\\RJ_Config.h"
#include "..\\..\\RJ_GameMain.h"
#include "../Text/font_defines.h"

#include "State_Config.h"

State_LevelMenu::State_LevelMenu(void) : Font("t_Dakota_font.dds","Resources/Fonts/Dakota_font.dat")
{
	m_nSelection = eMIN;
	tSC = NULL;
	renderSelection = false;
	m_bTutorial = true;
	LoadSettings();

	CEventManager::GetInstanceThreaded()->RegisterListenerRender(ET_CAMERADONEMOVING,this);
}
State_LevelMenu::State_LevelMenu(int _numPlayers) : Font("t_Dakota_font.dds","Resources/Fonts/Dakota_font.dat")
{
	m_nSelection = eMIN;
	tSC = NULL;
	NumPlayers = _numPlayers;
	renderSelection = true;
	m_bTutorial = true;
	LoadSettings();

	CEventManager::GetInstanceThreaded()->RegisterListenerRender(ET_CAMERADONEMOVING,this);
}

void State_LevelMenu::OnPush(void)
{
	if(tSC == NULL)
		tSC = tStateConfig::GetInstanceRender();

	D3DXMATRIX camera =
		D3DXMATRIX(0.998445f, 0.0f, 0.0557419f, 0.0f,
		-0.00408244f, 0.997314f, 0.0731243f, 0.0f,
		-0.0555923f, -0.0732382f, 0.995764f, 0.0f,
		3.89455f, 15.987f, -17.5849f, 1.0f);

	volatile tCameraData* tCam = new tCameraData(EM_MENUCAMERA, camera);
	CEventManager::GetInstanceThreaded()->SendEventRenderCrossToUpdate(ET_MENUCAMERA, (tBaseEventData*)tCam);
	tCam = NULL;

	texBack = CAssetManager::getInstanceRender()->LoadTexture("t_SelectedBack_d.DDS");
	texFreezer = CAssetManager::getInstanceRender()->LoadTexture("t_SelectedFreezer_d.DDS");
	texSink = CAssetManager::getInstanceRender()->LoadTexture("t_SelectedSink_d.DDS");
	texStove = CAssetManager::getInstanceRender()->LoadTexture("t_SelectedStove_d.DDS");

	int temp = 500;
	for (int i = 0; i < 5; ++i)
	{
		m_tHighScores[i].m_szName[0] = 'A';
		m_tHighScores[i].m_szName[1] = 'A';
		m_tHighScores[i].m_szName[2] = 'A';
		m_tHighScores[i].m_szName[3] = '\0';
		m_tHighScores[i].m_nScore = temp;
		m_tHighScores[i].m_nNumPlayers = 1;

		temp -= 50;
	}

	renderSelection = true;
}

void State_LevelMenu::OnEnter(void)
{
	renderSelection = true;
}

void State_LevelMenu::OnExit(void)
{
	CEventManager::GetInstanceThreaded()->UnregisterListenerRender(ET_CAMERADONEMOVING, this);
	renderSelection = false;

}
void State_LevelMenu::SendLevelMenuEvent(void)
{
	volatile tLevelSelect* tLS = new tLevelSelect(EM_LEVELSELECT, tSC->m_nLevelSelect);
	CEventManager::GetInstanceThreaded()->SendEventRenderCrossToUpdate(ET_LEVELSELECT,(tBaseEventData*)tLS);
	tLS = NULL;

	RJ_GameMain::GetInstance()->setRenderPause(false);

	volatile tNumPlayers* tNP = new tNumPlayers(EM_NUMPLAYERS, tSC->m_nNumberofPlayers);
	CEventManager::GetInstanceThreaded()->SendEventRenderCrossToUpdate(ET_NUMPLAYERS,(tBaseEventData*)tNP);
	tNP = NULL;
	volatile tPlaySoundEnum* tSOUND = new tPlaySoundEnum(EM_PLAYSOUNDENUM, RJ_Config::RJ_UI_SELECT,true); 
	CEventManager::GetInstanceThreaded()->SendEventRenderLocal(ET_PLAYSOUND,(tBaseEventData*)tSOUND);
	tSOUND = NULL;
}
bool State_LevelMenu::Input(double _fDelta)
{
	if(CInputManager::GetInstanceRender()->getKeysPressed() & RJ_Config::AP_CANCEL)
	{
		volatile tCameraPop* tPop = new tCameraPop(EM_CAMERAPOP);
		CEventManager::GetInstanceThreaded()->SendEventRenderCrossToUpdate(ET_CAMERAPOP, (tBaseEventData*)tPop);
		tPop = NULL;
		volatile tPlaySoundEnum* tSOUND = new tPlaySoundEnum(EM_PLAYSOUNDENUM, RJ_Config::RJ_UI_SELECT,true); 
		CEventManager::GetInstanceThreaded()->SendEventRenderLocal(ET_PLAYSOUND,(tBaseEventData*)tSOUND);
		tSOUND = NULL;

		RJ_MAIN_GAME->PopState();
		//RJ_MAIN_GAME->PushState(new State_PlayerMenu());
	}
	if (renderSelection)
	{
		if(CInputManager::GetInstanceRender()->getKeysPressed() & RJ_Config::AP_UP || CInputManager::GetInstanceRender()->getKeysPressed() & RJ_Config::AP_LEFT)
		{
			m_nSelection--;
			if(m_nSelection < eMIN)
				m_nSelection = eMAX;
			volatile tPlaySoundEnum* tSOUND = new tPlaySoundEnum(EM_PLAYSOUNDENUM, RJ_Config::RJ_UI_NAVIGATE,true); 
			CEventManager::GetInstanceThreaded()->SendEventRenderLocal(ET_PLAYSOUND,(tBaseEventData*)tSOUND);
			tSOUND = NULL;
		}
		else if(CInputManager::GetInstanceRender()->getKeysPressed() & RJ_Config::AP_DOWN || CInputManager::GetInstanceRender()->getKeysPressed() & RJ_Config::AP_RIGHT)
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
			bool temp = m_bTutorial;
			switch(m_nSelection)
			{
			case eLEVELONE:
				{
					volatile tCameraPop* tPop = new tCameraPop(EM_CAMERAPOP);
					CEventManager::GetInstanceThreaded()->SendEventRenderCrossToUpdate(ET_CAMERAPOP, (tBaseEventData*)tPop);
					tPop = NULL;

					LevelSelect = 1;
					tSC->m_nLevelSelect = 1;
					tSC->m_bIsLoading = true;
					SendLevelMenuEvent();
					//RJ_MAIN_GAME->PopState();
					// pop every state
					volatile tEnterLevel* tLevel = new tEnterLevel(EM_ENTERLEVEL, temp);
					CEventManager::GetInstanceThreaded()->SendEventRenderLocal(ET_ENTERLEVEL,(tBaseEventData*)tLevel);
					tLevel = NULL;

					/*RJ_MAIN_GAME->ClearAllStates();
					RJ_MAIN_GAME->PushState(new State_HUD());
					if(temp)
						RJ_MAIN_GAME->PushState(new State_Tutorial());
					RJ_MAIN_GAME->PushState(new State_CamFly());
					RJ_MAIN_GAME->PushState(new State_Loading());*/
				} break;
			case eLEVELTWO:
				{
					volatile tCameraPop* tPop = new tCameraPop(EM_CAMERAPOP);
					CEventManager::GetInstanceThreaded()->SendEventRenderCrossToUpdate(ET_CAMERAPOP, (tBaseEventData*)tPop);
					tPop = NULL;

					LevelSelect = 2;
					tSC->m_nLevelSelect = 2;
					tSC->m_bIsLoading = true;
					SendLevelMenuEvent();
					//RJ_MAIN_GAME->PopState();
					// pop every state
					volatile tEnterLevel* tLevel = new tEnterLevel(EM_ENTERLEVEL, temp);
					CEventManager::GetInstanceThreaded()->SendEventRenderLocal(ET_ENTERLEVEL,(tBaseEventData*)tLevel);
					tLevel = NULL;

					/*RJ_MAIN_GAME->ClearAllStates();
					RJ_MAIN_GAME->PushState(new State_HUD());
					if(temp)
						RJ_MAIN_GAME->PushState(new State_Tutorial());
					RJ_MAIN_GAME->PushState(new State_CamFly());
					RJ_MAIN_GAME->PushState(new State_Loading());*/
				} break;
			case eLEVELTHREE:
				{
					volatile tCameraPop* tPop = new tCameraPop(EM_CAMERAPOP);
					CEventManager::GetInstanceThreaded()->SendEventRenderCrossToUpdate(ET_CAMERAPOP, (tBaseEventData*)tPop);
					tPop = NULL;

					LevelSelect = 3;
					tSC->m_nLevelSelect = 3;
					tSC->m_bIsLoading = true;
					SendLevelMenuEvent();
					//RJ_MAIN_GAME->PopState();
					// pop every state
					volatile tEnterLevel* tLevel = new tEnterLevel(EM_ENTERLEVEL, temp);
					CEventManager::GetInstanceThreaded()->SendEventRenderLocal(ET_ENTERLEVEL,(tBaseEventData*)tLevel);
					tLevel = NULL;

					/*RJ_MAIN_GAME->ClearAllStates();
					RJ_MAIN_GAME->PushState(new State_HUD());
					if(temp)
						RJ_MAIN_GAME->PushState(new State_Tutorial());
					RJ_MAIN_GAME->PushState(new State_CamFly());
					RJ_MAIN_GAME->PushState(new State_Loading());*/
				} break;
			case eBACK:
				{
					volatile tCameraPop* tPop = new tCameraPop(EM_CAMERAPOP);
					CEventManager::GetInstanceThreaded()->SendEventRenderCrossToUpdate(ET_CAMERAPOP, (tBaseEventData*)tPop);
					tPop = NULL;

					RJ_MAIN_GAME->PopState();
					//RJ_MAIN_GAME->PushState(new State_PlayerMenu());
				} break;
			}
		}
	}

	return true;
}

bool State_LevelMenu::InputUnder(double _fDelta)
{
	return true;
}

bool State_LevelMenu::Update(double _fDelta)
{
	return true;
}

bool State_LevelMenu::UpdateUnder(double _fDelta)
{
	return true;
}

bool State_LevelMenu::Render(double _fDelta)
{
	//CAssetManager::getInstanceRender()->DrawFlatSprite(tSC->m_nLevelMenuBG,0.0f,0.0f,1.0f,0.75f);

	//Font.DrawString("Crumbs!",300,100);

	//char szBuffer[5];

	switch(m_nSelection)
	{
	case eLEVELONE:
		{
			CAssetManager::getInstanceRender()->GetMesh("LvMM_SM_LevelSelectScreen")->setTextureID(texFreezer);
			if(NumPlayers == 1)
			{
				if(!LoadHighScores1())
					SaveHighScores1();
			}
			else
			{
				if(!LoadHighScores1M())
					SaveHighScores1M();
			}
			//if(renderSelection)
			//{
			//	sprintf_s(szBuffer, "250");
			//	//Font.DrawString(szBuffer, 220, 300, 1.0f, 1.0f, 0.0f, D3DCOLOR_ARGB(245, 255, 255, 255));
			//	D3DXMATRIX _world;
			//	D3DXMatrixIdentity(&_world);
			//	D3DXMatrixTranslation(&_world, 7.7f, 16.3f, 0.0f);
			//	Font.DrawString(szBuffer, &D3DXVECTOR3(-0.4f,0.0f,-0.001f), &_world, 0.7f, 0.7f, D3DXVECTOR4(1.0f,1.0f,1.0f,1.0f));
			//}
		} break;
	case eLEVELTWO:
		{
			CAssetManager::getInstanceRender()->GetMesh("LvMM_SM_LevelSelectScreen")->setTextureID(texSink);
			if(NumPlayers == 1)
			{
				if(!LoadHighScores2())
					SaveHighScores2();
			}
			else
			{
				if(!LoadHighScores2M())
					SaveHighScores2M();
			}
			//if(renderSelection)
			//{
			//	sprintf_s(szBuffer, "250");
			//	//Font.DrawString(szBuffer, 400, 330, 1.0f, 1.0f, 0.0f, D3DCOLOR_ARGB(245, 255, 255, 255));
			//	D3DXMATRIX _world;
			//	D3DXMatrixIdentity(&_world);
			//	D3DXMatrixTranslation(&_world, 4.2f, 15.8f, 0.0f);
			//	Font.DrawString(szBuffer, &D3DXVECTOR3(-0.4f,0.0f,-0.001f), &_world, 0.7f, 0.7f, D3DXVECTOR4(1.0f,1.0f,1.0f,1.0f));
			//}
		} break;
	case eLEVELTHREE:
		{
			CAssetManager::getInstanceRender()->GetMesh("LvMM_SM_LevelSelectScreen")->setTextureID(texStove);
			if(NumPlayers == 1)
			{
				if(!LoadHighScores3())
					SaveHighScores3();
			}
			else
			{
				if(!LoadHighScores3M())
					SaveHighScores3M();
			}
			//if(renderSelection)
			//{	
			//	sprintf_s(szBuffer, "250");
			//	//Font.DrawString(szBuffer, 590, 260, 1.0f, 1.0f, 0.0f, D3DCOLOR_ARGB(245, 255, 255, 255));
			//	D3DXMATRIX _world;
			//	D3DXMatrixIdentity(&_world);
			//	D3DXMatrixTranslation(&_world, 1.0f, 17.2f, 0.0f);
			//	Font.DrawString(szBuffer, &D3DXVECTOR3(-0.4f,0.0f,-0.001f), &_world, 0.7f, 0.7f, D3DXVECTOR4(1.0f,1.0f,1.0f,1.0f));
			//}	
		} break;
	case eBACK:
		{
			CAssetManager::getInstanceRender()->GetMesh("LvMM_SM_LevelSelectScreen")->setTextureID(texBack);
		} break;
	}
	//if(renderSelection)
	//{
		char szBufferHS[128];
		sprintf_s(szBufferHS, "1) %s - %i", m_tHighScores[0].m_szName, m_tHighScores[0].m_nScore);

		D3DXMATRIX _world;
		D3DXMatrixIdentity(&_world);
		D3DXMatrixTranslation(&_world, -1.7f, 20.7f, 0.0f);
		Font.DrawString(szBufferHS, &D3DXVECTOR3(-0.2f,0.0f,-0.001f), &_world, 0.3f, 0.3f, D3DXVECTOR4(0.0f,0.0f,0.0f,1.0f));

		sprintf_s(szBufferHS, "2) %s - %i", m_tHighScores[1].m_szName, m_tHighScores[1].m_nScore);
		//Font.DrawString(szBuffer, 740, 80, 1.0f, 1.0f, 0.0f, D3DCOLOR_ARGB(245, 0, 0, 0));
		D3DXMatrixIdentity(&_world);
		D3DXMatrixTranslation(&_world, -1.7f, 20.0f, 0.0f);
		Font.DrawString(szBufferHS, &D3DXVECTOR3(-0.2f,0.0f,-0.001f), &_world, 0.3f, 0.3f, D3DXVECTOR4(0.0f,0.0f,0.0f,1.0f));

		sprintf_s(szBufferHS, "3) %s - %i", m_tHighScores[2].m_szName, m_tHighScores[2].m_nScore);
		//Font.DrawString(szBuffer, 740, 100, 1.0f, 1.0f, 0.0f, D3DCOLOR_ARGB(245, 0, 0, 0));
		D3DXMatrixIdentity(&_world);
		D3DXMatrixTranslation(&_world, -1.7f, 19.4f, 0.0f);
		Font.DrawString(szBufferHS, &D3DXVECTOR3(-0.2f,0.0f,-0.001f), &_world, 0.3f, 0.3f, D3DXVECTOR4(0.0f,0.0f,0.0f,1.0f));

		sprintf_s(szBufferHS, "4) %s - %i", m_tHighScores[3].m_szName, m_tHighScores[3].m_nScore);
		//Font.DrawString(szBuffer, 740, 120, 1.0f, 1.0f, 0.0f, D3DCOLOR_ARGB(245, 0, 0, 0));
		D3DXMatrixIdentity(&_world);
		D3DXMatrixTranslation(&_world, -1.7f, 18.8f, 0.0f);
		Font.DrawString(szBufferHS, &D3DXVECTOR3(-0.2f,0.0f,-0.001f), &_world, 0.3f, 0.3f, D3DXVECTOR4(0.0f,0.0f,0.0f,1.0f));

		sprintf_s(szBufferHS, "5) %s - %i", m_tHighScores[4].m_szName, m_tHighScores[4].m_nScore);
		//Font.DrawString(szBuffer, 740, 140, 1.0f, 1.0f, 0.0f, D3DCOLOR_ARGB(245, 0, 0, 0));
		D3DXMatrixIdentity(&_world);
		D3DXMatrixTranslation(&_world, -1.7f, 18.1f, 0.0f);
		Font.DrawString(szBufferHS, &D3DXVECTOR3(-0.2f,0.0f,-0.001f), &_world, 0.3f, 0.3f, D3DXVECTOR4(0.0f,0.0f,0.0f,1.0f));
	//}

	//Cursor::GetInstanceRender()->Render();
	//RJ_RENDERER->getDirect3DDevice()->SetTransform(D3DTS_WORLD, &matrix);


	return true;
}

bool State_LevelMenu::RenderUnder(double _fDelta)
{
	return true;
}

void State_LevelMenu::HandleEvent(CEvent* pEvent)
{			
	if(pEvent->GetEventID() == ET_CAMERADONEMOVING)
	{
		if(pEvent->GetData()->GetMsgID() == EM_CAMERADONEMOVING)
		{
			if(pEvent->GetData() != NULL)
				renderSelection = true;
		}
	}

	pEvent->m_nRefs--;
}
void State_LevelMenu::LoadSettings()
{
	// parse file
	std::fstream file;
	file.open("Resources/Data/data.sav", std::ios_base::in | std::ios_base::binary);

	float fFX, fMusic, fBrightness;
	if (file.is_open())
	{
		// load info
		file.read( (char*)&fFX, sizeof(float) );
		file.read( (char*)&fMusic, sizeof(float) );
		file.read( (char*)&fBrightness, sizeof(float) );
		file.read( (char*)&m_bTutorial, sizeof(bool) );

		file.close();
	}
}
bool State_LevelMenu::LoadHighScores1(void)
{
	std::fstream file;
	file.open("Resources/Data/highScores1.sav", std::ios_base::in | std::ios_base::binary);

	if (file.is_open())
	{
		// load info
		for(unsigned int i = 0; i < 5; ++i)
		{
			file.read( (char*)&m_tHighScores[i].m_szName, sizeof(char) * 4);
			file.read( (char*)&m_tHighScores[i].m_nScore, sizeof(int) );
			file.read( (char*)&m_tHighScores[i].m_nNumPlayers, sizeof(int) );
		}
		file.close();
		return true;
	}
	return false;
}
bool State_LevelMenu::SaveHighScores1(void)
{
	std::fstream file;
	file.open("Resources/Data/highScores1.sav", std::ios_base::out | std::ios_base::trunc | std::ios_base::binary);

	if (file.is_open())
	{
		for(unsigned int i = 0; i < 5; ++i)
		{
			file.write( (char*)&m_tHighScores[i].m_szName, sizeof(char)  * 4);
			file.write( (char*)&m_tHighScores[i].m_nScore, sizeof(int) );
			file.write( (char*)&m_tHighScores[i].m_nNumPlayers, sizeof(int) );
		}
		file.close();
		return true;
	}
	return false;
}

bool State_LevelMenu::LoadHighScores2(void)
{
	std::fstream file;
	file.open("Resources/Data/highScores2.sav", std::ios_base::in | std::ios_base::binary);

	if (file.is_open())
	{
		// load info
		for(unsigned int i = 0; i < 5; ++i)
		{
			file.read( (char*)&m_tHighScores[i].m_szName, sizeof(char) * 4);
			file.read( (char*)&m_tHighScores[i].m_nScore, sizeof(int) );
			file.read( (char*)&m_tHighScores[i].m_nNumPlayers, sizeof(int) );
		}
		file.close();
		return true;
	}
	return false;
}
bool State_LevelMenu::SaveHighScores2(void)
{
	std::fstream file;
	file.open("Resources/Data/highScores2.sav", std::ios_base::out | std::ios_base::trunc | std::ios_base::binary);

	if (file.is_open())
	{
		for(unsigned int i = 0; i < 5; ++i)
		{
			file.write( (char*)&m_tHighScores[i].m_szName, sizeof(char) * 4);
			file.write( (char*)&m_tHighScores[i].m_nScore, sizeof(int) );
			file.write( (char*)&m_tHighScores[i].m_nNumPlayers, sizeof(int) );
		}
		file.close();
		return true;
	}
	return false;
}
bool State_LevelMenu::LoadHighScores3(void)
{
	std::fstream file;
	file.open("Resources/Data/highScores3.sav", std::ios_base::in | std::ios_base::binary);

	if (file.is_open())
	{
		// load info
		for(unsigned int i = 0; i < 5; ++i)
		{
			file.read( (char*)&m_tHighScores[i].m_szName, sizeof(char) * 4);
			file.read( (char*)&m_tHighScores[i].m_nScore, sizeof(int) );
			file.read( (char*)&m_tHighScores[i].m_nNumPlayers, sizeof(int) );
		}
		file.close();
		return true;
	}
	return false;
}
bool State_LevelMenu::SaveHighScores3(void)
{
	std::fstream file;
	file.open("Resources/Data/highScores3.sav", std::ios_base::out | std::ios_base::trunc | std::ios_base::binary);

	if (file.is_open())
	{
		for(unsigned int i = 0; i < 5; ++i)
		{
			file.write( (char*)&m_tHighScores[i].m_szName, sizeof(char) * 4);
			file.write( (char*)&m_tHighScores[i].m_nScore, sizeof(int) );
			file.write( (char*)&m_tHighScores[i].m_nNumPlayers, sizeof(int) );
		}
		file.close();
		return true;
	}
	return false;
}
bool State_LevelMenu::LoadHighScores1M(void)
{
	std::fstream file;
	file.open("Resources/Data/highScores1M.sav", std::ios_base::in | std::ios_base::binary);

	if (file.is_open())
	{
		// load info
		for(unsigned int i = 0; i < 5; ++i)
		{
			file.read( (char*)&m_tHighScores[i].m_szName, sizeof(char) * 4);
			file.read( (char*)&m_tHighScores[i].m_nScore, sizeof(int) );
			file.read( (char*)&m_tHighScores[i].m_nNumPlayers, sizeof(int) );
		}
		file.close();
		return true;
	}
	return false;
}
bool State_LevelMenu::SaveHighScores1M(void)
{
	std::fstream file;
	file.open("Resources/Data/highScores1M.sav", std::ios_base::out | std::ios_base::trunc | std::ios_base::binary);

	if (file.is_open())
	{
		for(unsigned int i = 0; i < 5; ++i)
		{
			file.write( (char*)&m_tHighScores[i].m_szName, sizeof(char) * 4);
			file.write( (char*)&m_tHighScores[i].m_nScore, sizeof(int) );
			file.write( (char*)&m_tHighScores[i].m_nNumPlayers, sizeof(int) );
		}
		file.close();
		return true;
	}
	return false;
}

bool State_LevelMenu::LoadHighScores2M(void)
{
	std::fstream file;
	file.open("Resources/Data/highScores2M.sav", std::ios_base::in | std::ios_base::binary);

	if (file.is_open())
	{
		// load info
		for(unsigned int i = 0; i < 5; ++i)
		{
			file.read( (char*)&m_tHighScores[i].m_szName, sizeof(char) * 4);
			file.read( (char*)&m_tHighScores[i].m_nScore, sizeof(int) );
			file.read( (char*)&m_tHighScores[i].m_nNumPlayers, sizeof(int) );
		}
		file.close();
		return true;
	}
	return false;
}
bool State_LevelMenu::SaveHighScores2M(void)
{
	std::fstream file;
	file.open("Resources/Data/highScores2M.sav", std::ios_base::out | std::ios_base::trunc | std::ios_base::binary);

	if (file.is_open())
	{
		for(unsigned int i = 0; i < 5; ++i)
		{
			file.write( (char*)&m_tHighScores[i].m_szName, sizeof(char) * 4);
			file.write( (char*)&m_tHighScores[i].m_nScore, sizeof(int) );
			file.write( (char*)&m_tHighScores[i].m_nNumPlayers, sizeof(int) );
		}
		file.close();
		return true;
	}
	return false;
}
bool State_LevelMenu::LoadHighScores3M(void)
{
	std::fstream file;
	file.open("Resources/Data/highScores3M.sav", std::ios_base::in | std::ios_base::binary);

	if (file.is_open())
	{
		// load info
		for(unsigned int i = 0; i < 5; ++i)
		{
			file.read( (char*)&m_tHighScores[i].m_szName, sizeof(char) * 4);
			file.read( (char*)&m_tHighScores[i].m_nScore, sizeof(int) );
			file.read( (char*)&m_tHighScores[i].m_nNumPlayers, sizeof(int) );
		}
		file.close();
		return true;
	}
	return false;
}
bool State_LevelMenu::SaveHighScores3M(void)
{
	std::fstream file;
	file.open("Resources/Data/highScores3M.sav", std::ios_base::out | std::ios_base::trunc | std::ios_base::binary);

	if (file.is_open())
	{
		for(unsigned int i = 0; i < 5; ++i)
		{
			file.write( (char*)&m_tHighScores[i].m_szName, sizeof(char) * 4);
			file.write( (char*)&m_tHighScores[i].m_nScore, sizeof(int) );
			file.write( (char*)&m_tHighScores[i].m_nNumPlayers, sizeof(int) );
		}
		file.close();
		return true;
	}
	return false;
}