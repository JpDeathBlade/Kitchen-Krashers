#include "State_3DControls.h"
#include "../Asset/AssetManager.h"
#include "..\\Input\\CInputManager.h"
#include "..\\..\\..\\RJ_Config.h"
#include "..\\..\\RJ_GameMain.h"

#include "State_Config.h"

State_3DControls::State_3DControls(void)
{
	m_nSelection = eMIN;
	tSC = NULL; 
	texID = CAssetManager::getInstanceRender()->LoadTexture("T_Toast_Menu_Controls.dds");

	start = D3DXVECTOR3(0.0f, 6.94687f, 0.0f);
	end = D3DXVECTOR3(0.0f, 6.94687f + 8.56f, 0.0f);
	temp = start;
	lerp = 0.0f;
	m_bDone = false;
	m_bExit = false;
}

void State_3DControls::OnPush(void)
{
	if(tSC == NULL)
		tSC = tStateConfig::GetInstanceRender();
	
	
}

void State_3DControls::OnEnter(void)
{
	D3DXMATRIX camera =
		D3DXMATRIX(	-0.870853f, -3.46945e-018f, 0.491543f, 0.0f,
		0.0520882f, 0.994369f, 0.0922831f, 0.0f,
		-0.488776f, 0.105969f, -0.86595f, 0.0f,
		15.0489f, 16.0727f, -21.9371f, 1.0f);

	volatile tCameraData* tCam = new tCameraData(EM_MENUCAMERA, camera);
	CEventManager::GetInstanceThreaded()->SendEventRenderCrossToUpdate(ET_MENUCAMERA, (tBaseEventData*)tCam);
	tCam = NULL;
}

void State_3DControls::OnExit(void){}

bool State_3DControls::Input(double _fDelta)
{
	if( (CInputManager::GetInstanceRender()->getKeysPressed() & RJ_Config::AP_CONFIRM)
		|| (CInputManager::GetInstanceRender()->getKeysPressed() & RJ_Config::AP_CANCEL) )
	{
		m_bExit = true;
		temp = end;
		lerp = 0;
		volatile tPlaySoundEnum* tCTSOUND = new tPlaySoundEnum(EM_PLAYSOUNDENUM, RJ_Config::RJ_GUI_TOASTDOWN,true); 
		CEventManager::GetInstanceThreaded()->SendEventRenderLocal(ET_PLAYSOUND,(tBaseEventData*)tCTSOUND);
		tCTSOUND = NULL;
	}


	return true;
}

bool State_3DControls::InputUnder(double _fDelta)
{
	return true;
}

bool State_3DControls::Update(double _fDelta)
{
	return true;
}

bool State_3DControls::UpdateUnder(double _fDelta)
{
	return true;
}

bool State_3DControls::Render(double _fDelta)
{
	//CAssetManager::getInstanceRender()->DrawFlatSprite(tSC->m_nControlsBG,255.0f,128.0f,1.0f,1.0f);

	if(m_bExit == false)
	{
		if(lerp < 1.2f && m_bDone == false)
		{
			lerp += (float)_fDelta * 3;
		}
		else if(lerp > 1.0f)
		{
			lerp -= (float)_fDelta * 3;
			m_bDone = true;
		}
		else
			lerp = 1.0f;


		D3DXVec3Lerp(&temp, &start, &end, lerp);
	}
	else
	{
		if(lerp < 1.0f)
		{
			lerp += (float)_fDelta * 3;
		}
		else
		{
			lerp = 1.0f;

			volatile tPlaySoundEnum* tSOUND = new tPlaySoundEnum(EM_PLAYSOUNDENUM, RJ_Config::RJ_UI_SELECT,true); 
			CEventManager::GetInstanceThreaded()->SendEventRenderLocal(ET_PLAYSOUND,(tBaseEventData*)tSOUND);
			tSOUND = NULL;

			volatile tCameraPop* tPop = new tCameraPop(EM_CAMERAPOP);
			CEventManager::GetInstanceThreaded()->SendEventRenderCrossToUpdate(ET_CAMERAPOP, (tBaseEventData*)tPop);
			tPop = NULL;

			RJ_MAIN_GAME->PopState();
		}

		D3DXVec3Lerp(&temp, &end, &start, lerp);
	}
	D3DXMATRIX matrix;
	//D3DXMatrixTranslation(&matrix, 8.30758f, 6.94687f + 8.56f, -32.4414f);
	D3DXMatrixTranslation(&matrix, 8.65756f, temp.y, -35.4149f);
	RJ_RENDERER->getDirect3DDevice()->SetTransform(D3DTS_WORLD, &matrix);

	CAssetManager::getInstanceRender()->GetMesh( "LvMM_SM_Toast" )->setTextureID(texID);
	CAssetManager::getInstanceRender()->GetMesh( "LvMM_SM_Toast" )->Render(_fDelta);

	if(m_bDone && !m_bExit)
	{
		D3DXMatrixTranslation(&matrix, 10.35f, 4.3f + 8.56f, -35.0f);
		RJ_RENDERER->getDirect3DDevice()->SetTransform(D3DTS_WORLD, &matrix);
		CAssetManager::getInstanceRender()->GetMesh("SM_Sarrow")->Render(_fDelta);
	}
	return true;
}

bool State_3DControls::RenderUnder(double _fDelta)
{
	return true;
}