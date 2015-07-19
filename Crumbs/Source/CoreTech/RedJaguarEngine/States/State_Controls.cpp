#include "State_Controls.h"
#include "State_MainMenu.h"
#include "State_LevelMenu.h"
#include "../Asset/AssetManager.h"
#include "..\\Input\\CInputManager.h"
//#include "..\\Input\\Cursor.h"
#include "..\\..\\..\\RJ_Config.h"
#include "..\\..\\RJ_GameMain.h"
#include "../Text/font_defines.h"

#include "State_Config.h"

State_Controls::State_Controls(void) : Font("t_Crumbs_font.dds","Resources/Fonts/Crumbs_font.dat")
{
	m_nSelection = eMIN;
	tSC = NULL; 

	start = D3DXVECTOR3(255.0f, 768.0f, 0.0f);
	end = D3DXVECTOR3(255.0f, 128.0f, 0.0f);
	temp = start;

	Astart = D3DXVECTOR3(575.0f, 768.0f + 500.0f, 0.0f);
	Aend = D3DXVECTOR3(575.0f, 500.0f, 0.0f);
	Atemp = Astart;

	lerpdown = 0;
}

void State_Controls::OnPush(void)
{
	if(tSC == NULL)
		tSC = tStateConfig::GetInstanceRender();
}

void State_Controls::OnEnter(void)
{

}

void State_Controls::OnExit(void)
{

}
//void State_Controls::SendControlsEvent(void)
//{
//	volatile tNumPlayers* tNP = new tNumPlayers(EM_NUMPLAYERS, NumPlayers);
//	CEventManager::GetInstanceThreaded()->SendEventRenderCrossToUpdate(ET_NUMPLAYERS,(tBaseEventData*)tNP);
//	tNP = NULL;
//	volatile tPlaySoundEnum* tSOUND = new tPlaySoundEnum(EM_PLAYSOUNDENUM, RJ_Config::RJ_UI_SELECT,true); 
//	CEventManager::GetInstanceThreaded()->SendEventRenderLocal(ET_PLAYSOUND,(tBaseEventData*)tSOUND);
//	tSOUND = NULL;
//}
bool State_Controls::Input(double _fDelta)
{
	if(CInputManager::GetInstanceRender()->getKeysPressed() & RJ_Config::AP_CANCEL)
	{
		volatile tPlaySoundEnum* tSOUND = new tPlaySoundEnum(EM_PLAYSOUNDENUM, RJ_Config::RJ_UI_SELECT,true); 
		CEventManager::GetInstanceThreaded()->SendEventRenderLocal(ET_PLAYSOUND,(tBaseEventData*)tSOUND);
		tSOUND = NULL;
		RJ_MAIN_GAME->PopState();
		//	RJ_MAIN_GAME->PushState(new State_MainMenu());
	}

	if(lerpdown == 3)
		if(CInputManager::GetInstanceRender()->getKeysPressed() & RJ_Config::AP_CONFIRM)
		{
			volatile tPlaySoundEnum* tSOUND = new tPlaySoundEnum(EM_PLAYSOUNDENUM, RJ_Config::RJ_UI_SELECT,true); 
			CEventManager::GetInstanceThreaded()->SendEventRenderLocal(ET_PLAYSOUND,(tBaseEventData*)tSOUND);
			tSOUND = NULL;
			switch(m_nSelection)
			{
			case eBACK:
				{
					lerpdown = 1;
					//	RJ_MAIN_GAME->PushState(new State_MainMenu());
				} break;
			}
		}

	return true;
}

bool State_Controls::InputUnder(double _fDelta)
{
	return true;
}

bool State_Controls::Update(double _fDelta)
{
	return true;
}

bool State_Controls::UpdateUnder(double _fDelta)
{
	return true;
}

bool State_Controls::Render(double _fDelta)
{
	float lerp = 2.5f * (float)_fDelta;
	if(lerpdown == 1)
	{
		end.y = 800.0f;
		D3DXVec3Lerp(&temp, &start, &end, lerp);
		start = temp;
		CAssetManager::getInstanceRender()->DrawFlatSprite(tSC->m_nControlsBG,temp.x,temp.y,1.0f,1.0f);
		if(start.y >= 768.0f)
			lerpdown = 2;
	}
	else if(lerpdown == 2)
	{
		RJ_MAIN_GAME->PopState();
	}
	else if (lerpdown == 0)
	{
		D3DXVec3Lerp(&temp, &start, &end, lerp);
		start = temp;
		CAssetManager::getInstanceRender()->DrawFlatSprite(tSC->m_nControlsBG,temp.x,temp.y,1.0f,1.0f);

		if(start.y - end.y <= 1.0f)
			lerpdown = 3;

	}
	else if(lerpdown == 3)
	{
		D3DXVec3Lerp(&temp, &start, &end, lerp);
		start = temp;
		CAssetManager::getInstanceRender()->DrawFlatSprite(tSC->m_nControlsBG,temp.x,temp.y,1.0f,1.0f);

		//Font.DrawString("Crumbs!",300,100);

		char Buffer[2];
		Buffer[0] = (char)(IMG_UP);
		Buffer[1] = (char)0;
		switch (m_nSelection)
		{
		case eBACK:
			{
				Font.DrawString(Buffer,575.0f,500.0f,0.75f, 0.75f, 1.5f);
			} break;
		}
		//Cursor::GetInstanceRender()->Render();
	}
	return true;
}

bool State_Controls::RenderUnder(double _fDelta)
{
	CAssetManager::getInstanceRender()->DrawFlatSprite(tSC->m_nControlsBG,temp.x,temp.y,1.0f,1.0f);

	return true;
}