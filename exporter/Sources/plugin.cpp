#include <stdio.h>
#define _BOOL


#include "exporter.h"

#include <maya/MFnPlugin.h>

#pragma comment(lib, "Foundation.lib")
#pragma comment(lib, "OpenMaya.lib") 
#pragma comment(lib, "OpenMayaAnim.lib") 


#ifdef _WIN32
#  define HMEXPORT_API __declspec( dllexport )
#else
#  define HMEXPORT_API
#endif 


class hello : public MPxCommand
{
public:
    MStatus        doIt( const MArgList& args );
    static void*   creator();
};


MStatus hello::doIt( const MArgList& args )
{
    printf("Hello %s\n", args.asString(0).asChar() );
    return MS::kSuccess;
}


void* hello::creator()
{
    return new hello;
}


HMEXPORT_API MStatus initializePlugin(MObject obj)
{
        printf("Start initalization...\n");
	MFnPlugin plugin(obj, "Vendor is me", "1.0", "Any");
	plugin.registerCommand("hello", hello::creator);
	plugin.registerFileTranslator("D3ba", "none", SceneExporter::Allocator, NULL, NULL, false);
        printf("Successfully initialized...\n");
	return MS::kSuccess;
}


HMEXPORT_API MStatus uninitializePlugin(MObject obj)
{
	MFnPlugin plugin(obj);
	plugin.deregisterCommand( "hello" );
	plugin.deregisterFileTranslator("D3ba");
	return MS::kSuccess;
}
