#ifndef CPLAYER_H_
#define CPLAYER_H_

#include <d3d9.h>
#include <d3dx9.h>
#include <d3dx9effect.h>
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")
#include <vector>

class CStone;
#include "../../CoreTech/RedJaguarEngine/Base Classes/IObject.h"
class CPowerUp;

class CPlayer : public IObject
{
private:
	unsigned int m_uiPlayerNumber;
	unsigned int m_uiTotalScore;
	unsigned int m_uiNumTurnsUsed;
	unsigned int CurrStone;

	float rotation;
	float timer;

	bool m_bEventFC;
	bool m_bCherry;
	bool m_bUsedCherry;
	
	D3DXVECTOR4 m_vColor;
	CStone *m_stones[5];
	std::vector<CPowerUp*> m_vPowerUps;
	CPowerUp* CherryPOW;
	D3DXMATRIX m_mStonePos;
public:
	CPlayer(unsigned int PlayerNum, D3DXVECTOR4 _color);
	~CPlayer();
	void updateScore();
	void UsedTurn(void);
	bool TurnOver(double fDelta);
	void Reset(void);
	void addPowerup(CPowerUp* _powerup, CStone* _stone);
	void handlePowerups(double fDelta);
	bool handleCherry(double fDelta);
	void GetPowerUps();
	void SetCherryPOW(CPowerUp* Cherry) { CherryPOW = Cherry; }

	bool GetUsedCherry(void) { return m_bUsedCherry; }
	void SetUsedCherry(bool val) { m_bUsedCherry = val; }
	bool GetFC(void) { return m_bEventFC; }
	void SetFC(bool _FC) { m_bEventFC = _FC; }
	void SetCurrStone(unsigned int _curr) { CurrStone = _curr; }
	void SetNumTurns(unsigned int _turns) { m_uiNumTurnsUsed = _turns; }
	void setPlayerNumber(unsigned int playerNumber) { m_uiPlayerNumber = playerNumber; }
	unsigned int getPlayerNumber(void) { return m_uiPlayerNumber; }
	unsigned int getScore() { return m_uiTotalScore; }
	unsigned int getCurrStoneNumber() { return CurrStone; }
	void SetScore(unsigned int _score) { m_uiTotalScore = _score; }
	unsigned int GetTurns(void) { return m_uiNumTurnsUsed; }
	CStone* GetCurrStone(void) { return m_stones[CurrStone]; }
	CStone* GetStone(int stone) { return m_stones[stone]; }
	CPowerUp* GetFirstPOW(void) { return (m_vPowerUps.size() ? m_vPowerUps.back() : NULL); }
	bool GetCherry(void) { return m_bCherry; }
	void SetCherry(bool val) { m_bCherry = val; }
	void RemovePOWfromCherryShot();

	
	void Render(double fDelta);
};

#endif