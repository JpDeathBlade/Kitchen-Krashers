#ifndef EVENT_DEFINITIONS_H_ 
#define EVENT_DEFINITIONS_H_ 
#include <Windows.h>
#include <d3d9.h>
#include <d3dx9.h>
#include <deque>
#include "../Base Classes/IObject.h"
#include "../../../RJ_Config.h"
#include <string>
using std::string;
using std::deque;

enum EVENTTYPE{ET_NA, ET_UPDATESPERSECOND, ET_INPUT, ET_CAMERA, ET_OBJECTINFRUSTUMLIST, ET_PLAYSOUND,
				ET_LOADMESH, ET_LOADANIMMESH, ET_PLAYERDATA, ET_RESTARTGAME, ET_RESETHUD, ET_RESETPOW,
				ET_SETPOW, ET_SETHAZARD, ET_STONELAUNCHED, ET_BROWNIEDATA, ET_NUMPLAYERS, ET_LEVELSELECT,
				ET_PAUSE, ET_GAMEPLAYSHUTDOWN, ET_TOGGLEALLBITS, ET_TOGGLEBIT, ET_TOGGLEPOW, ET_TOGGLEALLPOW,
				ET_PLAYEFFECT, ET_STOPSOUND, ET_MENUCAMERA, ET_CAMERAPOP, ET_CAMERADONEMOVING, ET_CREATELINK,
				ET_BREAKLINK, ET_CLEARLIST, ET_LISTCLEARED, ET_FORWARD, ET_STOPEFFECT, ET_ADJUSTVOLUME,
				ET_ADJUSTGAMMA, ET_CAMDROP, ET_ANIMSTATECHANGE, ET_CAMFLY, ET_GOALEFFECT, ET_GOALPOS,
				ET_RENDERSTUFF, ET_ANIMCOLLISIONDONE, ET_MOTION, ET_TUTOVER, ET_TOGGLETUT, ET_CHERRYSHOT, ET_FOULSHOT,
				ET_NOMULTIPLIER, ET_PLAYERGOTCREAM, ET_SCENEPAUSE, ET_PUSHTURNOVER, ET_PUSHGAMEOVER, ET_PUSHHIGHSCORES,
				ET_POPSTATE, ET_ADDSPECTATOR, ET_CLEARSPECTATORS, ET_JELLOWIGGLE, ET_CREATESPEC, ET_RETURNID, ET_LOADINGDONE,
				ET_SETGUM, ET_SETTURKEY, ET_NOPRESSUP, ET_CURRENTVOLUME, ET_ENTERLEVEL};

enum DATAMSGTYPE{EM_NA, EM_UPDATESPERSECOND, EM_INPUT, EM_CAMERA, EM_OBJECTINFRUSTUMLIST, EM_PLAYSOUNDID,
				EM_PLAYSOUNDENUM, EM_LOADMESH, EM_LOADANIMMESH, EM_PLAYERDATA, EM_RESTARTGAME, EM_RESETHUD,
				EM_RESETPOW, EM_SETPOW, EM_SETHAZARD, EM_STONELAUNCHED, EM_BROWNIEDATA, EM_NUMPLAYERS,
				EM_LEVELSELECT, EM_PAUSE, EM_GAMEPLAYSHUTDOWN, EM_TOGGLEALLBITS, EM_TOGGLEBIT, EM_TOGGLEPOW,
				EM_TOGGLEALLPOW, EM_PLAYEFFECT, EM_STOPSOUNDID, EM_STOPSOUNDENUM, EM_MENUCAMERA, EM_CAMERAPOP,
				EM_CAMERADONEMOVING, EM_CREATELINK, EM_BREAKLINK, EM_CLEARLIST, EM_LISTCLEARED, EM_FORWARD,
				EM_STOPEFFECT, EM_ADJUSTVOLUME, EM_ADJUSTGAMMA, EM_CAMDROP, EM_ANIMSTATECHANGE, EM_CAMFLY,
				EM_GOALEFFECT, EM_GOALPOS, EM_RENDERSTUFF, EM_ANIMCOLLISIONDONE, EM_MOTION, EM_TUTOVER, EM_TOGGLETUT,
				EM_CHERRYSHOT, EM_FOULSHOT, EM_NOMULTIPLIER, EM_PLAYERGOTCREAM, EM_SCENEPAUSE, EM_PUSHTURNOVER,
				EM_PUSHGAMEOVER, EM_PUSHHIGHSCORES, EM_POPSTATE, EM_ADDSPECTATOR, EM_CLEARSPECTATORS, EM_JELLOWIGGLE,
				EM_CREATESPEC, EM_RETURNID, EM_LOADINGDONE, EM_SETGUM, EM_SETTURKEY, EM_NOPRESSUP, EM_CURRENTVOLUME,
				EM_ENTERLEVEL};

//BASE
struct tBaseEventData
{
	DATAMSGTYPE m_ID;
public:
	tBaseEventData()
	{
		m_ID = EM_NA;
	}
	tBaseEventData(DATAMSGTYPE id)
	{
		m_ID = id;
	}
	virtual ~tBaseEventData(){}
	DATAMSGTYPE GetMsgID() {return m_ID;}
};

struct tUpdatesPerSecond : public tBaseEventData
{
	float m_fUpdatesPS;
public:
	tUpdatesPerSecond(DATAMSGTYPE id, float fUpdatesPS) : tBaseEventData(EM_UPDATESPERSECOND)
	{
		m_ID = id;
		m_fUpdatesPS = fUpdatesPS;
	}
	~tUpdatesPerSecond(){m_fUpdatesPS = 0;}
};

struct tInput : public tBaseEventData
{
	INT64 m_n64KD;
	INT64 m_n64KP;
	float m_fMposX;
	float m_fMposY;
public:
	tInput()
	{
		m_ID = EM_NA;
		m_n64KD = 0;
		m_n64KP = 0;
		m_fMposX = 0;
		m_fMposY = 0;
	}
	tInput(DATAMSGTYPE id, INT64 n64KD, INT64 n64KP, float fMposX, float fMposY) : tBaseEventData(EM_INPUT)
	{
		m_ID = id;
		m_n64KD = n64KD;
		m_n64KP = n64KP;
		m_fMposX = fMposX;
		m_fMposY = fMposY;
	}
	~tInput(){m_n64KD = 0; m_n64KP = 0; m_fMposX = 0; m_fMposY = 0;}
};

struct tCameraUpdate : public tBaseEventData
{
	D3DXMATRIX m_mProjectionMatrix;
	D3DXMATRIX m_mViewMatrix;
	D3DXMATRIX m_mCamera;
	float m_fPower;
	float m_fAngle;
	float m_fSpeed;
	float m_fStoneAngle;
	D3DXVECTOR3 m_vStoneForward;
	unsigned int m_nScore;
public:
	tCameraUpdate(DATAMSGTYPE id, D3DXMATRIX mProjectionMatrix, D3DXMATRIX mViewMatrix, D3DXMATRIX mCamera,
		float fPower, float fAngle, float fSpeed, float fStoneAngle, D3DXVECTOR3 vStoneForward, unsigned int nScore = 0) : tBaseEventData(EM_CAMERA)
	{
		m_ID = id;
		m_mProjectionMatrix = mProjectionMatrix;
		m_mViewMatrix = mViewMatrix;
		m_mCamera = mCamera;
		m_fPower = fPower;
		m_fAngle = fAngle;
		m_fSpeed = fSpeed;
		m_fStoneAngle = fStoneAngle;
		m_vStoneForward = vStoneForward;
		m_nScore = nScore;
	}
	~tCameraUpdate(){}
};

struct tObjectInFrustumList : public tBaseEventData
{
	deque<IObject*>	m_dqObjectListOpaque;
	deque<IObject*>	m_dqObjectListTransparent;
public:
	tObjectInFrustumList(DATAMSGTYPE id, deque<IObject*> dqObjectListOpaque,
		deque<IObject*> dqObjectListTransparent) : tBaseEventData(EM_OBJECTINFRUSTUMLIST)
	{
		m_ID = id;
		m_dqObjectListOpaque = dqObjectListOpaque;
		m_dqObjectListTransparent = dqObjectListTransparent;
	}
	~tObjectInFrustumList(){}
};

struct tPlaySoundID : public tBaseEventData
{
	int m_nSoundID;
	bool m_bIfNotPlay;
public:
	tPlaySoundID(DATAMSGTYPE id, int nSoundID, bool bIfNotPlay = false) : tBaseEventData(EM_PLAYSOUNDID)
	{
		m_ID = id;
		m_nSoundID = nSoundID;
		m_bIfNotPlay = bIfNotPlay;
	}
	~tPlaySoundID(){}
};

struct tPlaySoundEnum : public tBaseEventData
{
	RJ_Config::SOUND_FILE m_sfSoundEnum;
	bool m_bIfNotPlay;
public:
	tPlaySoundEnum(DATAMSGTYPE id, RJ_Config::SOUND_FILE sfSoundEnum, bool bIfNotPlay = false) : tBaseEventData(EM_PLAYSOUNDENUM)
	{
		m_ID = id;
		m_sfSoundEnum = sfSoundEnum;
		m_bIfNotPlay = bIfNotPlay;
	}
	~tPlaySoundEnum(){}
};

struct tStopSoundID : public tBaseEventData
{
	int m_nSoundID;
public:
	tStopSoundID(DATAMSGTYPE id, int nSoundID) : tBaseEventData(EM_PLAYSOUNDID)
	{
		m_ID = id;
		m_nSoundID = nSoundID;
	}
	~tStopSoundID(){}
};

struct tStopSoundEnum : public tBaseEventData
{
	RJ_Config::SOUND_FILE m_sfSoundEnum;
public:
	tStopSoundEnum(DATAMSGTYPE id, RJ_Config::SOUND_FILE sfSoundEnum) : tBaseEventData(EM_PLAYSOUNDENUM)
	{
		m_ID = id;
		m_sfSoundEnum = sfSoundEnum;
	}
	~tStopSoundEnum(){}
};

struct tLoadMesh : public tBaseEventData
{
	string	m_szMeshName;
public:
	tLoadMesh(DATAMSGTYPE id, string szMeshName) : tBaseEventData(EM_LOADMESH)
	{
		m_ID = id;
		m_szMeshName = szMeshName;
	}
	~tLoadMesh(){}
};
struct tLoadAnimMesh : public tBaseEventData
{
	string	m_szMeshName;
public:
	tLoadAnimMesh(DATAMSGTYPE id, string szMeshName) : tBaseEventData(EM_LOADANIMMESH)
	{
		m_ID = id;
		m_szMeshName = szMeshName;
	}
	~tLoadAnimMesh(){}
};

struct tPlayerData : public tBaseEventData
{
	int			m_nCurrPlayer;
	int			m_nCurrPlayerMultiplier;
	int			m_nCurrStoneScore;
	int			m_eCurrPowerUp;
	int			m_nCurrHazard;
	int			m_nCurrVolSpeed;
	float		m_fSpeed;
	int			m_nScores[4];
	int			m_nRemainingShots[4];
	int			m_nStanding[4];
	int			m_nLevelID;
	int			m_nPar;
	bool		m_bGameOver;
	bool		m_bFortuneCookie;

public:
	tPlayerData(void)
	{
		m_nCurrHazard = -1;
		m_nCurrVolSpeed = 0;
		m_nCurrPlayer = 0;
		m_nCurrStoneScore = 0;
		m_fSpeed = 0;
		m_eCurrPowerUp = 0;
		m_nScores[0] = 0;
		m_nScores[1] = 0;
		m_nScores[2] = 0;
		m_nScores[3] = 0;
		m_nRemainingShots[0] = 0;
		m_nRemainingShots[1] = 0;
		m_nRemainingShots[2] = 0;
		m_nRemainingShots[3] = 0;
		m_nStanding[0] = 0;
		m_nStanding[1] = 0;
		m_nStanding[2] = 0;
		m_nStanding[3] = 0;
		m_nLevelID = 0;
		m_nPar = 0;
		m_bGameOver = false;
		m_bFortuneCookie = false;
		m_nCurrPlayerMultiplier = -1;
	}
	tPlayerData(DATAMSGTYPE id, int nCurrPlayer, int nCurrScore, float fSpeed, int nHazard, int nVolSpeed, int ePowerup, bool bFortuneCookie, int nScores[4],
			int nRemainingShots[4], int nStanding[4], int nLevelID, int nPar = -1,
			int nCurrPlayerMultiplier = 1, bool bGameOver = false) : tBaseEventData(EM_PLAYERDATA)
	{
		m_ID = id;
		m_nCurrHazard = nHazard;
		m_nCurrVolSpeed = nVolSpeed;
		m_nCurrPlayer = nCurrPlayer;
		m_nCurrStoneScore = nCurrScore;
		m_fSpeed = fSpeed;
		m_eCurrPowerUp = ePowerup;
		m_nScores[0] = nScores[0];m_nScores[1] = nScores[1];
		m_nScores[2] = nScores[2];m_nScores[3] = nScores[3];
		m_nRemainingShots[0] = nRemainingShots[0];m_nRemainingShots[1] = nRemainingShots[1];
		m_nRemainingShots[2] = nRemainingShots[2];m_nRemainingShots[3] = nRemainingShots[3];
		m_nStanding[0] = nStanding[0];m_nStanding[1] = nStanding[1];
		m_nStanding[2] = nStanding[2];m_nStanding[3] = nStanding[3];
		m_nLevelID = nLevelID;
		m_bGameOver = bGameOver;
		m_bFortuneCookie = bFortuneCookie;
		m_nPar = nPar; 
		m_nCurrPlayerMultiplier = nCurrPlayerMultiplier;
	}
	~tPlayerData(){}
};

struct tRestartGame : public tBaseEventData
{
public:
	tRestartGame(void){}
	tRestartGame(DATAMSGTYPE id) : tBaseEventData(EM_RESTARTGAME)
	{
		m_ID = id;
	}
	~tRestartGame(){}
};

struct tResetHUD : public tBaseEventData
{
public:
	tResetHUD(void){}
	tResetHUD(DATAMSGTYPE id) : tBaseEventData(EM_RESETHUD)
	{
		m_ID = id;
	}
	~tResetHUD(){}
};

struct tResetPowerUp : public tBaseEventData
{
public:
	tResetPowerUp(void){}
	tResetPowerUp(DATAMSGTYPE id) : tBaseEventData(EM_RESETPOW)
	{
		m_ID = id;
	}
	~tResetPowerUp(){}
};

struct tPowerUpData : public tBaseEventData
{
	int m_nPowerUp;

public:
	tPowerUpData(void){ m_nPowerUp = -1; }
	tPowerUpData(DATAMSGTYPE id, int nPow) : tBaseEventData(EM_SETPOW)
	{
		m_ID = id;
		m_nPowerUp = nPow;
	}
	~tPowerUpData(){}
};

struct tHazardData : public tBaseEventData
{
	int m_nHazardID;

public:
	tHazardData(void){ m_nHazardID = -1; }
	tHazardData(DATAMSGTYPE id, int nHazard) : tBaseEventData(EM_SETHAZARD)
	{
		m_ID = id;
		m_nHazardID = nHazard;
	}
	~tHazardData(){}
};

struct tStoneLaunched : public tBaseEventData
{
public:
	tStoneLaunched(void){}
	tStoneLaunched(DATAMSGTYPE id) : tBaseEventData(EM_STONELAUNCHED)
	{
		m_ID = id;
	}
	~tStoneLaunched(){}
};

struct tBrownieData : public tBaseEventData
{
	int		m_nValue;
	char*	m_szName;

public:
	tBrownieData(void)
	{
		m_nValue = 0;
		m_szName = 0;
	}
	tBrownieData(DATAMSGTYPE id, char* name, int value) : tBaseEventData(EM_BROWNIEDATA)
	{
		m_ID = id;
		m_szName = name;
		m_nValue = value;
	}
	~tBrownieData(){}
};

struct tNumPlayers : public tBaseEventData
{
	int	m_nNumPlayers;
public:
	tNumPlayers(void)
	{
		m_nNumPlayers = 0;
	}
	tNumPlayers(DATAMSGTYPE id, int	nNumPlayers) : tBaseEventData(EM_NUMPLAYERS)
	{
		m_ID = id;
		m_nNumPlayers = nNumPlayers;
	}
	~tNumPlayers(){}
};

struct tLevelSelect : public tBaseEventData
{
	int m_nLevelSelect;
public:
	tLevelSelect(void)
	{
		m_nLevelSelect = 0;
	}
	tLevelSelect(DATAMSGTYPE id, int nLevelSelect) : tBaseEventData(EM_LEVELSELECT)
	{
		m_ID = id;
		m_nLevelSelect = nLevelSelect;
	}
	~tLevelSelect(){}
};

struct tPause : public tBaseEventData
{
	bool m_bIsPause;
public:
	tPause(void)
	{
		m_bIsPause = false;
	}
	tPause(DATAMSGTYPE id, bool bIsPause = false) : tBaseEventData(EM_PAUSE)
	{
		m_ID = id;
		m_bIsPause = bIsPause;
	}
	~tPause(){}
};

struct tScenePause : public tBaseEventData
{
	bool m_bIsPause;
public:
	tScenePause(void)
	{
		m_bIsPause = false;
	}
	tScenePause(DATAMSGTYPE id, bool bIsPause = false) : tBaseEventData(EM_SCENEPAUSE)
	{
		m_ID = id;
		m_bIsPause = bIsPause;
	}
	~tScenePause(){}
};

struct tCamFly : public tBaseEventData
{
public:
	tCamFly(void)
	{
	}
	tCamFly(DATAMSGTYPE id) : tBaseEventData(EM_CAMFLY)
	{
		m_ID = id;
	}
	~tCamFly(){}
};
struct tCherryShot : public tBaseEventData
{
public:
	tCherryShot(void)
	{
	}
	tCherryShot(DATAMSGTYPE id) : tBaseEventData(EM_CHERRYSHOT)
	{
		m_ID = id;
	}
	~tCherryShot(){}
};
struct tFoulShot : public tBaseEventData
{
public:
	tFoulShot(void)
	{
	}
	tFoulShot(DATAMSGTYPE id) : tBaseEventData(EM_FOULSHOT)
	{
		m_ID = id;
	}
	~tFoulShot(){}
};
struct tNoMultiplier : public tBaseEventData
{
	bool m_bValue;
public:
	tNoMultiplier(void)
	{
		m_bValue = true;
		m_ID = EM_NOMULTIPLIER;
	}
	tNoMultiplier(DATAMSGTYPE id, bool val = true) : tBaseEventData(EM_NOMULTIPLIER)
	{
		m_ID = id;
		m_bValue = val;
	}
	~tNoMultiplier(){}
};
struct tTutOver : public tBaseEventData
{
public:
	tTutOver(void)
	{
	}
	tTutOver(DATAMSGTYPE id) : tBaseEventData(EM_TUTOVER)
	{
		m_ID = id;
	}
	~tTutOver(){}
};
struct tToggleTut : public tBaseEventData
{
	bool m_bTutorial;
public:
	tToggleTut(void)
	{
		m_bTutorial = true;
	}
	tToggleTut(DATAMSGTYPE id, bool bTutorial) : tBaseEventData(EM_TOGGLETUT)
	{
		m_bTutorial = bTutorial;
		m_ID = id;
	}
	~tToggleTut(){}
};
struct tGamePlayShutdown : public tBaseEventData
{
	bool	m_bShutdown;
public:
	tGamePlayShutdown(void)
	{
		m_bShutdown = true;
	}
	tGamePlayShutdown(DATAMSGTYPE id, bool bShutdown = true) : tBaseEventData(EM_GAMEPLAYSHUTDOWN)
	{
		m_ID = id;
		m_bShutdown = bShutdown;
	}
	~tGamePlayShutdown(){}
};
struct tToggleAllBits : public tBaseEventData
{
	bool	m_bBitsOn;
public:
	tToggleAllBits(void)
	{
		m_bBitsOn = true;
	}
	tToggleAllBits(DATAMSGTYPE id, bool bBitsOn = true) : tBaseEventData(EM_TOGGLEALLBITS)
	{
		m_ID = id;
		m_bBitsOn = bBitsOn;
	}
	~tToggleAllBits(){}
};
struct tToggleBit : public tBaseEventData
{
	bool	m_bBitOn;
	unsigned int m_nId;
public:
	tToggleBit(void)
	{
		m_bBitOn = true;
		m_nId = 0;
	}
	tToggleBit(DATAMSGTYPE id, unsigned int nId, bool bBitOn = true) : tBaseEventData(EM_TOGGLEBIT)
	{
		m_ID = id;
		m_nId = nId;
		m_bBitOn = bBitOn;
	}
	~tToggleBit(){}
};
struct tToggleAllPOW : public tBaseEventData
{
	bool	m_bPOWOn;
public:
	tToggleAllPOW(void)
	{
		m_bPOWOn = true;
	}
	tToggleAllPOW(DATAMSGTYPE id, bool bPOWOn = true) : tBaseEventData(EM_TOGGLEALLPOW)
	{
		m_ID = id;
		m_bPOWOn = bPOWOn;
	}
	~tToggleAllPOW(){}
};
struct tTogglePOW : public tBaseEventData
{
	bool	m_bPOWOn;
	unsigned int m_nId;
	const char* m_szName;
public:
	tTogglePOW(void)
	{
		m_bPOWOn = true;
		m_nId = 0;
		m_szName = NULL;
	}
	tTogglePOW(DATAMSGTYPE id, unsigned int nId, const char* szName, bool bPOWOn = true) : tBaseEventData(EM_TOGGLEPOW)
	{
		m_ID = id;
		m_nId = nId;
		m_bPOWOn = bPOWOn;
		m_szName = szName;
	}
	~tTogglePOW(){}
};
struct tPlayEffect : public tBaseEventData
{
	bool							m_bAttached;
	D3DXVECTOR3						m_vPos;
	RJ_Config::RJ_Effect_Types		m_eType;
	int								m_nUniqueIDAttached;
	D3DXMATRIX						m_matAttachMat;
	RJ_Config::RJ_Effect_Hat		m_eHat;
	RJ_Config::RJ_Effect_Position   m_ePos;

public:
	tPlayEffect(void)
	{
		m_bAttached = false;
		m_vPos *= 0;
		m_eType = RJ_Config::RJ_UNDEFINED;
		m_nUniqueIDAttached = -1;
		m_matAttachMat *= 0;
	}
	tPlayEffect(DATAMSGTYPE id, bool bAttached, RJ_Config::RJ_Effect_Types eType,
		D3DXVECTOR3 vPos, RJ_Config::RJ_Effect_Hat eHat = RJ_Config::RJ_HAT_NONE,
		RJ_Config::RJ_Effect_Position ePos = RJ_Config::RJ_POS_NONE) : tBaseEventData(EM_PLAYEFFECT)
	{
		m_ID = id;
		m_bAttached = bAttached;
		m_eType = eType;
		m_vPos = vPos;
		m_matAttachMat *= 0;
		m_eHat = eHat;
		m_ePos = ePos;
	}
	tPlayEffect(DATAMSGTYPE id, bool bAttached, RJ_Config::RJ_Effect_Types eType,
		D3DXMATRIX matAttachMat, int nUniqueIDAttached) : tBaseEventData(EM_PLAYEFFECT)
	{
		m_ID = id;
		m_bAttached = bAttached;
		m_eType = eType;
		m_vPos *= 0;
		m_matAttachMat = matAttachMat;
		m_nUniqueIDAttached = nUniqueIDAttached;
	}
	~tPlayEffect(){}
};

struct tCameraData : public tBaseEventData
{
	D3DXMATRIX	m_mCamera;

public:
	tCameraData(void)
	{
		m_mCamera = D3DXMATRIX();
	}
	tCameraData(DATAMSGTYPE id, D3DXMATRIX cam) : tBaseEventData(EM_MENUCAMERA)
	{
		m_ID = id;
		m_mCamera = cam;
	}
	~tCameraData(){}
};

struct tCameraPop : public tBaseEventData
{
public:
	tCameraPop(void){}
	tCameraPop(DATAMSGTYPE id) : tBaseEventData(EM_CAMERAPOP){}
	~tCameraPop(){}
};
struct tCreateAnimLink : public tBaseEventData
{
	int m_gaID;
	RJ_Config::ANIM_TYPE m_aniType;
public:
	tCreateAnimLink(void)
	{
		m_gaID = -1;
		m_aniType = RJ_Config::RJ_ANIM_NA;
	}
	tCreateAnimLink(DATAMSGTYPE id, int gaID, RJ_Config::ANIM_TYPE aniType) : tBaseEventData(EM_CREATELINK)
	{
		m_ID = id;
		m_gaID = gaID;
		m_aniType = aniType;
	}
	~tCreateAnimLink(){}
};
struct tCameraDone : public tBaseEventData
{
public:
	tCameraDone(void){}
	tCameraDone(DATAMSGTYPE id) : tBaseEventData(EM_CAMERADONEMOVING){}
	~tCameraDone(){}
};
struct tBreakAnimLink : public tBaseEventData
{
	int m_gaID;
	RJ_Config::ANIM_TYPE m_aniType;
public:
	tBreakAnimLink(void)
	{
		m_gaID = -1;
		m_aniType = RJ_Config::RJ_ANIM_NA;
	}
	tBreakAnimLink(DATAMSGTYPE id, int gaID, RJ_Config::ANIM_TYPE aniType) : tBaseEventData(EM_BREAKLINK)
	{
		m_ID = id;
		m_gaID = gaID;
		m_aniType = aniType;
	}
	~tBreakAnimLink(){}
};

struct tClearList : public tBaseEventData
{
	
public:
	tClearList(void)
	{
	}
	tClearList(DATAMSGTYPE id) : tBaseEventData(EM_CLEARLIST)
	{
		m_ID = id;
	}
	~tClearList(){}
};

struct tListCleared : public tBaseEventData
{
public:
	tListCleared(void)
	{
	}
	tListCleared(DATAMSGTYPE id) : tBaseEventData(EM_LISTCLEARED)
	{
		m_ID = id;
	}
	~tListCleared(){}
};

struct tForward : public tBaseEventData
{
	D3DXVECTOR3 forward;
public:
	tForward(void)
	{
		forward.x = 0.0f;
		forward.y = 0.0f;
		forward.z = 0.0f;
	}
	tForward(DATAMSGTYPE id, D3DXVECTOR3 _forward) : tBaseEventData(EM_FORWARD)
	{
		m_ID = id;
		forward.x = _forward.x;
		forward.y = _forward.y;
		forward.z = _forward.z;
	}
	~tForward(){}
};

struct tStopEffects : public tBaseEventData
{
	RJ_Config::RJ_Effect_Types m_eType;

public:
	tStopEffects(void)
	{
		m_eType = RJ_Config::RJ_UNDEFINED;
	}
	tStopEffects(DATAMSGTYPE id, RJ_Config::RJ_Effect_Types eType) : tBaseEventData(EM_STOPEFFECT)
	{
		m_ID = id;
		m_eType = eType;
	}
	~tStopEffects(){}
};
struct tVolumeData : public tBaseEventData
{
	float m_fVolume;
	int m_eType;

public:
	tVolumeData(DATAMSGTYPE id, int eType, float fVolume) : tBaseEventData(EM_ADJUSTVOLUME)
	{
		m_ID = id;
		m_eType = eType;
		m_fVolume = fVolume;
	}
	~tVolumeData(){}
};
struct tGammaData : public tBaseEventData
{
	float m_fGamma;

public:
	tGammaData(DATAMSGTYPE id, float fGamma) : tBaseEventData(EM_ADJUSTGAMMA)
	{
		m_ID = id;
		m_fGamma = fGamma;
	}
	~tGammaData(){}
};
struct tCamDrop : public tBaseEventData
{
	bool m_bCamDrop;

public:
	tCamDrop(DATAMSGTYPE id, bool bCamDrop) : tBaseEventData(EM_CAMDROP)
	{
		m_ID = id;
		m_bCamDrop = bCamDrop;
	}
	~tCamDrop(){}
};

struct tAnimStateChange : public tBaseEventData
{
	int m_gaID;
	RJ_Config::ANIM_TYPE m_aniType;
	int m_nStateID;
	bool m_bRestart;
public:
	tAnimStateChange(void)
	{
		m_gaID = -1;
		m_aniType = RJ_Config::RJ_ANIM_NA;
		m_nStateID = 0;
		m_bRestart = true;
	}
	tAnimStateChange(DATAMSGTYPE id, int gaID, RJ_Config::ANIM_TYPE aniType, int nStateID, bool bRestart) : tBaseEventData(EM_ANIMSTATECHANGE)
	{
		m_ID = id;
		m_gaID = gaID;
		m_aniType = aniType;
		m_nStateID = nStateID;
		m_bRestart = bRestart;
	}
	~tAnimStateChange(){}
};

struct tGoalEffect : public tBaseEventData
{
	int m_Multiplier;

public:
	tGoalEffect(DATAMSGTYPE id, int _multiplier) : tBaseEventData(EM_GOALEFFECT)
	{
		m_ID = id;
		m_Multiplier = _multiplier;
	}
	~tGoalEffect(){}
};

struct tGoalPosition : public tBaseEventData
{
	D3DXVECTOR3 m_Position;

public:
	tGoalPosition(DATAMSGTYPE id, D3DXVECTOR3 _pos) : tBaseEventData(EM_GOALPOS)
	{
		m_ID = id;
		m_Position = _pos;
	}
	~tGoalPosition(){}
};
struct tRenderStuff : public tBaseEventData
{
	bool	m_bRenderSelection;
public:
	tRenderStuff(void)
	{
		m_bRenderSelection = true;
	}
	tRenderStuff(DATAMSGTYPE id, bool bRenderSelection = true) : tBaseEventData(EM_RENDERSTUFF)
	{
		m_ID = id;
		m_bRenderSelection = bRenderSelection;
	}
	~tRenderStuff(){}
};
struct tAnimCollisionDone : public tBaseEventData
{
	char	m_cDone;;
public:
	tAnimCollisionDone(void)
	{
		m_cDone = 'a';
	}
	tAnimCollisionDone(DATAMSGTYPE id) : tBaseEventData(EM_ANIMCOLLISIONDONE)
	{
		m_ID = id;
		m_cDone = 'a';
	}
	~tAnimCollisionDone(){}
};

struct tMotionLines : public tBaseEventData
{
	float timeToPlay;

public:
	tMotionLines(DATAMSGTYPE id, float _timeToPlay) : tBaseEventData(EM_MOTION)
	{
		m_ID = id;
		timeToPlay = _timeToPlay;
	}
	~tMotionLines(){}
};

struct tPlayerGotWC : public tBaseEventData
{
public:
	tPlayerGotWC(void)
	{
	}
	tPlayerGotWC(DATAMSGTYPE id) : tBaseEventData(EM_PLAYERGOTCREAM)
	{
		m_ID = id;
	}
	~tPlayerGotWC(){}
};

struct tPushTurnOver : public tBaseEventData
{
	int m_nCurrPlayer;
	int m_nBits;
	int m_nBounces;
	int m_nBrownie;
	int m_nMultiplier;
	int m_nPar;
public:
	tPushTurnOver(void)
	{
		m_nCurrPlayer = 0;
		m_nBits = 0;
		m_nBounces = 0;
		m_nBrownie = 0;
		m_nMultiplier = 0;
		m_nPar = 0;
	}
	tPushTurnOver(DATAMSGTYPE id, int nCurrPlayer,	int nBits,	int nBounces,	int nBrownie,	int nMultiplier,	int nPar) : tBaseEventData(EM_SCENEPAUSE)
	{
		m_ID = id;
		m_nCurrPlayer = nCurrPlayer;
		m_nBits = nBits;
		m_nBounces = nBounces;
		m_nBrownie = nBrownie;
		m_nMultiplier = nMultiplier;
		m_nPar = nPar;
	}
	~tPushTurnOver(){}
};

struct tPushGameOver : public tBaseEventData
{
public:
	tPushGameOver(void)
	{
		m_ID = EM_PUSHGAMEOVER;
	}
	tPushGameOver(DATAMSGTYPE id) : tBaseEventData(EM_PUSHGAMEOVER)
	{
		m_ID = id;
	}
	~tPushGameOver(){}
};
struct tPushHighScores : public tBaseEventData
{
	int m_nPlayerNum;
	int m_nIndex;
	int m_nCurrLevel;
	int m_nNumPlayers;
public:
	tPushHighScores(void)
	{
		m_nPlayerNum = 0;
		m_nIndex = 0;
		m_nCurrLevel = 0;
		m_nNumPlayers = 0;
		m_ID = EM_PUSHHIGHSCORES;
	}
	tPushHighScores(DATAMSGTYPE id, int nPlayerNum,	int nIndex,	int nCurrLevel,	int nNumPlayers) : tBaseEventData(EM_PUSHHIGHSCORES)
	{
		m_ID = id;
		m_nPlayerNum = nPlayerNum;
		m_nIndex = nIndex;
		m_nCurrLevel = nCurrLevel;
		m_nNumPlayers = nNumPlayers;
	}
	~tPushHighScores(){}
};

struct tPopState : public tBaseEventData
{
public:
	tPopState(void)
	{
		m_ID = EM_POPSTATE;
	}
	tPopState(DATAMSGTYPE id) : tBaseEventData(EM_POPSTATE)
	{
		m_ID = id;
	}
	~tPopState(){}
};

struct tClearSpectators : public tBaseEventData
{
public:
	tClearSpectators(void)
	{
		m_ID = EM_CLEARSPECTATORS;
	}
	tClearSpectators(DATAMSGTYPE id) : tBaseEventData(EM_CLEARSPECTATORS)
	{
		m_ID = id;
	}
	~tClearSpectators(){}
};

struct tAddSpectators : public tBaseEventData
{
	D3DXMATRIX m_mLocation;
	bool m_bSitting;
public:
	tAddSpectators(DATAMSGTYPE id, D3DXMATRIX mLocation, bool bSitting = false) : tBaseEventData(EM_ADDSPECTATOR)
	{
		m_ID = id;
		m_mLocation = mLocation;
		m_bSitting =  bSitting;
	}
	~tAddSpectators(){}
};

struct tCreateSpecType : public tBaseEventData
{
	int m_nGlobalID;
	int m_nTypeID;
public:
	tCreateSpecType(DATAMSGTYPE id, int nGlobalID, int nTypeID) : tBaseEventData(EM_ADDSPECTATOR)
	{
		m_ID = id;
		m_nGlobalID = nGlobalID;
		m_nTypeID =  nTypeID;
	}
	~tCreateSpecType(){}
};

struct tJelloWiggle : public tBaseEventData
{
	char* m_Name;
public:
	tJelloWiggle(DATAMSGTYPE id, char* _name) : tBaseEventData(EM_JELLOWIGGLE)
	{
		m_ID = id;
		m_Name = _name;
	}
	~tJelloWiggle(){}
};

struct tReturnID : public tBaseEventData
{
	int m_nGlobalID;
public:
	tReturnID(DATAMSGTYPE id, int nGlobalID) : tBaseEventData(EM_ADDSPECTATOR)
	{
		m_ID = id;
		m_nGlobalID = nGlobalID;
	}
	~tReturnID(){}
};

struct tLoadingDone : public tBaseEventData
{
public:
	tLoadingDone(DATAMSGTYPE id) : tBaseEventData(EM_ADDSPECTATOR)
	{
		m_ID = id;
	}
	~tLoadingDone(){}
};

struct tSetGum : public tBaseEventData
{
public:
	tSetGum(DATAMSGTYPE id) : tBaseEventData(EM_SETGUM)
	{
		m_ID = id;
	}
	~tSetGum(){}
};

struct tSetTurkey : public tBaseEventData
{
	bool m_bValue;
public:
	tSetTurkey(DATAMSGTYPE id, bool val) : tBaseEventData(EM_SETTURKEY)
	{
		m_ID = id;
		m_bValue = val;
	}
	~tSetTurkey(){}
};

struct tPressUpData : public tBaseEventData
{
public:
	tPressUpData(DATAMSGTYPE id) : tBaseEventData(EM_NOPRESSUP)
	{
		m_ID = id;
	}
	~tPressUpData(){}
};

struct tCurrentVolume : public tBaseEventData
{
	int m_eType;

public:
	tCurrentVolume(DATAMSGTYPE id, int eType) : tBaseEventData(EM_CURRENTVOLUME)
	{
		m_ID = id;
		m_eType = eType;
	}
	~tCurrentVolume(){}
};

struct tEnterLevel : public tBaseEventData
{
	bool m_Tut;
public:
	tEnterLevel(DATAMSGTYPE id, bool tut) : tBaseEventData(EM_ENTERLEVEL)
	{
		m_ID = id;
		m_Tut = tut;
	}
	~tEnterLevel(){}
};

#endif