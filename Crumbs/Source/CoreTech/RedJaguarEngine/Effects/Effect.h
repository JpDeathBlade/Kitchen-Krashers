#ifndef  REDJAGUAR_JS_EFFECT_H_
#define  REDJAGUAR_JS_EFFECT_H_

#include <windows.h>
#include <d3d9.h>
#include <vector>
#include "..\\Renderer\\RJ_Renderer.h"
#include "..\\Renderer\\RJ_Vertex.h"

struct EffectVertex
{
	D3DXVECTOR4		position;
	D3DXVECTOR2		textureUV;
};

class Effect
{
	private:
		int			m_nTextureID;
		int			m_nAlphaTextureID;
		float		m_v4AlphaColor[3];
		int			m_nMapWidth;
		int			m_nMapHeight;
		int			m_nCellWidth;
		int			m_nCellHeight;
		int			m_nColumnCount;
		int			m_nRowCount;
		int			m_nFrameCount;
		int			m_nEffectType;
		bool m_bPlayedOnce;
		bool m_bMultiTextured;
		D3DXVECTOR3 m_Postition;
		D3DXMATRIX  _world;
		D3DXVECTOR3 cam_pos;
		D3DXVECTOR3 forward;

		float		m_fFrameDuration;
		double		m_dCurrentTime;
		float		m_fEffectWidth;
		float		m_fEffectHeight;

		int			m_nCurrentFrame;
		float		m_nScale;
		float		m_fOffsetX;
		float		m_fOffsetY;

		bool		m_bBillboarded;
		float		time;

		ID3DXEffect*				m_Shader;
		D3DXHANDLE					m_Shader_Technique;
		D3DXHANDLE					m_Shader_VeiwProjection;
		D3DXHANDLE					m_Shader_TextureCoords;
		D3DXHANDLE					m_Shader_AlphaTextureCoords;
		D3DXHANDLE					m_Shader_OffsetX;
		D3DXHANDLE					m_Shader_OffsetY;
		D3DXHANDLE					m_Shader_Time;

		EffectVertex	m_VertexList[4];
		IDirect3DVertexBuffer9*		m_vbVertexBuffer;

		Effect(const Effect &ref);
		Effect &operator= (const Effect &ref);

		void eCellAlgorithm(int _cellID);
		void BillBoard();

	public:
		Effect();
		~Effect(void) { Shutdown(); }

		void Load(char* _effectName);
		void Render(double _fDelta);
		void Shutdown();
	
		void SetForward(D3DXVECTOR3 _forward) { forward = _forward; }
		void SetPosition(D3DXVECTOR3 _position);
		void SetPositionWithoutUpdate(D3DXVECTOR3 _position);
		void SetCameraPosition(D3DXVECTOR3 _position);

		bool isPlaying() { return !m_bPlayedOnce; }
		void SetPlaying(bool _playing) { m_bPlayedOnce = _playing; }
		void StopPlaying() { m_bPlayedOnce = true;  m_dCurrentTime = 0.0;}
};

#endif //end of REDJAGUAR_JS_EFFECT_H_