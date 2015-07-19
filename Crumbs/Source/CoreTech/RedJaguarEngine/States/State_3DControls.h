#ifndef REDJAGUAR_JS_STATE_3DControls_H_
#define REDJAGUAR_JS_STATE_3DControls_H_

#include "RJ_IGameState.h"
#include "..\\Text\\CBitmapFont.h"
#include "../Event/EventDefinitions.h"

struct tStateConfig;

class State_3DControls : public RJ_IGameState
{
private:
	enum EMenu { eMIN = 0, eBACK = eMIN, eMAX = eBACK};
	int m_nSelection;
	int texID;

	D3DXVECTOR3 temp, start, end;
	float lerp;

	tStateConfig* tSC;

	bool m_bDone, m_bExit;

public:
	State_3DControls(void);
	State_3DControls(const State_3DControls& ref);
	State_3DControls& operator=(const State_3DControls& ref);
	virtual ~State_3DControls(void) {}

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