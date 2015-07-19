/**
 * 0===========================================================================0
 * | IVolumeObject.h                                                           |
 * |  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  |
 * |   Author        : Andrew Gleeson                                          |
 * |   E-mail        : atgleeson@gmail.com                                     |
 * |  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  |
 * |   Date Created  : 10:20 am (Eastern) on 06/14/2010                        |
 * |   Date Edited   : 10:10 pm (Eastern) on 06/25/2010                        |
 * 0=====0===============================================================0=====0
 *       |                                                               |      
 * 0=====0===============================================================0=====0
 * | Code Description:                                                         |
 * |  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  |           
 * |   Interface for all collision volumes (solids, hazards)                   |
 * 0===========================================================================0
 */

#ifndef  REDJAGUAR_IVOLUMEOBJECT_H_		// Check to see if this .h is defined
#define  REDJAGUAR_IVOLUMEOBJECT_H_		//  if not, define it

// 0===========================================================================0
// | ~ Includes ~                                                              |
// 0===========================================================================0
#include "../../Utilities/RJ_MathLibrary.hpp"
#include "IObject.h"
#include "../../../RJ_Config.h"

enum MATERIAL_TYPE	{WALL = 0, STONE, BIT, HAZARD, POWERUP, GOAL, JELLO, MAX_M_TYPES};

class IVolumeObject : public IObject
{
	// Members
private:
	//D3DXVECTOR3 m_vPosition;
	MATERIAL_TYPE m_eMatType;
	float m_fValue;	//"SpringValue" or "Friction"
	RJ_Config::SOUND_FILE m_soCollide;

	// Functions
protected:
	D3DXVECTOR3 m_vCollisionPoint;

public:
	virtual ~IVolumeObject(void) = 0 {};

	IVolumeObject(void) : IObject(true, D3DXVECTOR3(1.0f, 1.0f, 1.0f), 0.0f, "volume", RJ_Config::RJ_SOLIDSPHEREVOLUME, true, 0)
	{
		m_eMatType = WALL;
		m_fValue = 0.0f;
		m_soCollide = RJ_Config::RJ_SOUND_NA;

		D3DXMATRIX matrix;
		D3DXMatrixIdentity(&matrix);
		SetWorldMat(matrix);
	}
	IVolumeObject(char* n, unsigned int id, D3DXVECTOR3 p, MATERIAL_TYPE mt, RJ_Config::RJ_Object_Types ot, float v, RJ_Config::SOUND_FILE soCollide = RJ_Config::RJ_SOUND_NA)
		: IObject(true, p, 0.0f, n, ot, true, id)
	{
		m_eMatType = mt;
		m_fValue = v;
		m_soCollide = soCollide;
	}


	float GetValue()					{ return m_fValue; }
	void SetValue(float v)				{ m_fValue = v; }
	MATERIAL_TYPE GetMaterialType()		{ return m_eMatType; }
	void SetMaterialType(MATERIAL_TYPE t){ m_eMatType = t; }
	RJ_Config::SOUND_FILE GetCollisionSound(){return m_soCollide;}
	void SetCollisionSound(RJ_Config::SOUND_FILE soCollide){m_soCollide = soCollide;}
	
	D3DXVECTOR3 GetPosition()			{ return getCenter(); }
	void SetPosition(D3DXVECTOR3 p)		{ SetCenter(p); }

	// force every IVO to be ableto collide with stones
	virtual bool TestAgainstSphere(D3DXVECTOR3 p, float r, unsigned int id = 0) = 0;

	// force every IVO to have a reaction 
	virtual void OnCollision() = 0;

	// force every IVO to have a radius calculation 
	virtual float GetClosestRadius(D3DXVECTOR3 p) = 0;

	// force every IVO to have a closest point calculation 
	virtual D3DXVECTOR3 GetClosestPoint(D3DXVECTOR3 p) = 0;

	// force every IVO to have a normal
	virtual D3DXVECTOR3 GetNormal(D3DXVECTOR3 position) = 0;

	// force every IVO to have a plane offset
	virtual float GetPlaneOffset(D3DXVECTOR3 normal) = 0;
};

#endif //REDJAGUAR_IVOLUMEOBJECT_H_