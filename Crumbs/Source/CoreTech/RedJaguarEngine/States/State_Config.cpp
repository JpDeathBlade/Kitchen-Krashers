#include "State_Config.h"
#include "../Asset/AssetManager.h"

tStateConfig* tStateConfig::GetInstanceRender()
{
	static tStateConfig instance;
	return &instance;
}
tStateConfig::tStateConfig()
{
	m_nNumberofPlayers = -1;
	m_nLevelSelect = -1;
	m_nStonesID = -1;
	m_nBottleID = -1;
	m_nGameOverBuildSetID= -1;
	m_bIsLoading = false;
	pAM = NULL;
	m_nCurrPlayer = 0;
	m_nGummy = 1;
	m_nWormGum = 1;
}
void tStateConfig::LoadStateImages(void)
{
	if(pAM == NULL)
		pAM = CAssetManager::getInstanceRender();

	m_nMultiplierSprites = pAM->LoadTexture("t_CapEffectSprites.dds");
	m_nOptionsMenuID = pAM->LoadTexture("t_Menu_2DOptions_D.dds");
	m_nStonesID = pAM->LoadTexture("t_BottleCapSpread.dds");
	m_nBottleID = pAM->LoadTexture("t_SpeedBottle.dds");
	m_nGameOverBuildSetID = pAM->LoadTexture("t_EndGameScreen.dds");
	m_nMainMenuBG = pAM->LoadTexture("MainMenuBG.dds");
	m_nPauseBG = pAM->LoadTexture("t_PauseBG.dds");
	m_nTurnOverBG = pAM->LoadTexture("t_TurnOver.dds");
	m_nPlayerMenuBG = pAM->LoadTexture("t_PlayerMenu.dds");
	m_nLevelMenuBG = pAM->LoadTexture("LevelMenuBG.tga");
	m_nLevel1 = pAM->LoadTexture("Selected-Freezer.png");
	m_nLevel2 = pAM->LoadTexture("Selected-Sink.png");
	m_nLevel3 = pAM->LoadTexture("Selected-Stove.png");
	m_nLevelBack = pAM->LoadTexture("Selected-Back.png");
	m_nControlsBG = pAM->LoadTexture("t_Menu_2DControls_D.dds");
	m_nCreditsBG = pAM->LoadTexture("T_Toast_Credits_D.tga");
	m_nSliderID = pAM->LoadTexture("T_Menu_Slider_D.dds");
	m_n3DSliderID = pAM->LoadTexture("T_Menu_Slider_3D_D.dds");
	m_nLoading = pAM->LoadTexture("T_LoadingScreen.dds");
	m_nHighScoreID = pAM->LoadTexture("T_HighScoreScreen.dds");
	m_nCherryShotID = pAM->LoadTexture("t_CherryShotSplash.dds");

	m_nTutorialIDs[0] = pAM->LoadTexture("T_Tutorial_Start.dds");
	m_nTutorialIDs[1] = pAM->LoadTexture("T_Tutorial_PreLaunch.dds");
	m_nTutorialIDs[2] = pAM->LoadTexture("T_Tutorial_Cutter_D.dds");
	m_nTutorialIDs[3] = pAM->LoadTexture("T_Tutorial_FoulShot_D.dds");
	m_nTutorialIDs[4] = pAM->LoadTexture("T_Tutorial_PostLaunch.dds");
	m_nTutorialIDs[5] = pAM->LoadTexture("T_Tutorial_Scoring.dds");
	m_nTutorialIDs[6] = pAM->LoadTexture("T_Tutorial_Scoring_Goal.dds");
	m_nTutorialIDs[7] = pAM->LoadTexture("T_Tutorial_Hazards.dds");
	m_nTutorialIDs[8] = pAM->LoadTexture("T_Tutorial_PowerUps.dds");
	m_nTutorialIDs[9] = pAM->LoadTexture("T_Tutorial_HUD.dds");
	m_nTutorialIDs[10] = pAM->LoadTexture("T_Tutorial_End.dds");
}
void tStateConfig::UnloadStateImages(void)
{
	if(pAM == NULL)
		pAM = CAssetManager::getInstanceRender();
	pAM->RemoveTexture(m_nStonesID);
	pAM->RemoveTexture(m_nBottleID);
	pAM->RemoveTexture(m_nGameOverBuildSetID);
	pAM->RemoveTexture(m_nMainMenuBG);
	pAM->RemoveTexture(m_nPauseBG);
	pAM->RemoveTexture(m_nPlayerMenuBG);
	pAM->RemoveTexture(m_nLevelMenuBG);
	pAM->RemoveTexture(m_nLevel1);
	pAM->RemoveTexture(m_nLevel2);
	pAM->RemoveTexture(m_nLevel3);
	pAM->RemoveTexture(m_nLevelBack);
	pAM->RemoveTexture(m_nControlsBG);
	pAM->RemoveTexture(m_nCreditsBG);
	pAM->RemoveTexture(m_nLoading);
	pAM->RemoveTexture(m_nHighScoreID);

	pAM->RemoveTexture(m_nTutorialIDs[0]);
	pAM->RemoveTexture(m_nTutorialIDs[1]);
	pAM->RemoveTexture(m_nTutorialIDs[2]);
	pAM->RemoveTexture(m_nTutorialIDs[3]);
	pAM->RemoveTexture(m_nTutorialIDs[4]);
	pAM->RemoveTexture(m_nTutorialIDs[5]);
	pAM->RemoveTexture(m_nTutorialIDs[6]);
	pAM->RemoveTexture(m_nTutorialIDs[7]);
	pAM->RemoveTexture(m_nTutorialIDs[8]);
	pAM->RemoveTexture(m_nTutorialIDs[9]);
	pAM->RemoveTexture(m_nTutorialIDs[10]);


}