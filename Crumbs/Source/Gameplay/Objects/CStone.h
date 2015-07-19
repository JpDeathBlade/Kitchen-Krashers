#ifndef CSTONE_H_
#define CSTONE_H_

#include <d3d9.h>
#include <d3dx9.h>
#include <d3dx9effect.h>
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")

#include "../../CoreTech/RedJaguarEngine/Collision/SolidSphere.h"
#include "../../CoreTech/RedJaguarEngine/Effects/Effect.h"
#include "Mesh.h"
class CGameplay;
class CPowerUp;
class CPlayer;

enum eHazardType {NONE = -1, WATER = 0, OIL, SODA, ICE, STICKY, STICKYP};
enum eVolumeSpeed {MINUS = -1, EVEN, PLUS};

class CStone : public IObject
{
private:
	CGameplay*		 Gameplay;
	CSolidSphere m_tVolume;
	CPlayer* m_pOwner;

	D3DXMATRIX m_mPosition;
	D3DXMATRIX m_mPrevPosition;
	D3DXMATRIX m_mGoodPrev;
	D3DXMATRIX m_mAngle;

	D3DXMATRIX m_mStartPos;

	D3DXVECTOR4 m_vColor;
	D3DXVECTOR3 directionVec;
	D3DXVECTOR3 goodDirection;
	D3DXVECTOR3 directionVecNorm;
	D3DXVECTOR3 goodDirectionNorm;
	D3DXVECTOR3 m_position;
	D3DXVECTOR3 m_velocity;
	D3DXVECTOR3 m_tFrictionVals;
	D3DXVECTOR3 m_vForward;
	D3DXVECTOR3 m_vOriginalForward;
	D3DXVECTOR3 m_vCollisionPoint1;
	D3DXVECTOR3 m_vCollisionPoint2;
	std::vector<CPowerUp*> m_vPowerUps;
	int POWHat;
	D3DXVECTOR3 prevNorm;

	Effect		m_effectWake;
	Effect		m_effectWakeCycle;
	Mesh		m_meshWave;
	int			ID_waterSpill;
	int			ID_orangeSpill;
	int			ID_oilSpill;
	int			ID_purpleSpill;
	int			ID_redSpill;
	double      m_EffectTime;

	char m_szCapName[16];

	float m_fCurrTime;
	float m_fSpeed;
	float m_fPower;
	float m_fAngle;
	float m_fInitAngle;
	float m_fMass;
	eHazardType m_eCurrHazard;
	int m_eCurrVolSpeed;

	int m_iBitPoints;
	int m_iWallPoints;
	int m_iBrowniePoints;
	int m_iGoalMultiplier;
	int m_iMultipilerForHUD;
	int m_nGumCollisions;
	int m_nNumPowerUps;
	int m_nNumWallHits;
	int m_nNumStoneHits;

	bool m_bIsMoving;
	bool m_bSpeedCap;
	bool m_bCanStop;
	bool m_bIsColliding;
	bool m_bCamUnattach;
	bool m_bHasCollided;
	bool m_bIsLaunched;
	bool m_bGum;
	bool m_bDoneCurrent;
	bool m_bTurkey;
	bool m_bUsedTurkey;
	bool m_bOnCherryShot;
	bool m_bDonePOW;
	bool m_bRenderAnthony;

	bool m_bIsFoul;

	volatile bool m_bCurrentStone;
	
public:
	CStone(D3DXVECTOR4 _color, CPlayer* pOwner);
	~CStone();

	void launch(float fPower, float fAngle);
	void Update(double fDelta);
	void Render(double _fDelta);
	void react(IVolumeObject* vol, D3DXVECTOR3 normal);
	void Reset(void);
	void AddPowerUp(CPowerUp* _POW);
	void EmptyPowerUps(void);

	//////////////////////////////////////////////////////////////////////////
	//Accessors
	CGameplay* GetGameplay() { return Gameplay; }
	bool GetPOWDone() {return m_bDonePOW;}
	int GetCurrentHazard() { return m_eCurrHazard; }
	int GetCurrentVolumeSpeed() { return m_eCurrVolSpeed; }
	int GetWallPoints() { return m_iWallPoints; }
	int GetBitPoints() { return m_iBitPoints; }
	int GetBrowniePoints() { return m_iBrowniePoints; }
	D3DXMATRIX getPosMat() { return m_mPosition; }
	bool isTransparent() { return false; }
	float getAngle(void) { return m_fAngle; }
	bool getHasCollided(void) { return m_bHasCollided; }
	D3DXVECTOR3 getPosition();
	D3DXVECTOR3 getVelocity() { return m_velocity; }
	D3DXVECTOR3 GetFrictionVals(){ return m_tFrictionVals; }
	D3DXVECTOR3 GetForward() { return m_vForward; }
	D3DXVECTOR3 GetColl1() { return m_vCollisionPoint1; }
	D3DXVECTOR3 GetColl2() { return m_vCollisionPoint2; }
	D3DXMATRIX GetAngle() { return m_mAngle; }
	bool GetCanStop() const { return m_bCanStop; }
	bool GetIsCurrentStone() { return m_bCurrentStone; }
	bool GetDoneCurrent() { return m_bDoneCurrent; }
	bool GetIsLaunched() { return m_bIsLaunched; }
	bool isColliding() {return m_bIsColliding;}
	float getSpeed(void) { return m_fSpeed; }
	bool IsMoving() { return m_bIsMoving; }
	int getGoalMultiplier(void) { return m_iGoalMultiplier; }
	int getHUDMultiplier(void) { return m_iMultipilerForHUD; }
	int getTotalScore() { return m_bIsFoul ? 0 : ( ((m_iWallPoints + m_iBitPoints) * m_iGoalMultiplier) + m_iBrowniePoints); }
	CSolidSphere& GetVolume() { return m_tVolume; }
	bool GetCamUnattach() { return m_bCamUnattach; }
	D3DXVECTOR3 getDirectionVector() { return directionVec; }
	bool GetGum() const { return m_bGum; }
	bool GetTurkey() const { return m_bTurkey; }
	bool GetUsedTurkey() const { return m_bUsedTurkey; }
	int GetGumCollision(void) { return m_nGumCollisions; }
	std::vector<CPowerUp*> GetPowerUps(void) { return m_vPowerUps; }
	int GetNumPowerUps() const { return m_nNumPowerUps; }
	int GetNumWallHits() const { return m_nNumWallHits; }
	int GetNumStoneHits() const { return m_nNumStoneHits; }
	CPowerUp* GetFirstPOW(void) { return (m_vPowerUps.size() ? m_vPowerUps.front() : NULL); }
	float getInitAngle(void) { return m_fInitAngle; }
	CPlayer* GetOwner(void) { return m_pOwner; }
	float getMass(void) { return m_fMass; }
	bool getFoul() {return m_bIsFoul;}
	float getLaunchAngle(void);
	bool GetOnCherryShot(void) {return m_bOnCherryShot; }
 	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	//Mutators
	void SetOnCherryShot(bool val) { m_bOnCherryShot = val; }
	void SetCurrentHazard(eHazardType e);
	void SetCurrentVolumeSpeed(int n) { m_eCurrVolSpeed = n; }
	void SetGumCollision(int val) { m_nGumCollisions = val; }
	void SetGum(bool val) { m_bGum = val; }
	void SetTurkey(bool val) { m_bTurkey = val; }
	void SetUsedTurkey(bool val) { m_bUsedTurkey = val; }
	void SetCanStop(bool val) { m_bCanStop = val; }
	void setSpeed(float _speed) { m_fSpeed = _speed; }
	void setPosMat(D3DMATRIX position) { m_mPosition = position; }
	void setPosition(D3DXVECTOR3 position);
	void setVelocity(D3DXVECTOR3 velocity) { m_velocity = velocity; }
	void SetFrictionVals(D3DXVECTOR3 vFric)	{ m_tFrictionVals = vFric; }
	void setIsMoving(bool bIsMoving) { m_bIsMoving = bIsMoving; }
	void addBitPoints(int iBitPoints) { m_iBitPoints += iBitPoints; }
	void addWallPoints(int iWallPoints) { m_iWallPoints += iWallPoints; }
	void addBrowniePoints(int iBrowniePoints) { m_iBrowniePoints += iBrowniePoints; }
	void setGoalMultiplier(int iGoalMultiplier) { m_iGoalMultiplier = iGoalMultiplier; }
	void isColliding(bool _isColliding) { m_bIsColliding = _isColliding; }
	void setColor(D3DXVECTOR4 color) { m_vColor = color; }
	void SetCamUnattach(bool bCamUnattach) { m_bCamUnattach = bCamUnattach; }
	void setAngle(float fAngle) { m_fAngle = fAngle; }
	void setPrevPosition(D3DXVECTOR3 prevPosition) { rj_mathlib::setWAxis(&m_mPrevPosition, &prevPosition); }
	void setDirectionVector(D3DXVECTOR3 direction) { directionVec = direction; D3DXVec3Normalize(&directionVecNorm, &directionVec); }
	void SetNumPowerUps(int val) { m_nNumPowerUps = val; }
	void SetNumWallHits(int val) { m_nNumWallHits = val; }
	void SetNumStoneHits(int val) { m_nNumStoneHits = val; }
	void SetColl1(D3DXVECTOR3 _col) { m_vCollisionPoint1 = _col; }
	void SetColl2(D3DXVECTOR3 _col) { m_vCollisionPoint2 = _col; }
	void setMass(float mass) { m_fMass = mass; }
	void setSpeedCap(bool bSpeedCap) { m_bSpeedCap = bSpeedCap; }
	void setFoul(bool bFoul){m_bIsFoul = bFoul;}
	void setRenderAnthony(bool bRenderAnthony) { m_bRenderAnthony = bRenderAnthony; }
	//////////////////////////////////////////////////////////////////////////
};

#endif