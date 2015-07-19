#ifndef STATE_PAUSE_H_
#define STATE_PAUSE_H_

#include "RJ_IGameState.h"
#include "../Text/CBitmapFont.h"
#include "../Event/EventDefinitions.h"

struct tStateConfig;


class State_Pause :	public RJ_IGameState
{
private:
	enum EMenu { eMIN = 0, eRESUME = eMIN, eRESTART, eOPTIONS, eMAINMENU,/* eEXIT, */eMAX = eMAINMENU};
	int m_nSelection;

	CBitmapFont Font;
	tStateConfig* tSC;

	D3DXVECTOR3 Atemp, Astart, Aend;
	D3DXVECTOR3 temp, start, end;
	int lerpdown;

public:
	State_Pause(void);
	State_Pause(const State_Pause& _ref);
	State_Pause& operator= (const State_Pause& _ref);
	virtual ~State_Pause(void) { }

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