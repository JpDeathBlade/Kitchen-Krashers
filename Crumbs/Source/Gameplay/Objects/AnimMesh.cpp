#include "AnimMesh.h"
#include "../../CoreTech/RedJaguarEngine/Animations/AnimManager.h"
#include "../../CoreTech/RedJaguarEngine/Renderer/RJ_Renderer.h"
#include "../../CoreTech/RedJaguarEngine/Renderer/RJ_Vertex.h"
#include "../../CoreTech/RedJaguarEngine/Asset/AssetManager.h"
#include "../../CoreTech/RedJaguarEngine/States/State_Config.h"
#include "CSpectator.h"
#include "../GameAnim/AnthonyAnimStates.h"
#include "../GameAnim/BroomAnimStates.h"
#include "../GameAnim/GummyAnimStates.h"
#include "../GameAnim/ToothpickAnimStates.h"
#include "../../CoreTech/Utilities/RJ_MathLibrary.hpp"
#include "Mesh.h"
#define BAD_WEIGHT 372615

AnimMesh::AnimMesh(void)
{
	char blank[128] = "";
	m_unNameLength		= 0;
	m_unTextureCount	= 0;
	m_unTextureNameLength = 0;
	m_unLightMapNameLength = 0;
	m_unNormalNameLength = 0;
	m_unSpecNameLength = 0;
	m_unShaderNameLength = 0;
	m_szTextureName = blank;
	m_szLightMapName = blank;
	m_szNormalName = blank;
	m_szSpecName = blank;
	m_szShaderName = blank;
	m_Texture = NULL;
	m_TextureTwo = NULL;
	m_TextureThree = NULL;
	m_TextureFour = NULL;
	m_nTextureID = -1;
	m_nTextureIDTwo = -1;
	m_nTextureIDThree = -1;
	m_nTextureIDFour = -1;

	Transparent = false;
	BackFaceCulled = false;
	Emissive = false;

	m_AnimVertexList		= NULL;
	m_IndexList			= NULL;

	m_vbVertexBuffer	= NULL;
	m_ibIndexBuffer		= NULL;

	m_unVertexCount		= 0;
	m_unIndexCount		= 0;
	m_unPrimitiveCount	= 0;

	m_Shader			= NULL;
	m_Shader_Technique = NULL;
	m_Shader_WorldVeiwProjection = NULL;
	m_Shader_TextureCoords = NULL;

	m_MeshType			= RJ_Config::RJ_MT_UNNAMED;
}

AnimMesh::~AnimMesh()
{
	Shutdown();
}
float D3DV4GETI(const D3DXVECTOR4& _v, unsigned int i) 
{ 
	if(i == 0)
		return _v.x;
	else if(i == 1)
		return _v.y;
	else if(i == 2)
		return _v.z;
	else if(i == 3)
		return _v.w;
	return 0.0f;
}
float D3DV3GETI(const D3DXVECTOR3& _v, unsigned int i) 
{ 
	if(i == 0)
		return _v.x;
	else if(i == 1)
		return _v.y;
	else if(i == 2)
		return _v.z;
	return 0.0f;
}
D3DXVECTOR4 V4MULMAT(const D3DXMATRIX& _m, const D3DXVECTOR4& _v) 
{ 
	D3DXVECTOR4 retvec;
	retvec.x = (_m._11*_v.x) + (_m._21*_v.y) + (_m._31*_v.z) + _m._41;
	retvec.y = (_m._12*_v.x) + (_m._22*_v.y) + (_m._32*_v.z) + _m._42;
	retvec.z = (_m._13*_v.x) + (_m._23*_v.y) + (_m._33*_v.z) + _m._43;
	return retvec;
}
D3DXVECTOR3 V3MULMAT(const D3DXMATRIX& _m, const D3DXVECTOR3& _v) 
{ 
	D3DXVECTOR3 retvec;
	retvec.x = (_m._11*_v.x) + (_m._21*_v.y) + (_m._31*_v.z) + _m._41;
	retvec.y = (_m._12*_v.x) + (_m._22*_v.y) + (_m._32*_v.z) + _m._42;
	retvec.z = (_m._13*_v.x) + (_m._23*_v.y) + (_m._33*_v.z) + _m._43;
	return retvec;
}
void AnimMesh::Initialize()
{
	RJ_RENDERER->getDirect3DDevice()->CreateVertexBuffer(m_unVertexCount*sizeof(AnimatedVert), 0,
		0, D3DPOOL_MANAGED, &m_vbVertexBuffer, 0);

	RJ_RENDERER->getDirect3DDevice()->CreateIndexBuffer(m_unIndexCount*sizeof(unsigned int), 0, D3DFMT_INDEX32, 
		D3DPOOL_MANAGED, &m_ibIndexBuffer, 0);

	m_vbVertexBuffer->Lock(0,0, &pBuffer, 0);
	memcpy(pBuffer, m_AnimVertexList, m_unVertexCount*sizeof(AnimatedVert));
	m_vbVertexBuffer->Unlock();
	m_ibIndexBuffer->Lock(0,0, &pBuffer, 0);
	memcpy(pBuffer, m_IndexList, m_unIndexCount*sizeof(unsigned int));
	m_ibIndexBuffer->Unlock();

	// Create the FX from a .fx file.
	/*ID3DXBuffer* errors = 0;
	D3DXCreateEffectFromFile(RJ_RENDERER->getDirect3DDevice(), "Resources\\Shaders\\Texture.fx", 
	0, 0, D3DXSHADER_DEBUG, 0, &m_Shader, &errors);
	if( errors )
	MessageBox(0, (char*)errors->GetBufferPointer(), 0, 0);*/

	int shaderID = ASSETMANAGER->LoadShader("AnimShaderWin.fx");
	m_Shader = ASSETMANAGER->GetShader(shaderID);

	// Obtain handles.
	m_Shader_Technique = m_Shader->GetTechniqueByName("Normal");
	m_Shader_WorldVeiwProjection  = m_Shader->GetParameterByName(0, "g_mtxViewProj");
	m_Shader_TextureCoords = m_Shader->GetParameterByName(0, "s2D_TextureCoords_Tex");

	int numJoints = m_dqJointDeck.size();
	D3DXMATRIX empty(0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0);
	for(int matCount=0;matCount<numJoints;++matCount)
	{
		if(matCount<numJoints)
		{
			bonePoses[matCount] = m_dqJointDeck[matCount].TransformFrame.GetWorldMatDX();
			DXOrthoNormal(&bonePoses[matCount],&bonePoses[matCount]);
		}
		else
			bonePoses[matCount] = empty;
	}
	for (unsigned int _vert = 0; _vert < m_unVertexCount; _vert++)
	{	
		//D3DXVECTOR4 bindInverse = m_BindVerts[_vert] * (1.0f - (m_AnimVertexList[_vert].weights.x+m_AnimVertexList[_vert].weights.y+m_AnimVertexList[_vert].weights.z));
		m_AnimVertexList[_vert].position = V3MULMAT(bonePoses[(int)m_AnimVertexList[_vert].m_nBoneIndex.x], m_BindVerts[_vert]);
		//m_AnimVertexList[_vert].position.w = 1.0f;
		m_AnimVertexList[_vert].positionone = V3MULMAT(bonePoses[(int)m_AnimVertexList[_vert].m_nBoneIndex.y], m_BindVerts[_vert]);
		//m_AnimVertexList[_vert].positionone.w = 1.0f;
		m_AnimVertexList[_vert].positiontwo = V3MULMAT(bonePoses[(int)m_AnimVertexList[_vert].m_nBoneIndex.z], m_BindVerts[_vert]);
		//m_AnimVertexList[_vert].positiontwo.w = 1.0f;
	}
}
void OrthoNormalInverse( D3DXMATRIX &MatrixO, const D3DXMATRIX &MatrixA )
{
	MatrixO = MatrixA;
	D3DXMatrixTranspose(&MatrixO,&MatrixO);

	D3DXVECTOR3 axisx, axisy, axisz, axisw;
	axisx.x = MatrixA._11; axisx.y = MatrixA._12; axisx.z = MatrixA._13;
	axisy.x = MatrixA._21; axisy.y = MatrixA._22; axisy.z = MatrixA._23;
	axisz.x = MatrixA._31; axisz.y = MatrixA._32; axisz.z = MatrixA._33;
	axisw.x = MatrixA._41; axisw.y = MatrixA._42; axisw.z = MatrixA._43;

	MatrixO._41 = -D3DXVec3Dot(&axisx, &axisw);
	MatrixO._42 = -D3DXVec3Dot(&axisy, &axisw);
	MatrixO._43 = -D3DXVec3Dot(&axisz, &axisw);
	MatrixO._44 = 1;

	MatrixO._14 = 0;
	MatrixO._24 = 0;
	MatrixO._34 = 0;
}
void AnimMesh::Render(double fDelta, AnimStateMachine* m_AStateMachine, CSpectator* Attach)
{  
	D3DXMATRIX _world;
	RJ_RENDERER->getDirect3DDevice()->GetTransform(D3DTS_WORLD, &_world);
	D3DXMATRIX WorldMat = (_world * RJ_RENDERER->getViewMatrix() * RJ_RENDERER->getProjectionMatrix());

	D3DXMATRIX currPoses[49];

	int numJoints = m_dqJointDeck.size();
	int numFrames = (int)m_AStateMachine->GetAnimObject().m_vBlendedFrames.size();
	D3DXMATRIX empty(0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0);
	for(int matCount=0;matCount<numJoints;++matCount)
	{
		if(matCount<numJoints &&  numFrames > matCount)
		{
			currPoses[matCount] = m_AStateMachine->GetAnimObject().m_vBlendedFrames[matCount];
			if (m_AStateMachine->GetCurrentState()->GetType() == RJ_Config::RJ_ANIM_ANTHONY)
			{
				if(strcmp(m_dqJointDeck[matCount].m_szJointName.c_str(), "Hats") == 0)
					CAssetManager::getInstanceRender()->SetHatJoint(currPoses[matCount]);
			}
		}
		else
			currPoses[matCount] = empty;
		
		//D3DXMATRIX SphereMat = currPoses[matCount] * _world;
		//RJ_RENDERER->DrawSphere(&SphereMat,&D3DXVECTOR4(255,255,0,255),0.1f,0.1f,0.1f);
	}

	m_vbVertexBuffer->Lock(0,0, &pBuffer, 0);
	memcpy(pBuffer, m_AnimVertexList, m_unVertexCount*sizeof(AnimatedVert));
	m_vbVertexBuffer->Unlock();

	RJ_RENDERER->getDirect3DDevice()->SetVertexDeclaration(_AnimatedVertex::VertexDecl);
	RJ_RENDERER->getDirect3DDevice()->SetIndices(m_ibIndexBuffer);
	RJ_RENDERER->getDirect3DDevice()->SetStreamSource(0, m_vbVertexBuffer, 0, sizeof(AnimatedVert));

	m_Shader->SetTechnique(m_Shader_Technique);
	m_Shader->SetMatrix(m_Shader_WorldVeiwProjection, &WorldMat);
	m_Shader->SetMatrixArray("g_BoneMatricies",currPoses,numJoints);
	if(m_TextureTwo != NULL && m_AStateMachine->GetCurrentState()->GetType() == RJ_Config::RJ_ANIM_ANTHONY)
	{
		if(tStateConfig::GetInstanceRender()->m_nCurrPlayer == 0)
			m_Shader->SetTexture(m_Shader_TextureCoords, m_Texture);
		else if(tStateConfig::GetInstanceRender()->m_nCurrPlayer == 3)
			m_Shader->SetTexture(m_Shader_TextureCoords, m_TextureTwo);
		else if(tStateConfig::GetInstanceRender()->m_nCurrPlayer == 1)
			m_Shader->SetTexture(m_Shader_TextureCoords, m_TextureThree);
		else if(tStateConfig::GetInstanceRender()->m_nCurrPlayer == 2)
			m_Shader->SetTexture(m_Shader_TextureCoords, m_TextureFour);
	}
	else if(m_TextureTwo != NULL && m_AStateMachine->GetCurrentState()->GetType() == RJ_Config::RJ_ANIM_GUMMY)
	{
		switch(tStateConfig::GetInstanceRender()->m_nGummy)
		{
		case 1:
			{
				m_Shader->SetTexture(m_Shader_TextureCoords, m_Texture);
				break;
			}
		case 2:
			{
				m_Shader->SetTexture(m_Shader_TextureCoords, m_TextureTwo);
				break;
			}
		case 3:
			{
				m_Shader->SetTexture(m_Shader_TextureCoords, m_TextureThree);
				break;
			}
		case 4:
			{
				m_Shader->SetTexture(m_Shader_TextureCoords, m_TextureFour);
				break;
			}
		}
	}
	else
		m_Shader->SetTexture(m_Shader_TextureCoords, m_Texture);
	if(m_AStateMachine->GetCurrentState()->GetType() == RJ_Config::RJ_SPECTATOR && Attach != NULL)
	{
		unsigned int SpecSize = 0;
		if(!Attach->m_dCheerDeck.empty())
			SpecSize = Attach->m_dCheerDeck.size();
		for(unsigned int i = 0; i < SpecSize; ++i)
		{
			D3DXMATRIX AntMat = (Attach->m_dCheerDeck[i].mLoc * RJ_RENDERER->getViewMatrix() * RJ_RENDERER->getProjectionMatrix());
			m_Shader->SetMatrix(m_Shader_WorldVeiwProjection, &AntMat);
			if((Attach->m_dCheerDeck[i].nColor+1) == 4)
			m_Shader->SetTexture(m_Shader_TextureCoords, m_Texture);
			else if((Attach->m_dCheerDeck[i].nColor+1) == 3)
			m_Shader->SetTexture(m_Shader_TextureCoords, m_TextureTwo);
			else if((Attach->m_dCheerDeck[i].nColor+1) == 1)
			m_Shader->SetTexture(m_Shader_TextureCoords, m_TextureThree);
			else if((Attach->m_dCheerDeck[i].nColor+1) == 2)
			m_Shader->SetTexture(m_Shader_TextureCoords, m_TextureFour);
			m_Shader->CommitChanges();

			// Begin passes.
			UINT numPasses = 0;
			m_Shader->Begin(&numPasses, 0);
			for(UINT i = 0; i < numPasses; ++i)
			{
				m_Shader->BeginPass(i);
				RJ_RENDERER->getDirect3DDevice()->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, m_unVertexCount, 0, m_unPrimitiveCount);
				m_Shader->EndPass();
			}
			m_Shader->End();
		}
	}
	else
	{
		m_Shader->CommitChanges();

		// Begin passes.
		UINT numPasses = 0;
		m_Shader->Begin(&numPasses, 0);
		for(UINT i = 0; i < numPasses; ++i)
		{
			m_Shader->BeginPass(i);
			RJ_RENDERER->getDirect3DDevice()->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, m_unVertexCount, 0, m_unPrimitiveCount);
			m_Shader->EndPass();
		}
		m_Shader->End();
	}
}

void AnimMesh::Shutdown()
{
	for (unsigned int i = 0; i < m_dqJointDeck.size(); ++i)
	{
		m_dqJointDeck.pop_back();
	}
	m_dqJointDeck.clear();

	if(m_vbVertexBuffer)
	{
		m_vbVertexBuffer->Release();
		m_vbVertexBuffer = 0;
	}

	if(m_ibIndexBuffer)
	{
		m_ibIndexBuffer->Release();
		m_ibIndexBuffer = 0;
	}

	if(m_AnimVertexList)
	{
		delete[] m_AnimVertexList;
		m_AnimVertexList = NULL;
	}
	m_unVertexCount		= 0;

	if(m_IndexList)
	{
		delete[] m_IndexList;
		m_IndexList = NULL;
	}
	m_unIndexCount		= 0;
	m_unPrimitiveCount	= 0;

	/*if(m_Shader)
	{
	m_Shader->Release();
	m_Shader = NULL;
	}*/
}

void AnimMesh::Update(double fDelta)
{
	
}
void D3DV3SETFLOATI(D3DXVECTOR3& _v, unsigned int i, float Value) 
{ 
	if(i == 0)
		_v.x = Value;
	else if(i == 1)
		_v.y = Value;
	else if(i == 2)
		_v.z = Value;
}
void D3DV3SETUINTI(D3DXVECTOR3& _v, unsigned int i, unsigned int Value) 
{ 
	if(i == 0)
		_v.x = (float)Value;
	else if(i == 1)
		_v.y = (float)Value;
	else if(i == 2)
		_v.z = (float)Value;
}
void AnimMesh::Load(std::fstream* _io)
{
	if(_io->is_open())
	{
		char buffer[_MAX_PATH];
		_io->read((char*)&m_unNameLength, sizeof(m_unNameLength));	//Length of the Mesh Name
		_io->read((char*)&m_szName, m_unNameLength);	//Mesh Name
		_io->read((char*)&m_unTextureCount, sizeof(m_unTextureCount));	//Number of Textures
		//m_unTextureNameLength = new unsigned int[m_unTextureCount];
		char AntTexR[_MAX_PATH] = "T_Anthony_Red.dds";
		char GummyTex[_MAX_PATH] = "T_WormBlueRed_D.dds";
		char ToothTex[_MAX_PATH] = "T_Worm_D.tga";
		for(unsigned int _text = 0; _text < m_unTextureCount; _text++)
		{
			_io->read((char*)&m_unTextureNameLength, sizeof(m_unTextureNameLength));	//Length of Texture Name
			_io->read((char*)&buffer, m_unTextureNameLength);	//Texture Name
			if(strcmp(buffer,"T_Anthony_D_3.tga") == 0)
			{
				strcpy_s(buffer,260,AntTexR);
				char AntTexG[_MAX_PATH] = "T_Anthony_Green.dds";
				char AntTexP[_MAX_PATH] = "T_Anthony_Purple.dds";
				char AntTexO[_MAX_PATH] = "T_Anthony_Orange.dds";
				ASSETMANAGER->LoadTexture(AntTexG);
				ASSETMANAGER->LoadTexture(AntTexP);
				ASSETMANAGER->LoadTexture(AntTexO);
				m_nTextureIDTwo = ASSETMANAGER->LoadTexture(AntTexG);
				m_nTextureIDThree = ASSETMANAGER->LoadTexture(AntTexP);
				m_nTextureIDFour = ASSETMANAGER->LoadTexture(AntTexO);
				m_TextureTwo = ASSETMANAGER->GetTexture(m_nTextureIDTwo);
				m_TextureThree = ASSETMANAGER->GetTexture(m_nTextureIDThree);
				m_TextureFour = ASSETMANAGER->GetTexture(m_nTextureIDFour);
			}
			if(strcmp(buffer,"512 Checker 04.jpg") == 0)
			{
				strcpy_s(buffer,260,GummyTex);
				char GummyTexGY[_MAX_PATH] = "T_WormGreenYellow_D.dds";
				char GummyTexRG[_MAX_PATH] = "T_WormRedGreen_D.dds";
				char GummyTexRW[_MAX_PATH] = "T_WormRedWhite_D.dds";

				m_nTextureIDTwo = ASSETMANAGER->LoadTexture(GummyTexGY);
				m_nTextureIDThree = ASSETMANAGER->LoadTexture(GummyTexRG);
				m_nTextureIDFour = ASSETMANAGER->LoadTexture(GummyTexRW);
				m_TextureTwo = ASSETMANAGER->GetTexture(m_nTextureIDTwo);
				m_TextureThree = ASSETMANAGER->GetTexture(m_nTextureIDThree);
				m_TextureFour = ASSETMANAGER->GetTexture(m_nTextureIDFour);
			}
			if(strcmp(buffer,"512 Checker 04.jpg") == 0)
			{
				strcpy_s(buffer,260,ToothTex);
			}
			m_szTextureName = buffer;
		}

		m_nTextureID = ASSETMANAGER->LoadTexture(m_szTextureName);
		m_Texture = ASSETMANAGER->GetTexture(m_nTextureID);

		bool bDynamic = true;
		_io->read((char*)&bDynamic, sizeof(bool));

		_io->read((char*)&m_unLightMapNameLength, sizeof(m_unLightMapNameLength));
		_io->read((char*)&buffer, m_unLightMapNameLength);
		m_szLightMapName = buffer;

		_io->read((char*)&m_unNormalNameLength, sizeof(m_unNormalNameLength));
		_io->read((char*)&buffer, m_unNormalNameLength);
		m_szNormalName = buffer;

		_io->read((char*)&m_unSpecNameLength, sizeof(m_unSpecNameLength));
		_io->read((char*)&buffer, m_unSpecNameLength);
		m_szSpecName = buffer;

		_io->read((char*)&m_unShaderNameLength, sizeof(m_unShaderNameLength));
		_io->read((char*)&buffer, m_unShaderNameLength);
		m_szShaderName = buffer;

		_io->read((char*)&Transparent, sizeof(Transparent));
		_io->read((char*)&BackFaceCulled, sizeof(BackFaceCulled));
		_io->read((char*)&Emissive, sizeof(Emissive));
		_io->read((char*)&VolumeType, sizeof(VolumeType));

		_io->read((char*)&m_unVertexCount, sizeof(m_unVertexCount));	//Vertex Count
		m_AnimVertexList = new AnimatedVert[m_unVertexCount];

		for(unsigned int _vert = 0; _vert < m_unVertexCount; _vert++)
		{
			_io->read((char*)&m_AnimVertexList[_vert].position, 3*sizeof(float));	//Position
			D3DXVECTOR3 d3skin; d3skin = m_AnimVertexList[_vert].position;
			//d3skin.w = 1.0f;
			m_BindVerts.push_back(d3skin);
			D3DXVECTOR3 ThrowAway;
			D3DXVECTOR4 AlsoThrowAway;
			_io->read((char*)&ThrowAway, 3*sizeof(float));	//Normal
			_io->read((char*)&m_AnimVertexList[_vert].uvTexture, 2*sizeof(float));	//TextureUV
			_io->read((char*)&AlsoThrowAway, 4*sizeof(float));	//Color
			unsigned int numinf;
			_io->read((char*)&numinf, sizeof(unsigned int));
			for (unsigned int i = 0; i < numinf; ++i)
			{
				if(i >= 3)
					break;
				float xyzweight;
				unsigned xyzindc;
				_io->read((char*)&xyzindc, sizeof(unsigned int));
				_io->read((char*)&xyzweight, sizeof(float));
				//if(xyzweight == 0)
				//	xyzweight = 1;
				D3DV3SETFLOATI(m_AnimVertexList[_vert].weights,i,xyzweight);
				D3DV3SETUINTI(m_AnimVertexList[_vert].m_nBoneIndex,i,xyzindc);
			}
			for (unsigned int i = (numinf); i < 3; ++i )
			{
				//if(i >= BAD_WEIGHT)
					//continue;
				D3DV3SETFLOATI(m_AnimVertexList[_vert].weights,i,0.0000f);
				D3DV3SETUINTI(m_AnimVertexList[_vert].m_nBoneIndex,i,0);
			}
			//m_AnimVertexList[_vert].position.w = 1.0f;
			//m_AnimVertexList[_vert].normal.w = 1.0f;
			//m_AnimVertexList[_vert].color = D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f);
		}

		_io->read((char*)&m_unPrimitiveCount, sizeof(m_unPrimitiveCount));	//Primitive Count
		m_unIndexCount = m_unPrimitiveCount * 3;
		m_IndexList = new unsigned int[m_unIndexCount];

		D3DXVECTOR3 IndCVec;
		for(unsigned int _index = 0; _index < m_unIndexCount; _index++)
		{
			if(_index%3 == 0 && _index != 0)
				m_IndCVecs.push_back(IndCVec);
			_io->read((char*)&m_IndexList[_index], sizeof(size_t));	//Index
		}
		
		//LOAD BONES
		_io->read((char*)&m_nNumBones, sizeof(size_t));
		m_dqJointDeck.resize(m_nNumBones);
		//int bonecount = 0;
		for(size_t iBones = 0; iBones < m_nNumBones; iBones++)
		{
			unsigned int boneid;
			_io->read((char*)&boneid, sizeof(size_t));
			m_dqJointDeck[iBones].TransformFrame.SetBoneIndex(boneid);
			//bonecount++;
			unsigned int bonenamelen;
			_io->read((char*)&bonenamelen, sizeof(unsigned int));
			char szJointName[128];
			_io->read((char*)&szJointName, bonenamelen);
			m_dqJointDeck[iBones].m_szJointName = szJointName;
			_io->read((char*)&m_dqJointDeck[iBones].TransformFrame.m_iParentIndex, sizeof(unsigned int));

			for(unsigned int irows = 0; irows < 4; irows++)
			{
				for(unsigned int icols = 0; icols < 4; icols++)
				{
					float LocalMat = 0.0f;
					_io->read((char*)&LocalMat, sizeof(float));
					m_dqJointDeck[iBones].TransformFrame.GetWorldMatDX().m[irows][icols] = LocalMat;
				}
			}
			m_dqJointDeck[iBones].TransformFrame.Update();

			int numChildren = 0;
			_io->read((char*)&numChildren,sizeof(int));
			m_dqJointDeck[iBones].TransformFrame.m_nNumChildren = numChildren;
			for(size_t iChild = 0; iChild < (size_t)numChildren; iChild++)
			{
				int childIndex;
				_io->read((char*)&childIndex, sizeof(unsigned int));
				m_dqJointDeck[iBones].TransformFrame.AddChild(&m_dqJointDeck[childIndex].TransformFrame);
				char* childName;
				int childNameLen;
				_io->read((char*)&childNameLen, sizeof(unsigned int));
				childName = new char[childNameLen];
				_io->read(childName, childNameLen);
				std::string temp = childName;
				m_dqJointDeck[childIndex].TransformFrame.SetBoneName(temp);
				delete[] childName;
			}
		}
		m_dqJointDeck[0].TransformFrame.Update();
		int jointCount = m_dqJointDeck.size();
		for(unsigned int numJoint=0;numJoint<(unsigned)jointCount;++numJoint)
		{
			m_dqJointDeck[numJoint].TransformFrame.GetWorldMatDX();
		}

		this->Initialize();
	}
}

void AnimMesh::onLostDevice(void)
{

}

void AnimMesh::onResetDevice(void)
{

}

D3DXMATRIX& MatrixHierarchy::GetWorldMatDX()
{
	//this->m_nNumChildren *= 1;
	if( (m_bFlag) == DIRTY )
	{
		//MatrixHierarchy* TESTME = GetParent();
		//if( GetParent() != NULL )
			//m_WorldMat = m_LocalMat * ((EDFrame *)GetParent())->GetWorldMat();
			//D3DXMatrixMultiply(&m_WorldMat,&m_LocalMat,&(GetParent()->m_WorldMat));
		//else
			//m_WorldMat = m_LocalMat;

		m_bFlag = !DIRTY;
	}

	return m_WorldMat;
}

MatrixHierarchy* MatrixHierarchy::AddChild(MatrixHierarchy *pNode)
{
	if( pNode->GetParent() != 0 )
		return 0;

	pNode->m_pParent = this;
	m_vrgChildren.push_back( pNode );
	return pNode;
}

MatrixHierarchy* MatrixHierarchy::RemoveChild(MatrixHierarchy* pNode)
{
	if( pNode->GetParent() != this )
		return 0;

	unsigned int nChildSize = m_vrgChildren.size();
	for( unsigned int i = 0; i < nChildSize; ++i )
	{
		if( m_vrgChildren[i] == pNode )
		{
			m_vrgChildren.erase( m_vrgChildren.begin() + i );
			pNode->m_pParent = 0;
			return pNode;
		}
	}

	return 0;
}

void DXInvert(D3DXMATRIX *mat)
{
	*mat = D3DXMATRIX((*mat)._11, (*mat)._21, (*mat)._31, 0,
		(*mat)._12, (*mat)._22, (*mat)._32, 0,
		(*mat)._13,(*mat)._23, (*mat)._33, 0,
		-((*mat)._41 * (*mat)._11 + (*mat)._42 * (*mat)._12 + (*mat)._43 * (*mat)._13),
		-((*mat)._41 * (*mat)._21 + (*mat)._42* (*mat)._22 + (*mat)._43 * (*mat)._23),
		-((*mat)._41 * (*mat)._31 + (*mat)._42 * (*mat)._32 + (*mat)._43 * (*mat)._33), 1);
}

void DXNorm(D3DXVECTOR3& vec)
{
	float inveSqrt = 1/(float)sqrt((vec.x*vec.x)+(vec.y*vec.y)+(vec.z*vec.z));
	vec.x*=inveSqrt;
	vec.y*=inveSqrt;
	vec.z*=inveSqrt;
}

D3DXMATRIX interpolate(D3DXMATRIX* matA,D3DXMATRIX* matB,float lamda)
{
	D3DXMATRIX iMat(*matA);
	D3DXMATRIX result(*matB);

	// Inverse of MatrixA
	//iMat.invert();
	//D3DXMatrixInverse(&iMat,NULL,&iMat);
	DXInvert(&iMat);
	// Remove MatrixA's transformation from MatrixB
	result = iMat * result;

	// iMat is now the intermediary transformation from MatrixA to MatrixB
	// ie: iMat * MatrixA = MatrixB
	iMat = result;

	// The trace of our matrix
	float trace = 1.0f + iMat.m[0][0] + iMat.m[1][1] + iMat.m[2][2];

	float quatResult[4];

	//Calculate the quaternion of iMat
	//If trace is greater than 0, but consider small values that
	//might result in 0 when operated upon due to floating point error
	if( trace > 0.00000001 )
	{
		float S = sqrt(trace)*2;
		quatResult[0] = (iMat.m[2][1] - iMat.m[1][2]) / S;
		quatResult[1] = (iMat.m[0][2] - iMat.m[2][0]) / S;
		quatResult[2] = (iMat.m[1][0] - iMat.m[0][1]) / S;
		quatResult[3] = 0.25f * S;
	}
	else
	{
		if( iMat.m[0][0] > iMat.m[1][1] && iMat.m[0][0] > iMat.m[2][2] )
		{
			float S = (float)(sqrt( 1.0 + iMat.m[0][0] - iMat.m[1][1] - iMat.m[2][2] ) * 2);
			quatResult[0] = 0.25f * S;
			quatResult[1] = (iMat.m[1][0] + iMat.m[0][1]) / S;
			quatResult[2] = (iMat.m[0][2] + iMat.m[2][0]) / S;
			quatResult[3] = (iMat.m[2][1] - iMat.m[1][2]) / S;
		}
		else if( iMat.m[1][1] > iMat.m[2][2] )
		{
			float S = (float)(sqrt( 1.0 + iMat.m[1][1] - iMat.m[0][0] - iMat.m[2][2] ) * 2);
			quatResult[0] = (iMat.m[1][0] + iMat.m[0][1]) / S;
			quatResult[1] = 0.25f * S;
			quatResult[2] = (iMat.m[2][1] + iMat.m[1][2]) / S;
			quatResult[3] = (iMat.m[0][2] - iMat.m[2][0]) / S;
		}
		else
		{
			float S = (float)(sqrt( 1.0 + iMat.m[2][2] - iMat.m[0][0] - iMat.m[1][1] ) * 2);
			quatResult[0] = (iMat.m[0][2] + iMat.m[2][0]) / S;
			quatResult[1] = (iMat.m[2][1] + iMat.m[1][2]) / S;
			quatResult[2] = 0.25f * S;
			quatResult[3] = (iMat.m[1][0] - iMat.m[0][1]) / S;
		}
	}

	//Get the magnitude of our quaternion
	float quatMagnitude = float(sqrt( quatResult[0]*quatResult[0] + quatResult[1]*quatResult[1] + quatResult[2]*quatResult[2] + quatResult[3]*quatResult[3] ));

	// Normalize our quaternion
	float quatNormalized[4] = { quatResult[0]/quatMagnitude, quatResult[1]/quatMagnitude, quatResult[2]/quatMagnitude, quatResult[3]/quatMagnitude }; 

	// Calculate the angles relevant to our quaternion
	float cos_a = quatNormalized[3];
	float angle = acos( cos_a ) * 2;
	float sin_a = (float)sqrt( 1.0 - cos_a * cos_a );

	// If there was no rotation between matrices, calculation
	//of the rotation matrix will end badly. So just do the linear
	// interpolation of the translation component and return
	if( angle == 0.0 )
	{
		result = *matA;

		result.m[3][0] = (*matA).m[3][0] + (((*matB).m[3][0]-(*matA).m[3][0])*lamda);
		result.m[3][1] = (*matA).m[3][1] + (((*matB).m[3][1]-(*matA).m[3][1])*lamda);
		result.m[3][2] = (*matA).m[3][2] + (((*matB).m[3][2]-(*matA).m[3][2])*lamda);

		return result;
	}


	//Our axis of abitrary rotation
	D3DXVECTOR3 axis;
	//vec3f axisTemp;

	if( fabs( sin_a ) < 0.0005 )
		sin_a = 1;

	axis.x = quatNormalized[0] / sin_a;
	axis.y = quatNormalized[1] / sin_a;
	axis.z = quatNormalized[2] / sin_a;

	//Get the portion of the angle to rotate by
	angle *= lamda;
	///axisTemp.x = axis.x;
	//axisTemp.y = axis.y;
	//axisTemp.z = axis.z;

	DXNorm(axis);
	//axisTemp.normalize();
	//axis

	// Calculate the quaternion for our new (partial) angle of rotation
	sin_a = sin( angle / 2 );
	cos_a = cos( angle / 2 );
	quatNormalized[0] = axis.x * sin_a;
	quatNormalized[1] = axis.y * sin_a;
	quatNormalized[2] = axis.z * sin_a;
	quatNormalized[3] = cos_a;

	quatMagnitude = sqrt( quatNormalized[0]*quatNormalized[0] + quatNormalized[1]*quatNormalized[1] + quatNormalized[2]*quatNormalized[2] + quatNormalized[3]*quatNormalized[3] );		
	quatNormalized[0] /= quatMagnitude;
	quatNormalized[1] /= quatMagnitude;
	quatNormalized[2] /= quatMagnitude;
	quatNormalized[3] /= quatMagnitude;

	// Calculate our partial rotation matrix
	float xx      = quatNormalized[0] * quatNormalized[0];
	float xy      = quatNormalized[0] * quatNormalized[1];
	float xz      = quatNormalized[0] * quatNormalized[2];
	float xw      = quatNormalized[0] * quatNormalized[3];
	float yy      = quatNormalized[1] * quatNormalized[1];
	float yz      = quatNormalized[1] * quatNormalized[2];
	float yw      = quatNormalized[1] * quatNormalized[3];
	float zz      = quatNormalized[2] * quatNormalized[2];
	float zw      = quatNormalized[2] * quatNormalized[3];

	result.m[0][0]  = 1 - 2 * ( yy + zz );
	result.m[0][1]  =     2 * ( xy - zw );
	result.m[0][2]  =     2 * ( xz + yw );
	result.m[1][0]  =     2 * ( xy + zw );
	result.m[1][1]  = 1 - 2 * ( xx + zz );
	result.m[1][2]  =     2 * ( yz - xw );
	result.m[2][0]  =     2 * ( xz - yw );
	result.m[2][1]  =     2 * ( yz + xw );
	result.m[2][2] = 1 - 2 * ( xx + yy );
	result.m[0][3]  = result.m[1][3] = result.m[2][3] = result.m[3][0] = result.m[3][1] = result.m[3][2] = 0;
	result.m[3][3] = 1;

	//Combine our partial rotation with MatrixA
	result = (*matA) * result;

	//Linear interpolation of the translation components of the matrices
	result.m[3][0] = (*matA).m[3][0] + (((*matB).m[3][0]-(*matA).m[3][0])*lamda);
	result.m[3][1] = (*matA).m[3][1] + (((*matB).m[3][1]-(*matA).m[3][1])*lamda);
	result.m[3][2] = (*matA).m[3][2] + (((*matB).m[3][2]-(*matA).m[3][2])*lamda);

	return result;
}

D3DXMATRIX interpolate(decompsedMat& mat1, decompsedMat& mat2, float delta)
{
	D3DXMATRIX tempMat;
	D3DXVECTOR3 scaleB;
	D3DXVECTOR3 transB;
	D3DXQUATERNION rotB;

	D3DXVec3Lerp(&scaleB,&mat1.scale,&mat2.scale,delta);
	D3DXVec3Lerp(&transB,&mat1.translate,&mat2.translate,delta);
	D3DXQuaternionSlerp(&rotB,&mat1.rot,&mat2.rot,delta);

	/*m_vBlendedFrames[i] =*/ D3DXMatrixTransformation(&tempMat,NULL,NULL,&scaleB,NULL,&rotB,&transB);//interpolate(&prevAnim,&nextAnim,dissolve);
	return tempMat;
}

void DXOrthoNormal(D3DXMATRIX* MatrixO,const D3DXMATRIX* MatrixA)
{
	D3DXMatrixInverse(MatrixO,NULL,MatrixA);
}
void MatrixHierarchy::Update()
{
	if( (m_bFlag) != DIRTY )
	{
		m_bFlag = DIRTY;

		unsigned int nChildSize = GetChildren().size();
		for( unsigned int i = 0; i < nChildSize; ++i )
			GetChildren()[i]->Update();
	}
}