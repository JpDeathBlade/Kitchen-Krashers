#ifndef STATE_CAMFLY_H_
#define STATE_CAMFLY_H_

#include "RJ_IGameState.h"
#include "../Text/CBitmapFont.h"

struct tStateConfig;
class CAssetManager;

class State_CamFly : public RJ_IGameState
{
private:
	CBitmapFont Font;
	tStateConfig* tSC;
	CAssetManager* pAM;

	double dTime;
	float fAlpha;
	bool bLeaving;
	bool bDone;

public:
	State_CamFly(void);
	State_CamFly(const State_CamFly& _ref);
	State_CamFly& operator= (const State_CamFly& _ref);
	virtual ~State_CamFly(void) { }

	bool Render(double _fDelta);
	void OnPush(void);

	void OnEnter(void){}
	void OnExit(void){}
	void OnPop(void){delete this;}

	bool Input(double _fDelta){ return true; }
	bool InputUnder(double _fDelta){ return true; }
	bool Update(double _fDelta){ return true; }
	bool UpdateUnder(double _fDelta){ return true; }
	bool RenderUnder(double _fDelta){ return true; }
};
#endif