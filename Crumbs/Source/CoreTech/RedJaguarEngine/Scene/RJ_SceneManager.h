/**
 * 0===========================================================================0
 * | RJ_SceneManager.h                                                         |
 * |  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  |
 * |   Author        : Matthew Walker                                          |
 * |   E-mail        : MattW1989@yahoo.com                                     |
 * |  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  |
 * |   Date Created  : 11:00 am (Eastern) on 06/14/2010                        |
 * |   Date Edited   : 00:00 pm (Eastern) on 00/00/2010                        |
 * 0=====0===============================================================0=====0
 *       |                                                               |      
 * 0=====0===============================================================0=====0
 * | Code Description:                                                         |
 * |  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  |
 * |   Stores all objects that need to be rendered and updates them            |
 * 0===========================================================================0
 */
#ifndef RJ_SCENE_MANAGER_H_
#define RJ_SCENE_MANAGER_H_

#include "../Event/IListener.h"
#include "../Event/EventDefinitions.h"
#include "../Base Classes/IObject.h"

#include "../Collision/CollisionMachine.h"

#include <vector>
using std::vector;
#include <deque>
using std::deque;

class CEvent;
class CCamera;
class CGameplay;
class CStone;
class CPlayer;

class SceneManager : public IListener
{
private:
	vector<IObject*> MasterList;
	vector<CStone*> StoneList;
	vector<CPlayer*> PlayerList;
	deque<IObject*> Opaque;
	deque<IObject*> Transparent;
	deque<D3DXMATRIX> m_qCameraList;
	CCamera*         MainCamera;
	int					m_nCurrLevel;
	float				m_fMainCamSpeed;
	float				m_fMainCamRotRate;
	double				m_dCamTime;
	CCollisionMachine	m_tCollisionMachine;
	D3DXMATRIX		m_mStartingPoint;
	D3DXMATRIX		m_mGummyMat;
	D3DXMATRIX		m_mPrevMat;

	bool CamAttached;
	bool CamQReset;
	bool GamePlayInit;
	bool CamUnattached;

	bool LoadLevelWaiting;
	bool GameplayReady;
	bool m_bCamDrop;
	bool WaitStoneStop;
	bool m_bPreLaunchView;
	bool m_bCamFly;
	bool m_bTutOver;
	bool m_bTutorial;
	bool m_bCamNotMoving;
	int m_nCurrCam;
	bool m_bCamChange;
	bool CameraNeeds;
	bool m_bResetMenu;

	int LeveltoLoad;
	int m_nNumPlayers;

	bool m_bGameplayShutdown;

	float fFX, fMusic, fBrightness;

	tInput			 tCurrentGameInput;
	tNumPlayers		 tNumberPlay;
	tLevelSelect	 tLevelPlay;
	CGameplay*		 Gameplay;

	SceneManager(void) {};
	SceneManager(const SceneManager&) {};
	SceneManager& operator=(const SceneManager&) {};
	~SceneManager(void) {};

public:
	bool init();//
	void InitGameplay(int NumPlayers, int CurrLevel);
	static SceneManager* getInstanceUpdate(void);//
	bool shutdown();
	void update(double fDelta);//
	void sort(void);//
	void add(IObject*);//
	void remove(char* szName);//
	void clear(void);//
	void clearOpaque(void);//
	void clearTransparent(void);//
	CCamera* getCamera(void) { return MainCamera; }//
	void HandleEvent(CEvent* pEvent);
	bool inFrustum(IObject* object);//
	void loadLevel(char* szFile);//
	void sendObjectEvent(void);
	void sendClearListEvent(void);
	void SwitchLevel(int Level);

	void SetCamDrop(bool _camDrop) { m_bCamDrop = _camDrop; }
	bool GetCamDrop(void) { return m_bCamDrop; }

	bool GetPreLaunchView() const { return m_bPreLaunchView; }
	void SetPreLaunchView(bool val) { m_bPreLaunchView = val; }

	void SetWaitStoneStop(bool _StoneStop) { WaitStoneStop = _StoneStop; }
	bool GetWaitStoneStop(void) { return WaitStoneStop; }

	D3DXMATRIX GetGummyStart() {return m_mGummyMat;}
	CCollisionMachine* GetCollisionMachine() { return &m_tCollisionMachine; }

	tInput getcurrentInput(){return tCurrentGameInput;}
	int getCurrLevel(){return m_nCurrLevel;}
	void CameraMainInput(double fDelta);
	void ClearCurrInput(void);

	bool LoadSettings(bool bLoadTut = false);
	bool SaveSettings(bool bReset = false);
};

#endif