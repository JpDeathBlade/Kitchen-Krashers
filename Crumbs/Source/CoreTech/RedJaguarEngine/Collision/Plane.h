/**
 * 0===========================================================================0
 * | Plane.h                                                                   |
 * |  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  |
 * |   Author        : Andrew Gleeson                                          |
 * |   E-mail        : atgleeson@gmail.com                                     |
 * |  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  |
 * |   Date Created  : 10:06 pm (Eastern) on 07/01/2010                        |
 * |   Date Edited   : 10:06 pm (Eastern) on 07/01/2010                        |
 * 0=====0===============================================================0=====0
 *       |                                                               |      
 * 0=====0===============================================================0=====0
 * | Code Description:                                                         |
 * |  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  |           
 * |   Defines collision walls; "collision walls?" "I know!"                   |
 * 0===========================================================================0
 */

#ifndef  REDJAGUAR_PLANE_H_		// Check to see if this .h is defined
#define  REDJAGUAR_PLANE_H_		//  if not, define it

// 0===========================================================================0
// | ~ Includes ~                                                              |
// 0===========================================================================0
#include "../Base Classes/IVolumeObject.h"

class CPlane : public IVolumeObject
{
private:

	D3DXVECTOR3	m_vVerts[4];
	D3DXVECTOR3	m_vNormal;
	D3DXVECTOR3 m_vMax;
	D3DXVECTOR3 m_vMin;
	long m_lBitField;

public:

	CPlane();
	CPlane(char* name, D3DXVECTOR3 v0, D3DXVECTOR3 v1, D3DXVECTOR3 v2, D3DXVECTOR3 v3,  D3DXVECTOR3 n, float fV, MATERIAL_TYPE mt, unsigned int id);

	// Overloaded derived functions
	//	these are from IVolumeObject
	void Render(double fDelta);
	bool TestAgainstSphere(D3DXVECTOR3 p, float r, unsigned int id = 0);
	void OnCollision();
	float GetClosestRadius(D3DXVECTOR3 p);
	D3DXVECTOR3 GetClosestPoint(D3DXVECTOR3 p);
	D3DXVECTOR3 GetNormal(D3DXVECTOR3 position);
	float GetPlaneOffset(D3DXVECTOR3 normal) { return m_fDistance; }
};

#endif