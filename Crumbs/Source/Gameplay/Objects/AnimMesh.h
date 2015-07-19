#ifndef  ANIMMESH_H_
#define  ANIMMESH_H_

#include <d3d9.h>
#include <d3dx9.h>
#include <dxerr.h>
#include <fstream>
#include <string>
#include <deque>
#include "../../CoreTech/RedJaguarEngine/Base Classes/IObject.h"
#include "../../RJ_Config.h"
using std::string;
using std::deque;

class AnimStateMachine;
class CSpectator;

struct decompsedMat
{
	D3DXVECTOR3 translate;
	D3DXVECTOR3 scale;
	D3DXQUATERNION rot;
};

class MatrixHierarchy
{
	D3DXMATRIX m_LocalMat;
	D3DXMATRIX m_WorldMat;
	bool m_bFlag;
	MatrixHierarchy *m_pParent;
	std::deque< MatrixHierarchy * > m_vrgChildren;
public:
	int m_nNumChildren;
	enum { DIRTY = true };
	bool GetFlag(){return m_bFlag;}
	void SetFlag(bool bFlag) {m_bFlag = bFlag;}
	MatrixHierarchy(void) : m_bFlag(0), m_pParent(0)
	{
	}
	virtual ~MatrixHierarchy(void)
	{
		m_vrgChildren.clear();
	}
	void Update(void);
	D3DXMATRIX& GetLocalMatDX(void){ return m_LocalMat; }
	D3DXMATRIX& GetWorldMatDX(void);
	MatrixHierarchy *GetParent(void){ return m_pParent; }
	std::deque< MatrixHierarchy *> &GetChildren(void){ return m_vrgChildren; }
	MatrixHierarchy *AddChild( MatrixHierarchy *pNode );
	MatrixHierarchy *RemoveChild( MatrixHierarchy *pNode );
	friend void DXInvert(D3DXMATRIX* mat);
	friend void DXNorm(D3DXVECTOR3& vec);
	friend D3DXMATRIX interpolate(D3DXMATRIX* matA,D3DXMATRIX* matB,float lamda);
	friend D3DXMATRIX interpolate(decompsedMat& matA,decompsedMat& matB,float lamda);
	friend void DXOrthoNormal(D3DXMATRIX* MatrixO,const D3DXMATRIX* MatrixA);
};

class Bone : public MatrixHierarchy
{
private:
	int m_iBoneIndex;
	std::string m_szBoneName;
public:
	int m_iParentIndex;
	Bone() 
	{ 
		m_iBoneIndex = -1; 
	}
	~Bone()
	{
		m_szBoneName = "";
	}
	int GetBoneIndex() { return m_iBoneIndex; }
	std::string  GetBoneName() { return m_szBoneName; }
	void SetBoneIndex(int iBoneIndex) { m_iBoneIndex = iBoneIndex; }
	void SetBoneName(std::string szBoneName) { m_szBoneName = szBoneName; }
};

struct Joint
{
	string m_szJointName;
	Bone TransformFrame;
	Joint(){}
	~Joint(){m_szJointName = ""; }
	Joint(const Joint &joint)
	{
		m_szJointName = joint.m_szJointName;
		TransformFrame = joint.TransformFrame;
	}
	Joint& operator=(const Joint &joint)
	{
		if(this != &joint)
		{
			m_szJointName = joint.m_szJointName;
			TransformFrame = joint.TransformFrame;
		}
		return *this;
	}
};

struct Influence
{
	int m_nBoneIndex;
	float m_fWeight;
	Influence(){}
	Influence(const Influence &influence)
	{
		m_nBoneIndex = influence.m_nBoneIndex;
		m_fWeight = influence.m_fWeight;
	}
	Influence& operator=(const Influence &influence)
	{
		if(this != &influence)
		{
			m_nBoneIndex = influence.m_nBoneIndex;
			m_fWeight = influence.m_fWeight;
		}
		return *this;
	}
};

struct AnimatedVert
{
	D3DXVECTOR3 position;
	D3DXVECTOR2 uvTexture;
	D3DXVECTOR3 weights;
	D3DXVECTOR3 m_nBoneIndex;
	D3DXVECTOR3 positionone;
	D3DXVECTOR3 positiontwo;
};


class AnimMesh
{
private:
	AnimMesh(AnimMesh& _ref);
	AnimMesh& operator= (AnimMesh& _ref);

	void Initialize(void);

protected:
	RJ_Config::RJ_Mesh_Types	m_MeshType;
	char						m_szName[64];
	unsigned int				m_unNameLength;
	unsigned int				m_unTextureCount;
	unsigned int				m_unTextureNameLength;
	char*						m_szTextureName;
	unsigned int				m_unLightMapNameLength;
	char*						m_szLightMapName;
	unsigned int				m_unNormalNameLength;
	char*						m_szNormalName;
	unsigned int				m_unSpecNameLength;
	char*						m_szSpecName;
	unsigned int				m_unShaderNameLength;
	char*						m_szShaderName;
	IDirect3DTexture9*			m_Texture;
	IDirect3DTexture9*			m_TextureTwo;
	IDirect3DTexture9*			m_TextureThree;
	IDirect3DTexture9*			m_TextureFour;
	int							m_nTextureID;
	int							m_nTextureIDTwo;
	int							m_nTextureIDThree;
	int							m_nTextureIDFour;
	bool						Transparent;
	bool						BackFaceCulled;
	bool						Emissive;
	int							VolumeType;

	AnimatedVert*				m_AnimVertexList;
	std::deque<D3DXVECTOR3>      m_BindVerts;
	std::deque<D3DXVECTOR3>      m_IndCVecs;

	unsigned int				m_unVertexCount;
	IDirect3DVertexBuffer9*		m_vbVertexBuffer;
	unsigned int*				m_IndexList;
	unsigned int				m_unIndexCount;
	IDirect3DIndexBuffer9*		m_ibIndexBuffer;
	unsigned int				m_unPrimitiveCount;
	ID3DXEffect*				m_Shader;
	D3DXHANDLE					m_Shader_Technique;
	D3DXHANDLE					m_Shader_WorldVeiwProjection;
	D3DXHANDLE					m_Shader_TextureCoords;
	void*						pBuffer;

	//BONES
	size_t						m_nNumBones;
	deque<Joint>				m_dqJointDeck;

	/*D3DXHANDLE				m_Shader_World;
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

	D3DXMATRIX bonePoses[49];

public:
	int m_nRef;
	AnimMesh(void);
	~AnimMesh();
	void Render(double fDelta, AnimStateMachine* m_AStateMachine, CSpectator* Attach = NULL);
	void Shutdown(void);
	void Update(double fDelta);
	const char*			getName(void)								{ return m_szName; }
	const unsigned int	getNameLength(void)							{ return m_unNameLength; }
	const unsigned int	getTextureCount(void)						{ return m_unTextureCount; }
	const char*			getTextureName(unsigned int _texture)		{ return m_szTextureName; }
	const unsigned int	getTextureNameLength(unsigned int _name)	{ return m_unTextureNameLength; }
	const AnimatedVert*				getVertexList(void)		{ return m_AnimVertexList; }
	const unsigned int				getVertexCount(void)	{ return m_unVertexCount; }
	const IDirect3DVertexBuffer9*	getVertexBuffer(void)	{ return m_vbVertexBuffer; }
	const unsigned int*				getIndexList(void)		{ return m_IndexList; }
	const unsigned int				getIndexCount(void)		{ return m_unIndexCount; }
	const IDirect3DIndexBuffer9*	getIndexBuffer(void)	{ return m_ibIndexBuffer; }
	const ID3DXEffect*				getShader(void)			{ return m_Shader; }
	void Load(std::fstream* _io);
	void setShader(ID3DXEffect* _shader)	{ m_Shader = _shader; }
	void setMeshType(RJ_Config::RJ_Mesh_Types _MeshType)	{ m_MeshType = _MeshType; }
	void setTextureName(unsigned int _texture, char * szTextureName)		{ m_szTextureName = szTextureName; }
	void onLostDevice(void);
	void onResetDevice(void);
};

#endif // end of JS_MESH_H_