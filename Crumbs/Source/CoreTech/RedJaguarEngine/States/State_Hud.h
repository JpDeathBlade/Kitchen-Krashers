#ifndef STATE_HUD_H_
#define STATE_HUD_H_

#include "../Event/IListener.h"
#include "RJ_IGameState.h"
#include "../Text/CBitmapFont.h"
#include <vector>

struct tStateConfig;
class CAssetManager;

struct tNewPoint
{
	float	fTime;
	int		nPoints;
};
struct tBrowniePoint
{
	char* szName;
	int	nValue;
};

class State_HUD : public RJ_IGameState, public IListener
{
private:
	char	m_cCherries;
	int		m_nNumPlayers;
	int		m_nCurrentPlayer;
	int		m_nCurrPlayerMultiplier;
	int		m_nCurrStoneScore;
	int		m_nCurrHazard;
	int		m_nPowerup;
	float	m_fSpeed;
	float	m_fPrevSpeed;
	unsigned int m_nPar;

	// for printing
	int		m_nDigits[4];
	int		m_nStoneDigits;

	// previous scores
	int		m_nPreviousScore[4];

	// animations
	int	m_nCookieFrame;
	bool m_bCookie;
	bool m_bNewFC;
	float m_fCookieTimer;
	float m_fFrameTimer;
	// Glow
	bool m_bGlow;
	float m_fGlowTimer;

	// fortune spin
	float m_fCurrX;
	float m_fCurrY;
	bool m_bDoneSpinning;
	bool m_bDrawPOW;

	// bottle FX
	float m_fSpinTimer;
	bool m_bFlipBottle;
	float m_fFillTimer;
	int m_nCurrentFillFrame;
	int m_nNextFillFrame;
	bool m_bSlowHazard;
	bool m_bVolSpeedChanged;
	float m_fSpeedChange;

	// multiplier
	bool m_bNoOverlay;
	int m_nPrevMultiplier;
	float m_fMultiplierTimer;
	int m_nValue;
	float m_fScale;
	bool m_bStopped;
	int m_nMultFrame;
	int m_nOffsetArray[2];

	// special shots
	float m_fBrownieTimer;
	std::vector<tBrowniePoint> m_vSpecialShots;

	// Notifications
	bool m_bIsCherryShot;
	float m_fCherryTimer;
	bool m_bIsFoulShot;
	int m_nFoulImageID;
	int m_nPressUpID;
	bool m_bDrawPressUp;
	bool m_bDrawGum;
	bool m_bDrawTurkey;

	// splashes
	int m_nSplashID[3];
	float m_fSplashDrip;
	float m_fSplashTimer;
	bool m_bSplash;
	bool m_bFadeSplash;

	// for "+5"
	std::vector<tNewPoint> m_vNewPoints[4];

	int		m_nPlayerOrder[3];
	int		m_nRemainingShots[4];
	int		m_nMultiplier[4];
	int		m_nScore[4];

	int m_vColors[5][3]; // 1-4 == players; 5 == Hazard

	CBitmapFont Font;
	CBitmapFont tDakotaFont;
	CBitmapFont	tSpecialFont;
	tStateConfig* tSC;
	CAssetManager* pAM;

public:
	State_HUD(void);
	State_HUD(const State_HUD& _ref);
	State_HUD& operator= (const State_HUD& _ref);
	virtual ~State_HUD(void) { }

	void HandleEvent(CEvent* pEvent);

	void OnPush(void);
	void OnEnter(void);
	void OnExit(void);
	void OnPop(void);

	bool Input(double _fDelta);
	bool InputUnder(double _fDelta);
	bool Update(double _fDelta);
	bool UpdateUnder(double _fDelta);
	bool Render(double _fDelta);
	bool RenderUnder(double _fDelta);
};
#endif