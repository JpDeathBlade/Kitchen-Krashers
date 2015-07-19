#ifndef REDJAGUAR_STATE_OPTIONS_H_
#define REDJAGUAR_STATE_OPTIONS_H_

#include "RJ_IGameState.h"
#include "..\\Text\\CBitmapFont.h"
#include "../Event/EventDefinitions.h"

struct tStateConfig;

class State_Options : public RJ_IGameState
{
private:
	enum EMenu { eMIN = 0, eFX = eMIN, eMUSIC, eBRIGHTNESS, eCONTROLS, eTUTORIAL, eBACK, eMAX = eBACK};
	int m_nSelection;

	CBitmapFont Font;
	tStateConfig* tSC;

	D3DXVECTOR3 Atemp, Astart, Aend;
	D3DXVECTOR3 temp, start, end;
	int lerpdown;

	float m_fFX, m_fMusic, m_fBrightness;
	bool m_bTutorial;

public:
	State_Options(void);
	State_Options(const State_Options& ref);
	State_Options& operator=(const State_Options& ref);
	virtual ~State_Options(void) {}

	virtual void OnPush(void);
	virtual void OnEnter(void);
	virtual void OnExit(void);

	virtual bool Input(double _fDelta);
	virtual bool InputUnder(double _fDelta);
	virtual bool Update(double _fDelta);
	virtual bool UpdateUnder(double _fDelta);
	virtual bool Render(double _fDelta);
	virtual bool RenderUnder(double _fDelta);

	bool LoadSettings();
	bool SaveSettings(bool bReset = false);
};

#endif