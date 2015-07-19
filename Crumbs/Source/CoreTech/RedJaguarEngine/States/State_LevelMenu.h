#ifndef REDJAGUAR_JS_STATE_LEVELMENU_H_
#define REDJAGUAR_JS_STATE_LEVELMENU_H_

#include "RJ_IGameState.h"
#include "..\\Text\\C3DFont.h"
#include "../Event/EventDefinitions.h"
#include "../Event/IListener.h"
#include "../../../Gameplay/Gameplay.h"

struct tStateConfig;

class State_LevelMenu : public RJ_IGameState, IListener
{
private:
	enum EMenu { eMIN = 0, eLEVELONE = eMIN, eLEVELTWO, eLEVELTHREE, eBACK, eMAX = eBACK};
	int m_nSelection;

	C3DFont Font;
	tStateConfig* tSC;
	HighScores m_tHighScores[5];

	int NumPlayers;
	int LevelSelect;
	int texBack;
	int texFreezer;
	int texSink;
	int texStove;
	bool renderSelection;
	bool m_bTutorial;

public:
	State_LevelMenu(void);
	State_LevelMenu(int _numPlayers);
	State_LevelMenu(const State_LevelMenu& ref);
	State_LevelMenu& operator=(const State_LevelMenu& ref);
	virtual ~State_LevelMenu(void) {}

	virtual void OnPush(void);
	virtual void OnEnter(void);
	virtual void OnExit(void);

	virtual void HandleEvent(CEvent* pEvent);

	virtual bool Input(double _fDelta);
	virtual bool InputUnder(double _fDelta);
	virtual bool Update(double _fDelta);
	virtual bool UpdateUnder(double _fDelta);
	virtual bool Render(double _fDelta);
	virtual bool RenderUnder(double _fDelta);

	//////////////////////////////////////////////////////////////////////////
	//Load/Save High Scores
	//Single Player
	////Level 1
	bool LoadHighScores1(void);
	bool SaveHighScores1(void);
	////Level 2
	bool LoadHighScores2(void);
	bool SaveHighScores2(void);
	////Level 3
	bool LoadHighScores3(void);
	bool SaveHighScores3(void);
	//////////////////////////////////////////////////////////////////////////
	//Multiplayer
	////Level 1
	bool LoadHighScores1M(void);
	bool SaveHighScores1M(void);
	////Level 2
	bool LoadHighScores2M(void);
	bool SaveHighScores2M(void);
	////Level 3
	bool LoadHighScores3M(void);
	bool SaveHighScores3M(void);
	//////////////////////////////////////////////////////////////////////////

	void LoadSettings();

	void SendLevelMenuEvent(void);
};

#endif