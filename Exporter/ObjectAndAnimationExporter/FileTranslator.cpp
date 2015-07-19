#include <maya/MPxFileTranslator.h>
#include "FileTranslator.h"
#include <string>
#include "MeshExporter.h"


//simply returns a new copy of ourselves for Maya to use as our plug in
void * CFileTranslator::creator(void)
{
	return new CFileTranslator;
}

//this is the function we will need to complete to actually export our model
MStatus CFileTranslator::writer(const MFileObject& file, const MString& optionString, FileAccessMode mode)
{
	//this function is the one that gets called when you actually export
	//make sure to call all relevant functions from the CStaticMeshExporter class
	
	m_MeshExporter.ClearAndReset();

	bool exportAll = true;

	//TODO: determine if we should export all here and set exportAll bool appropriately
	if (mode != kExportActiveAccessMode)
		exportAll = true;
	else
		exportAll = false;

	m_MeshExporter.ExportMeshes( exportAll );
	m_MeshExporter.WriteOutMeshes(true);
	m_MeshExporter.WriteAnimations(file, true);

	//return success
	return MStatus::kSuccess;
}