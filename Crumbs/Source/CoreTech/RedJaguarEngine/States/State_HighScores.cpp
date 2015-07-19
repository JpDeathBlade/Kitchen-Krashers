#include "State_HighScores.h"
#include "..\\Input\\CInputManager.h"
#include "..\\..\\..\\RJ_Config.h"
#include "../Asset/AssetManager.h"
#include "../Event/EventManager.h"
#include "..\\..\\RJ_GameMain.h"
#include "../../../Gameplay/Objects/CPlayer.h"
#include "State_MainMenu.h"

// HACK!
void _SetRectPosition(RECT& r, int index, int nCols, int nRows, int xOffeset = 256, int yOffset = 256)
{
	if(index == 0 || nCols == 0)
		r.left = 0;
	else
		r.left = (index % nCols) * xOffeset;
	if(index == 0 || nRows == 0)
		r.top = 0;
	else
		r.top = (index / nRows) * yOffset;
	r.right = r.left + xOffeset;
	r.bottom = r.top + yOffset;
}

State_HighScores::State_HighScores(void) : Font("t_Dakota_font.dds","Resources/Fonts/dakota_font.dat")
{
	Gameplay = CGameplay::GetInstanceUpdate();
	Config = tStateConfig::GetInstanceRender();

	m_vColors[0][0] = 153; m_vColors[0][1] = 13; m_vColors[0][2] = 0;
	m_vColors[1][0] = 72;  m_vColors[1][1] = 27; m_vColors[1][2] = 108;
	m_vColors[2][0] = 216; m_vColors[2][1] = 118; m_vColors[2][2] = 6;
	m_vColors[3][0] = 12;  m_vColors[3][1] = 204; m_vColors[3][2] = 87;
}
State_HighScores::State_HighScores(int _playerNum, int _index, int _currLevel, int _numPlayers) : Font("t_Dakota_font.dds","Resources/Fonts/dakota_font.dat")
{
	m_vColors[0][0] = 153; m_vColors[0][1] = 13; m_vColors[0][2] = 0;
	m_vColors[1][0] = 72;  m_vColors[1][1] = 27; m_vColors[1][2] = 108;
	m_vColors[2][0] = 216; m_vColors[2][1] = 118; m_vColors[2][2] = 6;
	m_vColors[3][0] = 12;  m_vColors[3][1] = 204; m_vColors[3][2] = 87;

	Gameplay = CGameplay::GetInstanceUpdate();
	Config = tStateConfig::GetInstanceRender();

	tempName[0] = '_';
	tempName[1] = '_';
	tempName[2] = '_';
	tempName[3] = (char)0;

	int temp = 500;
	for (int i = 0; i < 5; ++i)
	{
		m_tHighScores[i].m_szName[0] = 'A';
		m_tHighScores[i].m_szName[1] = 'A';
		m_tHighScores[i].m_szName[2] = 'A';
		m_tHighScores[i].m_szName[3] = (char)0;
		m_tHighScores[i].m_nScore = temp;
		m_tHighScores[i].m_nNumPlayers = 1;

		temp -= 50;
	}
	NumPlayers = _numPlayers;
	Index = _index;
	m_nCurrLevel = _currLevel;
	Player = _playerNum;

	if(_numPlayers == 1)
	{
		if(_currLevel == 1)
		{
			if(!LoadHighScores1())
				SaveHighScores1();
		}

		if(_currLevel == 2)
		{
			if(!LoadHighScores2())
				SaveHighScores2();
		}
		if(_currLevel == 3)
		{
			if(!LoadHighScores3())
				SaveHighScores3();
		}
	}
	else
	{
		if(_currLevel == 1)
		{
			if(!LoadHighScores1M())
				SaveHighScores1M();
		}
		if(_currLevel == 2)
		{
			if(!LoadHighScores2M())
				SaveHighScores2M();
		}
		if(_currLevel == 3)
		{
			if(!LoadHighScores3M())
				SaveHighScores3M();
		}
	}
}
void State_HighScores::OnPush(void){}
void State_HighScores::OnEnter(void)
{
	if(NumPlayers == 1)
	{
		if(m_nCurrLevel == 1)
		{
			if(!LoadHighScores1())
				SaveHighScores1();
		}

		if(m_nCurrLevel == 2)
		{
			if(!LoadHighScores2())
				SaveHighScores2();
		}
		if(m_nCurrLevel == 3)
		{
			if(!LoadHighScores3())
				SaveHighScores3();
		}
	}
	else
	{
		if(m_nCurrLevel == 1)
		{
			if(!LoadHighScores1M())
				SaveHighScores1M();
		}
		if(m_nCurrLevel == 2)
		{
			if(!LoadHighScores2M())
				SaveHighScores2M();
		}
		if(m_nCurrLevel == 3)
		{
			if(!LoadHighScores3M())
				SaveHighScores3M();
		}
	}

	/*volatile tPause* tPD = new tPause(EM_PAUSE, true);
	CEventManager::GetInstanceThreaded()->SendEventRenderCrossToUpdate(ET_PAUSE, (tBaseEventData*)tPD);
	tPD = NULL;*/
}

void State_HighScores::OnExit(void)
{
	if(NumPlayers == 1)
	{
		if(m_nCurrLevel == 1)
		{
			SaveHighScores1();
		}
		if(m_nCurrLevel == 2)
		{
			SaveHighScores2();
		}
		if(m_nCurrLevel == 3)
		{
			SaveHighScores3();
		}
	}
	else
	{
		if(m_nCurrLevel == 1)
		{
			SaveHighScores1M();
		}
		if(m_nCurrLevel == 2)
		{
			SaveHighScores2M();
		}
		if(m_nCurrLevel == 3)
		{
			SaveHighScores3M();
		}
	}
}
void State_HighScores::HandleEvent(CEvent* pEvent)
{
	pEvent->m_nRefs--;
}
bool State_HighScores::Input(double _fDelta)
{
	if (Index != -1)
	{
		char key = 0;
		key = CInputManager::GetInstanceRender()->CheckBufferedKeysEx();
		if(key != 0)
		{
			if(key == 8)
			{
				for(int i = 2; i >= 0; i--)
				{
					if(tempName[i] != '_')
					{
						tempName[i] = '_';
						break;
					}
				}
			}
			if(key >= 65 && key <= 122)
			{
				if(key >= 97 && key <= 122)
					key -= 32;
				if(key >= 65 && key <= 90)
				{
					for(int i = 0; i < 3; ++i)
					{
						if(tempName[i] == '_')
						{
							tempName[i] = key;
							break;
						}
						tempName[3] = (char)0;
					}
				}
			}
		}
		if(tempName[2] != '_' && key == 13)
		{
			m_tHighScores[Index].m_szName[0] = tempName[0];
			m_tHighScores[Index].m_szName[1] = tempName[1];
			m_tHighScores[Index].m_szName[2] = tempName[2];
			m_tHighScores[Index].m_szName[3] = tempName[3];
			Index = -1;
		}
	}
	else
	{
		if(CInputManager::GetInstanceRender()->getKeysPressed() & RJ_Config::AP_CONFIRM)
		{
			volatile tPlaySoundEnum* tSOUND = new tPlaySoundEnum(EM_PLAYSOUNDENUM, RJ_Config::RJ_UI_SELECT,true); 
			CEventManager::GetInstanceThreaded()->SendEventRenderLocal(ET_PLAYSOUND,(tBaseEventData*)tSOUND);
			tSOUND = NULL;

			RJ_MAIN_GAME->PopState();

			/*volatile tPause* tPD = new tPause(EM_PAUSE);
			CEventManager::GetInstanceThreaded()->SendEventRenderCrossToUpdate(ET_PAUSE, (tBaseEventData*)tPD);
			tPD = NULL;*/
		}
	}

	return true;
}

bool State_HighScores::InputUnder(double _fDelta)	{ return true; }
bool State_HighScores::Update(double _fDelta)		{ return true; }
bool State_HighScores::UpdateUnder(double _fDelta){ return true; }

bool State_HighScores::Render(double _fDelta)
{
	CAssetManager::getInstanceRender()->DrawFlatSprite(Config->m_nHighScoreID,0.0f,0.0f,1.0f,0.75f);

	char szBuffer[128];
	if(Index == -1)
	{
		Font.DrawString("Press Enter", 86, 618, 1.0f, 1.0f, 0.0f, D3DCOLOR_XRGB(0,0,0));
		Font.DrawString("to Continue", 86, 648, 1.0f, 1.0f, 0.0f, D3DCOLOR_XRGB(0,0,0));
	}
	else
	{
		if(NumPlayers > 1)
		{
			sprintf_s(szBuffer, "Player %i", Player);
			Font.DrawString(szBuffer, 86, 588, 1.0f, 1.0f, 0.0f, D3DCOLOR_XRGB(0,0,0));
		}
		Font.DrawString("Enter your", 74, 618, 1.0f, 1.0f, 0.0f, D3DCOLOR_XRGB(0,0,0));
		Font.DrawString("Name", 92, 648, 1.0f, 1.0f, 0.0f, D3DCOLOR_XRGB(0,0,0));
	}

	if(Index == -1)
	{
		/// <Player 1>
		sprintf_s(szBuffer, "1) %s %i", m_tHighScores[0].m_szName, m_tHighScores[0].m_nScore);
		Font.DrawString(szBuffer, 414, 263, 1.0f, 1.0f, 0.0f, D3DCOLOR_ARGB(245, 0, 0, 0));
		/*sprintf_s(szBuffer, "%i", Score[0]);
		Font.DrawString(szBuffer, 574, 334, 1.0f, 1.0f, 0.0f,
		D3DCOLOR_ARGB(245, m_vColors[Standing[0]][0], m_vColors[Standing[0]][1], m_vColors[Standing[0]][2]));*/

		sprintf_s(szBuffer, "2) %s %i", m_tHighScores[1].m_szName, m_tHighScores[1].m_nScore);
		Font.DrawString(szBuffer, 414, 334, 1.0f, 1.0f, 0.0f, D3DCOLOR_ARGB(245, 0, 0, 0));
		/*sprintf_s(szBuffer, "%i", Score[1]);
		Font.DrawString(szBuffer, 574, 404, 1.0f, 1.0f, 0.0f,
		D3DCOLOR_ARGB(245, m_vColors[Standing[1]][0], m_vColors[Standing[1]][1], m_vColors[Standing[1]][2]));*/

		sprintf_s(szBuffer, "3) %s %i", m_tHighScores[2].m_szName, m_tHighScores[2].m_nScore);
		Font.DrawString(szBuffer, 414, 404, 1.0f, 1.0f, 0.0f, D3DCOLOR_ARGB(245, 0, 0, 0));
		/*sprintf_s(szBuffer, "%i", Score[2]);
		Font.DrawString(szBuffer, 574, 478, 1.0f, 1.0f, 0.0f,
		D3DCOLOR_ARGB(245, m_vColors[Standing[2]][0], m_vColors[Standing[2]][1], m_vColors[Standing[2]][2]));*/
		sprintf_s(szBuffer, "4) %s %i", m_tHighScores[3].m_szName, m_tHighScores[3].m_nScore);
		Font.DrawString(szBuffer, 414, 478, 1.0f, 1.0f, 0.0f, D3DCOLOR_ARGB(245, 0, 0, 0));
		/*sprintf_s(szBuffer, "%i", Score[3]);
		Font.DrawString(szBuffer, 574, 550, 1.0f, 1.0f, 0.0f,
		D3DCOLOR_ARGB(245, m_vColors[Standing[3]][0], m_vColors[Standing[3]][1], m_vColors[Standing[3]][2]));*/
		sprintf_s(szBuffer, "5) %s %i", m_tHighScores[4].m_szName, m_tHighScores[4].m_nScore);
		Font.DrawString(szBuffer, 414, 550, 1.0f, 1.0f, 0.0f, D3DCOLOR_ARGB(245, 0, 0, 0));
	}
	else
	{
		float y = 263;
		for(int i = 0; i < 5; ++i)
		{
			if(Index == i)
			{
				sprintf_s(szBuffer, "%i) %s %i", i+1, tempName, m_tHighScores[i].m_nScore);
				Font.DrawString(szBuffer, 414, y, 1.0f, 1.0f, 0.0f,
					D3DCOLOR_ARGB(245, m_vColors[Player-1][0], m_vColors[Player-1][1], m_vColors[Player-1][2]));
				/*sprintf_s(szBuffer, "%i", Score[0]);
				Font.DrawString(szBuffer, 574, 334, 1.0f, 1.0f, 0.0f,
				D3DCOLOR_ARGB(245, m_vColors[Standing[0]][0], m_vColors[Standing[0]][1], m_vColors[Standing[0]][2]));*/

				// draw bottle cap
				RECT rCapRect;
				_SetRectPosition(rCapRect, Player-1, 2, 2);
				CAssetManager::getInstanceRender()->DrawFlatSprite(Config->m_nStonesID, 352.0f, y - 32.0f, 0.25f, 0.25f, &rCapRect);
			}
			else
			{
				sprintf_s(szBuffer, "%i) %s %i", i+1, m_tHighScores[i].m_szName, m_tHighScores[i].m_nScore);
				Font.DrawString(szBuffer, 414, y, 1.0f, 1.0f, 0.0f, D3DCOLOR_ARGB(245, 0, 0, 0));
				/*sprintf_s(szBuffer, "%i", Score[0]);
				Font.DrawString(szBuffer, 574, 334, 1.0f, 1.0f, 0.0f,
				D3DCOLOR_ARGB(245, m_vColors[Standing[0]][0], m_vColors[Standing[0]][1], m_vColors[Standing[0]][2]));*/
			}
			y+=71;
		}
	}
		

	/*else if(Config->m_nNumberofPlayers == 1)
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
	}*/

	return true;
}

bool State_HighScores::RenderUnder(double _fDelta){ return true; }

bool State_HighScores::LoadHighScores1(void)
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
bool State_HighScores::SaveHighScores1(void)
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

bool State_HighScores::LoadHighScores2(void)
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
bool State_HighScores::SaveHighScores2(void)
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
bool State_HighScores::LoadHighScores3(void)
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
bool State_HighScores::SaveHighScores3(void)
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
bool State_HighScores::LoadHighScores1M(void)
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
bool State_HighScores::SaveHighScores1M(void)
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

bool State_HighScores::LoadHighScores2M(void)
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
bool State_HighScores::SaveHighScores2M(void)
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
bool State_HighScores::LoadHighScores3M(void)
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
bool State_HighScores::SaveHighScores3M(void)
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