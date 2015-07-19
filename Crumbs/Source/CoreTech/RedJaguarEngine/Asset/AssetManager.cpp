#include "AssetManager.h"
#include "../Base Classes/IObject.h"
#include "../../../Gameplay/Objects/Mesh.h"
#include "../../../Gameplay/Objects/AnimMesh.h"
#include "../../../Gameplay/Objects/CSpectator.h"
#include "../../../Gameplay/GameAnim/AState.h"
#include "../../../Gameplay/GameAnim/AnthonyAnimStates.h"
#include "../../../Gameplay/GameAnim/BroomAnimStates.h"
#include "../../../Gameplay/GameAnim/GummyAnimStates.h"
#include "../../../Gameplay/GameAnim/ToothPickAnimStates.h"
#include "../../../Gameplay/GameAnim/SpectatorAnimStates.h"
#include "../States/State_TurnOver.h"
#include "../States/State_HighScores.h"
#include "../States/State_Config.h"
#include "../../RJ_GameMain.h"
#include "../Animations/AnimManager.h"
#include "../Renderer/RJ_Renderer.h"
#include <fstream>

#ifndef SAFE_RELEASE
#define SAFE_RELEASE(p)			if (p) { p->Release(); p = NULL; }
#endif

CAssetManager CAssetManager::m_Instance;
CAssetManager CAssetManager::m_InstanceUpdate;

CAssetManager::CAssetManager(void)
{
	dev = NULL;
	m_lpSprite = NULL;
	m_nCurrentMeshCount = -1;
	m_nCurrentShaderCount = 0;
	m_nCurrentTexCount = 0;
	m_nLOADCurrentTexCount = 0;
	m_nCurrentAnimMeshCount = 0;
	m_currentVolume = 0;
}
bool CAssetManager::Initialize(void)
{
	dev = RJ_Renderer::GetInstance()->getDirect3DDevice();
	m_lpSprite = RJ_Renderer::GetInstance()->getSprite();

	LoadTexture("Error.dds");
	LoadMesh("Error");
	LoadShader("Jello.fx");

	// Register for event
	CEventManager::GetInstanceThreaded()->RegisterListenerRender(ET_LOADMESH, this);
	CEventManager::GetInstanceThreaded()->RegisterListenerRender(ET_LOADANIMMESH, this);
	CEventManager::GetInstanceThreaded()->RegisterListenerRender(ET_CREATELINK, this);
	CEventManager::GetInstanceThreaded()->RegisterListenerRender(ET_BREAKLINK, this);
	CEventManager::GetInstanceThreaded()->RegisterListenerRender(ET_PLAYEFFECT,this);
	CEventManager::GetInstanceThreaded()->RegisterListenerRender(ET_STOPEFFECT,this);
	CEventManager::GetInstanceThreaded()->RegisterListenerRender(ET_FORWARD,this);
	CEventManager::GetInstanceThreaded()->RegisterListenerRender(ET_ANIMSTATECHANGE,this);
	CEventManager::GetInstanceThreaded()->RegisterListenerRender(ET_GOALPOS,this);
	CEventManager::GetInstanceThreaded()->RegisterListenerRender(ET_PUSHTURNOVER,this);
	CEventManager::GetInstanceThreaded()->RegisterListenerRender(ET_PUSHHIGHSCORES,this);
	CEventManager::GetInstanceThreaded()->RegisterListenerRender(ET_GOALEFFECT,this);
	CEventManager::GetInstanceThreaded()->RegisterListenerRender(ET_JELLOWIGGLE,this);
	CEventManager::GetInstanceThreaded()->RegisterListenerRender(ET_ADDSPECTATOR,this);
	CEventManager::GetInstanceThreaded()->RegisterListenerRender(ET_CLEARSPECTATORS,this);
	CEventManager::GetInstanceThreaded()->RegisterListenerRender(ET_CREATESPEC,this);
	CEventManager::GetInstanceThreaded()->RegisterListenerRender(ET_LOADINGDONE,this);
	CEventManager::GetInstanceThreaded()->RegisterListenerRender(ET_CURRENTVOLUME,this);


	return true;
}
bool CAssetManager::Shutdown(void)
{
	ClearSpectators();
	ClearTextures();
	ClearShaders();
	ClearMeshes();
	ClearAnimMeshes();
	ClearAnimLinks();
	// Unregister for event
	CEventManager::GetInstanceThreaded()->UnregisterListenerAllRender(this);

	//SAFE_RELEASE(m_lpSprite);
	//SAFE_RELEASE(dev);
	return true;
}
void CAssetManager::Update(double dDelta)
{
	unsigned int animsize = AnimLinkDeq.size();
	for(unsigned int i = 0; i < animsize; ++i)
	{
		AnimLinkDeq[i]->Update(dDelta);
	}
	unsigned int specsize = AnimSpectatorDeq.size();
	for(unsigned int i = 0; i < specsize; ++i)
		AnimSpectatorDeq[i]->Render(dDelta);
}
bool CAssetManager::CheckDevices()
{
	return true;
}

void CAssetManager::onLostDevice(void)
{
	unsigned int _sizeMesh = MeshDeq.size();
	for (unsigned int _mesh = 0; _mesh < _sizeMesh; _mesh++)
	{
		MeshDeq[_mesh]->onLostDevice();
	}

	unsigned int _sizeShader = shadeSVector.size();
	for (unsigned int _shader = 0; _shader < _sizeShader; _shader++)
	{
		shadeSVector[_shader].d3dShader->OnLostDevice();
	}
}

void CAssetManager::onResetDevice(void)
{
	unsigned int _sizeMesh = MeshDeq.size();
	for (unsigned int _mesh = 0; _mesh < _sizeMesh; _mesh++)
	{
		MeshDeq[_mesh]->onResetDevice();
	}

	unsigned int _sizeShader = shadeSVector.size();
	for (unsigned int _shader = 0; _shader < _sizeShader; _shader++)
	{
		shadeSVector[_shader].d3dShader->OnResetDevice();
	}
}

// 0===========================================================================0
// | LoadTexture(char* Filename, D3DCOLOR dwColorkey);                         |
// |  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  |
// | Author: Zac Andersen (ZA)                                                 |
// | Last Edit: 6/14/2010                                                      |
// |  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  |
// | In:  Filename                                                             |
// |      dwColorkey                                                           |                                                              |
// | Out:		                                                               |
// |  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  |
// | Purpose: Loads in a Texture                                               |
// 0===========================================================================0

//////////////////////////////////////////////////////////////////////////
//Texture
int CAssetManager::LoadTexture(const char* Filename, D3DCOLOR dwColorkey, bool bIsLoad)
{
	if (!Filename)	
		return 0;
	if(bIsLoad)
	{
		for (unsigned int i = 0; i < m_nLOADCurrentTexCount; i++)
		{
			if (_stricmp(Filename, LOADtexSVector[i].texName) == 0)
			{
				LOADtexSVector[i].nRef++;
				return i;
			}
		}
		IDirect3DTexture9 *TexPointer = NULL;
		TexStruct TxStruct;
		char TempFile[256];
		strcpy_s(TempFile,sizeof(char)*64,"Resources\\Textures\\");
		strncat_s(TempFile, Filename, strlen(Filename));
		HRESULT hr = D3DXCreateTextureFromFileEx(dev, (LPCSTR)TempFile, 0, 0, D3DX_DEFAULT, 0,
			D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT,
			D3DX_DEFAULT, dwColorkey, 0, 0, &TexPointer);
		if (hr == S_OK)
		{
			TxStruct.texID = m_nLOADCurrentTexCount;
			TxStruct.nRef = 1;
			TxStruct.d3dTexture = TexPointer;
			unsigned int nLength = strlen(Filename) + 1;
			TxStruct.texName = new char[nLength];
			strcpy_s(TxStruct.texName, nLength, Filename);
			D3DSURFACE_DESC d3dSurfDesc;
			ZeroMemory(&d3dSurfDesc, sizeof(d3dSurfDesc));
			TxStruct.d3dTexture->GetLevelDesc(0, &d3dSurfDesc);
			TxStruct.texWidth	= d3dSurfDesc.Width;
			TxStruct.texHeight	= d3dSurfDesc.Height;
			TxStruct.bLoaded = true;
			m_nLOADCurrentTexCount++;
			LOADtexSVector.push_back(TxStruct);
			return (int)(m_nLOADCurrentTexCount - 1);
		}
	}
	else
	{
		for (unsigned int i = 0; i < m_nCurrentTexCount; i++)
		{
			if (_stricmp(Filename, texSVector[i].texName) == 0)
			{
				texSVector[i].nRef++;
				return i;
			}
		}
		IDirect3DTexture9 *TexPointer = NULL;
		TexStruct TxStruct;
		char TempFile[256];
		strcpy_s(TempFile,sizeof(char)*64,"Resources\\Textures\\");
		strncat_s(TempFile, Filename, strlen(Filename));
		HRESULT hr = D3DXCreateTextureFromFileEx(dev, (LPCSTR)TempFile, 0, 0, D3DX_DEFAULT, 0,
			D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT,
			D3DX_DEFAULT, dwColorkey, 0, 0, &TexPointer);
		if (hr == S_OK)
		{
			TxStruct.texID = m_nCurrentTexCount;
			TxStruct.nRef = 1;
			TxStruct.d3dTexture = TexPointer;
			unsigned int nLength = strlen(Filename) + 1;
			TxStruct.texName = new char[nLength];
			strcpy_s(TxStruct.texName, nLength, Filename);
			D3DSURFACE_DESC d3dSurfDesc;
			ZeroMemory(&d3dSurfDesc, sizeof(d3dSurfDesc));
			TxStruct.d3dTexture->GetLevelDesc(0, &d3dSurfDesc);
			TxStruct.texWidth	= d3dSurfDesc.Width;
			TxStruct.texHeight	= d3dSurfDesc.Height;
			TxStruct.bLoaded = true;
			m_nCurrentTexCount++;
			texSVector.push_back(TxStruct);
			return (int)(m_nCurrentTexCount - 1);
		}
	}
	return 0;
}
// 0===========================================================================0
// | GetTexture(char* Name);						                           |
// |  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  |
// | Author: Zac Andersen (ZA)                                                 |
// | Last Edit: 6/14/2010                                                      |
// |  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  |
// | In:  Filename                                                             |
// | Out:		                                                               |
// |  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  |
// | Purpose: Returns a Texture                                                |
// 0===========================================================================0
IDirect3DTexture9* CAssetManager::GetTexture(const char* Name)
{
	unsigned int _size = texSVector.size();
	for (unsigned int tex = 0; tex < _size; ++tex)
	{
		if (_stricmp(texSVector[tex].texName,Name) == 0)
		{
			return texSVector[tex].d3dTexture;
		}
	}
	return texSVector[0].d3dTexture;
}
IDirect3DTexture9* CAssetManager::GetTexture(unsigned int nTexID)
{
	unsigned int _size = texSVector.size();
	if(_size >= nTexID && nTexID >= 0)
	{
		return texSVector[nTexID].d3dTexture;
	}
	return texSVector[0].d3dTexture;
}
void CAssetManager::RemoveTexture(int nTexID)
{
	std::deque<TexStruct>::iterator itr;
	std::size_t nNumTextures = texSVector.size();
	for (itr = texSVector.begin(); itr != texSVector.end(); itr++)
	{
		if ((*itr).texID == nTexID)
		{
			(*itr).bLoaded = false;
			(*itr).d3dTexture->Release();
			(*itr).d3dTexture = NULL;
			(*itr).texID = -1;
			delete[] (*itr).texName;

			texSVector.erase(itr);
			m_nCurrentTexCount--;

			break;
		}
	}
}
void CAssetManager::RemoveTexture(const char* Name)
{
	std::deque<TexStruct>::iterator itr;
	std::size_t nNumTextures = texSVector.size();
	for (itr = texSVector.begin(); itr != texSVector.end(); itr++)
	{
		if ((*itr).texName == Name)
		{
			(*itr).bLoaded = false;
			(*itr).d3dTexture->Release();
			(*itr).d3dTexture = NULL;
			(*itr).texID = -1;
			delete[] (*itr).texName;

			texSVector.erase(itr);
			m_nCurrentTexCount--;

			break;
		}
	}
}
void CAssetManager::ClearTextures()
{
	int tvsize = texSVector.size();
	for (int tv = 0; tv < tvsize; ++tv)
		RemoveTexture(texSVector[0].texID);
	texSVector.clear();
	m_nCurrentTexCount = 0;
}
void CAssetManager::ClearLoadTextures()
{
	int tvsize = LOADtexSVector.size();
	for (int tv = 0; tv < tvsize; ++tv)
	{	
		LOADtexSVector[tv].bLoaded = false;
		LOADtexSVector[tv].d3dTexture->Release();
		LOADtexSVector[tv].d3dTexture = NULL;
		LOADtexSVector[tv].texID = -1;
		delete[] LOADtexSVector[tv].texName;
		m_nLOADCurrentTexCount--;
	}
	LOADtexSVector.clear();
	m_nLOADCurrentTexCount = 0;
}
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//Shader
int CAssetManager::LoadShader(char* Filename)
{
	if (!Filename)	
		return -1;
	for (unsigned int shade = 0; shade < m_nCurrentShaderCount; ++shade)
	{
		if (_stricmp(Filename, shadeSVector[shade].shadeName) == 0)
		{
			shadeSVector[shade].nRef++;
			return shade;
		}
	}

	char TempFile[256];
		strcpy_s(TempFile,sizeof(char)*64,"Resources\\Shaders\\");
		strncat_s(TempFile, Filename, strlen(Filename));

	ID3DXBuffer *errors;
	ID3DXEffect* tempShader;
	HRESULT hr = D3DXCreateEffectFromFile(dev, (LPCSTR)TempFile, 0, 0, D3DXSHADER_DEBUG, 0, 
		&tempShader, &errors);
	if (hr == S_OK)
	{
		ShadeStruct ShaderStruct;
		ShaderStruct.shadeID = m_nCurrentShaderCount;
		ShaderStruct.shadeName = Filename;
		ShaderStruct.nRef = 1;
		ShaderStruct.d3dShader = tempShader;
		shadeSVector.push_back(ShaderStruct);
		m_nCurrentShaderCount++;
		return (int)(m_nCurrentShaderCount - 1);
	}
	return -1;
}
ID3DXEffect* CAssetManager::GetShader(const char* Name)
{
	unsigned int _size = shadeSVector.size();
	for (unsigned int shade = 0; shade < _size; ++shade)
	{
		if (_stricmp(shadeSVector[shade].shadeName,Name) == 0)
		{
			return shadeSVector[shade].d3dShader;
		}
	}
	return NULL;
}
ID3DXEffect* CAssetManager::GetShader(unsigned int nShaderID)
{
	unsigned int _size = shadeSVector.size();
	if(_size >= nShaderID && nShaderID >= 0)
	{
		return shadeSVector[nShaderID].d3dShader;
	}
	return NULL;
}
void CAssetManager::AddShader(ID3DXEffect* _object, char* _name)
{
	unsigned int _size = shadeSVector.size();
	for (unsigned int shade = 0; shade < _size; ++shade)
	{
		if (shadeSVector[shade].d3dShader == _object)
		{
			return;
		}
	}
	ShadeStruct tempStruct;
	tempStruct.shadeID = shadeSVector.size() - 1;
	tempStruct.shadeName = _name;
	tempStruct.d3dShader = _object;
	tempStruct.nRef++;
	shadeSVector.push_back(tempStruct);
}
void CAssetManager::RemoveShader(const char* Name)
{
	std::deque<ShadeStruct>::iterator itr = shadeSVector.begin();
	unsigned int effect = 0;
	while(true)
	{
		if(shadeSVector[effect].shadeName == Name)
		{
			shadeSVector.erase(itr);
			break;
		}
		if(itr == shadeSVector.end())
			break;
		else
		{
			++itr;
			++effect;
		}
	}
}
void RemoveShader(int nShaderID)
{

}

void CAssetManager::ClearShaders()
{
	int shsize = shadeSVector.size();
	for (int sh = 0; sh < shsize; sh++)
	{
		shadeSVector[sh].d3dShader->Release();
		shadeSVector[sh].d3dShader = NULL;
	}

	m_nCurrentShaderCount = 0;
	shadeSVector.clear();
}

void CAssetManager::onLostDeviceShader()
{
	unsigned int _size = shadeSVector.size();
	for (unsigned int effect = 0; effect < _size; ++effect)
	{
		shadeSVector[effect].d3dShader->OnLostDevice();
	}
}

void CAssetManager::onResetDeviceShader()
{
	unsigned int _size = shadeSVector.size();
	for (unsigned int effect = 0; effect < _size; ++effect)
	{
		shadeSVector[effect].d3dShader->OnResetDevice();
	}
}

void CAssetManager::CommitChanges(char* _name)
{
	ID3DXEffect* temp;
	temp = GetShader(_name);
	temp->CommitChanges();
}
void CAssetManager::BeginPass(char* _name, int _pass)
{
	ID3DXEffect* temp;
	temp = GetShader(_name);
	temp->BeginPass(_pass);
}
void CAssetManager::EndPass(char* _name)
{
	ID3DXEffect* temp;
	temp = GetShader(_name);
	temp->EndPass();
}
void CAssetManager::Begin(char* _name)
{
	ID3DXEffect* temp;
	temp = GetShader(_name);
	UINT numPasses = 0;
	temp->Begin(&numPasses, 0);
}
void CAssetManager::End(char* _name)
{
	ID3DXEffect* temp;
	temp = GetShader(_name);
	temp->End();
}
void CAssetManager::SetMatrix(char* _name, D3DXHANDLE _var, D3DXMATRIX* _matrix)
{
	ID3DXEffect* temp;
	temp = GetShader(_name);
	temp->SetMatrix(_var, _matrix);
}
void CAssetManager::SetFloat(char* _name, D3DXHANDLE _var, float _float)
{
	ID3DXEffect* temp;
	temp = GetShader(_name);
	temp->SetFloat(_var, _float);
}
void CAssetManager::SetVector(char* _name, D3DXHANDLE _var, D3DXVECTOR4 _vector)
{
	ID3DXEffect* temp;
	temp = GetShader(_name);
	temp->SetVector(_var, &_vector);
}
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//Object
int CAssetManager::LoadMesh(const char* Filename)
{
	unsigned int _size = MeshDeq.size();
	for (unsigned int _mesh = 0; _mesh < _size; _mesh++)
	{
		if (strcmp(MeshDeq[_mesh]->getName(),Filename) == 0)
		{
			return _mesh;
		}
	}

	char TempFile[256];
		strcpy_s(TempFile,sizeof(char)*64,"Resources\\Models\\");
		strncat_s(TempFile, Filename, strlen(Filename));
		strncat_s(TempFile, ".mesh", 5);

	Mesh* tempMesh = new Mesh();
	std::fstream FileIn(TempFile, std::ios_base::in | std::ios_base::binary);
		tempMesh->Load(&FileIn);
	FileIn.close();

	MeshDeq.push_back(tempMesh);
	m_nCurrentMeshCount++;

	return m_nCurrentMeshCount;
}
Mesh* CAssetManager::GetMesh(const char* Name)
{
	unsigned int _size = MeshDeq.size();
	for (unsigned int _mesh = 0; _mesh < _size; _mesh++)
	{
		if (strcmp(MeshDeq[_mesh]->getName(),Name) == 0)
		{
			return MeshDeq[_mesh];
		}
	}

	return MeshDeq[0];
}
Mesh* CAssetManager::GetMesh(RJ_Config::RJ_Mesh_Types _MeshType)
{
	return MeshDeq[0];
}
void CAssetManager::AddMesh(Mesh* _object)
{
	unsigned int _size = MeshDeq.size();
	for (unsigned int obj = 0; obj < _size; ++obj)
	{
		if (MeshDeq[obj] == _object)
		{
			return;
		}
	}
}
void CAssetManager::RemoveMesh(const char* Name)
{
	std::deque<Mesh*>::iterator itr = MeshDeq.begin();
	while(true)
	{
		
	}
}
void CAssetManager::ClearMeshes()
{
	unsigned int _size = MeshDeq.size();
	for (unsigned int _mesh = 0; _mesh < _size; _mesh++)
	{
		MeshDeq[_mesh]->Shutdown();
		delete MeshDeq[_mesh];
	}

	MeshDeq.clear();
}

void CAssetManager::HandleEvent(CEvent* pEvent)
{
	switch ( pEvent->GetEventID() )
	{

	case ET_JELLOWIGGLE:
		{
			if ( pEvent->GetData() != NULL )
			{
				if ( pEvent->GetData()->GetMsgID() == EM_JELLOWIGGLE)
				{
					tJelloWiggle* t_Jello;
					t_Jello = (tJelloWiggle*)pEvent->GetData();

					unsigned int _size = MeshDeq.size();
					for (unsigned int _mesh = 0; _mesh < _size; _mesh++)
					{
						if(strcmp(MeshDeq[_mesh]->getName(), t_Jello->m_Name) == 0)
						{
							MeshDeq[_mesh]->setShader(GetShader("Jello.fx"), true, 1.0);
							break;
						}
					}

					delete[] t_Jello->m_Name;
				}
			}
		}
		break;

	case ET_GOALPOS:
		{
			if ( pEvent->GetData() != NULL )
			{
				if ( pEvent->GetData()->GetMsgID() == EM_GOALPOS)
				{
					tGoalPosition* tGoal;
					tGoal = (tGoalPosition*)pEvent->GetData();

					goalPos = tGoal->m_Position;
				}
			}
		}
		break;

	case ET_GOALEFFECT:
		{
			if ( pEvent->GetData() != NULL )
			{
				if ( pEvent->GetData()->GetMsgID() == EM_GOALEFFECT)
				{
					tGoalEffect* tGoal;
					tGoal = (tGoalEffect*)pEvent->GetData();

					if(tGoal->m_Multiplier == 2)
					{
						bgoal1 = true;
					}
					else if(tGoal->m_Multiplier == 4)
					{
						bgoal2 = true;
					}
					else if(tGoal->m_Multiplier == 6)
					{
						bgoal3 = true;
					}
					else if(tGoal->m_Multiplier == 8)
					{
						bgoal4 = true;
					}
				}
			}
		}
		break;

	case ET_LOADMESH:
		{
			if ( pEvent->GetData() != NULL )
			{
				if ( pEvent->GetData()->GetMsgID() == EM_LOADMESH )
				{
					tLoadMesh* tMesh;
					tMesh = (tLoadMesh*)pEvent->GetData();

					LoadMesh(tMesh->m_szMeshName.c_str());
				}
			}
		}
		break;
	case ET_LOADANIMMESH:
		{
			if ( pEvent->GetData() != NULL )
			{
				if ( pEvent->GetData()->GetMsgID() == EM_LOADANIMMESH )
				{
					tLoadAnimMesh* tAMesh;
					tAMesh = (tLoadAnimMesh*)pEvent->GetData();

					LoadAnimMesh(tAMesh->m_szMeshName.c_str());
				}
			}
		}
		break;
	case ET_CREATELINK:
		{
			if ( pEvent->GetData() != NULL )
			{
				if ( pEvent->GetData()->GetMsgID() == EM_CREATELINK )
				{
					tCreateAnimLink* tACreate;
					tACreate = (tCreateAnimLink*)pEvent->GetData();
					AddAnimLink(tACreate->m_gaID,tACreate->m_aniType);
				}
			}
		}
	case ET_BREAKLINK:
		{
			if ( pEvent->GetData() != NULL )
			{
				if ( pEvent->GetData()->GetMsgID() == EM_BREAKLINK )
				{
					tBreakAnimLink* tABreak;
					tABreak = (tBreakAnimLink*)pEvent->GetData();
					RemoveAnimLink(tABreak->m_gaID, tABreak->m_aniType);
				}
			}
		}
	case ET_ANIMSTATECHANGE:
		{
			if ( pEvent->GetData() != NULL )
			{
				if ( pEvent->GetData()->GetMsgID() == EM_ANIMSTATECHANGE )
				{
					tAnimStateChange* tAChange;
					tAChange = (tAnimStateChange*)pEvent->GetData();
  					AnimLink* ALink = GetAnimLink(tAChange->m_gaID,tAChange->m_aniType);
					if(!ALink)
 						break;
					if(tAChange->m_aniType == RJ_Config::RJ_ANIM_ANTHONY)
					{

						AnimAnthony* AnthLink = (AnimAnthony*)ALink;
						if(AnthLink->AnthAnimState == RJ_Config::ANTH_ANIM_COLLITION && tAChange->m_nStateID == (int)RJ_Config::ANTH_ANIM_COLLITION)
							AnthLink->m_dCurrAnimRunTime = 0;
						if(AnthLink->AnthAnimState == tAChange->m_nStateID && !tAChange->m_bRestart)
							break;
						if(AnthLink->AnthAnimState == RJ_Config::ANTH_ANIM_COLLITION || AnthLink->AnthAnimState == RJ_Config::ANTH_ANIM_LAUNCH)
							break;
						if((tAChange->m_nStateID == RJ_Config::ANTH_ANIM_SWEEPFORWARD || tAChange->m_nStateID == RJ_Config::ANTH_ANIM_SCRAPE) &&
							(AnthLink->AnthAnimState == (int)RJ_Config::ANTH_ANIM_SWEEPLEFT || AnthLink->AnthAnimState == (int)RJ_Config::ANTH_ANIM_SWEEPRIGHT))
							break;
						if((RJ_Config::ANTHONY_ANIM_STATES)tAChange->m_nStateID != RJ_Config::ANTH_ANIM_ENDSWEEP)
						{
							if(tAChange->m_nStateID != RJ_Config::ANTH_ANIM_LAUNCH)
								AnthLink->m_dCurrAnimRunTime = 0;
							AnthLink->PrevAnthAnimState = AnthLink->AnthAnimState;
							AnthLink->AnthAnimState = (RJ_Config::ANTHONY_ANIM_STATES)tAChange->m_nStateID;
						}
						AState* NewState;
						if(AnthLink->AnthAnimState == RJ_Config::ANTH_ANIM_IDLE)
							NewState = AAnthIdle::GetState();
						else if(AnthLink->AnthAnimState == RJ_Config::ANTH_ANIM_IDLERIDE)
							NewState = AAnthIdleRide::GetState();
						else if(AnthLink->AnthAnimState == RJ_Config::ANTH_ANIM_LAUNCH)
							NewState = AAnthLaunch::GetState();
						else if(AnthLink->AnthAnimState == RJ_Config::ANTH_ANIM_IDLETOGUMMY)
							NewState = AAnthtoGummy::GetState();
						else if(AnthLink->AnthAnimState == RJ_Config::ANTH_ANIM_PUSHBACK)
							NewState = AAnthPushBack::GetState();
						else if(AnthLink->AnthAnimState == RJ_Config::ANTH_ANIM_SWEEPFORWARD && tAChange->m_nStateID != (int)RJ_Config::ANTH_ANIM_ENDSWEEP)
						{
							NewState = AAnthSweepForward::GetState();
							AnthLink->m_bSweepEnd = false;
						}
						else if(AnthLink->AnthAnimState == RJ_Config::ANTH_ANIM_SCRAPE && tAChange->m_nStateID != (int)RJ_Config::ANTH_ANIM_ENDSWEEP)
						{
							NewState = AAnthScrape::GetState();
							AnthLink->m_bSweepEnd = false;
						}
						else if(AnthLink->AnthAnimState == RJ_Config::ANTH_ANIM_SWEEPLEFT && tAChange->m_nStateID != (int)RJ_Config::ANTH_ANIM_ENDSWEEP)
						{
							NewState = AAnthSweepLeft::GetState();
							AnthLink->m_bSweepEnd = false;
						}
						else if(AnthLink->AnthAnimState == RJ_Config::ANTH_ANIM_SWEEPRIGHT && tAChange->m_nStateID != (int)RJ_Config::ANTH_ANIM_ENDSWEEP)
						{
							NewState = AAnthSweepRight::GetState();
							AnthLink->m_bSweepEnd = false;
						}
						else if(tAChange->m_nStateID == (int)RJ_Config::ANTH_ANIM_ENDSWEEP &&
							(AnthLink->AnthAnimState == RJ_Config::ANTH_ANIM_SWEEPRIGHT || AnthLink->AnthAnimState == RJ_Config::ANTH_ANIM_SWEEPLEFT ||
							AnthLink->AnthAnimState == RJ_Config::ANTH_ANIM_SWEEPFORWARD || AnthLink->AnthAnimState == RJ_Config::ANTH_ANIM_SCRAPE))
							AnthLink->m_bSweepEnd = true;
						else if(AnthLink->AnthAnimState == RJ_Config::ANTH_ANIM_COLLITION)
							NewState = AAnthCollition::GetState();
						else if(AnthLink->AnthAnimState == RJ_Config::ANTH_ANIM_WIN)
							NewState = AAnthWin::GetState();
						else if(AnthLink->AnthAnimState == RJ_Config::ANTH_ANIM_FOUL)
							NewState = AAnthFoul::GetState();
						if((RJ_Config::ANTHONY_ANIM_STATES)tAChange->m_nStateID != RJ_Config::ANTH_ANIM_ENDSWEEP)
							AnthLink->m_AStateMachine->ChangeState(NewState);
					}
					else if(tAChange->m_aniType == RJ_Config::RJ_ANIM_GUMMY)
					{

						AnimGummyWorm* AnthLink = (AnimGummyWorm*)ALink;
						if(AnthLink->GummyAnimState == tAChange->m_nStateID && !tAChange->m_bRestart)
							break;
						if((RJ_Config::GUMMY_ANIM_STATES)tAChange->m_nStateID != RJ_Config::GUMMY_ANIM_RELEASE)
						{
							AnthLink->m_dCurrAnimRunTime = 0;
							AnthLink->PrevGummyAnimState = AnthLink->GummyAnimState;
							AnthLink->GummyAnimState = (RJ_Config::GUMMY_ANIM_STATES)tAChange->m_nStateID;
						}
						AState* NewState;
						if(AnthLink->GummyAnimState == RJ_Config::GUMMY_ANIM_LAUNCH && 
							(RJ_Config::GUMMY_ANIM_STATES)tAChange->m_nStateID != RJ_Config::GUMMY_ANIM_RELEASE)
							NewState = AGWLaunch::GetState();
						else if(AnthLink->GummyAnimState == RJ_Config::GUMMY_ANIM_IDLE)
							NewState = AGWIdle::GetState();
						else if((RJ_Config::GUMMY_ANIM_STATES)tAChange->m_nStateID == RJ_Config::GUMMY_ANIM_RELEASE)
							AnthLink->m_bReleaseLaunch = true;
						if((RJ_Config::GUMMY_ANIM_STATES)tAChange->m_nStateID != RJ_Config::GUMMY_ANIM_RELEASE)
 							AnthLink->m_AStateMachine->ChangeState(NewState);
					}
					else if(tAChange->m_aniType == RJ_Config::RJ_ANIM_TOOTHPICK)
					{
						AnimToothPick* AnthLink = (AnimToothPick*)ALink;
						if(AnthLink->ToothAnimState == tAChange->m_nStateID && !tAChange->m_bRestart)
							break;
						if((RJ_Config::TOOTH_ANIM_STATES)tAChange->m_nStateID != RJ_Config::TOOTH_ANIM_RELEASE)
						{
							AnthLink->m_dCurrAnimRunTime = 0;
							AnthLink->PrevToothAnimState = AnthLink->ToothAnimState;
							AnthLink->ToothAnimState = (RJ_Config::TOOTH_ANIM_STATES)tAChange->m_nStateID;
						}
						AState* NewState;
						if(AnthLink->ToothAnimState == RJ_Config::TOOTH_ANIM_LAUNCH && 
							(RJ_Config::TOOTH_ANIM_STATES)tAChange->m_nStateID != RJ_Config::TOOTH_ANIM_RELEASE)
							NewState = ATPLaunch::GetState();
						else if(AnthLink->ToothAnimState == RJ_Config::TOOTH_ANIM_IDLE)
							NewState = ATPIdle::GetState();
						else if((RJ_Config::TOOTH_ANIM_STATES)tAChange->m_nStateID == RJ_Config::TOOTH_ANIM_RELEASE)
							AnthLink->m_bReleaseLaunch = true;
						if((RJ_Config::TOOTH_ANIM_STATES)tAChange->m_nStateID != RJ_Config::TOOTH_ANIM_RELEASE)
							AnthLink->m_AStateMachine->ChangeState(NewState);
					}
					else if(tAChange->m_aniType == RJ_Config::RJ_ANIM_BROOM)
					{

						AnimBroom* AnthLink = (AnimBroom*)ALink;
						if(AnthLink->BroomAnimState == RJ_Config::BROOM_ANIM_COLLITION && tAChange->m_nStateID == (int)RJ_Config::BROOM_ANIM_COLLITION)
							AnthLink->m_dCurrAnimRunTime = 0;
						if(AnthLink->BroomAnimState == tAChange->m_nStateID && !tAChange->m_bRestart)
							break;
						if(AnthLink->BroomAnimState == RJ_Config::BROOM_ANIM_COLLITION || AnthLink->BroomAnimState == RJ_Config::BROOM_ANIM_LAUNCH)
							break;
						if((tAChange->m_nStateID == RJ_Config::BROOM_ANIM_SWEEPFORWARD || tAChange->m_nStateID == RJ_Config::BROOM_ANIM_SCRAPE) &&
							(AnthLink->BroomAnimState == (int)RJ_Config::BROOM_ANIM_SWEEPLEFT || AnthLink->BroomAnimState == (int)RJ_Config::BROOM_ANIM_SWEEPRIGHT))
							break;
						if((RJ_Config::BROOM_ANIM_STATES)tAChange->m_nStateID != RJ_Config::BROOM_ANIM_ENDSWEEP)
						{
							if(tAChange->m_nStateID != RJ_Config::BROOM_ANIM_LAUNCH)
								AnthLink->m_dCurrAnimRunTime = 0;
							AnthLink->PrevBroomAnimState = AnthLink->BroomAnimState;
							AnthLink->BroomAnimState = (RJ_Config::BROOM_ANIM_STATES)tAChange->m_nStateID;
						}
						AState* NewState;
						if(AnthLink->BroomAnimState == RJ_Config::BROOM_ANIM_IDLE)
							NewState = ABRIdle::GetState();
						else if(AnthLink->BroomAnimState == RJ_Config::BROOM_ANIM_IDLERIDE)
							NewState = ABRIdleRide::GetState();
						else if(AnthLink->BroomAnimState == RJ_Config::BROOM_ANIM_LAUNCH)
							NewState = ABRLaunch::GetState();
						else if(AnthLink->BroomAnimState == RJ_Config::BROOM_ANIM_IDLETOGUMMY)
							NewState = ABRtoGummy::GetState();
						else if(AnthLink->BroomAnimState == RJ_Config::BROOM_ANIM_PUSHBACK)
							NewState = ABRPushBack::GetState();
						else if(AnthLink->BroomAnimState == RJ_Config::BROOM_ANIM_SWEEPFORWARD && tAChange->m_nStateID != (int)RJ_Config::BROOM_ANIM_ENDSWEEP)
						{
							NewState = ABRSweepForward::GetState();
							AnthLink->m_bSweepEnd = false;
						}
						else if(AnthLink->BroomAnimState == RJ_Config::BROOM_ANIM_SCRAPE && tAChange->m_nStateID != (int)RJ_Config::BROOM_ANIM_ENDSWEEP)
						{
							NewState = ABRScrape::GetState();
							AnthLink->m_bSweepEnd = false;
						}
						else if(AnthLink->BroomAnimState == RJ_Config::BROOM_ANIM_SWEEPLEFT && tAChange->m_nStateID != (int)RJ_Config::BROOM_ANIM_ENDSWEEP)
						{
							NewState = ABRSweepLeft::GetState();
							AnthLink->m_bSweepEnd = false;
						}
						else if(AnthLink->BroomAnimState == RJ_Config::BROOM_ANIM_SWEEPRIGHT && tAChange->m_nStateID != (int)RJ_Config::BROOM_ANIM_ENDSWEEP)
						{
							NewState = ABRSweepRight::GetState();
							AnthLink->m_bSweepEnd = false;
						}
						else if(tAChange->m_nStateID == (int)RJ_Config::BROOM_ANIM_ENDSWEEP &&
							(AnthLink->BroomAnimState == RJ_Config::BROOM_ANIM_SWEEPRIGHT || AnthLink->BroomAnimState == RJ_Config::BROOM_ANIM_SWEEPLEFT ||
							AnthLink->BroomAnimState == RJ_Config::BROOM_ANIM_SWEEPFORWARD || AnthLink->BroomAnimState == RJ_Config::BROOM_ANIM_SCRAPE))
							AnthLink->m_bSweepEnd = true;
						else if(AnthLink->BroomAnimState == RJ_Config::BROOM_ANIM_COLLITION)
							NewState = ABRCollition::GetState();
						else if(AnthLink->BroomAnimState == RJ_Config::BROOM_ANIM_WIN)
							NewState = ABRWin::GetState();
						else if(AnthLink->BroomAnimState == RJ_Config::BROOM_ANIM_FOUL)
							NewState = ABRFoul::GetState();
						if((RJ_Config::BROOM_ANIM_STATES)tAChange->m_nStateID != RJ_Config::BROOM_ANIM_ENDSWEEP)
							AnthLink->m_AStateMachine->ChangeState(NewState);
					}
					else if(tAChange->m_aniType == RJ_Config::RJ_SPECTATOR)
					{
						AnimSpectator* AnthLink = (AnimSpectator*)ALink;
						if(AnthLink->SpecAnimState == tAChange->m_nStateID && !tAChange->m_bRestart)
							break;
						AnthLink->m_dCurrAnimRunTime = 0;
						AnthLink->PrevSpecAnimState = AnthLink->SpecAnimState;
						AnthLink->SpecAnimState = (RJ_Config::SPECTATOR_ANIM_STATES)tAChange->m_nStateID;
						AState* NewState;
						if(AnthLink->SpecAnimState == RJ_Config::SPEC_ANIM_CHEER1)
							NewState = ASTCheer1::GetState();
						else if(AnthLink->SpecAnimState == RJ_Config::SPEC_ANIM_CHEER2)
							NewState = ASTCheer2::GetState();
						else if(AnthLink->SpecAnimState == RJ_Config::SPEC_ANIM_CHEER3)
							NewState = ASTCheer3::GetState();
						else if(AnthLink->SpecAnimState == RJ_Config::SPEC_ANIM_CHEER4)
							NewState = ASTCheer4::GetState();
						AnthLink->m_AStateMachine->ChangeState(NewState);
					}
				}
			}
		}
	}

	

	if(pEvent->GetEventID() == ET_FORWARD)
	{
		tForward* tF;
		tF = (tForward*)pEvent->GetData();
		forward = tF->forward;
	}

	else if(pEvent->GetEventID() == ET_CURRENTVOLUME)
	{
		tCurrentVolume* tCV;
		tCV = (tCurrentVolume*)pEvent->GetData();

		m_currentVolume = tCV->m_eType;
	}

	else if(pEvent->GetEventID() == ET_PUSHTURNOVER)
	{
		tPushTurnOver* tTO;
		tTO = (tPushTurnOver*)pEvent->GetData();
		int CurPlayer, Bits, Bounces, Brownie, Multiplier, Par = 0;
		CurPlayer = tTO->m_nCurrPlayer; Bits = tTO->m_nBits; Bounces = tTO->m_nBounces; Brownie =  tTO->m_nBrownie; Multiplier = tTO->m_nMultiplier; Par = tTO->m_nPar;
		RJ_GameMain::GetInstance()->PushStateNow(new State_TurnOver(CurPlayer, Bits, Bounces, Brownie, Multiplier, Par) );
	}
	else if(pEvent->GetEventID() == ET_ADDSPECTATOR)
	{
		tAddSpectators* tAS;
		tAS = (tAddSpectators*)pEvent->GetData();
		AddSpectator(tAS->m_mLocation, tAS->m_bSitting);
		
	}
	else if(pEvent->GetEventID() == ET_LOADINGDONE)
	{
		tLoadingDone* tAS;
		tAS = (tLoadingDone*)pEvent->GetData();
		tStateConfig::GetInstanceRender()->m_bIsLoading = false;
	}
	else if(pEvent->GetEventID() == ET_CLEARSPECTATORS)
	{
		tClearSpectators* tAS;
		tAS = (tClearSpectators*)pEvent->GetData();
		ClearCrowd();
	}
	else if(pEvent->GetEventID() == ET_CREATESPEC)
	{
		tCreateSpecType* tAS;
		tAS = (tCreateSpecType*)pEvent->GetData();
		CreateSpectator(tAS->m_nGlobalID,tAS->m_nTypeID);
	}
	else if(pEvent->GetEventID() == ET_PUSHHIGHSCORES)
	{
		tPushHighScores* tHS;
		tHS = (tPushHighScores*)pEvent->GetData();
		int PlayerNum, Index, CurrLevel, NumPlayers = 0;
		PlayerNum = tHS->m_nPlayerNum; Index = tHS->m_nIndex; CurrLevel = tHS->m_nCurrLevel; NumPlayers =  tHS->m_nNumPlayers;
		RJ_GameMain::GetInstance()->PushState(new State_HighScores(PlayerNum, Index, CurrLevel, NumPlayers) );
	}
	else if(pEvent->GetEventID() == ET_STOPEFFECT)
	{
		tStopEffects* tPEF;
		tPEF = (tStopEffects*)pEvent->GetData();

		switch(tPEF->m_eType)
		{
			case RJ_Config::RJ_MOTION:
				{
					//effectMotion.StopPlaying();
					bMotionLines = false;
				}
				break;
			case RJ_Config::RJ_SKID:
				{
					effectSkid.StopPlaying();
				}
				break;
		}
	}
	else if(pEvent->GetEventID() == ET_PLAYEFFECT)
	{
		if ( pEvent->GetData() != NULL )
		{
			if ( pEvent->GetData()->GetMsgID() == EM_PLAYEFFECT )
			{
				tPlayEffect* tPEF;
				tPEF = (tPlayEffect*)pEvent->GetData();
				switch (tPEF->m_eType)
				{
				case RJ_Config::RJ_UNDEFINED:
					break;
				case RJ_Config::RJ_COLLISION:
					{
						if(!effectCollision.isPlaying())
						{
							effectCollision.SetPosition(tPEF->m_vPos);
						}
						else
						{
							effectCollision.SetPositionWithoutUpdate(tPEF->m_vPos);
						}
					}
					break;
				case RJ_Config::RJ_HAT:
					{
						D3DXVECTOR3 pos = tPEF->m_vPos;
						pos.y -= 1.2f;

						if(!effectHat.isPlaying())
						{
							if(eHat != tPEF->m_eHat)
							{
								eHat = tPEF->m_eHat;
								effectHat.SetPosition(pos);
							}
						}
					}
					break;
				case RJ_Config::RJ_COLLECTION:
					{
						if(!effectCollection.isPlaying())
						{
							effectCollection.SetPosition(tPEF->m_vPos);
						}
						else
						{
							effectCollection.SetPositionWithoutUpdate(tPEF->m_vPos);
						}
					}
					break;
				case RJ_Config::RJ_SCRAPE:
					{
						D3DXVECTOR3 pos = tPEF->m_vPos;
						pos += forward;

						if(m_currentVolume == -1)
						{
							if(!effectScrape.isPlaying())
							{
								effectScrape.SetPosition(pos);
							}
							else
							{
								effectScrape.SetPositionWithoutUpdate(pos);
							}
						}
					}
					break;
				case RJ_Config::RJ_SMOKE:
					{
						if(!effectSmoke1.isPlaying() && !effectSmoke2.isPlaying() &&
							!effectSmoke3.isPlaying() && (m_currentVolume == -1))
						{
							int smoke = 0;
							smoke += (rand() % 3);

							D3DXVECTOR3 pos = tPEF->m_vPos;

							if(tPEF->m_ePos == RJ_Config::RJ_POS_UP)
							{
								pos += forward;
							}
							else if(tPEF->m_ePos == RJ_Config::RJ_POS_LEFT)
							{
								D3DXMATRIX newPos;
								D3DXMatrixTranslation(&newPos, forward.x, forward.y, forward.z);
								D3DXMATRIX newRot;
								D3DXMatrixRotationY(&newRot, D3DXToRadian(30.0f));
								newPos = newPos * newRot;

								pos.x += newPos._41; pos.y += newPos._42; pos.z += newPos._43;
							}
							else if(tPEF->m_ePos == RJ_Config::RJ_POS_RIGHT)
							{
								D3DXMATRIX newPos;
								D3DXMatrixTranslation(&newPos, forward.x, forward.y, forward.z);
								D3DXMATRIX newRot;
								D3DXMatrixRotationY(&newRot, D3DXToRadian(-30.0f));
								newPos = newPos * newRot;

								pos.x += newPos._41; pos.y += newPos._42; pos.z += newPos._43;
							}
							
							if(smoke == 0)
								effectSmoke3.SetPosition(pos);
							else if(smoke == 1)
								effectSmoke2.SetPosition(pos);
							else
								effectSmoke1.SetPosition(pos);
						}
						//else
						//{
						//	effectSmoke1.SetPositionWithoutUpdate(tPEF->m_vPos);
						//	wakePos = tPEF->m_vPos;
						//}
						//waveBool = true;
					}
					break;

				case RJ_Config::RJ_DRIP:
					{
						bDrip = true;
						dripPos = tPEF->m_vPos;

						if(!effectDrip.isPlaying())
						{
							effectDrip.SetPosition(tPEF->m_vPos);
						}
						else
						{
							effectDrip.SetPositionWithoutUpdate(tPEF->m_vPos);
						}
					}
					break;

				case RJ_Config::RJ_MOTION:
					{
						bMotionLines = true;
						motionPos = tPEF->m_vPos;
					}
					break;

				case RJ_Config::RJ_SPLAT:
					{
						if(!effectSplat.isPlaying())
						{
							effectSplat.SetPosition(tPEF->m_vPos);
						}
						else
						{
							effectSplat.SetPositionWithoutUpdate(tPEF->m_vPos);
						}
					}
					break;

				case RJ_Config::RJ_JELLO:
					{
						if(!effectJello.isPlaying())
						{
							effectJello.SetPosition(tPEF->m_vPos);
						}
						else
						{
							effectJello.SetPositionWithoutUpdate(tPEF->m_vPos);
						}
					}
					break;

				case RJ_Config::RJ_SKID:
					{
						if(!effectSkid.isPlaying())
						{
							effectSkid.SetForward(forward);
							effectSkid.SetPosition(tPEF->m_vPos);
						}
						else
						{
							effectSkid.SetPositionWithoutUpdate(tPEF->m_vPos);
						}
					}
					break;

				//case RJ_Config::RJ_HAZARD_WATER:
				//	{
				//		waveMesh.setTextureID(waterID);
				//		wavePos = tPEF->m_vPos;
				//		waveBool = true;
				//	}
				//	break;

				//case RJ_Config::RJ_HAZARD_ORANGE:
				//	{
				//		waveMesh.setTextureID(orangeID);
				//		wavePos = tPEF->m_vPos;
				//		waveBool = true;
				//	}
				//	break;

				//case RJ_Config::RJ_HAZARD_OIL:
				//	{
				//		waveMesh.setTextureID(oilID);
				//		wavePos = tPEF->m_vPos;
				//		waveBool = true;
				//	}
				//	break;

				//case RJ_Config::RJ_WAKE:
				//	{
				//		if(!effectWake.isPlaying())
				//		{
				//			effectWake.SetPosition(tPEF->m_vPos);
				//		}
				//		else
				//		{
				//			effectWake.SetPositionWithoutUpdate(tPEF->m_vPos);
				//			wakePos = tPEF->m_vPos;
				//		}
				//		wakeBool = true;
				//	}
				//	break;
				}
				tPEF = NULL;
			}
		}
	}
	pEvent->m_nRefs--;
}
bool CAssetManager::DrawFlatSprite(int nID, float fX, float fY, float fScaleX, float fScaleY,
					RECT* pSection, float fRotCenterX, float fRotCenterY, float fRotation, DWORD dwColor, bool bIsLoad)
{
	//m_lpSprite->Begin(D3DXSPRITE_ALPHABLEND | D3DXSPRITE_SORT_DEPTH_BACKTOFRONT);

	//assert(texSVector[nID].d3dTexture != NULL && "Attempting to draw released texture id");
	//assert(nID > -1 && nID < (int)m_nCurrentTexCount && "nID is out of range");
	if (!m_lpSprite)
		return false;

	D3DXMATRIX scale;
	D3DXMATRIX rotation;
	D3DXMATRIX translate;
	D3DXMATRIX combined;
	D3DXMatrixIdentity(&combined);
	D3DXMatrixTranslation(&translate, -fRotCenterX, -fRotCenterY, 0.0f);
	combined *= translate;
	D3DXMatrixRotationZ(&rotation, fRotation);
	combined *= rotation;
	D3DXMatrixTranslation(&translate, fRotCenterX, fRotCenterY, 0.0f);
	combined *= translate;
	D3DXMatrixScaling(&scale, fScaleX, fScaleY, 1.0f);
	combined *= scale;
	D3DXMatrixTranslation(&translate, fX, fY, 0.0f);
	combined *= translate;
	m_lpSprite->SetTransform(&combined);
	if(bIsLoad)
		m_lpSprite->Draw(LOADtexSVector[nID].d3dTexture, pSection, NULL, NULL, dwColor);
	else
		m_lpSprite->Draw(texSVector[nID].d3dTexture, pSection, NULL, NULL, dwColor);
	D3DXMatrixIdentity(&combined);
	m_lpSprite->SetTransform(&combined);
	//m_lpSprite->Flush();
	//m_lpSprite->End();
	return true;
}
//////////////////////////////////////////////////////////////////////////
int CAssetManager::LoadAnimMesh(const char* Filename)
{
	unsigned int _size = AnimMeshDeq.size();
	for (unsigned int _mesh = 0; _mesh < _size; _mesh++)
	{
		if (strcmp(AnimMeshDeq[_mesh]->getName(),Filename) == 0)
		{
			return _mesh;
		}
	}

	char TempFile[256];
	strcpy_s(TempFile,sizeof(char)*64,"Resources\\Models\\");
	strncat_s(TempFile, Filename, strlen(Filename));
	strncat_s(TempFile, ".mesh", 5);

	AnimMesh* tempMesh = new AnimMesh();
	std::fstream FileIn(TempFile, std::ios_base::in | std::ios_base::binary);
	tempMesh->Load(&FileIn);
	FileIn.close();

	AnimMeshDeq.push_back(tempMesh);
	m_nCurrentAnimMeshCount++;

	return m_nCurrentAnimMeshCount;
}
AnimMesh* CAssetManager::GetAnimMesh(const char* Name)
{
	unsigned int _size = AnimMeshDeq.size();
	for (unsigned int _mesh = 0; _mesh < _size; _mesh++)
	{
		if (strcmp(AnimMeshDeq[_mesh]->getName(),Name) == 0)
		{
			return AnimMeshDeq[_mesh];
		}
	}
	return AnimMeshDeq[0];
}
AnimMesh* CAssetManager::GetAnimMesh(RJ_Config::RJ_Mesh_Types _MeshType)
{
	return AnimMeshDeq[0];
}
void CAssetManager::AddAnimMesh(AnimMesh* _object)
{
	unsigned int _size = AnimMeshDeq.size();
	for (unsigned int obj = 0; obj < _size; ++obj)
	{
		if (AnimMeshDeq[obj] == _object)
		{
			return;
		}
	}
}
void CAssetManager::RemoveAnimMesh(const char* Name)
{
	std::deque<AnimMesh*>::iterator itr = AnimMeshDeq.begin();
	while(true)
	{

	}
}
void CAssetManager::RemoveAnimMesh(RJ_Config::RJ_Mesh_Types _MeshType)
{

}
void CAssetManager::ClearAnimMeshes()
{
	unsigned int _size = AnimMeshDeq.size();
	for (unsigned int _mesh = 0; _mesh < _size; _mesh++)
	{
		AnimMeshDeq[_mesh]->Shutdown();
		delete AnimMeshDeq[_mesh];
	}
	AnimMeshDeq.clear();
}
void CAssetManager::AddAnimLink(int gaId, RJ_Config::ANIM_TYPE anitype)
{
	switch (anitype)
	{
	case RJ_Config::RJ_ANIM_ANTHONY:
		{
			AnimAnthony* _object = new AnimAnthony();
			_object->nLinkedID = gaId;
			_object->AnimType = anitype;
			_object->Init();
			AnimLinkDeq.push_back((AnimLink*)_object);
		}
		break;
	case RJ_Config::RJ_ANIM_BROOM:
		{
			AnimBroom* _object = new AnimBroom();
			_object->nLinkedID = gaId;
			_object->AnimType = anitype;
			_object->Init();
			AnimLinkDeq.push_back((AnimLink*)_object);
		}
		break;
	case RJ_Config::RJ_ANIM_GUMMY:
		{
			AnimGummyWorm* _object = new AnimGummyWorm();
			_object->nLinkedID = gaId;
			_object->AnimType = anitype;
			_object->Init();
			AnimLinkDeq.push_back((AnimLink*)_object);
		}
		break;
	case RJ_Config::RJ_ANIM_TOOTHPICK:
		{
			AnimToothPick* _object = new AnimToothPick();
			_object->nLinkedID = gaId;
			_object->AnimType = anitype;
			_object->Init();
			AnimLinkDeq.push_back((AnimLink*)_object);
		}
		break;
	case RJ_Config::RJ_SPECTATOR:
		{
			AnimSpectator* _object = new AnimSpectator();
			_object->nLinkedID = gaId;
			_object->AnimType = anitype;
			_object->Init();
			AnimLinkDeq.push_back((AnimLink*)_object);
		}
		break;
	}
}
void CAssetManager::ClearAnimLinks()
{
	while(!AnimLinkDeq.empty())
	{
		AnimLink* DieLink = AnimLinkDeq.front();
		AnimLinkDeq.pop_front();
		delete DieLink;
	}
	AnimLinkDeq.clear();
}
void CAssetManager::RemoveAnimLink(int gaId, RJ_Config::ANIM_TYPE anitype)
{
	for(deque<AnimLink*>::iterator itr = AnimLinkDeq.begin(); itr != AnimLinkDeq.end(); itr++)
	{
		if((*itr)->nLinkedID == gaId && (*itr)->AnimType == anitype)
		{
			AnimLink* DieLink = *itr;
			AnimLinkDeq.erase(itr);
			delete DieLink;
			break;
		}
	}
}
AnimLink* CAssetManager::GetAnimLink(int gaId, RJ_Config::ANIM_TYPE atAnimType)
{
	unsigned int _size = AnimLinkDeq.size();
	for (unsigned int _mesh = 0; _mesh < _size; _mesh++)
	{
		if(AnimLinkDeq[_mesh]->nLinkedID == gaId && AnimLinkDeq[_mesh]->AnimType == atAnimType)
			return AnimLinkDeq[_mesh];
	}
	return NULL;
}
//AnimLink* CAssetManager::GetAnimLink(const char* Name)
//{
//	unsigned int _size = AnimLinkDeq.size();
//	for (unsigned int _mesh = 0; _mesh < _size; _mesh++)
//	{
//		if(strcmp(AnimLinkDeq[_mesh]->AMesh->getName(),Name) == 0)
//			return AnimLinkDeq[_mesh];
//	}
//	return NULL;
//}

void CAssetManager::ResetEffects()
{
	bgoal1 = false;
	bgoal2 = false;
	bgoal3 = false;
	bgoal4 = false;
	bMotionLines = false;
	bDrip = false;
}

void CAssetManager::RenderEffects(double _dDelta)
{
	D3DXVECTOR3 cameraPos = D3DXVECTOR3(RJ_RENDERER->getCameraPosition()._41, 
			RJ_RENDERER->getCameraPosition()._42, RJ_RENDERER->getCameraPosition()._43);
	effectCollection.SetCameraPosition(cameraPos);
	effectCollision.SetCameraPosition(cameraPos);
	effectSmoke1.SetCameraPosition(cameraPos);
	effectSmoke2.SetCameraPosition(cameraPos);
	effectSmoke3.SetCameraPosition(cameraPos);
	effectDrip.SetCameraPosition(cameraPos);
	effectSplat.SetCameraPosition(cameraPos);
	effectJello.SetCameraPosition(cameraPos);
	effectSugar.SetCameraPosition(cameraPos);
	effectHat.SetCameraPosition(cameraPos);
	effectScrape.SetCameraPosition(cameraPos);
	effectSkid.SetCameraPosition(cameraPos);

	effectCollection.Render(_dDelta);
	effectCollision.Render(_dDelta);
	effectSmoke1.Render(_dDelta);
	effectSmoke2.Render(_dDelta);
	effectSmoke3.Render(_dDelta);
	effectDrip.Render(_dDelta);
	effectSplat.Render(_dDelta);
	effectJello.Render(_dDelta);
	effectHat.Render(_dDelta);
	effectScrape.Render(_dDelta);
	effectSkid.Render(_dDelta);

	if(bMotionLines)
	{
		D3DXMATRIX _rot1;
		D3DXMATRIX _translate1;
		D3DXMATRIX _world1;
		D3DXMatrixIdentity(&_rot1); D3DXMatrixIdentity(&_translate1); D3DXMatrixIdentity(&_world1);
		D3DXMatrixRotationY(&_rot1,atan2f(forward.x,forward.z));
		D3DXMatrixTranslation(&_translate1, motionPos.x, motionPos.y, motionPos.z);
		_world1 = _rot1 * _translate1;
		RJ_RENDERER->getDirect3DDevice()->SetTransform(D3DTS_WORLD, &_world1);
		MotionLines.Render(_dDelta);
	}

	if(bDrip)
	{
		D3DXMATRIX _worldGoal1;
		D3DXMatrixIdentity(&_worldGoal1);
		dripPos.y -= 0.5f;
		D3DXMatrixTranslation(&_worldGoal1, dripPos.x, dripPos.y, dripPos.z);
		RJ_RENDERER->getDirect3DDevice()->SetTransform(D3DTS_WORLD, &_worldGoal1);
		Drip.Render(_dDelta);
		/*volatile tPlaySoundEnum* tSCSOUND = new tPlaySoundEnum(EM_PLAYSOUNDENUM, RJ_Config::RJ_AMBIENT_SINKDRIP,true); 
		CEventManager::GetInstanceThreaded()->SendEventUpdateCrossToRender(ET_PLAYSOUND,(tBaseEventData*)tSCSOUND);
		tSCSOUND = NULL;*/
	}

	if(bgoal1)
	{
		D3DXMATRIX _worldGoal1;
		D3DXMatrixIdentity(&_worldGoal1);
		D3DXMatrixTranslation(&_worldGoal1, goalPos.x, goalPos.y, goalPos.z);
		RJ_RENDERER->getDirect3DDevice()->SetTransform(D3DTS_WORLD, &_worldGoal1);
		goalRing4.Render(_dDelta);
	}

	if(bgoal2)
	{
		D3DXMATRIX _worldGoal1;
		D3DXMatrixIdentity(&_worldGoal1);
		D3DXMatrixTranslation(&_worldGoal1, goalPos.x, goalPos.y, goalPos.z);
		RJ_RENDERER->getDirect3DDevice()->SetTransform(D3DTS_WORLD, &_worldGoal1);
		goalRing3.Render(_dDelta);
	}

	if(bgoal3)
	{
		D3DXMATRIX _worldGoal1;
		D3DXMatrixIdentity(&_worldGoal1);
		D3DXMatrixTranslation(&_worldGoal1, goalPos.x, goalPos.y, goalPos.z);
		RJ_RENDERER->getDirect3DDevice()->SetTransform(D3DTS_WORLD, &_worldGoal1);
		goalRing2.Render(_dDelta);
	}

	if(bgoal4)
	{
		D3DXMATRIX _worldGoal1;
		D3DXMatrixIdentity(&_worldGoal1);
		D3DXMatrixTranslation(&_worldGoal1, goalPos.x, goalPos.y, goalPos.z);
		RJ_RENDERER->getDirect3DDevice()->SetTransform(D3DTS_WORLD, &_worldGoal1);
		goalRing1.Render(_dDelta);
	}

	//if(waveBool)
	//{
	//	D3DXMATRIX _rot1;
	//	D3DXMATRIX _translate1;
	//	D3DXMATRIX _world1;
	//	D3DXMatrixIdentity(&_rot1); D3DXMatrixIdentity(&_translate1); D3DXMatrixIdentity(&_world1);
	//	D3DXMatrixRotationY(&_rot1,atan2f(forward.x,forward.z));
	//	D3DXMatrixTranslation(&_translate1, wavePos.x, wavePos.y, wavePos.z);
	//	_world1 = _rot1 * _translate1;
	//	RJ_RENDERER->getDirect3DDevice()->SetTransform(D3DTS_WORLD, &_world1);
	//	waveMesh.Render(_dDelta);
	//}

	//if(wakeBool)
	//{
	//	D3DXMATRIX _rot;
	//	D3DXMATRIX _translate;
	//	D3DXMATRIX _world;
	//	D3DXMatrixIdentity(&_rot); D3DXMatrixIdentity(&_translate); D3DXMatrixIdentity(&_world);
	//	D3DXMatrixRotationY(&_rot,atan2f(forward.x,forward.z));
	//	D3DXMatrixTranslation(&_translate, wakePos.x, wakePos.y, wakePos.z);
	//	_world = _rot * _translate;
	//	RJ_RENDERER->getDirect3DDevice()->SetTransform(D3DTS_WORLD, &_world);
	//	wakeMesh.Render(_dDelta);
	//	effectWake.SetForward(forward);
	//}
}

void CAssetManager::LoadEffects()
{
	effectCollection.Load("Collection");
	effectCollision.Load("Collision");
	effectScrape.Load("Scrape");
	effectSmoke1.Load("Smoke1");
	effectSmoke2.Load("Smoke2");
	effectSmoke3.Load("Smoke3");
	effectDrip.Load("Drip");
	effectSplat.Load("Splat");
	effectJello.Load("Splat");
	effectSugar.Load("Sugar");
	effectHat.Load("HatApperance1");
	effectSkid.Load("SkidMark");

	char TempFile[256];

	strcpy_s(TempFile,sizeof(char)*64,"Resources\\Models\\");
	strncat_s(TempFile, "DM_BullshineOne", strlen("DM_BullshineOne"));
	strncat_s(TempFile, ".mesh", 5);
	std::fstream FileInGoal1(TempFile, std::ios_base::in | std::ios_base::binary);
		goalRing1.Load(&FileInGoal1);
	FileInGoal1.close();

	strcpy_s(TempFile,sizeof(char)*64,"Resources\\Models\\");
	strncat_s(TempFile, "DM_BullshineTwo", strlen("DM_BullshineTwo"));
	strncat_s(TempFile, ".mesh", 5);
	std::fstream FileInGoal2(TempFile, std::ios_base::in | std::ios_base::binary);
		goalRing2.Load(&FileInGoal2);
	FileInGoal2.close();

	strcpy_s(TempFile,sizeof(char)*64,"Resources\\Models\\");
	strncat_s(TempFile, "DM_BullshineThree", strlen("DM_BullshineThree"));
	strncat_s(TempFile, ".mesh", 5);
	std::fstream FileInGoal3(TempFile, std::ios_base::in | std::ios_base::binary);
		goalRing3.Load(&FileInGoal3);
	FileInGoal3.close();

	strcpy_s(TempFile,sizeof(char)*64,"Resources\\Models\\");
	strncat_s(TempFile, "DM_BullshineFour", strlen("DM_BullshineFour"));
	strncat_s(TempFile, ".mesh", 5);
	std::fstream FileInGoal4(TempFile, std::ios_base::in | std::ios_base::binary);
		goalRing4.Load(&FileInGoal4);
	FileInGoal4.close();

	strcpy_s(TempFile,sizeof(char)*64,"Resources\\Models\\");
	strncat_s(TempFile, "DM_MotionCone", strlen("DM_MotionCone"));
	strncat_s(TempFile, ".mesh", 5);
	std::fstream Motion(TempFile, std::ios_base::in | std::ios_base::binary);
		MotionLines.Load(&Motion);
	Motion.close();

	strcpy_s(TempFile,sizeof(char)*64,"Resources\\Models\\");
	strncat_s(TempFile, "DM_DripSplash", strlen("DM_DripSplash"));
	strncat_s(TempFile, ".mesh", 5);
	std::fstream dRip(TempFile, std::ios_base::in | std::ios_base::binary);
		Drip.Load(&dRip);
	dRip.close();
}

void CAssetManager::ShutdownEffects()
{
	ClearSpectators();

	goalRing1.Shutdown();
	goalRing2.Shutdown();
	goalRing3.Shutdown();
	goalRing4.Shutdown();

	MotionLines.Shutdown();
	Drip.Shutdown();

	effectCollection.Shutdown();
	effectCollision.Shutdown();
	effectSmoke1.Shutdown();
	effectSmoke2.Shutdown();
	effectSmoke3.Shutdown();
	effectDrip.Shutdown();
	effectSplat.Shutdown();
	effectJello.Shutdown();
	effectSugar.Shutdown();
	effectHat.Shutdown();
	effectSkid.Shutdown();
}

void CAssetManager::AddSpectator(D3DXMATRIX mLocation, bool sitting)
{
	if(sitting)
	{
		int SpecSize = AnimSpectatorDeq.size();
		for(int i = 0; i < SpecSize; ++i)
		{
			if(AnimSpectatorDeq[i]->m_nCheerAnimID == 3)
			{
				AnimSpectatorDeq[i]->AddAntCheer(mLocation);
			}
		}
	}
	else
	{
		int randanim = rand()%3;
		int SpecSize = AnimSpectatorDeq.size();
		for(int i = 0; i < SpecSize; ++i)
		{
			if(AnimSpectatorDeq[i]->m_nCheerAnimID == randanim)
			{
				AnimSpectatorDeq[i]->AddAntCheer(mLocation);
			}
		}
	}
}
void CAssetManager::ClearSpectators()
{
	ClearCrowd();
	while (!AnimSpectatorDeq.empty())
	{
		CSpectator* DieSpec = AnimSpectatorDeq.front();
		AnimSpectatorDeq.pop_front();
		delete DieSpec;
		DieSpec = NULL;
	}
}
void CAssetManager::CreateSpectator(int nGlobalID, int nTypeID)
{
	CSpectator* cSpec = new CSpectator(nGlobalID);
	cSpec->Initialize(nTypeID);
	AnimSpectatorDeq.push_back(cSpec);
}
void CAssetManager::RenderSpectators(double dDelta)
{
	if(!RJ_GameMain::GetInstance()->getRenderPause())
	{
		unsigned int specsize = AnimSpectatorDeq.size();
		for(unsigned int i = 0; i < specsize; ++i)
			AnimSpectatorDeq[i]->Render(dDelta);
	}
}
void CAssetManager::ClearCrowd()
{
	int CrowdSize = AnimSpectatorDeq.size();
	for(int i = 0; i < CrowdSize; ++i)
	{
		while (!AnimSpectatorDeq[i]->m_dCheerDeck.empty())
			AnimSpectatorDeq[i]->m_dCheerDeck.pop_front();
	}
}