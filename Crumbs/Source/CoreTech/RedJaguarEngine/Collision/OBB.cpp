/**
 * 0===========================================================================0
 * | OBB.cpp	                                                           |
 * |  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  |
 * |   Author        : Andrew Gleeson                                          |
 * |   E-mail        : atgleeson@gmail.com                                     |
 * |  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  |
 * |   Date Created  : 11:29 am (Eastern) on 06/14/2010                        |
 * |   Date Edited   : 10:10 pm (Eastern) on 06/25/2010                        |
 * 0=====0===============================================================0=====0
*/

// 0===========================================================================0
// | ~ Includes ~                                                              |
// 0===========================================================================0
#include "OBB.h"
#include "../Renderer/RJ_Renderer.h"
#include "../Event/EventDefinitions.h"
#include "../Event/EventManager.h"

COBB::COBB(void) : IVolumeObject()
{
	m_vHalfWidth = D3DXVECTOR3();
	m_vQ = D3DXVECTOR3();
	m_vColor = D3DXVECTOR4(0.0f, 0.0f, 0.75f, 1.0f);
	for(int plane = 0; plane < 4; plane++)
	{
		planes[plane].vNormal = D3DXVECTOR3();
		planes[plane].vPosition = D3DXVECTOR3();
	}

	IObject::SetObjectType(RJ_Config::RJ_OBBVOLUME);
}
COBB::COBB(char* szName, D3DXVECTOR3 vPosition, D3DXVECTOR3 vAxis[3], D3DXVECTOR3 vHalfWidth, float fSpringValue,
					 MATERIAL_TYPE eMatType, unsigned int uiID, bool bActive)
					 : IVolumeObject(szName, uiID, vPosition, eMatType, RJ_Config::RJ_OBBVOLUME, fSpringValue)
{	
	for (unsigned int i = 0; i < 3; i++)
		m_vAxis[i]	= vAxis[i];

	m_vQ = D3DXVECTOR3();
	m_vHalfWidth = vHalfWidth;
	m_vColor = D3DXVECTOR4(0.0f, 0.0f, 0.75f, 1.0f);

	/*D3DXMATRIX matrix( 1.0f, 0.0f, 0.0f, 0.0f,
						0.0f, 1.0f, 0.0f, 0.0f,
						0.0f, 0.0f, 1.0f, 0.0f,
						vPosition.x, vPosition.y, vPosition.z, 1.0f );*/

	D3DXMATRIX matrix( vAxis[0].x, vAxis[0].y, vAxis[0].z, 0.0f,
						vAxis[1].x, vAxis[1].y, vAxis[1].z, 0.0f,
						vAxis[2].x, vAxis[2].y, vAxis[2].z, 0.0f,
						vPosition.x, vPosition.y, vPosition.z, 1.0f );
	SetWorldMat(matrix);

	D3DXVec3Normalize(&planes[0].vNormal, &vAxis[0]);
	planes[1].vNormal = -planes[0].vNormal;
	D3DXVec3Normalize(&planes[2].vNormal, &vAxis[2]);
	planes[3].vNormal = -planes[2].vNormal;
	
	D3DXVECTOR3 xAxisNorm;
	D3DXVec3Normalize(&xAxisNorm, &vAxis[0]);
	D3DXVECTOR3 zAxisNorm;
	D3DXVec3Normalize(&zAxisNorm, &vAxis[2]);
	planes[0].vPosition = vPosition + (zAxisNorm * vHalfWidth.z);
	planes[1].vPosition = vPosition + (zAxisNorm * -vHalfWidth.z);
	planes[2].vPosition = vPosition + (xAxisNorm * vHalfWidth.x);
	planes[3].vPosition = vPosition + (zAxisNorm * -vHalfWidth.x);
	planes[0].fDistance = m_vHalfWidth.x;
	planes[1].fDistance = m_vHalfWidth.x;
	planes[2].fDistance = m_vHalfWidth.z;
	planes[3].fDistance = m_vHalfWidth.z;

	//IObject::SetObjectType(RJ_Config::RJ_OBBVOLUME);
}

float COBB::GetClosestRadius(D3DXVECTOR3 p)
{
	D3DXVECTOR3 q = GetClosestPoint(p);

	/// <Sphere? SPHERE!>
	m_vQ = q;

	// return Squared Distance
	D3DXVECTOR3 vecRadius = D3DXVECTOR3(p - q);
	return D3DXVec3Length(&vecRadius);//D3DXVec3Dot(&vecRadius, &vecRadius);
}

bool COBB::TestAgainstSphere(D3DXVECTOR3 p, float r, unsigned int id)
{
	// Test against Squared Radius
	if ( GetClosestRadius(p) <= r )
	{
		m_vCollisionPoint = GetClosestPoint(p);
		OnCollision();
		m_vColor = D3DXVECTOR4(1.0f, 0.0f, 1.0f, 1.0f);
		return true;
	}

	m_vColor = D3DXVECTOR4(0.0f, 0.0f, 0.75f, 1.0f);
	return false;
}

void COBB::OnCollision()
{
	switch ( GetMaterialType() )
	{
	case BIT:
		{
			volatile tPlayEffect* tEFFECT = new tPlayEffect(EM_PLAYEFFECT, false, RJ_Config::RJ_COLLECTION, m_vCollisionPoint); 
			CEventManager::GetInstanceThreaded()->SendEventUpdateCrossToRender(ET_PLAYEFFECT,(tBaseEventData*)tEFFECT);
			tEFFECT = NULL;
		}
		break;
	case STONE:
		{
			volatile tPlayEffect* tEFFECT = new tPlayEffect(EM_PLAYEFFECT, false, RJ_Config::RJ_COLLISION, m_vCollisionPoint); 
			CEventManager::GetInstanceThreaded()->SendEventUpdateCrossToRender(ET_PLAYEFFECT,(tBaseEventData*)tEFFECT);
			tEFFECT = NULL;
		}
		break;
	case WALL:
		{
			volatile tPlayEffect* tEFFECT = new tPlayEffect(EM_PLAYEFFECT, false, RJ_Config::RJ_COLLISION, m_vCollisionPoint); 
			CEventManager::GetInstanceThreaded()->SendEventUpdateCrossToRender(ET_PLAYEFFECT,(tBaseEventData*)tEFFECT);
			tEFFECT = NULL;
		}
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

D3DXVECTOR3 COBB::GetClosestPoint(D3DXVECTOR3 p)
{
	/* code by: Christer Ericson
	*	Real-Time Collision Detection
	*	pg. 133
	*/

	/* begin "ClosestPtPointOBB()" */
	D3DXVECTOR3 q = GetPosition();
	D3DXVECTOR3 d = p - q;

	for (unsigned int i = 0; i < 3; i++)
	{
		float dist = D3DXVec3Dot( &d, &m_vAxis[i] );

		if (dist > m_vHalfWidth[i])
			dist = m_vHalfWidth[i];
		if (dist < -m_vHalfWidth[i])
			dist = -m_vHalfWidth[i];

		q += dist * m_vAxis[i];
	}

	return q;
	/* end "ClosestPtPointOBB()" */
}

void COBB::Render(double fDelta)
{
	D3DXMATRIX matrix = GetWorldMat();

	// build 8 verts
	ColorVertex verts[8];
	verts[0].position = D3DXVECTOR4(-m_vHalfWidth.x, m_vHalfWidth.y, m_vHalfWidth.z, 1.0f);	// TOP-BACK-LEFT
	verts[1].position = D3DXVECTOR4(m_vHalfWidth.x, m_vHalfWidth.y, m_vHalfWidth.z, 1.0f);	// TOP-BACK-RGHT
	verts[2].position = D3DXVECTOR4(m_vHalfWidth.x, m_vHalfWidth.y, -m_vHalfWidth.z, 1.0f);	// TOP-FRNT-RGHT
	verts[3].position = D3DXVECTOR4(-m_vHalfWidth.x, m_vHalfWidth.y, -m_vHalfWidth.z, 1.0f);// TOP-FRNT-LEFT
	verts[4].position = D3DXVECTOR4(-m_vHalfWidth.x, -m_vHalfWidth.y, m_vHalfWidth.z, 1.0f);
	verts[5].position = D3DXVECTOR4(m_vHalfWidth.x, -m_vHalfWidth.y, m_vHalfWidth.z, 1.0f);
	verts[6].position = D3DXVECTOR4(m_vHalfWidth.x, -m_vHalfWidth.y, -m_vHalfWidth.z, 1.0f);
	verts[7].position = D3DXVECTOR4(-m_vHalfWidth.x, -m_vHalfWidth.y, -m_vHalfWidth.z, 1.0f);

	ColorVertex Box[8];
	Box[0].position = D3DXVECTOR4(-m_vHalfWidth.x,-m_vHalfWidth.y,-m_vHalfWidth.z, 1.0f);
	Box[1].position = D3DXVECTOR4(-m_vHalfWidth.x,m_vHalfWidth.y,-m_vHalfWidth.z, 1.0f);
	Box[2].position = D3DXVECTOR4(m_vHalfWidth.x,m_vHalfWidth.y,-m_vHalfWidth.z, 1.0f);
	Box[3].position = D3DXVECTOR4(m_vHalfWidth.x,-m_vHalfWidth.y,-m_vHalfWidth.z, 1.0f);
	Box[4].position = D3DXVECTOR4(-m_vHalfWidth.x,-m_vHalfWidth.y,m_vHalfWidth.z, 1.0f);
	Box[5].position = D3DXVECTOR4(-m_vHalfWidth.x,m_vHalfWidth.y,m_vHalfWidth.z, 1.0f);
	Box[6].position = D3DXVECTOR4(m_vHalfWidth.x,m_vHalfWidth.y,m_vHalfWidth.z, 1.0f);
	Box[7].position = D3DXVECTOR4(m_vHalfWidth.x,-m_vHalfWidth.y,m_vHalfWidth.z, 1.0f);

	//D3DXMATRIX mat = GetWorldMat();
	RJ_RENDERER->DrawOOB(Box,&matrix);

	RJ_RENDERER->Render3DLine(planes[0].vPosition, planes[0].vPosition + (planes[0].vNormal*2.0f), D3DXVECTOR4(1.0f, 0.0f, 0.0f, 1.0f));
	RJ_RENDERER->Render3DLine(planes[1].vPosition, planes[1].vPosition + (planes[1].vNormal*2.0f), D3DXVECTOR4(0.0f, 1.0f, 0.0f, 1.0f));
	RJ_RENDERER->Render3DLine(planes[2].vPosition, planes[2].vPosition + (planes[2].vNormal*2.0f), D3DXVECTOR4(0.0f, 0.0f, 1.0f, 1.0f));
	RJ_RENDERER->Render3DLine(planes[3].vPosition, planes[3].vPosition + (planes[3].vNormal*2.0f), D3DXVECTOR4(1.0f, 1.0f, 0.0f, 1.0f));

	// Draw Lines
	//RJ_RENDERER->draw

	/// <Sphere? SPHERE!>
	D3DXMATRIX sphereMatrix;
	D3DXMatrixTranslation(&sphereMatrix, m_vQ.x, m_vQ.y, m_vQ.z);
	RJ_RENDERER->DrawSphere(&sphereMatrix, &m_vColor);
}

D3DXVECTOR3 COBB::GetNormal(D3DXVECTOR3 position)
{
	// inverse transform
	D3DXMATRIX inverse;
	D3DXMatrixInverse(&inverse, NULL, &GetWorldMat());

	D3DXVECTOR3 stonePos = rj_mathlib::vecMult3x3Mat(position, inverse.m);
	D3DXVECTOR3 OBBPos = rj_mathlib::vecMult3x3Mat(GetPosition(), inverse.m);

	if ( stonePos.x >= (OBBPos.x + m_vHalfWidth.x) )
		return planes[0].vNormal;
	if ( stonePos.z >= (OBBPos.z + m_vHalfWidth.z) )
		return planes[2].vNormal;
	if ( stonePos.x <= (OBBPos.x - m_vHalfWidth.x) )
		return planes[1].vNormal;
	
	// else // ( stonePos.z <= (OBBPos.z - m_vHalfWidth.z) )
	return planes[3].vNormal;
}

float COBB::GetPlaneOffset(D3DXVECTOR3 normal)
{
	for(int plane = 0; plane < 4; plane++)
	{
		if(planes[plane].vNormal == normal)
			return planes[plane].fDistance;
	}
	return 0.0f;
}