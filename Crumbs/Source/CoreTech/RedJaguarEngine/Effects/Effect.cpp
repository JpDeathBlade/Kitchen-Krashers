#include "Effect.h"
#include "..\\Asset\\AssetManager.h"
#include <fstream>

Effect::Effect()
{
	m_nTextureID	= 0;
	m_nMapWidth		= 0;
	m_nMapHeight	= 0;
	m_nCellWidth	= 0;
	m_nCellHeight	= 0;
	m_nColumnCount	= 0;
	m_nRowCount		= 0;
	m_nEffectType   = 0;
	m_dCurrentTime = 0.0;
	time = 0.0f;

	m_fEffectWidth	= 0.0f;
	m_fEffectHeight = 0.0f;
	m_nCurrentFrame = 0;
	m_fOffsetX		= 0.0f;
	m_fOffsetY		= 0.0f;
	m_nScale		= 1.0f;
	m_bPlayedOnce = true;
	m_bBillboarded = true;
	m_bMultiTextured = false;
}

void Effect::SetCameraPosition(D3DXVECTOR3 _position)
{
	cam_pos.x = _position.x;
	cam_pos.y = _position.y;
	cam_pos.z = _position.z;
}

void Effect::BillBoard()
{
	// get the object's current xyz pos
	D3DXVECTOR3 bill_pos(_world._41, _world._42, _world._43);
	
	D3DXVECTOR3 vec_z = bill_pos - cam_pos;

	// cross the world up vector with vec_z
	D3DXVECTOR3 vec_x;
	D3DXVec3Cross(&vec_x, &D3DXVECTOR3(0,1,0), &vec_z);

	D3DXVECTOR3 vec_y;
	D3DXVec3Cross(&vec_y, &vec_z, &vec_x);

	// normalize the three vectors 0.0 - 1.0
	D3DXVec3Normalize(&vec_z, &vec_z);
	D3DXVec3Normalize(&vec_x, &vec_x);
	D3DXVec3Normalize(&vec_y, &vec_y);

	// set the new vectors. when applied will direct the object towards the camera
	// forward vector
	_world._31 = vec_z.x;
	_world._32 = vec_z.y;
	_world._33 = vec_z.z;

	// up vector
	_world._21 = vec_y.x;
	_world._22 = vec_y.y;
	_world._23 = vec_y.z;

	// right vector
	_world._11 = vec_x.x;
	_world._12 = vec_x.y;
	_world._13 = vec_x.z;
}

void Effect::Load(char* _effectName)
{
	char TempFile[256];
		strcpy_s(TempFile,sizeof(char)*64,"Resources\\Effects\\");
		strncat_s(TempFile, _effectName, strlen(_effectName));
		strncat_s(TempFile, ".effect", 7);

	std::fstream FileIn(TempFile, std::ios_base::in);

		char cBuffer[64];
		FileIn.getline(cBuffer, 64, '\n');
		sscanf_s(cBuffer,"%f %f %f",
			&m_v4AlphaColor[0], &m_v4AlphaColor[1],
			&m_v4AlphaColor[2]);

		FileIn.getline(cBuffer, 64, '\n');
		sscanf_s(cBuffer,"%d", &m_nEffectType);

		FileIn.getline(cBuffer, 64, '\n');
		m_nTextureID = ASSETMANAGER->LoadTexture(cBuffer);

		if(m_nEffectType == 1)
		{
			m_bMultiTextured = true;
			FileIn.getline(cBuffer, 64, '\n');
			m_nAlphaTextureID = ASSETMANAGER->LoadTexture(cBuffer);
			m_bBillboarded = false;
		}
		else if(m_nEffectType == 2 || m_nEffectType == 4)
		{
			m_bBillboarded = false;
		}

		FileIn.getline(cBuffer, 64, '\n');
		sscanf_s(cBuffer,"%f", &m_nScale);
		FileIn.getline(cBuffer, 64, '\n');
		sscanf_s(cBuffer,"%d", &m_nMapWidth);
		FileIn.getline(cBuffer, 64, '\n');
		sscanf_s(cBuffer,"%d", &m_nMapHeight);
		FileIn.getline(cBuffer, 64, '\n');
		sscanf_s(cBuffer,"%d", &m_nCellWidth);
		FileIn.getline(cBuffer, 64, '\n');
		sscanf_s(cBuffer,"%d", &m_nCellHeight);
		FileIn.getline(cBuffer, 64, '\n');
		sscanf_s(cBuffer,"%d", &m_nColumnCount);
		FileIn.getline(cBuffer, 64, '\n');
		sscanf_s(cBuffer,"%d", &m_nRowCount);
		FileIn.getline(cBuffer, 64, '\n');
		sscanf_s(cBuffer,"%d", &m_nFrameCount);
		FileIn.getline(cBuffer, 64, '\n');
		sscanf_s(cBuffer,"%f", &m_fFrameDuration);

		m_fEffectWidth = (1.0f / m_nColumnCount);
		m_fEffectHeight = (1.0f / m_nRowCount);

	FileIn.close();

	if(m_bMultiTextured)
	{
		// Create the FX from a .fx file.
		ID3DXBuffer* errors = 0;
		D3DXCreateEffectFromFile(RJ_RENDERER->getDirect3DDevice(), "Resources\\Shaders\\EffectsAlpha.fx", 
			0, 0, D3DXSHADER_DEBUG, 0, &m_Shader, &errors);
		if( errors )
			MessageBox(0, (char*)errors->GetBufferPointer(), 0, 0);
	}
	else if(m_nEffectType == 3)
	{
		// Create the FX from a .fx file.
		ID3DXBuffer* errors = 0;
		D3DXCreateEffectFromFile(RJ_RENDERER->getDirect3DDevice(), "Resources\\Shaders\\EffectDrip.fx", 
			0, 0, D3DXSHADER_DEBUG, 0, &m_Shader, &errors);
		if( errors )
			MessageBox(0, (char*)errors->GetBufferPointer(), 0, 0);
	}
	else
	{
		// Create the FX from a .fx file.
		ID3DXBuffer* errors = 0;
		D3DXCreateEffectFromFile(RJ_RENDERER->getDirect3DDevice(), "Resources\\Shaders\\Effect.fx", 
			0, 0, D3DXSHADER_DEBUG, 0, &m_Shader, &errors);
		if( errors )
			MessageBox(0, (char*)errors->GetBufferPointer(), 0, 0);
	}

	// Obtain handles.
	m_Shader_Technique = m_Shader->GetTechniqueByName("ShaderTechnique");
	m_Shader_VeiwProjection  = m_Shader->GetParameterByName(0, "m4x4_ViewProjectionMatrix");
	m_Shader_TextureCoords = m_Shader->GetParameterByName(0, "s2d_Diffused_Tex");
	m_Shader_AlphaTextureCoords = m_Shader->GetParameterByName(0, "s2D_AlphaTextureCoords_Tex");
	m_Shader_OffsetX = m_Shader->GetParameterByName(0, "f_OffsetX");
	m_Shader_OffsetY = m_Shader->GetParameterByName(0, "f_OffsetY");
	m_Shader_Time = m_Shader->GetParameterByName(0, "f_Time");

	float tempScale = m_nScale / 2.0f;
	if(m_bMultiTextured || m_nEffectType == 2)
	{
		m_VertexList[0].position.x = tempScale; m_VertexList[0].position.y = 0.05f; m_VertexList[0].position.z = 0.0f;
		m_VertexList[1].position.x = -tempScale; m_VertexList[1].position.y = 0.05f; m_VertexList[1].position.z = 0.0f;
		m_VertexList[2].position.x = -tempScale; m_VertexList[2].position.y = 0.05f; m_VertexList[2].position.z = tempScale;
		m_VertexList[3].position.x = tempScale; m_VertexList[3].position.y = 0.05f; m_VertexList[3].position.z = tempScale;
	}
	else if(m_nEffectType == 3)
	{
		m_VertexList[0].position.x = tempScale; m_VertexList[0].position.y = 0.0f; m_VertexList[0].position.z = 0.0f;
		m_VertexList[1].position.x = -tempScale; m_VertexList[1].position.y = 0.0f; m_VertexList[1].position.z = 0.0f;
		m_VertexList[2].position.x = -tempScale; m_VertexList[2].position.y = 11.0f; m_VertexList[2].position.z = 0.0f;
		m_VertexList[3].position.x = tempScale; m_VertexList[3].position.y = 11.0f; m_VertexList[3].position.z = 0.0f;
	}
	else if(m_nEffectType == 4)
	{
		m_VertexList[0].position.x = tempScale; m_VertexList[0].position.y = 0.05f; m_VertexList[0].position.z = 0.0f;
		m_VertexList[1].position.x = -tempScale; m_VertexList[1].position.y = 0.05f; m_VertexList[1].position.z = 0.0f;
		m_VertexList[2].position.x = -tempScale; m_VertexList[2].position.y = 0.05f; m_VertexList[2].position.z = -tempScale;
		m_VertexList[3].position.x = tempScale; m_VertexList[3].position.y = 0.05f; m_VertexList[3].position.z = -tempScale;
	}
	else
	{
		m_VertexList[0].position.x = tempScale; m_VertexList[0].position.y = 0.0f; m_VertexList[0].position.z = 0.0f;
		m_VertexList[1].position.x = -tempScale; m_VertexList[1].position.y = 0.0f; m_VertexList[1].position.z = 0.0f;
		m_VertexList[2].position.x = -tempScale; m_VertexList[2].position.y = tempScale; m_VertexList[2].position.z = 0.0f;
		m_VertexList[3].position.x = tempScale; m_VertexList[3].position.y = tempScale; m_VertexList[3].position.z = 0.0f;
	}
		m_VertexList[0].position.w = 1.0f;
		m_VertexList[1].position.w = 1.0f;
		m_VertexList[2].position.w = 1.0f;
		m_VertexList[3].position.w = 1.0f;

	float width = (float)m_nCellWidth / (float)m_nMapWidth;
	float height = (float)m_nCellHeight / (float)m_nMapHeight;
	

	if(m_nEffectType == 2)
	{
		m_VertexList[0].textureUV.x = width; m_VertexList[3].textureUV.y = height;
		m_VertexList[1].textureUV.x = 0.0f; m_VertexList[2].textureUV.y = height;
		m_VertexList[2].textureUV.x = 0.0f; m_VertexList[1].textureUV.y = 0.0f;
		m_VertexList[3].textureUV.x = width; m_VertexList[0].textureUV.y = 0.0f;
	}
	else
	{
		m_VertexList[3].textureUV.x = width; m_VertexList[0].textureUV.y = height;
		m_VertexList[2].textureUV.x = 0.0f; m_VertexList[1].textureUV.y = height;
		m_VertexList[1].textureUV.x = 0.0f; m_VertexList[2].textureUV.y = 0.0f;
		m_VertexList[0].textureUV.x = width; m_VertexList[3].textureUV.y = 0.0f;
	}


	RJ_RENDERER->getDirect3DDevice()->CreateVertexBuffer(6*sizeof(m_VertexList[0]), 0,
		0, D3DPOOL_MANAGED, &m_vbVertexBuffer, 0);

	EffectVertex* pBuffer;
	m_vbVertexBuffer->Lock(0,0, (void**)&pBuffer, 0);
	{
		pBuffer[0] = m_VertexList[0];
		pBuffer[1] = m_VertexList[1];
		pBuffer[2] = m_VertexList[2];

		pBuffer[3] = m_VertexList[2];
		pBuffer[4] = m_VertexList[3];
		pBuffer[5] = m_VertexList[0];
	}
	m_vbVertexBuffer->Unlock();
}

void Effect::Render(double _fDelta)
{
	if(!m_bPlayedOnce)
	{
		D3DXMATRIX rot;
		D3DXMatrixIdentity(&_world);

		if(m_bMultiTextured || m_nEffectType == 2 || m_nEffectType == 4)
		{
			D3DXMATRIX rot90;
			D3DXMatrixIdentity(&rot90);
			D3DXMatrixIdentity(&rot);

			D3DXMatrixRotationY(&rot90, D3DXToRadian(180.0f));
			D3DXMatrixRotationY(&rot,atan2f(forward.x,forward.z));
			rot = rot90 * rot;
		}
		D3DXMatrixTranslation(&_world, m_Postition.x, m_Postition.y, m_Postition.z);

		if(m_bMultiTextured || m_nEffectType == 2 || m_nEffectType == 4)
			_world = rot * _world;

		if(m_bBillboarded)
			BillBoard();

		RJ_RENDERER->getDirect3DDevice()->SetVertexDeclaration(_EffectVertex::VertexDecl);
		RJ_RENDERER->getDirect3DDevice()->SetStreamSource(0, m_vbVertexBuffer, 0, sizeof(EffectVertex));

		m_Shader->SetTechnique(m_Shader_Technique);
		m_Shader->SetMatrix(m_Shader_VeiwProjection, &(_world * RJ_RENDERER->getViewMatrix() * RJ_RENDERER->getProjectionMatrix()));
		m_Shader->SetTexture(m_Shader_TextureCoords, ASSETMANAGER->GetTexture(m_nTextureID));

		m_dCurrentTime += _fDelta;

		if(m_dCurrentTime >= m_fFrameDuration)
		{
			m_nCurrentFrame++;
			if(m_nCurrentFrame >= m_nFrameCount)
			{
				m_nCurrentFrame = 0;
				m_bPlayedOnce = true;
			}
			m_dCurrentTime = 0.0;
		}

		if(m_bMultiTextured)
		{
			m_Shader->SetTexture(m_Shader_AlphaTextureCoords, ASSETMANAGER->GetTexture(m_nAlphaTextureID));
		}
		else
		{
			eCellAlgorithm(m_nCurrentFrame);
			if(m_Shader_OffsetX)
				m_Shader->SetFloat(m_Shader_OffsetX, m_fOffsetX);
			if(m_Shader_OffsetY)
				m_Shader->SetFloat(m_Shader_OffsetY, m_fOffsetY);

			if(m_nEffectType == 3)
				m_Shader->SetFloat(m_Shader_OffsetY, time);
		}

		time += (float)_fDelta;
		if(m_Shader_Time)
			m_Shader->SetFloat(m_Shader_Time, time);
		m_Shader->CommitChanges();

		// Begin passes.
		UINT numPasses = 0;
		m_Shader->Begin(&numPasses, 0);
		for(UINT i = 0; i < numPasses; ++i)
		{
			m_Shader->BeginPass(i);
				RJ_RENDERER->getDirect3DDevice()->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 2);
			m_Shader->EndPass();
		}
		m_Shader->End();
	}
}

void Effect::SetPosition(D3DXVECTOR3 _position)
{
	m_Postition.x = _position.x;
	m_Postition.y = _position.y;
	m_Postition.z = _position.z;
	m_bPlayedOnce = false;
	m_dCurrentTime = 0.0;
	m_nCurrentFrame = 0;
}

void Effect::SetPositionWithoutUpdate(D3DXVECTOR3 _position)
{
	m_Postition.x = _position.x;
	m_Postition.y = _position.y;
	m_Postition.z = _position.z;
}

void Effect::eCellAlgorithm(int _cellID)
{
	if(m_nColumnCount == 0)
		m_nColumnCount = 1;

	m_fOffsetX = (_cellID % m_nColumnCount) * m_fEffectWidth;
	m_fOffsetY = (_cellID / m_nColumnCount) * m_fEffectHeight;
}

void Effect::Shutdown()
{
	if(m_vbVertexBuffer)
	{
		m_vbVertexBuffer->Release();
		m_vbVertexBuffer = 0;
	}

	if(m_Shader)
	{
		m_Shader->Release();
		m_Shader = NULL;
	}
}