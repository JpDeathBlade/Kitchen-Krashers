#include "SceneExporter.h"
#include <fstream>
//////////////////////////////////////////////////////////////////////////
//	Function Name: ClearAndReset
//	Purpose: to clear out CSceneExporter
//	Parameters: none
//	Return: void
//	Notes: clears out m_sceneItems vector
//////////////////////////////////////////////////////////////////////////
void CSceneExporter::ClearAndReset(void)
{
	//TODO: make sure the scene exporter is ready to start
}
//////////////////////////////////////////////////////////////////////////
//	Function Name: ExportScene
//	Purpose: exports the Maya scene and stores it in appropriate data structures
//	Parameters: none
//	Return: void
//	Notes: None
//////////////////////////////////////////////////////////////////////////
void CSceneExporter::ExportScene(void)
{
	//this function will go through the dag looking for meshes and export the appropriate information
	//associated with that mesh, including world matrix, mesh name, and any attributes to create a scene

	//when exporting the attributes, use the #defines that are in the SceneExporter.h for the attribute names


}
//////////////////////////////////////////////////////////////////////////
//	Function Name:WriteScene_XML
//	Purpose: creates and writes out the xml doc for the scene
//	Parameters: const char * fileName -- name of the file to create
//	Return: none
//	Notes: creates the xml document that is the scene
//////////////////////////////////////////////////////////////////////////
void CSceneExporter::WriteScene_XML(const char* fileName)
{
	//you will need to create and write out the scene xml file
	//the xml formatting is as follows:
	/*
		<Scene>
			<SceneItem>
				<Mesh name=""/> <---if this is a waypoint, then it's name should be NONE
				<WorldMatrix>
					<xAxis><x></x><y></y><z></z><w></w></xAxis>
					<yAxis><x></x><y></y><z></z><w></w></yAxis>
					<zAxis><x></x><y></y><z></z><w></w></zAxis>
					<wAxis><x></x><y></y><z></z><w></w></wAxis>
				</WorldMatrix>
				<ShaderPair>				--
					<pixel></pixel>			  |
											  |--- You will not have to write these two out if it is a waypoint
					<vertex></vertex>		  |
				</ShaderPair>				--
			</SceneItem>
		</Scene>
	*/

	//must be the first thing written out to xml file, makes xml file legal
	char XMLProlog[] = "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n";

}