/**
 * 0===========================================================================0
 * | Plane.cpp                                                                 |
 * |  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  |
 * |   Author        : Andrew Gleeson                                          |
 * |   E-mail        : atgleeson@gmail.com                                     |
 * |  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  |
 * |   Date Created  : 10:15 pm (Eastern) on 07/01/2010                        |
 * |   Date Edited   : 10:15 pm (Eastern) on 07/01/2010                        |
 * 0=====0===============================================================0=====0
 */

// 0===========================================================================0
// | ~ Includes ~                                                              |
// 0===========================================================================0
#include "Plane.h"
#include "../Renderer/RJ_Renderer.h"

CPlane::CPlane() : IVolumeObject()
{
	// unit-length @ origin
	m_vVerts[0] = D3DXVECTOR3(-0.5f, 0.5f, 0.0f);
	m_vVerts[1] = D3DXVECTOR3(0.5f, 0.5f, 0.0f);
	m_vVerts[2] = D3DXVECTOR3(0.5f, -0.5f, 0.0f);
	m_vVerts[3] = D3DXVECTOR3(-0.5f, -0.5f, 0.0f);

	m_vNormal = D3DXVECTOR3(0.0f, 0.0f, 1.0f);
	m_fDistance = 0;
	m_lBitField = 0;

	IVolumeObject::SetObjectType(RJ_Config::RJ_PLANEVOLUME);
}

CPlane::CPlane(char* name, D3DXVECTOR3 v0, D3DXVECTOR3 v1, D3DXVECTOR3 v2, D3DXVECTOR3 v3,  D3DXVECTOR3 n, float fV, MATERIAL_TYPE mt, unsigned int id)
	: IVolumeObject(name, id, D3DXVECTOR3((m_vMax - m_vMin)*0.5f), mt, RJ_Config::RJ_PLANEVOLUME, fV)
{
	m_vVerts[0] = v0;
	m_vVerts[1] = v1;
	m_vVerts[2] = v2;
	m_vVerts[3] = v3;
	m_vNormal = n;
	m_lBitField = 0;

	m_fDistance = D3DXVec3Dot(&m_vNormal, &m_vVerts[0]);

	// clamping
	m_vMax = m_vVerts[0];
	m_vMin = m_vVerts[0];

	for (unsigned int i = 0; i < 4; i++)
	{
		if ( m_vVerts[i].x > m_vMax.x )
			m_vMax.x = m_vVerts[i].x;
		else if ( m_vVerts[i].x < m_vMin.x )
			m_vMin.x = m_vVerts[i].x;

		if ( m_vVerts[i].y > m_vMax.y )
			m_vMax.y = m_vVerts[i].y;
		else if ( m_vVerts[i].y < m_vMin.y )
			m_vMin.y = m_vVerts[i].y;

		if ( m_vVerts[i].z > m_vMax.z )
			m_vMax.z = m_vVerts[i].z;
		else if ( m_vVerts[i].z < m_vMin.z )
			m_vMin.z = m_vVerts[i].z;
	}
}

void CPlane::Render(double fDelta)
{
	D3DXMATRIX matrix = GetWorldMat();

	D3DXVECTOR4 color = D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f);
	if ( strcmp(getName(),"CL_SM_IceCube") == 0 )
		color = D3DXVECTOR4(0.0f, 1.0f, 1.0f, 1.0f);

	// CW
	RJ_RENDERER->Render3DLine(m_vVerts[0], m_vVerts[1], color);
	RJ_RENDERER->Render3DLine(m_vVerts[1], m_vVerts[2], color);
	RJ_RENDERER->Render3DLine(m_vVerts[2], m_vVerts[3], color);
	RJ_RENDERER->Render3DLine(m_vVerts[3], m_vVerts[0], color);
	RJ_RENDERER->Render3DLine(m_vVerts[0], m_vVerts[2], color);
	RJ_RENDERER->Render3DLine(m_vVerts[1], m_vVerts[3], color);
	// Cheat Culling: CCW
	//RJ_RENDERER->Render3DLine(m_vVerts[0], m_vVerts[3], D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f));
	//RJ_RENDERER->Render3DLine(m_vVerts[3], m_vVerts[2], D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f));
	//RJ_RENDERER->Render3DLine(m_vVerts[2], m_vVerts[1], D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f));
	//RJ_RENDERER->Render3DLine(m_vVerts[1], m_vVerts[0], D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f));

	// Normal
	D3DXVECTOR3 center = (m_vMax + m_vMin) / 2;
	center.y += 1.0f;
	RJ_RENDERER->Render3DLine(center, center + (m_vNormal*2.0f), D3DXVECTOR4(1.0f, 0.0f, 0.0f, 1.0f));

	D3DXMATRIX mat;
	D3DXMatrixTranslation(&mat, m_vCollisionPoint.x, m_vCollisionPoint.y, m_vCollisionPoint.z);
	RJ_RENDERER->DrawSphere(&mat, &D3DXVECTOR4(0.0f, 0.0f, 1.0f, 1.0f));
}

bool CPlane::TestAgainstSphere(D3DXVECTOR3 p, float r, unsigned int id)
{
	//float dist = D3DXVec3Dot(&p, &m_vNormal) - m_fDistance;
	//	if ( dist < r )

	// TODO:
	//	find out if my direction == normal
	//	if (true) toss me out!
	
	D3DXVECTOR3 q = GetClosestPoint(p);
	D3DXVECTOR3 testVec = q - p;
	
	if ( D3DXVec3Dot(&testVec, &testVec) <= r*r )
	{
		if ( !(m_lBitField & (1 << id)) )
		{
			if ( D3DXVec3Dot(&m_vNormal, &testVec) < 0 )
			{
				OnCollision();
				return true;
			}
			else// turn Bit on
				m_lBitField |= (1 << id);
		}
	}
	else// turn Bit off
		m_lBitField &= ~(1 << id);

	return false;
}

void CPlane::OnCollision()
{
	switch ( GetMaterialType() )
	{
	case STONE:
	case WALL:
		{
			volatile tPlayEffect* tEFFECT = new tPlayEffect(EM_PLAYEFFECT, false, RJ_Config::RJ_COLLISION, m_vCollisionPoint); 
			CEventManager::GetInstanceThreaded()->SendEventUpdateCrossToRender(ET_PLAYEFFECT,(tBaseEventData*)tEFFECT);
			tEFFECT = NULL;
		}
		break;

	case JELLO:
		{
			volatile tPlayEffect* tEFFECT = new tPlayEffect(EM_PLAYEFFECT, false, RJ_Config::RJ_JELLO, m_vCollisionPoint); 
			CEventManager::GetInstanceThreaded()->SendEventUpdateCrossToRender(ET_PLAYEFFECT,(tBaseEventData*)tEFFECT);
			tEFFECT = NULL;

			//string nameBuffer(const_cast<char*>(getName())+3);
			char* nameBuffer = new char[64];
			strcpy_s(nameBuffer,64,const_cast<char*>(getName())+3);

			volatile tJelloWiggle* tJELLO = new tJelloWiggle(EM_JELLOWIGGLE, nameBuffer); 
			CEventManager::GetInstanceThreaded()->SendEventUpdateCrossToRender(ET_JELLOWIGGLE,(tBaseEventData*)tJELLO);
			tJELLO = NULL;
		}
		break;
	}
}

float CPlane::GetClosestRadius(D3DXVECTOR3 p)
{
	// don't use this
	return 0.0f;
}

D3DXVECTOR3 CPlane::GetClosestPoint(D3DXVECTOR3 p)
{
	/* code by: Christer Ericson
	*	Real-Time Collision Detection
	*	pg. 126
	*/

	/* begin "ClosestPtPointPlane()" */
	float t = (D3DXVec3Dot(&m_vNormal, &p) - m_fDistance) / D3DXVec3Dot(&m_vNormal, &m_vNormal);
	m_vCollisionPoint = p - t * m_vNormal;
	/* end "ClosestPtPointPlane()" */

	if ( m_vCollisionPoint.x > m_vMax.x )
		m_vCollisionPoint.x = m_vMax.x;
	else if ( m_vCollisionPoint.x < m_vMin.x )
		m_vCollisionPoint.x = m_vMin.x;
	if ( m_vCollisionPoint.y > m_vMax.y )
		m_vCollisionPoint.y = m_vMax.y;
	else if ( m_vCollisionPoint.y < m_vMin.y )
		m_vCollisionPoint.y = m_vMin.y;
	if ( m_vCollisionPoint.z > m_vMax.z )
		m_vCollisionPoint.z = m_vMax.z;
	else if ( m_vCollisionPoint.z < m_vMin.z )
		m_vCollisionPoint.z = m_vMin.z;

	return m_vCollisionPoint;
}

D3DXVECTOR3 CPlane::GetNormal(D3DXVECTOR3 position)
{
	return m_vNormal;
}