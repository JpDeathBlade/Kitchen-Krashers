#include "Mesh.h"
#include "..\\..\\CoreTech\\RedJaguarEngine\\Renderer\\RJ_Renderer.h"
#include "..\\..\\CoreTech\\RedJaguarEngine\\Renderer\\RJ_Vertex.h"
#include "..\\..\\CoreTech\\RedJaguarEngine\\Asset\\AssetManager.h"
#include "../../CoreTech/RedJaguarEngine/States/State_Config.h"

Mesh::Mesh(void)
{
	m_unNameLength		= 0;
	m_unTextureCount	= 0;
	m_unTextureNameLength = 0;
	m_unLightMapNameLength = 0;
	m_unNormalNameLength = 0;
	m_unSpecNameLength = 0;
	m_unShaderNameLength = 0;
	m_nTextureID = -1;
	m_nTextureIDTwo = -1;
	m_nTextureIDThree = -1;
	m_nTextureIDFour = -1;
	m_nTextureIDFive = -1;
	m_nTextureIDSix = -1;
	m_fBounce = 0.2f;
	time = 0.0f;

	Transparent = false;
	BackFaceCulled = false;
	Emissive = false;

	m_VertexList		= NULL;
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
	effectSugar = NULL;
	effectUpdate = true;

	switchBack = false;
	shaderTimer = -1.0;
	oldShader = NULL;
}

Mesh::~Mesh()
{
	Shutdown();
}

void Mesh::Initialize()
{
	if(strcmp(m_szName,"DM_SugarBit") == 0)
	{
		effectSugar = ASSETMANAGER->GetEffect_Sugar();
	}

	RJ_RENDERER->getDirect3DDevice()->CreateVertexBuffer(m_unVertexCount*sizeof(VertexFF1), 0,
		0, D3DPOOL_MANAGED, &m_vbVertexBuffer, 0);

	RJ_RENDERER->getDirect3DDevice()->CreateIndexBuffer(m_unIndexCount*sizeof(unsigned int), 0, D3DFMT_INDEX32, 
		D3DPOOL_MANAGED, &m_ibIndexBuffer, 0);

	void *pBuffer;
	m_vbVertexBuffer->Lock(0,0, &pBuffer, 0);
		memcpy(pBuffer, m_VertexList, m_unVertexCount*sizeof(VertexFF1));
	m_vbVertexBuffer->Unlock();
	m_ibIndexBuffer->Lock(0,0, &pBuffer, 0);
		memcpy(pBuffer, m_IndexList, m_unIndexCount*sizeof(unsigned int));
	m_ibIndexBuffer->Unlock();

	int shaderID = ASSETMANAGER->LoadShader(m_szShaderName);
	if(shaderID == -1)
		shaderID = ASSETMANAGER->LoadShader("Texture.fx");
	if(Transparent)
		shaderID = ASSETMANAGER->LoadShader("TextureBack.fx");
	m_Shader = ASSETMANAGER->GetShader(shaderID);

	// Obtain handles.
	m_Shader_Technique = m_Shader->GetTechniqueByName("ShaderTechnique");
	m_Shader_WorldVeiwProjection  = m_Shader->GetParameterByName(0, "m4x4_ViewProjection");
	m_Shader_TextureCoords = m_Shader->GetParameterByName(0, "s2D_TextureCoords_Tex");
	m_Shader_Time			= m_Shader->GetParameterByName(0, "f_Time");
	m_Shader_Bounce			= m_Shader->GetParameterByName(0, "f_Bounce");
}

void Mesh::setShader(ID3DXEffect* _shader, bool _switchBack, double _time )
{
	if(_shader)
	{
		if(_switchBack && (_shader != m_Shader))
		{
			oldShader = m_Shader;
		}
			m_Shader = _shader;
			switchBack = true;
			shaderTimer = _time;

			// Obtain handles.
			m_Shader_Technique = m_Shader->GetTechniqueByName("ShaderTechnique");
			m_Shader_WorldVeiwProjection  = m_Shader->GetParameterByName(0, "m4x4_ViewProjection");
			m_Shader_TextureCoords = m_Shader->GetParameterByName(0, "s2D_TextureCoords_Tex");
			m_Shader_Time			= m_Shader->GetParameterByName(0, "f_Time");
			m_Shader_Bounce			= m_Shader->GetParameterByName(0, "f_Bounce");
	}
}

void Mesh::Render(double fDelta)
{  
	if(switchBack)
	{
		static double time = 0.0;
		time += fDelta;

		if(time >= shaderTimer)
		{
			setShader(oldShader);
			oldShader = NULL;
			time = 0.0;
			shaderTimer = -1.0;
			switchBack = false;
		}
	}

	RJ_RENDERER->getDirect3DDevice()->SetVertexDeclaration(_BaseVertex::VertexDecl);
	RJ_RENDERER->getDirect3DDevice()->SetIndices(m_ibIndexBuffer);
	RJ_RENDERER->getDirect3DDevice()->SetStreamSource(0, m_vbVertexBuffer, 0, sizeof(VertexFF1));

	D3DXMATRIX _world;
	RJ_RENDERER->getDirect3DDevice()->GetTransform(D3DTS_WORLD, &_world);
	D3DXVECTOR3 _pos;
	_pos.x = _world._41; _pos.y = _world._42; _pos.z = _world._43;

	m_Shader->SetTechnique(m_Shader_Technique);
	m_Shader->SetMatrix(m_Shader_WorldVeiwProjection, &(_world * RJ_RENDERER->getViewMatrix() * RJ_RENDERER->getProjectionMatrix()));
	if(m_nTextureIDTwo >= 0)
	{
		switch(tStateConfig::GetInstanceRender()->m_nWormGum)
		{
		case 1:
			{
				m_Shader->SetTexture(m_Shader_TextureCoords, ASSETMANAGER->GetTexture(m_nTextureID));
				break;
			}
		case 2:
			{
				m_Shader->SetTexture(m_Shader_TextureCoords, ASSETMANAGER->GetTexture(m_nTextureIDTwo));
				break;
			}
		case 3:
			{
				m_Shader->SetTexture(m_Shader_TextureCoords, ASSETMANAGER->GetTexture(m_nTextureIDThree));
				break;
			}
		case 4:
			{
				m_Shader->SetTexture(m_Shader_TextureCoords, ASSETMANAGER->GetTexture(m_nTextureIDFour));
				break;
			}
		case 5:
			{
				m_Shader->SetTexture(m_Shader_TextureCoords, ASSETMANAGER->GetTexture(m_nTextureIDFive));
				break;
			}
		case 6:
			{
				m_Shader->SetTexture(m_Shader_TextureCoords, ASSETMANAGER->GetTexture(m_nTextureIDSix));
				break;
			}
		}
	}
	else
		m_Shader->SetTexture(m_Shader_TextureCoords, ASSETMANAGER->GetTexture(m_nTextureID));

	time += (float)fDelta;
	if(m_Shader_Time)
		m_Shader->SetFloat(m_Shader_Time, time + m_fCurrAdditive);
	if(m_Shader_Bounce)
		m_Shader->SetFloat(m_Shader_Bounce, m_fBounce);

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

	if(effectSugar)
	{
		if(!effectSugar->isPlaying())
		{
			effectSugar->SetPosition(_pos);
		}
		else
			effectSugar->SetPositionWithoutUpdate(_pos);
		effectSugar->Render(fDelta);
	}
}

void Mesh::Shutdown()
{
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

	if(m_VertexList)
	{
		delete[] m_VertexList;
		m_VertexList = NULL;
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

void Mesh::Update(double fDelta)
{

}

void Mesh::Load(std::fstream* _io)
{
	if(_io->is_open())
	{
		_io->read((char*)&m_unNameLength, sizeof(m_unNameLength));	//Length of the Mesh Name
		_io->read((char*)&m_szName, m_unNameLength);	//Mesh Name
		_io->read((char*)&m_unTextureCount, sizeof(m_unTextureCount));	//Number of Textures
			//m_unTextureNameLength = new unsigned int[m_unTextureCount];

		for(unsigned int _text = 0; _text < m_unTextureCount; _text++)
		{
			_io->read((char*)&m_unTextureNameLength, sizeof(m_unTextureNameLength));	//Length of Texture Name
			_io->read((char*)&m_szTextureName, m_unTextureNameLength);	//Texture Name
		}

		if(strcmp(m_szTextureName, "T_GumBlue_D.tga") == 0)
		{
			strcpy_s(m_szTextureName,260, "T_GumBlue_D.dds");
			char GumTexGreen[_MAX_PATH] = "T_GumGreen_D.dds";
			char GumTexOrange[_MAX_PATH] = "T_GumOrange_D.dds";
			char GumTexPink[_MAX_PATH] = "T_GumPink_D.dds";
			char GumTexPurple[_MAX_PATH] = "T_GumPurp_D.dds";
			char GumTexRed[_MAX_PATH] = "T_GumRed_D.dds";

			m_nTextureIDTwo = ASSETMANAGER->LoadTexture(GumTexGreen);
			m_nTextureIDThree = ASSETMANAGER->LoadTexture(GumTexOrange);
			m_nTextureIDFour = ASSETMANAGER->LoadTexture(GumTexPink);
			m_nTextureIDFive = ASSETMANAGER->LoadTexture(GumTexPurple);
			m_nTextureIDSix = ASSETMANAGER->LoadTexture(GumTexRed);
		}
		m_nTextureID = ASSETMANAGER->LoadTexture(m_szTextureName);
		//m_Texture = ASSETMANAGER->GetTexture(m_nTextureID);

		bool bDynamic = false;
		_io->read((char*)&bDynamic, sizeof(bool));

		_io->read((char*)&m_unLightMapNameLength, sizeof(m_unLightMapNameLength));
		_io->read((char*)&m_szLightMapName, m_unLightMapNameLength);

		_io->read((char*)&m_unNormalNameLength, sizeof(m_unNormalNameLength));
		_io->read((char*)&m_szNormalName, m_unNormalNameLength);

		_io->read((char*)&m_unSpecNameLength, sizeof(m_unSpecNameLength));
		_io->read((char*)&m_szSpecName, m_unSpecNameLength);

		_io->read((char*)&m_unShaderNameLength, sizeof(m_unShaderNameLength));
		_io->read((char*)&m_szShaderName, m_unShaderNameLength);

		_io->read((char*)&Transparent, sizeof(Transparent));
		_io->read((char*)&BackFaceCulled, sizeof(BackFaceCulled));
		_io->read((char*)&Emissive, sizeof(Emissive));
		_io->read((char*)&VolumeType, sizeof(VolumeType));

		_io->read((char*)&m_unVertexCount, sizeof(m_unVertexCount));	//Vertex Count
		m_VertexList = new VertexFF1[m_unVertexCount];

		for(unsigned int _vert = 0; _vert < m_unVertexCount; _vert++)
		{
			_io->read((char*)&m_VertexList[_vert].position, 3*sizeof(float));	//Position
			_io->read((char*)&m_VertexList[_vert].normal, 3*sizeof(float));	//Normal
			_io->read((char*)&m_VertexList[_vert].uvTexture, 2*sizeof(float));	//TextureUV
			_io->read((char*)&m_VertexList[_vert].color, 4*sizeof(float));	//Color
			m_VertexList[_vert].position.w = 1.0f;
			m_VertexList[_vert].normal.w = 1.0f;

			//m_VertexList[_vert].color = D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f);
		}

		_io->read((char*)&m_unPrimitiveCount, sizeof(m_unPrimitiveCount));	//Primitive Count
		m_unIndexCount = m_unPrimitiveCount * 3;
		m_IndexList = new unsigned int[m_unIndexCount];

		for(unsigned int _index = 0; _index < m_unIndexCount; _index++)
		{
			_io->read((char*)&m_IndexList[_index], sizeof(unsigned int));	//Index
		}

		Initialize();
	}
}

void Mesh::onLostDevice(void)
{
	
}

void Mesh::onResetDevice(void)
{

}