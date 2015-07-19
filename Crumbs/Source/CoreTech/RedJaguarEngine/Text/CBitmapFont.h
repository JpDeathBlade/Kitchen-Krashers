#ifndef  REDJAGUAR_JS_CBITMAPFONT_H_
#define  REDJAGUAR_JS_CBITMAPFONT_H_

#include <windows.h>
#include <d3d9.h>

class CBitmapFont
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

	CBitmapFont(const CBitmapFont &ref);
	CBitmapFont &operator= (const CBitmapFont &ref);

	RECT CellAlgorithm(int _cellID);
	void LoadTexture(char* _fileName);
	void LoadData(char* _fileName);

protected:
public:
	CBitmapFont(char* _texture, char* _data);
	~CBitmapFont(void) { }

	void DrawLetter(const char _letter, float _posX, float _posY, float _scaleX = 1.0f, float _scaleY = 1.0f, 
		float _rotation = 0.0f, D3DCOLOR _color = D3DCOLOR_XRGB(255,255,255));
	void DrawString(const char* _string, float _posX, float _posY, float _scaleX = 1.0f, float _scaleY = 1.0f, 
		float _rotation = 0.0f, D3DCOLOR _color = D3DCOLOR_XRGB(255,255,255));


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

#endif //end of REDJAGUAR_JS_CBITMAPFONT_H_