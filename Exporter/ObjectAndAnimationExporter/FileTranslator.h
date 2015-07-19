#pragma once
#include <maya/MPxFileTranslator.h>
#include "ExporterIncludes.h"
#include "MeshExporter.h"


//class must be derived from MPxFileTranslator to gain Maya's file translator functionality
class CFileTranslator : public MPxFileTranslator
{
	CMeshExporter				m_MeshExporter; //our mesh exporter class
public:
	CFileTranslator(void){}
	~CFileTranslator(void){}

	//////////////////////////////////////////////////////////////////////////////////////
	// Functions that must be specifically written to override parent's virtual versions//
	//////////////////////////////////////////////////////////////////////////////////////

	//The creator function initializes the file translator
	static void* creator(void);

	//The writer function gets called when you actually export the scene
	MStatus writer(const MFileObject& file, const MString& optionsString, FileAccessMode mode);

	//This function checks to see if we actually have a writer function. This is silly because
	//Maya forces us to have the writer function, then forces us to have a function to make sure we 
	//actually did it. Ha!
	bool haveWriteMethod(void) const { return true; }

	//This function sets the default extension that we will use for our scene exporting.
	MString defaultExtension(void) const { return "cra"; }

	//This function is used to filter through files when we open up a save file dialog box.
	MString filter(void) const { return "cra"; }
};
