#include "State_3DOptions.h"
#include "State_3DControls.h"
#include "../Asset/AssetManager.h"
#include "../Event/EventManager.h"
#include "..\\Input\\CInputManager.h"
#include "..\\..\\..\\RJ_Config.h"
#include "..\\..\\RJ_GameMain.h"
#include "../text/font_defines.h"
#include <fstream>

#include "State_Config.h"
#define SPEED 27.0f

State_3DOptions::State_3DOptions(void) : Font("t_Crumbs_font.dds","Resources/Fonts/Crumbs_font.dat"), Font3D("t_Crumbs_font.dds","Resources/Fonts/Crumbs_font.dat")
{
	m_nSelection = eMIN;
	tSC = NULL; 
	texID = CAssetManager::getInstanceRender()->LoadTexture("T_Toast_Menu_Options.dds");

	start = D3DXVECTOR3(0.0f, 6.94687f, 0.0f);
	end = D3DXVECTOR3(0.0f, 6.94687f + 8.56f, 0.0f);
	temp = start;
	lerp = 0.0f;
	m_bDone = false;
	m_bExit = false;
	m_bExitControls = false;

	CEventManager::GetInstanceThreaded()->RegisterListenerRender(ET_CAMERADONEMOVING,this);
}

bool State_3DOptions::LoadSettings()
{
	// parse file
	std::fstream file;
	file.open("Resources/Data/data.sav", std::ios_base::in | std::ios_base::binary);

	if (file.is_open())
	{
		// load info
		file.read( (char*)&m_fFX, sizeof(float) );
		file.read( (char*)&m_fMusic, sizeof(float) );
		file.read( (char*)&m_fBrightness, sizeof(float) );
		file.read( (char*)&m_bTutorial, sizeof(bool) );

		volatile tVolumeData* tVD = new tVolumeData(EM_ADJUSTVOLUME, 1, m_fFX); 
		CEventManager::GetInstanceThreaded()->SendEventRenderLocal(ET_ADJUSTVOLUME,(tBaseEventData*)tVD);
		tVD = NULL;
		tVD = new tVolumeData(EM_ADJUSTVOLUME, 2, m_fMusic); 
		CEventManager::GetInstanceThreaded()->SendEventRenderLocal(ET_ADJUSTVOLUME,(tBaseEventData*)tVD);
		tVD = NULL;
		volatile tGammaData* tGD = new tGammaData(EM_ADJUSTGAMMA, m_fBrightness); 
		CEventManager::GetInstanceThreaded()->SendEventRenderLocal(ET_ADJUSTGAMMA,(tBaseEventData*)tGD);
		tGD = NULL;

		volatile tToggleTut* tNP = new tToggleTut(EM_TOGGLETUT, m_bTutorial);
		CEventManager::GetInstanceThreaded()->SendEventRenderCrossToUpdate(ET_TOGGLETUT,(tBaseEventData*)tNP);
		tNP = NULL;

		file.close();
		return true;
	}
	
	return false;
}

bool State_3DOptions::SaveSettings(bool bReset)
{
	std::fstream file;
	file.open("Resources/Data/data.sav", std::ios_base::out | std::ios_base::trunc | std::ios_base::binary);

	if (file.is_open())
	{
		if (bReset)
		{
			m_fFX = 50.0f;
			m_fMusic = 50.0f;
			m_fBrightness = 0.0f;
			m_bTutorial = true;
		}

		file.write( (char*)&m_fFX, sizeof(float) );
		file.write( (char*)&m_fMusic, sizeof(float) );
		file.write( (char*)&m_fBrightness, sizeof(float) );
		file.write( (char*)&m_bTutorial, sizeof(bool) );

		file.close();
		return true;
	}

	return false;
}

void State_3DOptions::OnPush(void)
{
	if(tSC == NULL)
		tSC = tStateConfig::GetInstanceRender();

	

	//	load from file
	if ( !LoadSettings() )
		SaveSettings(true);
	m_bRenderBars = false;
}

void State_3DOptions::OnEnter(void)
{
	D3DXMATRIX camera =
		D3DXMATRIX(	-0.870854f, -3.46945e-018f, 0.491543f, 0.0f,
		0.0520882f, 0.994369f, 0.0922831f, 0.0f,
		-0.488776f, 0.105969f, -0.86595f, 0.0f,
		17.4715f, 16.9277f, -15.8677f, 1.0f );

	m_bRenderBars = false;
	temp = start;
	lerp = 0.0f;
	m_bDone = false;
	m_bExit = false;
	m_bExitControls = false;
	CEventManager::GetInstanceThreaded()->RegisterListenerRender(ET_CAMERADONEMOVING,this);


	volatile tCameraData* tCam = new tCameraData(EM_MENUCAMERA, camera);
	CEventManager::GetInstanceThreaded()->SendEventRenderCrossToUpdate(ET_MENUCAMERA, (tBaseEventData*)tCam);
	tCam = NULL;

	volatile tRenderStuff* tRen = new tRenderStuff(EM_RENDERSTUFF, true);
	CEventManager::GetInstanceThreaded()->SendEventRenderCrossToUpdate(ET_RENDERSTUFF, (tBaseEventData*)tRen);
	tRen = NULL;
}

void State_3DOptions::OnExit(void)
{
	volatile tRenderStuff* tCam = new tRenderStuff(EM_RENDERSTUFF, false);
	CEventManager::GetInstanceThreaded()->SendEventRenderCrossToUpdate(ET_RENDERSTUFF, (tBaseEventData*)tCam);
	tCam = NULL;
	CEventManager::GetInstanceThreaded()->UnregisterListenerRender(ET_CAMERADONEMOVING, this);
}

bool State_3DOptions::Input(double _fDelta)
{
	if (m_bRenderBars)
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
		else if ( CInputManager::GetInstanceRender()->getKeysDown() & RJ_Config::AP_LEFT )
		{
			switch(m_nSelection)
			{
			case eFX:
				{
					m_fFX -= (SPEED * (float)_fDelta);
					if (m_fFX <= 0.0f)
						m_fFX = 0.0f;
	
					volatile tVolumeData* tVD = new tVolumeData(EM_ADJUSTVOLUME, 1, m_fFX); 
					CEventManager::GetInstanceThreaded()->SendEventRenderLocal(ET_ADJUSTVOLUME,(tBaseEventData*)tVD);
					tVD = NULL;
					/*volatile tPlaySoundEnum* tSOUND = new tPlaySoundEnum(EM_PLAYSOUNDENUM, RJ_Config::RJ_SWEEP_ANTHONY,true); 
					CEventManager::GetInstanceThreaded()->SendEventRenderLocal(ET_PLAYSOUND,(tBaseEventData*)tSOUND);
					tSOUND = NULL;*/
				} break;
			case eMUSIC:
				{
					m_fMusic -= (SPEED * (float)_fDelta);
					if (m_fMusic <= 0.0f)
						m_fMusic = 0.0f;
	
					volatile tVolumeData* tVD = new tVolumeData(EM_ADJUSTVOLUME, 2, m_fMusic); 
					CEventManager::GetInstanceThreaded()->SendEventRenderLocal(ET_ADJUSTVOLUME,(tBaseEventData*)tVD);
					tVD = NULL;  
				} break;
			case eBRIGHTNESS:
				{
					m_fBrightness -= (0.75f * (float)_fDelta);
					if (m_fBrightness <= -1.0f)
						m_fBrightness = -1.0f;
	
					volatile tGammaData* tGD = new tGammaData(EM_ADJUSTGAMMA, m_fBrightness); 
					CEventManager::GetInstanceThreaded()->SendEventRenderLocal(ET_ADJUSTGAMMA,(tBaseEventData*)tGD);
					tGD = NULL;
				} break;
			case eBACK:
				{
					m_nSelection = eTUTORIAL;
				} break;
			}
		}
		else if ( CInputManager::GetInstanceRender()->getKeysDown() & RJ_Config::AP_RIGHT )
		{
			switch(m_nSelection)
			{
			case eFX:
				{
					m_fFX += (SPEED * (float)_fDelta);
					if (m_fFX >= 100.0f)
						m_fFX = 100.0f;
	
					volatile tVolumeData* tVD = new tVolumeData(EM_ADJUSTVOLUME, 1, m_fFX); 
					CEventManager::GetInstanceThreaded()->SendEventRenderLocal(ET_ADJUSTVOLUME,(tBaseEventData*)tVD);
					tVD = NULL;
					/*volatile tPlaySoundEnum* tSOUND = new tPlaySoundEnum(EM_PLAYSOUNDENUM, RJ_Config::RJ_SWEEP_ANTHONY,true); 
					CEventManager::GetInstanceThreaded()->SendEventRenderLocal(ET_PLAYSOUND,(tBaseEventData*)tSOUND);
					tSOUND = NULL;*/
				} break;
			case eMUSIC:
				{
					m_fMusic += (SPEED * (float)_fDelta);
					if (m_fMusic >= 100.0f)
						m_fMusic = 100.0f;
	
					volatile tVolumeData* tVD = new tVolumeData(EM_ADJUSTVOLUME, 2, m_fMusic); 
					CEventManager::GetInstanceThreaded()->SendEventRenderLocal(ET_ADJUSTVOLUME,(tBaseEventData*)tVD);
					tVD = NULL;
				} break;
			case eBRIGHTNESS:
				{
					m_fBrightness += (0.75f * (float)_fDelta);
					if (m_fBrightness >= 1.0f)
						m_fBrightness = 1.0f;
	
					volatile tGammaData* tGD = new tGammaData(EM_ADJUSTGAMMA, m_fBrightness); 
					CEventManager::GetInstanceThreaded()->SendEventRenderLocal(ET_ADJUSTGAMMA,(tBaseEventData*)tGD);
					tGD = NULL;
				} break;
			case eCONTROLS:
				{
					m_nSelection = eBACK;
				} break;
			case eTUTORIAL:
				{
					m_nSelection = eBACK;
				} break;
			}
		}
		else if (CInputManager::GetInstanceRender()->getKeysPressed() & RJ_Config::AP_CONFIRM )
		{
			volatile tPlaySoundEnum* tSOUND = new tPlaySoundEnum(EM_PLAYSOUNDENUM, RJ_Config::RJ_UI_SELECT,true); 
			CEventManager::GetInstanceThreaded()->SendEventRenderLocal(ET_PLAYSOUND,(tBaseEventData*)tSOUND);
			tSOUND = NULL;
			switch(m_nSelection)
			{
			case eCONTROLS:
				{
					m_bExitControls = true;
					temp = end;
					lerp = 0;
					m_bRenderBars = false;
				}
				break;
			case eBACK:
				{
					volatile tPlaySoundEnum* tCTSOUND = new tPlaySoundEnum(EM_PLAYSOUNDENUM, RJ_Config::RJ_GUI_TOASTDOWN,true); 
					CEventManager::GetInstanceThreaded()->SendEventRenderLocal(ET_PLAYSOUND,(tBaseEventData*)tCTSOUND);
					tCTSOUND = NULL;
					m_bExit = true;
					temp = end;
					lerp = 0;
					m_bRenderBars = false;
				} break;
			case eTUTORIAL:
				{
					if(m_bTutorial)
						m_bTutorial = false;
					else
						m_bTutorial = true;
				}
			}
		}
	}
	if (CInputManager::GetInstanceRender()->getKeysPressed() & RJ_Config::AP_CANCEL)
	{
		m_bExit = true;
		temp = end;
		lerp = 0;
		m_bRenderBars = false;
		volatile tPlaySoundEnum* tCTSOUND = new tPlaySoundEnum(EM_PLAYSOUNDENUM, RJ_Config::RJ_GUI_TOASTDOWN,true); 
		CEventManager::GetInstanceThreaded()->SendEventRenderLocal(ET_PLAYSOUND,(tBaseEventData*)tCTSOUND);
		tCTSOUND = NULL;
	}

	return true;
}

bool State_3DOptions::InputUnder(double _fDelta){ return true; }
bool State_3DOptions::Update(double _fDelta){ return true; }
bool State_3DOptions::UpdateUnder(double _fDelta){ return true; }
bool State_3DOptions::RenderUnder(double _fDelta){	return true; }

bool State_3DOptions::Render(double _fDelta)
{
	if(m_bExit == false && m_bExitControls == false)
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
			if(m_bExit)
			{
				volatile tPlaySoundEnum* tSOUND = new tPlaySoundEnum(EM_PLAYSOUNDENUM, RJ_Config::RJ_UI_SELECT,true); 
				CEventManager::GetInstanceThreaded()->SendEventRenderLocal(ET_PLAYSOUND,(tBaseEventData*)tSOUND);
				tSOUND = NULL;
				volatile tCameraPop* tPop = new tCameraPop(EM_CAMERAPOP);
				CEventManager::GetInstanceThreaded()->SendEventRenderCrossToUpdate(ET_CAMERAPOP, (tBaseEventData*)tPop);
				tPop = NULL;

				SaveSettings();

				RJ_MAIN_GAME->PopState();
			}
			else if(m_bExitControls)
			{
				RJ_MAIN_GAME->PushState(new State_3DControls());
			}
		}


		D3DXVec3Lerp(&temp, &end, &start, lerp);
	}
	
	D3DXMATRIX matrix, matrixX;
	D3DXMatrixTranslation(&matrix, 8.30758f, temp.y, -32.4414f);
	RJ_RENDERER->getDirect3DDevice()->SetTransform(D3DTS_WORLD, &matrix);

	CAssetManager::getInstanceRender()->GetMesh( "LvMM_SM_Toast" )->setTextureID(texID);
	CAssetManager::getInstanceRender()->GetMesh( "LvMM_SM_Toast" )->Render(_fDelta);
	
	if (m_bRenderBars)
	{
		switch(m_nSelection)
		{
		case eFX:
			{
				D3DXMatrixTranslation(&matrix, 4.6f, 9.15f + 8.56f, -32.0f);
			} break;
		case eMUSIC:
			{
				D3DXMatrixTranslation(&matrix, 4.6f, 8.0f + 8.56f, -32.0f);
			} break;
		case eBRIGHTNESS:
			{
				D3DXMatrixTranslation(&matrix, 4.6f, 7.0f + 8.56f, -32.0f);
			} break;
		case eCONTROLS:
			{
				D3DXMatrixTranslation(&matrix, 4.6f, 6.15f + 8.56f, -32.0f);
			} break;
		case eTUTORIAL:
			{
				D3DXMatrixTranslation(&matrix, 4.6f, 5.15f + 8.56f, -32.0f);
			} break;
		case eBACK:
			{
				D3DXMatrixTranslation(&matrix, 10.5f, 5.1f + 8.56f, -32.0f);
			} break;
		}
		D3DXMatrixTranslation(&matrixX, 7.8f, 5.37f + 8.56f, -32.0f);
		D3DXMATRIX matrixXRot;
		D3DXMatrixRotationY(&matrixXRot, D3DX_PI);

		D3DXMatrixMultiply(&matrixX, &matrixXRot, &matrixX);

		RJ_RENDERER->getDirect3DDevice()->SetTransform(D3DTS_WORLD, &matrix);
		CAssetManager::getInstanceRender()->GetMesh("SM_Sarrow")->Render(_fDelta);

		// render sliders
		RECT rSlider = {0,0,256,128};
		DWORD color = D3DCOLOR_ARGB(128,255,255,255);
		// draw totals
		CAssetManager::getInstanceRender()->DrawFlatSprite(tSC->m_n3DSliderID, 519.0f, 452.0f, 0.65f, 0.65f, &rSlider,
			1.0f, 1.0f, 0.0f, color);												 
		CAssetManager::getInstanceRender()->DrawFlatSprite(tSC->m_n3DSliderID, 519.0f, 505.0f, 0.65f, 0.65f, &rSlider,
			1.0f, 1.0f, 0.0f, color);												 
		CAssetManager::getInstanceRender()->DrawFlatSprite(tSC->m_n3DSliderID, 519.0f, 547.0f, 0.65f, 0.65f, &rSlider,
			1.0f, 1.0f, 0.0f, color);
		// draw percentages
		rSlider.right = (long)( (m_fFX / 100.0f) * 256.0f ); 
		CAssetManager::getInstanceRender()->DrawFlatSprite(tSC->m_n3DSliderID, 519.0f, 452.0f, 0.65f, 0.65f, &rSlider);
		rSlider.right = (long)( (m_fMusic / 100.0f) * 256.0f ); 
		CAssetManager::getInstanceRender()->DrawFlatSprite(tSC->m_n3DSliderID, 519.0f, 505.0f, 0.65f, 0.65f, &rSlider);
		rSlider.right = (long)( ((m_fBrightness + 1.0f) / 2.0f) * 256.0f ); 
		CAssetManager::getInstanceRender()->DrawFlatSprite(tSC->m_n3DSliderID, 519.0f, 547.0f, 0.65f, 0.65f, &rSlider);

		if(m_bTutorial)
		{
			char buffer[2];
			buffer[0] = (char)IMG_CHECKBOX;
			buffer[1] = '\0';
			Font3D.DrawString(buffer, &D3DXVECTOR3(0.0f, 0.0f, 0.0f), &matrixX, 0.7f, 0.7f);
		}
	}
	RJ_RENDERER->getDirect3DDevice()->SetTransform(D3DTS_WORLD, &matrixX);


	return true;
}

void State_3DOptions::HandleEvent(CEvent* pEvent)
{
	if(pEvent->GetEventID() == ET_CAMERADONEMOVING)
	{
		if(pEvent->GetData()->GetMsgID() == EM_CAMERADONEMOVING)
		{
			if(pEvent->GetData() != NULL)
			{
				m_bRenderBars = true;
				volatile tRenderStuff* tCam = new tRenderStuff(EM_RENDERSTUFF, false);
				CEventManager::GetInstanceThreaded()->SendEventRenderCrossToUpdate(ET_RENDERSTUFF, (tBaseEventData*)tCam);
				tCam = NULL;
			}
		}
	}

	pEvent->m_nRefs--;
}