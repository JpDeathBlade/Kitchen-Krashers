#include "Camera.h"
#include "../../../RJ_Config.h"
#include "../Event/IListener.h"
#include "../Event/EventManager.h"
#include "RJ_SceneManager.h"
#include "../../Utilities/RJ_MathLibrary.hpp"
#include "../../../Gameplay/Gameplay.h"

CCamera::CCamera(void)
{
	D3DXMatrixIdentity(&m_mProjMat);
	D3DXMatrixIdentity(&PrevMat);
	D3DXMatrixIdentity(&PrevLook);
	D3DXMatrixIdentity(&m_mViewMat);


	FieldofView = 45.0f;
	Near = 0.1f;
	Far = 1000.0f;
	Width = 1024.0f;
	Height = 768.0f;
	lerp = 0.0f;
	Speed = 0;

	Look = true;
	Done = false;
	LerpisDone = false;
	SendDoneMess = false;
	CEventManager::GetInstanceThreaded()->RegisterListenerUpdate(ET_RENDERSTUFF,this);

	m_fSentTimer = 0.5f;
}

CCamera::~CCamera(void)
{
	CEventManager::GetInstanceThreaded()->UnregisterListenerUpdate(ET_RENDERSTUFF,this);

	while(false == SoftAttachQueue.empty())
		SoftAttachQueue.pop();
}
void CCamera::HandleEvent(CEvent* pEvent)
{
	if(pEvent->GetEventID() == ET_RENDERSTUFF)
	{
		if(pEvent->GetData() != NULL)
		{
			if(pEvent->GetData()->GetMsgID() == EM_RENDERSTUFF)
			{
				tRenderStuff* tInp;
				tInp = (tRenderStuff*)pEvent->GetData();
				SendDoneMess = tInp->m_bRenderSelection;
			}
		}
	}
	pEvent->m_nRefs--;
}
bool CCamera::init(float Near, float far, float ClientWidth, float ClientHeight)
{
	BuildPerspective();
	BuildViewFrustum();
	m_cGameplay = CGameplay::GetInstanceUpdate();
	return true;
}
void CCamera::attachTo(D3DXMATRIX &OutMat, const D3DXMATRIX &AttachMat, const D3DXMATRIX &AttachToMat,
					   const D3DXVECTOR3 &offset, const D3DXVECTOR3 &target, double fDelta, bool _launched, bool MenuCam, bool ClearQ, bool CamFly)
{
	//m_fSentTimer += (float)fDelta;
	//if(!_collide)
	SoftAttach(OutMat, AttachMat, AttachToMat, offset, fDelta, _launched, ClearQ, CamFly);

	LookAt(OutMat, AttachMat, target, fDelta);
}
void CCamera::SoftAttach(D3DXMATRIX &OutMat, const D3DXMATRIX &AttachMat, const D3DXMATRIX &AttachToMat,
						 const D3DXVECTOR3 &offset, double fDelta, bool _launched, bool MenuCam, bool ClearQ, bool CamFly)
{
	m_fSentTimer += (float)fDelta;
	if(!MenuCam)
	{
		if(ClearQ)
		{
			while(false == SoftAttachQueue.empty())
				SoftAttachQueue.pop();

			for( unsigned int i = 0; i < 50; ++i )
				SoftAttachQueue.push(AttachToMat);
		}
	}

	if (!_launched)
	{
		lerp = 0;
		SoftAttachQueue.push(AttachToMat);
		OutMat = SoftAttachQueue.front();
		SoftAttachQueue.pop();
		OutMat = *rj_mathlib::translatePost(&OutMat, &offset); 
		PrevMat = OutMat;
	}
	else
	{
		D3DXMATRIX dest;
		D3DXVECTOR3 temp, tempX, tempY, tempZ, temp2;
		double dist, timeDist, CamFlyDist;
		SoftAttachQueue.push(AttachToMat);
		dest = SoftAttachQueue.front();
		dest = *rj_mathlib::translatePost(&dest, &offset/*D3DXVECTOR3(0, 3, 0)*/);
		temp = rj_mathlib::getWAxis(&dest) - rj_mathlib::getWAxis(&PrevMat);
		//temp.y = 0;
		dist = D3DXVec3Dot(&temp, &temp);
		temp2 = rj_mathlib::getWAxis(&dest) - rj_mathlib::getWAxis(&AttachMat);
		
		if(MenuCam)
		{		
		//	if(dist > 0.09)
			//{ 
			//timeDist = dist * (fDelta * 0.1);
			/*if(!CamFly)
				 lerp = 2.0f * (float)fDelta;
			else
				lerp = (float)fDelta;*/
			float speed = 0.0f;

			if(ClearQ)
				speed = 1.5f;
			else
			{
				CamFlyDist = D3DXVec3Dot(&temp2, &temp2);
				speed = (float)sqrt(CamFlyDist) / 10.0f;

				if(Speed > 7.0f)
					speed = 7.0f;
			}

			//if(Speed < 0.75f)
			//	Speed = 1.0f;
			
			if(lerp < 1.0)
			{
				if(CamFly)
				{
					lerp += (float)fDelta / speed;
				}
				else
				{
					lerp += (float)fDelta;
				}
			}
			else if(lerp > 1.0f && !Done)
				lerp = 1.0f;
			else
			{
				Done = true;
			}

			if (!Done)
			{
				D3DXVec3Lerp(&temp, &rj_mathlib::getWAxis(&AttachMat), &rj_mathlib::getWAxis(&dest), lerp);
				

		/*		D3DXQUATERNION qStart, qEnd, qCurrent;

				D3DXQuaternionRotationMatrix(&qEnd, &dest);
				D3DXQuaternionRotationMatrix(&qStart, &AttachMat);

				D3DXQuaternionSlerp(&qCurrent, &qStart, &qEnd, lerp);

				D3DXMatrixRotationQuaternion(&PrevMat, &qCurrent);*/

				rj_mathlib::setWAxis(&PrevMat, &temp);

				/*D3DXVec3Lerp(&tempZ, &rj_mathlib::getZAxis(&AttachMat), &rj_mathlib::getZAxis(&dest), lerp);
				D3DXVec3Normalize(&tempZ, &tempZ);

				D3DXVec3Cross(&tempX, &tempZ, &D3DXVECTOR3(0.0f, -1.0f, 0.0f));
				D3DXVec3Normalize(&tempX, &tempX);

				D3DXVec3Cross(&tempY, &tempZ, &tempX);
				D3DXVec3Normalize(&tempY, &tempY);


				rj_mathlib::setXAxis(&PrevMat, &tempX);
				rj_mathlib::setYAxis(&PrevMat, &tempY);
				rj_mathlib::setZAxis(&PrevMat, &tempZ);
				rj_mathlib::setWAxis(&PrevMat, &temp);*/
				
 				D3DXVec3Lerp(&tempX, &rj_mathlib::getXAxis(&AttachMat), &rj_mathlib::getXAxis(&dest), lerp);
 				D3DXVec3Normalize(&tempX, &tempX);
 
 				D3DXVec3Lerp(&tempY, &rj_mathlib::getYAxis(&AttachMat), &rj_mathlib::getYAxis(&dest), lerp);
 				D3DXVec3Normalize(&tempY, &tempY);
 
 				D3DXVec3Cross(&tempZ, &tempX, &tempY);
 				D3DXVec3Normalize(&tempZ, &tempZ);
 
 				rj_mathlib::setXAxis(&PrevMat, &tempX);
 				rj_mathlib::setYAxis(&PrevMat, &tempY);
 				rj_mathlib::setZAxis(&PrevMat, &tempZ);
// 
// 				PrevMat._14 = 0.0f;
// 				PrevMat._24 = 0.0f;
// 				PrevMat._34 = 0.0f;
// 				PrevMat._44 = 1.0f;
			}



			//D3DXVec3Lerp(&temp, &rj_mathlib::getZAxis(&PrevMat), &rj_mathlib::getZAxis(&dest), 0.0001f);
			//rj_mathlib::setZAxis(&PrevMat, &temp);

			

			//PrevMat = roty;

			D3DXVECTOR3 checkDist = rj_mathlib::getWAxis(&dest) - rj_mathlib::getWAxis(&PrevMat);

			float stopped = D3DXVec3Dot(&checkDist, &checkDist);
			if(SendDoneMess)
			{
				if(stopped < 0.0009765625 && LerpisDone == false)
				{
					volatile tCameraDone* tCAM = new tCameraDone(EM_CAMERADONEMOVING); 
					CEventManager::GetInstanceThreaded()->SendEventUpdateCrossToRender(ET_CAMERADONEMOVING,(tBaseEventData*)tCAM);
					tCAM = NULL;

					LerpisDone = true;
					Done = true;

				}
				else if(stopped > 0.0009765625)
				{
					LerpisDone = false;
					if(Done)
					{
						lerp = 0.0f;
					}
					Done = false;	

				}
			}
			//else if(stopped > 0.0009765625)
			//{
			//	//LerpisDone = false;
			//	if(Done)
			//		lerp = 0.0f;
			//	Done = false;	
			//}

			if(stopped < 0.0009765625 && LerpisDone == false)
			{
				volatile tCameraDone* tCAM2 = new tCameraDone(EM_CAMERADONEMOVING); 
				CEventManager::GetInstanceThreaded()->SendEventUpdateLocal(ET_CAMERADONEMOVING,(tBaseEventData*)tCAM2);
				tCAM2 = NULL;
				LerpisDone = true;
				Done = true;
			}
			else if(stopped >= 0.0009765625)
			{
				LerpisDone = false;
				if(Done)
				{
					lerp = 0.0f;
				}
				Done = false;	
			}
			else if(stopped < 0.0009765625 && LerpisDone)
			{
				LerpisDone = false;
			}
			//D3DXVec3Normalize(&temp, &temp);
			//PrevMat = *rj_mathlib::translatePost(&PrevMat, &(temp * (FLOAT)timeDist));
			//}
			//else
				//PrevMat = dest;
			OutMat = PrevMat;

		}
		else
		{
		//else if(dist > 9)
		//{
			lerp = 0;
			D3DXVec3Normalize(&temp, &temp);
			timeDist = dist * (fDelta * 0.25);	
			OutMat = *rj_mathlib::translatePost(&PrevMat, &(temp * (FLOAT)timeDist));
			//PrevMat = *rj_mathlib::translatePost(&PrevMat, &D3DXVECTOR3(0, 0, 0));
		}
		/*else if(dist > 2)
		{
			temp.y = -2;
			D3DXVec3Normalize(&temp, &temp);
			timeDist = dist * (fDelta * 0.25);	
			PrevMat = *rj_mathlib::translatePost(&PrevMat, &-(temp * (FLOAT)timeDist));
		}
		else
		{
			temp.y = -4;
			D3DXVec3Normalize(&temp, &temp);
			timeDist = dist * (fDelta * 0.25);	
			PrevMat = *rj_mathlib::translatePost(&PrevMat, &-(temp * (FLOAT)timeDist));
		}*/
		PrevMat = OutMat;
		

		SoftAttachQueue.pop();
	}
}
void CCamera::LookAt(D3DXMATRIX &OutMat, const D3DXMATRIX &Mat, const D3DXVECTOR3 &target, double fDelta)
{
	D3DXVECTOR3 AxisZ;
	OutMat = Mat;
	AxisZ = target - rj_mathlib::getWAxis(&OutMat);
	D3DXVec3Normalize(&AxisZ, &AxisZ);
	/*AxisZ.x = -AxisZ.x;
	AxisZ.y = -AxisZ.y;
	AxisZ.z = -AxisZ.z;*/
	D3DXVECTOR3 AxisX;
	D3DXVec3Cross(&AxisX, &D3DXVECTOR3(0, 1, 0), &AxisZ);
	D3DXVec3Normalize(&AxisX, &AxisX);
	D3DXVECTOR3 AxisY;
	D3DXVec3Cross(&AxisY, &AxisZ, &AxisX);
	D3DXVec3Normalize(&AxisY, &AxisY);
	rj_mathlib::setXAxis(&OutMat, &AxisX);
	rj_mathlib::setYAxis(&OutMat, &AxisY);
	rj_mathlib::setZAxis(&OutMat, &AxisZ);
	PrevLook = OutMat;

}
void CCamera::SendCameraEvent(void)
{
	if(m_fSentTimer > 0.002f)
	{
		Power = 0;//m_cGameplay->GetPower();
		Angle = 0;//m_cGameplay->GetAngle();
		Speed = 0;//m_cGameplay->GetSpeed();
		StoneAngle = 0;//m_cGameplay->GetStoneAngle();
		StoneForward = D3DXVECTOR3(0.0f, 0.0f,0.0f);//m_cGameplay->GetStoneForward();
		CurrPlayerScore = 0;//m_cGameplay->GetCurrPlayer()->getScore();
		volatile tCameraUpdate* tCAM = new tCameraUpdate(EM_CAMERA, m_mProjMat, GetViewMatrix(), m_mViewMat, Power, Angle, Speed, StoneAngle, StoneForward, CurrPlayerScore); 
		CEventManager::GetInstanceThreaded()->SendEventUpdateCrossToRender(ET_CAMERA,(tBaseEventData*)tCAM);
		tCAM = NULL;
		m_fSentTimer = 0.0000000f;
	}
}
// 0===========================================================================0
// | BuildViewFrustum();			     			                           |
// |  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  |
// | Author: Chad Vernon                                                       |
// | http://www.chadvernon.com/blog/resources/directx9/frustum-culling/        |
// | Last Edit: 2010	                                                       |
// |  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  |
// | In:		                                                               |
// | Out:		                                                               |
// |  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  |
// | Purpose: Builds the View Frustum                                          |
// 0===========================================================================0
void CCamera::BuildViewFrustum() 
{
	D3DXMATRIX viewProjection; 
	D3DXMatrixMultiply( &viewProjection, &m_mViewMat, &m_mProjMat ); 
	//D3DXMatrixMultiply( &viewProjection, &m_mView, &m_mProjection ); 
	// Left plane 
	m_frustum[0].a = viewProjection._14 + viewProjection._11; 
	m_frustum[0].b = viewProjection._24 + viewProjection._21; 
	m_frustum[0].c = viewProjection._34 + viewProjection._31; 
	m_frustum[0].d = viewProjection._44 + viewProjection._41;
	// Right plane 
	m_frustum[1].a = viewProjection._14 - viewProjection._11; 
	m_frustum[1].b = viewProjection._24 - viewProjection._21; 
	m_frustum[1].c = viewProjection._34 - viewProjection._31; 
	m_frustum[1].d = viewProjection._44 - viewProjection._41;

	// Top plane 
	m_frustum[2].a = viewProjection._14 - viewProjection._12; 
	m_frustum[2].b = viewProjection._24 - viewProjection._22; 
	m_frustum[2].c = viewProjection._34 - viewProjection._32; 
	m_frustum[2].d = viewProjection._44 - viewProjection._42;

	// Bottom plane 
	m_frustum[3].a = viewProjection._14 + viewProjection._12; 
	m_frustum[3].b = viewProjection._24 + viewProjection._22; 
	m_frustum[3].c = viewProjection._34 + viewProjection._32; 
	m_frustum[3].d = viewProjection._44 + viewProjection._42;

	// Near plane 
	m_frustum[4].a = viewProjection._13; 
	m_frustum[4].b = viewProjection._23; 
	m_frustum[4].c = viewProjection._33; 
	m_frustum[4].d = viewProjection._43;

	// Far plane 
	m_frustum[5].a = viewProjection._14 - viewProjection._13; 
	m_frustum[5].b = viewProjection._24 - viewProjection._23; 
	m_frustum[5].c = viewProjection._34 - viewProjection._33; 
	m_frustum[5].d = viewProjection._44 - viewProjection._43;

	// Normalize planes 
	for ( int i = 0; i < 6; i++ ) 
	{

		D3DXPlaneNormalize( &m_frustum[i], &m_frustum[i] );
	}

}

void CCamera::SetProjectionMatrix(D3DXMATRIX *_Matrix)
{
	memcpy(&m_mProjMat, _Matrix, sizeof(D3DXMATRIX));
}
void CCamera::BuildPerspective()
{
	D3DXMatrixPerspectiveFovLH(&m_mProjMat, D3DXToRadian(FieldofView), Width/Height, Near, Far);
}

const D3DXMATRIX CCamera::GetViewMatrix(bool _Translate)
{
	D3DXMATRIX _Rot(m_mViewMat);
	D3DXMATRIX _Trans;
	_Rot._41 = 0;
	_Rot._42 = 0;
	_Rot._43 = 0;
	D3DXMatrixTranspose(&_Rot, &_Rot); 

	D3DXMatrixIdentity(&_Trans);
	_Trans._41 = -m_mViewMat._41;
	_Trans._42 = -m_mViewMat._42;
	_Trans._43 = -m_mViewMat._43;
	
	_Trans = _Trans * _Rot;


	if(_Translate)
		return _Trans;
	else
		return _Rot;
}
void CCamera::SetViewMatrix(D3DXMATRIX *_Matrix)
{
	memcpy(&m_mViewMat, _Matrix, sizeof(D3DXMATRIX));


	m_mViewMat._11 = -m_mViewMat._11;
	m_mViewMat._12 = -m_mViewMat._12;
	m_mViewMat._13 = -m_mViewMat._13;
}

D3DXVECTOR3 CCamera::GetViewXAxis(void)
{
	return D3DXVECTOR3(m_mViewMat._11, m_mViewMat._12, m_mViewMat._13);
}
void CCamera::SetViewXAxis(D3DXVECTOR3 _Position)
{
	m_mViewMat._11 = _Position.x;
	m_mViewMat._12 = _Position.y;
	m_mViewMat._13 = _Position.z;
}
void CCamera::SetViewXAxis(float _x, float _y, float _z)
{
	m_mViewMat._11 = _x;
	m_mViewMat._12 = _y;
	m_mViewMat._13 = _z;
}

D3DXVECTOR3 CCamera::GetViewYAxis(void)
{
	return D3DXVECTOR3(m_mViewMat._21, m_mViewMat._22, m_mViewMat._23);
}
void CCamera::SetViewYAxis(D3DXVECTOR3 _Position)
{
	m_mViewMat._21 = _Position.x;
	m_mViewMat._22 = _Position.y;
	m_mViewMat._23 = _Position.z;
}
void CCamera::SetViewYAxis(float _x, float _y, float _z)
{
	m_mViewMat._21 = _x;
	m_mViewMat._22 = _y;
	m_mViewMat._23 = _z;
}

D3DXVECTOR3 CCamera::GetViewZAxis(void)
{
	return D3DXVECTOR3(m_mViewMat._31, m_mViewMat._32, m_mViewMat._33);
}
void CCamera::SetViewZAxis(D3DXVECTOR3 _Position)
{
	m_mViewMat._31 = _Position.x;
	m_mViewMat._32 = _Position.y;
	m_mViewMat._33 = _Position.z;
}
void CCamera::SetViewZAxis(float _x, float _y, float _z)
{
	m_mViewMat._31 = _x;
	m_mViewMat._32 = _y;
	m_mViewMat._33 = _z;
}

D3DXVECTOR3 CCamera::GetViewPosition(void)
{
	return D3DXVECTOR3(m_mViewMat._41, m_mViewMat._42, m_mViewMat._43);
}
void CCamera::SetViewPosition(D3DXVECTOR3 _Position)
{
	m_mViewMat._41 = _Position.x;
	m_mViewMat._42 = _Position.y;
	m_mViewMat._43 = _Position.z;
}
void CCamera::SetViewPosition(float _x, float _y, float _z)
{
	m_mViewMat._41 = _x;
	m_mViewMat._42 = _y;
	m_mViewMat._43 = _z;
}


void CCamera::ViewRotateLocalX(float _Angle)
{
	D3DXMATRIX _Rot;
	D3DXVECTOR3 _LocalX(m_mViewMat._11, m_mViewMat._12, m_mViewMat._13);
	D3DXMatrixRotationAxis(&_Rot, &_LocalX, _Angle);

	D3DXVECTOR4 _Pos(m_mViewMat._41, m_mViewMat._42, m_mViewMat._43, m_mViewMat._44);
	m_mViewMat._41 = 0;
	m_mViewMat._42 = 0;
	m_mViewMat._43 = 0;

	m_mViewMat = m_mViewMat * _Rot;
	m_mViewMat._41 = _Pos.x;
	m_mViewMat._42 = _Pos.y;
	m_mViewMat._43 = _Pos.z;
	m_mViewMat._44 = _Pos.w;
}
void CCamera::ViewRotateLocalY(float _Angle)
{
	D3DXMATRIX _Rot;
	D3DXVECTOR3 _LocalY(m_mViewMat._21, m_mViewMat._22, m_mViewMat._23);
	D3DXMatrixRotationAxis(&_Rot, &_LocalY, _Angle);

	D3DXVECTOR4 _Pos(m_mViewMat._41, m_mViewMat._42, m_mViewMat._43, m_mViewMat._44);
	m_mViewMat._41 = 0;
	m_mViewMat._42 = 0;
	m_mViewMat._43 = 0;

	m_mViewMat = m_mViewMat * _Rot;
	m_mViewMat._41 = _Pos.x;
	m_mViewMat._42 = _Pos.y;
	m_mViewMat._43 = _Pos.z;
	m_mViewMat._44 = _Pos.w;
}
void CCamera::ViewRotateLocalZ(float _Angle)
{
	D3DXMATRIX _Rot;
	D3DXVECTOR3 _LocalZ(m_mViewMat._31, m_mViewMat._32, m_mViewMat._33);
	D3DXMatrixRotationAxis(&_Rot, &_LocalZ, _Angle);

	D3DXVECTOR4 _Pos(m_mViewMat._41, m_mViewMat._42, m_mViewMat._43, m_mViewMat._44);
	m_mViewMat._41 = 0;
	m_mViewMat._42 = 0;
	m_mViewMat._43 = 0;

	m_mViewMat = m_mViewMat * _Rot;
	m_mViewMat._41 = _Pos.x;
	m_mViewMat._42 = _Pos.y;
	m_mViewMat._43 = _Pos.z;
	m_mViewMat._44 = _Pos.w;
}

void CCamera::ViewTranslateLocal(D3DXVECTOR3 _Axis, bool _CantRoll)
{
	if (!_CantRoll)
	{
		ViewTranslateLocalX(_Axis.x, _CantRoll);
		ViewTranslateLocalY(_Axis.y, _CantRoll);
		ViewTranslateLocalZ(_Axis.z, _CantRoll);
		return;
	}

	ViewTranslateLocalX(_Axis.x);
	ViewTranslateLocalY(_Axis.y);
	ViewTranslateLocalZ(_Axis.z);
}
void CCamera::ViewTranslateLocalX(float _Scale, bool _CantRoll)
{
	if (_CantRoll)
	{
		D3DXVECTOR3 _Trans(m_mViewMat._11, 0.0f, m_mViewMat._13);

		D3DXVec3Normalize(&_Trans, &_Trans);

		m_mViewMat._41 += _Trans.x * _Scale;
		m_mViewMat._42 += _Trans.y * _Scale;
		m_mViewMat._43 += _Trans.z * _Scale;
	}
	else
	{
		m_mViewMat._41 += m_mViewMat._11 * _Scale;
		m_mViewMat._42 += m_mViewMat._12 * _Scale;
		m_mViewMat._43 += m_mViewMat._13 * _Scale;
	}
}
void CCamera::ViewTranslateLocalY(float _Scale, bool _CantRoll)
{

	if (_CantRoll)
	{
		D3DXVECTOR3 _Trans(m_mViewMat._21, 0.0f, m_mViewMat._23);

		D3DXVec3Normalize(&_Trans, &_Trans);

		m_mViewMat._41 += _Trans.x * _Scale;
		m_mViewMat._42 += _Trans.y * _Scale;
		m_mViewMat._43 += _Trans.z * _Scale;
	}
	else
	{
		m_mViewMat._41 += m_mViewMat._21 * _Scale;
		m_mViewMat._42 += m_mViewMat._22 * _Scale;
		m_mViewMat._43 += m_mViewMat._23 * _Scale;
	}
}
void CCamera::ViewTranslateLocalZ(float _Scale, bool _CantRoll)
{

	if (_CantRoll)
	{
		D3DXVECTOR3 _Trans(m_mViewMat._31, 0.0f, m_mViewMat._33);

		D3DXVec3Normalize(&_Trans, &_Trans);

		m_mViewMat._41 += _Trans.x * _Scale;
		m_mViewMat._42 += _Trans.y * _Scale;
		m_mViewMat._43 += _Trans.z * _Scale;
	}
	else
	{
		m_mViewMat._41 += m_mViewMat._31 * _Scale;
		m_mViewMat._42 += m_mViewMat._32 * _Scale;
		m_mViewMat._43 += m_mViewMat._33 * _Scale;
	}
}


void CCamera::ViewRotateGlobalX(float _Angle)
{
	D3DXMATRIX _Rot;
	D3DXVECTOR4 _Pos(m_mViewMat._41, m_mViewMat._42, m_mViewMat._43, m_mViewMat._44);

	D3DXMatrixRotationX(&_Rot, _Angle);

	m_mViewMat._41 = 0;
	m_mViewMat._42 = 0;
	m_mViewMat._43 = 0;

	m_mViewMat = m_mViewMat * _Rot;
	m_mViewMat._41 = _Pos.x;
	m_mViewMat._42 = _Pos.y;
	m_mViewMat._43 = _Pos.z;
	m_mViewMat._44 = _Pos.w;
}
void CCamera::ViewRotateGlobalY(float _Angle)
{
	D3DXMATRIX _Rot;
	D3DXVECTOR4 _Pos(m_mViewMat._41, m_mViewMat._42, m_mViewMat._43, m_mViewMat._44);

	D3DXMatrixRotationY(&_Rot, _Angle);

	m_mViewMat._41 = 0;
	m_mViewMat._42 = 0;
	m_mViewMat._43 = 0;

	m_mViewMat = m_mViewMat * _Rot;
	m_mViewMat._41 = _Pos.x;
	m_mViewMat._42 = _Pos.y;
	m_mViewMat._43 = _Pos.z;
	m_mViewMat._44 = _Pos.w;
}
void CCamera::ViewRotateGlobalZ(float _Angle)
{
	D3DXMATRIX _Rot;
	D3DXVECTOR4 _Pos(m_mViewMat._41, m_mViewMat._42, m_mViewMat._43, m_mViewMat._44);

	D3DXMatrixRotationZ(&_Rot, _Angle);

	m_mViewMat._41 = 0;
	m_mViewMat._42 = 0;
	m_mViewMat._43 = 0;

	m_mViewMat = m_mViewMat * _Rot;
	m_mViewMat._41 = _Pos.x;
	m_mViewMat._42 = _Pos.y;
	m_mViewMat._43 = _Pos.z;
	m_mViewMat._44 = _Pos.w;
}

void CCamera::ViewTranslateGlobal(D3DXVECTOR3 _Axis)
{
	m_mViewMat._41 += _Axis.x;
	m_mViewMat._42 += _Axis.y;
	m_mViewMat._43 += _Axis.z;
}
void CCamera::ViewTranslateGlobalX(float _Scale)
{
	m_mViewMat._41 += _Scale;
}
void CCamera::ViewTranslateGlobalY(float _Scale)
{
	m_mViewMat._42 += _Scale;
}
void CCamera::ViewTranslateGlobalZ(float _Scale)
{
	m_mViewMat._43 += _Scale;
}
