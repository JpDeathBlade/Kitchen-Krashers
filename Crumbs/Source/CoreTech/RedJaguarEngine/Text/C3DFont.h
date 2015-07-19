#ifndef  REDJAGUAR_JS_C3DFONT_H_
#define  REDJAGUAR_JS_C3DFONT_H_

#include <windows.h>
#include <d3d9.h>
#include <vector>
#include "..\\Renderer\\RJ_Renderer.h"
#include "..\\Renderer\\RJ_Vertex.h"

struct FontVertex
{
	D3DXVECTOR4		position;
	D3DXVECTOR4		color;
	D3DXVECTOR2		textureUV;
};

class C3DFont
{
private:
	int		m_nFontFileID;
	int		m_nMapWidth;
	int		m_nMapHeight;
	int		m_nCellWidth;
	int		m_nCellHeight;
	int		m_nColumnCount;
	int		m_nRowCount;
	char	m_cStartCharacter;
	char	m_cCharacterWidth[256];

	ID3DXEffect*				m_Shader;
	D3DXHANDLE					m_Shader_Technique;
	D3DXHANDLE					m_Shader_WorldVeiwProjection;
	D3DXHANDLE					m_Shader_TextureCoords;

	C3DFont(const C3DFont &ref);
	C3DFont &operator= (const C3DFont &ref);

	RECT CellAlgorithm(int _cellID);
	void LoadTexture(char* _fileName);
	void LoadData(char* _fileName);

protected:
public:
	C3DFont(char* _texture, char* _data);
	~C3DFont(void) { }

	void DrawLetter(const char _letter, D3DXMATRIX* _pos, 
		float _scaleX = 1.0f, float _scaleY = 1.0f, D3DXVECTOR4 _color = D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f));
	void DrawString(const char* _string, D3DXVECTOR3* _direction, D3DXMATRIX* _pos, 
		float _scaleX = 1.0f, float _scaleY = 1.0f, D3DXVECTOR4 _color = D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f));


	const int	getMapWidth(void)			{ return m_nMapWidth;			}
	const int	getMapHeight(void)			{ return m_nMapHeight;			}
	const int	getCellWidth(void)			{ return m_nCellWidth;			}
	const int	getCellHeight(void)			{ return m_nCellHeight;			}
	const int	getColumnCount(void)		{ return m_nColumnCount;		}
	const int	getRowCount(void)			{ return m_nRowCount;			}
	const char	getStartCharacter(void)		{ return m_cStartCharacter;		}

	void  setMapWidth(const int _mapWidth)				{ m_nMapWidth = _mapWidth; }
	void  setMapHeight(const int _mapHeight)			{ m_nMapHeight = _mapHeight; }
	void  setCellWidth(const int _cellWidth)			{ m_nCellWidth = _cellWidth; }
	void  setCellHeight(const int _cellHeight)			{ m_nCellHeight = _cellHeight; }
	void  setColumnCount(const int _column)				{ m_nColumnCount = _column; }
	void  setRowCount(const int _row)					{ m_nRowCount = _row; }
	void  setStartCharacter(const char _startingChar)	{ m_cStartCharacter = _startingChar; }
};

#endif //end of REDJAGUAR_JS_C3DFONT_H_