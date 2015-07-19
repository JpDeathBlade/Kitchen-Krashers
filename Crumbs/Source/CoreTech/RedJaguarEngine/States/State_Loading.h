#ifndef REDJAGUAR_JS_STATE_LOADING_H_
#define REDJAGUAR_JS_STATE_LOADING_H_

#include "RJ_IGameState.h"
#include "..\\Text\\CBitmapFont.h"
#include "../Event/EventDefinitions.h"

struct tStateConfig;

class State_Loading : public RJ_IGameState
{
private:
	CBitmapFont Font;
	tStateConfig* tSC;
	D3DXVECTOR2 m_vBanana;
	D3DXVECTOR2 m_vPotato;
	D3DXVECTOR2 m_vPopsicle;
	D3DXVECTOR2 m_vApple;
	D3DXVECTOR2 m_vFish;
	D3DXVECTOR2 m_vAnthony;
	double uberClocked;

public:
	State_Loading(void);
	State_Loading(const State_Loading& ref);
	State_Loading& operator=(const State_Loading& ref);
	virtual ~State_Loading(void) {}

	virtual void OnPush(void);
	virtual void OnEnter(void);
	virtual void OnExit(void);

	virtual bool Input(double _fDelta);
	virtual bool InputUnder(double _fDelta);
	virtual bool Update(double _fDelta);
	virtual bool UpdateUnder(double _fDelta);
	virtual bool Render(double _fDelta);
	virtual bool RenderUnder(double _fDelta);
};

#endif