/**
 * 0===========================================================================0
 * | CollisionMachine.h                                                        |
 * |  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  |
 * |   Author        : Andrew Gleeson                                          |
 * |   E-mail        : atgleeson@gmail.com                                     |
 * |  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  |
 * |   Date Created  : 09:23 am (Eastern) on 06/14/2010                        |
 * |   Date Edited   : 10:10 pm (Eastern) on 06/25/2010                        |
 * 0=====0===============================================================0=====0
 *       |                                                               |      
 * 0=====0===============================================================0=====0
 * | Code Description:                                                         |
 * |  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  |
 * |   An engine for testing Sphere and OBB collisions                         |
 * |   and performing phyics-based reactions after them                        |
 * |																		   |
 * |   NOTE: not a singleton!                                                  |
 * 0===========================================================================0
 */

#ifndef  REDJAGUAR_COLLISIONMACHINE_H_		// Check to see if this .h is defined
#define  REDJAGUAR_COLLISIONMACHINE_H_		//  if not, define it

// 0===========================================================================0
// | ~ Includes ~                                                              |
// 0===========================================================================0
#include <vector>
using std::vector;
#include "../Event/EventDefinitions.h"
#include "../Event/IListener.h"
#include "../../../RJ_Config.h"

class CStone;
class IVolumeObject;
class CEvent;

class CCollisionMachine : public IListener
{
	// Members
private:
	vector<IVolumeObject*> m_vSolids;
	vector<IVolumeObject*> m_vHazards;
	vector<CStone*> m_vStones;
	bool m_fWasColliding;
	bool m_bLoopCollide;

	//Hack for GOAL
	IVolumeObject* m_pGoal;

	// Functions
private:
	
	// private test functions
	//	these are called by TestCollision()
	int CheckSolids(CStone* obj, D3DXVECTOR3& _n);
	bool CheckHazards(CStone* obj);
	void React(CStone* obj, IVolumeObject* vol, D3DXVECTOR3& n);

public:
	CCollisionMachine(void);
	CCollisionMachine(char* szFileName);	// constructor
	~CCollisionMachine(void);				// destructor

	// loads binary collision files
	bool LoadData(const char* szFileName);

	// tests the given stone against
	//	all of the volumes in the lists
	bool TestCollision(CStone* obj);

	// clears all data in memory
	bool UnloadData();

	// handles events sent to CM
	void HandleEvent(CEvent* pEvent);

	// tells SceneManager to toggle the sugarbit with the given id
	void SendToggleEvent(unsigned int _id);

	// tells SM to toggle the PowerUp with the given name and id
	void SendTogglePOWEvent(unsigned int _id, const char* szName);

	// getters and Setters for lists
	vector<IVolumeObject*>& GetSolidList() { return m_vSolids; }
	vector<IVolumeObject*>& GetHazardList() { return m_vHazards; }
	vector<CStone*>& GetStoneList() { return m_vStones; }
	void SetLoopCollide(bool bLoopCollide) { m_bLoopCollide = bLoopCollide; }
	RJ_Config::SOUND_FILE LookUpCollisionSound(const char* szCollName);
};

#endif //REDJAGUAR_COLLISIONMACHINE_H_