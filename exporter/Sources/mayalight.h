#pragma once

#include "Matrix.h"


class MayaLightType
{
public:
        enum Enum {
                Ambient,
                Point,
                Spot,
                Directional
        };
};


class MayaLight
{
public:
        const char*             mName;
        MayaLightType::Enum     mType;
        Matrixx                 mTransform;
        float                   mRed;
        float                   mGreen;
        float                   mBlue;
        float                   mIntensity;
        float                   mAttenuation[3];
};


bool ExtractMayaLight(MayaLight* light, MDagPath& rkDagPath);
