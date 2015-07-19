#include "State_PlayerMenu.h"
#include "State_MainMenu.h"
#include "State_LevelMenu.h"
#include "../Asset/AssetManager.h"
#include "..\\Input\\CInputManager.h"
//#include "..\\Input\\Cursor.h"
#include "..\\..\\..\\RJ_Config.h"
#include "..\\..\\RJ_GameMain.h"
#include "../Text/font_defines.h"

#include "State_Config.h"

State_PlayerMenu::State_PlayerMenu(void) : Font("t_Crumbs_font.dds","Resources/Fonts/Crumbs_font.dat")
{
	m_nSelection = eMIN;
	tSC = NULL; 
}

void State_PlayerMenu::OnPush(void)
{
	if(tSC == NULL)
		tSC = tStateConfig::GetInstanceRender();

	texID = CAssetManager::getInstanceRender()->LoadTexture("T_Toaster_Players.DDS");
	CAssetManager::getInstanceRender()->GetMesh( "LvMM_SM_Toaster" )->setTextureID(texID);
}

void State_PlayerMenu::OnEnter(void)
{
	D3DXMATRIX camera =
		D3DXMATRIX( -0.857733f, 0.00597801f, 0.514061f, 0.0f,
		0.0580067f, 0.994672f, 0.0852196f, 0.0f,
		-0.510813f, 0.102915f, -0.85351f, 0.0f,
		21.5262f, 3.3434f, -10.7377f, 1.0f );

	volatile tCameraData* tCam = new tCameraData(EM_MENUCAMERA, camera);
	CEventManager::GetInstanceThreaded()->SendEventRenderCrossToUpdate(ET_MENUCAMERA, (tBaseEventData*)tCam);
	tCam = NULL;
}

void State_PlayerMenu::OnExit(void)
{

}
void State_PlayerMenu::SendPlayerMenuEvent(void)
{
	volatile tNumPlayers* tNP = new tNumPlayers(EM_NUMPLAYERS, NumPlayers);
	CEventManager::GetInstanceThreaded()->SendEventRenderCrossToUpdate(ET_NUMPLAYERS,(tBaseEventData*)tNP);
	tNP = NULL;
	volatile tPlaySoundEnum* tSOUND = new tPlaySoundEnum(EM_PLAYSOUNDENUM, RJ_Config::RJ_UI_SELECT,true); 
	CEventManager::GetInstanceThreaded()->SendEventRenderLocal(ET_PLAYSOUND,(tBaseEventData*)tSOUND);
	tSOUND = NULL;
}
bool State_PlayerMenu::Input(double _fDelta)
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
	else if(CInputManager::GetInstanceRender()->getKeysPressed() & RJ_Config::AP_CANCEL)
	{
		volatile tCameraPop* tPop = new tCameraPop(EM_CAMERAPOP);
		CEventManager::GetInstanceThreaded()->SendEventRenderCrossToUpdate(ET_CAMERAPOP, (tBaseEventData*)tPop);
		tPop = NULL;
	/*	volatile tPlaySoundEnum* tCTSOUND = new tPlaySoundEnum(EM_PLAYSOUNDENUM, RJ_Config::RJ_GUI_TOASTDOWN,true); 
		CEventManager::GetInstanceThreaded()->SendEventRenderLocal(ET_PLAYSOUND,(tBaseEventData*)tCTSOUND);
		tCTSOUND = NULL;*/
		volatile tPlaySoundEnum* tSOUND = new tPlaySoundEnum(EM_PLAYSOUNDENUM, RJ_Config::RJ_UI_SELECT,true); 
		CEventManager::GetInstanceThreaded()->SendEventRenderLocal(ET_PLAYSOUND,(tBaseEventData*)tSOUND);
		tSOUND = NULL;
		RJ_MAIN_GAME->PopState();
		//RJ_MAIN_GAME->PushState(new State_MainMenu());
	}
	else if ( CInputManager::GetInstanceRender()->getKeysDown() & RJ_Config::AP_RIGHT )
	{
		switch(m_nSelection)
		{
		case ePLAYONE:
			{
				m_nSelection = ePLAYTHREE;
				volatile tPlaySoundEnum* tSOUND = new tPlaySoundEnum(EM_PLAYSOUNDENUM, RJ_Config::RJ_UI_NAVIGATE,true); 
				CEventManager::GetInstanceThreaded()->SendEventRenderLocal(ET_PLAYSOUND,(tBaseEventData*)tSOUND);
				tSOUND = NULL;
			} break;
		case ePLAYTWO:
			{
				m_nSelection = ePLAYFOUR;
				volatile tPlaySoundEnum* tSOUND = new tPlaySoundEnum(EM_PLAYSOUNDENUM, RJ_Config::RJ_UI_NAVIGATE,true); 
				CEventManager::GetInstanceThreaded()->SendEventRenderLocal(ET_PLAYSOUND,(tBaseEventData*)tSOUND);
				tSOUND = NULL;
			} break;
		}
	}
	else if ( CInputManager::GetInstanceRender()->getKeysDown() & RJ_Config::AP_LEFT )
	{
		switch(m_nSelection)
		{
		case ePLAYTHREE:
			{
				m_nSelection = ePLAYONE;
				volatile tPlaySoundEnum* tSOUND = new tPlaySoundEnum(EM_PLAYSOUNDENUM, RJ_Config::RJ_UI_NAVIGATE,true); 
				CEventManager::GetInstanceThreaded()->SendEventRenderLocal(ET_PLAYSOUND,(tBaseEventData*)tSOUND);
				tSOUND = NULL;
			} break;
		case ePLAYFOUR:
			{
				m_nSelection = ePLAYTWO;
				volatile tPlaySoundEnum* tSOUND = new tPlaySoundEnum(EM_PLAYSOUNDENUM, RJ_Config::RJ_UI_NAVIGATE,true); 
				CEventManager::GetInstanceThreaded()->SendEventRenderLocal(ET_PLAYSOUND,(tBaseEventData*)tSOUND);
				tSOUND = NULL;
			} break;
		}
	}

	if(CInputManager::GetInstanceRender()->getKeysPressed() & RJ_Config::AP_CONFIRM)
	{
		volatile tPlaySoundEnum* tSOUND = new tPlaySoundEnum(EM_PLAYSOUNDENUM, RJ_Config::RJ_UI_SELECT,true); 
		CEventManager::GetInstanceThreaded()->SendEventRenderLocal(ET_PLAYSOUND,(tBaseEventData*)tSOUND);
		tSOUND = NULL;
		switch(m_nSelection)
		{
		case ePLAYONE:
			{	
				NumPlayers = 1;
				tSC->m_nNumberofPlayers = 1;				
			} break;
		case ePLAYTWO:
			{	
				NumPlayers = 2;
				tSC->m_nNumberofPlayers = 2;
			} break;
		case ePLAYTHREE:
			{	
				NumPlayers = 3;
				tSC->m_nNumberofPlayers = 3;
			} break;
		case ePLAYFOUR:
			{	
				NumPlayers = 4;
				tSC->m_nNumberofPlayers = 4;
			} break;
		case eBACK:
			{
				RJ_MAIN_GAME->PopState();
				//RJ_MAIN_GAME->PushState(new State_MainMenu());
				return true;
			} break;
		}

		//RJ_MAIN_GAME->PopState();
		RJ_MAIN_GAME->PushState(new State_LevelMenu(NumPlayers));
	}

	return true;
}

bool State_PlayerMenu::InputUnder(double _fDelta)
{
	return true;
}

bool State_PlayerMenu::Update(double _fDelta)
{
	return true;
}

bool State_PlayerMenu::UpdateUnder(double _fDelta)
{
	return true;
}

bool State_PlayerMenu::Render(double _fDelta)
{
	//CAssetManager::getInstanceRender()->DrawFlatSprite(tSC->m_nPlayerMenuBG, 305.0f, 301.0f);

	D3DXMATRIX matrix;
	switch(m_nSelection)
	{
		case ePLAYONE:
			{
				D3DXMatrixTranslation(&matrix, 5.0f, 6.55f, -28.5f);
			} break;
		case ePLAYTWO:
			{
				D3DXMatrixTranslation(&matrix, 5.0f, 5.6f, -28.5f);
			} break;
		case ePLAYTHREE:
			{
				D3DXMatrixTranslation(&matrix, 11.25f, 6.55f, -28.5f);
			} break;
		case ePLAYFOUR:
			{
				D3DXMatrixTranslation(&matrix, 11.25f, 5.6f, -28.5f);
			} break;
		case eBACK:
			{
				D3DXMatrixTranslation(&matrix, 5.0f, 4.0f, -28.5f);
			} break;
	}
	
	RJ_RENDERER->getDirect3DDevice()->SetTransform(D3DTS_WORLD, &matrix);
	CAssetManager::getInstanceRender()->GetMesh("SM_Sarrow")->Render(_fDelta);

	return true;
}

bool State_PlayerMenu::RenderUnder(double _fDelta)
{
	return true;
}