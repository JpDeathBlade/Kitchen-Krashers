#ifndef REDJAGUAR_JS_STATE_Credits_H_
#define REDJAGUAR_JS_STATE_Credits_H_

#include "RJ_IGameState.h"
#include "..\\Text\\CBitmapFont.h"
#include "../Event/EventDefinitions.h"

struct tStateConfig;

class State_Credits : public RJ_IGameState
{
private:
	enum EMenu { eMIN = 0, eBACK = eMIN, eMAX = eBACK};
	int m_nSelection;

	CBitmapFont Font;
	tStateConfig* tSC;

	int NumPlayers;
	int texID1, texID2, texID3, texID4;
	int m_nPressESC;

	D3DXVECTOR3 temp, start, end;
	float timer;
	bool toast1, page2, page3, page4, m_bExit, m_bDone, m_bSwitch;

public:
	State_Credits(void);
	State_Credits(const State_Credits& ref);
	State_Credits& operator=(const State_Credits& ref);
	virtual ~State_Credits(void) {}

	virtual void OnPush(void);
	virtual void OnEnter(void);
	virtual void OnExit(void);

	virtual bool Input(double _fDelta);
	virtual bool InputUnder(double _fDelta);
	virtual bool Update(double _fDelta);
	virtual bool UpdateUnder(double _fDelta);
	virtual bool Render(double _fDelta);
	virtual bool RenderUnder(double _fDelta);

	//void SendCreditsEvent(void);
};

#endif