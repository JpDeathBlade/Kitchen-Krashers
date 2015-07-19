#ifndef  JS_MESH_H_
#define  JS_MESH_H_

// The include files for Direct3D9
#include <d3d9.h>
#include <d3dx9.h>
#include <dxerr.h>
#include <fstream>

#include "..\\..\\CoreTech\\RedJaguarEngine\\Effects\\Effect.h"
#include "..\\..\\CoreTech\\RedJaguarEngine\\Base Classes\\IObject.h"
#include "../../RJ_Config.h"

struct VertexFF1
{
	D3DXVECTOR4 position;
	D3DXVECTOR4 normal;
	D3DXVECTOR2 uvTexture;
	D3DXVECTOR2 uvLightMap;
	D3DXVECTOR4	color;
};

class Mesh
{
	private:
		Mesh(Mesh& _ref);
		Mesh& operator= (Mesh& _ref);

		void Initialize(void);

	protected:
		RJ_Config::RJ_Mesh_Types	m_MeshType;
		char						m_szName[64];
		unsigned int				m_unNameLength;
		unsigned int				m_unTextureCount;
		unsigned int				m_unTextureNameLength;
		char						m_szTextureName[128];
		unsigned int				m_unLightMapNameLength;
		char						m_szLightMapName[128];
		unsigned int				m_unNormalNameLength;
		char						m_szNormalName[128];
		unsigned int				m_unSpecNameLength;
		char						m_szSpecName[128];
		unsigned int				m_unShaderNameLength;
		char						m_szShaderName[128];
		int							m_nTextureID;
		int							m_nTextureIDTwo;
		int							m_nTextureIDThree;
		int							m_nTextureIDFour;
		int							m_nTextureIDFive;
		int							m_nTextureIDSix;
		float						m_fBounce;
		float						time;
		float						m_fCurrAdditive;

		bool						Transparent;
		bool						BackFaceCulled;
		bool						Emissive;
		int							VolumeType;

		VertexFF1*					m_VertexList;
		unsigned int				m_unVertexCount;
		IDirect3DVertexBuffer9*		m_vbVertexBuffer;

		unsigned int*				m_IndexList;
		unsigned int				m_unIndexCount;
		IDirect3DIndexBuffer9*		m_ibIndexBuffer;

		unsigned int				m_unPrimitiveCount;
		Effect*						effectSugar;
		bool						effectUpdate;

		bool	switchBack;
		double  shaderTimer;
		ID3DXEffect*	oldShader;

		ID3DXEffect*				m_Shader;
		D3DXHANDLE					m_Shader_Technique;
		D3DXHANDLE					m_Shader_WorldVeiwProjection;
		D3DXHANDLE					m_Shader_TextureCoords;
		D3DXHANDLE					m_Shader_Time;
		D3DXHANDLE					m_Shader_Bounce;
		/*D3DXHANDLE					m_Shader_World;
		D3DXHANDLE					m_Shader_Veiw;
		D3DXHANDLE					m_Shader_Projection;
		D3DXHANDLE					m_Shader_Color;
		D3DXHANDLE					m_Shader_Normal;
		D3DXHANDLE					m_Shader_LightMapCoords;
		D3DXHANDLE					m_Shader_EyePosition;
		D3DXHANDLE					m_Shader_LightPosition;
		D3DXHANDLE					m_Shader_Ambient;
		D3DXHANDLE					m_Shader_Specular;
		D3DXHANDLE					m_Shader_Diffuse;*/

	public:
		int m_nRef;


		Mesh(void);
		~Mesh();

		void Render(double fDelta);
		void Shutdown(void);
		void Update(double fDelta);

		
		float getAdditive(void)											{ return m_fCurrAdditive; }
		const char*			getName(void)								{ return m_szName; }
		const unsigned int	getNameLength(void)							{ return m_unNameLength; }
		const unsigned int	getTextureCount(void)						{ return m_unTextureCount; }
		const char*			getTextureName(unsigned int _texture)		{ return m_szTextureName; }
		const unsigned int	getTextureNameLength(unsigned int _name)	{ return m_unTextureNameLength; }

		const VertexFF1*				getVertexList(void)		{ return m_VertexList; }
		const unsigned int				getVertexCount(void)	{ return m_unVertexCount; }
		const IDirect3DVertexBuffer9*	getVertexBuffer(void)	{ return m_vbVertexBuffer; }
		const unsigned int*				getIndexList(void)		{ return m_IndexList; }
		const unsigned int				getIndexCount(void)		{ return m_unIndexCount; }
		const IDirect3DIndexBuffer9*	getIndexBuffer(void)	{ return m_ibIndexBuffer; }
		const ID3DXEffect*				getShader(void)			{ return m_Shader; }

		void Load(std::fstream* _io);
		void setShader(ID3DXEffect* _shader, bool _switchBack = false, double _time = -1.0);
		void setMeshType(RJ_Config::RJ_Mesh_Types _MeshType)	{ m_MeshType = _MeshType; }
		int setTextureID(int _id)	{ int temp = m_nTextureID; m_nTextureID = _id; return temp; }
		void setAdditive(float fAdditive) { m_fCurrAdditive = fAdditive; }

		void onLostDevice(void);
		void onResetDevice(void);
};

#endif // end of JS_MESH_H_