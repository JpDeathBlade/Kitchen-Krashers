/**
* 0===========================================================================0
* | RJ_SceneManager.cpp                                                       |
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
#include "RJ_SceneManager.h"
#include "Camera.h"
#include "../Event/IListener.h"
#include "../Event/EventManager.h"
#include "../../../RJ_Config.h"
#include "../../Utilities/RJ_MathLibrary.hpp"
#include "../../../Gameplay/Gameplay.h"
#include "../../../Gameplay/Objects/CStone.h"
#include "../../../Gameplay/Objects/CPlayer.h"
#include "../../../Gameplay/Objects/PowerUp.h"
#include "../Base Classes/ISceneObject.h"
#include "../Collision/SolidSphere.h"
#include "../Sound/CSoundEngine.h"
#include <fstream>

bool SceneManager::init()
{
	//LoadSettings(true);
	//MasterList.resize(64);
	//Opaque.resize(32);
	//Transparent.resize(32);
	CEventManager::GetInstanceThreaded()->RegisterListenerUpdate(ET_INPUT,this);
	CEventManager::GetInstanceThreaded()->RegisterListenerUpdate(ET_LEVELSELECT,this);
	CEventManager::GetInstanceThreaded()->RegisterListenerUpdate(ET_NUMPLAYERS,this);
	CEventManager::GetInstanceThreaded()->RegisterListenerUpdate(ET_GAMEPLAYSHUTDOWN,this);
	CEventManager::GetInstanceThreaded()->RegisterListenerUpdate(ET_TOGGLEBIT,this);
	CEventManager::GetInstanceThreaded()->RegisterListenerUpdate(ET_TOGGLEALLBITS,this);
	CEventManager::GetInstanceThreaded()->RegisterListenerUpdate(ET_MENUCAMERA,this);
	CEventManager::GetInstanceThreaded()->RegisterListenerUpdate(ET_CAMERAPOP,this);
	CEventManager::GetInstanceThreaded()->RegisterListenerUpdate(ET_TOGGLEPOW, this);
	CEventManager::GetInstanceThreaded()->RegisterListenerUpdate(ET_TOGGLEALLPOW, this);
	CEventManager::GetInstanceThreaded()->RegisterListenerUpdate(ET_LISTCLEARED, this);
	CEventManager::GetInstanceThreaded()->RegisterListenerUpdate(ET_CAMDROP, this);
	CEventManager::GetInstanceThreaded()->RegisterListenerUpdate(ET_CAMERADONEMOVING, this);
	CEventManager::GetInstanceThreaded()->RegisterListenerUpdate(ET_CAMFLY, this);
	CEventManager::GetInstanceThreaded()->RegisterListenerUpdate(ET_TUTOVER, this);
	CEventManager::GetInstanceThreaded()->RegisterListenerUpdate(ET_TOGGLETUT, this);
	
	MainCamera = new CCamera;
	MainCamera->SetCamType(RJ_Config::RJ_CAM_MAIN);
	MainCamera->init(0.1f,1000.0f,1024.0f,768.0f);
	m_fMainCamSpeed = 10.0f;
	m_fMainCamRotRate = 5.0f;
	m_dCamTime = 0;
	D3DXMatrixIdentity(&m_mPrevMat);

	//if(Gameplay != NULL)
	//	Gameplay->Init();
	CamAttached = true;
	CamQReset = false;
	GamePlayInit = false;
	CamUnattached = false;
	LoadLevelWaiting = false;
	m_bPreLaunchView = false;
	GameplayReady = false;
	m_bCamFly = false;
	m_bCamNotMoving = false;
	m_bTutorial = true;
	m_bTutOver = false;
	CameraNeeds = false;
	m_bResetMenu = true;
	m_nCurrCam = 1;

	//int NumPlayers = Gameplay->GetNumPlayers();
	//for(int player = 0; player < NumPlayers; player++)
	//{
	//	MasterList.push_back(Gameplay->GetPlayer(player));
	//	PlayerList.push_back(Gameplay->GetPlayer(player));
	//	for(int stone = 0; stone < 5; stone++)
	//	{
	//		MasterList.push_back(Gameplay->GetPlayer(player)->GetStone(stone));
	//		StoneList.push_back(Gameplay->GetPlayer(player)->GetStone(stone));
	//	}
	//}

	// load level 1
	loadLevel("Resources/Levels/mainmenu.scn");
	m_nCurrLevel = 0;
	LeveltoLoad = 0;
	m_nNumPlayers = 0;

	// set first camera position
	/*D3DXMATRIX menuCamera =
	D3DXMATRIX( -0.857733f, 0.00597801f, 0.514061f, 0.0f,
	0.0580067f, 0.994672f, 0.0852196f, 0.0f,
	-0.510813f, 0.102915f, -0.85351f, 0.0f,
	21.5262f, 3.3434f, -10.7377f, 1.0f );
	D3DXMATRIX tmp = MainCamera->GetViewMatrix();
	MainCamera->SoftAttach(tmp, menuCamera, D3DXVECTOR3(0,0,0), 0.0, false, true);
	MainCamera->SetViewMatrix(&tmp);
	m_qCameraList.push_front(menuCamera);*/

	// HACK: manually add goal //
	//CPowerUp* goal = new CPowerUp("PowerUp", D3DXVECTOR3(10.0f, 0.0f, 0.0f), FORTUNE_COOKIE, 1.0f, 1.0f, POWERUP, 0, true);
	//MasterList.push_back(goal);
	//m_tCollisionMachine.GetSolidList().push_back(goal);

	// load Anthony into Asset Manager?
	/*tLoadMesh* tMesh = new tLoadMesh(EM_LOADMESH, "C_Anthony");
	CEventManager::GetInstanceThreaded()->SendEventUpdateCrossToRender(ET_LOADMESH, (tBaseEventData*)tMesh);
	tMesh = NULL;*/

	//tLoadAnimMesh* tAMesh = new tLoadAnimMesh(EM_LOADANIMMESH, "AnthonyBody");
	//CEventManager::GetInstanceThreaded()->SendEventUpdateCrossToRender(ET_LOADANIMMESH, (tBaseEventData*)tAMesh);
	//tAMesh = NULL;

	//tLoadAnimMesh* tABMesh = new tLoadAnimMesh(EM_LOADANIMMESH, "SM_Broom");
	//CEventManager::GetInstanceThreaded()->SendEventUpdateCrossToRender(ET_LOADANIMMESH, (tBaseEventData*)tABMesh);
	//tABMesh = NULL;

	//tLoadAnimMesh* tGAMesh = new tLoadAnimMesh(EM_LOADANIMMESH, "toothPicks");
	//CEventManager::GetInstanceThreaded()->SendEventUpdateCrossToRender(ET_LOADANIMMESH, (tBaseEventData*)tGAMesh);
	//tGAMesh = NULL;

	//tLoadAnimMesh* tTAMesh = new tLoadAnimMesh(EM_LOADANIMMESH, "GummyWorm");
	//CEventManager::GetInstanceThreaded()->SendEventUpdateCrossToRender(ET_LOADANIMMESH, (tBaseEventData*)tTAMesh);
	//tTAMesh = NULL;

	////GumMESS!
	//tLoadMesh* tGumMesh = new tLoadMesh(EM_LOADMESH, "DM_GumMESS");
	//CEventManager::GetInstanceThreaded()->SendEventUpdateCrossToRender(ET_LOADMESH, (tBaseEventData*)tGumMesh);
	//tGumMesh = NULL;

	////Hats
	//tLoadMesh* tGumHat = new tLoadMesh(EM_LOADMESH, "H_GumHat");
	//CEventManager::GetInstanceThreaded()->SendEventUpdateCrossToRender(ET_LOADMESH, (tBaseEventData*)tGumHat);
	//tGumHat = NULL;
	//tLoadMesh* tCherryHat = new tLoadMesh(EM_LOADMESH, "H_CherryHat");
	//CEventManager::GetInstanceThreaded()->SendEventUpdateCrossToRender(ET_LOADMESH, (tBaseEventData*)tCherryHat);
	//tCherryHat = NULL;
	//tLoadMesh* tColdTurkeyHat = new tLoadMesh(EM_LOADMESH, "H_ColdTurkeyHat");
	//CEventManager::GetInstanceThreaded()->SendEventUpdateCrossToRender(ET_LOADMESH, (tBaseEventData*)tColdTurkeyHat);
	//tColdTurkeyHat = NULL;
	//tLoadMesh* tFortuneHat = new tLoadMesh(EM_LOADMESH, "H_FortuneCookie");
	//CEventManager::GetInstanceThreaded()->SendEventUpdateCrossToRender(ET_LOADMESH, (tBaseEventData*)tFortuneHat);
	//tFortuneHat = NULL;
	//tLoadMesh* tCreamHat = new tLoadMesh(EM_LOADMESH, "H_WhippedCreamHat");
	//CEventManager::GetInstanceThreaded()->SendEventUpdateCrossToRender(ET_LOADMESH, (tBaseEventData*)tCreamHat);
	//tCreamHat = NULL;

	volatile tCreateSpecType* tObjC1 = new tCreateSpecType(EM_CREATESPEC, ObjectID::GetInstanceUpdate()->CheckOutID(), 0);
	CEventManager::GetInstanceThreaded()->SendEventUpdateCrossToRender(ET_CREATESPEC, (tBaseEventData*)tObjC1);
	tObjC1 = NULL;
	volatile tCreateSpecType* tObjC2 = new tCreateSpecType(EM_CREATESPEC, ObjectID::GetInstanceUpdate()->CheckOutID(), 1);
	CEventManager::GetInstanceThreaded()->SendEventUpdateCrossToRender(ET_CREATESPEC, (tBaseEventData*)tObjC2);
	tObjC2 = NULL;
	volatile tCreateSpecType* tObjC3 = new tCreateSpecType(EM_CREATESPEC, ObjectID::GetInstanceUpdate()->CheckOutID(), 2);
	CEventManager::GetInstanceThreaded()->SendEventUpdateCrossToRender(ET_CREATESPEC, (tBaseEventData*)tObjC3);
	tObjC3 = NULL;
	volatile tCreateSpecType* tObjC4 = new tCreateSpecType(EM_CREATESPEC, ObjectID::GetInstanceUpdate()->CheckOutID(), 3);
	CEventManager::GetInstanceThreaded()->SendEventUpdateCrossToRender(ET_CREATESPEC, (tBaseEventData*)tObjC4);
	tObjC4 = NULL;

	m_bGameplayShutdown = false;
	ClearCurrInput();
	return true;
}

SceneManager* SceneManager::getInstanceUpdate(void)
{
	static SceneManager instance;
	return &instance;
}
void SceneManager::HandleEvent(CEvent* pEvent)
{
	m_bCamNotMoving = false;
	m_bCamDrop = false;
	if(pEvent->GetEventID() == ET_INPUT)
	{
		if(pEvent->GetData() != NULL)
		{
			if(pEvent->GetData()->GetMsgID() == EM_INPUT)
			{
				ClearCurrInput();
				tInput* tInp;
				tInp = (tInput*)pEvent->GetData();
				tCurrentGameInput = *tInp;
			}
		}
	}

	else if(pEvent->GetEventID() == ET_LEVELSELECT)
	{
		if(pEvent->GetData() != NULL)
		{
			if(pEvent->GetData()->GetMsgID() == EM_LEVELSELECT)
			{
				tLevelSelect* tLS;
				tLS = (tLevelSelect*)pEvent->GetData();
				tLevelPlay = *tLS;
				if(tLevelPlay.m_nLevelSelect != m_nCurrLevel)
				{
					sendClearListEvent();
					LeveltoLoad = tLevelPlay.m_nLevelSelect;
					LoadLevelWaiting = true;
				}
			}
		}
	}

	else if(pEvent->GetEventID() == ET_NUMPLAYERS)
	{
		if(pEvent->GetData() != NULL)
		{
			if(pEvent->GetData()->GetMsgID() == EM_NUMPLAYERS)
			{
				tNumPlayers* tNP;
				tNP = (tNumPlayers*)pEvent->GetData();
				tNumberPlay = *tNP;
				m_nNumPlayers = tNumberPlay.m_nNumPlayers;
				if(LoadLevelWaiting == false)
					InitGameplay(tNumberPlay.m_nNumPlayers, m_nCurrLevel);
				else
					GameplayReady = true;
			}
		}
	}

	else if(pEvent->GetEventID() == ET_GAMEPLAYSHUTDOWN)
	{
		if(pEvent->GetData() != NULL)
		{
			if(pEvent->GetData()->GetMsgID() == EM_GAMEPLAYSHUTDOWN)
			{
				unsigned int temp = Gameplay->GetNumPlayers() * 6;
				for(unsigned int i = 0; i < temp; ++i)
					MasterList.pop_back();

				m_bGameplayShutdown = true;

				StoneList.clear();
				PlayerList.clear();
				GamePlayInit = false;
			}
		}
	}
	else if(pEvent->GetEventID() == ET_TOGGLEALLBITS)
	{
		if(pEvent->GetData() != NULL)
		{
			if(pEvent->GetData()->GetMsgID() == EM_TOGGLEALLBITS)
			{
				tToggleAllBits* tNP;
				tNP = (tToggleAllBits*)pEvent->GetData();
				for (unsigned int i = 0; i < MasterList.size(); ++i)
				{
					if (strcmp(MasterList[i]->getName(), "DM_SugarBit") == 0)
					{
						MasterList[i]->SetIsActive(tNP->m_bBitsOn);
					}
				}
			}
		}
	}
	else if(pEvent->GetEventID() == ET_TOGGLEBIT)
	{
		if(pEvent->GetData() != NULL)
		{
			if(pEvent->GetData()->GetMsgID() == EM_TOGGLEBIT)
			{
				tToggleBit* tNP;
				tNP = (tToggleBit*)pEvent->GetData();
				for (unsigned int i = 0; i < MasterList.size(); ++i)
				{
					if (strcmp(MasterList[i]->getName(), "DM_SugarBit") == 0 && MasterList[i]->GetID() == tNP->m_nId)
					{
						MasterList[i]->SetIsActive(tNP->m_bBitOn);
						break;
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
				tToggleAllPOW* tNP;
				tNP = (tToggleAllPOW*)pEvent->GetData();
				for (unsigned int i = 0; i < MasterList.size(); ++i)
				{
					if (strcmp(MasterList[i]->getName(), "DM_Gum") == 0 ||
						strcmp(MasterList[i]->getName(), "DM_Cherries") == 0 || strcmp(MasterList[i]->getName(), "DM_Whipcream") == 0 || 
						strcmp(MasterList[i]->getName(), "DM_Fortune_Cookie") == 0 || strcmp(MasterList[i]->getName(), "DM_ColdTurkey") == 0)/* || 
						strcmp(MasterList[i]->getName(), "DM_Peanut") == 0 || strcmp(MasterList[i]->getName(), "DM_Butter") == 0)*/
					{
						MasterList[i]->SetIsActive(tNP->m_bPOWOn);
					}
				}
			}
		}
	}
	else if(pEvent->GetEventID() == ET_TOGGLEPOW)
	{
		if(pEvent->GetData() != NULL)
		{
			if(pEvent->GetData()->GetMsgID() == EM_TOGGLEPOW)
			{
				tTogglePOW* tNP;
				tNP = (tTogglePOW*)pEvent->GetData();
				for (unsigned int i = 0; i < MasterList.size(); ++i)
				{				
					if (strcmp(MasterList[i]->getName(), tNP->m_szName) == 0)
						if(MasterList[i]->GetID() == tNP->m_nId)
						{
							MasterList[i]->SetIsActive(tNP->m_bPOWOn);
							break;
						}
				}
			}
		}
	}
	else if(pEvent->GetEventID() == ET_MENUCAMERA)
	{
		if(pEvent->GetData() != NULL)
		{
			if(pEvent->GetData()->GetMsgID() == EM_MENUCAMERA)
			{
				tCameraData* tCD;
				tCD = (tCameraData*)pEvent->GetData();

				m_qCameraList.clear();

				//D3DXMATRIX tmp = MainCamera->GetViewMatrix();
				//MainCamera->SoftAttach(tmp, tCD->m_mCamera, D3DXVECTOR3(0,0,0), 0.0, true);
				//MainCamera->SetViewMatrix(&tmp);
				m_qCameraList.push_front(tCD->m_mCamera);
			}
		}
	}
	else if(pEvent->GetEventID() == ET_CAMERAPOP)
	{
		if(pEvent->GetData() != NULL)
		{
			if(pEvent->GetData()->GetMsgID() == EM_CAMERAPOP)
			{
				m_qCameraList.pop_front();
			}
		}
	}
	else if(pEvent->GetEventID() == ET_LISTCLEARED)
	{
		if(pEvent->GetData() != NULL)
		{
			if(pEvent->GetData()->GetMsgID() == EM_LISTCLEARED)
			{
				clear();
				SwitchLevel(LeveltoLoad);
			}
		}
	}
	else if(pEvent->GetEventID() == ET_CAMDROP)
	{
		if(pEvent->GetData() != NULL)
		{
			if(pEvent->GetData()->GetMsgID() == EM_CAMDROP)
			{
				tCamDrop* tCD;
				tCD = (tCamDrop*)pEvent->GetData();
				m_bCamDrop = true;
			}
		}
	}
	else if(pEvent->GetEventID() == ET_CAMERADONEMOVING)
	{
		if(pEvent->GetData() != NULL)
		{
			if(pEvent->GetData()->GetMsgID() == EM_CAMERADONEMOVING)
			{
				m_bCamNotMoving = true;
			}
		}
	}
	else if(pEvent->GetEventID() == ET_CAMFLY)
	{
		if(pEvent->GetData() != NULL)
		{
			if(pEvent->GetData()->GetMsgID() == EM_CAMFLY)
			{
				m_bCamFly = false;
			}
		}
	}
	else if(pEvent->GetEventID() == ET_TUTOVER)
	{
		if(pEvent->GetData() != NULL)
		{
			if(pEvent->GetData()->GetMsgID() == EM_TUTOVER)
			{
				m_bTutOver = true;
			}
		}
	}
	else if(pEvent->GetEventID() == ET_TOGGLETUT)
	{
		if(pEvent->GetData() != NULL)
		{
			if(pEvent->GetData()->GetMsgID() == EM_TOGGLETUT)
			{
				tToggleTut* tCD;
				tCD = (tToggleTut*)pEvent->GetData();
				m_bTutorial = tCD->m_bTutorial;

				if ( !LoadSettings() )
					SaveSettings(true);
				else
					SaveSettings();
			}
		}
	}
	pEvent->m_nRefs--;
}
void SceneManager::SwitchLevel(int Level)
{
	volatile tClearSpectators* KillCrowd = new tClearSpectators(EM_CLEARSPECTATORS);
	CEventManager::GetInstanceThreaded()->SendEventUpdateCrossToRender(ET_CLEARSPECTATORS, (tBaseEventData*)KillCrowd);
	KillCrowd = NULL;

	LoadLevelWaiting = false;
	LoadSettings(true);
	switch(Level)
	{
	case 0:
		{
			m_mPrevMat = D3DXMATRIX( -0.857733f, 0.00597801f, 0.514061f, 0.0f,
				0.0580067f, 0.994672f, 0.0852196f, 0.0f,
				-0.510813f, 0.102915f, -0.85351f, 0.0f,
				21.5262f, 3.3434f, -10.7377f, 1.0f );
			loadLevel("Resources/Levels/mainmenu.scn");

			/*m_mStartingPoint = D3DXMATRIX( -0.857733f, 0.00597801f, 0.514061f, 0.0f,
			0.0580067f, 0.994672f, 0.0852196f, 0.0f,
			-0.510813f, 0.102915f, -0.85351f, 0.0f,
			21.5262f, 3.3434f, -10.7377f, 1.0f );

			D3DXMATRIX tmp = MainCamera->GetViewMatrix();
			MainCamera->SoftAttach(tmp, m_qCameraList[0], D3DXVECTOR3(0,0,0), 1.0f, false, true);
			MainCamera->SetViewMatrix(&tmp);
			MainCamera->SendCameraEvent();*/

			m_nCurrLevel = 0;
			break;
		}
	case 1:
		{
			m_qCameraList.clear();

			loadLevel("Resources/Levels/freezer_level.scn");
			m_bCamFly = true;
			m_bTutOver = false;
			m_nCurrLevel = 1;
			if(GameplayReady)
			{
				InitGameplay(m_nNumPlayers, m_nCurrLevel);
				GameplayReady = false;
			}

			break;
		}
	case 2:
		{
			m_qCameraList.clear();

			loadLevel("Resources/Levels/sink_level.scn");
			m_bTutOver = false;
			m_bCamFly = true;
			m_nCurrLevel = 2;
			if(GameplayReady)
			{
				InitGameplay(m_nNumPlayers, m_nCurrLevel);
				GameplayReady = false;
			}

			break;
		}
	case 3:
		{
			m_qCameraList.clear();

			loadLevel("Resources/Levels/stove_level.scn");
			m_bCamFly = true;
			m_bTutOver = false;

			m_nCurrLevel = 3;
			if(GameplayReady)
			{
				InitGameplay(m_nNumPlayers, m_nCurrLevel);
				GameplayReady = false;
			}

			break;
		}
	}
}
void SceneManager::InitGameplay(int NumPlayers, int CurrLevel)
{
	Gameplay = CGameplay::GetInstanceUpdate();
	Gameplay->SetStartingPoint(m_mStartingPoint);
	Gameplay->Init(NumPlayers, CurrLevel);
	m_nNumPlayers = NumPlayers;
	m_tCollisionMachine.GetStoneList().clear();
	StoneList.clear();

	for(int player = 0; player < m_nNumPlayers; player++)
	{
		MasterList.push_back(Gameplay->GetPlayer(player));
		PlayerList.push_back(Gameplay->GetPlayer(player));
		for(int stone = 0; stone < 5; stone++)
		{
			CStone* newStone = Gameplay->GetPlayer(player)->GetStone(stone);
			newStone->GetVolume().SetIsActive(false);
			newStone->setTransparent(true);

			MasterList.push_back(newStone);
			StoneList.push_back(newStone);
			m_tCollisionMachine.GetStoneList().push_back(newStone);
		}
	}

	if(MainCamera != NULL)
	{
		D3DXMATRIX _mat, _CamMat;
		_mat = Gameplay->GetCurrPlayer()->GetCurrStone()->getPosMat();
		_CamMat = MainCamera->GetViewMatrix();
		MainCamera->attachTo(_CamMat, _CamMat, _mat, D3DXVECTOR3(0, 5, -8), rj_mathlib::getWAxis(&_mat), 0, false, true);
		MainCamera->SetViewMatrix(&_CamMat);
		Gameplay->GetCurrPlayer()->GetCurrStone()->setPosMat(_mat);
	}
	Gameplay->SetUpdating(true);
	GamePlayInit = true;
}
bool SceneManager::shutdown()
{
	sendClearListEvent();
	CEventManager::GetInstanceThreaded()->UnregisterListenerAllUpdate(this);
	if(Gameplay != NULL)
		Gameplay->Shutdown();
	Gameplay = NULL;
	delete MainCamera;
	MainCamera = NULL;

	clear();
	return true;
}

void SceneManager::add(IObject* object)
{
	MasterList.push_back(object);
}

void SceneManager::remove(char* szName)
{
	unsigned int size = MasterList.size();
	for(unsigned int index = 0; index < size; index++)
	{
		if(strcmp(szName, MasterList[index]->getName()) == 0)
		{
			MasterList.erase(MasterList.begin()+index);
			return;
		}
	}
}

void SceneManager::update(double fDelta)
{
	if(Gameplay && m_bCamFly == false)
	{
		if(m_bTutOver || m_bTutorial == false)
		{
			if (Gameplay->GetUpdating())
			{
				if(MainCamera)
					CameraMainInput(fDelta);
				clearOpaque();
				clearTransparent();

				Gameplay->Input(fDelta);

				unsigned int size = MasterList.size();
				for(unsigned int index = 0; index < size; index++)
				{
					if(MasterList[index])
						MasterList[index]->Update(fDelta);
				}

				if (GamePlayInit)
				{
					Gameplay->Update(fDelta);
					int NumStones = Gameplay->GetNumPlayers() * 5;
					bool collision = false;
					m_tCollisionMachine.SetLoopCollide(false);
					unsigned int collisionChecks = 0;
					for(int stone = 0; stone < NumStones; stone++)
					{
						if ( StoneList[stone]->GetIsLaunched() &&
							StoneList[stone]->IsMoving() )
						{
							if(m_tCollisionMachine.TestCollision(StoneList[stone]))
							{
								collision = true;
								m_tCollisionMachine.SetLoopCollide(true);
								StoneList[stone]->isColliding(true);
							}
							else
								StoneList[stone]->isColliding(false);
						}
					}
					while(collision)
					{
						collision = false;
						collisionChecks++;
						if(collisionChecks < 10)
						{
							for(int stone = 0; stone < NumStones; stone++)
							{
								if ( StoneList[stone]->GetIsLaunched() &&
									StoneList[stone]->IsMoving() )
								{
									if(m_tCollisionMachine.TestCollision(StoneList[stone]))
									{
										collision = true;
										StoneList[stone]->isColliding(true);
									}
									else
										StoneList[stone]->isColliding(false);
								}
							}
						}
					}
				}
				if(m_nCurrLevel == 2)
				{
					volatile tPlayEffect* tEffect = new tPlayEffect(EM_PLAYEFFECT, true, RJ_Config::RJ_DRIP, D3DXVECTOR3(-47.532f, 0.733f, -43.835f)); 
					CEventManager::GetInstanceThreaded()->SendEventUpdateCrossToRender(ET_PLAYEFFECT,(tBaseEventData*)tEffect);
					tEffect = NULL;
				}

				if(!LoadLevelWaiting)
				{
					sort();
					sendObjectEvent();
				}
				if (GamePlayInit)
				{
					if(MainCamera)
					{
						if(CamAttached)
						{
							if(WaitStoneStop || m_bPreLaunchView)
							{
								D3DXMATRIX _CamMat, temp;
								D3DXVECTOR3 offset, StoneForward, wAxis;
								offset = D3DXVECTOR3(1, 1, 1);

								temp = m_qCameraList[0];
								_CamMat = MainCamera->GetViewMatrix();

								MainCamera->SoftAttach(_CamMat, m_mPrevMat, temp, D3DXVECTOR3(0,0,0), fDelta, true, true);
								MainCamera->SetViewMatrix(&_CamMat);

								// tell HUD not to draw the multiplier
								volatile tNoMultiplier* tNM = new tNoMultiplier(EM_NOMULTIPLIER);
								CEventManager::GetInstanceThreaded()->SendEventUpdateCrossToRender(ET_NOMULTIPLIER,(tBaseEventData*)tNM);
								tNM = NULL;
							}
							else
							{
								// tell HUD to draw the multiplier
								volatile tNoMultiplier* tNM = new tNoMultiplier(EM_NOMULTIPLIER, false);
								CEventManager::GetInstanceThreaded()->SendEventUpdateCrossToRender(ET_NOMULTIPLIER,(tBaseEventData*)tNM);
								tNM = NULL;

								D3DXMATRIX _CamMat, temp;
								D3DXVECTOR3 offset, StoneForward, wAxis;

								temp = Gameplay->GetCurrPlayer()->GetCurrStone()->getPosMat();
								_CamMat = MainCamera->GetViewMatrix();

								StoneForward = Gameplay->GetCurrPlayer()->GetCurrStone()->GetForward();
								wAxis = rj_mathlib::getWAxis(&temp);

								wAxis.y += 1.0f;

								offset.x = -StoneForward.x * 2;
								if(m_bCamDrop)
									offset.y = 1.0f;
								else
									offset.y = 2.5f;
								offset.z = -StoneForward.z * 2;

								//if(_Collide && dCamTimer <= 3.0)
								//{
								//	offset.x += 15;
								//	offset.y = 15.5f;
								//	offset.z += 15;
								//	_Collide = false;
								//	dCamTimer = 0.0;
								//} 


								if (Gameplay->GetLaunched())
								{
									if(CamQReset == false)
										MainCamera->attachTo(_CamMat, _CamMat, temp, offset, wAxis, fDelta, true);
									else
									{
										MainCamera->attachTo(_CamMat, _CamMat, temp, -StoneForward * 2, wAxis, fDelta, true, false, true);
										CamQReset = false;
									}
								}
								else
								{
									offset.x *= 5;
									offset.y = 5;
									offset.z *= 5;
									if(CamQReset == false)
										MainCamera->attachTo(_CamMat, _CamMat, temp, offset, wAxis, fDelta, false);
									else
									{
										MainCamera->attachTo(_CamMat, _CamMat, temp, offset, wAxis, fDelta, false, false, true);
										CamQReset = false;
									}
								}
								MainCamera->SetViewMatrix(&_CamMat);
								m_mPrevMat = _CamMat;
							}

							//Gameplay->GetCurrPlayer()->GetCurrStone()->setPosMat(temp);

						}
					}
					MainCamera->SendCameraEvent();
					Gameplay->SendGameplayEvent();
				}
			}
			if(m_bGameplayShutdown)
			{
				Gameplay->Shutdown();
				Gameplay = NULL;
				m_bGameplayShutdown = false;
			}
		}
		else
		{
			switch (m_nCurrLevel)
			{
			case 1:
				{
					D3DXMATRIX tempCam = D3DXMATRIX(0.0f, 0.0f, 1.0f, 0.0f, 
												-0.37139061f, 0.92847657f, 0.0f, 0.0f,
												-0.92847675f, -0.37139067f, 0.0f, 0.0f,
												21.783001f, 5.0f, 12.153092f, 1.0f);
					MainCamera->SetViewMatrix(&tempCam);
					MainCamera->SendCameraEvent();
				}
				break;
			case 2:
				{
					D3DXMATRIX tempCam = D3DXMATRIX(-2.3841856e-008f, 0.0f, 0.99999994f, 0.0f, 
						-0.37139067f, 0.92847675f, -8.8546441e-009f, 0.0f,
						-0.92847669f, -0.37139067f, -2.2136609e-008f, 0.0f,
						4.3026576f, 5.0f, -3.6316047f, 1.0f);
					MainCamera->SetViewMatrix(&tempCam);
					MainCamera->SendCameraEvent();
				}
				break;
			}
		}
	}
	else
	{
		if(MainCamera)
			CameraMainInput(fDelta);
		clearOpaque();
		clearTransparent();

		unsigned int size = MasterList.size();
		for(unsigned int index = 0; index < size; index++)
		{
			if(MasterList[index])
				MasterList[index]->Update(fDelta);
		}

		if(!LoadLevelWaiting)
		{
			sort();
			sendObjectEvent();
		}

		if(MainCamera)
		{
			if (CamAttached)
			{
				if (!m_qCameraList.empty())
				{
					if(m_bCamFly)
					{
						int _size = m_qCameraList.size();
						if(m_bCamNotMoving && m_bCamChange)
						{
							m_nCurrCam++;
							m_bCamChange = false;
						}
						else if(m_bCamNotMoving == false)
							m_bCamChange = true;

						if(m_nCurrCam == _size)
							m_nCurrCam = 1;
						
						D3DXMATRIX PassIn;
						D3DXMATRIX tmp = MainCamera->GetViewMatrix();

						if (m_nCurrCam == 1)
							PassIn = m_qCameraList[_size - 1];
						else
							PassIn = m_qCameraList[m_nCurrCam - 1];

						bool bLevel1 = false;
						if(m_nCurrLevel == 1)
							bLevel1 = true;
						else
							bLevel1 = false;

						MainCamera->SoftAttach(tmp, PassIn, m_qCameraList[m_nCurrCam], D3DXVECTOR3(0,0,0), fDelta, true, true, bLevel1, true);
						MainCamera->SetViewMatrix(&tmp);
						MainCamera->SendCameraEvent();
					}
					else
					{
						m_nCurrCam = 1;
						D3DXMATRIX tmp;
						D3DXMatrixIdentity(&tmp);
						if(m_mPrevMat == tmp && !CameraNeeds)
							m_mPrevMat = m_qCameraList[0];
						MainCamera->SoftAttach(tmp, m_mPrevMat, m_qCameraList[0], D3DXVECTOR3(0,0,0), fDelta, true, true);

						D3DXVECTOR3 checkDist = rj_mathlib::getWAxis(&m_qCameraList[0]) - rj_mathlib::getWAxis(&tmp);

						float stopped = D3DXVec3Dot(&checkDist, &checkDist);
						if(stopped < 0.0009765625 && !CameraNeeds)
						{
							m_mPrevMat = m_qCameraList[0];
							CameraNeeds = true;
						}
						else if(stopped > 0.0009765625)
						{
							CameraNeeds = false;
						}
					

						MainCamera->SetViewMatrix(&tmp);
						MainCamera->SendCameraEvent();
					}

				}
			}
			MainCamera->SendCameraEvent();
		}
	}
}
void SceneManager::ClearCurrInput(void)
{ 
	tCurrentGameInput.m_n64KD = 0;
	tCurrentGameInput.m_n64KP = 0;
}
void SceneManager::CameraMainInput(double fDelta)
{
	//D3DMATRIX d3dm_Cam = camera->GetViewMatrix();
	if(tCurrentGameInput.m_n64KD & RJ_Config::CAM_FORWARD)
	{
		MainCamera->ViewTranslateLocalZ(m_fMainCamSpeed*(float)fDelta);
	}
	if(tCurrentGameInput.m_n64KD & RJ_Config::CAM_BACKWARD)
	{
		MainCamera->ViewTranslateLocalZ(-m_fMainCamSpeed*(float)fDelta);
	}
	if(tCurrentGameInput.m_n64KD & RJ_Config::CAM_LEFT)
	{
		MainCamera->ViewTranslateLocalX(-m_fMainCamSpeed*(float)fDelta);
	}
	if(tCurrentGameInput.m_n64KD & RJ_Config::CAM_RIGHT)
	{
		MainCamera->ViewTranslateLocalX(m_fMainCamSpeed*(float)fDelta);
	}
	if(tCurrentGameInput.m_n64KD & RJ_Config::CAM_UP)
	{
		MainCamera->ViewTranslateGlobalY(m_fMainCamSpeed*(float)fDelta);
	}
	if(tCurrentGameInput.m_n64KD & RJ_Config::CAM_DOWN)
	{
		MainCamera->ViewTranslateGlobalY(-m_fMainCamSpeed*(float)fDelta);
	}
	if(tCurrentGameInput.m_n64KD & RJ_Config::CAM_ROTLEFT)
	{
		MainCamera->ViewRotateGlobalY(m_fMainCamRotRate*(float)fDelta);
	}
	if(tCurrentGameInput.m_n64KD & RJ_Config::CAM_ROTRIGHT)
	{
		MainCamera->ViewRotateGlobalY(-m_fMainCamRotRate*(float)fDelta);
	}
	if(tCurrentGameInput.m_n64KD & RJ_Config::CAM_PITCHUP)
	{
		MainCamera->ViewRotateLocalX(-m_fMainCamRotRate*(float)fDelta);
	}
	if(tCurrentGameInput.m_n64KD & RJ_Config::CAM_PITCHDOWN)
	{
		MainCamera->ViewRotateLocalX(m_fMainCamRotRate*(float)fDelta);
	}
	if(tCurrentGameInput.m_n64KP & RJ_Config::AP_CAMLOCK)
	{
		if(CamAttached)
		{
			CamAttached = false;
		}
		else
		{
			CamAttached = true;
			CamQReset = true;
		}
	}
}
void SceneManager::sort()
{
	// camera information
	D3DXVECTOR3 camPos = MainCamera->GetViewPosition();
	D3DXMATRIX viewMatrix = MainCamera->GetViewMatrix();
	D3DXVECTOR3 objVect, itrVect;
	float objDistance = 0;
	float itrDistance = 0;

	// HACK: fix the sink water
	bool bFoundWater = false;
	IObject* pSinkA = NULL;
	IObject* pSinkB = NULL;

	unsigned int masterSize = MasterList.size();
	for(unsigned int masterIndex = 0; masterIndex < masterSize; masterIndex++)
	{
		// HACK for sink
		const char* name = MasterList[masterIndex]->getName();
		if ( strcmp(name, "LvB_SM_Water_a") == 0 )
		{
			pSinkA = MasterList[masterIndex];
			bFoundWater = true;
			continue;
		}
		else if ( strcmp(name, "LvB_SM_Water_b") == 0 )
		{
			pSinkB = MasterList[masterIndex];
			bFoundWater = true;
			continue;
		}

		if (MasterList[masterIndex]->GetIsActive() == false)
			continue;
		//if(!inFrustum(MasterList[masterIndex]))
		//	continue;

		// get objPosition
		objVect.x = (MasterList[masterIndex]->getCenter().x - MasterList[masterIndex]->getDistance());
		objVect.y = (MasterList[masterIndex]->getCenter().y - MasterList[masterIndex]->getDistance());
		objVect.z = (MasterList[masterIndex]->getCenter().z - MasterList[masterIndex]->getDistance());
		objVect -= camPos;
		objDistance = D3DXVec3LengthSq( &objVect );

		if(MasterList[masterIndex]->isTransparent())
		{
			if(Transparent.size() == 0)
				Transparent.push_back(MasterList[masterIndex]);
			else
			{
				bool binserted = false;
				for(std::deque<IObject*>::iterator theIterator = Transparent.begin(); theIterator != Transparent.end(); theIterator++)
				{
					IObject* IObjectKey = *theIterator;
					// get itrPosition
					itrVect.x = IObjectKey->getCenter().x - IObjectKey->getDistance();
					itrVect.y = IObjectKey->getCenter().y - IObjectKey->getDistance();
					itrVect.z = IObjectKey->getCenter().z - IObjectKey->getDistance();
					itrVect -= camPos;
					itrDistance = D3DXVec3LengthSq( &itrVect );

					// back to front
					if(itrDistance < objDistance)
					{
						Transparent.insert(theIterator,MasterList[masterIndex]);
						binserted = true;
						break;
					}
				}
				if(!binserted)
					Transparent.push_back(MasterList[masterIndex]);
			}
		}
		else
		{
			if(Opaque.size() == 0)
				Opaque.push_back(MasterList[masterIndex]);
			else
			{
				bool binserted = false;
				for(std::deque<IObject*>::iterator theIterator = Opaque.begin(); theIterator != Opaque.end(); theIterator++)
				{
					IObject* IObjectKey = *theIterator;
					// get itrPosition
					itrVect.x = IObjectKey->getCenter().x - IObjectKey->getDistance();
					itrVect.y = IObjectKey->getCenter().y - IObjectKey->getDistance();
					itrVect.z = IObjectKey->getCenter().z - IObjectKey->getDistance();
					itrVect -= camPos;
					itrDistance = D3DXVec3LengthSq( &itrVect );

					// front to back
					if(itrDistance > objDistance)
					{
						Opaque.insert(theIterator,MasterList[masterIndex]);
						binserted = true;
						break;
					}
				}
				if(!binserted)
					Opaque.push_back(MasterList[masterIndex]);
			}
		}
	}

	// HACK for water
	if (bFoundWater)
	{
		if (pSinkA)
			Transparent.push_front(pSinkA);
		if (pSinkB)
			Transparent.push_front(pSinkB);
	}
}

void SceneManager::clear()
{
	clearOpaque();
	clearTransparent();
	unsigned int size = MasterList.size();
	for(unsigned int i = 0; i < size; ++i)
	{
		if(MasterList[i] != NULL)
		{
			delete MasterList[i];
			MasterList[i] = NULL;
		}
	}
	MasterList.clear();
}

void SceneManager::clearOpaque()
{
	while (!Opaque.empty())
	{
		Opaque.pop_back();
	}
	Opaque.clear();
}

void SceneManager::clearTransparent()
{
	while (!Transparent.empty())
	{
		Transparent.pop_back();
	}
	Transparent.clear();
}

bool SceneManager::inFrustum(IObject *object)
{
	if(object)
	{
		D3DXPLANE currPlane;
		float pos[3];
		float radius = object->getDistance();
		D3DXVECTOR3 normal;

		for(unsigned int plane = 0; plane < 6; plane++)
		{
			//if(D3DXPlaneDotCoord(MainCamera->GetFrustum(), &rj_mathlib::getWAxis(&object->GetWorldMat())) + MainCamera->GetFrustum()[planes].d < 0)
				//return false;

			currPlane = MainCamera->GetFrustum()[plane];
			normal = D3DXVECTOR3(-currPlane.a, -currPlane.b, -currPlane.c);
			D3DXVec3Normalize(&normal, &normal);

			// move center towards plane by obj.radius
			pos[0] = object->getCenter().x;// + ( radius * normal.x);
			pos[1] = object->getCenter().y;// + ( radius * normal.y);
			pos[2] = object->getCenter().z;// + ( radius * normal.z);

			// Plane equation: (s = Ax + By + Cz + D)
			if ( ((currPlane.a*pos[0]) + (currPlane.b*pos[1]) + (currPlane.c*pos[2]) + currPlane.d) > 0 )
				return false;
		}
		return true;
	}
	return false;
}

void SceneManager::loadLevel(char* szFile)
{
	if(szFile)
	{
		// SceneData.scn /////////////////////////////////////////////////////
		/*
		-- number of Items in scene
		for each (Item)
		-- length of name + '0' (unsigned int)
		-- name + '0' (char*)
		-- material ID (unsigned int)
		-- transparent (bool)
		-- xAxis (xyzw) (4 x float)
		-- yAxis (xyzw) (4 x float)
		-- zAxis (xyzw) (4 x float)
		-- wAxis (xyzw) (4 x float)
		-- length of lightmap fileName + '0' (unsigned int)
		-- lightmap name (char*)
		*/////////////////////////////////////////////////////////////////////

		//int popbackcounter = 0;
		unsigned int numObjects = 0;
		bool isTransparent = false;
		char szName[128];//, szLightMapName[128];
		unsigned int strlen = 0;
		unsigned int id = 0;
		float matrix[16];
		IObject* temp;
		float vCenter[3] = {0.0f, 0.0f, 0.0f};
		float fRadius = 0;

		std::ifstream in(szFile, std::ios_base::in | std::ios_base::binary);
		if(in.is_open())
		{
			in.read((char*)&numObjects, sizeof(numObjects));

			// Don't Do This: MasterList.resize(numObjects);
			/// <Do this instead:>
			unsigned int i = MasterList.size();
			//MasterList.resize(numObjects + i);
			//bool bLocFound = false;
			/// </"Do This">

			//Opaque.resize(numObjects);
			//Transparent.resize(numObjects);
			for(unsigned int objects = 0; objects < numObjects; objects++)
			{
				in.read((char*)&strlen, sizeof(strlen));
				//szName = new char[strlen];
				in.read(szName, sizeof(char) * strlen);
				in.read((char*)&id, sizeof(id));
				in.read((char*)&isTransparent, sizeof(isTransparent));
				for(int index = 0; index < 16; index++)
					in.read((char*)&matrix[index], sizeof(float));
				//in.read((char*)&strlen, sizeof(strlen));
				//szLightMapName = new char[strlen];
				//in.read(szLightMapName, sizeof(char) * strlen);
				in.read((char*)&vCenter[0], sizeof(float));
				in.read((char*)&vCenter[1], sizeof(float));
				in.read((char*)&vCenter[2], sizeof(float));
				in.read((char*)&fRadius, sizeof(float));

				temp = new ISceneObject();
				temp->setName(szName);
				temp->SetWorldMat(matrix);
				temp->setTransparent(isTransparent);
				temp->SetID(id);
				temp->SetCenter( D3DXVECTOR3(vCenter[0], vCenter[1], vCenter[2]) );
				temp->setDistance(fRadius);
				//temp->SetLM(szLightMapName);	//ISceneObject needs this

				bool bInclude = true;
				if(strcmp(szName, "GummyWormLoc") == 0)
				{
					//m_mStartingPoint = temp->GetWorldMat();
					//temp->setName("C_GummyWorm");
					bInclude = false;
					//bLocFound = true;
					m_mGummyMat = temp->GetWorldMat();
					//popbackcounter++;
					delete temp;
				}
				else if(strcmp(szName, "AnthonyStartLoc") == 0)
				{
					bInclude = false;
					//bLocFound = true;
					m_mStartingPoint = temp->GetWorldMat();
					//popbackcounter++;
					delete temp;
				}
				else if(strcmp(szName, "PartyAnt") == 0)
				{
					bInclude = false;
					volatile tAddSpectators* tObjC = new tAddSpectators(EM_ADDSPECTATOR, temp->GetWorldMat());
					CEventManager::GetInstanceThreaded()->SendEventUpdateCrossToRender(ET_ADDSPECTATOR, (tBaseEventData*)tObjC);
					tObjC = NULL;
					delete temp;
				}
				else if(strcmp(szName, "PartyAntSit") == 0)
				{
					bInclude = false;
					volatile tAddSpectators* tObjC = new tAddSpectators(EM_ADDSPECTATOR, temp->GetWorldMat(), true);
					CEventManager::GetInstanceThreaded()->SendEventUpdateCrossToRender(ET_ADDSPECTATOR, (tBaseEventData*)tObjC);
					tObjC = NULL;
					delete temp;
				}
				// HACK FOR SUGAR TRANSPARANCY
				//	something I should've put in the exporter
				//	I'm sorry. - AtG
				if(strcmp(szName, "DM_SugarBit") == 0)
					temp->setTransparent(true);

				if (bInclude)
				{
					// load into Asset Manager
					string strName = temp->getName();
					tLoadMesh* tMesh = new tLoadMesh(EM_LOADMESH, strName);
					CEventManager::GetInstanceThreaded()->SendEventUpdateCrossToRender(ET_LOADMESH, (tBaseEventData*)tMesh);
					tMesh = NULL;

					MasterList.push_back(temp);
					i++;
				}
			}

			// load the camera joints
			numObjects = 0;
			in.read((char*)&numObjects, sizeof(numObjects));
			for(unsigned int _joints = 0; _joints < numObjects; _joints++)
			{
				D3DXMATRIX position;
				for(int index = 0; index < 16; index++)
					in.read((char*)&position[index], sizeof(float));

				m_qCameraList.push_back(position);
			}

			in.close();

			/*if (bLocFound)
			{
				for(int i = 0; i < popbackcounter; ++i)			
					MasterList.pop_back();
			}*/

		}
		// Load C_Cap.mesh
		tLoadMesh* tMesh = new tLoadMesh(EM_LOADMESH, "C_Red_Cap");
		CEventManager::GetInstanceThreaded()->SendEventUpdateCrossToRender(ET_LOADMESH, (tBaseEventData*)tMesh);
		tMesh = NULL;
		tMesh = new tLoadMesh(EM_LOADMESH, "C_Purple_Cap");
		CEventManager::GetInstanceThreaded()->SendEventUpdateCrossToRender(ET_LOADMESH, (tBaseEventData*)tMesh);
		tMesh = NULL;
		tMesh = new tLoadMesh(EM_LOADMESH, "C_Orange_Cap");
		CEventManager::GetInstanceThreaded()->SendEventUpdateCrossToRender(ET_LOADMESH, (tBaseEventData*)tMesh);
		tMesh = NULL;
		tMesh = new tLoadMesh(EM_LOADMESH, "C_Green_Cap");
		CEventManager::GetInstanceThreaded()->SendEventUpdateCrossToRender(ET_LOADMESH, (tBaseEventData*)tMesh);
		tMesh = NULL;
		// Load DM_Arrow.mesh
		tMesh = new tLoadMesh(EM_LOADMESH, "DM_Arrow");
		CEventManager::GetInstanceThreaded()->SendEventUpdateCrossToRender(ET_LOADMESH, (tBaseEventData*)tMesh);
		tMesh = NULL;
	}

	// load collision file
	string temp;
	temp = szFile;
	//char* file_bnc = szFile;
	int length = temp.length(); //strlen(szFile);
	int i = length - 3;
	temp[i] = 'b'; i++;
	temp[i] = 'n'; i++;
	temp[i] = 'c';

	m_tCollisionMachine.LoadData(temp.c_str());

	/*	DRAW DEBUG INFO
	for (unsigned int i = 0; i < m_tCollisionMachine.GetSolidList().size(); i++)
	MasterList.push_back( (IObject*)m_tCollisionMachine.GetSolidList()[i] ); */
	//D3DXMATRIX mat1; D3DXMATRIX mat2; D3DXMATRIX mat3; D3DXMATRIX mat4;
	//D3DXMatrixIdentity(&mat1);D3DXMatrixIdentity(&mat2);D3DXMatrixIdentity(&mat3);
	//mat1._41 = -1.0f; mat2._41 = 4.0f; mat3._41 = 2.0f;
	//mat1._42 = 0.0f; mat2._42 = 0.0f; mat3._42 = 0.0f;
	//mat1._43 = -5.0f; mat2._43 = 0.0f; mat3._43 = -4.0f;
	//mat1._44 = 1.0f; mat2._44 = 1.0f; mat3._44 = 1.0f;
	//volatile tAddSpectators* tObj = new tAddSpectators(EM_ADDSPECTATOR, mat1);
	//CEventManager::GetInstanceThreaded()->SendEventUpdateCrossToRender(ET_ADDSPECTATOR, (tBaseEventData*)tObj);
	//tObj = NULL;
	//volatile tAddSpectators* tObjB = new tAddSpectators(EM_ADDSPECTATOR, mat2);
	//CEventManager::GetInstanceThreaded()->SendEventUpdateCrossToRender(ET_ADDSPECTATOR, (tBaseEventData*)tObjB);
	//tObjB = NULL;
	//volatile tAddSpectators* tObjC = new tAddSpectators(EM_ADDSPECTATOR, mat3);
	//CEventManager::GetInstanceThreaded()->SendEventUpdateCrossToRender(ET_ADDSPECTATOR, (tBaseEventData*)tObjC);
	//tObjC = NULL;
}

void SceneManager::sendObjectEvent()
{
	volatile tObjectInFrustumList* tObj = new tObjectInFrustumList(EM_OBJECTINFRUSTUMLIST, Opaque, Transparent);
	CEventManager::GetInstanceThreaded()->SendEventUpdateCrossToRender(ET_OBJECTINFRUSTUMLIST, (tBaseEventData*)tObj);
	tObj = NULL;
}
void SceneManager::sendClearListEvent(void)
{
	volatile tClearList* tObj = new tClearList(EM_CLEARLIST);
	CEventManager::GetInstanceThreaded()->SendEventUpdateCrossToRender(ET_CLEARLIST, (tBaseEventData*)tObj);
	tObj = NULL;
}
bool SceneManager::LoadSettings(bool bLoadTut)
{
	// parse file
	std::fstream file;
	file.open("Resources/Data/data.sav", std::ios_base::in | std::ios_base::binary);

	if (file.is_open())
	{
		// load info
		file.read( (char*)&fFX, sizeof(float) );
		file.read( (char*)&fMusic, sizeof(float) );
		file.read( (char*)&fBrightness, sizeof(float) );
		if(bLoadTut)
			file.read( (char*)&m_bTutorial, sizeof(float) );

		file.close();
		return true;
	}

	return false;
}

bool SceneManager::SaveSettings(bool bReset)
{
	std::fstream file;
	file.open("Resources/Data/data.sav", std::ios_base::out | std::ios_base::trunc | std::ios_base::binary);

	if (file.is_open())
	{
		if (bReset)
		{
			fFX = 50.0f;
			fMusic = 50.0f;
			fBrightness = 0.0f;
		}

		file.write( (char*)&fFX, sizeof(float) );
		file.write( (char*)&fMusic, sizeof(float) );
		file.write( (char*)&fBrightness, sizeof(float) );
		file.write( (char*)&m_bTutorial, sizeof(bool) );

		file.close();
		return true;
	}

	return false;
}