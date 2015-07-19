/**
 * 0===========================================================================0
 * | OBB.h				                                                   |
 * |  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  |
 * |   Author        : Andrew Gleeson                                          |
 * |   E-mail        : atgleeson@gmail.com                                     |
 * |  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  |
 * |   Date Created  : 11:29 am (Eastern) on 06/14/2010                        |
 * |   Date Edited   : 10:10 pm (Eastern) on 06/25/2010                        |
 * 0=====0===============================================================0=====0
 *       |                                                               |      
 * 0=====0===============================================================0=====0
 * | Code Description:                                                         |
 * |  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  |           
 * |   Collision volume for solid OBBs, derives from IVolumeObject             |
 * 0===========================================================================0
 */

#ifndef  REDJAGUAR_COBB_H_		// Check to see if this .h is defined
#define  REDJAGUAR_COBB_H_		//  if not, define it

// 0===========================================================================0
// | ~ Includes ~                                                              |
// 0===========================================================================0
#include "../Base Classes/IVolumeObject.h"

class COBB: public IVolumeObject
{
	// Members
private:
	D3DXVECTOR3 m_vHalfWidth;
	D3DXVECTOR3 m_vAxis[3];
	D3DXVECTOR3 m_vQ;
	D3DXVECTOR4 m_vColor;
	struct plane
	{
		D3DXVECTOR3 vPosition;
		D3DXVECTOR3 vNormal;
		float fDistance;
	};
	plane planes[4];

	// Functions
private:
public:
	COBB(void);
	COBB(char* szName, D3DXVECTOR3 vPosition, D3DXVECTOR3 vAxis[3], D3DXVECTOR3 vHalfWidth, float fSpringValue,
		MATERIAL_TYPE eMatType, unsigned int uiID, bool bActive = true);

	// Overloaded derived functions
	//	these are from IVolumeObject
	bool TestAgainstSphere(D3DXVECTOR3 p, float r, unsigned int id = 0);
	void OnCollision();
	float GetClosestRadius(D3DXVECTOR3 p);
	D3DXVECTOR3 GetClosestPoint(D3DXVECTOR3 p);
	D3DXVECTOR3 GetNormal(D3DXVECTOR3 position);
	float GetPlaneOffset(D3DXVECTOR3 normal);

	void Render(double fDelta);
};

#endif //REDJAGUAR_COBB_H_