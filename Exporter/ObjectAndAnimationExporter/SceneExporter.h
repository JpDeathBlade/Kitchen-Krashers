#pragma once
#include "ExporterIncludes.h"
#include <maya/MMatrix.h>

#define PIXEL_SHADER "PixelShaderString"
#define VERTEX_SHADER "VertexShaderString"
#define WAYPOINT "WaypointBool"

class CSceneExporter
{
	//this structure will define an item exported from the scene
	struct SceneItem
	{
		MString		meshName;
		MMatrix		worldTransform;

		//the following are the attributes that can be placed on a mesh in our scene
		MayaAttribute<MString>	pixelShaderAttribute;
		MayaAttribute<MString>	vertexShaderAttribute;
		MayaAttribute<bool>		waypointAttribute;
	};

	std::vector<SceneItem> m_sceneItems; //our collection of scene items


public:
	CSceneExporter(void){}
	~CSceneExporter(void){}
	
	//////////////////////////////////////////////////////////////////////////
	//	Function Name: ClearAndReset
	//	Purpose: to clear out CSceneExporter
	//	Parameters: none
	//	Return: void
	//	Notes: clears out m_sceneItems vector
	//////////////////////////////////////////////////////////////////////////
	void ClearAndReset(void);
	//////////////////////////////////////////////////////////////////////////
	//	Function Name: ExportScene
	//	Purpose: exports the Maya scene and stores it in appropriate data structures
	//	Parameters: none
	//	Return: void
	//	Notes: None
	//////////////////////////////////////////////////////////////////////////
	void ExportScene(void);
	//////////////////////////////////////////////////////////////////////////
	//	Function Name:WriteScene_XML
	//	Purpose: creates and writes out the xml doc for the scene
	//	Parameters: const char * fileName -- name of the file to create
	//	Return: none
	//	Notes: creates the xml document that is the scene
	//////////////////////////////////////////////////////////////////////////
	void WriteScene_XML(const char* fileName);


};
