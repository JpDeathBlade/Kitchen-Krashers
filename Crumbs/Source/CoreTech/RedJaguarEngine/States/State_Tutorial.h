#ifndef STATE_PAUSE_H_
#define STATE_PAUSE_H_

#include "RJ_IGameState.h"
#include "../Text/CBitmapFont.h"
#include "../Event/EventDefinitions.h"

struct tStateConfig;

class State_Tutorial :	public RJ_IGameState
{
private:
	enum EMenu { eMIN = 0, eYES = eMIN, eNONE, eNO, eMAX = eNO};
	enum ETutScreens{ etMIN = 0, etVIEW = etMIN, etPRECONTROLS, etPIZZA, etFOUL, etPOSTCONTROLS, etSCORING, etGOAL, etPOWERUPS, etHAZARDS, etHUD, etAGAIN, etMAX = etAGAIN};
	int m_nSelection;
	int CurrScreen;
	int PrevScreen;
	
	CBitmapFont Font;
	tStateConfig* tSC;

	D3DXVECTOR3 temp, start, end, gone;
	D3DXVECTOR3 Ptemp, Pstart, Pend, Pgone;
	int lerpdown;
	int Plerpdown;

	

public:
	State_Tutorial();
	State_Tutorial(const State_Tutorial& _ref);
	State_Tutorial& operator= (const State_Tutorial& _ref);
	virtual ~State_Tutorial(void) { }

	virtual void OnPush(void);
	virtual void OnEnter(void);
	virtual void OnExit(void);

	virtual bool Input(double _fDelta);
	virtual bool InputUnder(double _fDelta);
	virtual bool Update(double _fDelta);
	virtual bool UpdateUnder(double _fDelta);
	virtual bool Render(double _fDelta);
	virtual bool RenderUnder(double _fDelta);

	void SendPauseEvent(void);
	void SendShutdownEvent(void);
	void SendRestartEvent(void);

};
#endif