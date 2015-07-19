// 0===========================================================================0
// | ~ Includes ~                                                              |
// 0===========================================================================0
#include "RJ_Renderer.h"
#include "..\\..\\Utilities\\Debugger\\RJ_Debugger.h"
#include "..\\..\\..\\RJ_Config.h"
#include "RJ_Vertex.h"
#include "..\\Event\\EventManager.h"
#include "..\\Asset\\AssetManager.h"
#include "..\\..\\..\\Gameplay\\Objects\\CStone.h"

// 0===========================================================================0
// | ~ DEFINES ~                                                               |
// 0===========================================================================0
#define RJ_SAFE_RELEASE(_p)			if(_p) { _p->Release(); _p = NULL;	}

// 0===========================================================================0
// | ~ STATICS ~                                                               |
// 0===========================================================================0
RJ_Renderer RJ_Renderer::m_Instance;

// 0===========================================================================0
// | ~ EXTERNS ~                                                               |
// 0===========================================================================0
#ifdef RJ_DLL
extern "C" __declspec(dllexport) bool dll_RJ_Renderer_Initialize(RJ_Renderer::RenderParameters _params);
extern "C" __declspec(dllexport) bool dll_RJ_Renderer_Shutdown(void);
extern "C" __declspec(dllexport) bool dll_RJ_Renderer_Render(float _fDelta);
extern "C" __declspec(dllexport) void dll_RJ_Renderer_Clear(unsigned char _ucRed, unsigned char _ucBlue, unsigned char _ucGreen);
extern "C" __declspec(dllexport) void dll_RJ_Renderer_FlipBuffers(void);
extern "C" __declspec(dllexport) bool dll_RJ_Renderer_BeginRender(void);
extern "C" __declspec(dllexport) bool dll_RJ_Renderer_BeginSprite(void);
extern "C" __declspec(dllexport) bool dll_RJ_Renderer_BeginLine(void);
extern "C" __declspec(dllexport) bool dll_RJ_Renderer_EndRender(void);
extern "C" __declspec(dllexport) bool dll_RJ_Renderer_EndSprite(void);
extern "C" __declspec(dllexport) bool dll_RJ_Renderer_EndLine(void);
extern "C" __declspec(dllexport) bool dll_RJ_Renderer_ChangeDisplay(unsigned int _width, unsigned int _height, bool _fullscreen);
extern "C" __declspec(dllexport) bool dll_RJ_Renderer_EnableFullScreen(bool _fullscreen);
extern "C" __declspec(dllexport) bool dll_RJ_Renderer_ToggleFullScreen(void);
extern "C" __declspec(dllexport) bool dll_RJ_Renderer_GoFullScreen(void);
extern "C" __declspec(dllexport) bool dll_RJ_Renderer_GoWindowed(void);
extern "C" __declspec(dllexport) bool dll_RJ_Renderer_CheckDeviceCaps(void);
extern "C" __declspec(dllexport) bool dll_RJ_Renderer_isLostDevice(void);
extern "C" __declspec(dllexport) void dll_RJ_Renderer_onLostDevice(void);
extern "C" __declspec(dllexport) void dll_RJ_Renderer_Reset(void);
extern "C" __declspec(dllexport) void dll_RJ_Renderer_onResetDevice(void);
extern "C" __declspec(dllexport) void dll_RJ_Renderer_RenderText(const char* _text, int _x, int _y, unsigned char _ucRed, 
																 unsigned char _ucBlue, unsigned char _ucGreen);
#endif

// for Render()
class CSolidSphere;
class CSolidOBB;
class CHazardSphere;

RJ_Renderer::RJ_Renderer(void)
{
	m_lpD3DObject	= NULL;
	m_lpD3DDevice	= NULL;
	m_lpSprite		= NULL;
	m_lpFont		= NULL;
	m_lpLine		= NULL;
	memset(&m_PresentParams, 0, sizeof(m_PresentParams));

	m_vbAxis		= NULL;
	m_vbGround		= NULL;
	m_ObjEvent		= NULL;
}

bool RJ_Renderer::Initialize(RJ_Renderer::RenderParameters _params)
{
	// Make sure the hWnd is valid.
	if (!_params.t_Window)
	{
		return false;
		RJ_DEBUGGER->Error(RJ_Debugger::RJ_FATAL,"hWnd is not valid!");
	}
	m_RenderParams = _params;

	//Let the Event Manager know what messages I am listening for
	CEventManager::GetInstanceThreaded()->RegisterListenerRender(ET_CAMERA,this);
	CEventManager::GetInstanceThreaded()->RegisterListenerRender(ET_OBJECTINFRUSTUMLIST,this);
	CEventManager::GetInstanceThreaded()->RegisterListenerRender(ET_CLEARLIST, this);
	CEventManager::GetInstanceThreaded()->RegisterListenerRender(ET_ADJUSTGAMMA, this);

	return InternalInitialize();
}

bool RJ_Renderer::Shutdown(void)
{
	return InternalShutdown();
}

void RJ_Renderer::HandleEvent(CEvent* pEvent)
{
	if(pEvent->GetEventID() == ET_CAMERA)
	{
		if(pEvent->GetData() != NULL)
		{
			if(pEvent->GetData()->GetMsgID() == EM_CAMERA)
			{
				tCameraUpdate* tCam;
				tCam = (tCameraUpdate*)pEvent->GetData();
				m_mProjMat = tCam->m_mProjectionMatrix;
				m_mViewMat = tCam->m_mViewMatrix;
				m_mCameraMat = tCam->m_mCamera;
				m_fLaunchPower = tCam->m_fPower;
				m_fLaunchAngle = tCam->m_fAngle;
				m_fStoneSpeed = tCam->m_fSpeed;
				m_fStoneAngle = tCam->m_fStoneAngle;
				m_vStoneForward = tCam->m_vStoneForward;
				m_nScore = tCam->m_nScore;
			}
		}
		pEvent->m_nRefs--;
	}
	if(pEvent->GetEventID() == ET_OBJECTINFRUSTUMLIST)
	{
		if(pEvent->GetData() != NULL)
		{
			if(pEvent->GetData()->GetMsgID() == EM_OBJECTINFRUSTUMLIST)
			{
				tObjectInFrustumList*	m_tObj;
				m_tObj = (tObjectInFrustumList*)pEvent->GetData();
				Opaque = m_tObj->m_dqObjectListOpaque;
				Transparent = m_tObj->m_dqObjectListTransparent;
			}
		}
		pEvent->m_nRefs--;
	}
	if(pEvent->GetEventID() == ET_CLEARLIST)
	{
		m_ObjEvent = pEvent;
		if(pEvent->GetData() != NULL)
		{
			if(pEvent->GetData()->GetMsgID() == EM_CLEARLIST)
			{
				while(!Opaque.empty())
					Opaque.pop_back();

				while(!Transparent.empty())
					Transparent.pop_back();

				Opaque.clear();
				Transparent.clear();
				SendListClearedEvent();
			}
		}
		pEvent->m_nRefs--;		
	}
	if(pEvent->GetEventID() == ET_ADJUSTGAMMA)
	{
		m_ObjEvent = pEvent;
		if(pEvent->GetData() != NULL)
		{
			if(pEvent->GetData()->GetMsgID() == EM_ADJUSTGAMMA)
			{
				tGammaData* tGD = (tGammaData*)pEvent->GetData();
				m_fGammaValue = tGD->m_fGamma; 
			}
		}
		pEvent->m_nRefs--;		
	}
}

// Sends a ListCleared event to the Update side, letting the Update thread
//  know the list is ready for new data.
void RJ_Renderer::SendListClearedEvent(void)
{
	volatile tListCleared* tObj = new tListCleared(EM_LISTCLEARED);
	CEventManager::GetInstanceThreaded()->SendEventRenderCrossToUpdate(ET_LISTCLEARED, (tBaseEventData*)tObj);
	tObj = NULL;
}

// Gets the device ready for rendering.
bool RJ_Renderer::BeginRender(void)
{
	Clear();
	BeginScene();
	BeginSprite();
	BeginLine();

	return true;
}

// Renders all of the games geometry and 2D assets to the screen.
bool RJ_Renderer::Render(double _fDelta)
{
	// Loops through the opaque list and renders them.
	unsigned int _Osize = Opaque.size();
	for (unsigned int opaque = 0; opaque < _Osize; opaque++)
	{
		if(Opaque[opaque]->GetIsActive())
			Opaque[opaque]->Render(_fDelta);
	}

	// Loops through the transparent list and renders them (2 pass shader).
	unsigned int _Tsize = Transparent.size();
	for (unsigned int transparent = 0; transparent < _Tsize; transparent++)
	{
		if(Transparent[transparent]->GetIsActive())
			Transparent[transparent]->Render(_fDelta);
	}

	// Tells the AssetManager to render the crowd.
	CAssetManager::getInstanceRender()->RenderSpectators(_fDelta);

	return true;
}

// Tells the device that we are done rendering.
bool RJ_Renderer::EndRender(void)
{
	EndLine();
	EndSprite();
	EndScene();
	FlipBuffers();

	return true;
}

// Clears the device to the specified color.
void RJ_Renderer::Clear(unsigned char _ucRed, unsigned char _ucBlue, unsigned char _ucGreen)
{
	m_lpD3DDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 
		D3DCOLOR_XRGB(m_RenderParams.t_ucClearRed, 
		m_RenderParams.t_ucClearGreen, m_RenderParams.t_ucClearBlue), 1.0f, 0);
}

// Begins the current Scene.
bool RJ_Renderer::BeginScene(void)
{
	m_lpD3DDevice->BeginScene();
	return true;
}

// Begins sprite drawing.
bool RJ_Renderer::BeginSprite(void)
{
	m_lpSprite->Begin(D3DXSPRITE_ALPHABLEND);
	return true;
}

// Begins line drawing.
bool RJ_Renderer::BeginLine(void)
{
	m_lpLine->Begin();
	return true;
}

// Ends the current scene.
bool RJ_Renderer::EndScene(void)
{
	m_lpD3DDevice->EndScene();
	return true;
}

// Ends sprite drawing.
bool RJ_Renderer::EndSprite(void)
{
	m_lpSprite->End();
	return true;
}

// Ends line drawing.
bool RJ_Renderer::EndLine(void)
{
	m_lpLine->End();
	return true;
}

// Toggles between the front and back buffer.
void RJ_Renderer::FlipBuffers(void)
{
	m_lpD3DDevice->Present(NULL,NULL,NULL,NULL);
}

// Builds the new PresentParameters based on the current state of the game.
bool RJ_Renderer::BuildPresentParameters(void)
{
	//ZeroMemory( &m_PresentParams, sizeof(m_PresentParams) );

	m_PresentParams.BackBufferWidth            = m_RenderParams.t_uiClientWidth;
    m_PresentParams.BackBufferHeight           = m_RenderParams.t_uiClientHeight;
    m_PresentParams.BackBufferFormat           = (m_RenderParams.t_bFullScreen) ? D3DFMT_R5G6B5 : D3DFMT_UNKNOWN;
    m_PresentParams.BackBufferCount            = 1;
    m_PresentParams.MultiSampleType            = D3DMULTISAMPLE_NONE;
    m_PresentParams.MultiSampleQuality         = 0;
    m_PresentParams.SwapEffect                 = D3DSWAPEFFECT_DISCARD; 
    m_PresentParams.hDeviceWindow              = m_RenderParams.t_Window;
    m_PresentParams.Windowed                   = !m_RenderParams.t_bFullScreen;
    m_PresentParams.EnableAutoDepthStencil     = true; 
	m_PresentParams.AutoDepthStencilFormat	   = D3DFMT_D16;
	m_PresentParams.Flags					   = D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;
	m_PresentParams.FullScreen_RefreshRateInHz = (m_RenderParams.t_bFullScreen) ? D3DPRESENT_RATE_DEFAULT : 0;
    m_PresentParams.PresentationInterval       = D3DPRESENT_INTERVAL_IMMEDIATE;

	return true;
}

// Changes the display between Fullscreen Mode and Windowed Mode.
bool RJ_Renderer::ChangeDisplay(unsigned int _width, unsigned int _height, bool _fullscreen)
{
	m_RenderParams.t_uiClientWidth = _width;
	m_RenderParams.t_uiClientHeight = _height;
	m_RenderParams.t_bFullScreen = _fullscreen;
	
	BuildPresentParameters();

	// Reset the device with the changes.
	onLostDevice();
	Reset();
	onResetDevice();

	DWORD dwWindowStyleFlags = 0;

	// Switch to fullscreen mode.
	if(_fullscreen)
	{
		dwWindowStyleFlags |= WS_POPUP;
	}
	else
	{
		dwWindowStyleFlags |= WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX;
	}

	SetWindowLongPtr(m_RenderParams.t_Window, GWL_STYLE, dwWindowStyleFlags);

	if(!_fullscreen)
	{
		RECT rWindow = {0, 0, m_RenderParams.t_uiClientWidth, m_RenderParams.t_uiClientHeight};

		AdjustWindowRectEx(&rWindow, dwWindowStyleFlags, false, WS_EX_APPWINDOW);	

		int windowWidth		= rWindow.right - rWindow.left;
		int windowHeight	= rWindow.bottom - rWindow.top;

		SetWindowPos(m_RenderParams.t_Window, HWND_TOP,
			(GetSystemMetrics(SM_CXSCREEN)>>1) - (windowWidth>>1),
			(GetSystemMetrics(SM_CYSCREEN)>>1) - (windowHeight>>1),
			windowWidth, windowHeight, SWP_SHOWWINDOW);
	}
	else
	{
		SetWindowPos(m_RenderParams.t_Window, HWND_TOP, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOREPOSITION | SWP_NOSIZE | SWP_SHOWWINDOW);
	}

	return true;
}

// Changes the display based on the current RenderParameters.
bool RJ_Renderer::ChangeDisplay(RenderParameters _params)
{
	// Make sure the hWnd is valid.
	if (!_params.t_Window)
	{
		return false;
		RJ_DEBUGGER->Error(RJ_Debugger::RJ_FATAL,"hWnd is not valid!");
	}
	m_RenderParams = _params;

	return ChangeDisplay(m_RenderParams.t_uiClientWidth, 
		m_RenderParams.t_uiClientHeight, m_RenderParams.t_bFullScreen);
}

// Enables Fullscreen Mode based on bool input.
bool RJ_Renderer::EnableFullScreen(bool _fullscreen)
{
	m_RenderParams.t_bFullScreen = _fullscreen;
	return ChangeDisplay(m_RenderParams.t_uiClientWidth,m_RenderParams.t_uiClientHeight,_fullscreen);
}

// Toggles between Fullscreen Mode and Windowed Mode.
bool RJ_Renderer::ToggleFullScreen(void)
{
	m_RenderParams.t_bFullScreen = !m_RenderParams.t_bFullScreen;
	return ChangeDisplay(m_RenderParams.t_uiClientWidth,m_RenderParams.t_uiClientHeight,m_RenderParams.t_bFullScreen);
}

// Sets the device to Fullscreen Mode.
bool RJ_Renderer::GoFullScreen(void)
{
	if(m_RenderParams.t_bFullScreen)
		return true;

	m_RenderParams.t_bFullScreen = true;
	return ChangeDisplay(m_RenderParams.t_uiClientWidth,m_RenderParams.t_uiClientHeight,true);
}

// Sets the device to Windowed Mode.
bool RJ_Renderer::GoWindowed(void)
{
	if(!m_RenderParams.t_bFullScreen)
		return true;

	m_RenderParams.t_bFullScreen = false;
	return ChangeDisplay(m_RenderParams.t_uiClientWidth,m_RenderParams.t_uiClientHeight,false);
}

// Checks for the Device Caps.
bool RJ_Renderer::CheckDeviceCaps(void)
{
	return true;
}

// Check to see if the 
bool RJ_Renderer::isLostDevice(void)
{
	HRESULT hr = m_lpD3DDevice->TestCooperativeLevel();

	if(hr != D3D_OK)
	{
		if(hr == D3DERR_DEVICELOST)
		{
			Sleep(20);
			RJ_DEBUGGER->Error(RJ_Debugger::RJ_WARN,"D3DDevice is lost.");
			return true;
		}
		else if(hr == D3DERR_DRIVERINTERNALERROR)
		{
			//#TODO : DRIVER FAIL
			RJ_DEBUGGER->Error(RJ_Debugger::RJ_FATAL,"Internal Driver Error: D3DDevice is lost!");
			PostQuitMessage(0);
			return true;
		}
		else if(hr == D3DERR_DEVICENOTRESET)
		{
			onLostDevice();
			Reset();
			onResetDevice();
			return false;
		}
		else
			return false;
	}
	return false;
}

// What to do if the device is lost
void RJ_Renderer::onLostDevice(void)
{
	m_lpLine->OnLostDevice();
	m_lpSprite->OnLostDevice();
	m_lpFont->OnLostDevice();

	ASSETMANAGER->onLostDevice();
}

// Resets the device
void RJ_Renderer::Reset(void)
{
	HRESULT hr = m_lpD3DDevice->Reset(&m_PresentParams);

	if(hr == D3D_OK)
	{
		RJ_DEBUGGER->Error(RJ_Debugger::RJ_DEBUG,"Device reset.");
	}
	else
		RJ_DEBUGGER->Error(RJ_Debugger::RJ_FATAL,"Device NOT reset!");
}

// Reset the device based on the parameters passed in
void RJ_Renderer::Reset(D3DPRESENT_PARAMETERS _params)
{
	m_lpD3DDevice->Reset(&_params);
	m_PresentParams = _params;
}

// Renders a rectangle.
void RJ_Renderer::RenderRect(RECT _rect, unsigned char _ucRed, unsigned char _ucBlue, unsigned char _ucGreen)
{
	D3DRECT d3dRect;
	d3dRect.x1 = _rect.left;
	d3dRect.y1 = _rect.top;
	d3dRect.x2 = _rect.right;
	d3dRect.y2 = _rect.bottom;

	m_lpD3DDevice->Clear(1, &d3dRect, D3DCLEAR_TARGET, D3DCOLOR_XRGB(_ucRed, _ucGreen, _ucBlue), 1.0f, 0);
}

// Renders a line.
void RJ_Renderer::RenderLine(D3DXVECTOR2 _start, D3DXVECTOR2 _end, unsigned char _ucRed,
							 unsigned char _ucBlue, unsigned char _ucGreen, float _thickness)
{
	D3DXVECTOR2 Points[2];
	Points[0] = _start;
	Points[1] = _end;

	m_lpSprite->Flush();

	m_lpLine->SetWidth(abs(_thickness));
	m_lpLine->Begin();
	D3DCOLOR _color = D3DCOLOR_XRGB(_ucRed, _ucGreen, _ucBlue);
	m_lpLine->Draw(Points, 2, _color);
	m_lpLine->End();
}

// Renders a line.
void RJ_Renderer::RenderLine(D3DXVECTOR2 _start, D3DXVECTOR2 _end, D3DCOLOR _color, float _thickness)
{
	D3DXVECTOR2 Points[2];
	Points[0] = _start;
	Points[1] = _end;

	m_lpSprite->Flush();

	m_lpLine->SetWidth(abs(_thickness));
	m_lpLine->Begin();
	m_lpLine->Draw(Points, 2, _color);
	m_lpLine->End();
}

// Renders text.
void RJ_Renderer::RenderText(const char* _text, int _x, int _y, unsigned char _ucRed, 
							 unsigned char _ucBlue, unsigned char _ucGreen)
{
	RECT fontRec;
	fontRec.bottom = 768; fontRec.left = 0+_x; fontRec.right = 1024; fontRec.top = 0+_y;
	m_lpFont->DrawText(0, _text, -1, &fontRec, 0, D3DCOLOR_XRGB(_ucRed, _ucBlue, _ucGreen));
}

// What to do when the device has been reset.
void RJ_Renderer::onResetDevice(void)
{
	m_lpSprite->OnResetDevice();
	m_lpLine->OnResetDevice();
	m_lpFont->OnResetDevice();

	ASSETMANAGER->onResetDevice();
}

// Draws a IMesh.
void RJ_Renderer::DrawPrimitive(ID3DXMesh* _mesh, D3DXMATRIX* _position, D3DXVECTOR4* _color)
{
	if(_mesh)
	{
		// Begin passes.
		UINT numPasses = 0;
		m_Shader->Begin(&numPasses, 0);
		for(UINT i = 0; i < numPasses; ++i)
		{
			m_Shader->BeginPass(i);
			m_Shader->SetMatrix(m_Shader_WorldVeiwProjection, &((*_position) * m_mViewMat * m_mProjMat));
			m_Shader->SetVector(m_Shader_Color, _color);
			m_Shader->CommitChanges();
			_mesh->DrawSubset(0);
			m_Shader->EndPass();
		}
		m_Shader->End();
	}
}

// Draws a Cylinder.
void RJ_Renderer::DrawCylinder(D3DXMATRIX* _position, D3DXVECTOR4* _color)
{
	DrawPrimitive(m_Cylinder, _position, _color);
}

// Draws a Sphere.
void RJ_Renderer::DrawSphere(D3DXMATRIX* _position, D3DXVECTOR4* _color, float _scaleX, float _scaleY, float _scaleZ)
{
	D3DXMATRIX newPos, newScale;
	D3DXMatrixIdentity(&newPos);
	D3DXMatrixIdentity(&newScale);
	D3DXMatrixScaling(&newScale, _scaleX, _scaleY, _scaleZ);
	D3DXMatrixTranslation(&newPos, _position->_41,  _position->_42,  _position->_43);

	DrawPrimitive(m_Sphere, &(newScale * newPos), _color);
}

// Draws a box.
void RJ_Renderer::DrawBox(D3DXMATRIX* _position, D3DXVECTOR4* _color)
{
	DrawPrimitive(m_Box, _position, _color);
}

// Draws a OOB.
void RJ_Renderer::DrawOOB(ColorVertex* _verts, D3DXMATRIX* _position)
{
	IDirect3DVertexBuffer9* _oob;

	m_lpD3DDevice->CreateVertexBuffer(36*sizeof(_verts[0]), 0,
		0, D3DPOOL_MANAGED, &_oob, 0);

	ColorVertex* pBuffer;
	_oob->Lock(0,0, (void**)&pBuffer, 0);
		pBuffer[0] = _verts[0];
		pBuffer[1] = _verts[1];
		pBuffer[2] = _verts[2];

		pBuffer[3] = _verts[2];
		pBuffer[4] = _verts[3];
		pBuffer[5] = _verts[0];

		pBuffer[6] = _verts[4];
		pBuffer[7] = _verts[5];
		pBuffer[8] = _verts[6];

		pBuffer[9] = _verts[6];
		pBuffer[10] = _verts[7];
		pBuffer[11] = _verts[4];

		pBuffer[12] = _verts[0];
		pBuffer[13] = _verts[3];
		pBuffer[14] = _verts[4];

		pBuffer[15] = _verts[7];
		pBuffer[16] = _verts[4];
		pBuffer[17] = _verts[3];

		pBuffer[18] = _verts[1];
		pBuffer[19] = _verts[5];
		pBuffer[20] = _verts[2];

		pBuffer[21] = _verts[5];
		pBuffer[22] = _verts[6];
		pBuffer[23] = _verts[2];

		pBuffer[24] = _verts[3];
		pBuffer[25] = _verts[2];
		pBuffer[26] = _verts[6];

		pBuffer[27] = _verts[3];
		pBuffer[28] = _verts[6];
		pBuffer[29] = _verts[7];

		pBuffer[30] = _verts[1];
		pBuffer[31] = _verts[4];
		pBuffer[32] = _verts[0];

		pBuffer[33] = _verts[1];
		pBuffer[34] = _verts[5];
		pBuffer[35] = _verts[4];
	_oob->Unlock();

	m_lpD3DDevice->SetTransform(D3DTS_WORLD, _position);
	m_lpD3DDevice->SetVertexDeclaration(_ColorVertex::VertexDecl);
	m_lpD3DDevice->SetStreamSource(0, _oob, 0, sizeof(_verts[0]));
	D3DXVECTOR4 color(1.0f, 1.0f, 1.0f, 1.0f);

	// Begin passes.
		UINT numPasses = 0;
		m_Shader->Begin(&numPasses, 0);
		for(UINT i = 0; i < numPasses; ++i)
		{
			m_Shader->BeginPass(i);
				m_Shader->SetMatrix(m_Shader_WorldVeiwProjection, &((*_position) * m_mViewMat * m_mProjMat));
				m_Shader->SetVector(m_Shader_Color, &color);
				m_Shader->CommitChanges();
			m_lpD3DDevice->DrawPrimitive(D3DPT_TRIANGLELIST,0,12);
			m_Shader->EndPass();
		}
		m_Shader->End();

	_oob->Release();
}

// Draws the Axis lines.
void RJ_Renderer::DrawAxisLines()
{
	D3DXMATRIX identity; 
	D3DXMatrixIdentity( &identity ); 
	m_lpD3DDevice->SetTransform( D3DTS_WORLD, &identity ); 
	m_lpD3DDevice->SetTransform( D3DTS_VIEW, &m_mViewMat );
	m_lpD3DDevice->SetTransform( D3DTS_PROJECTION, &m_mProjMat );

	m_lpD3DDevice->SetRenderState( D3DRS_LIGHTING, FALSE );

	m_lpD3DDevice->SetVertexDeclaration(_ColorVertex::VertexDecl);
	m_lpD3DDevice->SetStreamSource(0, m_vbAxis, 0, sizeof(m_Axis[0]));
	m_lpD3DDevice->DrawPrimitive(D3DPT_LINELIST,0,3);
}

// Renders a line in 3D.
void RJ_Renderer::Render3DLine(D3DXVECTOR3 _start, D3DXVECTOR3 _end, D3DXVECTOR4 _color)
{
	IDirect3DVertexBuffer9* _oob;
	ColorVertex _verts[2];

	_verts[0].position.x = _start.x; _verts[0].position.y = _start.y; _verts[0].position.z = _start.z; _verts[0].position.w = 1.0f;
	_verts[0].color = 0;
	_verts[1].position.x = _end.x; _verts[1].position.y = _end.y; _verts[1].position.z = _end.z; _verts[1].position.w = 1.0f;
	_verts[1].color = 0;

	m_lpD3DDevice->CreateVertexBuffer(2*sizeof(_verts[0]), 0,
		0, D3DPOOL_MANAGED, &_oob, 0);

	ColorVertex* pBuffer;
	_oob->Lock(0,0, (void**)&pBuffer, 0);
		pBuffer[0] = _verts[0];
		pBuffer[1] = _verts[1];
	_oob->Unlock();

	D3DXMATRIX _world;
	D3DXMatrixIdentity(&_world);

	m_lpD3DDevice->SetTransform(D3DTS_WORLD, &_world);
	m_lpD3DDevice->SetVertexDeclaration(_ColorVertex::VertexDecl);
	m_lpD3DDevice->SetStreamSource(0, _oob, 0, sizeof(_verts[0]));

	// Begin passes.
		UINT numPasses = 0;
		m_Shader->Begin(&numPasses, 0);
		for(UINT i = 0; i < numPasses; ++i)
		{
			m_Shader->BeginPass(i);
				m_Shader->SetMatrix(m_Shader_WorldVeiwProjection, &(_world * m_mViewMat * m_mProjMat));
				m_Shader->SetVector(m_Shader_Color, &_color);
				m_Shader->CommitChanges();
				m_lpD3DDevice->DrawPrimitive(D3DPT_LINESTRIP,0,1);
			m_Shader->EndPass();
		}
		m_Shader->End();

	_oob->Release();
}

// Draws a grid.
void RJ_Renderer::DrawGrid()
{
	D3DXMATRIX identity; 
	D3DXMatrixIdentity( &identity ); 
	m_lpD3DDevice->SetTransform( D3DTS_WORLD, &identity ); 
	m_lpD3DDevice->SetTransform( D3DTS_VIEW, &m_mViewMat );
	m_lpD3DDevice->SetTransform( D3DTS_PROJECTION, &m_mProjMat );

	m_lpD3DDevice->SetRenderState( D3DRS_LIGHTING, FALSE );

	m_lpD3DDevice->SetVertexDeclaration(_ColorVertex::VertexDecl);
	m_lpD3DDevice->SetStreamSource(0, m_vbGround, 0, sizeof(m_Ground[0]));
	m_lpD3DDevice->DrawPrimitive(D3DPT_LINELIST,0,42);
}

///////////////

bool RJ_Renderer::InternalInitialize(void)
{
	// Create the Direct3D9 Object.
	m_lpD3DObject = Direct3DCreate9(D3D_SDK_VERSION);

	// Make sure the object is valid.
	if (!m_lpD3DObject)
	{
		RJ_DEBUGGER->Error(RJ_Debugger::RJ_FATAL,"Failed to create D3DObject!");
		MessageBox(NULL, TEXT("Direct3D: Failed to create D3DObject!"),
			NULL, MB_OK | MB_ICONSTOP);
		return false;
	}

	// Setup the parameters for using Direct3D.
	m_PresentParams.BackBufferWidth				= m_RenderParams.t_uiClientWidth;
	m_PresentParams.BackBufferHeight			= m_RenderParams.t_uiClientHeight;
	m_PresentParams.BackBufferFormat			= (m_RenderParams.t_bFullScreen) ? D3DFMT_R5G6B5 : D3DFMT_UNKNOWN;
	m_PresentParams.BackBufferCount				= 1;
	m_PresentParams.MultiSampleType				= D3DMULTISAMPLE_NONE;
	m_PresentParams.MultiSampleQuality			= 0;
	m_PresentParams.SwapEffect					= D3DSWAPEFFECT_DISCARD;
	m_PresentParams.hDeviceWindow				= m_RenderParams.t_Window;
	m_PresentParams.Windowed					= !m_RenderParams.t_bFullScreen;
	m_PresentParams.EnableAutoDepthStencil		= true;
	m_PresentParams.AutoDepthStencilFormat		= D3DFMT_D16;
	m_PresentParams.Flags						= D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;
	m_PresentParams.FullScreen_RefreshRateInHz	= (m_RenderParams.t_bFullScreen) ? D3DPRESENT_RATE_DEFAULT : 0;
	m_PresentParams.PresentationInterval		= D3DPRESENT_INTERVAL_IMMEDIATE;

	// Create the Direct3D Device.
	if (FAILED(m_lpD3DObject->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, m_RenderParams.t_Window,
		D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED,
		&m_PresentParams, &m_lpD3DDevice)))
	{
		RJ_DEBUGGER->Error(RJ_Debugger::RJ_FATAL,"Failed to create D3DDevice!");
		return false;
	}

	// Create Sprite Object.
	if (FAILED(D3DXCreateSprite(m_lpD3DDevice, &m_lpSprite)))
	{
		RJ_DEBUGGER->Error(RJ_Debugger::RJ_FATAL,"Failed to create the Sprite object!");
		return false;
	}

	// Create the Line Object.
	if (FAILED(D3DXCreateLine(m_lpD3DDevice, &m_lpLine)))
	{
		RJ_DEBUGGER->Error(RJ_Debugger::RJ_FATAL,"Failed to create the Line Object!");
		return false;
	}
	if (FAILED(D3DXCreateFont(m_lpD3DDevice, 20, 0, FW_BOLD, 0, false, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
		DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, TEXT("Arial"), &m_lpFont)))
	{
		RJ_DEBUGGER->Error(RJ_Debugger::RJ_FATAL,"Failed to create the Font object!");
		return false;
	}
	// Setup Line parameters.
	m_lpLine->SetAntialias(TRUE);
	m_lpLine->SetWidth(3.0f);

	InitializeAllVertexDeclarations();

	//RenderSets
	m_lpD3DDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);

	// Create the FX from a .fx file.
	ID3DXBuffer* errors = 0;
	D3DXCreateEffectFromFile(m_lpD3DDevice, "Resources\\Shaders\\Wireframe.fx", 
		0, 0, D3DXSHADER_DEBUG, 0, &m_Shader, &errors);
	if( errors )
		MessageBox(0, (char*)errors->GetBufferPointer(), 0, 0);

	// Obtain handles.
	m_Shader_Technique = m_Shader->GetTechniqueByName("ShaderTechnique");
	m_Shader_WorldVeiwProjection  = m_Shader->GetParameterByName(0, "m4x4_ViewProjection");
	m_Shader_Color = m_Shader->GetParameterByName(0, "v4_Color");

	D3DXCreateBox(m_lpD3DDevice, 0.5f, 0.5f, 0.5f, &m_Box, 0);
	D3DXCreateCylinder(m_lpD3DDevice, 0.5f, 0.5f, 1.0f, 20, 1, &m_Cylinder, 0);
	D3DXCreateSphere(m_lpD3DDevice, 0.5f, 20, 20, &m_Sphere, 0);

	InitializeAxis();
	InitializeGrid();

	return true;
}

bool RJ_Renderer::InternalShutdown(void)
{
	CEventManager::GetInstanceThreaded()->UnregisterListenerAllRender(this);
	ShutdownAllVertexDeclarations();

	RJ_SAFE_RELEASE(m_vbAxis);
	RJ_SAFE_RELEASE(m_vbGround);
	RJ_SAFE_RELEASE(m_Shader);
	RJ_SAFE_RELEASE(m_Cylinder);
	RJ_SAFE_RELEASE(m_Box);
	RJ_SAFE_RELEASE(m_Sphere);
	RJ_SAFE_RELEASE(m_lpFont);
	RJ_SAFE_RELEASE(m_lpLine);
	RJ_SAFE_RELEASE(m_lpSprite);
	RJ_SAFE_RELEASE(m_lpD3DDevice);
	RJ_SAFE_RELEASE(m_lpD3DObject);

	return true;
}

void RJ_Renderer::InitializeAxis(void)
{
	// Create the Vertex Buffer for the Axis lines
	//{
	m_Axis[0].position.x = 0.0f; m_Axis[0].position.y = 0.0f; m_Axis[0].position.z = 0.0f;
	m_Axis[1].position.x = 1.0f; m_Axis[1].position.y = 0.0f; m_Axis[1].position.z = 0.0f;
	m_Axis[2].position.x = 0.0f; m_Axis[2].position.y = 0.0f; m_Axis[2].position.z = 0.0f;
	m_Axis[3].position.x = 0.0f; m_Axis[3].position.y = 1.0f; m_Axis[3].position.z = 0.0f;
	m_Axis[4].position.x = 0.0f; m_Axis[4].position.y = 0.0f; m_Axis[4].position.z = 0.0f;
	m_Axis[5].position.x = 0.0f; m_Axis[5].position.y = 0.0f; m_Axis[5].position.z = 1.0f;
	m_Axis[0].color = D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f);
	m_Axis[1].color = D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f);
	m_Axis[2].color = D3DXCOLOR(0.0f, 1.0f, 0.0f, 1.0f);
	m_Axis[3].color = D3DXCOLOR(0.0f, 1.0f, 0.0f, 1.0f);
	m_Axis[4].color = D3DXCOLOR(0.0f, 0.0f, 1.0f, 1.0f);
	m_Axis[5].color = D3DXCOLOR(0.0f, 0.0f, 1.0f, 1.0f);

	m_lpD3DDevice->CreateVertexBuffer(6*sizeof(m_Axis[0]), 0,
		0, D3DPOOL_MANAGED, &m_vbAxis, 0);

	void* pBuffer;
	m_vbAxis->Lock(0,0, &pBuffer, 0);
	memcpy(pBuffer, m_Axis, 6*sizeof(m_Axis[0]));
	m_vbAxis->Unlock();
	//}
}

void RJ_Renderer::InitializeGrid(void)
{
	int groundRadius = 10;
	int groundStepSize = 1;
	float groundLevel = -0.001f;

	for(int vert = 0, x = -groundRadius; x <= groundRadius; x += groundStepSize, vert += 4)
	{
		// Vertical lines
		m_Ground[vert].position.x = (float)x; m_Ground[vert].position.y = groundLevel; 
		m_Ground[vert].position.z = (float)groundRadius;
		m_Ground[vert+1].position.x = (float)x; m_Ground[vert+1].position.y = groundLevel; 
		m_Ground[vert+1].position.z = (float)-groundRadius;
		// Horizontal lines
		m_Ground[vert+2].position.x = (float)groundRadius; m_Ground[vert+2].position.y = groundLevel; 
		m_Ground[vert+2].position.z = (float)x;
		m_Ground[vert+3].position.x = (float)-groundRadius; m_Ground[vert+3].position.y = groundLevel; 
		m_Ground[vert+3].position.z = (float)x;

		m_Ground[vert].color = D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f);
		m_Ground[vert+1].color = D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f);
		m_Ground[vert+2].color = D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f);
		m_Ground[vert+3].color = D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f);
	}		
  
	m_lpD3DDevice->CreateVertexBuffer(84*sizeof(m_Ground[0]), 0,
		0, D3DPOOL_MANAGED, &m_vbGround, 0);

	void* pBuffer;
	m_vbGround->Lock(0,0, &pBuffer, 0);
	memcpy(pBuffer, m_Ground, 84*sizeof(m_Ground[0]));
	m_vbGround->Unlock();
}

void RJ_Renderer::PostProcess(void)
{
	SetGammaRamp(m_fGammaValue);
}

void RJ_Renderer::SetGammaRamp(float fGamma)
{
	// Windows Gamma
	for (unsigned int i = 0; i < 256; i++)
	{
		m_pRamp.red[i] = unsigned short(min(255, i * (fGamma + 1.0))) << 8;
		m_pRamp.green[i] = unsigned short(min(255, i * (fGamma + 1.0))) << 8;
		m_pRamp.blue[i] = unsigned short(min(255, i * (fGamma + 1.0))) << 8;
	}

	SetDeviceGammaRamp(GetDC(m_RenderParams.t_Window), &m_pRamp);
}

///////////////

#ifdef RJ_DLL
bool dll_RJ_Renderer_Initialize(RJ_Renderer::RenderParameters _params)
{
	return RJ_RENDERER->Initialize(_params);
}

bool dll_RJ_Renderer_Shutdown(void)
{
	return RJ_RENDERER->Shutdown();
}

bool dll_RJ_Renderer_Render(float _fDelta)
{
	return RJ_RENDERER->Render(_fDelta);
}

void dll_RJ_Renderer_Clear(unsigned char _ucRed, unsigned char _ucBlue, unsigned char _ucGreen)
{
	RJ_RENDERER->Clear(_ucRed, _ucBlue, _ucGreen);
}

void dll_RJ_Renderer_FlipBuffers(void)
{
	RJ_RENDERER->FlipBuffers();
}

bool dll_RJ_Renderer_BeginRender(void)
{
	return RJ_RENDERER->BeginRender();
}

bool dll_RJ_Renderer_BeginSprite(void)
{
	return RJ_RENDERER->BeginSprite();
}

bool dll_RJ_Renderer_BeginLine(void)
{
	return RJ_RENDERER->BeginLine();
}

bool dll_RJ_Renderer_EndRender(void)
{
	return RJ_RENDERER->EndRender();
}

bool dll_RJ_Renderer_EndSprite(void)
{
	return RJ_RENDERER->EndSprite();
}

bool dll_RJ_Renderer_EndLine(void)
{
	return RJ_RENDERER->EndLine();
}

bool dll_RJ_Renderer_ChangeDisplay(unsigned int _width, unsigned int _height, bool _fullscreen)
{
	return RJ_RENDERER->ChangeDisplay(_width, _height, _fullscreen);
}

bool dll_RJ_Renderer_EnableFullScreen(bool _fullscreen)
{
	return RJ_RENDERER->EnableFullScreen(_fullscreen);
}

bool dll_RJ_Renderer_ToggleFullScreen(void)
{
	return RJ_RENDERER->ToggleFullScreen();
}

bool dll_RJ_Renderer_GoFullScreen(void)
{
	return RJ_RENDERER->GoFullScreen();
}

bool dll_RJ_Renderer_GoWindowed(void)
{
	return RJ_RENDERER->GoWindowed();
}

bool dll_RJ_Renderer_CheckDeviceCaps(void)
{
	return RJ_RENDERER->CheckDeviceCaps();
}

bool dll_RJ_Renderer_isLostDevice(void)
{
	return RJ_RENDERER->isLostDevice();
}

void dll_RJ_Renderer_onLostDevice(void)
{
	RJ_RENDERER->onLostDevice();
}

void dll_RJ_Renderer_Reset(void)
{
	RJ_RENDERER->Reset();
}

void dll_RJ_Renderer_onResetDevice(void)
{
	RJ_RENDERER->onResetDevice();
}

void dll_RJ_Renderer_RenderText(const char* _text, int _x, int _y, unsigned char _ucRed, 
								unsigned char _ucBlue, unsigned char _ucGreen)
{
	RJ_RENDERER->RenderText(_text, _x, _y, _ucRed, _ucBlue, _ucGreen);
}
#endif