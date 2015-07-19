///////////////////////////////////////////////////////////////
//// File Name	:	"Cursor.h"
////
//// Author		:	Kurt Young (KY)
////
//// Purpose		:	The Game mouse, active only inside of the game window. 
////					Bound to window edges.
///////////////////////////////////////////////////////////////					
//
//#ifndef RJ_CURSOR_H_
//#define RJ_CURSOR_H_
//
//class CAssetManager;
//class CInputManager;
//
//class Cursor
//{
//	float		m_fposX;
//	float		m_fposY;
//	int			m_nCursorImageID;
//	bool		m_bDrawCursors;
//	CAssetManager* m_pAM;
//
//	CInputManager* IM;
//	Cursor& operator=(const Cursor&);
//	Cursor(const Cursor&);
//	Cursor(void);
//	~Cursor(void);
//public:
//	bool	Init();
//	static Cursor* GetInstanceRender();
//	float	 GetX(void) {return m_fposX;}
//	void SetX(float fposX)	{m_fposX = fposX;}
//	float	 GetY(void) {return m_fposY;}
//	void SetY(float fposY)	{m_fposY = fposY;}
//	int	 GetCImage(void) {return m_nCursorImageID;}
//	void SetCImage(int nImageID)	{m_nCursorImageID = nImageID;}
//	void Render();
//	void Update(double dElapsedTime);
//};
//#endif