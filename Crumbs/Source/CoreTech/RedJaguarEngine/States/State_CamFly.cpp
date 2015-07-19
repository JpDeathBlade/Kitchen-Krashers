#include "State_CamFly.h"
#include "State_Config.h"
#include "../Event/EventDefinitions.h"
#include "../Event/EventManager.h"
#include "../Asset/AssetManager.h"
#include "../Text/CBitmapFont.h"
#include "../Input/CInputManager.h"
#include "../../RJ_GameMain.h"
#include "../text/font_defines.h"

State_CamFly::State_CamFly(void) : Font("t_Crumbs_font.dds","Resources/Fonts/Crumbs_font.dat")
{
	tSC = NULL;
	pAM = NULL;
}

void State_CamFly::OnPush(void)
{
	if(tSC == NULL)
		tSC = tStateConfig::GetInstanceRender();
	if(pAM == NULL)
		pAM = CAssetManager::getInstanceRender();

	volatile tPause* tNP = new tPause(EM_PAUSE, true);
	CEventManager::GetInstanceThreaded()->SendEventRenderCrossToUpdate(ET_PAUSE,(tBaseEventData*)tNP);
	tNP = NULL;

	dTime = 0.0;
	fAlpha = 0.0f;
	bLeaving = false;
	bDone = false;
}

bool State_CamFly::Render(double _fDelta)
{
	// kill yoself
	if ( CInputManager::GetInstanceRender()->getKeysPressed() & RJ_Config::AP_CONFIRM )
		bLeaving = true;

	// update time/alpha
	if ( !bLeaving )
	{
		dTime += _fDelta;
		if (dTime > 2.0f)
			dTime = 2.0f;
		if(dTime != 0.0f)
			fAlpha = (float)( 255 * (dTime/2.0) );
	}
	else 
	{
		if (dTime > 1.0f)
			dTime = 1.0f;

		dTime -= _fDelta;
		if (dTime < 0.01f)
		{
			dTime = 0.01f;
			bDone = true;
		}
		if(dTime != 0.0f)
			fAlpha = (float)( 255 * (dTime/1.0) );
	}

	// camera bars!
	Font.DrawLetter( (char)A_BLACK_BOX, -32.0f, -32.0f, 17.0f, 2.0f, 0.0, D3DCOLOR_ARGB((int)fAlpha, 255, 255, 255) );
	Font.DrawLetter( (char)A_BLACK_BOX, -32.0f, 672.0f, 17.0f, 2.0f, 0.0, D3DCOLOR_ARGB((int)fAlpha, 255, 255, 255) );

	char buffer[16] = "";
	sprintf_s(buffer, 16, "press  ");
	buffer[6] = (char)KEY_ENTER;
	buffer[7] = 0;
	Font.DrawString(buffer, 470.0, 688.0f, 1.0f, 1.0f, 0.0, D3DCOLOR_ARGB((int)fAlpha, 255, 255, 255) );

	// are we done yet?
	if (bDone)
	{
		volatile tCamFly* tNP = new tCamFly(EM_CAMFLY);
		CEventManager::GetInstanceThreaded()->SendEventRenderCrossToUpdate(ET_CAMFLY,(tBaseEventData*)tNP);
		tNP = NULL;
		volatile tPause* tPG = new tPause(EM_PAUSE);
		CEventManager::GetInstanceThreaded()->SendEventRenderCrossToUpdate(ET_PAUSE,(tBaseEventData*)tPG);
		tPG = NULL;
		RJ_MAIN_GAME->PopState();
	}

	return true;
}
