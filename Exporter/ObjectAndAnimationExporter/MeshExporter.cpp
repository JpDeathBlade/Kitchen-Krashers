#include "MeshExporter.h"

MStatus CMeshExporter::ClearAndReset(void)
{
	//TODONE: make sure you clear out any vectors
	size_t exmSize = m_exportedMeshes.size();

	for(size_t _exm = 0; _exm < exmSize; _exm++)
	{
		m_exportedMeshes[_exm].ThemBones.clear();
		m_exportedMeshes[_exm].textureNames.clear();
		m_exportedMeshes[_exm].triList.clear();
		m_exportedMeshes[_exm].uniqueVerts.clear();
		m_exportedMeshes[_exm].RootCoreNode.m_nJointIndex=-1;
		m_exportedMeshes[_exm].RootCoreNode.m_vChildren.clear();
	}
	m_exportedMeshes.clear();
	//return success
	return MStatus::kSuccess;
}
void CMeshExporter::ExportMeshes(bool bExportAll)
{
	if(bExportAll)
	{
		MItDag daggIt( MItDag::kDepthFirst, MFn::kMesh);
		for(; !daggIt.isDone(); daggIt.next())
		{
			MDagPath currPath;
			daggIt.getPath(currPath);
			MFnMesh currMesh(currPath);
			if(currMesh.isIntermediateObject())
				continue;
			exMesh outMesh;
			outMesh.bDynamic = false;
			ExportMesh(currMesh,outMesh);
			GatherMeshInformation(currPath,outMesh);
			m_exportedMeshes.push_back(outMesh);
		}
	}
	else
	{
		MItDag iter(MItDag::kDepthFirst,MFn::kMesh);
		for(;!iter.isDone();iter.next())
		{
			ClearAndReset();
			MDagPath currPath;
			iter.getPath(currPath);
			MObject object;
			object = currPath.node();
			MFn::Type objectType = object.apiType();
			MFnMesh PMesh( currPath );
			if(PMesh.isIntermediateObject())
				continue;
			MFn::Type temp = PMesh.type();
			ExportMesh(PMesh,BindMesh);
			if(!BindMesh.uniqueVerts.size())
				continue;
			std::deque<std::pair<MDagPath,int>> bonePaths;

			MStatus status;
			MItDag jointIter(MItDag::kDepthFirst,MFn::kJoint,&status);
			while(!jointIter.isDone())
			{	
				MDagPath jointPath;
				jointIter.getPath(jointPath);
				MString name = jointPath.partialPathName();
				MFnTransform jointTransform(jointPath);
				MMatrix localTransform = jointTransform.transformationMatrix();
				MMatrix transformWorld = jointPath.inclusiveMatrix();
				D3DXMATRIX Local;
				Local._11 = (float)localTransform[0][0];Local._12 = (float)localTransform[0][1];Local._13 = (float)localTransform[0][2];Local._14 = (float)localTransform[0][3];
				Local._21 = (float)localTransform[1][0];Local._22 = (float)localTransform[1][1];Local._23 = (float)localTransform[1][2];Local._24 = (float)localTransform[2][3];
				Local._31 = (float)localTransform[2][0];Local._32 = (float)localTransform[2][1];Local._33 = (float)localTransform[2][2];Local._34 = (float)localTransform[2][3];
				Local._41 = (float)localTransform[3][0];Local._42 = (float)localTransform[3][1];Local._43 = (float)localTransform[3][2];Local._44 = (float)localTransform[3][3];

				D3DXMATRIX World;
				World._11 = (float)transformWorld[0][0];World._12 = (float)transformWorld[0][1];World._13 = (float)transformWorld[0][2];World._14 = (float)transformWorld[0][3];
				World._21 = (float)transformWorld[1][0];World._22 = (float)transformWorld[1][1];World._23 = (float)transformWorld[1][2];World._24 = (float)transformWorld[2][3];
				World._31 = (float)transformWorld[2][0];World._32 = (float)transformWorld[2][1];World._33 = (float)transformWorld[2][2];World._34 = (float)transformWorld[2][3];
				World._41 = (float)transformWorld[3][0];World._42 = (float)transformWorld[3][1];World._43 = (float)transformWorld[3][2];World._44 = (float)transformWorld[3][3];

				tJoint joint;
				joint.m_Name = name;
				joint.m_LocalTransform = localTransform;
				joint.m_WorldTransform = transformWorld;
				BindMesh.ThemBones.push_back(joint);
				
				MFnDagNode mNode(jointPath);
				bool isRoot = true;
				for(int i=0;i<(int)mNode.parentCount();++i)
				{
					MObject parent = mNode.parent(i);
					if(parent.apiType() == MFn::kJoint)
						isRoot = false;
				}
				if(isRoot)
					rootPath = jointPath;
				if(MAnimUtil::isAnimated(jointPath))
				{	
					std::pair<MDagPath,int> temp(jointPath,(int)BindMesh.ThemBones.size()-1);
					bonePaths.push_back(temp);
				}
				jointIter.next();
			}
			BuildSkeletonHierarchy(&BindMesh.RootCoreNode,rootPath,BindMesh);

			MPlug plug = PMesh.findPlug("inMesh",&status);
			MItDependencyGraph dgIt(plug,MFn::kInvalid,MItDependencyGraph::kUpstream,
					MItDependencyGraph::kDepthFirst, MItDependencyGraph::kPlugLevel, &status);
			if(status!=MStatus::kSuccess)
				continue;

			dgIt.disablePruningOnFilter();
			for(;!dgIt.isDone(); dgIt.next())
			{
				MObject skinObject = dgIt.thisNode();
				if(skinObject.apiType() != MFn::kSkinClusterFilter)
					continue;
				MFnSkinCluster skinCluster(skinObject );
				MItGeometry geoIter(currPath);
				for(;!geoIter.isDone();geoIter.next())
				{
					MFloatArray influences;
					unsigned int weigthCount=0;
					skinCluster.getWeights(currPath,geoIter.component(),influences, weigthCount);

					string testbone = currPath.fullPathName().asChar();
					for(int weigthNum=0;weigthNum<(int)weigthCount;++weigthNum)
					{
						if(influences[weigthNum] > .001)
						{
							tInfluence influence;
							influence.m_fWeight = influences[weigthNum];
							influence.m_nJointIndex = weigthNum;

							for(int vertCount=0;vertCount<(int)BindMesh.uniqueVerts.size();++vertCount)
							{
								if(BindMesh.uniqueVerts[vertCount].m_nMayaPointIndex == geoIter.index())
								{
									BindMesh.uniqueVerts[vertCount].m_vInfluences.push_back(influence);
								}
							}
						}
					}
				}
			}
			WriteMesh_XML(BindMesh);
			WriteMesh_Binary(BindMesh);
			BindMesh.ThemBones.clear();
			BindMesh.textureNames.clear();
			BindMesh.triList.clear();
			BindMesh.uniqueVerts.clear();
			BindMesh.RootCoreNode.m_nJointIndex=-1;
			BindMesh.RootCoreNode.m_vChildren.clear();
		}
		MSelectionList selectList;
		MGlobal::getActiveSelectionList(selectList);
		size_t selectLength = selectList.length();
		for(unsigned int selectIndex = 0; selectIndex < selectLength; selectIndex++)
		{
			MDagPath currentPath;
			selectList.getDagPath(selectIndex, currentPath);
			if(currentPath.apiType() == MFn::kTransform)
			{
				MFnTransform currentTransform(currentPath);
				unsigned int childCount = currentTransform.childCount();
				for(unsigned int childIndex = 0; childIndex < childCount; childIndex++)
				{
					MObject child = currentTransform.child(childIndex);
					if(child.apiType() == MFn::kMesh)
					{
						MFnDagNode childNode;
						childNode.setObject(child);
						MDagPath childPath;
						childNode.getPath(childPath);

						MFnMesh childMesh(childPath);
						if(childMesh.isIntermediateObject())
							continue;
						if(ExportAnim == false)
						{
							ExportAnim = true;
							TheAnimation.meshName = childMesh.name().asChar();
							GetBoneData(TheAnimation);
							BuildSkeletonHierarchy(&TheAnimation.RootCoreNode,TheAnimation.rootPath,TheAnimation);
							GetAnimInfo(TheAnimation);
						}
					}
				}
			}
		}
	}
}


bool CMeshExporter::GatherBindInformation(MDagPath currPath, MFnMesh PMesh, exMesh &outMesh)
{
	/*std::deque<std::pair<MDagPath,int>> bonePaths;
	MStatus status;
	MItDag jointIter(MItDag::kDepthFirst,MFn::kJoint,&status);
	while(!jointIter.isDone())
	{	
		MDagPath jointPath;
		jointIter.getPath(jointPath);
		MString name = jointPath.partialPathName();
		MFnTransform jointTransform(jointPath);
		MMatrix localTransform = jointTransform.transformationMatrix();
		MMatrix transformWorld = jointPath.inclusiveMatrix();
		tJoint joint;
		joint.m_Name = name;
		joint.m_LocalTransform = localTransform;
		joint.m_WorldTransform = transformWorld;
		outMesh.ThemBones.push_back(joint);
		MFnDagNode mNode(jointPath);
		bool isRoot = true;
		for(int i=0;i<(int)mNode.parentCount();++i)
		{
			MObject parent = mNode.parent(i);
			if(parent.apiType() == MFn::kJoint)
				isRoot = false;
		}
		if(isRoot)
			rootPath = jointPath;
		if(MAnimUtil::isAnimated(jointPath))
		{	
			std::pair<MDagPath,int> temp(jointPath,(int)outMesh.ThemBones.size()-1);
			bonePaths.push_back(temp);
		}
		jointIter.next();
	}
	BuildSkeletonHierarchy(&outMesh.RootCoreNode,rootPath,outMesh);
	MPlug plug = PMesh.findPlug("inMesh",&status);
	MItDependencyGraph dgIt(plug,MFn::kInvalid,MItDependencyGraph::kUpstream,
		MItDependencyGraph::kDepthFirst, MItDependencyGraph::kPlugLevel, &status);
	if(status!=MStatus::kSuccess)
		return false;
	dgIt.disablePruningOnFilter();
	for(;!dgIt.isDone(); dgIt.next())
	{
		MObject skinObject = dgIt.thisNode();
		if(skinObject.apiType() != MFn::kSkinClusterFilter)
			continue;
		MFnSkinCluster skinCluster(skinObject );
		MItGeometry geoIter(currPath);
		for(;!geoIter.isDone();geoIter.next())
		{
			MFloatArray influences;
			unsigned int weigthCount=0;
			skinCluster.getWeights(currPath,geoIter.component(),influences,weigthCount);

			for(int weigthNum=0;weigthNum<(int)weigthCount;++weigthNum)
			{
				if(influences[weigthNum] > .001)
				{
					tInfluence influence;
					influence.m_fWeight = influences[weigthNum];
					influence.m_nJointIndex = weigthNum;

					for(int vertCount=0;vertCount<(int)outMesh.uniqueVerts.size();++vertCount)
					{
						if(outMesh.uniqueVerts[vertCount].m_nMayaPointIndex == geoIter.index())
						{
							outMesh.uniqueVerts[vertCount].m_vInfluences.push_back(influence);
						}
					}
				}
			}
		}
	}
	for(int bonePathNum=0;bonePathNum<(int)bonePaths.size();++bonePathNum)
	{
		MPlugArray animPlugs;
		MAnimUtil::findAnimatedPlugs(bonePaths[bonePathNum].first,animPlugs);
		for(int animPlugNum=0;animPlugNum<(int)animPlugs.length();++animPlugNum)
		{
			MObjectArray curves;
			MAnimUtil::findAnimation(animPlugs[animPlugNum],curves);
			for(int curveNum=0;curveNum<(int)curves.length();++curveNum)
			{
				MFnAnimCurve curve(curves[curveNum]);
				int numKeyFrames = curve.numKeyframes();
				tKeyFrame keyFrame;
				MTime keyframeTime = curve.time(0);
				double keyTime = keyframeTime.as(MTime::kSeconds);
				keyFrame.m_dTimeInSeconds = keyTime;
				keyFrame.m_KeyTime = keyframeTime;
				SmartInsertKeyFrame(outMesh.ThemBones[bonePaths[bonePathNum].second].m_vKeyFrames,keyFrame);
			}
		}
	}
	bonePaths.clear();*/
	return true;
}
void CMeshExporter::GatherMeshInformation(MDagPath currPath, exMesh &outMesh)
{
	MFnDependencyNode dep(currPath.node());
	MStatus status;
	MPlug plug = dep.findPlug("LightMapName", false, &status);
	if(status==MStatus::kSuccess)
	{
		outMesh.LightMapName.attName = "LightMapName";
		plug.getValue(outMesh.LightMapName.attValue);
	}
	else
		outMesh.LightMapName.attName = "NULL";
	plug = dep.findPlug("Normal", false, &status);
	if(status==MStatus::kSuccess)
	{
		outMesh.NormalMapName.attName = "Normal";
		plug.getValue(outMesh.NormalMapName.attValue);
	}
	else
		outMesh.NormalMapName.attName = "NULL";
	plug = dep.findPlug("Specular", false, &status);
	if(status==MStatus::kSuccess)
	{
		outMesh.SpecMapName.attName = "Specular";
		plug.getValue(outMesh.SpecMapName.attValue);
	}
	else
		outMesh.SpecMapName.attName = "NULL";

	plug = dep.findPlug("Shader", false, &status);
	if(status==MStatus::kSuccess)
	{
		outMesh.ShaderName.attName = "Shader";
		plug.getValue(outMesh.ShaderName.attValue);
		if(outMesh.ShaderName.attValue.length() <= 2)
			outMesh.ShaderName.attValue = "Texture.fx";
	}
	else
	{
		outMesh.ShaderName.attName = "Shader";
		outMesh.ShaderName.attValue = "Texture.fx";
	}

	plug = dep.findPlug("Transparent", false, &status);
	if(status==MStatus::kSuccess)
	{
		outMesh.Transparent.attName = "Transparent";
		plug.getValue(outMesh.Transparent.attValue);
	}
	else
		outMesh.Transparent.attName = "NULL";
	plug = dep.findPlug("BackFaceCulling", false, &status);
	if(status==MStatus::kSuccess)
	{
		outMesh.BackFaceCulled.attName = "BackFaceCulling";
		plug.getValue(outMesh.BackFaceCulled.attValue);
	}
	else
		outMesh.BackFaceCulled.attName = "NULL";
	plug = dep.findPlug("Emissive", false, &status);
	if(status==MStatus::kSuccess)
	{
		outMesh.Emissive.attName = "Emissive";
		plug.getValue(outMesh.Emissive.attValue);
	}
	else
		outMesh.Emissive.attName = "NULL";
	plug = dep.findPlug("VolumeType", false, &status);
	if(status==MStatus::kSuccess)
	{
		outMesh.VolumeType.attName = "VolumeType";
		plug.getValue(outMesh.VolumeType.attValue);
	}
	else
		outMesh.VolumeType.attName = "NULL";
}
void CMeshExporter::ExportMesh(MFnMesh & currMesh, exMesh & outMesh)
{
	string cstr = currMesh.name().asChar();
	for(unsigned int i = 0; i < cstr.length(); ++i)
	{
		if(cstr[i] > 47 && cstr[i] < 58)
			cstr.erase(i,i);
	}
	char nameEnd[6];
	int nLetters = (int)cstr.length();
	unsigned int currLetter = nLetters - 5;
	nameEnd[0] = cstr[currLetter++];
	nameEnd[1] = cstr[currLetter++];
	nameEnd[2] = cstr[currLetter++];
	nameEnd[3] = cstr[currLetter++];
	nameEnd[4] = cstr[currLetter];
	nameEnd[5] = 0;
	if ( strcmp("Shape", nameEnd) == 0 )
		cstr.erase(cstr.length()-5,cstr.length());
	outMesh.meshName = cstr.c_str();
	outMesh.meshTrackBack = currMesh.fullPathName();
	MStringArray textureNames;
	unsigned int tnameCount = 0;
	getTextureNamesFromMesh(currMesh,textureNames,tnameCount);
	for(unsigned int _names = 0; _names < tnameCount; _names++)
	{
		std::string szDirectPath = textureNames[_names].asChar();
		size_t DPindex = szDirectPath.find_last_of('/');
		if(DPindex > 0)
			szDirectPath.erase(0, DPindex + 1);
		textureNames[_names].set(szDirectPath.c_str());
		outMesh.textureNames.push_back(textureNames[_names]);
		volatile int debug = textureNames[_names].length();
	}
	if(tnameCount == 0)
	{
		MString NoTexture = "NOTEXTURE";
		outMesh.textureNames.push_back(NoTexture);
	}
	MDagPath meshPath;
	currMesh.getPath( meshPath );
	MItMeshPolygon polyIt(meshPath);
	MPointArray vVerts;
	currMesh.getPoints(vVerts);
	MFloatVectorArray vNormals;
	currMesh.getNormals(vNormals);
	MFloatArray vU, vV;
	currMesh.getUVs(vU,vV);
	for(; !polyIt.isDone(); polyIt.next())
	{
		indexedTris triangle;
		for(int _vert = 0; _vert < 3; _vert++)
		{
			int pIndex = polyIt.vertexIndex(_vert);
			int nIndex = polyIt.normalIndex(_vert);
			int uvIndex;
			polyIt.getUVIndex( _vert, uvIndex );
			ed2Vert vTestVertex;
			vTestVertex.pos.x = (float)vVerts[pIndex].x;
			vTestVertex.pos.y = (float)vVerts[pIndex].y;
			vTestVertex.pos.z = (float)vVerts[pIndex].z;
			vTestVertex.normal.x = (float)vNormals[nIndex].x;
			vTestVertex.normal.y = (float)vNormals[nIndex].y;
			vTestVertex.normal.z = (float)vNormals[nIndex].z;
			vTestVertex.textureU = vU[uvIndex];
			vTestVertex.textureV = 1 - vV[uvIndex];
			if(polyIt.hasColor(_vert))
			{
				MColor vertColor;
				polyIt.getColor(vertColor,_vert);
				vTestVertex.color.x = vertColor.r;
				vTestVertex.color.y = vertColor.g;
				vTestVertex.color.z = vertColor.b;
				vTestVertex.color.w = vertColor.a;
			}
			else
			{
				vTestVertex.color.x = 1.0f;
				vTestVertex.color.y = 1.0f;
				vTestVertex.color.z = 1.0f;
				vTestVertex.color.w = 1.0f;
			}
			vTestVertex.m_nMayaPointIndex = pIndex;
			size_t uVerts = outMesh.uniqueVerts.size();
			bool isunique = true;
			for(size_t _uVerts = 0; _uVerts < uVerts; _uVerts++)
			{
				if(outMesh.uniqueVerts[_uVerts] == vTestVertex)
				{
					isunique = false;
					triangle.vertIndices[_vert] = (unsigned int)_uVerts;
					break;
				}
			}
			if (isunique)
			{
				outMesh.uniqueVerts.push_back(vTestVertex);
				triangle.vertIndices[_vert] = (unsigned int)(outMesh.uniqueVerts.size()-1);
			}
		}
		outMesh.triList.push_back(triangle);
	}
}
void CMeshExporter::WriteMesh_Binary( const exMesh& writeOutMesh )
{
	string cstr = writeOutMesh.meshName.asChar();
	cstr += ".mesh";
	std::fstream fstr;
	fstr.open(cstr.c_str(),std::ios_base::out|std::ios_base::trunc|std::ios_base::binary);
	if(!fstr.is_open())
	{
		return;
	}
	size_t nNamelen = (writeOutMesh.meshName.length()+1);
	fstr.write((char*)&nNamelen,sizeof(size_t));
	fstr.write((char*)writeOutMesh.meshName.asChar(),sizeof(char)*(std::streamsize)nNamelen);
	size_t nNumTex = (size_t)writeOutMesh.textureNames.size();
	fstr.write((char*)&nNumTex,sizeof(size_t));
	for (unsigned int Texcount = 0; Texcount < nNumTex; ++Texcount)
	{
		size_t nTNamelen = (writeOutMesh.textureNames[Texcount].length()+1);
		fstr.write((char*)&nTNamelen,sizeof(size_t));
		fstr.write(writeOutMesh.textureNames[Texcount].asChar(),sizeof(char)*(std::streamsize)nTNamelen);
	}
	fstr.write((char*)&writeOutMesh.bDynamic,sizeof(bool));
	//ATTRIBUTESHIT
	//LightMap
	size_t nAttNamelen;
	if(strcmp(writeOutMesh.LightMapName.attName.asChar(), "NULL") == 0)
		nAttNamelen = 0;
	else 
		nAttNamelen = (writeOutMesh.LightMapName.attValue.length()+1);
	fstr.write((char*)&nAttNamelen,sizeof(size_t));
	fstr.write((char*)writeOutMesh.LightMapName.attValue.asChar(),sizeof(char)*(std::streamsize)nAttNamelen);
	
	//NormalMapName
	if(strcmp(writeOutMesh.NormalMapName.attName.asChar(), "NULL") == 0)
		nAttNamelen = 0;
	else
		nAttNamelen = (writeOutMesh.NormalMapName.attValue.length()+1);
	fstr.write((char*)&nAttNamelen,sizeof(size_t));
	fstr.write((char*)writeOutMesh.NormalMapName.attValue.asChar(),sizeof(char)*(std::streamsize)nAttNamelen);
	
	//SpecMapName
	if(strcmp(writeOutMesh.SpecMapName.attName.asChar(), "NULL") == 0)
		nAttNamelen = 0;
	else
		nAttNamelen = (writeOutMesh.SpecMapName.attValue.length()+1);
	fstr.write((char*)&nAttNamelen,sizeof(size_t));
	fstr.write((char*)writeOutMesh.SpecMapName.attValue.asChar(),sizeof(char)*(std::streamsize)nAttNamelen);
	
	//ShaderName
	if(strcmp(writeOutMesh.ShaderName.attName.asChar(), "NULL") == 0)
		nAttNamelen = 0;
	else 
		nAttNamelen = (writeOutMesh.ShaderName.attValue.length()+1);
	fstr.write((char*)&nAttNamelen,sizeof(size_t));
	fstr.write((char*)writeOutMesh.ShaderName.attValue.asChar(),sizeof(char)*(std::streamsize)nAttNamelen);
	
	//Transparent
	if(strcmp(writeOutMesh.Transparent.attName.asChar(), "NULL") == 0)
		(bool)writeOutMesh.Transparent.attValue = 0;
	fstr.write((char*)&writeOutMesh.Transparent.attValue,sizeof(char)*(std::streamsize)sizeof(bool));
	
	//BackFaceCulled
	if(strcmp(writeOutMesh.BackFaceCulled.attName.asChar(), "NULL") == 0)
		(bool)writeOutMesh.BackFaceCulled.attValue = 0;
	fstr.write((char*)&writeOutMesh.BackFaceCulled.attValue,sizeof(char)*(std::streamsize)sizeof(bool));
	
	//Emissive
	if(strcmp(writeOutMesh.Emissive.attName.asChar(), "NULL") == 0)
		(bool)writeOutMesh.Emissive.attValue = 0;
	fstr.write((char*)&writeOutMesh.Emissive.attValue,sizeof(char)*(std::streamsize)sizeof(bool));
	
	//VolumeType
	if(strcmp(writeOutMesh.VolumeType.attName.asChar(), "NULL") == 0)
		(int)writeOutMesh.VolumeType.attValue = 0;
	fstr.write((char*)&writeOutMesh.VolumeType.attValue,sizeof(char)*(std::streamsize)sizeof(int));


	size_t nNumVerts = (size_t)writeOutMesh.uniqueVerts.size();
	fstr.write((char*)&nNumVerts,sizeof(size_t));
	for (unsigned int Vertcount = 0; Vertcount < nNumVerts; ++Vertcount)
	{
		//Position
		fstr.write((char*)&writeOutMesh.uniqueVerts[Vertcount].pos.x, sizeof(float));
		fstr.write((char*)&writeOutMesh.uniqueVerts[Vertcount].pos.y, sizeof(float));
		fstr.write((char*)&writeOutMesh.uniqueVerts[Vertcount].pos.z, sizeof(float));
		//Normal
		fstr.write((char*)&writeOutMesh.uniqueVerts[Vertcount].normal.x, sizeof(float));
		fstr.write((char*)&writeOutMesh.uniqueVerts[Vertcount].normal.y, sizeof(float));
		fstr.write((char*)&writeOutMesh.uniqueVerts[Vertcount].normal.z, sizeof(float));
		//Tex Coord
		fstr.write((char*)&writeOutMesh.uniqueVerts[Vertcount].textureU, sizeof(float));
		fstr.write((char*)&writeOutMesh.uniqueVerts[Vertcount].textureV, sizeof(float));
		//Color
		fstr.write((char*)&writeOutMesh.uniqueVerts[Vertcount].color.x, sizeof(float));
		fstr.write((char*)&writeOutMesh.uniqueVerts[Vertcount].color.y, sizeof(float));
		fstr.write((char*)&writeOutMesh.uniqueVerts[Vertcount].color.z, sizeof(float));
		fstr.write((char*)&writeOutMesh.uniqueVerts[Vertcount].color.w, sizeof(float));
		if(writeOutMesh.bDynamic)
		{
			// Influences
			unsigned int numInfluence = (unsigned int)writeOutMesh.uniqueVerts[Vertcount].m_vInfluences.size();
			fstr.write((char*)&numInfluence, sizeof(unsigned int));
			for(size_t iInfluence = 0; iInfluence < numInfluence; iInfluence++)
			{
				fstr.write((char*)&writeOutMesh.uniqueVerts[Vertcount].m_vInfluences[iInfluence].m_nJointIndex, sizeof(unsigned int));
				fstr.write((char*)&writeOutMesh.uniqueVerts[Vertcount].m_vInfluences[iInfluence].m_fWeight, sizeof(float));
			}
		}
	}
	size_t nNumTris = (size_t)writeOutMesh.triList.size();
	fstr.write((char*)&nNumTris,sizeof(size_t));
	for (unsigned int Tricount = 0; Tricount < nNumTris; ++Tricount)
	{
		fstr.write((char*)&writeOutMesh.triList[Tricount],sizeof(indexedTris));
	}
	if(writeOutMesh.bDynamic)
	{
		// Bones
		size_t numBones = writeOutMesh.ThemBones.size();
		fstr.write((char*)&numBones, sizeof(size_t));
		unsigned int namelen = 0;
		unsigned int parentInd = 0;
		size_t childCount = 0;
		for(unsigned int iBones = 0; iBones < numBones; ++iBones)
		{
			fstr.write((char*)&iBones, sizeof(size_t));
			namelen = writeOutMesh.ThemBones[iBones].m_Name.length()+1;
			fstr.write((char*)&namelen, sizeof(unsigned int));
			fstr.write(writeOutMesh.ThemBones[iBones].m_Name.asChar(), namelen);
			parentInd = GetParentByIndex((unsigned int)iBones, (tSkeletonNode&)writeOutMesh.RootCoreNode);
			fstr.write((char*)&parentInd, sizeof(unsigned int));
			for(unsigned int irows = 0; irows < 4; irows++)
			{
				for(unsigned int icols = 0; icols < 4; icols++)
				{
					float matfloat = (float)writeOutMesh.ThemBones[iBones].m_WorldTransform[irows][icols];
					fstr.write((char*)&matfloat, sizeof(float));
				}
			}
			tSkeletonNode* pCurrSkel = 0;
			pCurrSkel = FindSkeletonNodeByIndex((unsigned int)iBones, (tSkeletonNode&)writeOutMesh.RootCoreNode);
			size_t numChildren;
			if(pCurrSkel)
				numChildren = pCurrSkel->m_vChildren.size();
			else
				numChildren = 0;
			fstr.write((char*)&numChildren,sizeof(size_t));
			for(size_t iChild = 0; iChild < numChildren; iChild++)
			{
				fstr.write((char*)&pCurrSkel->m_vChildren[iChild].m_nJointIndex, sizeof(unsigned int));
				unsigned int childLen = writeOutMesh.ThemBones[pCurrSkel->m_vChildren[iChild].m_nJointIndex].m_Name.length()+1;
				fstr.write((char*)&childLen, sizeof(unsigned int));
				fstr.write(writeOutMesh.ThemBones[pCurrSkel->m_vChildren[iChild].m_nJointIndex].m_Name.asChar(), childLen);
			}
		}
	}
	fstr.close();
	fstr.clear();
}
void CMeshExporter::WriteOutMeshes( bool bWriteBinary )
{
	for(size_t i = 0; i < m_exportedMeshes.size(); ++i)
	{
		WriteMesh_XML( m_exportedMeshes[i] );
		if(bWriteBinary)
			WriteMesh_Binary( m_exportedMeshes[i] );
	}
}
void CMeshExporter::WriteAnimations( const MFileObject& file, bool bWriteBinary )
{
	if(ExportAnim)
	{
		TheAnimation.meshName = file.fullName().asChar();
		size_t DPindex = TheAnimation.meshName.find_last_of('/');
		if(DPindex > 0)
			TheAnimation.meshName.erase(0, DPindex + 1);
		if(bWriteBinary)
			WriteAnimBinary( TheAnimation );
		TheAnimation.meshName += ".xml";
		WriteAnimXML( TheAnimation );
	}
}
void CMeshExporter::WriteMesh_XML( const exMesh& writeOutMesh )
{
	//must be the first thing written out to xml file, makes xml file legal
	char XMLProlog[] = "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n";

	char buffer[256];

	std::string fileName = writeOutMesh.meshName.asChar();
	fileName += ".xml";

	std::ofstream xmlOut;
	//open xml file
	xmlOut.open( fileName.c_str(), std::ios_base::trunc);

	//write out prolog
	xmlOut << XMLProlog;

	//write out the root element <Model>
	xmlOut << "<Model>\n";
	//the mesh element with name attribute <Mesh name="">
	xmlOut << "<Mesh name=\"" << writeOutMesh.meshName.asChar() << "\">\n";
	//the trackback <TrackBack>
	xmlOut << "<TrackBack>" << writeOutMesh.meshTrackBack.asChar() << "</TrackBack>\n";
	//VertCount
	xmlOut << "<VertCount>" << writeOutMesh.uniqueVerts.size() << "</VertCount>\n";
	//PolyCount
	xmlOut << "<PolyCount>" << writeOutMesh.triList.size() << "</PolyCount>\n";
	
	//the textures
	if(writeOutMesh.textureNames.size())
	{
		for(size_t currTex = 0; currTex < writeOutMesh.textureNames.size(); ++currTex)
		{
			std::string tempTex = writeOutMesh.textureNames[currTex].asChar();
			size_t index = tempTex.find_last_of('/');
			
			if(index > 0)
				tempTex.erase(0, index + 1);

			xmlOut << "<Texture>" << tempTex.c_str() << "</Texture>\n";
		}
	}
	else
		xmlOut << "<Texture/>\n";

	xmlOut << "<Dynamic>" << writeOutMesh.bDynamic << "</Dynamic>";

	xmlOut << "<Attributes>\n";
	////ATTRIBUTESHIT
	////LightMap
	xmlOut << "<LightMap>";
	if(strcmp(writeOutMesh.LightMapName.attName.asChar(), "NULL") == 0)
		xmlOut << 0;
	else 
		xmlOut << writeOutMesh.LightMapName.attValue.asChar();
	xmlOut << "</LightMap>";

	////NormalMapName
	xmlOut << "<NormalMap>";
	if(strcmp(writeOutMesh.NormalMapName.attName.asChar(), "NULL") == 0)
		xmlOut << 0;
	else 
		xmlOut << writeOutMesh.NormalMapName.attValue.asChar();
	xmlOut << "</NormalMap>";

	////SpecMapName
	xmlOut << "<SpecMap>";
	if(strcmp(writeOutMesh.SpecMapName.attName.asChar(), "NULL") == 0)
		xmlOut << 0;
	else 
		xmlOut << writeOutMesh.SpecMapName.attValue.asChar();
	xmlOut << "</SpecMap>";

	////ShaderName
	xmlOut << "<Shader>";
	if(strcmp(writeOutMesh.ShaderName.attName.asChar(), "NULL") == 0)
		xmlOut << 0;
	else 
		xmlOut << writeOutMesh.ShaderName.attValue.asChar();
	xmlOut << "</Shader>";

	////Transparent
	xmlOut << "<Transparent>";
	if(strcmp(writeOutMesh.Transparent.attName.asChar(), "NULL") == 0)
		xmlOut << 0;
	else 
		xmlOut << writeOutMesh.Transparent.attValue;
	xmlOut << "</Transparent>";

	////BackFaceCulled
	xmlOut << "<BackFaceCulled>";
	if(strcmp(writeOutMesh.BackFaceCulled.attName.asChar(), "NULL") == 0)
		xmlOut << 0;
	else 
		xmlOut << writeOutMesh.BackFaceCulled.attValue;
	xmlOut << "</BackFaceCulled>";

	////Emissive
	xmlOut << "<Emissive>";
	if(strcmp(writeOutMesh.Emissive.attName.asChar(), "NULL") == 0)
		xmlOut << 0;
	else 
		xmlOut << writeOutMesh.Emissive.attValue;
	xmlOut << "</Emissive>";

	////VolumeType
	xmlOut << "<VolumeType>";
	if(strcmp(writeOutMesh.VolumeType.attName.asChar(), "NULL") == 0)
		xmlOut << 0;
	else 
		xmlOut << writeOutMesh.VolumeType.attValue;
	xmlOut << "</VolumeType>";


	xmlOut << "</Attributes>\n";


	//write out the vertList
	xmlOut << "<vertList>\n";

		//loop through verts <Vert id=""><x></x><y></y><z></z><Nx></Nx><Ny></Ny><Nz></Nz><Tu></Tu><Tv></Tv></Vert>
		for(size_t currVert = 0; currVert < writeOutMesh.uniqueVerts.size(); ++currVert)
		{
			xmlOut << "<Vert id=\"" << currVert << "\">\n";
			memset(buffer, 0, 256);
			sprintf_s(buffer, "<x>%f</x><y>%f</y><z>%f</z><Nx>%f</Nx><Ny>%f</Ny><Nz>%f</Nz><Tu>%f</Tu><Tv>%f</Tv>\n<Cr>%f</Cr><Cg>%f</Cg><Cb>%f</Cb><Ca>%f</Ca>\n",
						writeOutMesh.uniqueVerts[currVert].pos.x, writeOutMesh.uniqueVerts[currVert].pos.y, writeOutMesh.uniqueVerts[currVert].pos.z,
						writeOutMesh.uniqueVerts[currVert].normal.x, writeOutMesh.uniqueVerts[currVert].normal.y, writeOutMesh.uniqueVerts[currVert].normal.z, 
						writeOutMesh.uniqueVerts[currVert].textureU, writeOutMesh.uniqueVerts[currVert].textureV,writeOutMesh.uniqueVerts[currVert].color.x,
						writeOutMesh.uniqueVerts[currVert].color.y, writeOutMesh.uniqueVerts[currVert].color.z, writeOutMesh.uniqueVerts[currVert].color.w);
			xmlOut << buffer;

			//write out influences
			if(writeOutMesh.bDynamic)
			{
				xmlOut << "<influences>\n";
				int InfNum = (int)writeOutMesh.uniqueVerts[currVert].m_vInfluences.size();
				xmlOut << InfNum;
				for(size_t currInf = 0; currInf < writeOutMesh.uniqueVerts[currVert].m_vInfluences.size(); ++currInf)
				{
					xmlOut << "<influence>\n";

					memset(buffer, 0, 256);

					sprintf_s(buffer, "<index>%d</index>\n<weight>%f</weight>\n", writeOutMesh.uniqueVerts[currVert].m_vInfluences[currInf].m_nJointIndex,
						writeOutMesh.uniqueVerts[currVert].m_vInfluences[currInf].m_fWeight);

					xmlOut << buffer;

					xmlOut << "</influence>\n";
				}
			}

			//close influences
			xmlOut << "</influences>\n";
			xmlOut << "</Vert>";
		}

	//close the vertList
	xmlOut << "</vertList>\n";

	//open polyList
	xmlOut << "<polyList>\n";
	
	//loop through triangles and write out indices <Polygon><i1></i1><i2></i2><i3></i3></Polygon>
	for(size_t currTri = 0; currTri < writeOutMesh.triList.size(); ++currTri)
	{
		xmlOut << "<Polygon>\n";
		
		xmlOut << "<i1>" << writeOutMesh.triList[currTri].index1 << "</i1><i2>" << writeOutMesh.triList[currTri].index2 << "</i2><i3>" << writeOutMesh.triList[currTri].index3 << "</i3>\n";

		xmlOut << "</Polygon>\n";
	}

	//close polyList
	xmlOut << "</polyList>\n";

	//close the mesh element
	xmlOut << "</Mesh>\n";
	if(writeOutMesh.bDynamic)
	{
		//open the skeleton element
		xmlOut << "<Skeleton numBones=\"" << writeOutMesh.ThemBones.size() << "\">\n";

		//for each bone
		for(unsigned int currBone = 0; currBone < writeOutMesh.ThemBones.size(); ++currBone )
		{
			xmlOut << "<bone>\n";

			memset(buffer,0,256);

			//find parent index
			int parentInt = GetParentByIndex((unsigned int)currBone, (tSkeletonNode&)writeOutMesh.RootCoreNode);

			sprintf_s(buffer, "<name index=\"%d\">%s</name>\n<parentIndex>%d</parentIndex>\n", currBone,
				writeOutMesh.ThemBones[currBone].m_Name.asChar(), parentInt);
			xmlOut << buffer;
			//matrix time
			xmlOut << "<localTransform>\n";

			//x axis
			memset(buffer, 0, 256);
			sprintf_s(buffer, "<xAxis><x>%f</x><y>%f</y><z>%f</z><w>%f</w></xAxis>\n", 
				writeOutMesh.ThemBones[currBone].m_LocalTransform[0][0], 
				writeOutMesh.ThemBones[currBone].m_LocalTransform[0][1], 
				writeOutMesh.ThemBones[currBone].m_LocalTransform[0][2], 
				writeOutMesh.ThemBones[currBone].m_LocalTransform[0][3]);
			xmlOut << buffer;

			//y axis
			memset(buffer, 0, 256);
			sprintf_s(buffer, "<yAxis><x>%f</x><y>%f</y><z>%f</z><w>%f</w></yAxis>\n", 
				writeOutMesh.ThemBones[currBone].m_LocalTransform[1][0], 
				writeOutMesh.ThemBones[currBone].m_LocalTransform[1][1], 
				writeOutMesh.ThemBones[currBone].m_LocalTransform[1][2], 
				writeOutMesh.ThemBones[currBone].m_LocalTransform[1][3]);
			xmlOut << buffer;

			//z axis
			memset(buffer, 0, 256);
			sprintf_s(buffer, "<zAxis><x>%f</x><y>%f</y><z>%f</z><w>%f</w></zAxis>\n", 
				writeOutMesh.ThemBones[currBone].m_LocalTransform[2][0], 
				writeOutMesh.ThemBones[currBone].m_LocalTransform[2][1], 
				writeOutMesh.ThemBones[currBone].m_LocalTransform[2][2], 
				writeOutMesh.ThemBones[currBone].m_LocalTransform[2][3]);
			xmlOut << buffer;

			//w axis
			memset(buffer, 0, 256);
			sprintf_s(buffer, "<wAxis><x>%f</x><y>%f</y><z>%f</z><w>%f</w></wAxis>\n",
				writeOutMesh.ThemBones[currBone].m_LocalTransform[3][0], 
				writeOutMesh.ThemBones[currBone].m_LocalTransform[3][1], 
				writeOutMesh.ThemBones[currBone].m_LocalTransform[3][2], 
				writeOutMesh.ThemBones[currBone].m_LocalTransform[3][3]);
			xmlOut << buffer;

			xmlOut << "</localTransform>\n";

			xmlOut << "<WorldTransform>\n";

			//x axis
			memset(buffer, 0, 256);
			sprintf_s(buffer, "<xAxis><x>%f</x><y>%f</y><z>%f</z><w>%f</w></xAxis>\n", 
				writeOutMesh.ThemBones[currBone].m_WorldTransform[0][0], 
				writeOutMesh.ThemBones[currBone].m_WorldTransform[0][1], 
				writeOutMesh.ThemBones[currBone].m_WorldTransform[0][2], 
				writeOutMesh.ThemBones[currBone].m_WorldTransform[0][3]);
			xmlOut << buffer;

			//y axis
			memset(buffer, 0, 256);
			sprintf_s(buffer, "<yAxis><x>%f</x><y>%f</y><z>%f</z><w>%f</w></yAxis>\n", 
				writeOutMesh.ThemBones[currBone].m_WorldTransform[1][0], 
				writeOutMesh.ThemBones[currBone].m_WorldTransform[1][1], 
				writeOutMesh.ThemBones[currBone].m_WorldTransform[1][2], 
				writeOutMesh.ThemBones[currBone].m_WorldTransform[1][3]);
			xmlOut << buffer;

			//z axis
			memset(buffer, 0, 256);
			sprintf_s(buffer, "<zAxis><x>%f</x><y>%f</y><z>%f</z><w>%f</w></zAxis>\n", 
				writeOutMesh.ThemBones[currBone].m_WorldTransform[2][0], 
				writeOutMesh.ThemBones[currBone].m_WorldTransform[2][1], 
				writeOutMesh.ThemBones[currBone].m_WorldTransform[2][2], 
				writeOutMesh.ThemBones[currBone].m_WorldTransform[2][3]);
			xmlOut << buffer;

			//w axis
			memset(buffer, 0, 256);
			sprintf_s(buffer, "<wAxis><x>%f</x><y>%f</y><z>%f</z><w>%f</w></wAxis>\n",
				writeOutMesh.ThemBones[currBone].m_WorldTransform[3][0], 
				writeOutMesh.ThemBones[currBone].m_WorldTransform[3][1], 
				writeOutMesh.ThemBones[currBone].m_WorldTransform[3][2], 
				writeOutMesh.ThemBones[currBone].m_WorldTransform[3][3]);
			xmlOut << buffer;

			xmlOut << "</WorldTransform>\n";

			//children
			xmlOut << "<children>\n";

			tSkeletonNode * pCurrNode = 0;
			pCurrNode = FindSkeletonNodeByIndex( (unsigned int)currBone, (tSkeletonNode&)writeOutMesh.RootCoreNode);
			for(size_t currChild = 0; currChild < pCurrNode->m_vChildren.size(); ++currChild)
			{
				xmlOut << "<child index=\"" << pCurrNode->m_vChildren[currChild].m_nJointIndex << "\">";
				xmlOut << writeOutMesh.ThemBones[pCurrNode->m_vChildren[currChild].m_nJointIndex].m_Name.asChar() << "</child>\n";
			}

			xmlOut << "</children>\n";

			xmlOut << "</bone>\n";
		}

		//close skeleton
		xmlOut << "</Skeleton>\n";
	}
	//close the root element <Model>
	xmlOut << "</Model>\n";
	xmlOut.close();
	xmlOut.clear();
}


void CMeshExporter::getTextureNamesFromMesh( MFnMesh &mesh, MStringArray &names, unsigned &count )
{
	// local vars
	MStatus         status;
	MObjectArray    meshSets;
	MObjectArray    meshComps;
	unsigned        meshSetCount = 0;
	mesh.getConnectedSetsAndMembers( 0, meshSets, meshComps, true );
	meshSetCount = meshSets.length();
	if ( meshSetCount > 1 ) meshSetCount--;
	names.clear();
	count = 0;
	for ( unsigned i = 0; i < meshSetCount; i++ )
	{
		MObject set  = meshSets[i];
		MObject comp = meshComps[i];
		MFnSet  fnSet( set );
		MItMeshPolygon meshSetIterator( mesh.dagPath(), comp, &status );
		if ( !status ) continue;
		MFnDependencyNode   fnNode( set ); 
		MPlugArray          connectedPlugs;            
		MPlug shaderPlug = fnNode.findPlug( "surfaceShader", &status );
		if ( !status ) continue;
		if ( shaderPlug.isNull() ) continue;
		shaderPlug.connectedTo( connectedPlugs, true, false, &status );
		if ( !status ) continue;
		if ( connectedPlugs.length() != 1 ) continue;
		MPlug colorPlug = MFnDependencyNode( connectedPlugs[0].node() ).findPlug("color", &status);
		if ( !status ) continue;
		MItDependencyGraph itDG( colorPlug, MFn::kFileTexture, MItDependencyGraph::kUpstream, MItDependencyGraph::kBreadthFirst, MItDependencyGraph::kNodeLevel, &status );
		if ( !status ) continue;
		itDG.disablePruningOnFilter();
		if ( itDG.isDone() ) continue;
		MObject textureNode = itDG.thisNode();
		MPlug   filePlug    = MFnDependencyNode( textureNode ).findPlug( "fileTextureName" );
		MString textureName( "" );
		if ( filePlug.getValue( textureName ) && textureName != "" )
		{
			names.append( textureName );            
			count++;
		}		
	}
}

void CMeshExporter::SmartInsertKeyFrame(std::vector<tKeyFrame> &myKeys, tKeyFrame keyToAdd)
{
	std::vector<tKeyFrame>::iterator keyIt = myKeys.begin();
	for( ; keyIt != myKeys.end(); keyIt++ )
	{
		if( (*keyIt).m_KeyTime == keyToAdd.m_KeyTime )
			return;
		if( keyToAdd.m_KeyTime < (*keyIt).m_KeyTime )
		{
			myKeys.insert(keyIt, keyToAdd);
			return;
		}
	}
	myKeys.push_back( keyToAdd );
}
int CMeshExporter::GetParentByIndex(unsigned index, tSkeletonNode & currNode)
{
	for(size_t currChild = 0; currChild < currNode.m_vChildren.size(); ++currChild)
	{
		if( index == currNode.m_vChildren[currChild].m_nJointIndex)
			return currNode.m_nJointIndex;
	}
	int parentIndex = -1;
	for(size_t currChild = 0; currChild < currNode.m_vChildren.size(); ++currChild)
	{
		parentIndex = GetParentByIndex( index, currNode.m_vChildren[currChild] );


		if(parentIndex != -1)
			return parentIndex;
	}	
	return -1;
}
tSkeletonNode * CMeshExporter::FindSkeletonNodeByIndex(const unsigned boneIndex, tSkeletonNode& currNode)
{
	//are we the node?
	if( boneIndex == currNode.m_nJointIndex)
		return &currNode;

	//go through kids
	for(size_t currKid = 0; currKid < currNode.m_vChildren.size(); ++currKid)
	{
		tSkeletonNode * pTempNode = 0;

		pTempNode = FindSkeletonNodeByIndex( boneIndex, currNode.m_vChildren[currKid] );

		if(pTempNode)
			return pTempNode;
	}

	return 0;
}
void CMeshExporter::WriteAnimXML(const Animation& writeOutAnim )
{
	char XMLProlog[] = "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n";
	char buffer[256];
	std::ofstream xmlOut;
	xmlOut.open(writeOutAnim.meshName.c_str(), std::ios_base::trunc);

	if( !xmlOut.is_open() )
		return;

	xmlOut << XMLProlog;

	string szAnimName = writeOutAnim.meshName;

	int index = (int)szAnimName.find_last_of("/");

	szAnimName.erase(0, index+1);

	szAnimName.resize(szAnimName.length() - 4);

	float animationDuration = 0.0;
	if(writeOutAnim.ThemBones.size())
	{
		for(unsigned int i = 0; i < writeOutAnim.ThemBones.size(); ++i)
		{
			if(writeOutAnim.ThemBones[i].m_vKeyFrames.size())
			{
				animationDuration = (float)(writeOutAnim.ThemBones[i].m_vKeyFrames[writeOutAnim.ThemBones[i].m_vKeyFrames.size() - 1].m_KeyTime.as(MTime::kSeconds));
				break;
			}
		}
	}
	//xmlOut << "<Mesh name=\"" << meshName.c_str() << "\">\n";
	xmlOut << "<Animation name=\"" << szAnimName.c_str() << "\">\n";
	xmlOut << "<Duration>" << animationDuration << "</Duration>\n";

	//for each bone's key frames
	for(unsigned int currBone = 0; currBone < writeOutAnim.ThemBones.size(); ++currBone )
	{
		xmlOut << "<animBone boneIndex=\"" << currBone << "\">\n";
		xmlOut << "<Bone Name=\"" << writeOutAnim.ThemBones[currBone].m_Name << "\">\n";
		xmlOut << "<KeyFrames numKeys=\"" << writeOutAnim.ThemBones[currBone].m_vKeyFrames.size()-1 << "\">\n";

		//for each keyframe
		for( size_t currKey = 1; currKey < writeOutAnim.ThemBones[currBone].m_vKeyFrames.size(); ++currKey )
		{
			//This is the actuall first animation keytime because the first keyframe of the models
			//we are exporting is the bindpose
			double firstKeyTime = writeOutAnim.ThemBones[currBone].m_vKeyFrames[1].m_KeyTime.as(MTime::kSeconds);
			xmlOut << "<keyFrame num=\"" << currKey-1 << "\">\n";
			xmlOut << "<keyTime>" << writeOutAnim.ThemBones[currBone].m_vKeyFrames[currKey].m_KeyTime.as(MTime::kSeconds)-firstKeyTime << "</keyTime>\n";

			//matrix at this time
			xmlOut << "<localTransform>\n";

			//x axis
			memset(buffer, 0, 256);
			sprintf_s(buffer, "<xAxis><x>%f</x><y>%f</y><z>%f</z><w>%f</w></xAxis>\n",
				writeOutAnim.ThemBones[currBone].m_vKeyFrames[currKey].m_LocalTransform[0][0],
				writeOutAnim.ThemBones[currBone].m_vKeyFrames[currKey].m_LocalTransform[0][1],
				writeOutAnim.ThemBones[currBone].m_vKeyFrames[currKey].m_LocalTransform[0][2],
				writeOutAnim.ThemBones[currBone].m_vKeyFrames[currKey].m_LocalTransform[0][3]);
			xmlOut << buffer;

			//y axis
			memset(buffer, 0, 256);
			sprintf_s(buffer, "<yAxis><x>%f</x><y>%f</y><z>%f</z><w>%f</w></yAxis>\n",
				writeOutAnim.ThemBones[currBone].m_vKeyFrames[currKey].m_LocalTransform[1][0],
				writeOutAnim.ThemBones[currBone].m_vKeyFrames[currKey].m_LocalTransform[1][1],
				writeOutAnim.ThemBones[currBone].m_vKeyFrames[currKey].m_LocalTransform[1][2],
				writeOutAnim.ThemBones[currBone].m_vKeyFrames[currKey].m_LocalTransform[1][3]);
			xmlOut << buffer;

			//z axis
			memset(buffer, 0, 256);
			sprintf_s(buffer, "<zAxis><x>%f</x><y>%f</y><z>%f</z><w>%f</w></zAxis>\n",
				writeOutAnim.ThemBones[currBone].m_vKeyFrames[currKey].m_LocalTransform[2][0],
				writeOutAnim.ThemBones[currBone].m_vKeyFrames[currKey].m_LocalTransform[2][1],
				writeOutAnim.ThemBones[currBone].m_vKeyFrames[currKey].m_LocalTransform[2][2],
				writeOutAnim.ThemBones[currBone].m_vKeyFrames[currKey].m_LocalTransform[2][3]);
			xmlOut << buffer;

			//w axis
			memset(buffer, 0, 256);
			sprintf_s(buffer, "<wAxis><x>%f</x><y>%f</y><z>%f</z><w>%f</w></wAxis>\n",
				writeOutAnim.ThemBones[currBone].m_vKeyFrames[currKey].m_LocalTransform[3][0],
				writeOutAnim.ThemBones[currBone].m_vKeyFrames[currKey].m_LocalTransform[3][1],
				writeOutAnim.ThemBones[currBone].m_vKeyFrames[currKey].m_LocalTransform[3][2],
				writeOutAnim.ThemBones[currBone].m_vKeyFrames[currKey].m_LocalTransform[3][3]);
			xmlOut << buffer;

			xmlOut << "</localTransform>\n";

			//matrix at this time
			xmlOut << "<worldTransform>\n";

			//x axis
			memset(buffer, 0, 256);
			sprintf_s(buffer, "<xAxis><x>%f</x><y>%f</y><z>%f</z><w>%f</w></xAxis>\n",
				writeOutAnim.ThemBones[currBone].m_vKeyFrames[currKey].m_WorldTransform[0][0],
				writeOutAnim.ThemBones[currBone].m_vKeyFrames[currKey].m_WorldTransform[0][1],
				writeOutAnim.ThemBones[currBone].m_vKeyFrames[currKey].m_WorldTransform[0][2],
				writeOutAnim.ThemBones[currBone].m_vKeyFrames[currKey].m_WorldTransform[0][3]);
			xmlOut << buffer;

			//y axis
			memset(buffer, 0, 256);
			sprintf_s(buffer, "<yAxis><x>%f</x><y>%f</y><z>%f</z><w>%f</w></yAxis>\n",
				writeOutAnim.ThemBones[currBone].m_vKeyFrames[currKey].m_WorldTransform[1][0],
				writeOutAnim.ThemBones[currBone].m_vKeyFrames[currKey].m_WorldTransform[1][1],
				writeOutAnim.ThemBones[currBone].m_vKeyFrames[currKey].m_WorldTransform[1][2],
				writeOutAnim.ThemBones[currBone].m_vKeyFrames[currKey].m_WorldTransform[1][3]);
			xmlOut << buffer;

			//z axis
			memset(buffer, 0, 256);
			sprintf_s(buffer, "<zAxis><x>%f</x><y>%f</y><z>%f</z><w>%f</w></zAxis>\n",
				writeOutAnim.ThemBones[currBone].m_vKeyFrames[currKey].m_WorldTransform[2][0],
				writeOutAnim.ThemBones[currBone].m_vKeyFrames[currKey].m_WorldTransform[2][1],
				writeOutAnim.ThemBones[currBone].m_vKeyFrames[currKey].m_WorldTransform[2][2],
				writeOutAnim.ThemBones[currBone].m_vKeyFrames[currKey].m_WorldTransform[2][3]);
			xmlOut << buffer;

			//w axis
			memset(buffer, 0, 256);
			sprintf_s(buffer, "<wAxis><x>%f</x><y>%f</y><z>%f</z><w>%f</w></wAxis>\n",
				writeOutAnim.ThemBones[currBone].m_vKeyFrames[currKey].m_WorldTransform[3][0],
				writeOutAnim.ThemBones[currBone].m_vKeyFrames[currKey].m_WorldTransform[3][1],
				writeOutAnim.ThemBones[currBone].m_vKeyFrames[currKey].m_WorldTransform[3][2],
				writeOutAnim.ThemBones[currBone].m_vKeyFrames[currKey].m_WorldTransform[3][3]);
			xmlOut << buffer;

			xmlOut << "</worldTransform>\n";

			xmlOut << "</keyFrame>\n";
		}
		xmlOut << "</KeyFrames>\n";
		xmlOut << "</animBone>\n";
	}

	xmlOut << "</Animation>\n";


	xmlOut.close();
	xmlOut.clear();
}
void CMeshExporter::WriteAnimBinary(const Animation& writeOutAnim )
{
	// Add on .bin extension while taking off .xml extension
	string szAnimName = writeOutAnim.meshName;
	//szAnimName.resize(szAnimName.length() - 4);
	//szAnimName = szAnimName + ".bin";

	std::ofstream animOut;
	animOut.open(szAnimName.c_str(), std::ios_base::out | std::ios_base::trunc | std::ios_base::binary);	

	if(!animOut.is_open())
		return;

	int index = (int)szAnimName.find_last_of("/");

	szAnimName.erase(0, index+1);

	// take off the file extension.
	szAnimName.resize(szAnimName.length() - 4);

	size_t namelen = szAnimName.size()+1;

	animOut.write((char*)&namelen, sizeof(size_t));
	animOut.write(szAnimName.c_str(), (int)namelen);

	float animDuration = 0.0;

	if(writeOutAnim.ThemBones.size())
	{
		for(unsigned int i = 0; i < writeOutAnim.ThemBones.size(); ++i)
		{
			if(writeOutAnim.ThemBones[i].m_vKeyFrames.size())
			{
				animDuration = (float)(writeOutAnim.ThemBones[i].m_vKeyFrames[writeOutAnim.ThemBones[i].m_vKeyFrames.size() - 1].m_KeyTime.as(MTime::kSeconds));
				break;
			}
		}
	}
	animOut.write((char*)&animDuration,sizeof(float));

	size_t numBones = writeOutAnim.ThemBones.size();
	animOut.write((char*)&numBones, sizeof(size_t));
	size_t numKeys = 0;

	for (unsigned int iBones = 0; iBones < numBones; ++iBones)
	{
		animOut.write((char*)&iBones, sizeof(size_t));
		int keyCounter = (int)writeOutAnim.ThemBones[iBones].m_vKeyFrames.size()-1;
		numKeys = writeOutAnim.ThemBones[iBones].m_vKeyFrames.size();
		animOut.write((char*)&numKeys, sizeof(size_t));

		size_t BoneNameLen = writeOutAnim.ThemBones[iBones].m_Name.length()+1;
		std::string BoneName = writeOutAnim.ThemBones[iBones].m_Name.asChar();
		animOut.write((char*)&BoneNameLen, sizeof(size_t));
		animOut.write(BoneName.c_str(), (int)BoneNameLen);

		if(keyCounter<=0)
		{
			for(unsigned int irows = 0; irows < 4; irows++)
			{
				for(unsigned int icols = 0; icols < 4; icols++)
				{
					float temp = (float)writeOutAnim.ThemBones[iBones].m_WorldTransform[irows][icols];
					animOut.write((char*)&temp, sizeof(float));
				}
			}
			continue;
		}

		double firstKeyTime = writeOutAnim.ThemBones[iBones].m_vKeyFrames[0].m_KeyTime.as(MTime::kSeconds);

		for(size_t iKeys = 0; iKeys < writeOutAnim.ThemBones[iBones].m_vKeyFrames.size(); iKeys++)
		{
			size_t curKey = iKeys;
			animOut.write((char*)&curKey, sizeof(size_t));
			double curKeyTime = writeOutAnim.ThemBones[iBones].m_vKeyFrames[iKeys].m_KeyTime.as(MTime::kSeconds) - firstKeyTime;
			animOut.write((char*)&curKeyTime, sizeof(double));

			for(unsigned int irows = 0; irows < 4; irows++)
			{
				for(unsigned int icols = 0; icols < 4; icols++)
				{
					float temp = (float)writeOutAnim.ThemBones[iBones].m_vKeyFrames[iKeys].m_WorldTransform[irows][icols];
					animOut.write((char*)&temp, sizeof(float));
				}
			}
		}
	}

	animOut.close();
	animOut.clear();
}
void CMeshExporter::GetBoneData(Animation &mAnim)
{
	MStatus status;
	MItDag jointIter(MItDag::kDepthFirst,MFn::kJoint,&status);
	while(!jointIter.isDone())
	{	
		MDagPath jointPath;
		jointIter.getPath(jointPath);

		MString name = jointPath.partialPathName();
		MFnTransform jointTransform(jointPath);
		MMatrix localTransform = jointTransform.transformationMatrix();
		MMatrix transformWorld = jointPath.inclusiveMatrix();


		tJoint joint;
		joint.m_Name = name;
		joint.m_LocalTransform = localTransform;
		joint.m_WorldTransform = transformWorld;
		mAnim.ThemBones.push_back(joint);

		MFnDagNode mNode(jointPath);
		bool isRoot = true;
		for(int i=0;i<(int)mNode.parentCount();++i)
		{
			MObject parent = mNode.parent(i);
			if(parent.apiType() == MFn::kJoint)
				isRoot = false;
		}
		if(isRoot)
			mAnim.rootPath = jointPath;

		if(MAnimUtil::isAnimated(jointPath))
		{	
			std::pair<MDagPath,int> temp(jointPath,(int)mAnim.ThemBones.size()-1);
			mAnim.bonePaths.push_back(temp);
		}
		jointIter.next();
	}
}



void CMeshExporter::GetAnimInfo(Animation &mAnim)
{
	for(int bonePathNum=0;bonePathNum<(int)mAnim.bonePaths.size();++bonePathNum)
	{
		MPlugArray animPlugs;
		MAnimUtil::findAnimatedPlugs(mAnim.bonePaths[bonePathNum].first,animPlugs);
		for(int animPlugNum=0;animPlugNum<(int)animPlugs.length();++animPlugNum)
		{
			MObjectArray curves;
			MAnimUtil::findAnimation(animPlugs[animPlugNum],curves);
			for(int curveNum=0;curveNum<(int)curves.length();++curveNum)
			{
				MFnAnimCurve curve(curves[curveNum]);
				int numKeyFrames = curve.numKeyframes();
				for(int keyFrameIndex = 0;keyFrameIndex<numKeyFrames;++keyFrameIndex)
				{
					tKeyFrame keyFrame;
					MTime keyframeTime = curve.time(keyFrameIndex);
					double keyTime = keyframeTime.as(MTime::kSeconds);
					keyFrame.m_dTimeInSeconds = keyTime;
					keyFrame.m_KeyTime = keyframeTime;
					SmartInsertKeyFrame(mAnim.ThemBones[mAnim.bonePaths[bonePathNum].second].m_vKeyFrames,keyFrame);

				}
			}
		}
		int keyFrameCount = (int)mAnim.ThemBones[mAnim.bonePaths[bonePathNum].second].m_vKeyFrames.size();

		MTime beginTime = MAnimControl::currentTime();
		float time = (float)beginTime.as(MTime::kSeconds);

		for(int keyFrameIndex = 0;keyFrameIndex<keyFrameCount;++keyFrameIndex)
		{
			//if(MAnimControl::currentTime() != joints[bonePaths[bonePathNum].second].m_KeyFrames[keyFrameIndex].m_KeyTime)
			MAnimControl::setCurrentTime(mAnim.ThemBones[mAnim.bonePaths[bonePathNum].second].m_vKeyFrames[keyFrameIndex].m_KeyTime);
			MMatrix localMat;
			MFnTransform jointTransform(mAnim.bonePaths[bonePathNum].first);
			localMat = jointTransform.transformationMatrix();
			MMatrix worldMat = mAnim.bonePaths[bonePathNum].first.inclusiveMatrix();
			mAnim.ThemBones[mAnim.bonePaths[bonePathNum].second].m_vKeyFrames[keyFrameIndex].m_LocalTransform = localMat;
			mAnim.ThemBones[mAnim.bonePaths[bonePathNum].second].m_vKeyFrames[keyFrameIndex].m_WorldTransform = worldMat;
		}
		MAnimControl::setCurrentTime(beginTime);
	}
}
bool CMeshExporter::BuildSkeletonHierarchy(tSkeletonNode* pCurrentNode, MDagPath& jointPath,  Animation &outAnim)
{
	bool status= true;
	int numJoints = (int)outAnim.ThemBones.size();
	for(int _boneIndex=0;_boneIndex<numJoints;++_boneIndex)
	{
		if(jointPath.partialPathName() == outAnim.ThemBones[_boneIndex].m_Name)
		{
			pCurrentNode->m_nJointIndex = _boneIndex;
		}
	}
	int childCount=jointPath.childCount();
	if(childCount)
	{
		for(int childNum=0;childNum<childCount;++childNum)
		{
			MObject child = jointPath.child(childNum);
			if(child.apiType() == MFn::kJoint)
			{
				MFnDagNode node(child);
				MDagPath path;
				node.getPath(path);
				tSkeletonNode _skeletonNode;
				BuildSkeletonHierarchy(&_skeletonNode,path,outAnim);
				pCurrentNode->m_vChildren.push_back(_skeletonNode);
			}
		}
	}
	return status;
}
bool CMeshExporter::BuildSkeletonHierarchy(tSkeletonNode* pCurrentNode, MDagPath& jointPath, exMesh &outMesh)
{
	bool status= true;
	int numJoints = (int)outMesh.ThemBones.size();
	for(int _boneIndex=0;_boneIndex<numJoints;++_boneIndex)
	{
		if(jointPath.partialPathName() == outMesh.ThemBones[_boneIndex].m_Name)
		{
			pCurrentNode->m_nJointIndex = _boneIndex;
		}
	}
	int childCount=jointPath.childCount();
	if(childCount)
	{
		for(int childNum=0;childNum<childCount;++childNum)
		{
			MObject child = jointPath.child(childNum);
			if(child.apiType() == MFn::kJoint)
			{
				MFnDagNode node(child);
				MDagPath path;
				node.getPath(path);
				tSkeletonNode _skeletonNode;
				BuildSkeletonHierarchy(&_skeletonNode,path,outMesh);
				pCurrentNode->m_vChildren.push_back(_skeletonNode);
			}
		}
	}
	return status;
}