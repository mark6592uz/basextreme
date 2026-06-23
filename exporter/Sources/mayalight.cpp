
#include <assert.h>

#define _BOOL
#include <maya/MDagPath.h>
#include <maya/MDagPathArray.h>
#include <maya/MFloatArray.h>
#include <maya/MPlug.h>
#include <maya/MPlugArray.h>
#include <maya/MPointArray.h>
#include <maya/MVector.h>
#include <maya/MMatrix.h>

#include <maya/MItDependencyNodes.h>
#include <maya/MItGeometry.h>
#include <maya/MItMeshPolygon.h>

#include <maya/MFnLight.h>
#include <maya/MFnAmbientLight.h>
#include <maya/MFnNonAmbientLight.h>

#include "profiler.h"

#include "mayalight.h"


bool ExtractMayaLight(MayaLight* light, MDagPath& rkDagPath)
{
	PROFILE(ExtractMayaLight);

	MStatus kStatus;

	MFnLight kLightFn(rkDagPath, &kStatus);

        if (kStatus != MStatus::kSuccess) {
                return false;
        }

	printf("  Exporting light: %s\n", kLightFn.partialPathName().asChar());


        if (rkDagPath.hasFn(MFn::kAmbientLight)) {
                light->mType = MayaLightType::Ambient;
        } else if (rkDagPath.hasFn(MFn::kSpotLight)) {
                light->mType = MayaLightType::Spot;
        } else if (rkDagPath.hasFn(MFn::kDirectionalLight)) {
                light->mType = MayaLightType::Directional;
        } else if (rkDagPath.hasFn(MFn::kPointLight)) {
                light->mType = MayaLightType::Point;
        } else {
                return false;
        }

        MMatrix m = kLightFn.transformationMatrix();
        Matrixx m2;
        int x, y;
        for (y = 0; y < 4; ++y) {
                for (x = 0; x < 4; ++x) {
                        m2[x][y] = (float)m.matrix[x][y];
                }
        } 

        light->mTransform = m2;

        if (light->mType == MayaLightType::Point || light->mType == MayaLightType::Spot) {
                MFnNonAmbientLight nonAmbient(rkDagPath);
		int decayRate = nonAmbient.decayRate();

		light->mAttenuation[0] = 0.0f;
		light->mAttenuation[1] = 0.0f;
		light->mAttenuation[2] = 0.0f;

                if (decayRate < 0) decayRate = 0;
                if (decayRate > 2) decayRate = 2;
		light->mAttenuation[decayRate] = 1.0f;
        }

        light->mName = kLightFn.name().asChar();
        light->mRed = kLightFn.color().r;
        light->mGreen = kLightFn.color().g;
        light->mBlue = kLightFn.color().b;
        light->mIntensity = kLightFn.intensity();

	return true;
}
