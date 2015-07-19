#ifndef REDJAGUAR_JS_STATE_PLAYERMENU_H_
#define REDJAGUAR_JS_STATE_PLAYERMENU_H_

#include "RJ_IGameState.h"
#include "..\\Text\\CBitmapFont.h"
#include "../Event/EventDefinitions.h"

struct tStateConfig;

class State_PlayerMenu : public RJ_IGameState
{
private:
	enum EMenu { eMIN = 0, ePLAYONE = eMIN, ePLAYTWO, eBACK, ePLAYTHREE, ePLAYFOUR, eMAX = ePLAYFOUR};
	int m_nSelection;

	CBitmapFont Font;
	tStateConfig* tSC;

	int NumPlayers;
	int texID;

public:
	State_PlayerMenu(void);
	State_PlayerMenu(const State_PlayerMenu& ref);
	State_PlayerMenu& operator=(const State_PlayerMenu& ref);
	virtual ~State_PlayerMenu(void) {}

	virtual void OnPush(void);
	virtual void OnEnter(void);
	virtual void OnExit(void);

	virtual bool Input(double _fDelta);
	virtual bool InputUnder(double _fDelta);
	virtual bool Update(double _fDelta);
	virtual bool UpdateUnder(double _fDelta);
	virtual bool Render(double _fDelta);
	virtual bool RenderUnder(double _fDelta);

	void SendPlayerMenuEvent(void);
};

#endif