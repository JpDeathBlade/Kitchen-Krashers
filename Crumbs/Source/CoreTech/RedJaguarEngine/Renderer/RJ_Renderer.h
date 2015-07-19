/**
* 0===========================================================================0
* | RJ_Renderer.h                                                             |
* |  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  |
* |   Author        : Joseph Stuhr                                            |
* |   E-mail        : JpDeathBlade@gmail.com                                  |
* |  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  |
* |   Date Created  : 04:07 pm (Eastern) on 05/25/2010                        |
* |   Date Edited   : 00:00 pm (Eastern) on 00/00/2010                        |
* 0=====0===============================================================0=====0
*       |                                                               |      
* 0=====0===============================================================0=====0
* | Code Description:                                                         |
* |  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  |
* |   The main renderer that controls the graphics API.                       |
* 0===========================================================================0
*/

#ifndef  REDJAGUAR_JS_RENDERER_H_	// Check to see if this .h is defined
#define  REDJAGUAR_JS_RENDERER_H_	//  if not, define it

// The include files for Direct3D9
#include <d3d9.h>
#include <d3dx9.h>
#include <dxerr.h>

// Includes
#include "../Event/IListener.h"
#include "../Event/EventManager.h"
#include "../Base Classes/IObject.h"

// The library files for Direct3D9
#pragma comment(lib, "d3d9.lib")
#pragma comment (lib, "dxguid.lib")

//Defines
#define RJ_RENDERER RJ_Renderer::GetInstance()

struct ColorVertex
{
	D3DXVECTOR4 position;
	D3DCOLOR    color;
};

class RJ_Renderer : public IListener
{
	public:
		//__declspec(align(4))		// Allow the struct to be 4 byte aligned
		struct RenderParameters		//  RenderParameters that the Render needs to
		{							//  initalize the DirectX API
			HWND t_Window;	
			unsigned int t_uiClientWidth;
			unsigned int t_uiClientHeight;
			unsigned int t_uiPositionX;
			unsigned int t_uiPositionY;
			float t_fNearClip;
			float t_fFarClip;
			unsigned char t_ucClearRed;
			unsigned char t_ucClearGreen;
			unsigned char t_ucClearBlue;
			bool t_bFullScreen;
		};	// end of struct RenderParameters

	private:
		static RJ_Renderer m_Instance;

		ColorVertex					m_Axis[6];
		ColorVertex					m_Ground[84];
		IDirect3DVertexBuffer9*		m_vbAxis;
		IDirect3DVertexBuffer9*		m_vbGround;
		IDirect3DIndexBuffer9*		m_ibGround;
		CEvent*						m_ObjEvent;
		D3DGAMMARAMP				m_pRamp;
		float						m_fGammaValue;
		deque<IObject*>				Opaque;
		deque<IObject*>				Transparent;

		RJ_Renderer(void);
		RJ_Renderer(RJ_Renderer& _ref);
		RJ_Renderer& operator= (RJ_Renderer& _ref);

		bool InternalInitialize(void);
		bool InternalShutdown(void);
		void InitializeAxis(void);
		void InitializeGrid(void);
		void DrawPrimitive(ID3DXMesh* _mesh, D3DXMATRIX* _position, D3DXVECTOR4* _color);

	protected:
		RenderParameters			m_RenderParams;

		LPDIRECT3D9					m_lpD3DObject;
		LPDIRECT3DDEVICE9			m_lpD3DDevice;
		LPD3DXSPRITE				m_lpSprite;
		LPD3DXFONT					m_lpFont;
		LPD3DXLINE					m_lpLine;
		D3DPRESENT_PARAMETERS		m_PresentParams;
		D3DXMATRIX					m_mProjMat;
		D3DXMATRIX					m_mViewMat;
		D3DXMATRIX					m_mCameraMat;
		float						m_fLaunchPower;
		float						m_fLaunchAngle;
		float						m_fStoneSpeed;
		float						m_fStoneAngle;
		D3DXVECTOR3					m_vStoneForward;
		unsigned int				m_nScore;
		ID3DXEffect*				m_Shader;
		D3DXHANDLE					m_Shader_Technique;
		D3DXHANDLE					m_Shader_WorldVeiwProjection;
		D3DXHANDLE					m_Shader_Color;
		ID3DXEffect*				m_PostShader;
		D3DXHANDLE					m_PostProcess_Technique;
		D3DXHANDLE					m_PostProcess_Tex;
		D3DXHANDLE					m_PostProcess_Gamma;
		ID3DXMesh*					m_Cylinder;
		ID3DXMesh*					m_Box;
		ID3DXMesh*					m_Sphere;

	public:
		static RJ_Renderer* GetInstance(void) { return &m_Instance; }
		virtual ~RJ_Renderer(void) { }

		bool Initialize(RenderParameters _params);
		bool Shutdown(void);
		bool BeginRender(void);
		bool Render(double _fDelta);
		bool EndRender(void);
		void Clear(unsigned char _ucRed = 195, unsigned char _ucBlue = 195, unsigned char _ucGreen = 195);
		void FlipBuffers(void);
		bool BeginScene(void);
		bool BeginSprite(void);
		bool BeginLine(void);
		bool EndScene(void);
		bool EndSprite(void);
		bool EndLine(void);
		void PostProcess(void);
		void SetGammaRamp(float fGamma);
		bool ChangeDisplay(unsigned int _width, unsigned int _height, bool _fullscreen);
		bool ChangeDisplay(RenderParameters _params);
		bool EnableFullScreen(bool _fullscreen);
		bool ToggleFullScreen(void);
		bool GoFullScreen(void);
		bool GoWindowed(void);
		bool BuildPresentParameters(void);
		bool CheckDeviceCaps(void);
		bool isLostDevice(void);
		void onLostDevice(void);
		void Reset(void);
		void Reset(D3DPRESENT_PARAMETERS _params);
		void onResetDevice(void);
		void HandleEvent(CEvent* pEvent);
		void SendListClearedEvent(void);
		void RenderRect(RECT _rect, unsigned char _ucRed, unsigned char _ucBlue, unsigned char _ucGreen);
		void RenderLine(D3DXVECTOR2 _start, D3DXVECTOR2 _end, unsigned char _ucRed = 255,
			unsigned char _ucBlue = 255, unsigned char _ucGreen = 255, float _thickness = 5.0f);
		void RenderLine(D3DXVECTOR2 _start, D3DXVECTOR2 _end, D3DCOLOR _color = D3DCOLOR_XRGB(255, 255, 255), float _thickness = 5.0f);
		void RenderText(const char* _text, int _x, int _y, unsigned char _ucRed = 255, 
			unsigned char _ucBlue = 255, unsigned char _ucGreen = 255);
		void Render3DLine(D3DXVECTOR3 _start, D3DXVECTOR3 _end, D3DXVECTOR4 _color);
		void DrawCylinder(D3DXMATRIX* _position, D3DXVECTOR4* _color);
		void DrawSphere(D3DXMATRIX* _position, D3DXVECTOR4* _color, float _scaleX = 1.0f, float _scaleY = 1.0f, float _scaleZ = 1.0f);
		void DrawBox(D3DXMATRIX* _position, D3DXVECTOR4* _color);
		void DrawOOB(ColorVertex* _verts, D3DXMATRIX* _position);
		void DrawAxisLines();
		void DrawGrid();

		// 0===================================================================0
		// | ~ ACCESSORS ~                                                     |
		// 0===================================================================0
		LPDIRECT3D9						getDirect3DObject(void)		{ return m_lpD3DObject; }
		LPDIRECT3DDEVICE9				getDirect3DDevice(void)		{ return m_lpD3DDevice; }
		LPD3DXSPRITE					getSprite(void)				{ return m_lpSprite; }
		LPD3DXLINE						getLine(void)				{ return m_lpLine; }
		const D3DPRESENT_PARAMETERS*	getPresentParams(void)		{ return &m_PresentParams; }

		const RenderParameters		getRenderParameters(void)	{ return m_RenderParams; }
		const HWND					getWindowHandle(void)		{ return m_RenderParams.t_Window; }
		const unsigned int			getClientWidth(void)		{ return m_RenderParams.t_uiClientWidth; }
		const unsigned int			getClientHeight(void)		{ return m_RenderParams.t_uiClientHeight; }
		const bool					getFullScreen(void)			{ return m_RenderParams.t_bFullScreen; }
		const unsigned int			getPositionX(void)			{ return m_RenderParams.t_uiPositionX; }
		const unsigned int			getPositionY(void)			{ return m_RenderParams.t_uiPositionY; }
		const float					getNearClip(void)			{ return m_RenderParams.t_fNearClip; }
		const float					getFarClip(void)			{ return m_RenderParams.t_fFarClip; }
		const unsigned char			getClearRed(void)			{ return m_RenderParams.t_ucClearRed; }
		const unsigned char			getClearGreen(void)			{ return m_RenderParams.t_ucClearGreen; }
		const unsigned char			getClearBlue(void)			{ return m_RenderParams.t_ucClearBlue; }
		float						getLaunchPower(void)		{ return m_fLaunchPower; }
		float						getLaunchAngle(void)		{ return m_fLaunchAngle; }
		float						getStoneSpeed(void)			{ return m_fStoneSpeed; }
		unsigned int				getCurrPlayerScore(void)	{ return m_nScore; }
		float						getStoneAngle(void)			{ return m_fStoneAngle; }
		D3DXVECTOR3					getStoneForward(void)		{ return m_vStoneForward; }
		const D3DXMATRIX&			getProjectionMatrix()		{ return m_mProjMat; }
		const D3DXMATRIX&			getViewMatrix()				{ return m_mViewMat; }
		const D3DXMATRIX&			getCameraPosition()			{ return m_mCameraMat; }
		std::deque<IObject*>*		GetOpaqueObjectList(void)		{ return &Opaque; }
		std::deque<IObject*>*		GetTransparentObjectList(void)	{ return &Transparent; }

		// 0===================================================================0
		// | ~ MUTATORS ~                                                      |
		// 0===================================================================0
		void setRenderParameters(const RenderParameters _params)	{ m_RenderParams = _params; }
		void setWindowHandle(const HWND _hWnd)						{ m_RenderParams.t_Window = _hWnd; }
		void setClientWidth(const unsigned int _width)				{ m_RenderParams.t_uiClientWidth = _width; }
		void setClientHeight(const unsigned int _height)			{ m_RenderParams.t_uiClientHeight = _height; }
		void setFullScreen(const bool _fullscreen)					{ m_RenderParams.t_bFullScreen = _fullscreen; }
		void setPositionX(const unsigned int _x)					{ m_RenderParams.t_uiPositionX = _x; }
		void setPositionY(const unsigned int _y)					{ m_RenderParams.t_uiPositionY = _y; }
		void setNearClip(const float _near)							{ m_RenderParams.t_fNearClip = _near; }
		void setFarClip(const float _far)							{ m_RenderParams.t_fFarClip = _far; }
		void setClearRed(const unsigned int _red)					{ m_RenderParams.t_ucClearRed = _red; }
		void setClearGreen(const unsigned int _green)				{ m_RenderParams.t_ucClearGreen = _green; }
		void setClearBlue(const unsigned int _blue)					{ m_RenderParams.t_ucClearBlue = _blue; }
};

#endif //REDJAGUAR_JS_RENDERER_H_