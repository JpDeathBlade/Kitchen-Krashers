#include "State_Controls.h"
#include "State_MainMenu.h"
#include "State_Options.h"
#include "../Asset/AssetManager.h"
#include "../Event/EventManager.h"
#include "..\\Input\\CInputManager.h"
//#include "..\\Input\\Cursor.h"
#include "..\\..\\..\\RJ_Config.h"
#include "..\\..\\RJ_GameMain.h"
#include "../Text/font_defines.h"

#include "State_Config.h"
#define SPEED 27.0f

State_Options::State_Options(void) : Font("t_Crumbs_font.dds","Resources/Fonts/Crumbs_font.dat")
{
	m_nSelection = eMIN;
	tSC = NULL; 

	start = D3DXVECTOR3(255.0f, 768.0f, 0.0f);
	end = D3DXVECTOR3(255.0f, 128.0f, 0.0f);
	temp = start;

	Astart = D3DXVECTOR3(306.0f, 768.0f + 173 + 128.0f, 0.0f);
	Aend = D3DXVECTOR3(306.0f, 173 + 128.0f, 0.0f);
	Atemp = Astart;

	lerpdown = 0;
}

bool State_Options::LoadSettings()
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

bool State_Options::SaveSettings(bool bReset)
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

void State_Options::OnPush(void)
{
	if(tSC == NULL)
		tSC = tStateConfig::GetInstanceRender();

	//	load from file
	if ( !LoadSettings() )
		SaveSettings(true);
}

void State_Options::OnEnter(void)
{
	volatile tRenderStuff* tCam = new tRenderStuff(EM_RENDERSTUFF, true);
	CEventManager::GetInstanceThreaded()->SendEventRenderCrossToUpdate(ET_RENDERSTUFF, (tBaseEventData*)tCam);
	tCam = NULL;
}
void State_Options::OnExit(void)
{
	volatile tRenderStuff* tCam = new tRenderStuff(EM_RENDERSTUFF, false);
	CEventManager::GetInstanceThreaded()->SendEventRenderCrossToUpdate(ET_RENDERSTUFF, (tBaseEventData*)tCam);
	tCam = NULL;
}

bool State_Options::Input(double _fDelta)
{
	if (lerpdown == 3)
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
				RJ_MAIN_GAME->PushState(new State_Controls());
				break;
			case eTUTORIAL:
				{
					m_bTutorial = !m_bTutorial;
				} break;
			case eBACK:
				{
					lerpdown = 1;
				} break;
			}
		}
	}
	if (CInputManager::GetInstanceRender()->getKeysPressed() & RJ_Config::AP_CANCEL)
	{
		volatile tPlaySoundEnum* tSOUND = new tPlaySoundEnum(EM_PLAYSOUNDENUM, RJ_Config::RJ_UI_SELECT,true); 
		CEventManager::GetInstanceThreaded()->SendEventRenderLocal(ET_PLAYSOUND,(tBaseEventData*)tSOUND);
		tSOUND = NULL;
		SaveSettings();
		RJ_MAIN_GAME->PopState();
	}

	return true;
}

bool State_Options::InputUnder(double _fDelta){ return true; }
bool State_Options::Update(double _fDelta){ return true; }
bool State_Options::UpdateUnder(double _fDelta){ return true; }

bool State_Options::RenderUnder(double _fDelta)
{ 
	CAssetManager::getInstanceRender()->DrawFlatSprite(tSC->m_nOptionsMenuID,temp.x,temp.y,1.0f,1.0f);

	return true; 
}

bool State_Options::Render(double _fDelta)
{
	float lerp = 2.5f * (float)_fDelta;
	if(lerpdown == 1)
	{
		end.y = 800.0f;
		D3DXVec3Lerp(&temp, &start, &end, lerp);
		start = temp;
		CAssetManager::getInstanceRender()->DrawFlatSprite(tSC->m_nOptionsMenuID,temp.x,temp.y,1.0f,1.0f);
		if(start.y >= 768.0f)
			lerpdown = 2;
	}
	else if(lerpdown == 2)
	{
		SaveSettings();
		RJ_MAIN_GAME->PopState();
	}
	else if(lerpdown == 0)
	{
		D3DXVec3Lerp(&temp, &start, &end, lerp);
		start = temp;
		CAssetManager::getInstanceRender()->DrawFlatSprite(tSC->m_nOptionsMenuID,temp.x,temp.y,1.0f,1.0f);

		if(start.y - end.y <= 1.0f)
			lerpdown = 3;
	}
	else if(lerpdown == 3)
	{
		D3DXVec3Lerp(&temp, &start, &end, lerp);
		start = temp;
		CAssetManager::getInstanceRender()->DrawFlatSprite(tSC->m_nOptionsMenuID,temp.x,temp.y,1.0f,1.0f);

		char Buffer[2];
		Buffer[0] = (char)(IMG_UP);
		Buffer[1] = (char)0;

		switch(m_nSelection)
		{
		case eFX:
			{
				Font.DrawString(Buffer,315.0f,140 + 128.0f,0.75f, 0.75f, 1.5f);
			} break;
		case eMUSIC:
			{
				Font.DrawString(Buffer, 315.0f,192 + 128.0f, 0.75f, 0.75f, 1.5f);
			} break;
		case eBRIGHTNESS:
			{
				Font.DrawString(Buffer,315.0f,238 + 128.0f, 0.75f, 0.75f, 1.5f);
			} break;
		case eCONTROLS:
			{
				Font.DrawString(Buffer, 315.0f,290 + 128.0f, 0.75f, 0.75f, 1.5f);
			} break;
		case eTUTORIAL:
			{
				Font.DrawString(Buffer, 315.0f,342 + 128.0f, 0.75f, 0.75f, 1.5f);
			} break;
		case eBACK:
			{
				Font.DrawString(Buffer,610.0f,342 + 128.0f, 0.75f, 0.75f, 1.5f);
			} break;
		}

		// render sliders
		RECT rSlider = {0,0,256,128};
		DWORD color = D3DCOLOR_ARGB(128,255,255,255);
		// draw totals
		CAssetManager::getInstanceRender()->DrawFlatSprite(tSC->m_nSliderID, 519.0f, 140 + 128.0f, 0.5f, 0.5f, &rSlider,
			1.0f, 1.0f, 0.0f, color);
		CAssetManager::getInstanceRender()->DrawFlatSprite(tSC->m_nSliderID, 519.0f, 192 + 128.0f, 0.5f, 0.5f, &rSlider,
			1.0f, 1.0f, 0.0f, color);
		CAssetManager::getInstanceRender()->DrawFlatSprite(tSC->m_nSliderID, 519.0f, 238 + 128.0f, 0.5f, 0.5f, &rSlider,
			1.0f, 1.0f, 0.0f, color);
		// draw percentages
		rSlider.right = (long)( (m_fFX / 100.0f) * 256.0f ); 
		CAssetManager::getInstanceRender()->DrawFlatSprite(tSC->m_nSliderID, 519.0f, 140 + 128.0f, 0.5f, 0.5f, &rSlider);
		rSlider.right = (long)( (m_fMusic / 100.0f) * 256.0f ); 
		CAssetManager::getInstanceRender()->DrawFlatSprite(tSC->m_nSliderID, 519.0f, 192 + 128.0f, 0.5f, 0.5f, &rSlider);
		rSlider.right = (long)( ((m_fBrightness + 1.0f) / 2.0f) * 256.0f ); 
		CAssetManager::getInstanceRender()->DrawFlatSprite(tSC->m_nSliderID, 519.0f, 238 + 128.0f, 0.5f, 0.5f, &rSlider);

		if(m_bTutorial)
		{
			char buffer[2];
			buffer[0] = (char)IMG_CHECKBOX;
			buffer[1] = '\0';
			Font.DrawString(buffer, 441.0f, 347 + 128.0f, 0.5f, 0.5f);
		}
	}
	return true;
}