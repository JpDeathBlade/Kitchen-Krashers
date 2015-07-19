#include "CStone.h"
#include "../../CoreTech/Utilities/RJ_MathLibrary.hpp"
#include "../../CoreTech/RedJaguarEngine/Renderer/RJ_Renderer.h"
#include "../../Coretech/RedjaguarEngine/Asset/AssetManager.h"
#include "../../Coretech/RedjaguarEngine/Event/EventManager.h"
#include "../../Coretech/RedjaguarEngine/Event/EventDefinitions.h"
#include "../../CoreTech/RedJaguarEngine/Animations/AnimManager.h"
#include "../../CoreTech/RedJaguarEngine/Scene/RJ_SceneManager.h"
#include "Mesh.h"
#include "AnimMesh.h"
#include "PowerUp.h"
#include "../Gameplay.h"
#include "../GameAnim/AnthonyAnimStates.h"

// TODO: fix rj_mathlib
#include <D3dx9math.h>

CStone::CStone(D3DXVECTOR4 _color, CPlayer* pOwner)
{
	Gameplay = CGameplay::GetInstanceUpdate();
	m_pOwner = pOwner;
	m_mPosition = Gameplay->GetStartingPoint();
	m_fInitAngle = acosf(m_mPosition._11)/D3DX_PI;
	m_fAngle = 1.0f;
	m_fMass = 1.5f;
	m_bRenderAnthony = true;

	D3DXVECTOR3 upVector(0.0f, 1.0f, 0.0f);
	D3DXVECTOR3 rightVector;
	rightVector.x = cos((m_fAngle * D3DX_PI) + (D3DX_PI/2.0f));
	rightVector.y = 0.0f;
	rightVector.z = sin((m_fAngle * D3DX_PI) + (D3DX_PI/2.0f));
	D3DXVec3Cross(&m_vForward, &upVector, &rightVector);

	m_vOriginalForward = m_vForward;

	//m_mPosition = *rj_mathlib::rotateYPre(&m_mPosition, ((D3DX_PI / 180.0f) * (-90)));
	//D3DXMatrixIdentity(&m_mPosition);
	D3DXMatrixIdentity(&m_mPrevPosition);
	m_velocity = rj_mathlib::zeroVector3();
	m_position = rj_mathlib::zeroVector3();
	//m_vForward = D3DXVECTOR3(-1.0f, 0.0f, 0.0f);
	m_WorldMatrix = m_mPosition;
	m_tFrictionVals = D3DXVECTOR3(5.0f, 5.0f, 5.0f);
	m_bIsMoving = false;
	m_bSpeedCap = false;
	m_bIsColliding = false;
	m_bIsLaunched = false;
	m_bTransparent = true;
	m_bCurrentStone = false;
	m_bCamUnattach = false;
	m_bOnCherryShot = false;
	m_bDonePOW = false;
	m_bGum = false;
	m_fSpeed = 0;
	m_iWallPoints = 0;
	m_iBitPoints = 0;
	m_iBrowniePoints = 0;
	m_nNumWallHits = 0;
	m_eCurrHazard = NONE;
	m_eCurrVolSpeed = EVEN;
	m_nNumStoneHits = 0;
	m_nNumPowerUps = 0;
	m_nGumCollisions = 0;
	m_iGoalMultiplier = 1;
	m_vColor = _color;
	m_bCanStop = false;
	m_bHasCollided = false;
	m_bDoneCurrent = false;
	m_bTurkey = false;
	m_bUsedTurkey = false;
	m_fCurrTime = 0.0f;
	m_tVolume.SetValue(0.65f);
	m_vCollisionPoint1 = rj_mathlib::getWAxis(&Gameplay->GetStartingPoint());
	m_vCollisionPoint2 = rj_mathlib::getWAxis(&Gameplay->GetStartingPoint());
	prevNorm = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_fPower = 0.0f;
	m_bIsFoul = false;
	POWHat = -1;

	// set cap mesh
	if ( m_vColor == D3DXVECTOR4(1.0f,0.0f,0.0f,1.0f) )
		sprintf_s(m_szCapName, 16, "C_Red_Cap");
	else if ( m_vColor == D3DXVECTOR4(0.0f,0.0f,1.0f,1.0f) )
		sprintf_s(m_szCapName, 16, "C_Purple_Cap");
	else if ( m_vColor == D3DXVECTOR4(1.0f,1.0f,0.0f,1.0f) )
		sprintf_s(m_szCapName, 16, "C_Orange_Cap");
	else if ( m_vColor == D3DXVECTOR4(0.0f,1.0f,0.0f,1.0f) )
		sprintf_s(m_szCapName, 16, "C_Green_Cap");
	//sprintf_s(m_szCapName, 16, "C_Cap");

	//unique ID
	// NOTE: is "static" in a constructor bad?
	static unsigned int _id = 0;
	_id++;
	SetID(_id);

	m_tVolume = CSolidSphere("Stone", D3DXVECTOR3(m_mPosition._41, m_mPosition._42, m_mPosition._43),
		0.5f, 1.0f, STONE, _id, false);

	m_mStartPos = Gameplay->GetStartingPoint();

	char TempFile[256];
	strcpy_s(TempFile,sizeof(char)*64,"Resources\\Models\\");
	strncat_s(TempFile, "SM_Wave", strlen("SM_Wave"));
	strncat_s(TempFile, ".mesh", 5);
		std::fstream FileIn(TempFile, std::ios_base::in | std::ios_base::binary);
		m_meshWave.Load(&FileIn);
		FileIn.close();
	ID_waterSpill = CAssetManager::getInstanceRender()->LoadTexture("T_Effect_Splash_Water.dds");
	ID_orangeSpill = CAssetManager::getInstanceRender()->LoadTexture("T_Effect_Splash_Orange.dds");
	ID_oilSpill = CAssetManager::getInstanceRender()->LoadTexture("T_Effect_Splash_Oil.dds");
	ID_purpleSpill = CAssetManager::getInstanceRender()->LoadTexture("T_Effect_Splash_Purple.dds");
	ID_redSpill = -1;

	m_meshWave.setTextureID(ID_waterSpill);

	m_effectWake.Load("WakeStart");
	m_effectWakeCycle.Load("WakeCycle");
}

CStone::~CStone()
{
	m_effectWake.Shutdown();
	m_effectWakeCycle.Shutdown();
	m_meshWave.Shutdown();	//This CRASHES the game, wtf?? 
}

void CStone::launch(float fPower, float fAngle)
{
	m_fPower = fPower * 7.0f;
	m_fAngle = fAngle;
	m_tVolume.SetIsActive(true);
	m_bIsMoving = true;
	m_bIsLaunched = true;
	m_bDoneCurrent = false;
	m_tVolume.SetValue(0.5f);

	// Send STONELAUNCHED event
	volatile tStoneLaunched* tSL = new tStoneLaunched(EM_STONELAUNCHED); 
	CEventManager::GetInstanceThreaded()->SendEventUpdateCrossToRender(ET_STONELAUNCHED,(tBaseEventData*)tSL);
	tSL = NULL;
}
void CStone::Reset(void)
{
	//D3DXMatrixIdentity(&m_mPosition);
	m_mPosition = Gameplay->GetStartingPoint();
	m_mStartPos = m_mPosition;
	m_fInitAngle = acosf(m_mPosition._11)/D3DX_PI;
	m_fAngle = 1.0f;
	m_fMass = 1.5f;
	m_tVolume.SetValue(0.65f);
	m_bRenderAnthony = true;
	
	D3DXVECTOR3 upVector(0.0f, 1.0f, 0.0f);
	D3DXVECTOR3 rightVector;
	rightVector.x = cos((m_fAngle * D3DX_PI) + (D3DX_PI/2.0f));
	rightVector.y = 0.0f;
	rightVector.z = sin((m_fAngle * D3DX_PI) + (D3DX_PI/2.0f));
	D3DXVec3Cross(&m_vForward, &upVector, &rightVector);

	D3DXMatrixIdentity(&m_mPrevPosition);
	m_vCollisionPoint1 = rj_mathlib::getWAxis(&Gameplay->GetStartingPoint());
	m_vCollisionPoint2 = rj_mathlib::getWAxis(&Gameplay->GetStartingPoint());
	m_velocity = rj_mathlib::zeroVector3();
	m_position = rj_mathlib::zeroVector3();
	//m_vForward = D3DXVECTOR3(-1.0f, 0.0f, 0.0f);
	m_WorldMatrix = m_mPosition;
	m_tFrictionVals = D3DXVECTOR3(5.0f, 5.0f, 5.0f);
	m_bIsMoving = false;
	m_bSpeedCap = false;
	m_bIsColliding = false;
	m_bIsLaunched = false;
	m_bCurrentStone = false;
	m_bHasCollided = false;
	m_bDonePOW = false;
	m_bOnCherryShot = false;
	m_bUsedTurkey = false;
	m_tVolume.SetIsActive(false);
	POWHat = -1;
	m_fSpeed = 0;
	m_eCurrHazard = NONE;
	m_eCurrVolSpeed = EVEN;
	m_iWallPoints = 0;
	m_nNumStoneHits = 0;
	m_nNumWallHits = 0;
	m_nGumCollisions = 0;
	m_nNumPowerUps = 0;
	m_iBitPoints = 0;
	m_iBrowniePoints = 0;
	m_iGoalMultiplier = 1;
	m_bCanStop = false;
	m_bDoneCurrent = false;
	m_bIsFoul = false;
	m_fCurrTime = 0.0f;
	m_vPowerUps.clear();
	m_bTurkey = false;
	m_bGum = false;
	m_bCamUnattach = false;
	prevNorm = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
}

void CStone::Update(double fDelta)
{
	// is this the current stone?
	m_iMultipilerForHUD = m_iGoalMultiplier;
	m_bCurrentStone = (Gameplay->GetCurrPlayer()->GetCurrStone() == this);

	double tempDelta = fDelta;
	if(tempDelta > 0.1)
		tempDelta = 0.1;
	if(m_bIsMoving == true)
	{
		if(m_fSpeed <= m_fPower && !m_bSpeedCap)
		{
			m_fSpeed += m_fPower * (float)tempDelta;
			if(m_fSpeed >= m_fPower)
			{
				m_fSpeed = m_fPower;
				m_bSpeedCap = true;

				//#StopEffect
				volatile tStopEffects* tStop = new tStopEffects(EM_STOPEFFECT, RJ_Config::RJ_MOTION); 
				CEventManager::GetInstanceThreaded()->SendEventUpdateCrossToRender(ET_STOPEFFECT,(tBaseEventData*)tStop);
				tStop = NULL;
				volatile tStopEffects* tStopL = new tStopEffects(EM_STOPEFFECT, RJ_Config::RJ_MOTION); 
				CEventManager::GetInstanceThreaded()->SendEventUpdateLocal(ET_STOPEFFECT,(tBaseEventData*)tStopL);
				tStopL = NULL;
			}
		}
		else if(m_bSpeedCap)
		{
			//D3DXVECTOR3 friction(1.0f, 1.0f, 1.0f);
			// Come up with super awesome friction thingie later on
			if(m_fSpeed > 7.0f)
				m_fSpeed = 7.0f;
			if(m_tFrictionVals.x <= 0.0f)
				m_tFrictionVals.x = 0.0001f;
			if(m_tFrictionVals.y <= 0.0f)
				m_tFrictionVals.y = 0.0001f;
			if(m_tFrictionVals.z <= 0.0f)
				m_tFrictionVals.z = 0.0001f;
			m_tVolume.SetValue(0.65f);
			if(m_fSpeed > 3.0f)
				m_fSpeed -= ((m_tFrictionVals.x + m_tFrictionVals.y + m_tFrictionVals.z)/3.0f) * (float)tempDelta * 0.03f;
			else if(m_fSpeed > 0.0f)
				m_fSpeed -= ((m_tFrictionVals.x + m_tFrictionVals.y + m_tFrictionVals.z)/(3.0f * (m_fSpeed / 7.0f))) * (float)tempDelta * 0.03f;
			else
			{
				m_fSpeed = 0.0f;
				
				m_fMass = 1.0f;
				m_bIsMoving = false;
				m_bDoneCurrent = true;
				m_tVolume.SetValue(1.35f);
			}
			m_tFrictionVals.x = 5.0f;
		}
		m_bRenderAnthony = true;
		m_fAngle += (m_tFrictionVals.y - m_tFrictionVals.z)*0.00005f;
		m_tFrictionVals.y = 5.0f;
		m_tFrictionVals.z = 5.0f;
		if(!m_bCurrentStone)
		{
			m_tFrictionVals.y = 6.0f;
			m_tFrictionVals.z = 6.0f;
		}
		//while(m_fAngle > 2.0f)
		//	m_fAngle -= 2.0f;
		//while(m_fAngle <= 0.0f)
		//	m_fAngle += 2.0f;
		//while(m_fAngle > 1.0f)
		//	m_fAngle -= 1.0f;
		//while(m_fAngle <= -1.0f)
		//	m_fAngle += 1.0f;
		D3DXVECTOR3 upVector(0.0f, 1.0f, 0.0f);
		D3DXVECTOR3 rightVector;
		if(m_fAngle == 0.0f)
			m_fAngle += 0.00001f;
		if(m_fInitAngle == 0.0f)
			m_fInitAngle += 0.00001f;
		rightVector.x = cos(((m_fAngle - m_fInitAngle) * D3DX_PI) + (D3DX_PI/2.0f));
		rightVector.y = 0.0f;
		rightVector.z = sin(((m_fAngle - m_fInitAngle) * D3DX_PI) + (D3DX_PI/2.0f));

		// <NOTE>
		//	this prevents accurate reactions 
		//	with other stones from being possible 
		// </NOTE>

		D3DXVECTOR3 tempForward;
		D3DXVec3Cross(&tempForward, &upVector, &rightVector);
		D3DXVECTOR3 normForward;
		D3DXVec3Normalize(&normForward, &tempForward);
	
		// recalculate m_vForward for GetForward()
		//	because we're silly or whatver
		rightVector.x = cos(((m_fAngle-m_fInitAngle) * D3DX_PI) + (D3DX_PI));
		rightVector.y = 0.0f;
		rightVector.z = sin(((m_fAngle-m_fInitAngle) * D3DX_PI) + (D3DX_PI));
		D3DXVec3Cross(&m_vForward, &upVector, &rightVector);

		m_velocity = normForward * m_fSpeed;
		m_position = m_velocity * (float)tempDelta;
		m_mPosition = *rj_mathlib::translatePre(&m_mPosition, &m_position);
		m_mPrevPosition = m_WorldMatrix;
		m_WorldMatrix = m_mPosition;
		directionVec = rj_mathlib::getWAxis(&m_mPosition) - rj_mathlib::getWAxis(&m_mPrevPosition);
		D3DXVec3Normalize(&directionVecNorm, &directionVec);
		if(!m_bHasCollided)
		{
			m_mGoodPrev = m_mPrevPosition;
			goodDirection = directionVec;
			goodDirectionNorm = directionVecNorm;
		}
		m_bCanStop = true;
		m_bHasCollided = false;
		//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		//HACK
		// WTF IS THIS BELOW??????
		//HACK
		//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		m_fCurrTime = (float)tempDelta;

		m_tVolume.SetPosition(D3DXVECTOR3(m_mPosition._41, m_mPosition._42, m_mPosition._43));
	}
}

D3DXVECTOR3 CStone::getPosition() 
{
	return rj_mathlib::getWAxis(&m_mPosition); 
}
void CStone::setPosition(D3DXVECTOR3 position)
{
	rj_mathlib::setWAxis(&m_mPosition, &position); 
}
void CStone::Render(double _fDelta)
{
	// World Matrix
	D3DXMATRIX _world = GetWorldMat();
	D3DXMATRIX arrowMat;
	if ( !Gameplay->GetLaunched() && m_mPosition == m_mStartPos )
	{
		// Render Arrow
		//	TODO: move mesh origin to end
		if ( m_bCurrentStone )
		{
			D3DXMatrixRotationY( &arrowMat, (-Gameplay->GetAngle() * D3DX_PI) + D3DX_PI);

			//float fRadians = acosf(_world._11);
			// put it in world-space instead
			arrowMat = (_world * arrowMat);

			arrowMat._41 = _world._41/* + (1.0f * GetForward().x)*/;
			arrowMat._42 = _world._42/* + (1.0f * GetForward().y)*/;
			arrowMat._43 = _world._43/* + (1.0f * GetForward().z)*/;

			//rj_mathlib::rotateYPre(&arrowMat, (Gameplay->GetAngle() * D3DX_PI) + D3DX_PI/2);

			RJ_RENDERER->getDirect3DDevice()->SetTransform(D3DTS_WORLD, &arrowMat);
			CAssetManager::getInstanceRender()->GetMesh( "DM_Arrow" )->Render(_fDelta);
		}
		//else			// Don't do this,
		//	return;		//	it prevents all stones from rendering
	}

	if ( m_bCurrentStone )
	{
		D3DXMATRIX _rot;
		D3DXMatrixIdentity( &_rot );
		float radians;
		if(m_bIsLaunched)
			radians = atan2f( GetForward().x, GetForward().z );
		else if(Gameplay->GetCurrLevel() == 1)
			radians = -D3DX_PI + (D3DX_PI * 0.5f);
		else
			radians = (-Gameplay->GetAngle() * D3DX_PI) + (D3DX_PI * 0.5f);
		float oldPos[3]; oldPos[0] = _world._41; oldPos[1] = _world._42; oldPos[2] = _world._43; 

		D3DXMatrixRotationY( &_rot, radians + (D3DX_PI/2));
		_rot = _world * _rot;
		_rot._41 = oldPos[0]; _rot._42 = oldPos[1]; _rot._43 = oldPos[2];
		/*if ( Gameplay->GetLaunched() )
		{*/
			RJ_RENDERER->getDirect3DDevice()->SetTransform(D3DTS_WORLD, &_rot);
		//}
		/*else
		{
			_rot = _world;
			D3DXMATRIX _startmat = SceneManager::getInstanceUpdate()->GetGummyStart();
			float oldStart[3]; oldStart[0] = _startmat._41; oldStart[1] = _startmat._42; oldStart[2] = _startmat._43; 
			_startmat = arrowMat;
			_startmat._41 = oldStart[0]; _startmat._42 = oldStart[1]; _startmat._43 = oldStart[2];
			RJ_RENDERER->getDirect3DDevice()->SetTransform(D3DTS_WORLD, &_startmat);
			AnimLink* AGniLink = CAssetManager::getInstanceRender()->GetAnimLink(m_pOwner->GetGlobalID(), RJ_Config::RJ_ANIM_GUMMY);
			if(AGniLink)
			{
				AnimStateMachine* GummyStateMachine = AGniLink->m_AStateMachine;
				CAssetManager::getInstanceRender()->GetAnimMesh( "GummyWorm" )->Render(_fDelta,GummyStateMachine);
				GummyStateMachine = NULL;
			}
			AGniLink = NULL;

			AnimLink* ATniLink = CAssetManager::getInstanceRender()->GetAnimLink(m_pOwner->GetGlobalID(), RJ_Config::RJ_ANIM_TOOTHPICK);
			if(ATniLink)
			{
				AnimStateMachine* GummyStateMachine = ATniLink->m_AStateMachine;
				CAssetManager::getInstanceRender()->GetAnimMesh( "toothPicks" )->Render(_fDelta,GummyStateMachine);
				GummyStateMachine = NULL;
			}
			ATniLink = NULL;
		}*/
		
		AnimAnthony* AniLink = (AnimAnthony*)CAssetManager::getInstanceRender()->GetAnimLink(m_pOwner->GetGlobalID(), RJ_Config::RJ_ANIM_ANTHONY);
		AnimLink* AniBLink = CAssetManager::getInstanceRender()->GetAnimLink(m_pOwner->GetGlobalID(), RJ_Config::RJ_ANIM_BROOM);
		bool bdrawtrans = AniLink->m_bLaunchDone;
		
		if(!bdrawtrans && m_fPower != 0.0f && Gameplay->GetCurrLevel() == 1)
		{
			D3DXMatrixIdentity( &_rot );
			float fDistZ = m_mPosition._43 - ( m_mStartPos._43 + (10.0f * m_fPower));
			float fDistX = m_mPosition._41 - m_mStartPos._41;
			float radians = atan2f( fDistZ, fDistX );
			float oldPos[3]; oldPos[0] = _world._41; oldPos[1] = _world._42; oldPos[2] = _world._43; 
			D3DXMatrixRotationY( &_rot, radians + (D3DX_PI/2));
			_rot = _world * _rot;
			_rot._41 = oldPos[0]; _rot._42 = oldPos[1]; _rot._43 = oldPos[2];
			RJ_RENDERER->getDirect3DDevice()->SetTransform(D3DTS_WORLD, &_rot);
		}
		
		// Render Ant + Cap
		if(AniLink)
		{
			AnimStateMachine* AnthonyStateMachine = AniLink->m_AStateMachine;
			if(m_bRenderAnthony)
				CAssetManager::getInstanceRender()->GetAnimMesh( "AnthonyBody" )->Render(_fDelta,AnthonyStateMachine);
			AnthonyStateMachine = NULL;
		}
		AniLink = NULL;
		if(AniBLink)
		{
			AnimStateMachine* BroomStateMachine = AniBLink->m_AStateMachine;
			if(m_bRenderAnthony)
				CAssetManager::getInstanceRender()->GetAnimMesh( "SM_Broom" )->Render(_fDelta,BroomStateMachine);
			BroomStateMachine = NULL;
		}
		CAssetManager::getInstanceRender()->GetMesh( m_szCapName )->Render(_fDelta);
		
		D3DXMATRIX HatWorld = CAssetManager::getInstanceRender()->GetHatJoint();
		//D3DXMatrixRotationY(&HatWorld,90.0f);
		//oldPos[0] = _rot._41 + HatWorld._41; oldPos[1] = _rot._42 + HatWorld._42; oldPos[2] = _rot._43 + HatWorld._43; 
		HatWorld *= _rot;
		//HatWorld._41 = oldPos[0]; HatWorld._42 = oldPos[1]; HatWorld._43 = oldPos[2];
		RJ_RENDERER->getDirect3DDevice()->SetTransform(D3DTS_WORLD, &HatWorld);

		if(m_bOnCherryShot)
		{
			CAssetManager::getInstanceRender()->GetMesh( "H_CherryHat" )->Render(_fDelta);
			m_bDonePOW = true;

			D3DXVECTOR3 m_vCollisionPoint;
			m_vCollisionPoint.x = HatWorld._41;
			m_vCollisionPoint.y = HatWorld._42;
			m_vCollisionPoint.z = HatWorld._43;

			volatile tPlayEffect* tEFFECT = new tPlayEffect(EM_PLAYEFFECT, false, RJ_Config::RJ_HAT, m_vCollisionPoint, RJ_Config::RJ_HAT_CHERRY); 
			CEventManager::GetInstanceThreaded()->SendEventRenderLocal(ET_PLAYEFFECT,(tBaseEventData*)tEFFECT);
			tEFFECT = NULL;
		}
		else
		{
			if(m_vPowerUps.size() != 0)
			{
				if(((CPowerUp*)m_vPowerUps.front())->GetPOWType() != CHERRIES)
				{
					POWHat = ((CPowerUp*)m_vPowerUps.front())->GetPOWType();
				}
					m_bDonePOW = true;
			}
			if(POWHat != -1)
			{
				switch(POWHat)
				{
				case FORTUNE_COOKIE:
					{
						CAssetManager::getInstanceRender()->GetMesh( "H_FortuneCookie" )->Render(_fDelta);
						m_bDonePOW = true;

						D3DXVECTOR3 m_vCollisionPoint;
						m_vCollisionPoint.x = HatWorld._41;
						m_vCollisionPoint.y = HatWorld._42;
						m_vCollisionPoint.z = HatWorld._43;

						volatile tPlayEffect* tEFFECT = new tPlayEffect(EM_PLAYEFFECT, false, RJ_Config::RJ_HAT, m_vCollisionPoint, RJ_Config::RJ_HAT_FC); 
						CEventManager::GetInstanceThreaded()->SendEventRenderLocal(ET_PLAYEFFECT,(tBaseEventData*)tEFFECT);
						tEFFECT = NULL;
					} break;
				case WHIP_CREAM:
					{
						CAssetManager::getInstanceRender()->GetMesh( "H_WhippedCreamHat" )->Render(_fDelta);
						m_bDonePOW = true;

						D3DXVECTOR3 m_vCollisionPoint;
						m_vCollisionPoint.x = HatWorld._41;
						m_vCollisionPoint.y = HatWorld._42;
						m_vCollisionPoint.z = HatWorld._43;
						volatile tPlayEffect* tEFFECT = new tPlayEffect(EM_PLAYEFFECT, false, RJ_Config::RJ_HAT, m_vCollisionPoint, RJ_Config::RJ_HAT_COW); 
						CEventManager::GetInstanceThreaded()->SendEventRenderLocal(ET_PLAYEFFECT,(tBaseEventData*)tEFFECT);
						tEFFECT = NULL;
					} break;
				case GUM:
					{
						CAssetManager::getInstanceRender()->GetMesh( "H_GumHat" )->Render(_fDelta);
						m_bDonePOW = true;

						D3DXVECTOR3 m_vCollisionPoint;
						m_vCollisionPoint.x = HatWorld._41;
						m_vCollisionPoint.y = HatWorld._42;
						m_vCollisionPoint.z = HatWorld._43;
						volatile tPlayEffect* tEFFECT = new tPlayEffect(EM_PLAYEFFECT, false, RJ_Config::RJ_HAT, m_vCollisionPoint, RJ_Config::RJ_HAT_GUM); 
						CEventManager::GetInstanceThreaded()->SendEventRenderLocal(ET_PLAYEFFECT,(tBaseEventData*)tEFFECT);
						tEFFECT = NULL;
					} break;
				case TURKEY:
					{
						CAssetManager::getInstanceRender()->GetMesh( "H_ColdTurkeyHat" )->Render(_fDelta);
						m_bDonePOW = true;

						D3DXVECTOR3 m_vCollisionPoint;
						m_vCollisionPoint.x = HatWorld._41;
						m_vCollisionPoint.y = HatWorld._42;
						m_vCollisionPoint.z = HatWorld._43;

						volatile tPlayEffect* tEFFECT = new tPlayEffect(EM_PLAYEFFECT, false, RJ_Config::RJ_HAT, m_vCollisionPoint, RJ_Config::RJ_HAT_TURKEY); 
						CEventManager::GetInstanceThreaded()->SendEventRenderLocal(ET_PLAYEFFECT,(tBaseEventData*)tEFFECT);
						tEFFECT = NULL;
					} break;
				}
			}
		}
		
	}
	else if ( m_mStartPos != m_mPosition  && !m_bIsFoul)
	{
		D3DXMATRIX flipMat;// = _world;
		D3DXMatrixRotationZ(&flipMat, D3DX_PI);
		flipMat._41 = _world._41; 
		flipMat._42 = _world._42 + 0.19f;
		flipMat._43 = _world._43;
		RJ_RENDERER->getDirect3DDevice()->SetTransform(D3DTS_WORLD, &flipMat);

		// Render flipped Cap
		CAssetManager::getInstanceRender()->GetMesh( m_szCapName )->Render(_fDelta);

		if(m_bGum)
		{
			RJ_RENDERER->getDirect3DDevice()->SetTransform(D3DTS_WORLD, &_world);
			CAssetManager::getInstanceRender()->GetMesh( "DM_GumMESS" )->Render(_fDelta);
		}
	}

	if(m_eCurrHazard != NONE && m_eCurrHazard != ICE &&
		m_eCurrHazard != STICKY && m_bIsMoving)	//ie I am in a hazard, draw effects or some shit, idk
		{
			//Holy shit, it worked =D
				D3DXMATRIX _rot1;
				D3DXMATRIX _translate1;
				D3DXMATRIX _world1;
				D3DXMatrixIdentity(&_rot1); D3DXMatrixIdentity(&_translate1); D3DXMatrixIdentity(&_world1);
				D3DXMatrixRotationY(&_rot1,atan2f(m_vForward.x,m_vForward.z));
				D3DXMatrixTranslation(&_translate1, getPosition().x, getPosition().y, getPosition().z);
				_world1 = _rot1 * _translate1;
				RJ_RENDERER->getDirect3DDevice()->SetTransform(D3DTS_WORLD, &_world1);
				m_meshWave.Render(_fDelta);

			if(m_eCurrHazard == WATER)
			{
				m_EffectTime += _fDelta;

				if(m_EffectTime >= 0.36f)
				{
						if(!m_effectWakeCycle.isPlaying())
						{
							m_effectWakeCycle.SetPosition(getPosition());
						}
						else
						{
							m_effectWakeCycle.SetPositionWithoutUpdate(getPosition());
						}
					m_effectWakeCycle.SetForward(m_vForward);
					m_effectWakeCycle.Render(_fDelta);
				}
				else
				{
					if(!m_effectWake.isPlaying())
						{
							m_effectWake.SetPosition(getPosition());
						}
						else
						{
							m_effectWake.SetPositionWithoutUpdate(getPosition());
						}
					m_effectWake.SetForward(m_vForward);
					m_effectWake.Render(_fDelta);
				}
			}
		}
	else
		{
			m_EffectTime = 0.0f;
			m_effectWake.StopPlaying();
			m_effectWakeCycle.StopPlaying();
		}

	//if(m_bCurrentStone)
	//{
	//	char szBuffer[128];
	//	sprintf_s(szBuffer, "collisions %i", collisions);
	//	RJ_RENDERER->RenderText(szBuffer, 0, 90);
	//	if(!allNormals.empty())
	//	{
	//		for(unsigned int i = 0; i < allNormals.size(); i++)
	//		{
	//			if(omitted[i])
	//			{
	//				sprintf_s(szBuffer, "Normal %i, %f %f %f", i, allNormals[i].x, allNormals[i].y, allNormals[i].z);
	//				RJ_RENDERER->RenderText(szBuffer, 0, 30 * (i+9));
	//			}
	//		}
	//		sprintf_s(szBuffer, "last normal %f %f %f", lastNormal.x, lastNormal.y, lastNormal.z);
	//		RJ_RENDERER->RenderText(szBuffer, 0, 120);
	//	}

	//	sprintf_s(szBuffer, "forward %f %f %f", directionVecNorm.x, directionVecNorm.y, directionVecNorm.z);
	//	RJ_RENDERER->RenderText(szBuffer, 0, 150);

	//	sprintf_s(szBuffer, "forward %f %f %f", m_vForward.x, m_vForward.y, m_vForward.z);
	//	RJ_RENDERER->RenderText(szBuffer, 0, 180);

	//	sprintf_s(szBuffer, "forward %f %f %f", normForward.x, normForward.y, normForward.z);
	//	RJ_RENDERER->RenderText(szBuffer, 0, 210);

	//	sprintf_s(szBuffer, "Angle %f", m_fAngle);
	//	RJ_RENDERER->RenderText(szBuffer, 0, 240);

	//	//sprintf_s(szBuffer, "total normals %i", allNormals.size());
	//	//RJ_RENDERER->RenderText(szBuffer, 0, 180);
	//}
	// DEBUG INFO
	//if(!m_bIsColliding)
	//	RJ_RENDERER->DrawSphere(&m_mPosition, &m_vColor);
	//	RJ_RENDERER->Render3DLine(rj_mathlib::getWAxis(&m_mPosition), rj_mathlib::getWAxis(&m_mPosition)+m_vForward, D3DXVECTOR4(0.0f, 1.0f, 0.0f, 1.0f));
	//else
	//	RJ_RENDERER->DrawSphere(&m_mPosition, &D3DXVECTOR4(1.0f, 0.0f, 1.0f, 1.0f));
}

void CStone::SetCurrentHazard(eHazardType e)
{
	CGameplay::GetInstanceUpdate()->m_bWaterCollide = false;;
	if(e == WATER)
	{
		CGameplay::GetInstanceUpdate()->m_bWaterCollide = true;
		m_meshWave.setTextureID(ID_waterSpill);
		volatile tPlaySoundEnum* tSOUND = new tPlaySoundEnum(EM_PLAYSOUNDENUM, RJ_Config::RJ_ANTHONY_HAZARD,false); 
		CEventManager::GetInstanceThreaded()->SendEventUpdateCrossToRender(ET_PLAYSOUND,(tBaseEventData*)tSOUND);
		tSOUND = NULL;
	}
	else if(e == OIL)
	{
		CGameplay::GetInstanceUpdate()->m_bWaterCollide = true;
		m_meshWave.setTextureID(ID_orangeSpill);
		volatile tPlaySoundEnum* tSOUND = new tPlaySoundEnum(EM_PLAYSOUNDENUM, RJ_Config::RJ_ANTHONY_HAZARD,false); 
		CEventManager::GetInstanceThreaded()->SendEventUpdateCrossToRender(ET_PLAYSOUND,(tBaseEventData*)tSOUND);
		tSOUND = NULL;
	}
	else if(e == SODA)
	{
		CGameplay::GetInstanceUpdate()->m_bWaterCollide = true;
		m_meshWave.setTextureID(ID_oilSpill);
		volatile tPlaySoundEnum* tSOUND = new tPlaySoundEnum(EM_PLAYSOUNDENUM, RJ_Config::RJ_ANTHONY_HAZARD,false); 
		CEventManager::GetInstanceThreaded()->SendEventUpdateCrossToRender(ET_PLAYSOUND,(tBaseEventData*)tSOUND);
		tSOUND = NULL;
	}
	else if(e == STICKYP)
	{
		CGameplay::GetInstanceUpdate()->m_bWaterCollide = true;
		m_meshWave.setTextureID(ID_purpleSpill);
		volatile tPlaySoundEnum* tSOUND = new tPlaySoundEnum(EM_PLAYSOUNDENUM, RJ_Config::RJ_ANTHONY_HAZARD,false); 
		CEventManager::GetInstanceThreaded()->SendEventUpdateCrossToRender(ET_PLAYSOUND,(tBaseEventData*)tSOUND);
		tSOUND = NULL;
	}
	
	if(m_bCurrentStone)
	{
		volatile tCurrentVolume* tCV = new tCurrentVolume(EM_CURRENTVOLUME, int(e)); 
		CEventManager::GetInstanceThreaded()->SendEventUpdateCrossToRender(ET_CURRENTVOLUME,(tBaseEventData*)tCV);
		tCV = NULL;
	}
	

	m_eCurrHazard = e;
}

void CStone::EmptyPowerUps(void)
{
	if(m_bDonePOW)
		m_vPowerUps.clear(); 
}
void CStone::AddPowerUp(CPowerUp* _POW)
{
	m_vPowerUps.push_back(_POW); 
	m_bDonePOW = false;
}
	
float CStone::getLaunchAngle()
{
	return Gameplay->GetAngle();
}

void CStone::react(IVolumeObject* vol, D3DXVECTOR3 normal)
{
	//D3DXVECTOR3 vNormal = vol->GetNormal(getPosition());// DON'T DO THIS: directionVecNorm);

	volatile tPlaySoundEnum* tSOUND = new tPlaySoundEnum(EM_PLAYSOUNDENUM, RJ_Config::RJ_COLLISION_ANTHONY,true); 
	CEventManager::GetInstanceThreaded()->SendEventUpdateCrossToRender(ET_PLAYSOUND,(tBaseEventData*)tSOUND);
	tSOUND = NULL;
	volatile tPlaySoundEnum* tCSOUND = new tPlaySoundEnum(EM_PLAYSOUNDENUM, vol->GetCollisionSound() ,true); 
	CEventManager::GetInstanceThreaded()->SendEventUpdateCrossToRender(ET_PLAYSOUND,(tBaseEventData*)tCSOUND);
	tCSOUND = NULL;
	if ( m_bCurrentStone )
	{
		volatile tAnimStateChange* tAChange = new tAnimStateChange(EM_ANIMSTATECHANGE, m_pOwner->GetGlobalID(),
			RJ_Config::RJ_ANIM_ANTHONY,RJ_Config::ANTH_ANIM_COLLITION,false); 
		CEventManager::GetInstanceThreaded()->SendEventUpdateCrossToRender(ET_ANIMSTATECHANGE,(tBaseEventData*)tAChange);
		tAChange = NULL;
		volatile tAnimStateChange* tABChange = new tAnimStateChange(EM_ANIMSTATECHANGE, m_pOwner->GetGlobalID(),
			RJ_Config::RJ_ANIM_BROOM,RJ_Config::BROOM_ANIM_COLLITION,false); 
		CEventManager::GetInstanceThreaded()->SendEventUpdateCrossToRender(ET_ANIMSTATECHANGE,(tBaseEventData*)tABChange);
		tABChange = NULL;
		CGameplay::GetInstanceUpdate()->ClearAnimSweepEvents();
		volatile tStopSoundEnum* tSCSOUND = new tStopSoundEnum(EM_STOPSOUNDENUM, RJ_Config::RJ_SWEEP_ANTHONY); 
		CEventManager::GetInstanceThreaded()->SendEventUpdateCrossToRender(ET_STOPSOUND,(tBaseEventData*)tSCSOUND);
		tSCSOUND = NULL;
		volatile tStopSoundEnum* tSTSOUND = new tStopSoundEnum(EM_STOPSOUNDENUM, RJ_Config::RJ_ANTHONY_SCRAPE); 
		CEventManager::GetInstanceThreaded()->SendEventUpdateCrossToRender(ET_STOPSOUND,(tBaseEventData*)tSTSOUND);
		tSTSOUND = NULL;
	}

	//if(prevNorm == normal)
	//{
	//	directionVec = goodDirection;
	//	directionVecNorm = goodDirectionNorm;
	//}

	//float dot = D3DXVec3Dot(&normal, &directionVecNorm);

	//D3DXVECTOR3 reflectVec;
	//reflectVec.x = directionVecNorm.x - fabs(2.0f * dot) * normal.x;
	//reflectVec.y = 0.0f;
	//reflectVec.z = directionVecNorm.z - fabs(2.0f * dot) * normal.z;
	//D3DXVECTOR3 negDir = -directionVec;
	//float norm = D3DXVec3Dot(&negDir, &normal);
	//D3DXVECTOR3 reflectVec(0.0f, 0.0f, 0.0f);
	//reflectVec = 2.0f * norm * (normal + directionVec);// * 2.0f * norm;
	//D3DXVECTOR3 vTangent;
	//D3DXVec3Cross(&vTangent, &D3DXVECTOR3(0.0f, 1.0f, 0.0f), &normal);
	//m_fAngle = (2.0f * acosf(D3DXVec3Dot(&directionVec, &vTangent)))/D3DX_PI;

	D3DXVECTOR3 reflectVec = directionVecNorm - 2.0f*D3DXVec3Dot(&directionVecNorm, &normal) * normal;
	//float rotate = D3DXVec3Dot(&negDir, &normal);
	//D3DXMATRIX tempRotated;
	//D3DXMatrixRotationY(&tempRotated, rotate*2.0f);

	//if(reflectVec.z == 0.0f)
	//	reflectVec.z += 0.00001f;
	//if(reflectVec.x == 0.0f)
	//	reflectVec.x += 0.00001f;
	D3DXVec3Normalize(&reflectVec, &reflectVec);
	m_fAngle = atan2f(reflectVec.z, reflectVec.x)/D3DX_PI;
	//while(m_fAngle > 2.0f)
	//	m_fAngle -= 2.0f;
	//while(m_fAngle <= 0.0f)
	//	m_fAngle += 2.0f;
	m_bHasCollided = true;
	
	//if(prevNorm == normal)
	//{
	//	m_fAngle = atan2f(normal.z, normal.x)/D3DX_PI;
	//}

	if(D3DXVec3Dot(&directionVecNorm, &normal) < 0.2f)
	{
		D3DXVECTOR3 PoP = vol->GetClosestPoint(getPosition());
		D3DXVECTOR3 V = getPosition() - PoP;
		float D = D3DXVec3Dot(&V, &normal);
		D3DXVECTOR3 normalTranslate = normal * ((m_tVolume.GetRadius() + 0.001f) - D);
		setPosition(getPosition() + normalTranslate);
	}
	else
	{
		float planeOffset;
		//if(vol->GetMaterialType() == STONE)
		//	planeOffset = vol->GetPlaneOffset(normal) + (GetVolume().GetRadius()*0.5f) + 0.001f;
		planeOffset = vol->GetPlaneOffset(normal) + GetVolume().GetRadius() + 0.001f;
		D3DXVECTOR3 newPlanePosition = vol->GetPosition() + (normal * planeOffset);
		float D0 = D3DXVec3Dot(&normal, &rj_mathlib::getWAxis(&m_mPrevPosition));
		float D1 = D3DXVec3Dot(&normal, &newPlanePosition);
		float D2 = D0 - D1;
		float D3 = D3DXVec3Dot(&normal, &directionVec);

		if(D2 == 0.0f)
			D2 += 0.00001f;
		if(D3 == 0.0f)
			D3 += 0.00001f;
		float DF = -(D2 / D3);
		D3DXVECTOR3 position = rj_mathlib::getWAxis(&m_mPrevPosition) +(directionVec * DF);
		position.y = 0.0f;
		m_mPosition._41 = position.x;
		m_mPosition._42 = position.y;
		m_mPosition._43 = position.z;
		if(abs(rj_mathlib::getWAxis(&m_mPosition).x - rj_mathlib::getWAxis(&m_mPrevPosition).x) > abs(m_position.x) || abs(rj_mathlib::getWAxis(&m_mPosition).z - rj_mathlib::getWAxis(&m_mPrevPosition).z) > abs(m_position.z))
		{
			m_mPosition._41 = m_mPrevPosition._41;
			m_mPosition._42 = m_mPrevPosition._42;
			m_mPosition._43 = m_mPrevPosition._43;
			m_mPrevPosition = *rj_mathlib::translatePre(&m_mPrevPosition, &(-m_position));
		}
		else
		{
			m_mPrevPosition._41 = position.x;
			m_mPrevPosition._42 = position.y;
			m_mPrevPosition._43 = position.z;
			m_mPrevPosition = *rj_mathlib::translatePre(&m_mPrevPosition, &(-m_position));
		}
		m_WorldMatrix = m_mPosition;
	}

	if(vol->GetMaterialType() != STONE)
		m_fSpeed = (m_fSpeed * vol->GetValue());
	if(m_fSpeed < 0.0f)
		m_fSpeed = 0.0f;

	prevNorm = normal;

	// ALL OF THE BELOW CODE IS USELESS!
	//
	// Manual Sphere check
	//if (vol->GetObjectType() == RJ_Config::RJ_SOLIDSPHEREVOLUME)
	//{
	//	D3DXVECTOR3 pt1 = vol->GetClosestPoint(rj_mathlib::getWAxis(&m_mPosition));
	//	D3DXVECTOR3 pt2 = GetVolume().GetClosestPoint(vol->GetPosition());
	//	D3DXVECTOR3 toVec = pt2 - pt1;

	//	float fDist = abs( D3DXVec3Dot(&toVec, &toVec) );
	//	m_WorldMatrix._41 += normal.x * fDist;
	//	m_WorldMatrix._43 += normal.z * fDist;
	//	m_mPosition._41 += normal.x * fDist;
	//	m_mPosition._43 += normal.z * fDist;
	//}

	//if(m_fSpeed == 0.0f)
	//	m_fSpeed = vol->GetValue() * 4.0f;
	//else
	//{
	//}

	//m_position += vNormal * m_fCurrTime*100.0f;
	//m_mPosition = *rj_mathlib::translatePre(&m_mPosition, &m_position);
	//m_mPrevPosition = m_WorldMatrix;
	//float angle = D3DXVec3Dot(&D3DXVECTOR3(0.0f, 0.0f, 1.0f), &D3DXVECTOR3(0.0f, 0.0f, 1.0f));
	//angle = D3DXVec3Dot(&D3DXVECTOR3(1.0f, 0.0f, 0.0f), &D3DXVECTOR3(0.0f, 0.0f, 1.0f));
	//D3DXVECTOR3 normal;
	//D3DXVec3Normalize(&normal, &D3DXVECTOR3(4.0f, 0.0f, 4.0f));
	//m_fAngle = D3DXVec3Dot(&reflectionVec, &D3DXVECTOR3(0.0f, 0.0f, 1.0f));
	//angle = D3DXVec3Dot(&D3DXVECTOR3(1.0f, 0.0f, 0.0f), &normal);
	//angle = D3DXVec3Dot(&D3DXVECTOR3(0.0f, 0.0f, 1.0f), &normal);
	//m_mPosition = m_mPrevPosition;
	//m_WorldMatrix = m_mPrevPosition;
	//D3DXVECTOR3 norm;
	//D3DXVec3Normalize(&norm, &rj_mathlib::getWAxis(&m_mPosition));
	//float dot = D3DXVec3Dot(&norm, &(vol->GetNormal(m_position)));
	//dot = ( dot < -1.0f ? -1.0f : ( dot > 1.0f ? 1.0f : dot ) );
	//float angle = acos( dot );

	//if(dot == 0.5f || dot == -0.5f)
	//	m_fAngle -= 1.0f;
	//else if(dot > 0.0f)
	//	m_fAngle = (m_fAngle - 1.0f) - dot;
	//	//m_fAngle = -m_fAngle + (dot*2.0f);
	//else if(dot < 0.0f)
	//	m_fAngle = (m_fAngle - 1.0f) + dot;
		//m_fAngle = -m_fAngle + (1.0f - (dot*2.0f));

	//while(m_fAngle > 1.0f)
	//	m_fAngle -= 2.0f;
	//while(m_fAngle <= -1.0f)
	//	m_fAngle += 2.0f;
}