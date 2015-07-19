#ifndef REDJAGUAR_STATE_3DOPTIONS_H_
#define REDJAGUAR_STATE_3DOPTIONS_H_

#include "RJ_IGameState.h"
#include "../Event/EventDefinitions.h"
#include "..\\Text\\CBitmapFont.h"
#include "..\\Text\\C3DFont.h"
#include "../Event/IListener.h"

struct tStateConfig;

class State_3DOptions : public RJ_IGameState, IListener
{
private:
	enum EMenu { eMIN = 0, eFX = eMIN, eMUSIC, eBRIGHTNESS, eCONTROLS, eTUTORIAL, eBACK, eMAX = eBACK};
	int m_nSelection;

	C3DFont Font3D;
	CBitmapFont Font;
	tStateConfig* tSC;
	int texID;
	D3DXVECTOR3 temp, start, end;
	float lerp;

	float m_fFX, m_fMusic, m_fBrightness;
	bool m_bRenderBars, m_bTutorial, m_bDone, m_bExit, m_bExitControls;

public:
	State_3DOptions(void);
	State_3DOptions(const State_3DOptions& ref);
	State_3DOptions& operator=(const State_3DOptions& ref);
	virtual ~State_3DOptions(void) {}

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

	bool LoadSettings();
	bool SaveSettings(bool bReset = false);
};

#endif