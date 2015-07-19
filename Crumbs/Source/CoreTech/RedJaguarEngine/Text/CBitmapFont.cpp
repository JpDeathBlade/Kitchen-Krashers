#include "CBitmapFont.h"
#include "..\\Asset\\AssetManager.h"
#include <fstream>

CBitmapFont::CBitmapFont(char* _texture, char* _data)
{
	m_nFontFileID = -1;
	LoadTexture(_texture);
	LoadData(_data);
}

void CBitmapFont::LoadTexture(char* _fileName)
{
	m_nFontFileID = ASSETMANAGER->LoadTexture(_fileName, D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f), false);
}

void CBitmapFont::LoadData(char* _fileName)
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
}

void CBitmapFont::DrawLetter(const char _letter, float _posX, float _posY, 
		float _scaleX, float _scaleY, float _rotation, D3DCOLOR _color)
{
	//Get the ID of the letter
	int id = (unsigned char)(_letter) - m_cStartCharacter;
	//Get Rect for the cell
	RECT rLetter = CellAlgorithm(id);

	ASSETMANAGER->DrawFlatSprite(m_nFontFileID,_posX,_posY,_scaleX,_scaleY,&rLetter,0,0,_rotation,_color);
}

void CBitmapFont::DrawString(const char* _string, float _posX, float _posY, 
		float _scaleX, float _scaleY, float _rotation, D3DCOLOR _color)
{
	//Get the ID of the letter
	unsigned int size = strlen(_string);
	float x = _posX;

	for(unsigned int _i = 0; _i < size; _i++)
	{
		int id = (unsigned char)(_string[_i]) - m_cStartCharacter;
		//Get Rect for the cell
		RECT rLetter = CellAlgorithm(id);

		ASSETMANAGER->DrawFlatSprite(m_nFontFileID,x,_posY,_scaleX,_scaleY,&rLetter,0,0,_rotation,_color);
		x += (m_cCharacterWidth[_string[_i]] * _scaleX);
	}
}

RECT CBitmapFont::CellAlgorithm(int _cellID)
{
	RECT rCell;

	rCell.left   = (_cellID % m_nColumnCount) * m_nCellWidth;
	rCell.top    = (_cellID / m_nColumnCount) * m_nCellHeight;
	rCell.right  = rCell.left + m_nCellWidth;
	rCell.bottom = rCell.top  + m_nCellHeight;

	return rCell;
}