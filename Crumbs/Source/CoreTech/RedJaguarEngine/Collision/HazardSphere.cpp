/**
 * 0===========================================================================0
 * | HazardSphere.cpp                                                          |
 * |  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  |
 * |   Author        : Andrew Gleeson                                          |
 * |   E-mail        : atgleeson@gmail.com                                     |
 * |  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  |
 * |   Date Created  : 11:29 am (Eastern) on 06/14/2010                        |
 * |   Date Edited   : 10:10 pm (Eastern) on 06/25/2010                        |
 * 0=====0===============================================================0=====0
 */
#include "HazardSphere.h"
#include "../Renderer/RJ_Renderer.h"

CHazardSphere::CHazardSphere(void)
{
	m_fRadius = 0;
	m_vNormal = D3DXVECTOR3();
	IObject::SetObjectType(RJ_Config::RJ_HAZARDSPHEREVOLUME);	
}

CHazardSphere::CHazardSphere(char* szName, D3DXVECTOR3 vPosition, float fRadius, float fFriction,
							 MATERIAL_TYPE eMatType, unsigned int uiID, bool bActive)
							 : IVolumeObject(szName, uiID, vPosition, eMatType, RJ_Config::RJ_HAZARDSPHEREVOLUME, fFriction)
{
	m_fRadius = fRadius;
	m_vNormal = D3DXVECTOR3();

	D3DXMATRIX matrix;
	D3DXMatrixIdentity(&matrix);
	matrix._41 = vPosition.x;
	matrix._42 = vPosition.y;
	matrix._43 = vPosition.z;
	SetWorldMat(matrix);

	IObject::SetObjectType(RJ_Config::RJ_HAZARDSPHEREVOLUME);
}

bool CHazardSphere::TestAgainstSphere(D3DXVECTOR3 p, float r, unsigned int id)
{
	D3DXVECTOR3 length = p - GetPosition();

	if ( D3DXVec3LengthSq(&length) > (r + m_fRadius)*(r + m_fRadius) )
		return false;
	else
	{
		m_vCollisionPoint = GetClosestPoint(p);
		OnCollision();
		return true;
	}
}

void CHazardSphere::OnCollision()
{
	switch ( GetMaterialType() )
	{
	case BIT:
		//post event
		break;
	case HAZARD:
		{
			if( strcmp(IVolumeObject::m_szName, "CL_HZ_Water") == 0 )
			{
				volatile tPlayEffect* tEFFECT = new tPlayEffect(EM_PLAYEFFECT, false, RJ_Config::RJ_HAZARD_WATER, m_vCollisionPoint); 
				CEventManager::GetInstanceThreaded()->SendEventUpdateCrossToRender(ET_PLAYEFFECT,(tBaseEventData*)tEFFECT);
				tEFFECT = NULL;
			}
			else if( strcmp(IVolumeObject::m_szName, "CL_HZ_SinkWater") == 0)
			{
				volatile tPlayEffect* tEFFECT = new tPlayEffect(EM_PLAYEFFECT, false, RJ_Config::RJ_HAZARD_WATER, m_vCollisionPoint); 
				CEventManager::GetInstanceThreaded()->SendEventUpdateCrossToRender(ET_PLAYEFFECT,(tBaseEventData*)tEFFECT);
				tEFFECT = NULL;

				volatile tPlayEffect* tEFFECTW = new tPlayEffect(EM_PLAYEFFECT, false, RJ_Config::RJ_WAKE, m_vCollisionPoint); 
				CEventManager::GetInstanceThreaded()->SendEventUpdateCrossToRender(ET_PLAYEFFECT,(tBaseEventData*)tEFFECTW);
				tEFFECTW = NULL;
			}
			else if( strcmp(IVolumeObject::m_szName, "CL_HZ_LightSpill") == 0)
			{
				volatile tPlayEffect* tEFFECT = new tPlayEffect(EM_PLAYEFFECT, false, RJ_Config::RJ_HAZARD_ORANGE, m_vCollisionPoint); 
				CEventManager::GetInstanceThreaded()->SendEventUpdateCrossToRender(ET_PLAYEFFECT,(tBaseEventData*)tEFFECT);
				tEFFECT = NULL;
			}
			else if( strcmp(IVolumeObject::m_szName, "CL_HZ_DarkSpill") == 0)
			{
				volatile tPlayEffect* tEFFECT = new tPlayEffect(EM_PLAYEFFECT, false, RJ_Config::RJ_HAZARD_OIL, m_vCollisionPoint); 
				CEventManager::GetInstanceThreaded()->SendEventUpdateCrossToRender(ET_PLAYEFFECT,(tBaseEventData*)tEFFECT);
				tEFFECT = NULL;
			}
		}
		break;
	}
}

float CHazardSphere::GetClosestRadius(D3DXVECTOR3 p)
{
	return m_fRadius;
}

float CHazardSphere::GetRadius()
{
	return m_fRadius;
}

D3DXVECTOR3 CHazardSphere::GetClosestPoint(D3DXVECTOR3 p)
{
	D3DXVECTOR3 length = p - GetPosition();
	D3DXVECTOR3 newPos = GetPosition();

	newPos.x = min(length.x, m_fRadius);
	newPos.y = min(length.y, m_fRadius);
	newPos.z = min(length.z, m_fRadius);

	return newPos;
}

void CHazardSphere::Render(double fDelta)
{
	D3DXMATRIX matrix = GetWorldMat();
	RJ_RENDERER->DrawSphere( &matrix, &D3DXVECTOR4(0.0f, 1.0f, 0.0f, 1.0f) );
}

D3DXVECTOR3 CHazardSphere::GetNormal(D3DXVECTOR3 position)
{
	D3DXVec3Normalize(&m_vNormal, &(position - IVolumeObject::GetPosition()));
	return m_vNormal;
}