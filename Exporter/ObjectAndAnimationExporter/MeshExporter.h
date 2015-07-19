#pragma once
#include "ExporterIncludes.h"
#include <maya/MFnMesh.h>
#include <maya/MFileObject.h>
#include <string>
#include <vector>
#include <fstream>
using std::vector;
using std::string;


class CMeshExporter
{
	//This will be your collection of meshes that you export, whether it be 1 or 100
	//exMesh is a template structure that can take either of the three types of verts that
	//are declared in ExporterIncludes.h
	std::vector< exMesh > m_exportedMeshes;
	bool	ExportAnim;
	Animation TheAnimation;
	MDagPath rootPath;
	exMesh	BindMesh;

	void ExportMesh(MFnMesh & currMesh, exMesh & outMesh);

	void WriteMesh_XML( const exMesh& writeOutMesh );
	void WriteMesh_Binary( const exMesh& writeOutMesh );

	void WriteAnimXML(const Animation& writeOutAnim );
	void WriteAnimBinary(const Animation& writeOutAnim );

public:
	CMeshExporter(void){ExportAnim = false;}
	~CMeshExporter(void){}

	void getTextureNamesFromMesh( MFnMesh &mesh, MStringArray &names, unsigned &count );
	MStatus ClearAndReset(void);
	void ExportMeshes(bool bExportAll = true);
	void WriteAnimations( const MFileObject& file, bool bWriteBinary = true );
	void WriteOutMeshes( bool bWriteBinary = true );
	MString GetTrackBack_MString(MDagPath & dagPath);
	void GetAnimInfo(Animation &mAnim);
	void GetBoneData(Animation &mAnim);

	void GatherMeshInformation(MDagPath currPath, exMesh &outMesh);
	bool GatherBindInformation(MDagPath currPath, MFnMesh PMesh, exMesh &outMesh);
	bool BuildSkeletonHierarchy(tSkeletonNode* pCurrentNode, MDagPath& jointPath,  exMesh &outMesh);
	bool BuildSkeletonHierarchy(tSkeletonNode* pCurrentNode, MDagPath& jointPath,  Animation &outAnim);
	void SmartInsertKeyFrame(std::vector<tKeyFrame> &myKeys, tKeyFrame keyToAdd);
	int GetParentByIndex(unsigned index, tSkeletonNode & currNode);
	tSkeletonNode * FindSkeletonNodeByIndex(const unsigned boneIndex, tSkeletonNode& currNode);
};

