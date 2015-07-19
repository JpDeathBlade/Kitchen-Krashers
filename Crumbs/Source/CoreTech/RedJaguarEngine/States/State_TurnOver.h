#ifndef STATE_TURNOVER_H_
#define STATE_TURNOVER_H_

#include "RJ_IGameState.h"
#include "../Text/CBitmapFont.h"

struct tStateConfig;
class CAssetManager;

class State_TurnOver : public RJ_IGameState
{
private:
	bool m_bRender;
	int	m_nPlayer;
	int	m_nMultiplier;
	int	m_nBitPoints;
	int	m_nBouncePoints;
	int	m_nBrowniePoints;
	int m_nBounceBitMult;
	int m_nTotal;
	int	m_nDigits[5];
	int m_nPar;

	CBitmapFont Font;
	tStateConfig* tSC;
	CAssetManager* pAM;

public:
	State_TurnOver(void);
	State_TurnOver(const State_TurnOver& _ref);
	State_TurnOver& operator= (const State_TurnOver& _ref);
	virtual ~State_TurnOver(void) { }

	State_TurnOver(int nCurrPlayer, int nBits, int nBounces, int nBrownie, int nMultiplier, int nPar);
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