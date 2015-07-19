#ifndef POWERUP_H_
#define POWERUP_H_
//#include "../../CoreTech/RedJaguarEngine/Base Classes/IVolumeObject.h"
#include "../../CoreTech/RedJaguarEngine/Collision/SolidSphere.h"
class CStone;
class CPlayer;

enum POWERUP_TYPE { CHERRIES = 0, TURKEY, GUM, WHIP_CREAM, FORTUNE_COOKIE, DOUBLE_GUMMY, PEANUT, BUTTER, MAX_POW_TYPES};

class CPowerUp : public CSolidSphere
{
	POWERUP_TYPE m_ePOWType;
	CStone* Stone;
	CPlayer* Player;
	bool m_bFC;
	double m_dTime;

public:
	CPowerUp(void);
	CPowerUp(char* szName, D3DXVECTOR3 vPosition, POWERUP_TYPE ePOWType, float fRadius, float fSpringValue,
		MATERIAL_TYPE eMatType, unsigned int uiID, bool bActive = true);
	~CPowerUp(void);

	POWERUP_TYPE GetPOWType(void) { return m_ePOWType; }
	CStone* GetStone(void) { return Stone; }
	
	void SetStone(CStone* _stone) { Stone = _stone; }
	void SetPlayer(CPlayer* _player) { Player = _player; }
	void SetPOWType(POWERUP_TYPE eType) { m_ePOWType = eType; }

	bool HandlePowerUp(double fDelta, bool currStone);
	bool HandleCherryShot(double fDelta, CPlayer* _player);
};
#endif