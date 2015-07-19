/**
* 0===========================================================================0
* | CollisionMachine.cpp                                                      |
* |  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  |
* |   Author        : Andrew Gleeson                                          |
* |   E-mail        : atgleeson@gmail.com                                     |
* |  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  |
* |   Date Created  : 01:23 pm (Eastern) on 06/14/2010                        |
* |   Date Edited   : 10:10 pm (Eastern) on 06/25/2010                        |
* 0=====0===============================================================0=====0
*/

// 0===========================================================================0
// | ~ Includes ~                                                              |
// 0===========================================================================0
#include "CollisionMachine.h"
#include "SolidSphere.h"
#include "OBB.h"
#include "HazardSphere.h"
#include "../../../Gameplay/Objects/PowerUp.h"

#include "../Base Classes/IVolumeObject.h"
#include "../../../Gameplay/Objects/CStone.h"
#include "../../../Gameplay/Objects/CPlayer.h"
#include "../Event/EventDefinitions.h"
#include "../Event/EventManager.h"
#include "plane.h"

#include <fstream>

#define STATIC_FRICTION_MAX 5.0f
#define STATIC_FRICTION_MIN 1.0f

CCollisionMachine::CCollisionMachine(char* szFileName)
{
	m_vSolids.clear();
	m_vHazards.clear();
	m_vStones.clear();
	m_fWasColliding = false;
	m_bLoopCollide = false;
	LoadData(szFileName);
	CEventManager::GetInstanceThreaded()->RegisterListenerUpdate(ET_TOGGLEALLBITS,this);
	CEventManager::GetInstanceThreaded()->RegisterListenerUpdate(ET_TOGGLEALLPOW,this);
}

CCollisionMachine::CCollisionMachine(void)
{
	m_vSolids.clear();
	m_vHazards.clear();
	m_vStones.clear();
	m_fWasColliding = false;
	m_bLoopCollide = false;
	CEventManager::GetInstanceThreaded()->RegisterListenerUpdate(ET_TOGGLEALLBITS,this);
	CEventManager::GetInstanceThreaded()->RegisterListenerUpdate(ET_TOGGLEALLPOW,this);
}

CCollisionMachine::~CCollisionMachine(void)
{
	UnloadData();
}

bool CCollisionMachine::LoadData(const char* szFileName)
{
	UnloadData();
	CEventManager::GetInstanceThreaded()->RegisterListenerUpdate(ET_TOGGLEALLBITS,this);
	CEventManager::GetInstanceThreaded()->RegisterListenerUpdate(ET_TOGGLEALLPOW,this);
	//m_vSolids.clear();
	//m_vHazards.clear();

	// Go ahead and make every variable,
	// for Speeeeed!
	char* matName;
	unsigned int nCount, matNameLength;
	int volumeType, matID;
	float x_axis[4], y_axis[4], z_axis[4], w_axis[4];
	float radius, x_width, y_width, z_width;
	float tmpFloat;
	D3DXVECTOR3 verts[4];
	D3DXVECTOR3 normal;

	// parse file
	std::fstream file;
	file.open(szFileName, std::ios_base::in | std::ios_base::binary);

	/// <number of Items>
	file.read((char*)&nCount, sizeof(unsigned int));

	for (unsigned int i = 0; i < nCount; i++)
	{
		/// <VolumeType (int)>
		file.read((char*)&volumeType, sizeof(volumeType));
		/// <length of MaterialName + '0' (unsigned int)>
		file.read((char*)&matNameLength, sizeof(matNameLength));
		matName = new char[matNameLength];
		/// <MaterialName + '0' (char*)>
		file.read(matName, sizeof(char) * matNameLength);
		/// <MatID (int)>
		file.read((char*)&matID, sizeof(matID));
		/// <xAxis (xyzw) (4 x double)>
		file.read((char*)&tmpFloat, sizeof(tmpFloat));
		x_axis[0] = tmpFloat;
		tmpFloat = 0;
		file.read((char*)&tmpFloat, sizeof(tmpFloat));
		x_axis[1] = tmpFloat;
		file.read((char*)&tmpFloat, sizeof(tmpFloat));
		x_axis[2] = tmpFloat;
		file.read((char*)&tmpFloat, sizeof(tmpFloat));
		x_axis[3] = tmpFloat;
		/// <yAxis (xyzw) (4 x double)>
		file.read((char*)&tmpFloat, sizeof(tmpFloat));
		y_axis[0] = -tmpFloat;
		file.read((char*)&tmpFloat, sizeof(tmpFloat));
		y_axis[1] = -tmpFloat;
		file.read((char*)&tmpFloat, sizeof(tmpFloat));
		y_axis[2] = -tmpFloat;
		file.read((char*)&tmpFloat, sizeof(tmpFloat));
		y_axis[3] = -tmpFloat;
		/// <zAxis (xyzw) (4 x double)>
		file.read((char*)&tmpFloat, sizeof(tmpFloat));
		z_axis[0] = -tmpFloat;
		file.read((char*)&tmpFloat, sizeof(tmpFloat));
		z_axis[1] = -tmpFloat;
		file.read((char*)&tmpFloat, sizeof(tmpFloat));
		z_axis[2] = -tmpFloat;
		file.read((char*)&tmpFloat, sizeof(tmpFloat));
		z_axis[3] = -tmpFloat;
		/// <wAxis (xyzw) (4 x double)>
		file.read((char*)&tmpFloat, sizeof(tmpFloat));
		w_axis[0] = tmpFloat;
		file.read((char*)&tmpFloat, sizeof(tmpFloat));
		w_axis[1] = tmpFloat;
		file.read((char*)&tmpFloat, sizeof(tmpFloat));
		w_axis[2] = tmpFloat;
		file.read((char*)&tmpFloat, sizeof(tmpFloat));
		w_axis[3] = tmpFloat;

		RJ_Config::SOUND_FILE sfCollisionSound = LookUpCollisionSound(matName);

		if ( volumeType == 1 )		// SolidSphere
		{
			/// <Radius (float)>
			file.read((char*)&radius, sizeof(radius));

			// create the new Sphere volume
			D3DXVECTOR3 position(w_axis[0], w_axis[1], w_axis[2]);

			MATERIAL_TYPE eType = WALL;
			POWERUP_TYPE ePOW = BUTTER;
			if( strcmp(matName, "DM_SugarBit") == 0 )
				eType = BIT;
			else if ( strcmp(matName, "Goal") == 0 )
			{
				eType = GOAL;
				volatile tGoalPosition* tGE = new tGoalPosition(EM_GOALPOS,position);
				CEventManager::GetInstanceThreaded()->SendEventRenderLocal(ET_GOALPOS,(tBaseEventData*)tGE);
				tGE = NULL;
			}
			else if ( strcmp(matName, "DM_Gum") == 0 )
			{
				eType = POWERUP;
				ePOW = GUM;
			}
			else if ( strcmp(matName, "DM_Cherries") == 0 )
			{
				eType = POWERUP;
				ePOW = CHERRIES;
			}
			else if ( strcmp(matName, "DM_Whipcream") == 0 )
			{
				eType = POWERUP;
				ePOW = WHIP_CREAM;
			}
			else if ( strcmp(matName, "DM_Fortune_Cookie") == 0 )
			{
				eType = POWERUP;
				ePOW = FORTUNE_COOKIE;
			}
			else if ( strcmp(matName, "DM_ColdTurkey") == 0 )
			{
				eType = POWERUP;
				ePOW = TURKEY;
			}

			if (eType == POWERUP)
			{
				CPowerUp* newItem = new CPowerUp( matName, position, ePOW, radius, 1.0f, eType, matID);
				m_vSolids.push_back(newItem);
			}
			else
			{
				CSolidSphere* newItem = new CSolidSphere( matName, position, radius, 1.0f, eType, matID );
				newItem->SetCollisionSound(sfCollisionSound);

				// HACK for goal
				if (eType == GOAL)
					m_pGoal = newItem;
				else
					m_vSolids.push_back(newItem);
			}
		}
		else if (volumeType == 2)	// Plane
		{
			/// <x></x><y></y><z></z>
			/// <x></x><y></y><z></z>
			/// <x></x><y></y><z></z>
			/// <x></x><y></y><z></z>
			for (unsigned int _vert = 0; _vert < 4; _vert++)
			{
				file.read((char*)&verts[_vert].x, sizeof(verts[_vert].x));
				file.read((char*)&verts[_vert].y, sizeof(verts[_vert].x));
				file.read((char*)&verts[_vert].z, sizeof(verts[_vert].x));
			}

			/// <Normal> <x></x><y></y><z></z> </Normal>
			file.read((char*)&normal.x, sizeof(normal.x));
			file.read((char*)&normal.y, sizeof(normal.x));
			file.read((char*)&normal.z, sizeof(normal.x));

			CPlane* newItem = new CPlane( matName, verts[0], verts[1], verts[2], verts[3], normal, 1.0f, WALL, matID );
			newItem->SetCollisionSound(sfCollisionSound);
			
			char nameBuffer[64];
			strncpy_s(nameBuffer,64,matName+10,7);
			nameBuffer[7] = '\0';

			if( strcmp(nameBuffer, "Gelatin") == 0)
			{
				newItem->SetMaterialType( JELLO ); 
				newItem->SetValue(1.2f);
			}
			else if( strcmp(matName, "CL_SM_PizzaBox") == 0 )
			{
				newItem->SetValue(0.9f);
			}
			else if( strcmp(matName, "CL_SM_Fish") == 0 )
			{
				newItem->SetValue(0.9f);
			}
			else if( strcmp(matName, "CL_SM_BagOfPeas") == 0 )
			{
				newItem->SetValue(0.9f);
			}
			m_vSolids.push_back(newItem);
		}
		else if (volumeType == 3)	// HazardSphere
		{
			float friction = 0.0f;
			if( strcmp(matName, "CL_HZ_Water") == 0 )
				friction = 2.0f;
			else if( strcmp(matName, "CL_HZ_SinkWater") == 0)
				friction = 2.0f;
			else if( strcmp(matName, "CL_HZ_LightSpill") == 0)
				friction = -3.0f;
			else if( strcmp(matName, "CL_HZ_DarkSpill") == 0)
				friction = 5.0f;
			else if( strcmp(matName, "CL_HZ_Ice") == 0)
				friction = -1.0f;
			else if( strcmp(matName, "CL_HZ_Sticky") == 0)
				friction = 5.0f;
			else if( strcmp(matName, "CL_HZ_StickyP") == 0)
				friction = 5.0f;

			/// <Radius (float)>
			file.read((char*)&radius, sizeof(radius));

			// create the new Sphere volume
			D3DXVECTOR3 position = D3DXVECTOR3(w_axis[0], w_axis[1], w_axis[2]);
			CHazardSphere* newItem = new CHazardSphere( matName, position, radius, friction, HAZARD, matID);
			newItem->SetCollisionSound(sfCollisionSound);

			m_vHazards.push_back(newItem);
		}
		else if (volumeType == 4)	// HazardOBB
		{
			float friction = 0.0f;
			if( strcmp(matName, "CL_HZ_Water") == 0 )
				friction = 2.0f;
			else if( strcmp(matName, "CL_HZ_SinkWater") == 0)
				friction = 2.0f;
			else if( strcmp(matName, "CL_HZ_LightSpill") == 0)
				friction = -3.0f;
			else if( strcmp(matName, "CL_HZ_DarkSpill") == 0)
				friction = 5.0f;
			else if( strcmp(matName, "CL_HZ_Ice") == 0)
				friction = -1.0f;
			else if( strcmp(matName, "CL_HZ_Sticky") == 0)
				friction = 5.0f;
			else if( strcmp(matName, "CL_HZ_StickyP") == 0)
				friction = 5.0f;

			// <Widths (xyz) (3 x float)>
			file.read((char*)&x_width, sizeof(x_width));
			file.read((char*)&y_width, sizeof(y_width));
			file.read((char*)&z_width, sizeof(z_width));

			D3DXVECTOR3 position(w_axis[0], w_axis[1], w_axis[2]);

			D3DXVECTOR3 axis[3];
			axis[0] = D3DXVECTOR3(x_axis[0], x_axis[1], x_axis[2]);
			axis[1] = D3DXVECTOR3(y_axis[0], y_axis[1], y_axis[2]);
			axis[2] = D3DXVECTOR3(z_axis[0], z_axis[1], z_axis[2]);

			// create the new OBB volume
			D3DXVECTOR3 halfWidths = D3DXVECTOR3(x_width, y_width, z_width);
			COBB* newItem = new COBB( matName, position, axis, halfWidths, friction, HAZARD, matID );
			newItem->SetCollisionSound(sfCollisionSound);
	
			m_vHazards.push_back(newItem);
		}

		delete matName;
	}

	file.close();

	return true;
}

bool CCollisionMachine::TestCollision(CStone* obj)
{
	// don't test stones that haven't been launched
	if ( !obj->GetIsLaunched() )
		return false;

	bool bReturn = false;
	m_fWasColliding = false;

	if(!m_bLoopCollide)
	{
		if ( CheckHazards(obj) )
			bReturn = false;
	}

	// for averaging
	D3DXVECTOR3 normal = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

	//	check Stones
	for (unsigned int stone = 0; stone < m_vStones.size(); stone++)
	{
		if ( m_vStones[stone]->GetIsLaunched() && !m_vStones[stone]->getFoul() &&
			m_vStones[stone]->GetID() != obj->GetID() && m_vStones[stone]->GetIsActive() )
		{
			if ( m_vStones[stone]->GetVolume().TestAgainstSphere(obj->getPosition(), obj->GetVolume().GetRadius()) )
			{
				if(m_vStones[stone]->GetOwner() == obj->GetOwner())
				{
					obj->SetNumStoneHits(1);
				}

				if(!(m_vStones[stone]->GetGum() && m_vStones[stone]->getSpeed() <= 0.0f))
				{
					D3DXVECTOR3 direction = m_vStones[stone]->getPosition() - obj->getPosition();
					D3DXVec3Normalize(&direction, &direction);
					float angle = atan2f(direction.z, direction.x)/D3DX_PI;
					bool headOn = false;
					if(fabsf(angle - obj->getAngle()) <= 0.01f)
						headOn = true;
					///////////////// GET CURRENT VELOCITIES /////////////////////
					D3DXVECTOR3 velocity1(cos(obj->getAngle() * D3DX_PI), 0.0f, sin(obj->getAngle() * D3DX_PI));
					velocity1 *= obj->getSpeed();
					
					D3DXVECTOR3 velocity2(cos(m_vStones[stone]->getAngle() * D3DX_PI), 0.0f, sin(m_vStones[stone]->getAngle() * D3DX_PI));
					velocity2 *= m_vStones[stone]->getSpeed();

					/////////// GET NECESSARY VALUES (FOR SIMPLICITY) ////////////
					D3DXVECTOR3 position1 = obj->getPosition();
					D3DXVECTOR3 position2 = m_vStones[stone]->getPosition();
					float mass1 = obj->getMass();
					float mass2 = m_vStones[stone]->getMass();
					float radius1 = obj->GetVolume().GetRadius();
					float radius2 = m_vStones[stone]->GetVolume().GetRadius();
					float d, Yxz, alpha, Yv, a, deltaVx2;//, t;
					D3DXVECTOR3 finalVelocity1, finalVelocity2;

					d = sqrtf((position2.x - position1.x)*(position2.x - position1.x) + (position2.z - position1.z)*(position2.z - position1.z));
					Yxz = atan2f((position2.z - position1.z), (position2.x - position1.x));
					Yv = atan2f((velocity1.z - velocity2.z), (velocity1.x - velocity2.x));
					alpha = asinf((d * sinf(Yxz - Yv)) / (radius1 + radius2));
					a = tanf(Yv + alpha);

					deltaVx2 = 2.0f * (velocity1.x - velocity2.x + a * (velocity1.z - velocity2.z)) / ((1 + a * a) * (1 + mass2/mass1));

					///////////////// CALCULATE NEW VELOCITIES ///////////////////
					finalVelocity2.x = (velocity2.x + deltaVx2) * obj->GetVolume().GetValue();
					finalVelocity2.y = 0.0f;
					finalVelocity2.z = (velocity2.z + a * deltaVx2) * obj->GetVolume().GetValue();
					finalVelocity1.x = (velocity1.x - (mass2/mass1) * deltaVx2) * m_vStones[stone]->GetVolume().GetValue();
					finalVelocity1.y = 0.0f;
					finalVelocity1.z = (velocity1.z - a * (mass2/mass1) * deltaVx2) * m_vStones[stone]->GetVolume().GetValue();

					if(headOn)
					{
						finalVelocity1.x += finalVelocity2.x * 0.5f;
						finalVelocity1.z += finalVelocity2.z * 0.5f;
					}

					///////////////////// CAP VELOCITIES /////////////////////////
					if(finalVelocity2.x > 7.0f)
						finalVelocity2.x = 7.0f;
					if(finalVelocity2.y > 7.0f)
						finalVelocity2.y = 7.0f;
					if(finalVelocity1.x > 7.0f)
						finalVelocity1.x = 7.0f;
					if(finalVelocity1.y > 7.0f)
						finalVelocity1.y = 7.0f;

					if(finalVelocity2.x < -7.0f)
						finalVelocity2.x = -7.0f;
					if(finalVelocity2.y < -7.0f)
						finalVelocity2.y = -7.0f;
					if(finalVelocity1.x < -7.0f)
						finalVelocity1.x = -7.0f;
					if(finalVelocity1.y < -7.0f)
						finalVelocity1.y = -7.0f;

					////////////////// PUSH THE STONES AWAY //////////////////////
					float distance = (position1.x - position2.x)*(position1.x - position2.x) + (position1.z - position2.z)*(position1.z - position2.z);
					if(fabs(finalVelocity1.x) > 0.001f || fabs(finalVelocity1.z) > 0.001f || fabs(finalVelocity2.x) > 0.001f || fabs(finalVelocity2.z) > 0.001f)
					{
						while(distance < (radius1 + radius2))
						{
							position1.x = position1.x + finalVelocity1.x * 0.001f;
							position1.y = 0.0f;
							position1.z = position1.z + finalVelocity1.z * 0.001f;
							position2.x = position2.x + finalVelocity2.x * 0.001f;
							position2.y = 0.0f;
							position2.z = position2.z + finalVelocity2.z * 0.001f;

							distance = (position1.x - position2.x)*(position1.x - position2.x) + (position1.z - position2.z)*(position1.z - position2.z);
						}
					}
					obj->setPosition(position1);
					m_vStones[stone]->setPosition(position2);
					
					///////////////// ADJUST NEW SPEEDS/ANGLES ///////////////////
					D3DXVECTOR3 normalizeVelocity1, normalizeVelocity2;
					D3DXVec3Normalize(&normalizeVelocity1, &finalVelocity1);
					D3DXVec3Normalize(&normalizeVelocity2, &finalVelocity2);
					
					obj->setAngle(atan2f(normalizeVelocity1.z, normalizeVelocity1.x)/D3DX_PI);
					m_vStones[stone]->setAngle(atan2f(normalizeVelocity2.z, normalizeVelocity2.x)/D3DX_PI);

					if(cosf(obj->getAngle() * D3DX_PI) == 0.0f)
						obj->setAngle(obj->getAngle() + 0.0001f);
					obj->setSpeed(finalVelocity1.x / (cosf(obj->getAngle() * D3DX_PI)));
					
					if(cosf(m_vStones[stone]->getAngle() * D3DX_PI) == 0.0f)
						m_vStones[stone]->setAngle(m_vStones[stone]->getAngle() + 0.0001f);
					m_vStones[stone]->setSpeed(finalVelocity2.x / (cosf(m_vStones[stone]->getAngle() * D3DX_PI)));

					if (m_vStones[stone]->getSpeed() <= 2 && obj->GetIsCurrentStone())
					{
						obj->SetNumStoneHits(2);
					}
				}
				else
				{
					m_fWasColliding = false;
					React( obj, &m_vStones[stone]->GetVolume(), normal );
					m_fWasColliding = false;
				}
				//React( m_vStones[stone], &obj->GetVolume(), normal );
				if(!(m_vStones[stone]->GetGum() && m_vStones[stone]->getSpeed() <= 0.0f))
					m_vStones[stone]->setIsMoving(true);
				else
					m_vStones[stone]->SetGumCollision(m_vStones[stone]->GetGumCollision()+1);

				// if we hit a stone don't check anything else
				// return true;
				/// <actually, that's a bad idea>
			}
		}
	}

	int nIndex = CheckSolids(obj, normal);
	int nVolSpeed = EVEN;
	obj->setGoalMultiplier(1);
	if (nIndex > -1)
	{
		bReturn = true;
		React(obj, m_vSolids[nIndex], normal);

		if ( m_vSolids[nIndex]->GetValue() > 1.0f )
			nVolSpeed = PLUS;
		else if ( m_vSolids[nIndex]->GetValue() < 1.0f )
			nVolSpeed = MINUS;
	}
	obj->SetCurrentVolumeSpeed(nVolSpeed);

	if(!m_bLoopCollide)
	{
		// Test GOAL
		if ( m_pGoal->TestAgainstSphere(obj->getPosition(), obj->GetVolume().GetRadius()) )
		{
			// react to the goal
			React(obj, m_pGoal, normal);
		}
	}

	if(nIndex > -1)
		m_fWasColliding = bReturn;

	return bReturn;
}

int CCollisionMachine::CheckSolids(CStone* obj, D3DXVECTOR3& _n)
{
	int nHitIndex = -1;
	float fObjRad = obj->GetVolume().GetRadius();
	float fDistance = fObjRad;

	D3DXVECTOR3 vObjPos = obj->getPosition();
	unsigned int nHitCount = 0;

	for( unsigned int i = 0; i < m_vSolids.size(); i++)
	{
		if ( m_vSolids[i]->GetIsActive() )
		{
			if ( m_vSolids[i]->TestAgainstSphere(vObjPos, fObjRad) )
			{
				_n += m_vSolids[i]->GetNormal(vObjPos);
				nHitCount++;

				// Return first volume hit in World-Space
				D3DXVECTOR3 length = vObjPos - m_vSolids[i]->GetClosestPoint(vObjPos);
				float tempDist = D3DXVec3Length(&length);

				if ( tempDist < (fDistance*fDistance) )//&& m_vSolids[i]->GetMaterialType() != GOAL)
				{
					fDistance = tempDist;
					nHitIndex = i;
				}
			}
		}
	}

	//_n /= (float)nHitCount;
	/// <is the divide causing errors?>
	/// <just normalize() instead>
	D3DXVec3Normalize(&_n, &_n);
	return nHitIndex;
}

bool CCollisionMachine::CheckHazards(CStone* obj)
{
	// this will set the friction values in the stone,
	//	don't reset them

	bool bCollided = false;

	float fPtRad = obj->GetVolume().GetRadius() / 3;
	D3DXVECTOR3 vObjPos = obj->getPosition();

	D3DXVECTOR3 vPtTop = vObjPos + D3DXVECTOR3(0, 0, fPtRad);
	D3DXVECTOR3 vPtLeft = vObjPos + D3DXVECTOR3(-fPtRad, 0, -fPtRad);
	D3DXVECTOR3 vPtRight = vObjPos + D3DXVECTOR3(fPtRad, 0, -fPtRad);

	D3DXVECTOR3 newFriction(0.0f, 0.0f, 0.0f);

	unsigned int nLastHit = 0;
	for( unsigned int i = 0; i < m_vHazards.size(); i++)
	{
		// test camera
		if ( strcmp(m_vHazards[i]->getName(), "CL_SM_World") == 0 )
		{
			if ( m_vHazards[i]->TestAgainstSphere(vObjPos, obj->GetVolume().GetRadius()) )
			{
				volatile tCamDrop* tDROP = new tCamDrop(EM_CAMDROP, true); 
				CEventManager::GetInstanceThreaded()->SendEventUpdateLocal(ET_CAMDROP,(tBaseEventData*)tDROP);
				tDROP = NULL;
			}

			continue;
		}

		if ( m_vHazards[i]->TestAgainstSphere(vPtTop, fPtRad) )
		{
			newFriction.x = m_vHazards[i]->GetValue();
			nLastHit = i;
			bCollided = true;
		}
		if ( m_vHazards[i]->TestAgainstSphere(vPtLeft, fPtRad) )
		{
			newFriction.z = m_vHazards[i]->GetValue();
			newFriction.y = m_vHazards[i]->GetValue();
			nLastHit = i;
			bCollided = true;
		}
		if ( m_vHazards[i]->TestAgainstSphere(vPtRight, fPtRad) )
		{
			newFriction.y = m_vHazards[i]->GetValue();
			newFriction.z = m_vHazards[i]->GetValue();
			nLastHit = i;
			bCollided = true;
		}
	}

	obj->SetFrictionVals(obj->GetFrictionVals() + newFriction);

	// set currHazard
	if (bCollided)
	{
		if ( strcmp(m_vHazards[nLastHit]->getName(), "CL_HZ_SinkWater") == 0 ||
			 strcmp(m_vHazards[nLastHit]->getName(), "CL_HZ_Water") == 0 )
		{
			obj->SetCurrentHazard( WATER );
		}
		else if ( strcmp(m_vHazards[nLastHit]->getName(), "CL_HZ_LightSpill") == 0 )
		{
			obj->SetCurrentHazard( OIL );
		}
		else if ( strcmp(m_vHazards[nLastHit]->getName(), "CL_HZ_DarkSpill") == 0 )
		{
			obj->SetCurrentHazard( SODA );
		}
		else if ( strcmp(m_vHazards[nLastHit]->getName(), "CL_HZ_Ice") == 0 )
		{
			obj->SetCurrentHazard( ICE );
		}
		else if ( strcmp(m_vHazards[nLastHit]->getName(), "CL_HZ_Sticky") == 0 )
		{
			obj->SetCurrentHazard( STICKY );
		}
		else if ( strcmp(m_vHazards[nLastHit]->getName(), "CL_HZ_StickyP") == 0 )
		{
			obj->SetCurrentHazard( STICKYP );
		}
		else if ( strcmp(m_vHazards[nLastHit]->getName(), "CL_FoulShot") == 0 )
		{
			if (obj->getSpeed() <= 0.00025f)
			{
				obj->setFoul(true);
				obj->setTransparent(true);

				volatile tFoulShot* tNP = new tFoulShot(EM_FOULSHOT);
				CEventManager::GetInstanceThreaded()->SendEventUpdateCrossToRender(ET_FOULSHOT,(tBaseEventData*)tNP);
				tNP = NULL;
				volatile tAnimStateChange* tAChange = new tAnimStateChange(EM_ANIMSTATECHANGE, obj->GetOwner()->GetGlobalID(),
					RJ_Config::RJ_ANIM_ANTHONY,RJ_Config::ANTH_ANIM_FOUL,false); 
				CEventManager::GetInstanceThreaded()->SendEventUpdateCrossToRender(ET_ANIMSTATECHANGE,(tBaseEventData*)tAChange);
				tAChange = NULL;
				volatile tAnimStateChange* tABChange = new tAnimStateChange(EM_ANIMSTATECHANGE, obj->GetOwner()->GetGlobalID(),
					RJ_Config::RJ_ANIM_BROOM,RJ_Config::BROOM_ANIM_FOUL,false); 
				CEventManager::GetInstanceThreaded()->SendEventUpdateCrossToRender(ET_ANIMSTATECHANGE,(tBaseEventData*)tABChange);
				tABChange = NULL;
			}
		}
	}
	else	// they aren't in anything
		obj->SetCurrentHazard(NONE);

	return bCollided;
}

void CCollisionMachine::React(CStone* obj, IVolumeObject* vol, D3DXVECTOR3& n)
{
	if (vol->GetValue() > 1.0f)
	{
		// tell HUD we got Whipped Cream
		volatile tPlayerGotWC* tWC = new tPlayerGotWC(EM_PLAYERGOTCREAM);
		CEventManager::GetInstanceThreaded()->SendEventUpdateCrossToRender(ET_PLAYERGOTCREAM, (tBaseEventData*)tWC);
		tWC = NULL;
	}

	if( (vol->GetMaterialType() == WALL || vol->GetMaterialType() == JELLO) && m_fWasColliding == false)
	{
		if(!obj->getHasCollided())
			obj->addWallPoints(1);
		obj->SetNumWallHits(obj->GetNumWallHits() + 1);
		obj->SetCamUnattach(true);
		obj->SetColl2(obj->GetColl1());
		obj->SetColl1(obj->getPosition());
		obj->react(vol, n);
		m_fWasColliding = true;
	}
	else if(vol->GetMaterialType() == STONE && m_fWasColliding == false)
	{
		obj->react(vol, vol->GetNormal(obj->getPosition()));
		m_fWasColliding = true;
		obj->SetCamUnattach(true);
	}
	else if(vol->GetMaterialType() == BIT)
	{

		volatile tPlaySoundEnum* tSOUND = new tPlaySoundEnum(EM_PLAYSOUNDENUM, RJ_Config::RJ_SUGAR_1,false); 
		CEventManager::GetInstanceThreaded()->SendEventUpdateCrossToRender(ET_PLAYSOUND,(tBaseEventData*)tSOUND);
		tSOUND = NULL;

		obj->addBitPoints(5);
		vol->SetIsActive(false);
		SendToggleEvent(vol->GetID());
	}
	else if(vol->GetMaterialType() == POWERUP)
	{
		if(!obj->GetIsCurrentStone())
			return;

		volatile tPlaySoundEnum* tSOUND = new tPlaySoundEnum(EM_PLAYSOUNDENUM, RJ_Config::RJ_POWERUP_ANTHONY,true); 
		CEventManager::GetInstanceThreaded()->SendEventUpdateCrossToRender(ET_PLAYSOUND,(tBaseEventData*)tSOUND);
		tSOUND = NULL;
		switch(((CPowerUp*)vol)->GetPOWType())
		{
		case CHERRIES:
			{
				if(obj->GetOwner()->GetCherry())
					return;

				volatile tPlaySoundEnum* tSOUND = new tPlaySoundEnum(EM_PLAYSOUNDENUM, RJ_Config::RJ_POWERUP_CHERRIES,true); 
				CEventManager::GetInstanceThreaded()->SendEventUpdateCrossToRender(ET_PLAYSOUND,(tBaseEventData*)tSOUND);
				tSOUND = NULL;
				break;
			}
		case GUM:
			{
				volatile tPlaySoundEnum* tSOUND = new tPlaySoundEnum(EM_PLAYSOUNDENUM, RJ_Config::RJ_POWERUP_GUM,true); 
				CEventManager::GetInstanceThreaded()->SendEventUpdateCrossToRender(ET_PLAYSOUND,(tBaseEventData*)tSOUND);
				tSOUND = NULL;
				break;
			}
		case TURKEY:
			{
				volatile tPlaySoundEnum* tSOUND = new tPlaySoundEnum(EM_PLAYSOUNDENUM, RJ_Config::RJ_POWERUP_COLDTURKEY,true); 
				CEventManager::GetInstanceThreaded()->SendEventUpdateCrossToRender(ET_PLAYSOUND,(tBaseEventData*)tSOUND);
				tSOUND = NULL;
				break;
			}
		case WHIP_CREAM:
			{
				volatile tPlaySoundEnum* tSOUND = new tPlaySoundEnum(EM_PLAYSOUNDENUM, RJ_Config::RJ_POWERUP_WHIPCREAM,true); 
				CEventManager::GetInstanceThreaded()->SendEventUpdateCrossToRender(ET_PLAYSOUND,(tBaseEventData*)tSOUND);
				tSOUND = NULL;
				break;
			}
		case DOUBLE_GUMMY:
			{
				break;
			}
		case PEANUT:
			{
				break;
			}
		case BUTTER:
			{
				break;
			}
		case FORTUNE_COOKIE:
			{
				volatile tPlaySoundEnum* tSOUND = new tPlaySoundEnum(EM_PLAYSOUNDENUM, RJ_Config::RJ_POWERUP_FORTUNECOOKIE,true); 
				CEventManager::GetInstanceThreaded()->SendEventUpdateCrossToRender(ET_PLAYSOUND,(tBaseEventData*)tSOUND);
				tSOUND = NULL;
				break;
			}
		}
		// for Powerups, fValue == type

	//	((CPowerUp*)vol)->SetStone((CStone*)obj);
		obj->SetNumPowerUps(obj->GetNumPowerUps() + 1);
		CPowerUp* temp = new CPowerUp();
		temp->SetPOWType(((CPowerUp*)vol)->GetPOWType());
		temp->SetPlayer(obj->GetOwner());

		if(((CPowerUp*)vol)->GetPOWType() == CHERRIES)
		{
			obj->GetOwner()->SetCherry(true);
			obj->GetOwner()->SetCherryPOW(temp);
		}
		obj->AddPowerUp(temp);

		vol->SetIsActive(false);
		SendTogglePOWEvent(vol->GetID(), vol->getName());
	}
	else if(vol->GetMaterialType() == GOAL/* && obj->getSpeed() == 0*/)
	{
		volatile tPlaySoundEnum* tSOUND = new tPlaySoundEnum(EM_PLAYSOUNDENUM, RJ_Config::RJ_BULLSEYE,true); 
		CEventManager::GetInstanceThreaded()->SendEventUpdateCrossToRender(ET_PLAYSOUND,(tBaseEventData*)tSOUND);
		tSOUND = NULL;
		D3DXVECTOR3 temp = vol->GetPosition() - obj->getPosition();
		float Dist = D3DXVec3Dot(&temp, &temp);
		obj->setGoalMultiplier(2);
		if (Dist <= (((((CSolidSphere*)vol)->GetRadius() * 2.0f)/ 3.0f)) * (((((CSolidSphere*)vol)->GetRadius() * 2.0f)/ 3.0f)))
		{
			obj->setGoalMultiplier(4);
			if (Dist <= ((((CSolidSphere*)vol)->GetRadius()/ 3.0f) * ((((CSolidSphere*)vol)->GetRadius()/ 3.0f))))
			{
				obj->setGoalMultiplier(6);
				if (Dist <= ((((CSolidSphere*)vol)->GetRadius()/ 6.0f) * ((((CSolidSphere*)vol)->GetRadius()/ 6.0f))))
				{
					obj->setGoalMultiplier(8);
				}
			}
		}
	}
}

bool CCollisionMachine::UnloadData()
{
	for (unsigned int nSolid = 0; nSolid < m_vSolids.size(); nSolid++)
		delete m_vSolids[nSolid];
	m_vSolids.clear();

	for (unsigned int nHazard = 0; nHazard < m_vHazards.size(); nHazard++)
		delete m_vHazards[nHazard];
	m_vHazards.clear();

	CEventManager::GetInstanceThreaded()->UnregisterListenerUpdate(ET_TOGGLEALLBITS,this);

	return true;
}
void CCollisionMachine::SendToggleEvent(unsigned int _id)
{
	volatile tToggleBit* tNP = new tToggleBit(EM_TOGGLEBIT, _id, false);
	CEventManager::GetInstanceThreaded()->SendEventUpdateLocal(ET_TOGGLEBIT,(tBaseEventData*)tNP);
	tNP = NULL;
}
void CCollisionMachine::SendTogglePOWEvent(unsigned int _id, const char* szName)
{
	volatile tTogglePOW* tNP = new tTogglePOW(EM_TOGGLEPOW, _id, szName, false);
	CEventManager::GetInstanceThreaded()->SendEventUpdateLocal(ET_TOGGLEPOW,(tBaseEventData*)tNP);
	tNP = NULL;
}
void CCollisionMachine::HandleEvent(CEvent* pEvent)
{
	if(pEvent->GetEventID() == ET_TOGGLEALLBITS)
	{
		if(pEvent->GetData() != NULL)
		{
			if(pEvent->GetData()->GetMsgID() == EM_TOGGLEALLBITS)
			{
				tToggleAllBits* tInp;
				tInp = (tToggleAllBits*)pEvent->GetData();
				if(tInp->m_bBitsOn)
				{
					//Set all active
					for( unsigned int i = 0; i < m_vSolids.size(); i++)
					{
						if (m_vSolids[i]->GetMaterialType() == BIT)
						{
							m_vSolids[i]->SetIsActive(true);
						}
					}
				}

			}
		}
	}
	else if(pEvent->GetEventID() == ET_TOGGLEALLPOW)
	{
		if(pEvent->GetData() != NULL)
		{
			if(pEvent->GetData()->GetMsgID() == EM_TOGGLEALLPOW)
			{
				tToggleAllPOW* tInp;
				tInp = (tToggleAllPOW*)pEvent->GetData();
				if(tInp->m_bPOWOn)
				{
					//Set all active
					for( unsigned int i = 0; i < m_vSolids.size(); i++)
					{
						if (m_vSolids[i]->GetMaterialType() == POWERUP)
						{
							m_vSolids[i]->SetIsActive(true);
						}
					}
				}

			}
		}
	}

	pEvent->m_nRefs--;
}
RJ_Config::SOUND_FILE CCollisionMachine::LookUpCollisionSound(const char* szCollName)
{
	if(strcmp(szCollName, "CL_SM_PieSlice") == 0)
		return RJ_Config::RJ_COLLIDE_WHOLE;
	else if(strcmp(szCollName, "CL_SM_PieBite") == 0)
		return RJ_Config::RJ_COLLIDE_BITTEN;
	else if(strcmp(szCollName, "CL_SM_PizzaBox") == 0)
		return RJ_Config::RJ_COLLIDE_PIZZA;
	else if(strcmp(szCollName, "CL_SM_IceTray") == 0)
		return RJ_Config::RJ_COLLIDE_ICETRAY;
	else if(strcmp(szCollName, "CL_SM_IceCube") == 0)
		return RJ_Config::RJ_COLLIDE_ICECUBE;
	else if(strcmp(szCollName, "CL_SM_IceCream") == 0)
		return RJ_Config::RJ_COLLIDE_ICECREAM;
	else if(strcmp(szCollName, "CL_SM_Popsicle") == 0)
		return RJ_Config::RJ_COLLIDE_POPSICLE;
	else if(strcmp(szCollName, "CL_SM_Fish") == 0)
		return RJ_Config::RJ_COLLIDE_FISH;
	else if(strcmp(szCollName, "CL_SM_Freezer") == 0 || strcmp(szCollName, "CL_SM_Fixtures") == 0)
		return RJ_Config::RJ_COLLIDE_FREEZERSEAL;
	else if(strcmp(szCollName, "CL_SM_Bottle") == 0)
		return RJ_Config::RJ_COLLIDE_SODA; 
	else if(strcmp(szCollName, "CL_SM_BagOfPeas") == 0)
		return RJ_Config::RJ_COLLIDE_PEAS;
	else if(strcmp(szCollName, "CL_SM_Jelly") == 0)
		return RJ_Config::RJ_COLLIDE_FREEZERSEAL;
	else if(strcmp(szCollName, "CL_SM_Carrot") == 0)
		return RJ_Config::RJ_COLLIDE_CARROT;

	char nameBuffer[64];
	strncpy_s(nameBuffer,64,szCollName+10,7);
	nameBuffer[7] = '\0';

	if( strcmp(nameBuffer, "Gelatin") == 0)
	//		newItem->SetMaterialType( JELLO );  
	//else if(strcmp(szCollName, "CL_SM_Gelatin") == 0)
		return RJ_Config::RJ_COLLIDE_GELATIN;

	else if(strcmp(szCollName, "CL_SM_Fruit") == 0)
		return RJ_Config::RJ_COLLIDE_FRUIT;
	else if(strcmp(szCollName, "CL_SM_Knife") == 0)
		return RJ_Config::RJ_COLLIDE_KNIFE;
	else if(strcmp(szCollName, "CL_SM_Sink") == 0)
		return RJ_Config::RJ_COLLIDE_KITCHENSINK;
	else if(strcmp(szCollName, "CL_SM_LightSwitch") == 0)
		return RJ_Config::RJ_COLLIDE_LIGHTSWITCH;
	else if(strcmp(szCollName, "CL_SM_JuiceCup") == 0)
		return RJ_Config::RJ_COLLIDE_JUICECUP;
	else if(strcmp(szCollName, "CL_SM_Cereal") == 0)
		return RJ_Config::RJ_COLLIDE_CEREALBOX;
	else if(strcmp(szCollName, "CL_SM_Coffee") == 0)
		return RJ_Config::RJ_COLLIDE_COFFEEPOT;
	else if(strcmp(szCollName, "Stone") == 0)
		return RJ_Config::RJ_COLLIDE_STONE;
	else if(strcmp(szCollName, "CL_SM_Cup") == 0)
		return RJ_Config::RJ_COLLIDE_JUICECUP;
	else if(strcmp(szCollName, "CL_SM_Border") == 0)
		return RJ_Config::RJ_SOUND_NA;
	else if(strcmp(szCollName, "CL_SM_Cereal") == 0)
		return RJ_Config::RJ_COLLIDE_CEREALBOX;
	else if(strcmp(szCollName, "CL_SM_Coffee") == 0)
		return RJ_Config::RJ_COLLIDE_COFFEEPOT;
	else if(strcmp(szCollName, "CL_SM_Faucet") == 0)
		return RJ_Config::RJ_COLLIDE_KITCHENSINK;
	else if(strcmp(szCollName, "CL_SM_Apple") == 0)
		return RJ_Config::RJ_COLLIDE_FRUIT;
	else if(strcmp(szCollName, "CL_SM_Knife") == 0)
		return RJ_Config::RJ_COLLIDE_KNIFE;
	else if(strcmp(szCollName, "CL_SM_Cantaloupe") == 0)
		return RJ_Config::RJ_COLLIDE_FRUIT;
	else if(strcmp(szCollName, "CL_SM_Banana") == 0)
		return RJ_Config::RJ_COLLIDE_FRUIT;
	else if(strcmp(szCollName, "CL_SM_Cable") == 0)
		return RJ_Config::RJ_SOUND_NA;
	else if(strcmp(szCollName, "CL_SM_Corner") == 0)
		return RJ_Config::RJ_SOUND_NA;
	else if(strcmp(szCollName, "CL_SM_AppleSlice") == 0)
		return RJ_Config::RJ_COLLIDE_FRUIT;
	else if(strcmp(szCollName, "CL_HZ_Water") == 0)
		return RJ_Config::RJ_ANTHONY_HAZARD;
	else if(strcmp(szCollName, "CL_HZ_DarkSpill") == 0)
		return RJ_Config::RJ_ANTHONY_HAZARD;
	else if(strcmp(szCollName, "CL_HZ_LightSpill") == 0)
		return RJ_Config::RJ_ANTHONY_HAZARD;
	else if(strcmp(szCollName, "CL_HZ_SinkWater") == 0)
		return RJ_Config::RJ_ANTHONY_HAZARD;
	else if(strcmp(szCollName, "CL_SM_Duck") == 0)
		return RJ_Config::RJ_COLLIDE_DUCK;
	return RJ_Config::RJ_SOUND_NA;
}