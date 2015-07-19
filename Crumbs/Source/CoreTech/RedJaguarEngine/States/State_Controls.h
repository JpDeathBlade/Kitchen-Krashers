#ifndef REDJAGUAR_JS_STATE_Controls_H_
#define REDJAGUAR_JS_STATE_Controls_H_

#include "RJ_IGameState.h"
#include "..\\Text\\CBitmapFont.h"
#include "../Event/EventDefinitions.h"

struct tStateConfig;

class State_Controls : public RJ_IGameState
{
private:
	enum EMenu { eMIN = 0, eBACK = eMIN, eMAX = eBACK};
	int m_nSelection;

	CBitmapFont Font;
	tStateConfig* tSC;

	D3DXVECTOR3 temp, start, end;
	D3DXVECTOR3 Atemp, Astart, Aend;

	int NumPlayers;
	int lerpdown;

public:
	State_Controls(void);
	State_Controls(const State_Controls& ref);
	State_Controls& operator=(const State_Controls& ref);
	virtual ~State_Controls(void) {}

	virtual void OnPush(void);
	virtual void OnEnter(void);
	virtual void OnExit(void);

	virtual bool Input(double _fDelta);
	virtual bool InputUnder(double _fDelta);
	virtual bool Update(double _fDelta);
	virtual bool UpdateUnder(double _fDelta);
	virtual bool Render(double _fDelta);
	virtual bool RenderUnder(double _fDelta);

	//void SendControlsEvent(void);
};

#endif