/**
 * 0===========================================================================0
 * | HazardSphere.h                                                           |
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
 * |   Collision volume for hazard spheres, derives from IVolumeObject         |
 * 0===========================================================================0
 */

#ifndef  REDJAGUAR_CHAZARDSPHERE_H_		// Check to see if this .h is defined
#define  REDJAGUAR_CHAZARDSPHERE_H_		//  if not, define it

// 0===========================================================================0
// | ~ Includes ~                                                              |
// 0===========================================================================0
#include "../Base Classes/IVolumeObject.h"

class CHazardSphere: public IVolumeObject
{
	// Members
private:
	float m_fRadius;
	D3DXVECTOR3 m_vNormal;

	// Functions
private:
public:
	CHazardSphere(void);
	CHazardSphere(char* szName, D3DXVECTOR3 vPosition, float fRadius, float friction,
		MATERIAL_TYPE eMatType, unsigned int uiID, bool bActive = true);

	// Overloaded derived functions
	//	these are from IVolumeObject
	bool TestAgainstSphere(D3DXVECTOR3 p, float r, unsigned int id = 0);
	void OnCollision();
	float GetClosestRadius(D3DXVECTOR3 p);
	D3DXVECTOR3 GetClosestPoint(D3DXVECTOR3 p);
	D3DXVECTOR3 GetNormal(D3DXVECTOR3 position);
	float GetPlaneOffset(D3DXVECTOR3 normal) { return m_fRadius; }
	
	float GetRadius();
	void Render(double fDelta);
};

#endif //REDJAGUAR_CHAZARDSPHERE_H_