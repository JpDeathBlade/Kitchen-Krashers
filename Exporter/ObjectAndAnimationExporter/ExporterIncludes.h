#pragma once
#include <vector>

#include "vec3.h"
#include "matrix4.h"
#include <vector>
using std::vector;
#include <deque>
using std::deque;
#include <string>
using std::string;

#include <maya/MDagPath.h>
#include <maya/MDagPathArray.h>
#include <maya/MString.h>
#include <maya/MStringArray.h>
#include <maya/MFnMesh.h>
#include <maya/MItMeshPolygon.h>
#include <maya/MPointArray.h>
#include <maya/MFloatPoint.h>
#include <maya/MFloatPointArray.h>
#include <maya/MFloatVector.h>
#include <maya/MFloatVectorArray.h>
#include <maya/MFloatArray.h>
#include <maya/MFnSet.h>
#include <maya/MPlugArray.h>
#include <maya/MPlug.h>
#include <maya/MItDependencyGraph.h>
#include <maya/MMatrix.h>
#include <maya/MTime.h>
#include <maya/MItDag.h>
#include <maya/mFnTransform.h>
#include <maya/MAnimUtil.h>
#include <maya/MFnSkinCluster.h>
#include <maya/MItGeometry.h>
#include <maya/MFnAnimCurve.h>
#include <maya/MAnimControl.h>
#include <maya/MSelectionList.h>
#include <maya/MGlobal.h>
#include <maya/MColor.h>

#include <d3d9.h>
#include <D3DX9.h>

//////////////////////////////////////////////////////////////////////////////////////////////////////
/*
	The following are structures that can be used for exporting data from Maya
*/
//////////////////////////////////////////////////////////////////////////////////////////////////////



//a structure holding the three indices that represent the three unique verts that make up a triangle
typedef struct tTri
{
	union
	{
		unsigned vertIndices[3];
		struct  
		{
			unsigned index1, index2, index3;
		};

	};
	

} indexedTris, *p_indexedTris;

//a template attribute structure that can hold any type of attribute
template <typename AttributeType>
struct MayaAttribute
{
	AttributeType	attValue;
	MString			attName;
};


struct tKeyFrame
{
	double m_dTimeInSeconds;
	MTime m_KeyTime;
	MMatrix m_LocalTransform;
	MMatrix m_WorldTransform;
};

//Encapsulates critical elements of a joint for use with skinned animation
struct tJoint
{
	MMatrix m_WorldTransform;
	MMatrix m_LocalTransform;
	MString m_Name;

	//List of key frames for the joint
	std::vector<tKeyFrame> m_vKeyFrames;
};

//Encapsulates critical information for nodes that reference joints
//in a skeleton hierarchy for use with skinned animation
struct tSkeletonNode
{
	unsigned int m_nJointIndex;

	//List of child nodes in the skeleton hierarchy
	std::vector<tSkeletonNode> m_vChildren;
};

//Encapsulates critical information for joint influences as it 
//pertains to smooth skinned animations
struct tInfluence
{
	unsigned int m_nJointIndex; //which joint we're influencing
	float m_fWeight; //How much we're influencing the joint by

	bool operator==(const tInfluence influence)
	{
		return (m_nJointIndex==influence.m_nJointIndex)&&(m_fWeight==influence.m_fWeight);
	}
};
//a vertex structure using ED2 Math structures
typedef struct tVert
{
	unsigned int m_nMayaPointIndex;
	std::vector<tInfluence> m_vInfluences;
	vec3<float>		pos;
	vec3<float>		normal;
	float		textureU, textureV;
	D3DXVECTOR4      	color;
	bool operator==(tVert& rh)
	{
		return (pos.x == rh.pos.x) && 
			(pos.y == rh.pos.y) && 
			(pos.z == rh.pos.z) && 
			(normal.x == rh.normal.x) && 
			(normal.y == rh.normal.y) && 
			(normal.z == rh.normal.z) && 
			(textureU == rh.textureU) && (textureV == rh.textureV);
	}
}ed2Vert, *p_ed2Vert;

struct Animation
{
	std::vector<tJoint> ThemBones;
	tSkeletonNode RootCoreNode;
	std::vector<std::pair<MDagPath,int>> bonePaths;	
	MDagPath rootPath;
	std::string meshName;
};

//a template mesh structure that can hold either vertex type that suits your needs
struct exMesh
{
	bool						bDynamic;

	std::vector<ed2Vert>		uniqueVerts; //our unique vertex list
	std::vector<indexedTris>	triList; //our list of triangle indices
	std::vector<MString>		textureNames; //our list of textures
	std::vector<tJoint>			ThemBones;
	tSkeletonNode				RootCoreNode;
	
	MayaAttribute<MString>		LightMapName;
	MayaAttribute<MString>		NormalMapName;
	MayaAttribute<MString>		SpecMapName;
	MayaAttribute<MString>		ShaderName;

	MayaAttribute<bool>			Transparent;
	MayaAttribute<bool>			BackFaceCulled;
	MayaAttribute<bool>			Emissive;

	MayaAttribute<int>			VolumeType;

	MString						meshName; //the name of the mesh
	MString						meshTrackBack; //the track back path of a mesh
};