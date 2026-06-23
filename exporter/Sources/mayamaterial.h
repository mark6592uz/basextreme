
#include <vector>

#define _BOOL
#include <maya/MString.h>
#include <maya/MFnMesh.h>
#include <maya/MFnLambertShader.h>


class MayaTexture
{
public:
	MString				mName;
	float				mRepeatU;
	float				mRepeatV;
};


class MayaMaterial
{
public:
	MString				m_strName;
        std::vector<MayaTexture*>	mTextures;
};


void ExtractMaterials(std::vector<MayaMaterial*>* papcMayaMaterial, MObjectArray& racShaders);
