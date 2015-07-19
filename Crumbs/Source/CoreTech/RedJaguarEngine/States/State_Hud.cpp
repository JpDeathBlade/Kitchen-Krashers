#include "State_Hud.h"
#include "../Event/EventDefinitions.h"
#include "../Event/EventManager.h"
#include "../Asset/AssetManager.h"
#include "..\\..\\RJ_GameMain.h"
#include "State_Pause.h"
#include "State_Config.h"
#include "../Text/font_defines.h"

#include <iostream>

void SetRectPosition(RECT& r, int index, int nCols, int nRows, int xOffeset = 256, int yOffset = 256)
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

State_HUD::State_HUD(void) : Font("t_Crumbs_font.dds","Resources/Fonts/Crumbs_font.dat"),
tDakotaFont("t_Dakota_font.dds","Resources/Fonts/Dakota_font.dat"), tSpecialFont("t_Special_font.dds","Resources/Fonts/Crumbs_font.dat")
{
	tSC = NULL;
	pAM = NULL;
	m_nCurrPlayerMultiplier = 1;
	m_nCurrHazard = -1;

	m_cCherries = 0;
	m_nPowerup = -1;
	m_fSpeed = 0.0f;
	m_fPrevSpeed = 0.0f;

	m_bCookie = false;
	m_bNewFC = false;
	m_nCookieFrame = 0;
	m_fCookieTimer = 0.0f;
	m_fFrameTimer = 0.0f;

	m_bGlow = false;
	m_fGlowTimer = 0.0f;

	m_fBrownieTimer = 0.0f;

	m_fSpinTimer = 0.0f;
	m_bFlipBottle = false;
	m_fFillTimer = 0.0f;
	m_nCurrentFillFrame = 0;
	m_nNextFillFrame = 1;
	m_bSlowHazard = false;

	m_bNoOverlay = false;
	m_nPrevMultiplier = 1;
	m_fMultiplierTimer = 0.0f;
	m_bStopped = false;
	m_nOffsetArray[0] = 0;
	m_nOffsetArray[1] = 480;
	m_nMultFrame = 0;

	m_nValue = 1;
	m_fScale = 1.0f;

	m_nScore[0] = 0;
	m_nScore[1] = 0;
	m_nScore[2] = 0;
	m_nScore[3] = 0;

	m_nPreviousScore[0] = 0;
	m_nPreviousScore[1] = 0;
	m_nPreviousScore[2] = 0;
	m_nPreviousScore[3] = 0;

	m_vColors[0][0] = 153; m_vColors[0][1] = 13;  m_vColors[0][2] = 0;
	m_vColors[1][0] = 129; m_vColors[1][1] = 29;  m_vColors[1][2] = 156;
	m_vColors[2][0] = 216; m_vColors[2][1] = 118; m_vColors[2][2] = 6;
	m_vColors[3][0] = 12;  m_vColors[3][1] = 204; m_vColors[3][2] = 87;
	m_vColors[4][0] = 255;  m_vColors[4][1] = 255; m_vColors[4][2] = 255;

	m_bVolSpeedChanged = false;
	m_fSpeedChange = 0.0f;

	m_bIsCherryShot = false;
	m_fCherryTimer = 0.0f;
	m_bIsFoulShot = false;
	m_bDrawPressUp = true;
	m_bDrawGum = false;
	m_bDrawTurkey = false;

	m_fCurrX = 465.0f;
	m_fCurrY = 104.0f;
	m_bDrawPOW = true;
	m_bDoneSpinning = false;

	m_nSplashID[0] = -1;
	m_nSplashID[1] = -1;
	m_nSplashID[2] = -1;
	m_fSplashTimer = 0.0f;
	m_fSplashDrip = 0;
	m_bSplash = false;
	m_bFadeSplash = false;
}
void State_HUD::OnPush(void)
{
	CEventManager::GetInstanceThreaded()->RegisterListenerRender(ET_PLAYERDATA,this);
	CEventManager::GetInstanceThreaded()->RegisterListenerRender(ET_PAUSE,this);
	CEventManager::GetInstanceThreaded()->RegisterListenerRender(ET_BROWNIEDATA,this);
	CEventManager::GetInstanceThreaded()->RegisterListenerRender(ET_RESETHUD,this);
	CEventManager::GetInstanceThreaded()->RegisterListenerRender(ET_RESETPOW,this);
	CEventManager::GetInstanceThreaded()->RegisterListenerRender(ET_SETPOW,this);
	CEventManager::GetInstanceThreaded()->RegisterListenerRender(ET_STONELAUNCHED, this);
	CEventManager::GetInstanceThreaded()->RegisterListenerRender(ET_CHERRYSHOT, this);
	CEventManager::GetInstanceThreaded()->RegisterListenerRender(ET_FOULSHOT, this);
	CEventManager::GetInstanceThreaded()->RegisterListenerRender(ET_NOMULTIPLIER, this);
	CEventManager::GetInstanceThreaded()->RegisterListenerRender(ET_PLAYERGOTCREAM, this);
	CEventManager::GetInstanceThreaded()->RegisterListenerRender(ET_NOPRESSUP, this);
	CEventManager::GetInstanceThreaded()->RegisterListenerRender(ET_SETTURKEY, this);
	CEventManager::GetInstanceThreaded()->RegisterListenerRender(ET_SETGUM, this);

	if(tSC == NULL)
		tSC = tStateConfig::GetInstanceRender();
	if(pAM == NULL)
		pAM = CAssetManager::getInstanceRender();

	// load images
	m_nFoulImageID = pAM->LoadTexture("t_FoulShot_Break.dds");
	m_nPressUpID = pAM->LoadTexture("t_PressUp.dds");
	m_nSplashID[0] = pAM->LoadTexture("T_SplashB_D.dds");
	m_nSplashID[1] = pAM->LoadTexture("T_SplashA_D.dds");
	m_nSplashID[2] = pAM->LoadTexture("T_SplashC_D.dds");
	tSC->m_nGummy = rand()%4 + 1;
	tSC->m_nWormGum = rand()%6 + 1;
}
void State_HUD::OnEnter(void)
{

}
void State_HUD::OnExit(void)
{

}
void State_HUD::OnPop(void)
{
	CEventManager::GetInstanceThreaded()->UnregisterListenerAllRender(this);

	tSC = NULL;
	pAM = NULL;
	for (unsigned int i = 0; i < 4; i++)
		m_vNewPoints[i].clear();
	m_vSpecialShots.clear();
	delete this;
}

bool State_HUD::Input(double _fDelta)
{
	return true;
}
bool State_HUD::InputUnder(double _fDelta)
{
	return true;
}
bool State_HUD::Update(double _fDelta)
{
	return true;
}
bool State_HUD::UpdateUnder(double _fDelta)
{
	return true;
}
bool State_HUD::Render(double _fDelta)
{
	RJ_GameMain::GetInstance()->setRenderPause(false);
	char buffer[128] = "";
	if(m_nCurrentPlayer >= 0 && m_nCurrentPlayer < 4)
	{		
		RECT rCapRect;
		int nMainColor = 255;
		int nCounter = 0;
		int nCapWidth = 0;
		float fAlpha = 0;
		float fScale = 1.0f;
		int nDynamicHeight = 64 * (tSC->m_nNumberofPlayers - 2);
		if(tSC->m_nNumberofPlayers == 0)
			return true;
		for(int i = 0; i < tSC->m_nNumberofPlayers-1; ++i)
		{
			SetRectPosition(rCapRect, m_nPlayerOrder[i], 2, 2);
			nCapWidth = 8 * (m_nRemainingShots[m_nPlayerOrder[i]]-1);
			// draw atleast 1 cap
			pAM->DrawFlatSprite(tSC->m_nStonesID, 22.0f, 683.0f + (64 * i) - nDynamicHeight, 0.25f, 0.25f, &rCapRect);
			for (int _cap = 1; _cap < m_nRemainingShots[m_nPlayerOrder[i]]; _cap++)
				pAM->DrawFlatSprite(tSC->m_nStonesID, 22.0f + (8*_cap), 683.0f + (64 * i) - nDynamicHeight, 0.25f, 0.25f, &rCapRect);
			// remaining shots
			buffer[0] = (char)(m_nRemainingShots[ m_nPlayerOrder[i] ] + NUMBER_0);
			buffer[1] = (char)0;
			Font.DrawString(buffer, 40.0f + nCapWidth, 683.0f + (i * 64.0f) - nDynamicHeight);
			//Font.DrawString(buffer, 42.0f + nCapWidth, 683.0f + (i * 64.0f) - nDynamicHeight, 1.0f, 1.0f, 0.0f, D3DCOLOR_ARGB(185,0,0,0));
			// total score
			sprintf_s(buffer, 128, "%i", (m_nScore[ m_nPlayerOrder[i] ]));
			Font.DrawString(buffer, 89.0f + nCapWidth, 685.0f + (i * 64.0f) - nDynamicHeight);
			if ( m_cCherries & (1<<m_nPlayerOrder[i]) )
			{
				buffer[1] = (char)0;
				buffer[0] = (char)(IMG_CHERRY);
				Font.DrawString(buffer, 80.0f + nCapWidth, 686.0f + (i * 64.0f) - nDynamicHeight, 0.4f, 0.4f);
			}

			// "+5"
			for ( unsigned int _point = 0; _point < m_vNewPoints[m_nPlayerOrder[i]].size(); _point++)
			{
				m_vNewPoints[m_nPlayerOrder[i]][_point].fTime += (float)_fDelta;
				if(m_vNewPoints[m_nPlayerOrder[i]][_point].fTime != 0.0f)
					fAlpha = 255 * ( 1 - (m_vNewPoints[m_nPlayerOrder[i]][_point].fTime/2.0f) );

				// switch color @ every other point
				if (nCounter%2 == 0)
					nMainColor = 199;
				else
					nMainColor = 255;

				if ( m_vNewPoints[m_nPlayerOrder[i]][_point].nPoints > 0 )
				{
					sprintf_s(buffer, 128, "+%i", m_vNewPoints[m_nPlayerOrder[i]][_point].nPoints);
					tDakotaFont.DrawString(buffer, 90.0f + nCapWidth + (m_nDigits[m_nPlayerOrder[i]]*16) + (nCounter * 56), 678.0f + (i * 64.0f) - nDynamicHeight, 1.5f, 1.5f, 0.0f,
						D3DCOLOR_ARGB((int)fAlpha,6,nMainColor,0) );
				}
				else
				{
					sprintf_s(buffer, 128, "%i", m_vNewPoints[m_nPlayerOrder[i]][_point].nPoints);
					tDakotaFont.DrawString(buffer, 90.0f + nCapWidth + (m_nDigits[m_nPlayerOrder[i]]*16) + (nCounter * 56), 678.0f + (i * 64.0f) - nDynamicHeight, 1.5f, 1.5f, 0.0f,
						D3DCOLOR_ARGB((int)fAlpha,nMainColor,6,0) );
				}
				nCounter++;
			}
		}

		/// <Current Player>
		SetRectPosition(rCapRect, m_nCurrentPlayer, 2, 2);
		nCapWidth = 10 * (m_nRemainingShots[m_nCurrentPlayer]-1);
		// draw atleast 1 cap
		pAM->DrawFlatSprite(tSC->m_nStonesID, 10.0f, 10.0f, 0.35f, 0.35f, &rCapRect);
		for (int i = 1; i < m_nRemainingShots[m_nCurrentPlayer]; i++)
			pAM->DrawFlatSprite(tSC->m_nStonesID, 10.0f + (10*i), 10.0f, 0.35f, 0.35f, &rCapRect);
		// remaining shots
		buffer[0] = (char)(m_nRemainingShots[m_nCurrentPlayer] + NUMBER_0);
		buffer[1] = (char)0;
		Font.DrawString(buffer, 37.0f + nCapWidth, 20.0f, 1.25f, 1.25f);
		//Font.DrawString(buffer, 39.0f + nCapWidth, 20.0f, 1.25f, 1.25f, 0.0f, D3DCOLOR_ARGB(185,0,0,0));
		// total score +
		sprintf_s(buffer, 128, "%i ", m_nScore[m_nCurrentPlayer] - m_nCurrStoneScore);
		Font.DrawString(buffer, 102.0f + nCapWidth, 30.0f);
		//	cherries
		int nPOWOffset = 0;
		if ( m_cCherries & (1<<m_nCurrentPlayer) )
		{
			buffer[0] = (char)IMG_CHERRY;
			buffer[1] = (char)0;
			Font.DrawString(buffer, 90.0f + nCapWidth, 15.0f, 0.6f, 0.6f);
			//nPOWOffset += 40;
		}
		// gum
		if (m_bDrawGum)
		{
			buffer[0] = (char)IMG_GUM;
			buffer[1] = (char)0;
			Font.DrawString(buffer, 130.0f + nCapWidth, 15.0f, 0.6f, 0.6f);
			//nPOWOffset += 40;
		}
		if (m_bDrawTurkey)
		{
			buffer[0] = (char)IMG_TURKEY;
			buffer[1] = (char)0;
			Font.DrawString(buffer, 170.0f + nCapWidth, 15.0f, 0.6f, 0.6f);
		}
			
		// turkey
		// stone score
		sprintf_s(buffer, 128, " + %i", m_nCurrStoneScore);
		Font.DrawString(buffer, 105.0f + nCapWidth + (16 * m_nDigits[m_nCurrentPlayer]), 30.0f, 1.0f, 1.0f, 0.0f,
			D3DCOLOR_ARGB(255, m_vColors[m_nCurrentPlayer][0], m_vColors[m_nCurrentPlayer][1], m_vColors[m_nCurrentPlayer][2]) );
		// "+5"
		for ( unsigned int _point = 0; _point < m_vNewPoints[m_nCurrentPlayer].size(); _point++)
		{
			// switch color @ every other point
			if (nCounter%2 == 0)
				nMainColor = 199;
			else
				nMainColor = 255;

			m_vNewPoints[m_nCurrentPlayer][_point].fTime += (float)_fDelta;
			if(m_vNewPoints[m_nCurrentPlayer][_point].fTime != 0.0f)
				fAlpha = 255 * ( 1 - (m_vNewPoints[m_nCurrentPlayer][_point].fTime/2.0f) );
			if ( m_vNewPoints[m_nCurrentPlayer][_point].fTime > 0 )
			{
				if ( m_vNewPoints[m_nCurrentPlayer][_point].nPoints > 0 )
				{
					sprintf_s(buffer, 128, "+%i ", m_vNewPoints[m_nCurrentPlayer][_point].nPoints);
					tDakotaFont.DrawString(buffer, 115.0f + nCapWidth + (16 * m_nDigits[m_nCurrentPlayer]) + (m_nStoneDigits * 16) + (nCounter * 56),
						30.0f, 1.5f, 1.5f, 0.0f, D3DCOLOR_ARGB((int)fAlpha,6,nMainColor,0));
				}
				else 
				{
					sprintf_s(buffer, 128, "%i", m_vNewPoints[m_nCurrentPlayer][_point].nPoints);
					tDakotaFont.DrawString(buffer, 115.0f + nCapWidth + (16 * m_nDigits[m_nCurrentPlayer]) + (m_nStoneDigits * 16) + (nCounter * 56),
						30.0f, 1.5f, 1.5f, 0.0f, D3DCOLOR_ARGB((int)fAlpha,nMainColor,6,0));
				}
				nCounter++;
			}
		}
		// if (alone)
		if (tSC->m_nNumberofPlayers == 1)
		{	// draw par
			tDakotaFont.DrawString("par:", 90.0f, 93.0f, 1.0f, 1.0f, 0.0f, D3DCOLOR_ARGB(255,0,0,0));
			tDakotaFont.DrawString("par:", 90.0f, 90.0f);
			sprintf_s(buffer, 128, " %i", m_nPar);
			tDakotaFont.DrawString(buffer, 125.0f, 93.0f, 1.0f, 1.0f, 0.0f, D3DCOLOR_ARGB(255,0,0,0));
			tDakotaFont.DrawString(buffer, 125.0f, 90.0f);
		}

		// draw or do not draw!
		if (!m_bNoOverlay)
		{
			/// <special Shots>
			if ( !m_vSpecialShots.empty() )
			{
				int nNameLength = strlen(m_vSpecialShots[0].szName);
				int nTmp = m_vSpecialShots[0].nValue;
				int nDigits = 1;
				while ( nTmp > 9 )
				{
					nDigits++;
					nTmp /= 10;
				}

				m_fBrownieTimer += (float)_fDelta;
				if (m_fBrownieTimer > 1.5f)
					m_fBrownieTimer = 1.5f;
				if(m_fBrownieTimer != 0.0f)
				{
					fAlpha = 255 * ( 1 - (m_fBrownieTimer/1.5f) );
					fScale = 1.0f + (m_fBrownieTimer/1.5f);
				}
				tSpecialFont.DrawString( m_vSpecialShots[0].szName, 512.0f - (nNameLength * 15), 234.0f - (32 * fScale) - (40 * m_fBrownieTimer), fScale, fScale, 0.0f,
					D3DCOLOR_ARGB( (int)fAlpha,255,255,255) );

				sprintf_s(buffer, 128, "%i", m_vSpecialShots[0].nValue);/*+ (16 * fScale)*/
				tSpecialFont.DrawString( buffer, 512.0f - (nDigits * 16), 254.0f - (40 * m_fBrownieTimer), fScale, fScale, 0.0f,
					D3DCOLOR_ARGB( (int)fAlpha,255,255,255) );
			}

			/// <splashes>
			int nNegAlpha = 255;
			m_fSplashDrip += (float)(20 * _fDelta);
			if (m_bFadeSplash)
			{
				m_fSplashTimer += (float)_fDelta;
				if (m_fSplashTimer > 2.5f)
				{
					m_fSplashTimer = 0.0f;
					m_fSplashDrip = 0.0f;
					m_bSplash = false;
				}
				if(m_fSplashTimer != 0.0f)
					fAlpha = m_fSplashTimer / 2.5f;
				nNegAlpha = 255 - (int)(fAlpha * 255);
			}

			if ( m_bSplash )
			{
				// sides
				pAM->DrawFlatSprite(m_nSplashID[1], -256.0f, 256.0f + (int)m_fSplashDrip, 1.0f, 1.0f, NULL, 0.0f, 0.0f, 0.0f,
					D3DCOLOR_ARGB(nNegAlpha, m_vColors[4][0], m_vColors[4][1], m_vColors[4][2]));
				pAM->DrawFlatSprite(m_nSplashID[1], 768.0f, 256.0f + (int)m_fSplashDrip, 1.0f, 1.0f, NULL, 0.0f, 0.0f, 0.0f,
					D3DCOLOR_ARGB(nNegAlpha, m_vColors[4][0], m_vColors[4][1], m_vColors[4][2]));

				// top & bottom
				for (unsigned int i = 0; i < 3; i++)
				{
					pAM->DrawFlatSprite(m_nSplashID[i], -256.0f + (512.0f * i), -256.0f + (int)m_fSplashDrip, 1.0f, 1.0f, NULL, 0.0f, 0.0f, 0.0f,
						D3DCOLOR_ARGB(nNegAlpha, m_vColors[4][0], m_vColors[4][1], m_vColors[4][2]));
					pAM->DrawFlatSprite(m_nSplashID[i], -256.0f + (512.0f * i), 512.0f + (int)m_fSplashDrip, 1.0f, 1.0f, NULL, 0.0f, 0.0f, 0.0f,
						D3DCOLOR_ARGB(nNegAlpha, m_vColors[4][0], m_vColors[4][1], m_vColors[4][2]));
				}
			}

			///	<power up>
			buffer[1] = (char)0;
			buffer[0] = (char)(m_nPowerup + POW_CHERRY);
			// move icon
			if (m_bDoneSpinning)
			{
				// move to top-right corner;
				//	quickly!
				m_fCurrX += 223.5f * (float)_fDelta;
				m_fCurrY += 50.0f * (float)_fDelta;

				if (m_fCurrX > 465.0f)
				{
					// we can turn glow on now
					// because these values should move evenly
					m_fCurrX = 465.0f;
					m_bDoneSpinning = false;	//we don't need to do this anymore
					m_bGlow = true;
				}
				if (m_fCurrY > 104.0f)
				{
					m_fCurrY = 104.0f;
					m_bDoneSpinning = false;
					m_bGlow = true;
				}

				// draw empty POW [?]
				buffer[0] = (char)POW_EMPTY;
				Font.DrawString(buffer, 945.0f, 24.0f);
				buffer[0] = (char)(m_nPowerup + POW_CHERRY);
			}
			if (m_bDrawPOW)
				Font.DrawString(buffer, 480.0f + m_fCurrX, 128.0f - m_fCurrY);//945.0f, 24.0f);
			//Update Cookie
			if (m_bCookie)
			{
				m_bDrawPOW = false;
				m_bGlow = false;

				m_fCookieTimer += (float)_fDelta;
				if (m_fCookieTimer >= 0.05f)
				{
					m_fCookieTimer = 0.0f;
					m_nCookieFrame++;
					if (m_nCookieFrame > 5)
						m_nCookieFrame = 0;
				}
				m_fFrameTimer += (float)_fDelta;
				if (m_fFrameTimer >= 2.0f)
				{
					m_fFrameTimer = 0;
					m_bDrawPOW = true;
					m_bCookie = false;
					
					m_fCurrX = 0.0f;
					m_fCurrY = 0.0f;
					m_bDoneSpinning = true;
				}

				buffer[0] = (char)(POW_CHERRY + m_nCookieFrame);
				//Font.DrawString(buffer, 945.0f, 16.0f);
				Font.DrawString(buffer, 480.0f, 128.0f);

				// draw empty POW [?]
				buffer[0] = (char)POW_EMPTY;
				Font.DrawString(buffer, 945.0f, 24.0f);
				buffer[0] = (char)(m_nPowerup + POW_CHERRY);

				// write "fortune"
				tSpecialFont.DrawString("fortune", 465.0f, 64.0f);
				// write "cookie"
				tSpecialFont.DrawString("cookie", 465.0f, 192.0f);
			}

			// glow
			if (m_bGlow)
			{
				m_fGlowTimer += (float)_fDelta;
				buffer[0] = (char)IMG_GLOW;
				if(m_fGlowTimer != 0.0f)
					fAlpha = 255 * (1 - (m_fGlowTimer/3.0f));/*913.0f*//*0.0f*/
				Font.DrawString( buffer, 448.0f + m_fCurrX, 104.0f - m_fCurrY, 2.0f, 2.0f, 0.0f,
					D3DCOLOR_ARGB((int)fAlpha, 255,245,255) );

				if (m_fGlowTimer >= 3.0f)
				{
					m_bGlow = false;
					m_fGlowTimer = 0.0f;
				}

				//std::cout << "<GLOW!: t = " << m_fGlowTimer <<"; " << "a = " << fAlpha << ";\n";
			}

			/// <speed indicator>
			RECT rBottle;
			if(m_fSpeed == 0.0f)
				m_fSpeed = 0.0000001f;
			float fPercent = m_fSpeed / 7.0f;
			float fSpin = 0.0f;
			float fMove = 0.0f;
			// animation
			m_fFillTimer += (float)_fDelta;
			if ( m_fFillTimer >= 1.5f )
			{
				m_fFillTimer = 0.0f;
				m_nCurrentFillFrame = m_nNextFillFrame;
				m_nNextFillFrame++;

				if (m_nNextFillFrame > 2)
					m_nNextFillFrame = 0;
			}

			// color for hazard-highlight
			DWORD dwColor = D3DCOLOR_ARGB(255, 255, 255, 255);
			if (m_bSlowHazard)
				dwColor = D3DCOLOR_ARGB(255, 245, 0, 0);
			else if( !m_bSlowHazard && m_nCurrHazard > -1 )
				dwColor = D3DCOLOR_ARGB(255, 0, 245, 0);

			// has the player launched yet?
			if (m_bFlipBottle && _fDelta != 0.0)
			{
				m_fSpinTimer += (float)_fDelta;
				if (m_fSpinTimer > 1.0f)
					m_fSpinTimer = 1.0f;
				if(m_fSpinTimer != 0.0f)
					fSpin = m_fSpinTimer / 1.0f;
				fMove = m_fSpinTimer * 256;

				if (m_fSpeedChange >= m_fSpeed)
					m_bVolSpeedChanged = false;

				// back-plate
				rBottle.left = 80; rBottle.top = 0; rBottle.right = 160; rBottle.bottom = 256;
				pAM->DrawFlatSprite(tSC->m_nBottleID, 920.0f, 495.0f - fMove, 1.0f, 1.0f, &rBottle, 40.0f, 256.0f, -fSpin * D3DX_PI, dwColor);

				if (m_bVolSpeedChanged)
				{
					/*rBottle.left = 480; rBottle.top = 0; rBottle.right = 480; rBottle.bottom = LONG(fPercent * 256);
					pAM->DrawFlatSprite(tSC->m_nBottleID, 918.0f, 495.0f - fMove, 1.0f, 1.0f, &rBottle, 40.0f, 256.0f, -fSpin * D3DX_PI);*/

					// current green
					rBottle.left = 480 + (80*m_nCurrentFillFrame); rBottle.top = 0; rBottle.right = 560 + (80*m_nCurrentFillFrame); rBottle.bottom = LONG(fPercent * 256);
					pAM->DrawFlatSprite(tSC->m_nBottleID, 922.0f, 495.0f - fMove, 1.0f, 1.0f, &rBottle, 40.0f, 256.0f, -fSpin * D3DX_PI);
					// next green
					if(m_fFillTimer != 0.0f)
						fAlpha = 255.0f * (m_fFillTimer / 1.5f);
					rBottle.left = 480 + (80*m_nNextFillFrame); rBottle.top = 0; rBottle.right = 560 + (80*m_nNextFillFrame); rBottle.bottom = LONG(fPercent * 256);
					pAM->DrawFlatSprite(tSC->m_nBottleID, 922.0f, 495.0f - fMove, 1.0f, 1.0f, &rBottle, 40.0f, 256.0f, -fSpin * D3DX_PI, D3DCOLOR_ARGB((int)fAlpha,255,255,255));

					// make other fill draw smaller
					if(m_fSpeedChange != 0.0f)
						fPercent = m_fSpeedChange/7.0f;
				}

				// current fill
				rBottle.left = 160 + (80*m_nCurrentFillFrame); rBottle.top = 0; rBottle.right = 240 + (80*m_nCurrentFillFrame); rBottle.bottom = LONG(fPercent * 256);
				pAM->DrawFlatSprite(tSC->m_nBottleID, 920.0f, 495.0f - fMove, 1.0f, 1.0f, &rBottle, 40.0f, 256.0f, -fSpin * D3DX_PI);
				// next fill
				if(m_fFillTimer != 0.0f)
					fAlpha = 255.0f * (m_fFillTimer / 1.5f);
				rBottle.left = 160 + (80*m_nNextFillFrame); rBottle.top = 0; rBottle.right = 240 + (80*m_nNextFillFrame); rBottle.bottom = LONG(fPercent * 256);
				pAM->DrawFlatSprite(tSC->m_nBottleID, 920.0f, 495.0f - fMove, 1.0f, 1.0f, &rBottle, 40.0f, 256.0f, -fSpin * D3DX_PI, D3DCOLOR_ARGB((int)fAlpha,255,255,255));
			}
			else
			{
				// back-plate
				rBottle.left = 80; rBottle.top = 0; rBottle.right = 160; rBottle.bottom = 256;
				pAM->DrawFlatSprite(tSC->m_nBottleID, 920.0f, 495.0f, 1.0f, 1.0f, &rBottle, 1.0f, 1.0f, 0.0f, dwColor);

				// current fill
				rBottle.left = 160 + (80*m_nCurrentFillFrame); rBottle.top = LONG(256 - fPercent * 256);
				rBottle.right = 240 + (80*m_nCurrentFillFrame); rBottle.bottom = 256;
				pAM->DrawFlatSprite(tSC->m_nBottleID, 920.0f, 495.0f + (float)rBottle.top, 1.0f, 1.0f, &rBottle);
				// next fill
				if(m_fFillTimer != 0.0f)
					fAlpha = 255.0f * (m_fFillTimer / 1.5f);
				rBottle.left = 160 + (80*m_nNextFillFrame); rBottle.top = LONG(256 - fPercent * 256);
				rBottle.right = 240 + (80*m_nNextFillFrame); rBottle.bottom = 256;
				pAM->DrawFlatSprite(tSC->m_nBottleID, 920.0f, 495.0f + (float)rBottle.top, 1.0f, 1.0f, &rBottle, 0.0f, 0.0f, 0.0f,
					D3DCOLOR_ARGB((int)fAlpha, 255,255,255));

				// Press Up!
				if (m_bDrawPressUp)
					pAM->DrawFlatSprite(m_nPressUpID, 256.0f, 640.0f);
			}

			// outline
			rBottle.left = 0; rBottle.top = 0; rBottle.right = 80; rBottle.bottom = 256;
			pAM->DrawFlatSprite(tSC->m_nBottleID, 920.0f, 495.0f - fMove, 1.0f, 1.0f, &rBottle, 40.0f, 256.0f, -fSpin * D3DX_PI, dwColor);

			/// <Multiplier>
			if (m_nCurrPlayerMultiplier > 1)
			{
				m_fMultiplierTimer += (float)_fDelta;
				/*m_fScale = 1.0f + (m_fMultiplierTimer/0.75f) * (float)m_nValue;
				if (m_fScale < 2.0f)
					m_fScale = 2.0f;*/

				if (m_bStopped)
				{
					RECT rMult;
					SetRectPosition(rMult, m_nMultFrame, 5, 5, 96, 96);
					if (m_fMultiplierTimer >= 0.04f)
					{
						m_fMultiplierTimer = 0.0f;
						m_nMultFrame++;
					}
					if (m_nMultFrame > 24)
						m_nMultFrame = 24;

					// add x-offset to rMult

					if(m_nCurrPlayerMultiplier == 0)
						m_nCurrPlayerMultiplier = 1;
					int nMult = (m_nCurrPlayerMultiplier / 2) - 1;
					rMult.left += m_nOffsetArray[nMult % 2];
					rMult.right += m_nOffsetArray[nMult % 2];
					// add y-offset to rMult
					nMult -= 1;
					if (nMult < 1) nMult = 0;
					else if (nMult > 0) nMult = 1;
					rMult.top += m_nOffsetArray[nMult];
					rMult.bottom += m_nOffsetArray[nMult];

					pAM->DrawFlatSprite(tSC->m_nMultiplierSprites, 400.0f/*464.0f*/, 32.0f/*96.0f*/, 2.0f, 2.0f, &rMult);
				}
				else
				{
					if (m_fMultiplierTimer >= 0.75f)
						m_nValue = -1;

					// set color
					int color[3] = {255,255,255};
					if (m_nCurrPlayerMultiplier == 4)
					{	// blue
						color[0] = 0;
						color[1] = 0;
						color[2] = 234;
					}
					else if (m_nCurrPlayerMultiplier == 6)
					{	// red
						color[0] = 234;
						color[1] = 0;
						color[2] = 0; 
					}
					else if (m_nCurrPlayerMultiplier == 8)
					{	// gold
						color[0] = 255;
						color[1] = 215;
						color[2] = 0; 
					}

					sprintf_s(buffer, 128, "x%i", m_nCurrPlayerMultiplier);
					Font.DrawString(buffer, 512.0f, 24.0f, m_fScale, m_fScale, 0.0f,
						D3DCOLOR_ARGB(255, color[0], color[1], color[2]) );
				}

				volatile tGoalEffect* tGE = new tGoalEffect(EM_GOALEFFECT,m_nCurrPlayerMultiplier);
				CEventManager::GetInstanceThreaded()->SendEventRenderLocal(ET_GOALEFFECT,(tBaseEventData*)tGE);
				tGE = NULL;
			}

			/// <current hazard>
			if (m_nCurrHazard > -1)
			{
				if (!m_bDoneSpinning && !m_bCookie)
				{				
					switch (m_nCurrHazard)
					{
					case 0:	// water
						Font.DrawString("Water", 480.0f, 100.0f);
						Font.DrawString("slowing down!", 428.0f, 132.0f, 1.0f, 1.0f, 0.0f, D3DCOLOR_ARGB(255, 245, 0, 0));
						break;
					case 1:	// oil
						Font.DrawString("Oil Slick", 465.0f, 100.0f);
						Font.DrawString("\"speeding up!\"", 428.0f, 132.0f, 1.0f, 1.0f, 0.0f, D3DCOLOR_ARGB(255, 0, 245, 0));
						break;
					case 2: // soda
						Font.DrawString("Soda Spill", 465.0f, 100.0f);
						Font.DrawString("slowing down!", 428.0f, 132.0f, 1.0f, 1.0f, 0.0f, D3DCOLOR_ARGB(255, 245, 0, 0));
						break;
					case 3:	// ice
						Font.DrawString("Ice Patch", 465.0f, 100.0f);
						Font.DrawString("\"speeding up!\"", 428.0f, 132.0f, 1.0f, 1.0f, 0.0f, D3DCOLOR_ARGB(255, 245, 0, 0));
						break;
					case 4:	// sticky mess (popsicle)
						Font.DrawString("Popsicle Mess!", 455.0f, 100.0f);
						Font.DrawString("slowing down!", 428.0f, 132.0f, 1.0f, 1.0f, 0.0f, D3DCOLOR_ARGB(255, 245, 0, 0));
					}
				}
			}

			/// <Notifications>
			// Cherry Shot
			if (m_bIsCherryShot)
			{
				m_fCherryTimer += (float)_fDelta;
				if(m_fCherryTimer != 0.0f)
					fAlpha = 255 * (m_fCherryTimer/2.0f);

				if (m_fCherryTimer > 2.0f)
				{
					m_fCherryTimer = 2.0f;
					m_bIsCherryShot = false;
				}

				//Font.DrawString("CHERRY SHOT!", 465.0f, 256.0f, 1.0f, 1.0f, 0.0f, D3DCOLOR_ARGB((int)fAlpha, 255, 255, 255));
				pAM->DrawFlatSprite(tSC->m_nCherryShotID, 0.0f, 0.0f, 1.0f, 1.0f, NULL, 1.0f, 1.30f, 0.0f, D3DCOLOR_ARGB((int)fAlpha, 255, 255, 255));
			}
			// Foul Shot
			if (m_bIsFoulShot)
				pAM->DrawFlatSprite(m_nFoulImageID); 
		}

		/// <clean up!>
		std::vector<tNewPoint>::iterator pointIter;
		for (unsigned int i = 0; i < 4; i++)
		{
			pointIter = m_vNewPoints[i].begin();

			// erase faster if there are too many
			while (m_vNewPoints[i].size() > 3)
				pointIter = m_vNewPoints[i].erase(pointIter);	//pop_front()

			while (pointIter != m_vNewPoints[i].end())
			{
				if (pointIter->fTime > 2.0f)
					pointIter = m_vNewPoints[i].erase(pointIter);
				else
					pointIter++;
			}
		}
		if (m_fBrownieTimer >= 1.5f)
		{
			m_fBrownieTimer = 0.0f;
			std::vector<tBrowniePoint>::iterator brownieIter = m_vSpecialShots.begin();
			m_vSpecialShots.erase(brownieIter);
		}
	}
	return true;
}
bool State_HUD::RenderUnder(double _fDelta)
{
	//Render(_fDelta);
	return true;
}
void State_HUD::HandleEvent(CEvent* pEvent)
{
	if(pEvent->GetEventID() == ET_PLAYERDATA)
	{
		if(pEvent->GetData() != NULL)
		{
			if(pEvent->GetData()->GetMsgID() == EM_PLAYERDATA)
			{
				tPlayerData* tPdat;
				tPdat = (tPlayerData*)pEvent->GetData();

				// check old speed
				if ( (tPdat->m_fSpeed < m_fSpeed) && (tPdat->m_fSpeed < 0.00025f) )
					m_bStopped = true;

				m_nCurrentPlayer = tPdat->m_nCurrPlayer;
				tSC->m_nCurrPlayer = m_nCurrentPlayer;
				m_nCurrStoneScore = tPdat->m_nCurrStoneScore;
				m_fPrevSpeed = m_fSpeed;
				m_bNewFC = tPdat->m_bFortuneCookie;
				m_nPar = tPdat->m_nPar;

				// current volSpeed
				if (tPdat->m_nCurrVolSpeed > 0)
				{
					m_bVolSpeedChanged = true;
					m_fSpeedChange = m_fSpeed;
				}
				m_fSpeed = tPdat->m_fSpeed;

				// current hazard
				m_nCurrHazard = tPdat->m_nCurrHazard;
				m_bSlowHazard = false;
				if ( m_nCurrHazard == 0 || 
					m_nCurrHazard == 2 ||
					m_nCurrHazard == 4 )
				{
					m_bSlowHazard = true;
				}

				// setup splash effect
				m_bFadeSplash = true;
				if (m_nCurrHazard > -1 && m_nCurrHazard != 3 && m_nCurrHazard != 4) // everything but ICE & STICKY
				{
					m_bSplash = true;
					m_bFadeSplash = false;
					m_fSplashTimer = 0.0f;

					// set color[4]
					if (m_nCurrHazard == 1) // OIL
					{
						m_vColors[4][0] = 245;
						m_vColors[4][1] = 237;
						m_vColors[4][2] = 137;
					}
					else if (m_nCurrHazard == 2) // SODA
					{
						m_vColors[4][0] = 52;
						m_vColors[4][1] = 29;
						m_vColors[4][2] = 12;
					}
					else	// default to white
					{
						m_vColors[4][0] = 255;
						m_vColors[4][1] = 255;
						m_vColors[4][2] = 255;
					}
				}

				// store multiplier
				m_nPrevMultiplier = m_nCurrPlayerMultiplier;
				m_nCurrPlayerMultiplier = tPdat->m_nCurrPlayerMultiplier;
				if ( m_nPrevMultiplier != m_nCurrPlayerMultiplier )
				{
					// reset
					m_fMultiplierTimer = 0.0f;
					m_nValue = 1;
					m_fScale = 1.0f;

					//std::cout << "<RESET MULTIPLIER>\n";
				}

				if (m_bNewFC)
					m_bCookie = true;

				/*if ( tPdat->m_eCurrPowerUp > -1 )
				{
				m_nPowerup = tPdat->m_eCurrPowerUp;
				m_bGlow = true;
				}

				if ( m_nPowerup == 0 )
				m_cCherries |= (1<<m_nCurrentPlayer);*/

				int nDifference = 0;
				for(int i = 0; i < tSC->m_nNumberofPlayers; ++i)
				{	
					m_nPreviousScore[i] = m_nScore[i];
					m_nRemainingShots[i] = tPdat->m_nRemainingShots[i];
					m_nScore[i] = tPdat->m_nScores[i];

					nDifference = m_nScore[i] - m_nPreviousScore[i];
					if (nDifference != 0)
					{
						tNewPoint nPoint;
						nPoint.fTime = 0.0f;
						nPoint.nPoints = nDifference;
						m_vNewPoints[i].push_back(nPoint);
					}

					// count digits
					m_nDigits[i] = 1;
					int nTmp = m_nScore[i];
					while( nTmp > 9 )
					{
						nTmp /= 10;
						m_nDigits[i]++;
					}
				}

				// count digits for current stone
				m_nDigits[m_nCurrentPlayer] = 1;
				int nTmp = m_nScore[m_nCurrentPlayer] - m_nCurrStoneScore;
				while( nTmp > 9 )
				{
					nTmp /= 10;
					m_nDigits[m_nCurrentPlayer]++;
				}
				m_nStoneDigits = 1;
				nTmp = m_nCurrStoneScore;
				while( nTmp > 9 )
				{
					nTmp /= 10;
					m_nStoneDigits++;
				}

				// sort players
				int nMax, nMin;
				int nMaxIndex, nMidIndex, nMinIndex;
				switch (m_nCurrentPlayer)
				{
				case 0:
					nMaxIndex = 1;
					nMidIndex = 2;
					nMinIndex = 3;
					break;
				case 1:
					nMaxIndex = 0;
					nMidIndex = 2;
					nMinIndex = 3;
					break;
				case 2:
					nMaxIndex = 0;
					nMidIndex = 1;
					nMinIndex = 3;
					break;
				case 3:
					nMaxIndex = 0;
					nMidIndex = 1;
					nMinIndex = 2;
					break;
				}
				nMax = m_nScore[nMaxIndex];
				nMin = m_nScore[nMinIndex];

				for (unsigned int i = 0; i < 4; i++)
				{
					if (i == m_nCurrentPlayer)
						continue;	

					if (m_nScore[i] > nMax)
					{
						nMax = m_nScore[i];
						nMaxIndex = i;
					}
					else if (m_nScore[i] <= nMin)
					{
						nMin = m_nScore[i];
						nMinIndex = i;
					}
				}

				for (unsigned int i = 0; i < 4; i++)
				{
					if (i != nMaxIndex &&
						i != nMinIndex &&
						i != m_nCurrentPlayer)
					{
						nMidIndex = i;
						break;
					}
				}

				m_nPlayerOrder[0] = nMaxIndex;
				m_nPlayerOrder[1] = nMidIndex;
				m_nPlayerOrder[2] = nMinIndex;
			}
		}
	}
	else if(pEvent->GetEventID() == ET_PAUSE)
	{
		if(pEvent->GetData() != NULL)
		{
			if(pEvent->GetData()->GetMsgID() == EM_PAUSE)
			{
				RJ_MAIN_GAME->setRenderPause(true);
				RJ_MAIN_GAME->PushState(new State_Pause());
			}
		}
	}
	else if(pEvent->GetEventID() == ET_BROWNIEDATA)
	{
		if(pEvent->GetData() != NULL)
		{
			if(pEvent->GetData()->GetMsgID() == EM_BROWNIEDATA)
			{
				tBrownieData* tBdat;
				tBdat = (tBrownieData*)pEvent->GetData();

				tBrowniePoint tPoint;
				tPoint.szName = tBdat->m_szName;
				tPoint.nValue = tBdat->m_nValue;

				m_vSpecialShots.push_back(tPoint);
			}
		}
	}
	else if(pEvent->GetEventID() == ET_RESETHUD)
	{
		if(pEvent->GetData() != NULL)
		{
			if(pEvent->GetData()->GetMsgID() == EM_RESETHUD)
			{
				m_bDrawPOW = true;
				m_nCurrPlayerMultiplier = 1;
				m_cCherries = 0;
				m_nPowerup = -1;
				m_bCookie = false;
				m_bNewFC = false;
				m_nCookieFrame = 0;
				m_fCookieTimer = 0.0f;
				m_fFrameTimer = 0.0f;
				m_bGlow = false;
				m_fGlowTimer = 0.0f;
				m_fBrownieTimer = 0.0f;
				m_nScore[0] = 0;
				m_nScore[1] = 0;
				m_nScore[2] = 0;
				m_nScore[3] = 0;
				m_nPreviousScore[0] = 0;
				m_nPreviousScore[1] = 0;
				m_nPreviousScore[2] = 0;
				m_nPreviousScore[3] = 0;

				m_fSpeed = 0.0f;
				m_fPrevSpeed = 0.0f;

				m_vSpecialShots.clear();
				for (unsigned int i = 0; i < 4; i++)
					m_vNewPoints[i].clear();

				m_fSpinTimer = 0.0f;
				m_bFlipBottle = false;
				m_fFillTimer = 0.0f;
				m_nCurrentFillFrame = 0;
				m_nNextFillFrame = 1;
				m_bSlowHazard = false;
				m_nCurrHazard = -1;

				m_bNoOverlay = false;
				m_nPrevMultiplier = 1;
				m_fMultiplierTimer = 0.0f;
				m_nMultFrame = 0;
				m_bStopped = false;

				m_bVolSpeedChanged = false;
				m_fSpeedChange = 0.0f;

				m_bIsCherryShot = false;
				m_fCherryTimer = 0.0f;
				m_bIsFoulShot = false;
				m_bDrawPressUp = true;
				m_bDrawGum = false;
				m_bDrawTurkey = false;

				m_fSplashTimer = 0.0f;
				m_fSplashDrip = 0;
				m_bSplash = false;
				m_bFadeSplash = false;

				m_fCurrX = 465.0f;
				m_fCurrY = 104.0f;
				m_bDoneSpinning = false;

				//std::cout << "<RESET HUD> " << m_nPowerup << "\n";
			}
		}
	}
	else if(pEvent->GetEventID() == ET_RESETPOW)
	{
		if(pEvent->GetData() != NULL)
		{
			if(pEvent->GetData()->GetMsgID() == EM_RESETPOW)
			{
				m_bDrawPOW = true;
				m_nPowerup = -1;
				m_bCookie = false;
				m_bNewFC = false;
				m_nCookieFrame = 0;
				m_fCookieTimer = 0.0f;
				m_fFrameTimer = 0.0f;
				m_bGlow = false;
				m_fGlowTimer = 0.0f;
				m_fBrownieTimer = 0.0f;

				m_fSpinTimer = 0.0f;
				m_bFlipBottle = false;
				m_fFillTimer = 0.0f;
				m_nCurrentFillFrame = 0;
				m_nNextFillFrame = 1;
				m_bSlowHazard = false;
				m_nCurrHazard = -1;

				m_fSpeed = 0.0f;
				m_fPrevSpeed = 0.0f;

				m_vSpecialShots.clear();
				m_bNoOverlay = false;
				m_nPrevMultiplier = 1;
				m_fMultiplierTimer = 0.0f;
				m_nMultFrame = 0;
				m_bStopped = false;

				m_bVolSpeedChanged = false;
				m_fSpeedChange = 0.0f;

				m_bIsCherryShot = false;
				m_fCherryTimer = 0.0f;
				m_bIsFoulShot = false;
				m_bDrawPressUp = true;
				m_bDrawGum = false;
				m_bDrawTurkey = false;

				m_fSplashTimer = 0.0f;
				m_fSplashDrip = 0;
				m_bSplash = false;
				m_bFadeSplash = false;

				m_fCurrX = 465.0f;
				m_fCurrY = 104.0f;
				m_bDoneSpinning = false;

				//std::cout << "<RESET POW> " << m_nPowerup << "\n";
			}
		}
	}
	else if(pEvent->GetEventID() == ET_SETPOW)
	{
		if(pEvent->GetData() != NULL)
		{
			if(pEvent->GetData()->GetMsgID() == EM_SETPOW)
			{
				tPowerUpData* tDat;
				tDat = (tPowerUpData*)pEvent->GetData();

				// reset
				//m_bDrawGum = false;
				//m_bDrawTurkey = false;
				/// <actually, don't do that>

				if ( !(m_cCherries & (1<<m_nCurrentPlayer))
					|| tDat->m_nPowerUp > 0 )
				{
					if ( tDat->m_nPowerUp > -1 && tDat->m_nPowerUp != m_nPowerup )
					{
						m_nPowerup = tDat->m_nPowerUp;
						m_bGlow = true;
					}

					if ( m_nPowerup == 0 )	// CHERRIES
						m_cCherries |= (1<<m_nCurrentPlayer);

					if (m_nPowerup == 1)	// TURKEY
						m_bDrawTurkey = true;
					if (m_nPowerup == 2)	// GUM
						m_bDrawGum = true;

					//std::cout << "<SET POW> " << m_nPowerup << "\n";
				}
			}
		}
	}
	else if(pEvent->GetEventID() == ET_STONELAUNCHED)
	{
		if(pEvent->GetData() != NULL)
		{
			if(pEvent->GetData()->GetMsgID() == EM_STONELAUNCHED)

			{
				m_fSpinTimer = 0.0f;
				m_bFlipBottle = true;
				m_bStopped = false;

				//std::cout << "<FLIP BOTTLE>\n";
			}
		}
	}
	else if(pEvent->GetEventID() == ET_CHERRYSHOT)
	{
		if(pEvent->GetData() != NULL)
		{
			if(pEvent->GetData()->GetMsgID() == EM_CHERRYSHOT)
			{
				m_bIsCherryShot = true;
				m_fCherryTimer = 0.0f;
				m_bDrawGum = false;
				m_bDrawTurkey = false;
			}
		}
	}
	else if(pEvent->GetEventID() == ET_FOULSHOT)
	{
		if(pEvent->GetData() != NULL)
		{
			if(pEvent->GetData()->GetMsgID() == EM_FOULSHOT)
				m_bIsFoulShot = true;
		}
	}
	else if(pEvent->GetEventID() == ET_NOMULTIPLIER) //Don't draw overly when the camera is up here
	{
		if(pEvent->GetData() != NULL)
		{
			if(pEvent->GetData()->GetMsgID() == EM_NOMULTIPLIER)
			{
				tNoMultiplier* tDat;
				tDat = (tNoMultiplier*)pEvent->GetData();

				m_bNoOverlay = tDat->m_bValue;
			}
		}
	}
	else if(pEvent->GetEventID() == ET_PLAYERGOTCREAM)
	{
		if(pEvent->GetData() != NULL)
		{
			if(pEvent->GetData()->GetMsgID() == EM_PLAYERGOTCREAM)
			{
				m_bVolSpeedChanged = true;
				m_fSpeedChange = m_fSpeed;
			}
		}
	}
	else if(pEvent->GetEventID() == ET_SETTURKEY)
	{
		if(pEvent->GetData() != NULL)
		{
			if(pEvent->GetData()->GetMsgID() == EM_SETTURKEY)
			{
				tSetTurkey* tDat;
				tDat = (tSetTurkey*)pEvent->GetData();
				//m_bDrawTurkey = tDat->m_bValue;
			}
		}
	}
	else if(pEvent->GetEventID() == ET_SETGUM)
	{
		if(pEvent->GetData() != NULL)
		{
			//if(pEvent->GetData()->GetMsgID() == EM_SETGUM)
				//m_bDrawGum = true;
		}
	}
	else if(pEvent->GetEventID() == ET_NOPRESSUP)
	{
		if(pEvent->GetData() != NULL)
		{
			if(pEvent->GetData()->GetMsgID() == EM_NOPRESSUP)
				m_bDrawPressUp = false;
		}
	}

	pEvent->m_nRefs--;
}