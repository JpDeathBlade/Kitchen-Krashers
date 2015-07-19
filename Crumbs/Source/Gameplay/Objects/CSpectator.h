#ifndef CSPECTATOR_H_
#define CSPECTATOR_H_
#include <deque>
#include "../../CoreTech/RedJaguarEngine/Base Classes/IObject.h"
using std::deque;

struct CheerAnt 
{
	D3DXMATRIX mLoc;
	int nColor;
};

class CSpectator
{
public:
	deque<CheerAnt> m_dCheerDeck;
	int GlobalID;
	bool m_bSitting;
	int m_nCheerAnimID;
	CSpectator(int nGlobalID);
	~CSpectator();
	void Initialize(int nCheerAnimID);
	void Shutdown();
	void Reset(void);
	void Render(double fDelta);
	void Update(double fDelta);
	void AddAntCheer(D3DXMATRIX mLoc);
	void ClearAntCheers();
};

#endif