#ifndef STATE_CONFIG_H_
#define STATE_CONFIG_H_

class CAssetManager;

struct  tStateConfig
{
	//State Values
	int	m_nNumberofPlayers;
	int m_nLevelSelect;
	bool m_bIsLoading;

	//Image IDS
	int m_nMultiplierSprites;
	int m_nOptionsMenuID;
	int m_nStonesID;
	int m_nBottleID;
	int m_nGameOverBuildSetID;
	int m_nHighScoreID;
	int m_nMainMenuBG;
	int m_nPauseBG;
	int m_nTurnOverBG;
	int m_nPlayerMenuBG;
	int m_nLevelMenuBG;
	int m_nLevel1;
	int m_nLevel2;
	int m_nLevel3;
	int m_nLevelBack;
	int m_nControlsBG;
	int m_nCreditsBG;
	int m_nSliderID;
	int m_n3DSliderID;
	int m_nLoading;
	int m_nCurrPlayer;
	int m_nGummy;
	int m_nWormGum;
	int m_nTutorialIDs[11];
	int m_nCherryShotID;

	//Class Pointers
	CAssetManager* pAM;

	static tStateConfig* GetInstanceRender();

	tStateConfig();
	void LoadStateImages(void);
	void UnloadStateImages(void);
};

#endif