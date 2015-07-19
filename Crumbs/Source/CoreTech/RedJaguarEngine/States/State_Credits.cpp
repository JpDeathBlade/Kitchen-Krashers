#include "State_Credits.h"
#include "../Asset/AssetManager.h"
#include "..\\Input\\CInputManager.h"
//#include "..\\Input\\Cursor.h"
#include "..\\..\\..\\RJ_Config.h"
#include "..\\..\\RJ_GameMain.h"
#include "../Text/font_defines.h"

#include "State_Config.h"

State_Credits::State_Credits(void) : Font("t_Crumbs_font.dds","Resources/Fonts/Crumbs_font.dat")
{
	m_nSelection = eMIN;
	tSC = NULL; 
	texID1 = CAssetManager::getInstanceRender()->LoadTexture("t_Credits_IPs_d.DDS");
	texID2 = CAssetManager::getInstanceRender()->LoadTexture("t_Credits_Art_Prog_d.DDS");
	texID3 = CAssetManager::getInstanceRender()->LoadTexture("t_Credits_Music_d.DDS");
	texID4 = CAssetManager::getInstanceRender()->LoadTexture("t_Credits_SpecialThanks_d.DDS");
	m_nPressESC = CAssetManager::getInstanceRender()->LoadTexture("t_PressESC.dds");
	
	start = D3DXVECTOR3(0.0f, 6.94687f, 0.0f);
	end = D3DXVECTOR3(0.0f, 6.94687f + 8.56f, 0.0f);
	temp = start;
	m_bExit = false;
	m_bDone = false;
	toast1 = true;
	m_bSwitch = false;
	page2 = false;
	page3 = false;
	page4 = false;
	timer = 0.0f;
}

void State_Credits::OnPush(void)
{
	if(tSC == NULL)
		tSC = tStateConfig::GetInstanceRender();

	m_bExit = false;
	m_bDone = false;
	toast1 = true;
	m_bSwitch = false;
	page2 = false;
	page3 = false;
	page4 = false;
	timer = 0.0f;
}

void State_Credits::OnEnter(void)
{
	D3DXMATRIX camera =
		D3DXMATRIX(	-0.870853f, -3.46945e-018f, 0.491543f, 0.0f,
		0.0520882f, 0.994369f, 0.0922831f, 0.0f,
		-0.488776f, 0.105969f, -0.86595f, 0.0f,
		15.0489f, 15.0727f, -27.5113f, 1.0f);

	volatile tCameraData* tCam = new tCameraData(EM_MENUCAMERA, camera);
	CEventManager::GetInstanceThreaded()->SendEventRenderCrossToUpdate(ET_MENUCAMERA, (tBaseEventData*)tCam);
	tCam = NULL;
}

void State_Credits::OnExit(void)
{

}
//void State_Credits::SendCreditsEvent(void)
//{
//	volatile tNumPlayers* tNP = new tNumPlayers(EM_NUMPLAYERS, NumPlayers);
//	CEventManager::GetInstanceThreaded()->SendEventRenderCrossToUpdate(ET_NUMPLAYERS,(tBaseEventData*)tNP);
//	tNP = NULL;
//	volatile tPlaySoundEnum* tSOUND = new tPlaySoundEnum(EM_PLAYSOUNDENUM, RJ_Config::RJ_UI_SELECT,true); 
//	CEventManager::GetInstanceThreaded()->SendEventRenderLocal(ET_PLAYSOUND,(tBaseEventData*)tSOUND);
//	tSOUND = NULL;
//}
bool State_Credits::Input(double _fDelta)
{
	if( (CInputManager::GetInstanceRender()->getKeysPressed() & RJ_Config::AP_CONFIRM)
		|| (CInputManager::GetInstanceRender()->getKeysPressed() & RJ_Config::AP_CANCEL) )
	{
		// We're always on eBACK...
		/*switch(m_nSelection)
		{
		case eBACK:
		{*/
		m_bExit = true;
		m_bSwitch = true;
		temp = end;
		volatile tPlaySoundEnum* tCTSOUND = new tPlaySoundEnum(EM_PLAYSOUNDENUM, RJ_Config::RJ_GUI_TOASTDOWN,true); 
		CEventManager::GetInstanceThreaded()->SendEventRenderLocal(ET_PLAYSOUND,(tBaseEventData*)tCTSOUND);
		tCTSOUND = NULL;
		//} break;
		//}
	}

	return true;
}

bool State_Credits::InputUnder(double _fDelta)
{
	return true;
}

bool State_Credits::Update(double _fDelta)
{
	timer += (float)_fDelta;
	if(timer >= 8.5f && !page2)
	{
		page2 = true;
		m_bSwitch = true;
		temp = end;
		volatile tPlaySoundEnum* tCTSOUND = new tPlaySoundEnum(EM_PLAYSOUNDENUM, RJ_Config::RJ_GUI_TOASTDOWN,true); 
		CEventManager::GetInstanceThreaded()->SendEventRenderLocal(ET_PLAYSOUND,(tBaseEventData*)tCTSOUND);
		tCTSOUND = NULL;
	}
	if(timer >= 17.0f && !page3)
	{
		page3 = true;
		m_bSwitch = true;
		temp = end;
		volatile tPlaySoundEnum* tCTSOUND = new tPlaySoundEnum(EM_PLAYSOUNDENUM, RJ_Config::RJ_GUI_TOASTDOWN,true); 
		CEventManager::GetInstanceThreaded()->SendEventRenderLocal(ET_PLAYSOUND,(tBaseEventData*)tCTSOUND);
		tCTSOUND = NULL;
	}
	if(timer >= 25.5f && !page4)
	{
		page4 = true;
		m_bSwitch = true;
		temp = end;
		volatile tPlaySoundEnum* tCTSOUND = new tPlaySoundEnum(EM_PLAYSOUNDENUM, RJ_Config::RJ_GUI_TOASTDOWN,true); 
		CEventManager::GetInstanceThreaded()->SendEventRenderLocal(ET_PLAYSOUND,(tBaseEventData*)tCTSOUND);
		tCTSOUND = NULL;
	}
	if(timer >= 34.0f && !m_bExit)
	{
		page2 = false;
		page3 = false;
		page4 = false;
		m_bSwitch = true;
		temp = end;
		timer = 0.0f;
		volatile tPlaySoundEnum* tCTSOUND = new tPlaySoundEnum(EM_PLAYSOUNDENUM, RJ_Config::RJ_GUI_TOASTDOWN,true); 
		CEventManager::GetInstanceThreaded()->SendEventRenderLocal(ET_PLAYSOUND,(tBaseEventData*)tCTSOUND);
		tCTSOUND = NULL;
	}
	if(!m_bSwitch && !m_bExit)
	{
		if(temp.y < end.y + 1.75f && m_bDone == false)
		{
			temp.y += (float)_fDelta * 25.0f;
			if(temp.y > end.y + 1.75f)
			{
				temp.y = end.y + 1.75f;
				m_bDone = true;
			}
		}
		else if(temp.y > end.y)
		{
			temp.y -= (float)_fDelta * 25.0f;
			if(temp.y < end.y)
				temp.y = end.y;
		}
	}
	else
	{
		if(temp.y > start.y)
		{
			temp.y -= (float)_fDelta * 25.0f;
			if(temp.y < start.y)
				temp.y = start.y;
		}
		else
		{
			//lerp = 1.0f;
			if(m_bExit)
			{	
				volatile tCameraPop* tPop = new tCameraPop(EM_CAMERAPOP);
				CEventManager::GetInstanceThreaded()->SendEventRenderCrossToUpdate(ET_CAMERAPOP, (tBaseEventData*)tPop);
				tPop = NULL;
				volatile tPlaySoundEnum* tSOUND = new tPlaySoundEnum(EM_PLAYSOUNDENUM, RJ_Config::RJ_UI_SELECT,true); 
				CEventManager::GetInstanceThreaded()->SendEventRenderLocal(ET_PLAYSOUND,(tBaseEventData*)tSOUND);
				tSOUND = NULL;

				RJ_MAIN_GAME->PopState();
			}
			else
			{
				toast1 = !toast1;
				temp = start;
				m_bSwitch = false;
				m_bDone = false;
				volatile tPlaySoundEnum* tCTSOUND = new tPlaySoundEnum(EM_PLAYSOUNDENUM, RJ_Config::RJ_GUI_TOASTUP,true); 
				CEventManager::GetInstanceThreaded()->SendEventRenderLocal(ET_PLAYSOUND,(tBaseEventData*)tCTSOUND);
				tCTSOUND = NULL;
			}
		}
	}
	return true;
}

bool State_Credits::UpdateUnder(double _fDelta)
{
	return true;
}

bool State_Credits::Render(double _fDelta)
{
	Update(_fDelta);
	//CAssetManager::getInstanceRender()->DrawFlatSprite(tSC->m_nCreditsBG,0.0f,0.0f,1.0f,1.0f);

	D3DXMATRIX matrix;
	if(toast1)
		D3DXMatrixTranslation(&matrix, 8.51941f, temp.y, -38.9727f);
	else
		D3DXMatrixTranslation(&matrix, 8.51941f, temp.y, -41.9462f);
	RJ_RENDERER->getDirect3DDevice()->SetTransform(D3DTS_WORLD, &matrix);

	if(page4 && !toast1)
		CAssetManager::getInstanceRender()->GetMesh( "LvMM_SM_Toast" )->setTextureID(texID4);
	else if(page3 && toast1)
		CAssetManager::getInstanceRender()->GetMesh( "LvMM_SM_Toast" )->setTextureID(texID3);
	else if(page2 && !toast1)
		CAssetManager::getInstanceRender()->GetMesh( "LvMM_SM_Toast" )->setTextureID(texID2);
	else
		CAssetManager::getInstanceRender()->GetMesh( "LvMM_SM_Toast" )->setTextureID(texID1);
	CAssetManager::getInstanceRender()->GetMesh( "LvMM_SM_Toast" )->Render(_fDelta);

	//if(!m_bExit)
	//{
	//	D3DXMatrixTranslation(&matrix, 5.0f, 6.15f + 8.56f, -38.5f);
	//	RJ_RENDERER->getDirect3DDevice()->SetTransform(D3DTS_WORLD, &matrix);
	//	CAssetManager::getInstanceRender()->GetMesh("SM_Sarrow")->Render(_fDelta);
	//}

	// render "Press ESC"
	CAssetManager::getInstanceRender()->DrawFlatSprite(m_nPressESC, 256.0f, 640.0f);

	return true;
}

bool State_Credits::RenderUnder(double _fDelta)
{
	return true;
}