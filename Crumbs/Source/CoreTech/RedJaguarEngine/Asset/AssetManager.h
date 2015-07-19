/**
* 0===========================================================================0
* | AssetManager.h                                                            |
* |  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  |
* |   Author        : Zac Andersen                                            |
* |   E-mail        : andersen@fullsail.edu                                   |
* |  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  |
* |   Date Created  : 09:25 am (Eastern) on 06/14/2010                        |
* |   Date Edited   : 00:00 pm (Eastern) on 04/30/2010                        |
* 0=====0===============================================================0=====0
*       |                                                               |      
* 0=====0===============================================================0=====0
* | Code Description:                                                         |
* |  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  |
* |   Managers all assets in the game.				                          |
* 0===========================================================================0
*/
#ifndef ASSETMANAGER_H_
#define ASSETMANAGER_H_

#define ASSETMANAGER CAssetManager::getInstanceRender()

#include "../Event/IListener.h"
#include "../../../RJ_Config.h"
#include "../Effects/Effect.h"
#include "..\\..\\..\\Gameplay\\Objects\\Mesh.h"
#include <d3d9.h>
#include <d3dx9.h>
#include <d3dx9effect.h>
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")

#include <deque>
using std::deque;
class Mesh;
class AnimMesh;
class AnimLink;
class CSpectator;

struct TexStruct
{
	int texID;
	int texWidth;
	int texHeight;
	char* texName;
	bool bLoaded;
	IDirect3DTexture9 * d3dTexture;
	int  nRef;
	TexStruct(){texWidth = 0; texHeight = 0; texID = -1; nRef = 0; d3dTexture= NULL; bLoaded = false;}
};
struct ShadeStruct
{
	int shadeID;
	char* shadeName;
	ID3DXEffect* d3dShader;
	int  nRef;
	ShadeStruct(){shadeID = -1; nRef = 0; d3dShader = NULL;}
};

class CAssetManager : public IListener
{
private:
	IDirect3DDevice9	*dev;
	ID3DXSprite			*m_lpSprite;
	CAssetManager(void);
	CAssetManager(CAssetManager& _ref);
	CAssetManager& operator= (CAssetManager& _ref);

	static CAssetManager m_Instance;
	static CAssetManager m_InstanceUpdate;
	//////////////////////////////////////////////////////////////////////////
	//Textures
	std::deque<TexStruct> texSVector;
	unsigned int m_nCurrentTexCount;
	std::deque<TexStruct> LOADtexSVector;
	unsigned int m_nLOADCurrentTexCount;
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	//Shader
	std::deque<ShadeStruct> shadeSVector;
	unsigned int m_nCurrentShaderCount;
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	//Object
	std::deque<Mesh*> MeshDeq;
	unsigned int m_nCurrentMeshCount;
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	//Animated Objects
	std::deque<AnimMesh*> AnimMeshDeq;
	unsigned int m_nCurrentAnimMeshCount;
	std::deque<AnimLink*> AnimLinkDeq;
	//////////////////////////////////////////////////////////////////////////

	std::deque<CSpectator*> AnimSpectatorDeq;
	RJ_Config::RJ_Effect_Hat eHat;

	//HAT JOINT
	D3DXMATRIX m_mHatJoint;

public:
	static CAssetManager* getInstanceRender(void) { return &m_Instance; }
	virtual ~CAssetManager(void) { }

	bool Initialize(void);
	bool Shutdown(void);
	void HandleEvent(CEvent* pEvent);
	bool CheckDevices();
	void Update(double dDelta);

	void onLostDevice(void);
	void onResetDevice(void);

	//////////////////////////////////////////////////////////////////////////
	//Texture Functions
	int LoadTexture(const char* Filename, D3DCOLOR dwColorkey = 0,  bool bIsLoad = false);
	IDirect3DTexture9* GetTexture(unsigned int nTexID);
	IDirect3DTexture9* GetTexture(const char* Name);
	void RemoveTexture(int nTexID);
	void RemoveTexture(const char* Name);
	void ClearTextures();
	void ClearLoadTextures();
	int GetTextureWidth(int nID);
	int GetTextureHeight(int nID);
	bool DrawFlatSprite(int nID, float fX = 0.0f, float fY = 0.0f, float fScaleX = 1.0f, float fScaleY = 1.0f,
		RECT* pSection = 0, float fRotCenterX = 1.0f, float fRotCenterY = 1.0f, float fRotation = 0.0f, DWORD dwColor = D3DCOLOR_ARGB(255,255,255,255), bool bIsLoad = false);
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	//Shader Functions
	int LoadShader(char* Filename);
	ID3DXEffect* GetShader(unsigned int nShaderID);
	ID3DXEffect* GetShader(const char* Name);
	void AddShader(ID3DXEffect* _object, char* _name);
	void RemoveShader(int nShaderID);
	void RemoveShader(const char* Name);
	void ClearShaders();
	void onLostDeviceShader();
	void onResetDeviceShader();
	void CommitChanges(char* _name);
	void BeginPass(char* _name, int _pass);
	void EndPass(char* _name);
	void Begin(char* _name);
	void End(char* _name);
	void SetMatrix(char* _name, D3DXHANDLE _var, D3DXMATRIX* _matrix);
	void SetFloat(char* _name, D3DXHANDLE _var, float _float);
	void SetVector(char* _name, D3DXHANDLE _var, D3DXVECTOR4 _vector);
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	//Mesh Functions
	int LoadMesh(const char* Filename);
	Mesh* GetMesh(const char* Name);
	Mesh* GetMesh(RJ_Config::RJ_Mesh_Types _MeshType);
	void AddMesh(Mesh* _object);
	void RemoveMesh(const char* Name);
	void RemoveMesh(RJ_Config::RJ_Mesh_Types _MeshType);
	void ClearMeshes();
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	//AnimMesh Functions
	int LoadAnimMesh(const char* Filename);
	AnimMesh* GetAnimMesh(const char* Name);
	AnimMesh* GetAnimMesh(RJ_Config::RJ_Mesh_Types _MeshType);
	void AddAnimMesh(AnimMesh* _object);
	void RemoveAnimMesh(const char* Name);
	void RemoveAnimMesh(RJ_Config::RJ_Mesh_Types _MeshType);
	void ClearAnimMeshes();

	void AddAnimLink(int gaId, RJ_Config::ANIM_TYPE anitype);
	void ClearAnimLinks();
	void RemoveAnimLink(int gaId, RJ_Config::ANIM_TYPE anitype);
	AnimLink* GetAnimLink(int gaId, RJ_Config::ANIM_TYPE atAnimType);
	//AnimLink* GetAnimLink(const char* Name);
	//////////////////////////////////////////////////////////////////////////
	
	//////////////////////////////////////////////////////////////////////////
	//Spectator Functions
	void AddSpectator(D3DXMATRIX mLocation, bool sitting = false);
	void CreateSpectator(int nGlobalID, int nTypeID);
	void ClearSpectators();
	void RenderSpectators(double dDelta);
	void ClearCrowd();
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	//Effects lul
	Effect effectCollection;
	Effect effectScrape;
	Effect effectCollision;
	Effect effectSmoke1;
	Effect effectSmoke2;
	Effect effectSmoke3;
	Effect effectDrip;
	Effect effectJello;
	Effect effectSplat;
	Effect effectSugar;
	Effect effectHat;
	Effect effectSkid;
	Effect* GetEffect_Sugar(void) { return &effectSugar; }
	int m_currentVolume;

	bool bMotionLines;
	D3DXVECTOR3 motionPos;
	Mesh MotionLines;
	D3DXVECTOR3 forward;

	bool bDrip;
	D3DXVECTOR3 dripPos;
	Mesh Drip;

	Mesh goalRing1;
	Mesh goalRing2;
	Mesh goalRing3;
	Mesh goalRing4;
	bool bgoal1;
	bool bgoal2;
	bool bgoal3;
	bool bgoal4;
	D3DXVECTOR3 goalPos;

	void ResetEffects();
	void RenderEffects(double _dDelta);
	void LoadEffects();
	void ShutdownEffects();

	//////////////////////////////////////////////////////////////////////////
	//HAT JOINT
	D3DXMATRIX GetHatJoint(){return m_mHatJoint;}
	void SetHatJoint(D3DXMATRIX mHatJoint){m_mHatJoint = mHatJoint;}
};

#endif