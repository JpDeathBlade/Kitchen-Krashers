#ifndef  REDJAGUAR_JS_STATE_HIGHSCORES_H_		// Check to see if this .h is defined
#define  REDJAGUAR_JS_STATE_HIGHSCORES_H_		//  if not, define it

#include "RJ_IGameState.h"
#include "..\\Text\\CBitmapFont.h"
#include "../Event/EventDefinitions.h"
#include "State_Config.h"
#include <vector>
#include "../Event/IListener.h"
#include "../../../Gameplay/Gameplay.h"

class CPlayer;
//class CGameplay;

class State_HighScores: public RJ_IGameState, public IListener
{
private:
	int m_nSelection;

	CBitmapFont Font;
	int NumPlayers;
	int Index;
	int m_nCurrLevel;
	int Player;
	char tempName[4];

	int m_vColors[4][3];

	HighScores m_tHighScores[5];

	CGameplay*		 Gameplay;
	tStateConfig*	 Config;
	tPlayerData playDat;

public:
	State_HighScores(void);
	State_HighScores(int _playerNum, int _index, int _currLevel, int _numPlayers);
	State_HighScores(const State_HighScores& _ref);
	State_HighScores& operator= (const State_HighScores& _ref);
	virtual ~State_HighScores(void) { }

	virtual void OnPush(void);
	virtual void OnEnter(void);
	virtual void OnExit(void);

	virtual bool Input(double _fDelta);
	virtual bool InputUnder(double _fDelta);
	virtual bool Update(double _fDelta);
	virtual bool UpdateUnder(double _fDelta);
	virtual bool Render(double _fDelta);
	virtual bool RenderUnder(double _fDelta);

	void HandleEvent(CEvent* pEvent);
	void SendShutdownEvent(void);

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

};

#endif