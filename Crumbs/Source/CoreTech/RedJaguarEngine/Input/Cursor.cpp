///////////////////////////////////////////////////////////////
//// File Name	:	"Cursor.cpp"
////
//// Author		:	Kurt Young (KY)
////
//// Purpose		:	The Game mouse, active only inside of the game window. 
////					Bound to window edges.
///////////////////////////////////////////////////////////////	
//#include "Cursor.h"
//#include "CInputManager.h"
//#include "../Asset/AssetManager.h"
//#include "../../RJ_GameMain.h"
//
//Cursor::Cursor(void)
//{
//	m_nCursorImageID = -1;
//	m_pAM = CAssetManager::getInstanceRender();
//	IM = CInputManager::GetInstanceRender();
//
//	m_fposX = 512.0f;
//	m_fposY = 384.0f;
//	m_bDrawCursors = true;
//}
//Cursor::~Cursor(void)
//{
//
//}
//bool Cursor::Init()
//{
//	m_nCursorImageID = m_pAM->LoadTexture("RJ_Game_Pointer.dds");
//	return true;
//}
//void Cursor::Render()
//{
//	Update(RJ_GameMain::GetInstance()->GetRenderTime()->GetElapsedTime());
//
//	if(m_nCursorImageID != -1)
//		m_pAM->DrawFlatSprite(m_nCursorImageID, m_fposX, m_fposY,0.4f,0.4f,0,1.0f,1.0f,0.0f,D3DCOLOR_ARGB(255,255,255,255));
//
//}
//void Cursor::Update(double dElapsedTime)
//{
//	m_fposX = IM->getMousePosX();
//	m_fposY = IM->getMousePosY();
//
//	if(m_fposX + 12.0f > 1024.0f)
//	{
//		m_fposX = 1024.0f - 13.0f;
//		IM->setMousePosX(m_fposX);
//	}
//	if(m_fposX < 0.0f)
//	{
//		m_fposX = 1.0f;
//		IM->setMousePosX(m_fposX);
//	}
//	if(m_fposY + 12.0f > 768.0f)
//	{
//		m_fposY = 768.0f - 13.0f;
//		IM->setMousePosY(m_fposY);
//	}
//	if(m_fposY < 0.0f)
//	{
//		m_fposY = 1.0f;
//		IM->setMousePosY(m_fposY);
//	}
//}
//Cursor* Cursor::GetInstanceRender()
//{
//	static Cursor instance;
//	return &instance;
//}