#ifndef _HMPLUGIN_EXPORTER_H
#define _HMPLUGIN_EXPORTER_H

#define _BOOL
#include <maya/MPxFileTranslator.h>

#include <maya/MAnimControl.h>
#include <maya/MString.h>
#include <maya/MArgList.h>
#include <maya/MDagPath.h>
#include <maya/MMatrix.h> 
#include <maya/MStatus.h>
#include <maya/MPxCommand.h>
#include <maya/MObject.h>
#include <maya/MGlobal.h>
#include <maya/MPlug.h>
#include <maya/MTime.h>
#include <maya/MQuaternion.h>

#include <maya/MDagPathArray.h>
#include <maya/MIntArray.h>
#include <maya/MFloatArray.h>
#include <maya/MObjectArray.h>
#include <maya/MPointArray.h>
#include <maya/MPlugArray.h>
#include <maya/MFloatPointArray.h>

#include <maya/MSelectionList.h>

#include <maya/MFnIkJoint.h>
#include <maya/MFnMatrixData.h>
#include <maya/MFnMesh.h>
#include <maya/MFnSkinCluster.h>
#include <maya/MFnSingleIndexedComponent.h>

#include <maya/MFnLambertShader.h>


#include <maya/MItDag.h>
#include <maya/MItDependencyNodes.h>
#include <maya/MItGeometry.h>
#include <maya/MItMeshPolygon.h>
#include <maya/MItSelectionList.h>

#include <math.h>
#include <vector>
#include <string>

#include "Vector3f.h"
#include "Quaternion.h"


class MayaAnimKeyFrame
{
public:
	float				t;
	float				x, y, z;
	float				rx, ry, rz, rw;
};


class MayaJoint
{
public:
	unsigned int			m_uiID;

	unsigned int			m_uiParentID;

	bool				m_bHasParent;

	std::string			m_strName;

	std::string			m_strParentName;

	Vector3x			m_kTranslation;

	Quaternionx			m_kRotation;

	Vector3x			m_kWorldTranslation;

	Quaternionx			m_kWorldRotation;

	std::vector<MayaAnimKeyFrame*>	m_vpkKeyFrames;

	MDagPath			m_kDagPath;
};


class SceneExporter: MPxFileTranslator
{
	FILE				*m_hFile;

	bool				m_bHasSkeleton;

	std::vector<MayaJoint*>		m_vpkJoints;

	bool				ExportModels(MString& rsPath);

	bool				ExportSkeletons(MFnSkinCluster& rkSkinCluster);

	bool				ExportSkins();

	bool				ExportAnimation();

public:
	virtual MStatus			writer(const MFileObject &rkFile, const MString &rkOptions, FileAccessMode eAccessMode);

	virtual bool			haveWriteMethod () const { return true; };
	
	virtual MString			defaultExtension () const { return "ba"; };

	virtual MString			filter () const { return "*.ba"; };

	static void*			Allocator() { return (void*)new SceneExporter; };
};

#endif // _HMPLUGIN_EXPORTER_H
