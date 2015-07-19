#include "State_MainMenu.h"
#include "State_PlayerMenu.h"
#include "State_Credits.h"
#include "State_3DOptions.h"
#include "../Asset/AssetManager.h"
#include "..\\Input\\CInputManager.h"
#include "../Event/EventManager.h"
//#include "..\\Input\\Cursor.h"
#include "..\\..\\..\\RJ_Config.h"
#include "..\\..\\RJ_GameMain.h"
#include "../Text/font_defines.h"

#include "State_Config.h"

State_MainMenu::State_MainMenu(void) : Font("t_Crumbs_font.dds","Resources/Fonts/Crumbs_font.dat")
{
	m_nSelection = eMIN;
	tSC = NULL; 
}

void State_MainMenu::OnPush(void)
{
	if(tSC == NULL)
		tSC = tStateConfig::GetInstanceRender(); 

	texID = CAssetManager::getInstanceRender()->LoadTexture("T_Toaster_Menu_Main.DDS");
	CAssetManager::getInstanceRender()->LoadMesh("SM_Sarrow");
	LoadSettings();

	/*D3DXMATRIX camera =
		D3DXMATRIX( -0.857733f, 0.00597801f, 0.514061f, 0.0f,
		0.0580067f, 0.994672f, 0.0852196f, 0.0f,
		-0.510813f, 0.102915f, -0.85351f, 0.0f,
		21.5262f, 3.3434f, -10.7377f, 1.0f );

	volatile tCameraData* tCam = new tCameraData(EM_MENUCAMERA, camera);
	CEventManager::GetInstanceThreaded()->SendEventRenderCrossToUpdate(ET_MENUCAMERA, (tBaseEventData*)tCam);
	tCam = NULL;*/
}

void State_MainMenu::OnEnter(void)
{
	CAssetManager::getInstanceRender()->LoadTexture("t_SelectedBack_d.DDS");
	CAssetManager::getInstanceRender()->LoadTexture("t_SelectedFreezer_d.DDS");
	CAssetManager::getInstanceRender()->LoadTexture("t_SelectedSink_d.DDS");
	CAssetManager::getInstanceRender()->LoadTexture("t_SelectedStove_d.DDS");

	D3DXMATRIX camera =
		D3DXMATRIX( -0.857733f, 0.00597801f, 0.514061f, 0.0f,
		0.0580067f, 0.994672f, 0.0852196f, 0.0f,
		-0.510813f, 0.102915f, -0.85351f, 0.0f,
		21.5262f, 3.3434f, -10.7377f, 1.0f );

	volatile tCameraData* tCam = new tCameraData(EM_MENUCAMERA, camera);
	CEventManager::GetInstanceThreaded()->SendEventRenderCrossToUpdate(ET_MENUCAMERA, (tBaseEventData*)tCam);
	tCam = NULL;
}

void State_MainMenu::OnExit(void)
{

}
bool State_MainMenu::Input(double _fDelta)
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
		case eSTART:
			{
				//RJ_MAIN_GAME->PopState();
				RJ_MAIN_GAME->PushState(new State_PlayerMenu());
			} break;
		case eOPTIONS:
			{
				volatile tPlaySoundEnum* tCTSOUND = new tPlaySoundEnum(EM_PLAYSOUNDENUM, RJ_Config::RJ_GUI_TOASTUP,true); 
				CEventManager::GetInstanceThreaded()->SendEventRenderLocal(ET_PLAYSOUND,(tBaseEventData*)tCTSOUND);
				tCTSOUND = NULL;
				//RJ_MAIN_GAME->PopState();
				RJ_MAIN_GAME->PushState(new State_3DOptions());
			} break;
		case eCREDITS:
			{
				volatile tPlaySoundEnum* tCTSOUND = new tPlaySoundEnum(EM_PLAYSOUNDENUM, RJ_Config::RJ_GUI_TOASTUP,true); 
				CEventManager::GetInstanceThreaded()->SendEventRenderLocal(ET_PLAYSOUND,(tBaseEventData*)tCTSOUND);
				tCTSOUND = NULL;
				RJ_MAIN_GAME->PushState(new State_Credits());
			} break;
		case eEXIT:
			{
				PostQuitMessage(0);
			} break;
		}
	}

	return true;
}

bool State_MainMenu::InputUnder(double _fDelta)
{
	return true;
}

bool State_MainMenu::Update(double _fDelta)
{
	return true;
}

bool State_MainMenu::UpdateUnder(double _fDelta)
{
	return true;
}

bool State_MainMenu::Render(double _fDelta)
{
	//CAssetManager::getInstanceRender()->DrawFlatSprite(tSC->m_nMainMenuBG,0.0f,0.0f,1.0f,0.75f);
	CAssetManager::getInstanceRender()->GetMesh( "LvMM_SM_Toaster" )->setTextureID(texID);

	D3DXMATRIX matrix;
	D3DXMatrixTranslation(&matrix, 5.0f, 6.5f - m_nSelection, -28.5f);
	RJ_RENDERER->getDirect3DDevice()->SetTransform(D3DTS_WORLD, &matrix);
	CAssetManager::getInstanceRender()->GetMesh("SM_Sarrow")->Render(_fDelta);

	return true;
}

bool State_MainMenu::RenderUnder(double _fDelta)
{
	tSC->m_bIsLoading = false;
	return true;
}

bool State_MainMenu::LoadSettings()
{
	// parse file
	std::fstream file;
	file.open("Resources/Data/data.sav", std::ios_base::in | std::ios_base::binary);

	if (file.is_open())
	{
		// load info
		float fFX, fMusic, fGamma;
		bool bTutorial;
		file.read( (char*)&fFX, sizeof(float) );
		file.read( (char*)&fMusic, sizeof(float) );
		file.read( (char*)&fGamma, sizeof(float) );
		file.read( (char*)&bTutorial, sizeof(bool) );

		volatile tVolumeData* tVD = new tVolumeData(EM_ADJUSTVOLUME, 1, fFX); 
		CEventManager::GetInstanceThreaded()->SendEventRenderLocal(ET_ADJUSTVOLUME,(tBaseEventData*)tVD);
		tVD = NULL;
		tVD = new tVolumeData(EM_ADJUSTVOLUME, 2, fMusic); 
		CEventManager::GetInstanceThreaded()->SendEventRenderLocal(ET_ADJUSTVOLUME,(tBaseEventData*)tVD);
		tVD = NULL;
		volatile tGammaData* tGD = new tGammaData(EM_ADJUSTGAMMA, fGamma); 
		CEventManager::GetInstanceThreaded()->SendEventRenderLocal(ET_ADJUSTGAMMA,(tBaseEventData*)tGD);
		tGD = NULL;

		volatile tToggleTut* tNP = new tToggleTut(EM_TOGGLETUT, bTutorial);
		CEventManager::GetInstanceThreaded()->SendEventRenderCrossToUpdate(ET_TOGGLETUT,(tBaseEventData*)tNP);
		tNP = NULL;

		file.close();
		return true;
	}
	else
	{
		volatile tVolumeData* tVD = new tVolumeData(EM_ADJUSTVOLUME, 1, 50.0f); 
		CEventManager::GetInstanceThreaded()->SendEventRenderLocal(ET_ADJUSTVOLUME,(tBaseEventData*)tVD);
		tVD = NULL;
		tVD = new tVolumeData(EM_ADJUSTVOLUME, 2, 50.0f); 
		CEventManager::GetInstanceThreaded()->SendEventRenderLocal(ET_ADJUSTVOLUME,(tBaseEventData*)tVD);
		tVD = NULL;
		volatile tGammaData* tGD = new tGammaData(EM_ADJUSTGAMMA, 0.0f); 
		CEventManager::GetInstanceThreaded()->SendEventRenderLocal(ET_ADJUSTGAMMA,(tBaseEventData*)tGD);
		tGD = NULL;

		volatile tToggleTut* tNP = new tToggleTut(EM_TOGGLETUT, true);
		CEventManager::GetInstanceThreaded()->SendEventRenderCrossToUpdate(ET_TOGGLETUT,(tBaseEventData*)tNP);
		tNP = NULL;
	}
	
	return false;
}