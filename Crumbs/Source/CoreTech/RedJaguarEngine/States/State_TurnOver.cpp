#include "State_TurnOver.h"
#include "State_Config.h"
#include "../Event/EventDefinitions.h"
#include "../Event/EventManager.h"
#include "../Asset/AssetManager.h"
#include "../Text/CBitmapFont.h"
#include "../Input/CInputManager.h"
#include "../../RJ_GameMain.h"
#include "../text/font_defines.h"

State_TurnOver::State_TurnOver(int nCurrPlayer, int nBits, int nBounces, int nBrownie, int nMultiplier, int nPar) : Font("t_Crumbs_font.dds","Resources/Fonts/Crumbs_font.dat")
{
	m_bRender = true;

	tSC = NULL;
	pAM = NULL;

	m_nPlayer = nCurrPlayer;
	m_nMultiplier = nMultiplier;
	m_nBitPoints = nBits;
	m_nBouncePoints = nBounces;
	m_nBrowniePoints = nBrownie;
	m_nBounceBitMult = (m_nBitPoints + m_nBouncePoints) * m_nMultiplier;
	m_nTotal = m_nBounceBitMult + m_nBrowniePoints;
	m_nPar = nPar;

	// count digits
	m_nDigits[0] = 1;
	int nTmp = m_nBouncePoints;
	while( nTmp > 9 )
	{
		nTmp /= 10;
		m_nDigits[0]++;
	}
	m_nDigits[1] = 1;
	nTmp = m_nBitPoints;
	while( nTmp > 9 )
	{
		nTmp /= 10;
		m_nDigits[1]++;
	}
	m_nDigits[2] = 1;
	nTmp = m_nBrowniePoints;
	while( nTmp > 9 )
	{
		nTmp /= 10;
		m_nDigits[2]++;
	}
	m_nDigits[3] = 1;
	nTmp = m_nBounceBitMult;
	while( nTmp > 9 )
	{
		nTmp /= 10;
		m_nDigits[3]++;
	}
	m_nDigits[4] = 1;
	nTmp = m_nTotal;
	while( nTmp > 9 )
	{
		nTmp /= 10;
		m_nDigits[4]++;
	}
}

void State_TurnOver::OnPush(void)
{
	if(tSC == NULL)
		tSC = tStateConfig::GetInstanceRender();
	if(pAM == NULL)
		pAM = CAssetManager::getInstanceRender();
}

bool State_TurnOver::Render(double _fDelta)
{
	// kill yoself
	if ( CInputManager::GetInstanceRender()->getKeysPressed() & RJ_Config::AP_CONFIRM )
	{
		m_bRender = false;
		volatile tPause* tNP = new tPause(EM_PAUSE);
		CEventManager::GetInstanceThreaded()->SendEventRenderCrossToUpdate(ET_PAUSE,(tBaseEventData*)tNP);
		tNP = NULL;
		volatile tPlaySoundEnum* tSOUND = new tPlaySoundEnum(EM_PLAYSOUNDENUM, RJ_Config::RJ_UI_SELECT,true); 
		CEventManager::GetInstanceThreaded()->SendEventRenderLocal(ET_PLAYSOUND,(tBaseEventData*)tSOUND);
		tSOUND = NULL;
		RJ_MAIN_GAME->PopState();
	}

	if (m_bRender)
	{
		// draw background image
		pAM->DrawFlatSprite(tSC->m_nTurnOverBG, 0.0f, 0.0f);//146.0f, 170.0f);

		char buffer[16] = "";
		/// <name>
		switch (m_nPlayer)
		{
		case 0:
			Font.DrawString("One Cherry Sherry", 382.0f, 185.0f, 1.0f, 1.0f, 0.0f, D3DCOLOR_ARGB(245,0,0,0));
			break;
		case 1:
			Font.DrawString("Two-Grape Crepes", 382.0f, 185.0f, 1.0f, 1.0f, 0.0f, D3DCOLOR_ARGB(245,0,0,0));
			break;
		case 2:
			Font.DrawString("Three Juiced Oranges", 342.0f, 185.0f, 1.0f, 1.0f, 0.0f, D3DCOLOR_ARGB(245,0,0,0));
			break;
		case 3:
			Font.DrawString("Thyme 4 Lime", 452.0f, 185.0f, 1.0f, 1.0f, 0.0f, D3DCOLOR_ARGB(245,0,0,0));
			break;
		}

		///	<numbers>
		sprintf_s(buffer, 16, "%i", m_nBouncePoints);
		Font.DrawString(buffer, 450.0f - ( (m_nDigits[0]-1) * 16 ), 250.0f, 1.0f, 1.0f, 0.0f, D3DCOLOR_ARGB(245,0,0,0));
		sprintf_s(buffer, 16, "%i", m_nBitPoints);
		Font.DrawString(buffer, 692.0f - ( (m_nDigits[1]-1) * 16 ), 250.0f, 1.0f, 1.0f, 0.0f, D3DCOLOR_ARGB(245,0,0,0));
		sprintf_s(buffer, 16, "%i", m_nBrowniePoints);
		Font.DrawString(buffer, 605.0f - ( (m_nDigits[2]-1) * 16 ), 322.0f, 1.0f, 1.0f, 0.0f, D3DCOLOR_ARGB(245,0,0,0));

		sprintf_s(buffer, 16, "%i", m_nMultiplier);
		Font.DrawString(buffer, 605.0f, 390.0f, 1.0f, 1.0f, 0.0f, D3DCOLOR_ARGB(245,0,0,0));
		sprintf_s(buffer, 16, "%i", m_nBounceBitMult);
		Font.DrawString(buffer, 730.0f - ( (m_nDigits[3]-1) * 16 ), 390.0f, 1.0f, 1.0f, 0.0f, D3DCOLOR_ARGB(245,0,0,0));
		sprintf_s(buffer, 16, "%i", m_nBounceBitMult);
		Font.DrawString(buffer, 556.0f, 438.0f, 1.0f, 1.0f, 0.0f, D3DCOLOR_ARGB(245,0,0,0));
		sprintf_s(buffer, 16, "%i", m_nTotal);
		Font.DrawString(buffer, 766.0f - ( (m_nDigits[4]-1) * 16 ), 438.0f, 1.0f, 1.0f, 0.0f, D3DCOLOR_ARGB(245,0,0,0));
		Font.DrawString(buffer, 588.0f, 488.0f, 1.0f, 1.0f, 0.0f, D3DCOLOR_ARGB(245,0,0,0));
		sprintf_s(buffer, 16, "%i", m_nPar);
		Font.DrawString(buffer, 665.0f, 535.0f, 1.0f, 1.0f, 0.0f, D3DCOLOR_ARGB(245,0,0,0));

		sprintf_s(buffer, 16, "press  ");
		buffer[6] = (char)KEY_ENTER;
		Font.DrawString(buffer, 470.0, 630.0f);
	}
	return true;
}
