#include "C3DFont.h"
#include "..\\Asset\\AssetManager.h"
#include <fstream>

C3DFont::C3DFont(char* _texture, char* _data)
{
	m_nFontFileID = -1;
	LoadTexture(_texture);
	LoadData(_data);
}

void C3DFont::LoadTexture(char* _fileName)
{
	m_nFontFileID = ASSETMANAGER->LoadTexture(_fileName, D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f), false);
}

void C3DFont::LoadData(char* _fileName)
{
	std::fstream FileIn(_fileName, std::ios_base::in | std::ios_base::binary);

	if(FileIn.is_open())
	{
		FileIn.read((char*)&m_nMapWidth, 4);
		FileIn.read((char*)&m_nMapHeight, 4);
		FileIn.read((char*)&m_nCellWidth, 4);
		FileIn.read((char*)&m_nCellHeight, 4);
		FileIn.read((char*)&m_cStartCharacter, 1);
		FileIn.read((char*)&m_cCharacterWidth, 256);

		m_nColumnCount = m_nMapWidth / m_nCellWidth;
		m_nRowCount = m_nMapHeight / m_nCellHeight;

		FileIn.close();
	}

	unsigned int shaderID = ASSETMANAGER->LoadShader("Texture.fx");
	m_Shader = ASSETMANAGER->GetShader(shaderID);

	// Obtain handles.
	m_Shader_Technique = m_Shader->GetTechniqueByName("ShaderTechnique");
	m_Shader_WorldVeiwProjection  = m_Shader->GetParameterByName(0, "m4x4_ViewProjection");
	m_Shader_TextureCoords = m_Shader->GetParameterByName(0, "s2D_TextureCoords_Tex");
}

void C3DFont::DrawLetter(const char _letter, D3DXMATRIX* _pos, 
		float _scaleX, float _scaleY, D3DXVECTOR4 _color)
{
	//Get the ID of the letter
	int id = (unsigned char)(_letter) - m_cStartCharacter;
	//Get Rect for the cell
	RECT rLetter = CellAlgorithm(id);

	//Create the quad for the letter
	FontVertex fontQuad[4]; 

	float scaleX = 1.0f * _scaleX;
	float scaleY = 1.0f * _scaleY;

	//Get the position ready
	fontQuad[0].position.x = 0.0f; fontQuad[0].position.y = 0.0f; fontQuad[0].position.z = 0.0f;
	fontQuad[1].position.x = scaleX; fontQuad[1].position.y = 0.0f; fontQuad[1].position.z = 0.0f;
	fontQuad[2].position.x = scaleX; fontQuad[2].position.y = -scaleY; fontQuad[2].position.z = 0.0f;
	fontQuad[3].position.x = 0.0f; fontQuad[3].position.y = -scaleY; fontQuad[3].position.z = 0.0f;

	//UV's....fun
	fontQuad[3].textureUV.x = (float)(rLetter.right / (float)m_nMapWidth); fontQuad[3].textureUV.y = (float)(rLetter.bottom / (float)m_nMapHeight);
	fontQuad[2].textureUV.x = (float)(rLetter.left / (float)m_nMapWidth); fontQuad[2].textureUV.y = (float)(rLetter.bottom / (float)m_nMapHeight);
	fontQuad[1].textureUV.x = (float)(rLetter.left / (float)m_nMapWidth); fontQuad[1].textureUV.y = (float)(rLetter.top / (float)m_nMapHeight);
	fontQuad[0].textureUV.x = (float)(rLetter.right / (float)m_nMapWidth); fontQuad[0].textureUV.y = (float)(rLetter.top / (float)m_nMapHeight);

	for(int i = 0; i < 4; i++)
	{
		fontQuad[i].position.w = 1.0f;
		fontQuad[i].color = _color;
	}

	//Render the quad
	IDirect3DVertexBuffer9*		m_vbVertexBuffer;
	RJ_RENDERER->getDirect3DDevice()->CreateVertexBuffer(6*sizeof(fontQuad[0]), 0,
		0, D3DPOOL_MANAGED, &m_vbVertexBuffer, 0);

	FontVertex* pBuffer;
	m_vbVertexBuffer->Lock(0,0, (void**)&pBuffer, 0);
	{
		pBuffer[0] = fontQuad[0];
		pBuffer[1] = fontQuad[1];
		pBuffer[2] = fontQuad[2];

		pBuffer[3] = fontQuad[2];
		pBuffer[4] = fontQuad[3];
		pBuffer[5] = fontQuad[0];
	}
	m_vbVertexBuffer->Unlock();

	RJ_RENDERER->getDirect3DDevice()->SetVertexDeclaration(_FontVertex::VertexDecl);
	RJ_RENDERER->getDirect3DDevice()->SetStreamSource(0, m_vbVertexBuffer, 0, sizeof(FontVertex));

	m_Shader->SetTechnique(m_Shader_Technique);
	m_Shader->SetMatrix(m_Shader_WorldVeiwProjection, &((*_pos) * RJ_RENDERER->getViewMatrix() * RJ_RENDERER->getProjectionMatrix()));
	m_Shader->SetTexture(m_Shader_TextureCoords, ASSETMANAGER->GetTexture(m_nFontFileID));
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

	if(m_vbVertexBuffer)
	{
		m_vbVertexBuffer->Release();
		m_vbVertexBuffer = 0;
	}
}

void C3DFont::DrawString(const char* _string, D3DXVECTOR3* _direction, D3DXMATRIX* _pos, 
		float _scaleX, float _scaleY, D3DXVECTOR4 _color)
{
	//Get the ID of the letter
	unsigned int size = strlen(_string);

	D3DXMATRIX newPos = (*_pos);

	for(unsigned int _i = 0; _i < size; _i++)
	{
		//int id = (unsigned char)(_string[_i]) - m_cStartCharacter;
		//Get Rect for the cell
		//RECT rLetter = CellAlgorithm(id);

		DrawLetter(_string[_i],&newPos,_scaleX,_scaleY,_color);
		int x = (m_cCharacterWidth[_string[_i]]);

		D3DXMATRIX newTranslation;
		D3DXMatrixTranslation(&newTranslation, _direction->x, _direction->y, _direction->z);
		newPos *= newTranslation;
	}
}

RECT C3DFont::CellAlgorithm(int _cellID)
{
	RECT rCell;

	rCell.left   = (_cellID % m_nColumnCount) * m_nCellWidth;
	rCell.top    = (_cellID / m_nColumnCount) * m_nCellHeight;
	rCell.right  = rCell.left + m_nCellWidth;
	rCell.bottom = rCell.top  + m_nCellHeight;

	return rCell;
}