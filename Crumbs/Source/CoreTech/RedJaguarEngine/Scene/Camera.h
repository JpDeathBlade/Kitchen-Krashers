/** 
* 0===========================================================================0
* | Camera.h                                                                  |
* |  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  |
* |   Author        : Zac Andersen                                            |
* |   E-mail        : andersen@fullsail.edu                                   |
* |  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  |
* |   Date Created  : 09:25 am (Eastern) on 06/14/2010                        |
* |   Date Edited   : 00:00 pm (Eastern) on 04/30/2010                        |
* 0=====0===============================================================0=====0
*       |                                                               |      
* 0=====0===============================================================0=====0
* | Code Description:                                                         |
* |  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  |
* |   Controls the Camera in the game   			                          |
* 0===========================================================================0
*/
#ifndef CCAMERA_H_
#define CCAMERA_H_

#include "../Event/IListener.h"
#include <queue>
#include <D3dx9math.h>
#include "../../../RJ_Config.h"

class CEvent;
class CGameplay;

class CCamera : public IListener
{
	D3DXMATRIX m_mProjMat;
	D3DXMATRIX m_mViewMat;
	//////////////////////////////////////////////////////////////////////////
	//Frustum Planes
	D3DXPLANE m_frustum[6];
	//0 == Left, 1 == Right, 2 == Top, 3 == Bottom, 4 == Near, 5 == Far;
	//////////////////////////////////////////////////////////////////////////

	std::queue<D3DXMATRIX> SoftAttachQueue;

	CGameplay* m_cGameplay;

	RJ_Config::CAM_TYPE			m_nCamID;

	float FieldofView;
	float Near;
	float Far;
	float Width;
	float Height;
	bool LerpisDone;

	float m_fSentTimer;
	
	float Power;
	float Angle;
	float Speed;
	float StoneAngle;
	float lerp;
	D3DXVECTOR3 StoneForward;
	unsigned int CurrPlayerScore;
	D3DXMATRIX PrevMat;
	D3DXMATRIX PrevLook;

	bool Look;
	bool SendDoneMess;
	bool Done;


public:
	CCamera(void);
	~CCamera(void);
	bool init(float Near, float far, float ClientWidth, float ClientHeight);

	//////////////////////////////////////////////////////////////////////////
	//Frustum Functions
	void BuildViewFrustum();
	const D3DXPLANE* GetFrustum() { return m_frustum; }
	const D3DXPLANE GetFrustumLeft() { return m_frustum[0]; }
	const D3DXPLANE GetFrustumRight() { return m_frustum[1]; }
	const D3DXPLANE GetFrustumTop() { return m_frustum[2]; }
	const D3DXPLANE GetFrustumBottom() { return m_frustum[3]; }
	const D3DXPLANE GetFrustumNear() { return m_frustum[4]; }
	const D3DXPLANE GetFrustumFar() { return m_frustum[5]; }
	//////////////////////////////////////////////////////////////////////////

	D3DXMATRIX GetProjectionMatrix(void) { return m_mProjMat; }
	void SetProjectionMatrix(D3DXMATRIX *_Matrix);
	void BuildPerspective();

	float GetFieldofView() const { return FieldofView; }
	float GetNear() const { return Near; }
	float GetFar() const { return Far; }
	float GetHeight() const { return Height; }
	float GetWidth() const { return Width; }


	void SetWidth(float val) { Width = val; BuildPerspective(); }
	void SetFar(float val) { Far = val; BuildPerspective(); }
	void SetNear(float val) { Near = val; BuildPerspective(); }
	void SetFieldofView(float val) { FieldofView = val; BuildPerspective(); }
	void SetHeight(float val) { Height = val; BuildPerspective(); }



	// 0===================================================================0
	// | ~ View Matrix Functions ~                                         |
	// 0===================================================================0
	void SetCamType(RJ_Config::CAM_TYPE nCamID){m_nCamID = nCamID;}

	const D3DXMATRIX GetViewMatrix  (bool _Translate = true);
	void SetViewMatrix(D3DXMATRIX *_Matrix);

	D3DXVECTOR3 GetViewXAxis(void);
	void SetViewXAxis(D3DXVECTOR3 _Position);
	void SetViewXAxis(float _x, float _y, float _z);

	D3DXVECTOR3 GetViewYAxis(void);
	void SetViewYAxis(D3DXVECTOR3 _Position);
	void SetViewYAxis(float _x, float _y, float _z);

	D3DXVECTOR3 GetViewZAxis(void);
	void SetViewZAxis(D3DXVECTOR3 _Position);
	void SetViewZAxis(float _x, float _y, float _z);

	D3DXVECTOR3 GetViewPosition(void);
	void SetViewPosition(D3DXVECTOR3 _Position);
	void SetViewPosition(float _x, float _y, float _z);

	// 0===================================================================0
	// | ~ Local Transform Functions ~                                         |
	// 0===================================================================0
	void ViewRotateLocalX(float _Angle);
	void ViewRotateLocalY(float _Angle);
	void ViewRotateLocalZ(float _Angle);

	void ViewTranslateLocal(D3DXVECTOR3 _Axis, bool _CantRoll = true);
	void ViewTranslateLocalX(float _Scale, bool _CantRoll = true);
	void ViewTranslateLocalY(float _Scale, bool _CantRoll = true);
	void ViewTranslateLocalZ(float _Scale, bool _CantRoll = true);

	// 0===================================================================0
	// | ~ Global Transform Functions ~                                    |
	// 0===================================================================0
	void ViewRotateGlobalX(float _Angle);
	void ViewRotateGlobalY(float _Angle);
	void ViewRotateGlobalZ(float _Angle);

	void ViewTranslateGlobal(D3DXVECTOR3 _Axis);
	void ViewTranslateGlobalX(float _Scale);
	void ViewTranslateGlobalY(float _Scale);
	void ViewTranslateGlobalZ(float _Scale);

	void HandleEvent(CEvent* pEvent);

	void attachTo(D3DXMATRIX &OutMat, const D3DXMATRIX &AttachMat, const D3DXMATRIX &AttachToMat,
		const D3DXVECTOR3 &offset, const D3DXVECTOR3 &target, double fDelta, bool _launched, bool MenuCam = false, bool ClearQ = false, bool CamFly = false);

	void SoftAttach(D3DXMATRIX &OutMat, const D3DXMATRIX &AttachMat, const D3DXMATRIX &AttachToMat,
		const D3DXVECTOR3 &offset, double fDelta, bool _launched, bool MenuCam = false, bool ClearQ = false, bool CamFly = false);

	void LookAt(D3DXMATRIX &OutMat, const D3DXMATRIX &AttachMat, const D3DXVECTOR3 &target, double fDelta);

	void SendCameraEvent(void);

};

#endif